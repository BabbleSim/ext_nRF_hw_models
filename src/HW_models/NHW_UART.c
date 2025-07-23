/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * UART(E) - Universal asynchronous receiver/transmitter (with EasyDMA)
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/uart.html?cp=5_1_0_5_30
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/uarte.html?cp=5_1_0_5_317
 *
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/uarte.html?cp=4_0_0_6_37
 */

/**
 * Notes (common for all):
 *   * Check the selected backend notes, for more possible limitations
 *
 *   * PSEL is ignored, pins are assumed connected.
 *
 *   * Unlike in real HW, it is irrelevant if the external oscillator is enabled
 *     or not. The UART timing will always be equally precise.
 *
 *   * After a STARTTX, the UART is ready to Tx right away
 *     (it does not take ~1 us as in the real HW)
 *
 *   * The effective transmit rate is going to be marginally different than in reality, due to
 *     rounding of the frame time to an integer number of microseconds, and the
 *     logic having immediate response time.
 *
 *   * Data line errors (parity, framing, break) are not yet modelled
 *
 *   * (UART) In UART mode, in real HW, a TASK_STOPTX would break a currently ongoing Tx frame
 *     mid way, while in this model, the Tx is allowed to finish (just like for a TASK_SUSPEND
 *     or a TASK_STOPTX in UART-E mode).
 *     Due to this, in the model, a (UART) TASK_SUSPEND is equivalent to a combination of
 *     TASK_STOPTX & TASK_STOPRX, while in real HW it is not.
 *
 *   * (UART) In real HW, it is unclear what happens when a STOPTX (in UART mode) is triggered, while
 *     a byte is queued for transmission (TXD was written), but where the transmission hasn't started
 *     yet due to the CTS line holding it. It seems the blocked pended Tx is just dropped/forgotten
 *     This is how the model behaves.
 *   * (UARTE) In real HW, when a STOPTX task is triggered (in UARTE mode),
 *     while a byte is queued for transmission, but where the transmission hasn't started yet
 *     due to the CTS line holding it. It seems that the blocked pended Tx is just dropped
 *     (and therefore TXSTOPPED comes right away).
 *     This is how the model behaves.
 *
 *   * (UART & UARTE) It seems in real HW the Rx FIFO status is kept after a STOPRX & a new STARTRX.
 *     and (if flow control is on) will lower RTS on STARTRX only if the FIFO is empty.
 *     The model behaves this way.
 *
 *   * In real HW, it seems RTS will be kept high (if flow control is on) if EITHER the Rx FIFO
 *     got more than 2 bytes used, and did not come back to empty yet, OR the Rx is not started/or stopped.
 *     (so just starting it, will not clear RTS unless the FIFO was emptied)
 *     The model behaves this way.
 *
 *   * In real HW, it seems all internal status is lost when the enable register is cleared.
 *     The HW models behaves this way.
 *
 * * 52:
 *   * The plain UART functionality is present in all UARTEs,
 *     for the nRF52
 *
 * * 54:
 *   * DMA_{RX,TX}.TERMINATEONBUSERROR is ignored EVENTS_DMA.{RX,TX}.BUSERROR is never generated
 *     and BUSERRORADDRESS is never set
 *
 *   * From the spec is unclear when DMA.RX.MATCH.CANDIDATE[n] is copied into the internal shadow
 *     register. It would seem it will happen when either TASKS_DMA.RX.START is triggered,
 *     or EVENTS_MATCH[n] happens. This is what the model does.
 *
 *   * MATCH functionality is untested in this model
 *
 *   * FRAMESIZE != 8 and ADDRESS functionality is untested in this model
 *
 * Implementation notes:
 *   * As for the 54 the data in the line can be a configurable amount between 4 and 9bits (due to address bit),
 *     the "byte" parameters were increased to 16bits. But the naming was kept in most of them as "byte".
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_templates.h"
#include "NHW_UART.h"
#include "NHW_UART_private.h"
#include "NHW_UART_backend_if.h"
#include "NHW_xPPI.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"
#include "bs_oswrap.h"
#include "bs_utils.h"
#include "bs_cmd_line.h"
#include "bs_dynargs.h"
#include "nsi_hw_scheduler.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

static struct uarte_status nhw_uarte_st[NHW_UARTE_TOTAL_INST];
NRF_UARTE_Type NRF_UARTE_regs[NHW_UARTE_TOTAL_INST];
#if (NHW_UARTE_HAS_UART)
NRF_UART_Type *NRF_UART_regs[NHW_UARTE_TOTAL_INST];
#endif

static bs_time_t Timer_UART_common = TIME_NEVER;
static bs_time_t Timer_UART_peri = TIME_NEVER;
extern bs_time_t nhw_Timer_ULoopback;

static void nhw_UARTE_signal_EVENTS_ERROR(unsigned int inst);
static void nhw_UARTE_signal_EVENTS_RXDRDY(unsigned int inst);
static void nhw_UARTE_signal_EVENTS_RXTO(unsigned int inst);
static void nhw_UARTE_signal_EVENTS_TXDRDY(unsigned int inst);
static void nhw_UARTE_signal_EVENTS_CTS(unsigned int inst);
static void nhw_UARTE_signal_EVENTS_NCTS(unsigned int inst);
static void nhw_UARTE_signal_EVENTS_TXSTARTED(unsigned int inst);
static void nhw_UARTE_signal_EVENTS_TXSTOPPED(unsigned int inst);
static void nhw_UARTE_signal_EVENTS_ENDTX(unsigned int inst);
static void nhw_UARTE_signal_EVENTS_RXSTARTED(unsigned int inst);
static void nhw_UARTE_signal_EVENTS_ENDRX(unsigned int inst);
#if (NHW_UARTE_HAS_FRAMETIMEOUT)
static void nhw_UARTE_signal_EVENTS_FRAMETIMEOUT(unsigned int inst);
#endif
#if (NHW_UARTE_HAS_MATCH)
static void nhw_UARTE_signal_EVENTS_DMA_RX_MATCH(unsigned int inst, unsigned int i);
#endif
static void nhw_UARTE_Tx_send_byte(unsigned int inst, struct uarte_status *u_el);
static void nhw_UART_Tx_queue_byte(uint inst, struct uarte_status *u_el, uint16_t byte);
static void nhw_UARTE_Rx_DMA_attempt(uint inst, struct uarte_status *u_el);
static void raise_RTS_R(uint inst, struct uarte_status *u_el);

static void nhw_uarte_init(void) {
#if (NHW_HAS_DPPI)
  static uint nhw_UARTE_dppi_map[NHW_UARTE_TOTAL_INST] = NHW_UARTE_DPPI_MAP;
#endif
#if NHW_UARTE_HAS_MATCH
  int n_match[NHW_UARTE_TOTAL_INST] = NHW_UARTE_N_MATCH;
#endif

  memset(NRF_UARTE_regs, 0, sizeof(NRF_UARTE_regs));

  uint uart_clocks[NHW_UARTE_TOTAL_INST] = NHW_UARTE_CLOCKS;

  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {
    struct uarte_status *u_el = &nhw_uarte_st[i];

    u_el->inst = i;
    u_el->clock_f = uart_clocks[i];

#if (NHW_UARTE_HAS_UART)
    NRF_UART_regs[i] = (NRF_UART_Type *)&NRF_UARTE_regs[i];
    u_el->UART_regs[i] = (NRF_UART_Type *)&NRF_UARTE_regs[i];
#endif
    u_el->UARTE_regs[i] = (NRF_UARTE_Type *)&NRF_UARTE_regs[i];

    u_el->Rx_TO_timer = TIME_NEVER;
    u_el->Tx_byte_done_timer = TIME_NEVER;
    u_el->frametimeout_timer = TIME_NEVER;

    NRF_UARTE_regs[i].PSEL.RTS = 0xFFFFFFFF;
    NRF_UARTE_regs[i].PSEL.TXD = 0xFFFFFFFF;
    NRF_UARTE_regs[i].PSEL.CTS = 0xFFFFFFFF;
    NRF_UARTE_regs[i].PSEL.RXD = 0xFFFFFFFF;
    NRF_UARTE_regs[i].BAUDRATE = 0x04000000;

#if NHW_UARTE_HAS_MATCH
    u_el->n_match = n_match[i];
    u_el->MATCH_CANDIDATE = bs_calloc(n_match[i], sizeof(uint32_t));
    u_el->DMA_RX_ENABLEMATCH_subscribed = bs_calloc(n_match[i], sizeof(struct nhw_subsc_mem));
    u_el->DMA_RX_DISABLEMATCH_subscribed = bs_calloc(n_match[i], sizeof(struct nhw_subsc_mem));
#endif

#if (NHW_HAS_DPPI)
    u_el->dppi_map = nhw_UARTE_dppi_map[i];
#endif

    raise_RTS_R(i, u_el);

    if (u_el->Tx_log_file_name) {
      bs_create_folders_in_path(u_el->Tx_log_file_name);
      u_el->Tx_log_file = bs_fopen(u_el->Tx_log_file_name, "w");
      fprintf(u_el->Tx_log_file, "time(microsecond),byte\n");
    }
    if (u_el->Rx_log_file_name) {
      bs_create_folders_in_path(u_el->Rx_log_file_name);
      u_el->Rx_log_file = bs_fopen(u_el->Rx_log_file_name, "w");
      fprintf(u_el->Rx_log_file, "time(microsecond),byte\n");
    }
  }
}

NSI_TASK(nhw_uarte_init, HW_INIT, 200); /* Allow backend to be registered before */

