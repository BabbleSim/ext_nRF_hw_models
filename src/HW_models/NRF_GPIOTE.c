/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * GPIOTE - GPIO tasks and events
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/gpiote.html?cp=5_1_0_5_8
 *
 * This model has the following limitations:
 *  * INTENCLR will always read as 0
 *  * Unlike in real HW, tasks cannot occur simultaneously, they always happen in some sequence
 *    so task priority is not accounted for
 *
 * 54L notes:
 *  * Unlike in real HW, a GPIOTE channel can be connected to any GPIO port and pin.
 *
 *  * Both EVENTS_PORT.SECURE & NONSECURE will be raised at the same time. This is due to the GPIO
 *    not considering which pins are labeled as secure or not secure in the SPU.
 *
 *  * Split security distinctions are ignored
 *    == there is no distinction for accesses from secure or non secure bus masters or the S/NS address ranges.
 *    Accessing from either the S or NS address range all registers are equally accessible.
 */

#include <stdint.h>
#include <string.h>
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NRF_GPIOTE.h"
#include "NRF_GPIO.h"
#include "NRF_PPI.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"
#include "nsi_tasks.h"

NRF_GPIOTE_Type NRF_GPIOTE_regs[NHW_GPIOTE_TOTAL_INST] = {0};
/* Mapping of peripheral instance to {int controller instance, int number} */
static struct nhw_irq_mapping nhw_gpiote_irq_map[NHW_GPIOTE_TOTAL_INST][NHW_GPIOTE_N_INT] = NHW_GPIOTE_INT_MAP;

/* For each GPIO channel, its status */
struct gpiote_ch_status_t {
  uint8_t mode;
  uint8_t port;  /* GPIO instance */
  uint8_t pin;   /* GPIO pin in that instance (psel) */
  uint8_t polarity; /* Content of the CONFIG[].polarity field */
  /* Level at which the GPIOTE has been driving this pin,
   * or which it has been getting from the GPIO
   * true = high, false = low */
  bool level;
};

struct gpiote_status {
  struct gpiote_ch_status_t gpiote_ch_status[NHW_GPIOTE_MAX_CHANNELS];
  uint n_channels;
  uint32_t GPIOTE_ITEN[NHW_GPIOTE_N_INT];
  bool gpiote_int_line[NHW_GPIOTE_N_INT]; /* Is the GPIOTE currently driving its interrupt line high */
};
static struct gpiote_status gpiote_st[NHW_GPIOTE_TOTAL_INST];

/**
 * Initialize the GPIOs model
 */
static void nrf_gpiote_init(void) {
  memset(&NRF_GPIOTE_regs, 0, sizeof(NRF_GPIOTE_regs));

  uint n_ch[] = NHW_GPIOTE_CHANNELS;
  for (int i = 0; i < NHW_GPIOTE_TOTAL_INST; i++) {
    gpiote_st[i].n_channels = n_ch[i];
  }
}

NSI_TASK(nrf_gpiote_init, HW_INIT, 100);

static void nhw_GPIOTE_signal_EVENTS_PORT(unsigned int inst);

/*
 * API for the GPIO components, in which they can signal that their DETECT output signal
 * has been raised.
 *
 * We do not keep track in the GPIOTE about the signal being lowered, as the GPIOTE
 * only reacts to raising edges.
 * Therefore it is the responsibility of the GPIO models to call this if and only
 * if there is a raising edge on their DETECT output signal.
 */
void nrf_gpiote_port_detect_raise(unsigned int te_inst, unsigned int port) {
  (void)port; /* unused */
  nhw_GPIOTE_signal_EVENTS_PORT(te_inst);
}

void nrf_gpiote_TASKS_OUT(unsigned int inst, unsigned int n) {
  struct gpiote_ch_status_t *sc = &gpiote_st[inst].gpiote_ch_status[n];
  if (sc->mode != GPIOTE_CONFIG_MODE_Task) {
    return;
  }
  switch (sc->polarity) {
  case GPIOTE_CONFIG_POLARITY_None:
    return;
  case GPIOTE_CONFIG_POLARITY_LoToHi:
    sc->level = true;
    break;
  case GPIOTE_CONFIG_POLARITY_HiToLo:
    sc->level = false;
    break;
  case GPIOTE_CONFIG_POLARITY_Toggle:
    sc->level = !sc->level;
    break;
  default:	/* LCOV_EXCL_START */
    bs_trace_error_time_line("%s: Missconfigured CONFIG.CONFIG[%i]\n", n);
    break;
  }		/* LCOV_EXCL_STOP */
  /* We may be calling the GPIO even if we haven't changed it, but that is fine */
  nrf_gpio_peri_change_output(sc->port, sc->pin, sc->level);
}