static void nhw_uarte_cleanup(void) {
  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {
    struct uarte_status *u_el = &nhw_uarte_st[i];
    if (u_el->Tx_log_file) {
      fclose(u_el->Tx_log_file);
      u_el->Tx_log_file = NULL;
    }
    if (u_el->Rx_log_file) {
      fclose(u_el->Rx_log_file);
      u_el->Rx_log_file = NULL;
    }
    if (u_el->MATCH_CANDIDATE) {
      free(u_el->MATCH_CANDIDATE);
      u_el->MATCH_CANDIDATE = NULL;
    }
#if (NHW_HAS_DPPI)
    if (u_el->DMA_RX_ENABLEMATCH_subscribed) {
      free(u_el->DMA_RX_ENABLEMATCH_subscribed);
      u_el->DMA_RX_ENABLEMATCH_subscribed = NULL;
    }
    if (u_el->DMA_RX_DISABLEMATCH_subscribed) {
      free(u_el->DMA_RX_DISABLEMATCH_subscribed);
      u_el->DMA_RX_DISABLEMATCH_subscribed = NULL;
    }
#endif
  }
}

NSI_TASK(nhw_uarte_cleanup, ON_EXIT_PRE, 100);

uart_rtxb_cb_f nhw_uarte_register_rx_cb(int inst, uart_rtxb_cb_f cb, bool Rx_NotTx) {
  struct uarte_status *u_el = &nhw_uarte_st[inst];
  uart_rtxb_cb_f prev = u_el->trx_callbacks[(int) Rx_NotTx];
  u_el->trx_callbacks[(int) Rx_NotTx] = cb;
  return prev;
}

/*
 * Register a backend to be used for a given UART instance
 */
void nhw_UARTE_backend_register(uint inst, struct backend_if *backend) {
  struct uarte_status *u_el = &nhw_uarte_st[inst];

  if (u_el->backend.tx_byte_f != NULL) {
    bs_trace_warning_line("UART%i backend selection overwritten\n", inst);
  }
  memcpy(&u_el->backend, backend, sizeof(struct backend_if));
}

void nhw_uarte_update_common_timer(void) {
  Timer_UART_common = BS_MIN(Timer_UART_peri, nhw_Timer_ULoopback);
  nsi_hws_find_next_event();
}

static void nhw_uarte_update_timer(void) {
  Timer_UART_peri = TIME_NEVER;
  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {
    struct uarte_status * u_el = &nhw_uarte_st[i];
    bs_time_t smaller = BS_MIN(u_el->Rx_TO_timer, u_el->Tx_byte_done_timer);
#if (NHW_UARTE_HAS_FRAMETIMEOUT)
    smaller = BS_MIN(smaller, u_el->frametimeout_timer);
#endif
    Timer_UART_peri = BS_MIN(Timer_UART_peri, smaller);
  }
  nhw_uarte_update_common_timer();
}

static bool uart_enabled(uint inst) {
#if (NHW_UARTE_HAS_UART)
  return NRF_UARTE_regs[inst].ENABLE == 4;
#else
  (void) inst;
  return false;
#endif
}

static bool uarte_enabled(uint inst) {
  return NRF_UARTE_regs[inst].ENABLE == 8;
}

/**
 * Return the duration of 1 bit in seconds, given the BAUDRATE register
 * value and the clock_frequency in Hz.
 *
 * Note that the actual bit rate is just 1.0/{nhw_uarte_bit_dur_from_reg()}
 */
static inline double nhw_uarte_bit_dur_from_reg(uint32_t reg, int clock_freq) {
	int cc_per_bit = ((uint64_t)1<<32) / reg;
	return (double)cc_per_bit / clock_freq;
}

/*
 * Return the duration of <nbits> bits in *microseconds*
 */
static inline bs_time_t nhw_uarte_nbits_time(uint inst, uint nbits) {
  //We provide the frequency in MHz, so we are already scaled in micros.
  return nbits * nhw_uarte_bit_dur_from_reg(NRF_UARTE_regs[inst].BAUDRATE, nhw_uarte_st[inst].clock_f) + 0.5;
}

static int nhw_uarte_get_frame_size(uint inst) {
  uint frame_size = 8;
#if defined(UARTE_CONFIG_FRAMESIZE_Msk)
  frame_size = (NRF_UARTE_regs[inst].CONFIG & UARTE_CONFIG_FRAMESIZE_Msk) >> UARTE_CONFIG_FRAMESIZE_Pos;
  if (frame_size < 4 || frame_size > 9) {
    frame_size = 8;
  }
#else
  (void) inst;
#endif
  return frame_size;
}

/**
 * Return the time in microseconds it takes for one byte to be Tx or Rx
 * including start, parity and stop bits.
 * Accounting for the UART configuration and baud rate
 */
bs_time_t nhw_uarte_one_byte_time(uint inst) {
  bs_time_t duration = 1 + 1; /* Start bit, and at least 1 stop bit */
  uint32_t CONFIG = NRF_UARTE_regs[inst].CONFIG;

  duration += nhw_uarte_get_frame_size(inst); /* data byte itself */

  if (CONFIG & UARTE_CONFIG_PARITY_Msk) {
    duration +=1;
  }
  if (CONFIG & UARTE_CONFIG_STOP_Msk) { /* Two stop bits */
    duration +=1;
  }

  return nhw_uarte_nbits_time(inst, duration);
}

static uint8_t Rx_FIFO_pop(uint inst, struct uarte_status *u_el) {
  uint8_t value;

  if (u_el->Rx_FIFO_cnt <= 0) {
    bs_trace_warning_time_line("UART%i: Attempted empty Rx FIFO pop\n", inst);
    return 0;
  }

  value = u_el->Rx_FIFO[0];

  for (int i = 1; i < u_el->Rx_FIFO_cnt; i++) {
    u_el->Rx_FIFO[i-1] = u_el->Rx_FIFO[i];
  }
  u_el->Rx_FIFO_cnt -=1;

  if (u_el->Rx_FIFO_cnt > 0) {
#if (NHW_UARTE_HAS_UART)
    NRF_UART_regs[inst]->RXD = u_el->Rx_FIFO[0];
#endif
    nhw_UARTE_signal_EVENTS_RXDRDY(inst);
  }

  return value;
}

static void Rx_FIFO_push(uint inst, struct uarte_status *u_el, uint8_t value) {
  if (u_el->Rx_FIFO_cnt >= RX_FIFO_SIZE) {
    Rx_FIFO_pop(inst, u_el);
    bs_trace_warning_time_line("UART%i: Pushed to full Rx FIFO, oldest value dropped\n", inst);
    NRF_UARTE_regs[inst].ERRORSRC |= UARTE_ERRORSRC_OVERRUN_Msk;
    nhw_UARTE_signal_EVENTS_ERROR(inst);
  }
  u_el->Rx_FIFO[u_el->Rx_FIFO_cnt++] = value;

  if (u_el->Rx_FIFO_cnt == 1){
#if (NHW_UARTE_HAS_UART)
    NRF_UART_regs[inst]->RXD = u_el->Rx_FIFO[0];
#endif
    nhw_UARTE_signal_EVENTS_RXDRDY(inst);
  }
}

static void nhw_UARTE_Rx_DMA_end(uint inst, struct uarte_status * u_el) {
  u_el->rx_dma_status = DMA_Off;
#if NHW_UARTE_54NAMING
  NRF_UARTE_regs[inst].DMA.RX.AMOUNT = u_el->RXD_AMOUNT;
#else
  NRF_UARTE_regs[inst].RXD.AMOUNT = u_el->RXD_AMOUNT;
#endif
  nhw_UARTE_signal_EVENTS_ENDRX(inst);
}

static void nhw_UARTE_Rx_match_check(uint inst, struct uarte_status * u_el, uint32_t value) {
#if (NHW_UARTE_HAS_MATCH)
  for (int i = 0; i < u_el->n_match; i++) {
    uint32_t enable_mask = (UARTE_DMA_RX_MATCH_CONFIG_ENABLE0_Msk << i);
    if ((NRF_UARTE_regs[inst].DMA.RX.MATCH.CONFIG & enable_mask)
        && (NRF_UARTE_regs[inst].DMA.RX.MATCH.CANDIDATE[i] == value)) {
      NRF_UARTE_regs[inst].DMA.RX.AMOUNT = u_el->RXD_AMOUNT;
      NRF_UARTE_regs[inst].DMA.TX.AMOUNT = u_el->TXD_AMOUNT;
      nhw_uarte_st[inst].MATCH_CANDIDATE[i] = NRF_UARTE_regs[i].DMA.RX.MATCH.CANDIDATE[i];

      if (NRF_UARTE_regs[inst].DMA.RX.MATCH.CONFIG & (UARTE_DMA_RX_MATCH_CONFIG_ONESHOT0_Msk << i)) {
        NRF_UARTE_regs[inst].DMA.RX.MATCH.CONFIG &= ~enable_mask;
      }
      nhw_UARTE_signal_EVENTS_DMA_RX_MATCH(inst, i);
    }
  }
#else
  (void) inst;
  (void) u_el;
  (void) value;
#endif
}

static void nhw_UARTE_Rx_DMA_attempt(uint inst, struct uarte_status * u_el) {
  if (u_el->rx_dma_status != DMAing) {
    return;
  }

  uint8_t *p = (uint8_t *)(u_el->RXD_PTR + u_el->RXD_AMOUNT);

  while (u_el->Rx_FIFO_cnt && (u_el->RXD_AMOUNT < u_el->RXD_MAXCNT)) {
    uint8_t value = Rx_FIFO_pop(inst, u_el);
    *p++ = value;
    u_el->RXD_AMOUNT++;
    nhw_UARTE_Rx_match_check(inst, u_el, value);
  }
  if (u_el->RXD_AMOUNT >= u_el->RXD_MAXCNT) {
    nhw_UARTE_Rx_DMA_end(inst, u_el);
  }
}

static bool flow_control_on(uint inst) {
  return (NRF_UARTE_regs[inst].CONFIG & UARTE_CONFIG_HWFC_Msk) != 0;
}

static void propagate_RTS_R(uint inst, struct uarte_status *u_el) {
  if (flow_control_on(inst)) {
    if (u_el->backend.RTS_pin_toggle_f) {
      u_el->backend.RTS_pin_toggle_f(inst, u_el->RTSR);
    }
  }
}

static void lower_RTS_R(uint inst, struct uarte_status *u_el) {
  if (u_el->RTSR == false) {
    return;
  }
  u_el->RTSR = false;
  propagate_RTS_R(inst, u_el);
}

static void raise_RTS_R(uint inst, struct uarte_status *u_el) {
  if (u_el->RTSR == true) {
    return;
  }
  u_el->RTSR = true;
  propagate_RTS_R(inst, u_el);
}

static void notify_backend_RxOnOff(uint inst, struct uarte_status *u_el, bool OnNotOff) {
  if (u_el->backend.uart_enable_notify_f) {
    u_el->backend.uart_enable_notify_f(inst, u_el->tx_status != Tx_Off, OnNotOff);
  }
}

static void notify_backend_TxOnOff(uint inst, struct uarte_status *u_el, bool OnNotOff) {
  if (u_el->backend.uart_enable_notify_f) {
    u_el->backend.uart_enable_notify_f(inst, OnNotOff, u_el->rx_status != Rx_Off);
  }
}

/*
 * Process the receive frame (up to 9 bits including address)
 * which may be to shift the data up (if it was less than 4 bits and ENDIAN was set)
 *
 * Returns true if the frame should be dropped (due to address filtering)
 * False otherwise
 */
static bool nhw_UARTE_process_Rx_byte(uint inst, struct uarte_status *u_el, uint16_t *byte) {
#if defined(UARTE_CONFIG_FRAMESIZE_Msk)
  uint frame_size = nhw_uarte_get_frame_size(inst);

  if (frame_size == 8) {
    /* Let's handle the typical case fast */
  } else if (frame_size < 8) {
    if (NRF_UARTE_regs[inst].CONFIG & UARTE_CONFIG_ENDIAN_Msk) { //Cut from LSB
      uint shift = (8 - frame_size);
      *byte = *byte << shift;
    }
  } else if (frame_size == 9) { //9 bits
    if (*byte & 0x100) { //It's an address
      if ((*byte & 0xFF) == NRF_UARTE_regs[inst].ADDRESS) {
        u_el->rx_addr_filter_matched = true;
      } else {
        u_el->rx_addr_filter_matched = false;
      }
      return true; /* Address is always dropped */
    }
    return !u_el->rx_addr_filter_matched;
  }
#else
  (void) inst;
  (void) u_el;
  (void) byte;
#endif
  return false;
}

/**
 * Process a byte incoming to the UART from a backend
 * This call should be done in the last micros when the byte frame is finishing in the line
 */
void nhw_UARTE_digest_Rx_byte(uint inst, uint16_t byte) {
  struct uarte_status *u_el = &nhw_uarte_st[inst];
  bs_time_t frame_start, now;

  if (u_el->rx_status == Rx_Off) {
    static int Received_error_count;
    Received_error_count++;
    if ((Received_error_count & 0xFF) <= 4) {
      bs_trace_warning_time_line("Byte received while UART%i is not enabled for Rx, ignoring it (warn count = %i)\n", inst, Received_error_count);
      if ((Received_error_count & 0xFF) == 4) {
        bs_trace_warning_time_line("Silencing this warning the next 252 times\n", inst);
      }
    }
    return;
  }

  now = nsi_hws_get_time();
  frame_start = now - nhw_uarte_one_byte_time(inst) + 1;

  if (u_el->Last_Rx_off_time >= frame_start) {
    bs_trace_warning_time_line("Byte partially received while UART%i was not enabled for Rx, "
                               "this would have likely caused a framing error. Ignoring it in the model\n", inst);
    return;
  }

  if (u_el->Rx_log_file) {
    fprintf(u_el->Rx_log_file, "%"PRItime",0x%02X\n", now, byte);
  }

  if (u_el->trx_callbacks[1]) {
    u_el->trx_callbacks[1](inst, &byte);
  }

  if (nhw_UARTE_process_Rx_byte(inst, u_el, &byte)) {
    return;
  }

  Rx_FIFO_push(inst, u_el, byte);
  nhw_UARTE_Rx_DMA_attempt(inst, u_el);
  if (u_el->Rx_FIFO_cnt >= RX_FIFO_RTS_THRESHOLD) {
      raise_RTS_R(inst, u_el);
  }
}

/**
 * Propagate internally the CTS pin having been lowered
 */
void nhw_UARTE_CTS_lowered(uint inst) {
  struct uarte_status *u_el = &nhw_uarte_st[inst];

  if (u_el->CTS_blocking == false) {
    //It was already low (or perceived as such) => nothing to be done
    return;
  }

  u_el->CTS_blocking = false;

  if ( !(uart_enabled(inst) || uarte_enabled(inst)) ) {
    return;
  }

  if (u_el->tx_status == Tx_Pend) {
    nhw_UARTE_Tx_send_byte(inst, u_el);
  }
  nhw_UARTE_signal_EVENTS_CTS(inst);
}

/**
 * Propagate internally the CTS pin having been raised
 */
void nhw_UARTE_CTS_raised(uint inst) {
  if (nhw_uarte_st[inst].CTS_blocking == true) {
    return;
  }
  nhw_uarte_st[inst].CTS_blocking = true;

  if ( !(uart_enabled(inst) || uarte_enabled(inst)) ) {
    return;
  }
  nhw_UARTE_signal_EVENTS_NCTS(inst);
}