void nrf_gpiote_TASKS_SET(unsigned int inst, unsigned int n) {
  struct gpiote_ch_status_t *sc = &gpiote_st[inst].gpiote_ch_status[n];
  if (sc->mode != GPIOTE_CONFIG_MODE_Task) {
    return;
  }
  sc->level = true;
  /* We may be calling the GPIO even if we haven't changed it, but that is fine */
  nrf_gpio_peri_change_output(sc->port, sc->pin, sc->level);
}

void nrf_gpiote_TASKS_CLR(unsigned int inst, unsigned int n) {
  struct gpiote_ch_status_t *sc = &gpiote_st[inst].gpiote_ch_status[n];
  if (sc->mode != GPIOTE_CONFIG_MODE_Task) {
    return;
  }
  sc->level = false;
  /* We may be calling the GPIO even if we haven't changed it, but that is fine */
  nrf_gpio_peri_change_output(sc->port, sc->pin, sc->level);
}

static void nrf_gpiote_eval_interrupt(unsigned int inst) {
  struct gpiote_status *st = &gpiote_st[inst];
  bool new_int_line;
  int mask;

  for (int line= 0; line < NHW_GPIOTE_N_INT; line ++) {
    new_int_line = false;

    for (int i = 0; i < st->n_channels; i++) {
      mask = (st->GPIOTE_ITEN[line] >> i) & 0x1;
      if (NRF_GPIOTE_regs[inst].EVENTS_IN[i] && mask) {
        new_int_line = true;
        break; /* No need to check more */
      }
    }
    //TODO: Double PORT interrupts for 54L (PORT0NONSECURE PORT0SECURE)

    mask = (st->GPIOTE_ITEN[line] & GPIOTE_INTENCLR_PORT_Msk) >> GPIOTE_INTENCLR_PORT_Pos;
    if (NRF_GPIOTE_regs[inst].EVENTS_PORT && mask) {
      new_int_line = true;
    }

    hw_irq_ctrl_toggle_level_irq_line_if(&st->gpiote_int_line[line],
        new_int_line,
        &nhw_gpiote_irq_map[inst][line]);
  }
}

static void nhw_GPIOTE_signal_EVENTS_IN(unsigned int inst, unsigned int n) {
  NRF_GPIOTE_regs[inst].EVENTS_IN[n] = 1;
  nrf_gpiote_eval_interrupt(inst);
  nrf_ppi_event(GPIOTE_EVENTS_IN_0 + n);
  //TODO: PPI vs DPPI
}

static void nhw_GPIOTE_signal_EVENTS_PORT(unsigned int inst) {
  NRF_GPIOTE_regs[inst].EVENTS_PORT = 1;
  nrf_gpiote_eval_interrupt(inst);
  nrf_ppi_event(GPIOTE_EVENTS_PORT);
  //TODO: PPI vs DPPI
  //TODO: 54L generate both PORT SECURE & NONSECURE EVENTS
}

/*
 * Function to be called (by the GPIO model) when a pin changes
 * for which an EVENTS_IN is registered
 */
static void nrf_gpiote_input_change_ntf(unsigned int port, unsigned int pin_n, bool value, void *cb_data)
{
  (void)pin_n; /* Unused */
  uint inst = (intptr_t)cb_data >> 8;
  int ch_n = (intptr_t)cb_data & 0xFF;
  struct gpiote_ch_status_t *sc;
  bool generate_event = false;

  sc = &gpiote_st[inst].gpiote_ch_status[ch_n];

  switch (sc->polarity) {
  case GPIOTE_CONFIG_POLARITY_None:
    return;
  case GPIOTE_CONFIG_POLARITY_LoToHi:
    if ((sc->level == false) && (value == true)) {
      generate_event = true;
    }
    break;
  case GPIOTE_CONFIG_POLARITY_HiToLo:
    if ((sc->level == true) && (value == false)) {
      generate_event = true;
    }
    break;
  case GPIOTE_CONFIG_POLARITY_Toggle:
    if (sc->level != value) {
      generate_event = true;
    }
    break;
  default: /* LCOV_EXCL_START */
    bs_trace_error_time_line("%s: Missconfigured CONFIG.CONFIG[%i]\n", ch_n);
    break;
  } /* LCOV_EXCL_STOP */
  sc->level = value;

  if (generate_event) {
    nhw_GPIOTE_signal_EVENTS_IN(inst, ch_n);
  }
}

/*
 * Register write side-effecting functions
 */

void nrf_gpiote_regw_sideeffects_TASKS_OUT(unsigned int inst, unsigned int n) {
  if (NRF_GPIOTE_regs[inst].TASKS_OUT[n]) {
    NRF_GPIOTE_regs[inst].TASKS_OUT[n] = 0;
    nrf_gpiote_TASKS_OUT(inst, n);
  }
}