static void nhw_UARTE_eval_interrupt(uint inst) {
  static bool uart_int_line[NHW_UARTE_TOTAL_INST]; /* Is the UART currently driving its interrupt line high */
  /* Mapping of peripheral instance to {int controller instance, int number} */
  static struct nhw_irq_mapping nhw_uart_irq_map[NHW_UARTE_TOTAL_INST] = NHW_UARTE_INT_MAP;
  bool new_int_line = false;
  uint32_t inten = NRF_UARTE_regs[inst].INTEN;

  NHW_CHECK_INTERRUPT(UARTE, NRF_UARTE_regs[inst]., CTS, inten)
  NHW_CHECK_INTERRUPT(UARTE, NRF_UARTE_regs[inst]., NCTS, inten)
  NHW_CHECK_INTERRUPT(UARTE, NRF_UARTE_regs[inst]., RXDRDY, inten)
  NHW_CHECK_INTERRUPT(UARTE, NRF_UARTE_regs[inst]., TXDRDY, inten)
  NHW_CHECK_INTERRUPT(UARTE, NRF_UARTE_regs[inst]., ERROR, inten)
  NHW_CHECK_INTERRUPT(UARTE, NRF_UARTE_regs[inst]., RXTO, inten)

  if (uarte_enabled(inst)) {
    /* It is unclear if these UART*E* events being still pended would still keep
     * the int line high even in UART(not E) mode */
#if !(NHW_UARTE_54NAMING)
    NHW_CHECK_INTERRUPT(UARTE, NRF_UARTE_regs[inst]., ENDRX, inten)
    NHW_CHECK_INTERRUPT(UARTE, NRF_UARTE_regs[inst]., ENDTX, inten)
    NHW_CHECK_INTERRUPT(UARTE, NRF_UARTE_regs[inst]., RXSTARTED, inten)
    NHW_CHECK_INTERRUPT(UARTE, NRF_UARTE_regs[inst]., TXSTARTED, inten)
#else
    NHW_CHECK_INTERRUPT_ST(UARTE, NRF_UARTE_regs[inst]., DMA.RX.END, DMARXEND, inten)
    NHW_CHECK_INTERRUPT_ST(UARTE, NRF_UARTE_regs[inst]., DMA.TX.END, DMATXEND, inten)
    NHW_CHECK_INTERRUPT_ST(UARTE, NRF_UARTE_regs[inst]., DMA.RX.READY, DMARXREADY, inten)
    NHW_CHECK_INTERRUPT_ST(UARTE, NRF_UARTE_regs[inst]., DMA.TX.READY, DMATXREADY, inten)
#endif
    NHW_CHECK_INTERRUPT(UARTE, NRF_UARTE_regs[inst]., TXSTOPPED, inten)
#if defined(UARTE_INTENSET_FRAMETIMEOUT_Msk)
    NHW_CHECK_INTERRUPT(UARTE, NRF_UARTE_regs[inst]., FRAMETIMEOUT, inten)
#endif
#if NHW_UARTE_HAS_MATCH
    for (int i = 0; i < nhw_uarte_st[inst].n_match; i++) {
      if (NRF_UARTE_regs[inst].EVENTS_DMA.RX.MATCH[i] &&
          (inten & (UARTE_INTEN_DMARXMATCH0_Msk << i))) {
        new_int_line = true;
      }
    }
#endif
  }

  hw_irq_ctrl_toggle_level_irq_line_if(&uart_int_line[inst],
                                       new_int_line,
                                       &nhw_uart_irq_map[inst]);
}

static void nhw_UARTE_RxDMA_start(int inst) {
  struct uarte_status * u_el = &nhw_uarte_st[inst];
#if !NHW_UARTE_54NAMING
  u_el->RXD_PTR = NRF_UARTE_regs[inst].RXD.PTR;
  u_el->RXD_MAXCNT = NRF_UARTE_regs[inst].RXD.MAXCNT;
#else
  u_el->RXD_PTR = NRF_UARTE_regs[inst].DMA.RX.PTR;
  u_el->RXD_MAXCNT = NRF_UARTE_regs[inst].DMA.RX.MAXCNT;
#endif
  u_el->RXD_AMOUNT = 0;
  u_el->rx_dma_status = DMAing;
#if NHW_UARTE_HAS_MATCH
    for (int i = 0; i < u_el->n_match; i++) {
      u_el->MATCH_CANDIDATE[i] = NRF_UARTE_regs[inst].DMA.RX.MATCH.CANDIDATE[i];
    }
#endif
  nhw_UARTE_signal_EVENTS_RXSTARTED(inst); /* Instantaneously ready */

  nhw_UARTE_Rx_DMA_attempt(inst, u_el);
}

void nhw_UARTE_TASK_STARTRX(uint inst)
{
  struct uarte_status *u_el = &nhw_uarte_st[inst];

  if (!uart_enabled(inst) && !uarte_enabled(inst)) {
    bs_trace_warning_time_line("Start RX triggered while UART%i is not enabled (%u). "
                               "Ignoring it.\n", inst, NRF_UARTE_regs[inst].ENABLE);
    return;
  }

  if (uart_enabled(inst) && (u_el->rx_status != Rx_Off)) {
    bs_trace_warning_time_line("Start Rx triggered for UART%i whose Rx is already started (%i). "
                               "Ignoring it\n", inst, u_el->rx_status);
    return;
  }

  if (u_el->rx_dma_status != DMA_Off) {
    bs_trace_warning_time_line("Start Rx triggered for UARTE%i whose Rx is already DMA'ing. "
                               "This seems like a SW error which the model does not handle. "
                               "Ignoring it\n", inst);
    return;
  }

  if (u_el->rx_status == Rx_turning_off) {
    /* The HW seems to support a STARTRX while it is turning off the Rx
     * In this case, it seems the Rx TO is just cancelled */
    u_el->Rx_TO_timer = TIME_NEVER;
    u_el->rx_status = Rx_On;
  }

  if (uarte_enabled(inst)) {
    nhw_UARTE_RxDMA_start(inst);
  }

  if (u_el->rx_status == Rx_Off) {
    u_el->Last_Rx_off_time = nsi_hws_get_time();
    u_el->rx_status = Rx_On;
    notify_backend_RxOnOff(inst, u_el, true);
  }

  if (u_el->Rx_FIFO_cnt == 0) {
    lower_RTS_R(inst, u_el);
  }
}

void nhw_UARTE_TASK_STOPRX(uint inst)
{
  /*
   * If in UART mode (at least) raise RTS/R
   * Set Rx in Off mode
   */
  struct uarte_status *u_el = &nhw_uarte_st[inst];

  if (u_el->rx_status == Rx_Off) {
    bs_trace_warning_time_line("UART%i Rx stopped while already stopped => ignored\n", inst);
    return;
  }
  if (u_el->rx_status == Rx_turning_off) {
    bs_trace_warning_time_line("UART%i Rx stopped while already stopping => ignored\n", inst);
    return;
  }

  raise_RTS_R(inst, u_el);

  //Start Rx TO timer to turn Rx fully off and generate RXTO
  u_el->Rx_TO_timer = nsi_hws_get_time() + 5*nhw_uarte_one_byte_time(inst);
  u_el->rx_status = Rx_turning_off;
  //And clear a possible frametimeout
  u_el->frametimeout_timer = TIME_NEVER;
  nhw_uarte_update_timer();
}

#if (NHW_UARTE_HAS_MATCH)
void nhw_UARTE_TASK_DMA_RX_ENABLEMATCH(uint inst, uint i) {
  NRF_UARTE_regs[inst].DMA.RX.MATCH.CONFIG |= UARTE_DMA_RX_MATCH_CONFIG_ENABLE0_Msk<<i;
}
void nhw_UARTE_TASK_DMA_RX_DISABLEMATCH(uint inst, uint i) {
  NRF_UARTE_regs[inst].DMA.RX.MATCH.CONFIG &= ~(UARTE_DMA_RX_MATCH_CONFIG_ENABLE0_Msk<<i);
}
#endif

static void nHW_UARTE_Tx_DMA_end(int inst, struct uarte_status * u_el) {
  u_el->tx_dma_status = DMA_Off;
#if !NHW_UARTE_54NAMING
  NRF_UARTE_regs[inst].TXD.AMOUNT = u_el->TXD_AMOUNT;
#else
  NRF_UARTE_regs[inst].DMA.TX.AMOUNT = u_el->TXD_AMOUNT;
#endif
  nhw_UARTE_signal_EVENTS_ENDTX(inst);
}

static uint16_t nhw_UART_prep_Tx_data(uint inst, struct uarte_status *u_el, uint16_t byte) {
#if defined(UARTE_CONFIG_FRAMESIZE_Msk)
  uint frame_size = nhw_uarte_get_frame_size(inst);

  if (frame_size == 8) {
    return byte;
  } else if (frame_size < 8) {
    uint shift = (8 - frame_size);
    if (NRF_UARTE_regs[inst].CONFIG & UARTE_CONFIG_ENDIAN_Msk) { //Cut from LSB
      return byte >> shift;
    } else {
      return byte & (0xFF >> shift);
    }
  } else { //9 bits => adding address bit
    if (u_el->TXD_AMOUNT == 0) {
      return byte | 0x100;
    } else {
      return byte;
    }
  }
#else
  (void) inst;
  (void) u_el;
  return byte;
#endif
}

static void nHW_UARTE_Tx_DMA_byte(int inst, struct uarte_status *u_el)
{
  uint8_t *ptr = (uint8_t *)(u_el->TXD_PTR + u_el->TXD_AMOUNT);

  uint16_t data = nhw_UART_prep_Tx_data(inst, u_el, *ptr);

  nhw_UART_Tx_queue_byte(inst, u_el, data);
}

void nhw_UARTE_TASK_STARTTX(uint inst)
{
  struct uarte_status *u_el = &nhw_uarte_st[inst];

  if (!uart_enabled(inst) && !uarte_enabled(inst)) {
    bs_trace_warning_time_line("Start TX triggered while UART%i is not enabled (%u). "
                               "Ignoring it.\n", inst, NRF_UARTE_regs[inst].ENABLE);
    return;
  }

  if (u_el->tx_dma_status != DMA_Off) {
    bs_trace_warning_time_line("Start Tx triggered for UARTE%i whose Rx is already DMA'ing. "
                               "This seems like a SW error which the model does not handle. "
                               "Ignoring it\n", inst);
    return;
  }

  if (u_el->tx_status == Tx_Off) {
    u_el->tx_status = Tx_Idle;
    notify_backend_TxOnOff(inst, u_el, true);
  } else if (u_el->tx_status == Tx_Stopping) {
    /* A frame was still in flight and it was trying to stop
     * We abort the stop */
    u_el->tx_status = Txing;
  }

  if (uarte_enabled(inst)) {
#if !(NHW_UARTE_54NAMING)
    u_el->TXD_PTR = NRF_UARTE_regs[inst].TXD.PTR;
    u_el->TXD_MAXCNT = NRF_UARTE_regs[inst].TXD.MAXCNT;
#else
    u_el->TXD_PTR = NRF_UARTE_regs[inst].DMA.TX.PTR;
    u_el->TXD_MAXCNT = NRF_UARTE_regs[inst].DMA.TX.MAXCNT;
#endif
    u_el->TXD_AMOUNT = 0;
    u_el->tx_dma_status = DMAing;
    nhw_UARTE_signal_EVENTS_TXSTARTED(inst); /* Instantaneously ready */
    if (u_el->TXD_MAXCNT > 0) {
      if (u_el->tx_status == Tx_Idle) {
        nHW_UARTE_Tx_DMA_byte(inst, u_el);
      }
    } else {
      nHW_UARTE_Tx_DMA_end(inst, u_el);
    }
  }
}

static void nhw_UARTE_tx_final_stop(int inst, struct uarte_status *u_el) {
  u_el->tx_status = Tx_Off;
  notify_backend_TxOnOff(inst, u_el, false);

  if (uarte_enabled(inst)) {
    if (u_el->tx_dma_status == DMAing) {
      nHW_UARTE_Tx_DMA_end(inst, u_el);
    }
    nhw_UARTE_signal_EVENTS_TXSTOPPED(inst);
  }
}

void nhw_UARTE_TASK_STOPTX(uint inst)
{
  struct uarte_status * u_el = &nhw_uarte_st[inst];

  /* STOP behavior is slightly different in UART and UART-E modes */
  if (uart_enabled(inst)) {
    if (u_el->tx_status > Tx_Idle) {
      bs_trace_warning_time_line("UART%i Tx stopped while not idle\n", inst);
      if (u_el->tx_status == Tx_Pend) {
        bs_trace_warning_time_line("A byte was pending to Tx awaiting CTS, it is dropped\n");
        nhw_UARTE_tx_final_stop(inst, u_el);
      } else if (u_el->tx_status == Txing) {
        bs_trace_warning_time_line("A Tx byte is already in flight, it will be completed"
                                   "(this is not how real HW behaves)\n");
        /* In the model, the byte is already in flight, so nothing can't be done anymore */
        u_el->tx_status = Tx_Stopping;
      }
    } else {
      nhw_UARTE_tx_final_stop(inst, u_el);
    }
  }

  if (uarte_enabled(inst)) {
    if (u_el->tx_status == Txing) {
      u_el->tx_status = Tx_Stopping;
    } else if (u_el->tx_status == Tx_Stopping) {
      bs_trace_info(3, "UART%i STOPTX received while already stopping, ignored\n", inst);
      return;
    } else { /* Pend or Idle */
      nhw_UARTE_tx_final_stop(inst, u_el);
    }
  }
}

/*
 * Queue a byte for transmission right away in the backends
 */
static void nhw_UARTE_Tx_byte(unsigned int inst, struct uarte_status *u_el, uint16_t data) {
  if (u_el->trx_callbacks[0]) {
    u_el->trx_callbacks[0](inst, &data);
  }
  if (u_el->backend.tx_byte_f) {
    u_el->backend.tx_byte_f(inst, data);
  }
  if (u_el->Tx_log_file) {
    fprintf(u_el->Tx_log_file, "%"PRItime",0x%02X\n", nsi_hws_get_time(), data);
  }
}

/*
 * Start the process of actually sending the byte thru the line
 */
static void nhw_UARTE_Tx_send_byte(unsigned int inst, struct uarte_status *u_el) {
  nhw_UARTE_Tx_byte(inst, u_el, u_el->Tx_byte);
  u_el->Tx_byte_done_timer = nsi_hws_get_time() + nhw_uarte_one_byte_time(inst);
  u_el->tx_status = Txing;
  nhw_uarte_update_timer();

  if (u_el->tx_dma_status == DMAing) {
    u_el->TXD_AMOUNT++;
    if (u_el->TXD_AMOUNT >= u_el->TXD_MAXCNT) {
      nHW_UARTE_Tx_DMA_end(inst, u_el);
    }
  }
}

/*
 * Queue a byte to Tx'ed as soon as possible
 */
static void nhw_UART_Tx_queue_byte(uint inst, struct uarte_status *u_el, uint16_t byte)
{
  if (u_el->tx_status != Tx_Idle) {
    bs_trace_error_time_line("Attempted to queue a byte for Tx but a transmission is currently ongoing. "
                             "This should not have happened\n", inst);
    return;
  }

  u_el->Tx_byte = byte;

  /*
   * If !flow_control or (flow control & CTS low)
   *   Start the actual Tx
   * otherwise // (flow control & CTS high)
   *   "Pend" the Tx (set in a state in which a lower of CTS will start it)
   */
  if ((flow_control_on(inst) == false) || (u_el->CTS_blocking == false)) {
    nhw_UARTE_Tx_send_byte(inst, u_el);
  } else {
    u_el->tx_status = Tx_Pend;
  }
}

#if (NHW_UARTE_HAS_FRAMETIMEOUT)
static void nhw_uart_maybe_program_frametimeout(int inst) {
  if (!(NRF_UARTE_regs[inst].CONFIG & UARTE_CONFIG_FRAMETIMEOUT_Msk)) {
    return;
  }

  struct uarte_status *u_el = &nhw_uarte_st[inst];

  u_el->frametimeout_timer = nsi_hws_get_time() +
                 nhw_uarte_nbits_time(inst, NRF_UARTE_regs[inst].FRAMETIMEOUT);
  nhw_uarte_update_timer();
}

/*
 * The frame timeout timer has timed out
 */
static void nhw_uart_frametimeout_timer_triggered(int inst, struct uarte_status *u_el)
{
  u_el->frametimeout_timer = TIME_NEVER;
  nhw_UARTE_signal_EVENTS_FRAMETIMEOUT(inst);
}
#endif

/*
 * The Rx TO timer has timed out
 */
static void nhw_uart_Rx_TO_timer_triggered(int inst, struct uarte_status *u_el)
{
  if (u_el->rx_status != Rx_turning_off) {
    bs_trace_error_time_line("Programming error\n");
  }
  u_el->Rx_TO_timer = TIME_NEVER;
  u_el->rx_status = Rx_Off;
  if (u_el->rx_dma_status == DMAing) {
    nhw_UARTE_Rx_DMA_end(inst, u_el);
  }
  if (u_el->rx_status == Rx_Off) {
    /* The DMA end may have triggered thru a short ENDRX->STARTRX, which restarts the RX and
     * prevents the RXTO from being generated */
    nhw_UARTE_signal_EVENTS_RXTO(inst);
    notify_backend_RxOnOff(inst, u_el, false);
  }
}

static void nhw_uart_Tx_byte_done_timer_triggered(int inst, struct uarte_status *u_el)
{
  u_el->Tx_byte_done_timer = TIME_NEVER;
  nhw_UARTE_signal_EVENTS_TXDRDY(inst);

  if (u_el->tx_status == Txing) {
    u_el->tx_status = Tx_Idle;

  } else if (u_el->tx_status == Tx_Stopping) {
    nhw_UARTE_tx_final_stop(inst, u_el);
    return;
  }

  if (u_el->tx_dma_status == DMAing) {
    nHW_UARTE_Tx_DMA_byte(inst, u_el);
  }
}