void nrf_gpiote_regw_sideeffects_TASKS_SET(unsigned int inst, unsigned int n) {
  if (NRF_GPIOTE_regs[inst].TASKS_SET[n]) {
    NRF_GPIOTE_regs[inst].TASKS_SET[n] = 0;
    nrf_gpiote_TASKS_SET(inst, n);
  }
}

void nrf_gpiote_regw_sideeffects_TASKS_CLR(unsigned int inst, unsigned int n) {
  if (NRF_GPIOTE_regs[inst].TASKS_CLR[n]) {
    NRF_GPIOTE_regs[inst].TASKS_CLR[n] = 0;
    nrf_gpiote_TASKS_CLR(inst, n);
  }
}

void nrf_gpiote_regw_sideeffects_EVENTS_IN(unsigned int inst, unsigned int n) {
  nrf_gpiote_eval_interrupt(inst);
}

void nrf_gpiote_regw_sideeffects_EVENTS_PORT(unsigned int inst) {
  nrf_gpiote_eval_interrupt(inst);
  //TODO: For 54, have HAL call this one for both PORT.SECURE and PORT.NONSECURE
}

#if NHW_GPIOTE_IS_54
static const ptrdiff_t gpiote_int_pdiff =
    offsetof(NRF_GPIOTE_Type, INTENSET1) - offsetof(NRF_GPIOTE_Type, INTENSET0);
#endif

void nrf_gpiote_regw_sideeffects_INTENSET(unsigned int inst, unsigned int interrupt_nbr) {
#if NHW_GPIOTE_IS_54
  uint32_t *INTENSET = (uint32_t *)((uintptr_t)&NRF_GPIOTE_regs[inst].INTENSET0 + interrupt_nbr*gpiote_int_pdiff);
#else
  uint32_t *INTENSET = (uint32_t *)(uintptr_t)&NRF_GPIOTE_regs[inst].INTENSET;
#endif

  if (*INTENSET) {
    gpiote_st[inst].GPIOTE_ITEN[interrupt_nbr] |= *INTENSET ;
    *INTENSET  = gpiote_st[inst].GPIOTE_ITEN[interrupt_nbr];
    nrf_gpiote_eval_interrupt(inst);
  }
}

void nrf_gpiote_regw_sideeffects_INTENCLR(unsigned int inst, unsigned int interrupt_nbr) {
#if NHW_GPIOTE_IS_54
  uint32_t *INTENCLR = (uint32_t *)((uintptr_t)&NRF_GPIOTE_regs[inst].INTENCLR0 + interrupt_nbr*gpiote_int_pdiff);
#else
  uint32_t *INTENCLR = (uint32_t *)(uintptr_t)&NRF_GPIOTE_regs[inst].INTENCLR;
#endif

  if (*INTENCLR) {
    gpiote_st[inst].GPIOTE_ITEN[interrupt_nbr] &= ~*INTENCLR;
    *INTENCLR = 0;
    nrf_gpiote_eval_interrupt(inst);
  }
}

void nrf_gpiote_regw_sideeffects_CONFIG(unsigned int inst, unsigned int ch_n) {
  struct gpiote_ch_status_t *sc = &gpiote_st[inst].gpiote_ch_status[ch_n];
  unsigned int mode = NRF_GPIOTE_regs[inst].CONFIG[ch_n] & GPIOTE_CONFIG_MODE_Msk;
  unsigned int pin = (NRF_GPIOTE_regs[inst].CONFIG[ch_n] & GPIOTE_CONFIG_PSEL_Msk)
                      >>GPIOTE_CONFIG_PSEL_Pos;
  unsigned int port = (NRF_GPIOTE_regs[inst].CONFIG[ch_n] & GPIOTE_CONFIG_PORT_Msk)
                       >>GPIOTE_CONFIG_PORT_Pos;
  unsigned int polarity = (NRF_GPIOTE_regs[inst].CONFIG[ch_n] & GPIOTE_CONFIG_POLARITY_Msk)
                           >>GPIOTE_CONFIG_POLARITY_Pos;
  unsigned int outinit = (NRF_GPIOTE_regs[inst].CONFIG[ch_n] & GPIOTE_CONFIG_OUTINIT_Msk)
                          >>GPIOTE_CONFIG_OUTINIT_Pos;

  if ((port != sc->port) || (pin != sc->pin)
      || (mode == GPIOTE_CONFIG_MODE_Disabled  && sc->mode != GPIOTE_CONFIG_MODE_Disabled)) {
    /* Disconnect the old GPIO pin from the GPIOTE */
    nrf_gpio_peri_pin_control(sc->port, sc->pin, 0, 0, 0, NULL, NULL, -1);
  }

  sc->mode = mode;
  sc->pin = pin;
  sc->port = port;
  sc->polarity = polarity;

  if (mode == GPIOTE_CONFIG_MODE_Event) {
    sc->level = nrf_gpio_get_pin_level(port, pin);
    intptr_t cb_data = (inst<<8) + ch_n;
    nrf_gpio_peri_pin_control(port, pin, 1, 3, 2, nrf_gpiote_input_change_ntf, (void *)cb_data, -1);
  } else if (mode == GPIOTE_CONFIG_MODE_Task) {
    sc->level = outinit;
    nrf_gpio_peri_pin_control(port, pin, 1, 2, 3, NULL, NULL, outinit);
  } else if (mode != GPIOTE_CONFIG_MODE_Disabled) { /* LCOV_EXCL_START */
    bs_trace_error_time_line("%s: GPIOTE.CONFIG[%u].mode configured to an illegal "
        "value(%u)\n", __func__, ch_n, mode);
  } /* LCOV_EXCL_STOP */
}