static void nhw_uart_timer_triggered(void)
{
  bs_time_t current_time = Timer_UART_peri;

  for (int inst = 0; inst < NHW_UARTE_TOTAL_INST; inst++) {
    struct uarte_status *u_el = &nhw_uarte_st[inst];

#if (NHW_UARTE_HAS_FRAMETIMEOUT)
    if (current_time == u_el->frametimeout_timer) {
      nhw_uart_frametimeout_timer_triggered(inst, u_el);
    }
#endif
    if (current_time == u_el->Rx_TO_timer) {
      nhw_uart_Rx_TO_timer_triggered(inst, u_el);
    }
    if (current_time == u_el->Tx_byte_done_timer) {
      nhw_uart_Tx_byte_done_timer_triggered(inst, u_el);
    }
  }
  nhw_uarte_update_timer();
}

static void nhw_uart_timer_common_triggered(void)
{
  bs_time_t current_time = Timer_UART_common;
  if (current_time == nhw_Timer_ULoopback) {
    extern void nhw_ublb_timer_triggered(void);
    nhw_ublb_timer_triggered();
  }
  if (current_time == Timer_UART_peri) {
    nhw_uart_timer_triggered();
  }
}

NSI_HW_EVENT(Timer_UART_common, nhw_uart_timer_common_triggered, 50);

void nhw_UARTE_TASK_FLUSHRX(uint inst) {
  if (!uarte_enabled(inst)) {
    bs_trace_warning_time_line("TASK_FLUSHRX for UART%i while it is not enabled in UARTE mode\n",
                               inst);
    return;
  }

  struct uarte_status * u_el = &nhw_uarte_st[inst];

  u_el->rx_dma_status = DMAing;
  if (!NHW_UARTE_FLUSH_AMOUNT_BUG || (u_el->Rx_FIFO_cnt > 0)) {
    nhw_UARTE_RxDMA_start(inst);
  }

  if (u_el->rx_dma_status == DMAing) {
    nhw_UARTE_Rx_DMA_end(inst, u_el);
  }
}

#if (NHW_UARTE_HAS_UART)
void nhw_UARTE_TASK_SUSPEND(uint inst) {
  /* UART(not-E) only task */
  nhw_UARTE_TASK_STOPTX(inst);
  nhw_UARTE_TASK_STOPRX(inst);
}
#endif

void nhw_UARTE_regw_sideeffects_ENABLE(unsigned int inst) {
  struct uarte_status * u_el = &nhw_uarte_st[inst];

  if (NRF_UARTE_regs[inst].ENABLE != 0) {
    propagate_RTS_R(inst, u_el);
    return;
  }

  if (u_el->tx_status != Tx_Off) {
    bs_trace_warning_time_line("UART%i disabled while Tx was not Off (%i)\n", inst, u_el->tx_status);
  }
  if (u_el->rx_status != Rx_Off) {
    bs_trace_warning_time_line("UART%i disabled while Rx was not Off (%i)\n", inst, u_el->rx_status);
  }

  if (u_el->Rx_TO_timer != TIME_NEVER) {
    bs_trace_warning_time_line("UART%i disabled while Rx was shutting Off. Events will be missed\n", inst);
  }
  if (u_el->Tx_byte_done_timer != TIME_NEVER) {
    bs_trace_warning_time_line("UART%i disabled while Tx was mid frame. Events will be missed\n", inst);
  }

  u_el->tx_status = Tx_Off;
  u_el->rx_status = Rx_Off;

  u_el->Rx_TO_timer = TIME_NEVER;
  u_el->Tx_byte_done_timer = TIME_NEVER;
  nhw_uarte_update_timer();

  u_el->Rx_FIFO_cnt = 0;

  if (u_el->tx_dma_status != DMA_Off) {
    bs_trace_warning_time_line("UARTE%i disabled while Tx DMA was not Off. DMA interrupted mid way, ENDTX will be missing\n", inst);
  }
  if (u_el->rx_dma_status != DMA_Off) {
    bs_trace_warning_time_line("UARTE%i disabled while Rx DMA was not Off. DMA interrupted mid way, ENDRX will be missing\n", inst);
  }

  u_el->tx_dma_status = DMA_Off;
  u_el->rx_dma_status = DMA_Off;

  notify_backend_RxOnOff(inst, u_el, false);
}

void nhw_UARTE_regw_sideeffects_CONFIG(unsigned int inst) {
#if defined(UARTE_CONFIG_FRAMESIZE_Msk)
  uint frame_size = nhw_uarte_get_frame_size(inst);
  NRF_UARTE_regs[inst].CONFIG &= ~UARTE_CONFIG_FRAMESIZE_Msk;
  NRF_UARTE_regs[inst].CONFIG |= frame_size << UARTE_CONFIG_FRAMESIZE_Pos;
#endif
  if (NRF_UARTE_regs[inst].ENABLE != 0) {
    struct uarte_status *u_el = &nhw_uarte_st[inst];
    propagate_RTS_R(inst, u_el);
  }
}

uint32_t nhw_UARTE_regr_sideeffects_ERRORSRC(unsigned int inst) {
  uint32_t value = NRF_UARTE_regs[inst].ERRORSRC;
  NRF_UARTE_regs[inst].ERRORSRC = 0;
  return value;
}

void nhw_UARTE_regw_sideeffects_ERRORSRC(unsigned int inst) {
  NRF_UARTE_regs[inst].ERRORSRC = 0;
}

#if (NHW_UARTE_HAS_UART)
uint32_t nhw_UARTE_regr_sideeffects_RXD(unsigned int inst) {
  if (!uart_enabled(inst)) {
    bs_trace_warning("RXD read while UART%i was not enabled\n", inst);
  }
  /*
   * Read logic: If RxFIFO has anything pop a new value into RXD, otherwise just warn.
   *
   * If RTS is high & Rx is not stopped & the FIFO is empty: lower RTS
   *
   * If there is anything else in the Rx FIFO, signal another RXDRDY
   */

  uint32_t value;
  struct uarte_status * u_el = &nhw_uarte_st[inst];

  if (u_el->Rx_FIFO_cnt == 0) {
    bs_trace_warning("UART%i: Reading RXD without any new data there\n", inst);
    return NRF_UART_regs[inst]->RXD;
  }

  value = Rx_FIFO_pop(inst, u_el);

  if ((u_el->Rx_FIFO_cnt == 0)
      && (u_el->rx_status == Rx_On)) {
    lower_RTS_R(inst, u_el);
  }

  return value;
}

void nhw_UARTE_regw_sideeffects_TXD(unsigned int inst)
{
  if (!uart_enabled(inst)) {
    bs_trace_warning("TXD written while UART%i was not enabled (in non-E mode)\n", inst);
  }

  /*
   * If Tx is off warn and ignore (return)
   * If Tx is busy, warn and ignore (return)
   */

  struct uarte_status *u_el = &nhw_uarte_st[inst];

  if ( u_el->tx_status == Tx_Off ) {
    bs_trace_warning("UART%i.TXD written but it was Tx was not started => ignoring\n", inst);
    return;
  }
  if ((u_el->tx_status == Tx_Pend) || (u_el->tx_status == Txing)) {
    bs_trace_warning("UART%i.TXD written but a transmission is currently ongoing => ignoring it\n", inst);
    return;
  }

  nhw_UART_Tx_queue_byte(inst, u_el, NRF_UART_regs[inst]->TXD);
}
#endif

#if (NHW_HAS_PPI)
  #define _NHW_UARTE_XPPI_EVENT(inst, event, eventl)    \
  if (inst == 0) { \
    nrf_ppi_event(UARTE0_EVENTS_##event); \
  } else { \
    nrf_ppi_event(UARTE1_EVENTS_##event); \
  }
#elif (NHW_HAS_DPPI)
  #define _NHW_UARTE_XPPI_EVENT(inst, event, eventl)    \
     nhw_dppi_event_signal_if(nhw_uarte_st[inst].dppi_map,  \
                              NRF_UARTE_regs[inst].PUBLISH_##eventl)
#endif /* (NHW_HAS_PPI) / (NHW_HAS_DPPI)*/

#define _NHW_UARTE_SIGNAL_EVENT_body(event, eventl) \
  { \
    NRF_UARTE_regs[inst].EVENTS_##eventl = 1; \
    nhw_UARTE_eval_interrupt(inst); \
    _NHW_UARTE_XPPI_EVENT(inst, event, eventl); \
  }

#define NHW_UARTE_SIGNAL_EVENT(event, eventl) \
  static void nhw_UARTE_signal_EVENTS_##event(unsigned int inst) \
    _NHW_UARTE_SIGNAL_EVENT_body(event, eventl)

#define NHW_UARTE_SIGNAL_EVENT_ns(event, eventl) \
  static void nhw_UARTE_signal_EVENTS_##event##_noshort(unsigned int inst) \
    _NHW_UARTE_SIGNAL_EVENT_body(event, eventl)

NHW_UARTE_SIGNAL_EVENT_ns(CTS, CTS)
NHW_UARTE_SIGNAL_EVENT_ns(NCTS, NCTS)
NHW_UARTE_SIGNAL_EVENT_ns(RXDRDY, RXDRDY)
#if !(NHW_UARTE_54NAMING)
NHW_UARTE_SIGNAL_EVENT_ns(ENDRX, ENDRX) /* DMA Rx done */
NHW_UARTE_SIGNAL_EVENT_ns(ENDTX, ENDTX) /* DMA Tx done */
NHW_UARTE_SIGNAL_EVENT(RXSTARTED, RXSTARTED)
NHW_UARTE_SIGNAL_EVENT(TXSTARTED, TXSTARTED)
#else
NHW_UARTE_SIGNAL_EVENT_ns(ENDRX, DMA.RX.END)
NHW_UARTE_SIGNAL_EVENT_ns(ENDTX, DMA.TX.END)
NHW_UARTE_SIGNAL_EVENT(RXSTARTED, DMA.RX.READY)
NHW_UARTE_SIGNAL_EVENT(TXSTARTED, DMA.TX.READY)
#endif
#if (NHW_UARTE_HAS_FRAMETIMEOUT)
NHW_UARTE_SIGNAL_EVENT_ns(FRAMETIMEOUT, FRAMETIMEOUT)
#endif
NHW_UARTE_SIGNAL_EVENT(TXDRDY, TXDRDY)
NHW_UARTE_SIGNAL_EVENT(ERROR, ERROR)
NHW_UARTE_SIGNAL_EVENT(RXTO, RXTO) /* Receiver done closing */
NHW_UARTE_SIGNAL_EVENT(TXSTOPPED, TXSTOPPED)

static void nhw_UARTE_signal_EVENTS_CTS(unsigned int inst) {
#if (NHW_UARTE_HAS_UART)
#define UARTE_SHORTS_CTS_STARTRX_Msk UART_SHORTS_CTS_STARTRX_Msk
  if (uart_enabled(inst)) { //Only in UART mode
    NHW_SHORT(UARTE, inst, NRF_UARTE_regs[inst]., CTS, STARTRX)
  }
#endif
  nhw_UARTE_signal_EVENTS_CTS_noshort(inst);
}

static void nhw_UARTE_signal_EVENTS_NCTS(unsigned int inst) {
#if (NHW_UARTE_HAS_UART)
#define UARTE_SHORTS_NCTS_STOPRX_Msk UART_SHORTS_NCTS_STOPRX_Msk
  if (uart_enabled(inst)) { //Only in UART mode
    NHW_SHORT(UARTE, inst, NRF_UARTE_regs[inst]., NCTS, STOPRX)
  }
#endif
  nhw_UARTE_signal_EVENTS_NCTS_noshort(inst);
}

static void nhw_UARTE_signal_EVENTS_RXDRDY(unsigned int inst) {
#if (NHW_UARTE_HAS_FRAMETIMEOUT)
  nhw_uart_maybe_program_frametimeout(inst);
#endif
  nhw_UARTE_signal_EVENTS_RXDRDY_noshort(inst);
}

static void nhw_UARTE_signal_EVENTS_ENDRX(unsigned int inst) {
  if (uarte_enabled(inst)) { //Only in UART-E mode
#if !(NHW_UARTE_54NAMING)
    NHW_SHORT(UARTE, inst, NRF_UARTE_regs[inst]., ENDRX, STARTRX)
    NHW_SHORT(UARTE, inst, NRF_UARTE_regs[inst]., ENDRX, STOPRX)
#else
    NHW_SHORT_ST(UARTE, inst, NRF_UARTE_regs[inst]., DMA_RX_END, STARTRX, DMA_RX_START)
    NHW_SHORT_ST(UARTE, inst, NRF_UARTE_regs[inst]., DMA_RX_END, STOPRX, DMA_RX_STOP)
#endif
  }
  nhw_UARTE_signal_EVENTS_ENDRX_noshort(inst);
}

static void nhw_UARTE_signal_EVENTS_ENDTX(unsigned int inst) {
#if (NHW_UARTE_54NAMING)
  NHW_SHORT_ST(UARTE, inst, NRF_UARTE_regs[inst]., DMA_TX_END, STOPTX, DMA_TX_STOP)
#endif
  nhw_UARTE_signal_EVENTS_ENDTX_noshort(inst);
}

#if (NHW_UARTE_HAS_FRAMETIMEOUT)
static void nhw_UARTE_signal_EVENTS_FRAMETIMEOUT(unsigned int inst) {
  NHW_SHORT_ST(UARTE, inst, NRF_UARTE_regs[inst]., FRAMETIMEOUT, STOPRX, DMA_RX_STOP)
  nhw_UARTE_signal_EVENTS_FRAMETIMEOUT_noshort(inst);
}
#endif

#if (NHW_UARTE_HAS_MATCH)
static void nhw_UARTE_signal_EVENTS_DMA_RX_MATCH(unsigned int inst, unsigned int i) {
  if (NRF_UARTE_regs[inst].SHORTS & (UARTE_SHORTS_DMA_RX_MATCH0_DMA_RX_ENABLEMATCH1_Msk << i)) {
    nhw_UARTE_TASK_DMA_RX_ENABLEMATCH(inst, (i+1) % nhw_uarte_st[inst].n_match);
  }
  if (NRF_UARTE_regs[inst].SHORTS & (UARTE_SHORTS_DMA_RX_MATCH0_DMA_RX_DISABLEMATCH0_Msk << i)) {
    nhw_UARTE_TASK_DMA_RX_ENABLEMATCH(inst, i);
  }

  NRF_UARTE_regs[inst].EVENTS_DMA.RX.MATCH[i] = 1;
  nhw_UARTE_eval_interrupt(inst);
  nhw_dppi_event_signal_if(nhw_uarte_st[inst].dppi_map,
                           NRF_UARTE_regs[inst].PUBLISH_DMA.RX.MATCH[i]);
}
#endif

NHW_SIDEEFFECTS_INTSET(UARTE, NRF_UARTE_regs[inst]., NRF_UARTE_regs[inst].INTEN)
NHW_SIDEEFFECTS_INTCLR(UARTE, NRF_UARTE_regs[inst]., NRF_UARTE_regs[inst].INTEN)
NHW_SIDEEFFECTS_INTEN(UARTE, NRF_UARTE_regs[inst]., NRF_UARTE_regs[inst].INTEN)

NHW_SIDEEFFECTS_EVENTS(UARTE)

#if !(NHW_UARTE_54NAMING)
NHW_SIDEEFFECTS_TASKS(UARTE, NRF_UARTE_regs[inst]., STARTRX)
NHW_SIDEEFFECTS_TASKS(UARTE, NRF_UARTE_regs[inst]., STOPRX)
NHW_SIDEEFFECTS_TASKS(UARTE, NRF_UARTE_regs[inst]., STARTTX)
NHW_SIDEEFFECTS_TASKS(UARTE, NRF_UARTE_regs[inst]., STOPTX)
#else
NHW_SIDEEFFECTS_TASKS_ST(UARTE, NRF_UARTE_regs[inst]., STARTRX, DMA.RX.START)
NHW_SIDEEFFECTS_TASKS_ST(UARTE, NRF_UARTE_regs[inst]., STOPRX, DMA.RX.STOP)
NHW_SIDEEFFECTS_TASKS_ST(UARTE, NRF_UARTE_regs[inst]., STARTTX, DMA.TX.START)
NHW_SIDEEFFECTS_TASKS_ST(UARTE, NRF_UARTE_regs[inst]., STOPTX, DMA.TX.STOP)
#endif
#if NHW_UARTE_HAS_MATCH
void nhw_UARTE_regw_sideeffects_TASKS_DMA_RX_ENABLEMATCH(uint inst, uint i) {
  if (NRF_UARTE_regs[inst].TASKS_DMA.RX.ENABLEMATCH[i]) {
    NRF_UARTE_regs[inst].TASKS_DMA.RX.ENABLEMATCH[i] = 0;
    nhw_UARTE_TASK_DMA_RX_ENABLEMATCH(inst, i);
  }
}

void nhw_UARTE_regw_sideeffects_TASKS_DMA_RX_DISABLEMATCH(uint inst, uint i) {
  if (NRF_UARTE_regs[inst].TASKS_DMA.RX.DISABLEMATCH[i]) {
    NRF_UARTE_regs[inst].TASKS_DMA.RX.DISABLEMATCH[i] = 0;
    nhw_UARTE_TASK_DMA_RX_DISABLEMATCH(inst, i);
  }
}
#endif

NHW_SIDEEFFECTS_TASKS(UARTE, NRF_UARTE_regs[inst]., FLUSHRX)

#if (NHW_UARTE_HAS_UART)
void nhw_UARTE_regw_sideeffects_TASKS_SUSPEND(unsigned int inst) {
  /* Needs special treatment for being an UART(non-E) only task */
  if ( NRF_UART_regs[inst]->TASKS_SUSPEND ) {
    NRF_UART_regs[inst]->TASKS_SUSPEND = 0;
    nhw_UARTE_TASK_SUSPEND(inst);
  }
}
#endif

#define NHW_UARTE_REGW_SIDEFFECTS_SUBSCRIBE(TASK_N, TASK_ST_N)                       \
  static void nhw_UARTE_TASK_##TASK_N##_wrap(void* param)                            \
  {                                                                                  \
    nhw_UARTE_TASK_##TASK_N((int) param);                                            \
  }                                                                                  \
                                                                                     \
  void nhw_UARTE_regw_sideeffects_SUBSCRIBE_##TASK_N(uint inst)                      \
  {                                                                                  \
     static struct nhw_subsc_mem TASK_N##_subscribed[NHW_UARTE_TOTAL_INST];          \
     struct uarte_status *this = &nhw_uarte_st[inst];                                \
                                                                                     \
     nhw_dppi_common_subscribe_sideeffect(this->dppi_map,                            \
                                          this->UARTE_regs[inst]->SUBSCRIBE_##TASK_ST_N,\
                                          &TASK_N##_subscribed[inst],                \
                                          nhw_UARTE_TASK_##TASK_N##_wrap,            \
                                          (void*) inst);                             \
  }

#if (NHW_HAS_DPPI)
#if !(NHW_UARTE_54NAMING)
NHW_UARTE_REGW_SIDEFFECTS_SUBSCRIBE(STARTRX, STARTRX)
NHW_UARTE_REGW_SIDEFFECTS_SUBSCRIBE(STOPRX, STOPRX)
NHW_UARTE_REGW_SIDEFFECTS_SUBSCRIBE(STARTTX, STARTTX)
NHW_UARTE_REGW_SIDEFFECTS_SUBSCRIBE(STOPTX, STOPTX)
#else
NHW_UARTE_REGW_SIDEFFECTS_SUBSCRIBE(STARTRX, DMA.RX.START)
NHW_UARTE_REGW_SIDEFFECTS_SUBSCRIBE(STOPRX, DMA.RX.STOP)
NHW_UARTE_REGW_SIDEFFECTS_SUBSCRIBE(STARTTX, DMA.TX.START)
NHW_UARTE_REGW_SIDEFFECTS_SUBSCRIBE(STOPTX, DMA.TX.STOP)
#endif
NHW_UARTE_REGW_SIDEFFECTS_SUBSCRIBE(FLUSHRX, FLUSHRX)
#if NHW_UARTE_HAS_MATCH
static void nhw_UARTE_TASK_nhw_UARTE_TASK_DMA_RX_ENABLEMATCH_wrap(void* param)
{
  uint inst = (intptr_t)param >> 8;
  uint i = (intptr_t)param & 0xFF;
  nhw_UARTE_TASK_DMA_RX_ENABLEMATCH(inst, i);
}

void nhw_UARTE_regw_sideeffects_SUBSCRIBE_DMA_RX_ENABLEMATCH(uint inst, uint i)
{
   struct uarte_status *this = &nhw_uarte_st[inst];
   uint param = (inst << 8 || (i & 0xFF));

   nhw_dppi_common_subscribe_sideeffect(this->dppi_map,
                                        this->UARTE_regs[inst]->SUBSCRIBE_DMA.RX.ENABLEMATCH[i],
                                        &this->DMA_RX_ENABLEMATCH_subscribed[i],
                                        nhw_UARTE_TASK_nhw_UARTE_TASK_DMA_RX_ENABLEMATCH_wrap,
                                        (void*) param);
}

static void nhw_UARTE_TASK_nhw_UARTE_TASK_DMA_RX_DISABLEMATCH_wrap(void* param)
{
  uint inst = (intptr_t)param >> 8;
  uint i = (intptr_t)param & 0xFF;
  nhw_UARTE_TASK_DMA_RX_DISABLEMATCH(inst, i);
}

void nhw_UARTE_regw_sideeffects_SUBSCRIBE_DMA_RX_DISABLEMATCH(uint inst, uint i)
{
   struct uarte_status *this = &nhw_uarte_st[inst];
   uint param = (inst << 8 || (i & 0xFF));

   nhw_dppi_common_subscribe_sideeffect(this->dppi_map,
                                        this->UARTE_regs[inst]->SUBSCRIBE_DMA.RX.DISABLEMATCH[i],
                                        &this->DMA_RX_DISABLEMATCH_subscribed[i],
                                        nhw_UARTE_TASK_nhw_UARTE_TASK_DMA_RX_DISABLEMATCH_wrap,
                                        (void*) param);
}
#endif
#endif /* NHW_HAS_DPPI */

#if (NHW_HAS_PPI)
void nhw_uarte0_TASKS_STARTRX(void) { nhw_UARTE_TASK_STARTRX(0); }
void nhw_uarte0_TASKS_STOPRX(void) { nhw_UARTE_TASK_STOPRX(0); }
void nhw_uarte0_TASKS_STARTTX(void) { nhw_UARTE_TASK_STARTTX(0); }
void nhw_uarte0_TASKS_STOPTX(void) { nhw_UARTE_TASK_STOPTX(0); }
void nhw_uarte0_TASKS_SUSPEND(void) { nhw_UARTE_TASK_SUSPEND(0); }
void nhw_uarte0_TASKS_FLUSHRX(void) { nhw_UARTE_TASK_FLUSHRX(0); }

void nhw_uarte1_TASKS_STARTRX(void) { nhw_UARTE_TASK_STARTRX(1); }
void nhw_uarte1_TASKS_STOPRX(void) { nhw_UARTE_TASK_STOPRX(1); }
void nhw_uarte1_TASKS_STARTTX(void) { nhw_UARTE_TASK_STARTTX(1); }
void nhw_uarte1_TASKS_STOPTX(void) { nhw_UARTE_TASK_STOPTX(1); }
void nhw_uarte1_TASKS_SUSPEND(void) { nhw_UARTE_TASK_SUSPEND(1); }
void nhw_uarte1_TASKS_FLUSHRX(void) { nhw_UARTE_TASK_FLUSHRX(1); }
#endif /* (NHW_HAS_PPI) */

static void uart_list(char *argv, int offset) {
  (void) argv;
  (void) offset;
  char *uart_names[NHW_UARTE_TOTAL_INST] = NHW_UARTE_NAMES;
  printf("UART #: UART name\n");
  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {
    printf("%6i: %s\n", i, uart_names[i]);
  }
  bs_trace_exit(0);
}

static void nhw_uart_register_cmdline(void) {
  static bs_args_struct_t args[] = {
    { .is_switch = true,
      .option = "uart_list",
      .type = 'b',
      .call_when_found = uart_list,
      .descript = "List available UARTs"
    },
    ARG_TABLE_ENDMARKER
  };

  bs_add_extra_dynargs(args);

  static bs_args_struct_t args2[2*NHW_UARTE_TOTAL_INST + 1 /* End marker */];
  static char descr_tx[] = "Path to a file where the Tx bytes will be logged. Setting this enables the Tx logging for this UART";
  static char descr_rx[] = "Path to a file where the Rx bytes will be logged. Setting this enables the Rx logging for this UART";
#define OPTION_LEN (4 + 2 + 11 + 1)
  static char options[NHW_UARTE_TOTAL_INST][2][OPTION_LEN];
  static char opt_name[]= "path";

  for (int i = 0 ; i < NHW_UARTE_TOTAL_INST; i++) {
    snprintf(options[i][0], OPTION_LEN, "uart%i_log_txfile", i);
    snprintf(options[i][1], OPTION_LEN, "uart%i_log_rxfile", i);

    args2[2*i].option = options[i][0];
    args2[2*i].name = opt_name;
    args2[2*i].type = 's';
    args2[2*i].dest = &nhw_uarte_st[i].Tx_log_file_name;
    args2[2*i].descript = descr_tx;

    args2[2*i + 1].option = options[i][1];
    args2[2*i + 1].name = opt_name;
    args2[2*i + 1].type = 's';
    args2[2*i + 1].dest = &nhw_uarte_st[i].Rx_log_file_name;
    args2[2*i + 1].descript = descr_rx;
  }

  bs_add_extra_dynargs(args2);
}

NSI_TASK(nhw_uart_register_cmdline, PRE_BOOT_1, 199); /* Just before the backends */