/*
 * Trampolines to automatically call from the PPI
 */
/* Generated with:
#! /usr/bin/env python3
#GPIOTE.c
for task in {"OUT","SET","CLR"}:
    for i in range(0,8):
       print("void nrf_gpiote_TASKS_%s_%i(void){ nrf_gpiote_TASKS_%s(%i); }"%(task,i,task,i))
#GPIOTE.h
for task in {"OUT","SET","CLR"}:
    for i in range(0,8):
       print("void nrf_gpiote_TASKS_%s_%i(void);"%(task,i))
 */
#if NHW_HAS_PPI
void nrf_gpiote_TASKS_SET_0(void){ nrf_gpiote_TASKS_SET(0, 0); }
void nrf_gpiote_TASKS_SET_1(void){ nrf_gpiote_TASKS_SET(0, 1); }
void nrf_gpiote_TASKS_SET_2(void){ nrf_gpiote_TASKS_SET(0, 2); }
void nrf_gpiote_TASKS_SET_3(void){ nrf_gpiote_TASKS_SET(0, 3); }
void nrf_gpiote_TASKS_SET_4(void){ nrf_gpiote_TASKS_SET(0, 4); }
void nrf_gpiote_TASKS_SET_5(void){ nrf_gpiote_TASKS_SET(0, 5); }
void nrf_gpiote_TASKS_SET_6(void){ nrf_gpiote_TASKS_SET(0, 6); }
void nrf_gpiote_TASKS_SET_7(void){ nrf_gpiote_TASKS_SET(0, 7); }
void nrf_gpiote_TASKS_CLR_0(void){ nrf_gpiote_TASKS_CLR(0, 0); }
void nrf_gpiote_TASKS_CLR_1(void){ nrf_gpiote_TASKS_CLR(0, 1); }
void nrf_gpiote_TASKS_CLR_2(void){ nrf_gpiote_TASKS_CLR(0, 2); }
void nrf_gpiote_TASKS_CLR_3(void){ nrf_gpiote_TASKS_CLR(0, 3); }
void nrf_gpiote_TASKS_CLR_4(void){ nrf_gpiote_TASKS_CLR(0, 4); }
void nrf_gpiote_TASKS_CLR_5(void){ nrf_gpiote_TASKS_CLR(0, 5); }
void nrf_gpiote_TASKS_CLR_6(void){ nrf_gpiote_TASKS_CLR(0, 6); }
void nrf_gpiote_TASKS_CLR_7(void){ nrf_gpiote_TASKS_CLR(0, 7); }
void nrf_gpiote_TASKS_OUT_0(void){ nrf_gpiote_TASKS_OUT(0, 0); }
void nrf_gpiote_TASKS_OUT_1(void){ nrf_gpiote_TASKS_OUT(0, 1); }
void nrf_gpiote_TASKS_OUT_2(void){ nrf_gpiote_TASKS_OUT(0, 2); }
void nrf_gpiote_TASKS_OUT_3(void){ nrf_gpiote_TASKS_OUT(0, 3); }
void nrf_gpiote_TASKS_OUT_4(void){ nrf_gpiote_TASKS_OUT(0, 4); }
void nrf_gpiote_TASKS_OUT_5(void){ nrf_gpiote_TASKS_OUT(0, 5); }
void nrf_gpiote_TASKS_OUT_6(void){ nrf_gpiote_TASKS_OUT(0, 6); }
void nrf_gpiote_TASKS_OUT_7(void){ nrf_gpiote_TASKS_OUT(0, 7); }
#endif
