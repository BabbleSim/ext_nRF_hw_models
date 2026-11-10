/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* PWM - Pulse Width Modulation
 *
 * https://docs.nordicsemi.com/r/bundle/ps_nrf52833/page/pwm.html
 * https://docs.nordicsemi.com/r/bundle/ps_nrf54l15/page/pwm.html
 *
 * Notes:
 *  - PWM outputs are routed through the GPIO model.
 *  - This model can be CPU-intensive, especially with longer or higher-frequency playbacks.
 *  - Sub-us timing is calculated but rounded to microsecond time (see pwm_subus_to_time())
 *
 * nRF54L15:
 *   - PWM20-22 (pins limited to P1).
 *   - RAMUNDERFLOW and the DMA bus-error events and shorts never fire.
 *     The interrupt bits can be set but nothing triggers them.
 *   - DMA transfers are instant. No bus transaction is modeled.
 *   - Writing PSEL directly while the PWM is already enabled takes effect at the next period start rather than immediately. With nrfx the pins are set before ENABLE, so the normal flow is covered.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_PWM.h"
#include "NRF_GPIO.h"
#include "NHW_xPPI.h"
#include "irq_ctrl.h"
#include "nsi_hw_scheduler.h"
#include "nsi_hws_models_if.h"
#include "nsi_tasks.h"
#include "bs_tracing.h"

#if defined(NRF52833) || defined(NRF52833_XXAA) || \
    defined(NRF54L15) || defined(NRF54L15_XXAA)

#define PWM_SUBUS_PER_US (1ULL << 9)
#define PWM_BASE_TICK_SUBUS 32ULL /* 1/16 MHz */
#define PWM_SEQUENCE_MASK 0x7FFFU
#define PWM_VALUE_MASK 0x7FFFU
#define PWM_VALUE_POLARITY_Msk 0x8000U
#define PWM_NO_EDGE UINT64_MAX

/* Event identifiers used internally. 0..6 are common to all chips */
enum {
  PWM_EV_STOPPED = 0,
  PWM_EV_SEQSTARTED0,
  PWM_EV_SEQSTARTED1,
  PWM_EV_SEQEND0,
  PWM_EV_SEQEND1,
  PWM_EV_PWMPERIODEND,
  PWM_EV_LOOPSDONE,
#if NHW_HAS_DPPI
  PWM_EV_DMA_SEQ0_END,
  PWM_EV_DMA_SEQ0_READY,
  PWM_EV_DMA_SEQ1_END,
  PWM_EV_DMA_SEQ1_READY,
  PWM_EV_COMPAREMATCH0,
  PWM_EV_COMPAREMATCH1,
  PWM_EV_COMPAREMATCH2,
  PWM_EV_COMPAREMATCH3,
#endif
  PWM_EV_COUNT
};

static const uint32_t pwm_event_inten_mask[PWM_EV_COUNT] = {
  [PWM_EV_STOPPED] = PWM_INTENSET_STOPPED_Msk,
  [PWM_EV_SEQSTARTED0] = PWM_INTENSET_SEQSTARTED0_Msk,
  [PWM_EV_SEQSTARTED1] = PWM_INTENSET_SEQSTARTED1_Msk,
  [PWM_EV_SEQEND0] = PWM_INTENSET_SEQEND0_Msk,
  [PWM_EV_SEQEND1] = PWM_INTENSET_SEQEND1_Msk,
  [PWM_EV_PWMPERIODEND] = PWM_INTENSET_PWMPERIODEND_Msk,
  [PWM_EV_LOOPSDONE] = PWM_INTENSET_LOOPSDONE_Msk,
#if NHW_HAS_DPPI
  [PWM_EV_DMA_SEQ0_END] = PWM_INTEN_DMASEQ0END_Msk,
  [PWM_EV_DMA_SEQ0_READY] = PWM_INTEN_DMASEQ0READY_Msk,
  [PWM_EV_DMA_SEQ1_END] = PWM_INTEN_DMASEQ1END_Msk,
  [PWM_EV_DMA_SEQ1_READY] = PWM_INTEN_DMASEQ1READY_Msk,
  [PWM_EV_COMPAREMATCH0] = PWM_INTEN_COMPAREMATCH0_Msk,
  [PWM_EV_COMPAREMATCH1] = PWM_INTEN_COMPAREMATCH1_Msk,
  [PWM_EV_COMPAREMATCH2] = PWM_INTEN_COMPAREMATCH2_Msk,
  [PWM_EV_COMPAREMATCH3] = PWM_INTEN_COMPAREMATCH3_Msk,
#endif
};

#if NHW_HAS_DPPI
/* Interrupts without a modeled event (RAMUNDERFLOW and BUSERROR) are accepted but never fire */
#define PWM_EVENT_MASK (PWM_INTEN_STOPPED_Msk | PWM_INTEN_SEQSTARTED0_Msk | \
                        PWM_INTEN_SEQSTARTED1_Msk | PWM_INTEN_SEQEND0_Msk | \
                        PWM_INTEN_SEQEND1_Msk | PWM_INTEN_PWMPERIODEND_Msk | \
                        PWM_INTEN_LOOPSDONE_Msk | PWM_INTEN_RAMUNDERFLOW_Msk | \
                        PWM_INTEN_DMASEQ0END_Msk | PWM_INTEN_DMASEQ0READY_Msk | \
                        PWM_INTEN_DMASEQ0BUSERROR_Msk | PWM_INTEN_DMASEQ1END_Msk | \
                        PWM_INTEN_DMASEQ1READY_Msk | PWM_INTEN_DMASEQ1BUSERROR_Msk | \
                        PWM_INTEN_COMPAREMATCH0_Msk | PWM_INTEN_COMPAREMATCH1_Msk | \
                        PWM_INTEN_COMPAREMATCH2_Msk | PWM_INTEN_COMPAREMATCH3_Msk)
#define PWM_SHORT_LOOPSDONE_SEQSTART(seq) (PWM_SHORTS_LOOPSDONE_DMA_SEQ0_START_Msk << (seq))
#else
#define PWM_EVENT_MASK (PWM_INTENSET_STOPPED_Msk | \
                        PWM_INTENSET_SEQSTARTED0_Msk | \
                        PWM_INTENSET_SEQSTARTED1_Msk | \
                        PWM_INTENSET_SEQEND0_Msk | \
                        PWM_INTENSET_SEQEND1_Msk | \
                        PWM_INTENSET_PWMPERIODEND_Msk | \
                        PWM_INTENSET_LOOPSDONE_Msk)
#define PWM_SHORT_LOOPSDONE_SEQSTART(seq) (PWM_SHORTS_LOOPSDONE_SEQSTART0_Msk << (seq))
#endif

struct pwm_output {
  bool owned;
  uint8_t port;
  uint8_t pin;
  bool active_level;
  bool edge_level;
  bool edge_is_period_start;
  uint64_t period_start_subus;
  uint64_t period_subus;
  uint64_t next_edge_subus;
  uint32_t compare;
  uint32_t countertop;
};

struct pwm_status {
  bool running;
  bool sequence_active;
  bool stop_pending;
  bool at_period_end;
  uint sequence;
  uint32_t sample;
  uint32_t periods_to_step;
  uint32_t loops_left;
  uint32_t enddelay_left;
  uint32_t countertop;
  uint64_t next_period_subus;
  uint64_t event_time_subus;
  uint32_t task_generation;
  uint32_t INTEN;
  bool in_event;
  struct pwm_output output[4];
};

NRF_PWM_Type NRF_PWM_regs[NHW_PWM_TOTAL_INST];
static struct pwm_status nhw_pwm_st[NHW_PWM_TOTAL_INST];
static bs_time_t PWM_timer = TIME_NEVER;

static bool nhw_pwm_sequence_complete(uint inst);
static void nhw_pwm_signal_event(uint inst, uint event);

static bs_time_t pwm_subus_to_time(uint64_t subus)
{
  if (subus == UINT64_MAX) {
    return TIME_NEVER;
  }
  uint64_t us = subus / PWM_SUBUS_PER_US;
  if (subus % PWM_SUBUS_PER_US != 0) {
    us++;
  }

  if (us >= (uint64_t)TIME_NEVER) {
    return TIME_NEVER;
  }
  return (bs_time_t)us;
}

static uint64_t pwm_now_subus(void)
{
  return (uint64_t)nsi_hws_get_time() * PWM_SUBUS_PER_US;
}

static uint64_t pwm_task_time_subus(struct pwm_status *this)
{
  return this->in_event ? this->event_time_subus : pwm_now_subus();
}

static uint64_t pwm_period_subus(uint inst, uint32_t countertop)
{
  uint32_t prescaler = NRF_PWM_regs[inst].PRESCALER & PWM_PRESCALER_PRESCALER_Msk;
  uint64_t ticks = (uint64_t)countertop << prescaler;

  if (NRF_PWM_regs[inst].MODE & PWM_MODE_UPDOWN_Msk) {
    ticks *= 2;
  }
  return ticks * PWM_BASE_TICK_SUBUS;
}

static uint64_t pwm_tick_subus(uint inst)
{
  return PWM_BASE_TICK_SUBUS <<
         (NRF_PWM_regs[inst].PRESCALER & PWM_PRESCALER_PRESCALER_Msk);
}

static uint pwm_sequence_stride(uint inst)
{
  switch (NRF_PWM_regs[inst].DECODER & PWM_DECODER_LOAD_Msk) {
  case PWM_DECODER_LOAD_Common:
    return 1;
  case PWM_DECODER_LOAD_Grouped:
    return 2;
  case PWM_DECODER_LOAD_Individual:
  case PWM_DECODER_LOAD_WaveForm:
    return 4;
  default:
    bs_trace_error_time_line("PWM%u has invalid decoder LOAD mode %u\n",
                             inst,
                             (unsigned int)(NRF_PWM_regs[inst].DECODER &
                                            PWM_DECODER_LOAD_Msk));
    return 1;
  }
}

static const uint16_t *pwm_sequence_ptr(uint inst, uint seq)
{
#if NHW_HAS_DPPI
  return (const uint16_t *)(uintptr_t)NRF_PWM_regs[inst].DMA.SEQ[seq].PTR;
#else
  return (const uint16_t *)(uintptr_t)NRF_PWM_regs[inst].SEQ[seq].PTR;
#endif
}

/* Number of 16 bit values in the sequence */
static uint32_t pwm_sequence_length(uint inst, uint seq)
{
#if NHW_HAS_DPPI
  /* MAXCNT is in bytes */
  return (NRF_PWM_regs[inst].DMA.SEQ[seq].MAXCNT &
          PWM_DMA_SEQ_MAXCNT_MAXCNT_Msk) / 2;
#else
  return NRF_PWM_regs[inst].SEQ[seq].CNT & PWM_SEQUENCE_MASK;
#endif
}

static uint32_t pwm_sequence_step_count(uint inst, uint seq)
{
  uint32_t count = pwm_sequence_length(inst, seq);
  uint stride = pwm_sequence_stride(inst);

  if (count % stride != 0) {
    bs_trace_warning_time_line("PWM%u sequence %u CNT=%u is not a multiple of decoder stride %u\n",
                               inst, seq, (unsigned int)count, stride);
  }
  return count / stride;
}

static uint16_t pwm_sequence_value(uint inst, uint channel)
{
  NRF_PWM_Type *regs = &NRF_PWM_regs[inst];
  uint32_t load = regs->DECODER & PWM_DECODER_LOAD_Msk;
  const uint16_t *values = pwm_sequence_ptr(inst, nhw_pwm_st[inst].sequence);
  uint stride = pwm_sequence_stride(inst);
  uint offset;

  if (values == NULL) {
    bs_trace_error_time_line("PWM%u sequence has a null PTR\n", inst);
    return 0;
  }
  switch (load) {
  case PWM_DECODER_LOAD_Common:
    offset = 0;
    break;
  case PWM_DECODER_LOAD_Grouped:
    offset = channel / 2;
    break;
  case PWM_DECODER_LOAD_Individual:
  case PWM_DECODER_LOAD_WaveForm:
    offset = channel;
    break;
  default:
    return 0;
  }
  return values[nhw_pwm_st[inst].sample * stride + offset];
}

static void pwm_output_set_pin(uint inst, uint channel, bool value)
{
  struct pwm_status *this = &nhw_pwm_st[inst];
  NRF_PWM_Type *regs = &NRF_PWM_regs[inst];
  uint32_t psel = regs->PSEL.OUT[channel];
  bool connected = !(psel & PWM_PSEL_OUT_CONNECT_Msk);
  uint port = (psel & PWM_PSEL_OUT_PORT_Msk) >> PWM_PSEL_OUT_PORT_Pos;
  uint pin = (psel & PWM_PSEL_OUT_PIN_Msk) >> PWM_PSEL_OUT_PIN_Pos;
  struct pwm_output *output = &this->output[channel];

  if (output->owned &&
      (!connected || output->port != port || output->pin != pin)) {
    nrf_gpio_peri_pin_control(output->port, output->pin,
                              0, -1, 0, NULL, NULL, -1);
    output->owned = false;
    output->next_edge_subus = PWM_NO_EDGE;
  }
  if (!connected) {
    return;
  }
#ifdef NHW_PWM_GPIO_PORT
  static const uint pwm_port[NHW_PWM_TOTAL_INST] = NHW_PWM_GPIO_PORT;
  if (port != pwm_port[inst]) {
    bs_trace_error_time_line("PWM%u channel %u selects GPIO port %u, but only pins of "
                             "port %u (same domain as the PWM) can be used (PSEL.OUT=0x%08x)\n",
                             inst, channel, port, pwm_port[inst], (unsigned int)psel);
    return;
  }
#endif
  if (port >= NHW_GPIO_TOTAL_INST ||
      pin >= nrf_gpio_get_number_pins_in_port((int)port)) {
    bs_trace_error_time_line("PWM%u channel %u selects invalid GPIO PSEL.OUT=0x%08x\n",
                             inst, channel, (unsigned int)psel);
    return;
  }
  if (!output->owned) {
    output->owned = true;
    output->port = (uint8_t)port;
    output->pin = (uint8_t)pin;
    nrf_gpio_peri_pin_control(port, pin, 1, -1, 3, NULL, NULL, value);
  } else {
    nrf_gpio_peri_change_output(port, pin, value);
  }
}

static void pwm_output_schedule_edge(uint inst, uint channel,
                                     uint64_t when, bool level,
                                     bool period_start)
{
  struct pwm_output *output = &nhw_pwm_st[inst].output[channel];
  output->next_edge_subus = when;
  output->edge_level = level;
  output->edge_is_period_start = period_start;
}

static void pwm_output_begin_period(uint inst, uint64_t start_subus)
{
  struct pwm_status *this = &nhw_pwm_st[inst];
  NRF_PWM_Type *regs = &NRF_PWM_regs[inst];
  uint64_t tick = pwm_tick_subus(inst);
  uint64_t period = pwm_period_subus(inst, this->countertop);
  bool updown = (regs->MODE & PWM_MODE_UPDOWN_Msk) != 0;

  for (uint channel = 0; channel < 4; channel++) {
    struct pwm_output *output = &this->output[channel];
    if (regs->PSEL.OUT[channel] & PWM_PSEL_OUT_CONNECT_Msk) {
      pwm_output_set_pin(inst, channel, false);
      continue;
    }
    uint16_t value = pwm_sequence_value(inst, channel);
    uint32_t compare = value & PWM_VALUE_MASK;
    /* Bit 15 set: the output starts high and falls at the compare match */
    bool active_level = (value & PWM_VALUE_POLARITY_Msk) != 0;
    uint32_t effective_compare = compare > this->countertop
                                     ? this->countertop
                                     : compare;

    output->active_level = active_level;
    output->compare = effective_compare;
    output->countertop = this->countertop;
    output->period_start_subus = start_subus;
    output->period_subus = period;
    output->next_edge_subus = PWM_NO_EDGE;

    bool initial_level;
    if (!updown) {
      initial_level = effective_compare != 0 ? active_level : !active_level;
    } else {
      /* Up-down: with bit 15 set the output is high while the counter is below COMPARE,
       * with bit 15 clear it is high while the counter is at or above COMPARE.
       * The counter is 0 at the start of the period */
      initial_level = active_level ? effective_compare != 0
                                   : effective_compare == 0;
    }
    pwm_output_set_pin(inst, channel, initial_level);
    if (!output->owned) {
      continue;
    }

    if (effective_compare == 0 ||
        effective_compare == this->countertop) {
      continue;
    }
    pwm_output_schedule_edge(inst, channel,
                             start_subus + effective_compare * tick,
                             !active_level, false);
  }
}

static bool pwm_output_idle_level(uint inst, uint channel)
{
#if NHW_HAS_DPPI
  return (NRF_PWM_regs[inst].IDLEOUT >> channel) & 1;
#else
  return !nhw_pwm_st[inst].output[channel].active_level;
#endif
}

static void pwm_output_stop(uint inst)
{
  struct pwm_status *this = &nhw_pwm_st[inst];
  for (uint channel = 0; channel < 4; channel++) {
    struct pwm_output *output = &this->output[channel];
    output->next_edge_subus = PWM_NO_EDGE;
    pwm_output_set_pin(inst, channel, pwm_output_idle_level(inst, channel));
  }
}

static uint64_t pwm_output_next_edge(uint inst)
{
  uint64_t next = PWM_NO_EDGE;
  for (uint channel = 0; channel < 4; channel++) {
    uint64_t edge = nhw_pwm_st[inst].output[channel].next_edge_subus;
    if (edge < next) {
      next = edge;
    }
  }
  return next;
}

static void pwm_output_handle_edge(uint inst, uint channel)
{
  struct pwm_status *this = &nhw_pwm_st[inst];
  struct pwm_output *output = &this->output[channel];
  uint64_t edge_time = output->next_edge_subus;
  bool updown = (NRF_PWM_regs[inst].MODE & PWM_MODE_UPDOWN_Msk) != 0;
#if NHW_HAS_DPPI
  bool was_period_start = output->edge_is_period_start;
#endif

  pwm_output_set_pin(inst, channel, output->edge_level);
  if (!output->owned) {
    output->next_edge_subus = PWM_NO_EDGE;
    return;
  }
  output->period_start_subus = output->edge_is_period_start
                                   ? edge_time
                                   : output->period_start_subus;

  if (!updown) {
    if (output->edge_is_period_start) {
      pwm_output_schedule_edge(inst, channel,
                               edge_time + output->compare * pwm_tick_subus(inst),
                               !output->active_level, false);
    } else {
      pwm_output_schedule_edge(inst, channel,
                               output->period_start_subus +
                                   output->period_subus,
                               output->active_level, true);
    }
  } else if (output->edge_level != output->active_level) {
    /* The edge on the way up has been applied, next is the one on the way down */
    pwm_output_schedule_edge(inst, channel,
                             output->period_start_subus +
                                 (2 * output->countertop - output->compare) *
                                     pwm_tick_subus(inst),
                             output->active_level, false);
  } else {
    output->period_start_subus += output->period_subus;
    pwm_output_schedule_edge(inst, channel,
                             output->period_start_subus +
                                 output->compare * pwm_tick_subus(inst),
                             !output->active_level, false);
  }
#if NHW_HAS_DPPI
  if (!was_period_start) {
    nhw_pwm_signal_event(inst, PWM_EV_COMPAREMATCH0 + channel);
  }
#endif
}

static uint64_t pwm_inst_next_period_event_subus(uint inst)
{
  struct pwm_status *this = &nhw_pwm_st[inst];

  if (!this->running) {
    return UINT64_MAX;
  }
  if (!this->sequence_active && !this->stop_pending) {
    return UINT64_MAX;
  }
  return this->next_period_subus;
}

static uint64_t pwm_inst_next_event_subus(uint inst)
{
  uint64_t period_event = pwm_inst_next_period_event_subus(inst);
  uint64_t edge_event = pwm_output_next_edge(inst);

  return period_event < edge_event ? period_event : edge_event;
}

static void nhw_pwm_update_master_timer(void)
{
  PWM_timer = TIME_NEVER;
  for (uint inst = 0; inst < NHW_PWM_TOTAL_INST; inst++) {
    bs_time_t when = pwm_subus_to_time(pwm_inst_next_event_subus(inst));
    if (when < PWM_timer) {
      PWM_timer = when;
    }
  }
  nsi_hws_find_next_event();
}

static volatile uint32_t *pwm_event_reg(NRF_PWM_Type *regs, uint event)
{
  switch (event) {
  case PWM_EV_STOPPED: return &regs->EVENTS_STOPPED;
  case PWM_EV_SEQSTARTED0: return &regs->EVENTS_SEQSTARTED[0];
  case PWM_EV_SEQSTARTED1: return &regs->EVENTS_SEQSTARTED[1];
  case PWM_EV_SEQEND0: return &regs->EVENTS_SEQEND[0];
  case PWM_EV_SEQEND1: return &regs->EVENTS_SEQEND[1];
  case PWM_EV_PWMPERIODEND: return &regs->EVENTS_PWMPERIODEND;
  case PWM_EV_LOOPSDONE: return &regs->EVENTS_LOOPSDONE;
#if NHW_HAS_DPPI
  case PWM_EV_DMA_SEQ0_END: return &regs->EVENTS_DMA.SEQ[0].END;
  case PWM_EV_DMA_SEQ0_READY: return &regs->EVENTS_DMA.SEQ[0].READY;
  case PWM_EV_DMA_SEQ1_END: return &regs->EVENTS_DMA.SEQ[1].END;
  case PWM_EV_DMA_SEQ1_READY: return &regs->EVENTS_DMA.SEQ[1].READY;
#endif
  default:
#if NHW_HAS_DPPI
    if (event >= PWM_EV_COMPAREMATCH0 && event <= PWM_EV_COMPAREMATCH3) {
      return &regs->EVENTS_COMPAREMATCH[event - PWM_EV_COMPAREMATCH0];
    }
#endif
    bs_trace_error_line_time("Unknown PWM event %u\n", event);
    return NULL;
  }
}

#if NHW_HAS_DPPI
static uint32_t pwm_publish_reg(NRF_PWM_Type *regs, uint event)
{
  switch (event) {
  case PWM_EV_STOPPED: return regs->PUBLISH_STOPPED;
  case PWM_EV_SEQSTARTED0: return regs->PUBLISH_SEQSTARTED[0];
  case PWM_EV_SEQSTARTED1: return regs->PUBLISH_SEQSTARTED[1];
  case PWM_EV_SEQEND0: return regs->PUBLISH_SEQEND[0];
  case PWM_EV_SEQEND1: return regs->PUBLISH_SEQEND[1];
  case PWM_EV_PWMPERIODEND: return regs->PUBLISH_PWMPERIODEND;
  case PWM_EV_LOOPSDONE: return regs->PUBLISH_LOOPSDONE;
  case PWM_EV_DMA_SEQ0_END: return regs->PUBLISH_DMA.SEQ[0].END;
  case PWM_EV_DMA_SEQ0_READY: return regs->PUBLISH_DMA.SEQ[0].READY;
  case PWM_EV_DMA_SEQ1_END: return regs->PUBLISH_DMA.SEQ[1].END;
  case PWM_EV_DMA_SEQ1_READY: return regs->PUBLISH_DMA.SEQ[1].READY;
  default:
    if (event >= PWM_EV_COMPAREMATCH0 && event <= PWM_EV_COMPAREMATCH3) {
      return regs->PUBLISH_COMPAREMATCH[event - PWM_EV_COMPAREMATCH0];
    }
    return 0;
  }
}
#endif

static void nhw_pwm_eval_interrupt(uint inst)
{
  static struct nhw_irq_mapping irq_map[NHW_PWM_TOTAL_INST] = NHW_PWM_INT_MAP;
  static bool irq_line[NHW_PWM_TOTAL_INST];
  NRF_PWM_Type *regs = &NRF_PWM_regs[inst];
  uint32_t pending = 0;

  for (uint event = 0; event < PWM_EV_COUNT; event++) {
    if (*pwm_event_reg(regs, event)) {
      pending |= pwm_event_inten_mask[event];
    }
  }
  pending &= nhw_pwm_st[inst].INTEN;
#if NHW_HAS_DPPI
  regs->INTPEND = pending;
#endif

  hw_irq_ctrl_toggle_level_irq_line_if(&irq_line[inst], pending != 0, &irq_map[inst]);
}

static void nhw_pwm_signal_event(uint inst, uint event)
{
  NRF_PWM_Type *regs = &NRF_PWM_regs[inst];
  volatile uint32_t *event_reg = pwm_event_reg(regs, event);
  bool was_set = *event_reg != 0;

  *event_reg = 1;
  if (!was_set && (nhw_pwm_st[inst].INTEN & pwm_event_inten_mask[event])) {
    nhw_pwm_eval_interrupt(inst);
  }
#if NHW_HAS_PPI
  static const ppi_event_types_t event_base[NHW_PWM_TOTAL_INST] = {
    PWM0_EVENTS_STOPPED, PWM1_EVENTS_STOPPED, PWM2_EVENTS_STOPPED,
    PWM3_EVENTS_STOPPED
  };
  nrf_ppi_event(event_base[inst] + event);
#elif NHW_HAS_DPPI
  static const uint dppi_map[NHW_PWM_TOTAL_INST] = NHW_PWM_DPPI_MAP;
  nhw_dppi_event_signal_if(dppi_map[inst], pwm_publish_reg(regs, event));
#endif
}

static uint32_t nhw_pwm_sequence_top(uint inst, uint seq, uint sample)
{
  NRF_PWM_Type *regs = &NRF_PWM_regs[inst];
  if ((regs->DECODER & PWM_DECODER_LOAD_Msk) == PWM_DECODER_LOAD_WaveForm) {
    const uint16_t *values = pwm_sequence_ptr(inst, seq);
    if (values == NULL) {
      bs_trace_error_time_line("PWM%u waveform sequence has a null PTR\n", inst);
      return 0;
    }
    return values[sample * 4 + 3] & PWM_COUNTERTOP_COUNTERTOP_Msk;
  }
  return regs->COUNTERTOP & PWM_COUNTERTOP_COUNTERTOP_Msk;
}

static void nhw_pwm_stop_now(uint inst)
{
  struct pwm_status *this = &nhw_pwm_st[inst];
  if (!this->running) {
    return;
  }
  this->running = false;
  this->sequence_active = false;
  this->stop_pending = false;
  this->next_period_subus = UINT64_MAX;
  this->task_generation++;
  pwm_output_stop(inst);
  nhw_pwm_signal_event(inst, 0);
  nhw_pwm_update_master_timer();
}

static bool nhw_pwm_start_following_sequence(uint inst, uint seq)
{
  struct pwm_status *this = &nhw_pwm_st[inst];
  uint32_t loops_left = this->loops_left;
  uint32_t generation = this->task_generation;

  nhw_pwm_TASK_SEQSTART(inst, seq);
  if (this->task_generation == generation + 1) {
    this->loops_left = loops_left;
  }
  return this->task_generation != generation;
}

static bool nhw_pwm_sequence_complete(uint inst)
{
  struct pwm_status *this = &nhw_pwm_st[inst];
  NRF_PWM_Type *regs = &NRF_PWM_regs[inst];
  uint32_t next_count = pwm_sequence_step_count(inst, 1);

  this->sequence_active = false;
  if (this->sequence == 0 && next_count != 0) {
    return nhw_pwm_start_following_sequence(inst, 1);
  }

  if (this->loops_left > 1) {
    uint seq = pwm_sequence_step_count(inst, 0) ? 0 : 1;
    this->loops_left--;
    return nhw_pwm_start_following_sequence(inst, seq);
  }

  uint32_t generation = this->task_generation;
  nhw_pwm_signal_event(inst, 6);
  if (this->task_generation != generation) {
    return true;
  }
  if (regs->SHORTS & PWM_SHORTS_LOOPSDONE_STOP_Msk) {
    nhw_pwm_stop_now(inst);
    return true;
  }
  if (regs->SHORTS & PWM_SHORT_LOOPSDONE_SEQSTART(0)) {
    return nhw_pwm_start_following_sequence(inst, 0);
  }
  if (regs->SHORTS & PWM_SHORT_LOOPSDONE_SEQSTART(1)) {
    return nhw_pwm_start_following_sequence(inst, 1);
  }
  /* Without shortcuts the outputs keep generating the last loaded values until STOP */
  return false;
}

static void nhw_pwm_period_end(uint inst)
{
  struct pwm_status *this = &nhw_pwm_st[inst];
  NRF_PWM_Type *regs = &NRF_PWM_regs[inst];
  uint32_t generation = this->task_generation;

  this->at_period_end = true;
  nhw_pwm_signal_event(inst, 5);

  if (!this->running) {
    this->at_period_end = false;
    return;
  }
  if (this->task_generation != generation) {
    this->at_period_end = false;
    return;
  }
  if (this->stop_pending) {
    nhw_pwm_stop_now(inst);
    this->at_period_end = false;
    return;
  }

  if (!this->sequence_active) {
    this->next_period_subus += pwm_period_subus(inst, this->countertop);
    this->at_period_end = false;
    return;
  }

  if (this->enddelay_left != 0) {
    this->enddelay_left--;
    if (this->enddelay_left == 0) {
      if (nhw_pwm_sequence_complete(inst)) {
        this->at_period_end = false;
        return;
      }
    }
    if (this->running) {
      this->next_period_subus += pwm_period_subus(inst, this->countertop);
      if (this->sequence_active) {
        pwm_output_begin_period(inst, this->event_time_subus);
      }
    }
    this->at_period_end = false;
    return;
  }

  if (this->periods_to_step > 1) {
    this->periods_to_step--;
    this->next_period_subus += pwm_period_subus(inst, this->countertop);
    this->at_period_end = false;
    return;
  }

  uint32_t step_count = pwm_sequence_step_count(inst, this->sequence);
  if (this->sample + 1 < step_count) {
    this->sample++;
    this->countertop = nhw_pwm_sequence_top(inst, this->sequence, this->sample);
    if (this->countertop == 0) {
      bs_trace_warning_time_line("PWM%u waveform sequence produced COUNTERTOP=0\n", inst);
      this->countertop = 1;
    }
    this->periods_to_step = (regs->DECODER & PWM_DECODER_MODE_Msk)
                            ? UINT32_MAX
                            : (regs->SEQ[this->sequence].REFRESH & 0xFFFFFFU) + 1;
    if (this->sample + 1 == step_count) {
      nhw_pwm_signal_event(inst, 3 + this->sequence);
      if (this->task_generation != generation) {
        this->at_period_end = false;
        return;
      }
      if (regs->SHORTS & (PWM_SHORTS_SEQEND0_STOP_Msk << this->sequence)) {
        this->stop_pending = true;
      }
    }
  } else {
    this->enddelay_left = (regs->DECODER & PWM_DECODER_MODE_Msk)
                              ? 0
                              : regs->SEQ[this->sequence].ENDDELAY;
    if (this->enddelay_left == 0 &&
        nhw_pwm_sequence_complete(inst)) {
      this->at_period_end = false;
      return;
    }
  }

  if (this->running) {
    this->next_period_subus += pwm_period_subus(inst, this->countertop);
    if (this->sequence_active) {
      pwm_output_begin_period(inst, this->event_time_subus);
    }
  }

  this->at_period_end = false;
}

static void nhw_pwm_init(void)
{
  memset(NRF_PWM_regs, 0, sizeof(NRF_PWM_regs));
  memset(nhw_pwm_st, 0, sizeof(nhw_pwm_st));
  for (uint inst = 0; inst < NHW_PWM_TOTAL_INST; inst++) {
    nhw_pwm_st[inst].next_period_subus = UINT64_MAX;
    for (uint channel = 0; channel < 4; channel++) {
      NRF_PWM_regs[inst].PSEL.OUT[channel] = PWM_PSEL_OUT_CONNECT_Msk;
      nhw_pwm_st[inst].output[channel].next_edge_subus = PWM_NO_EDGE;
    }
  }
  PWM_timer = TIME_NEVER;
}

NSI_TASK(nhw_pwm_init, HW_INIT, 100);

static void nhw_pwm_timer_triggered(void)
{
  uint64_t now = pwm_now_subus();

  for (uint inst = 0; inst < NHW_PWM_TOTAL_INST; inst++) {
    struct pwm_status *this = &nhw_pwm_st[inst];
    while (this->running && pwm_inst_next_event_subus(inst) <= now) {
      uint64_t edge_time = pwm_output_next_edge(inst);
      uint64_t period_time = pwm_inst_next_period_event_subus(inst);

      /* At equal times the period end goes first, as it may stop the PWM or reload the
       * outputs, in which case the pending period start edge must not be applied */
      if (edge_time < period_time && edge_time <= now) {
        this->event_time_subus = edge_time;
        this->in_event = true;
        for (uint channel = 0; channel < 4; channel++) {
          if (this->output[channel].next_edge_subus == edge_time) {
            pwm_output_handle_edge(inst, channel);
          }
        }
        this->in_event = false;
        continue;
      }

      if (period_time > now) {
        break;
      }
      uint64_t period = pwm_period_subus(inst, this->countertop);
      if (period == 0 || UINT64_MAX - this->next_period_subus < period) {
        bs_trace_error_time_line("PWM%u period timing overflow or invalid countertop\n", inst);
        break;
      }
      this->event_time_subus = this->next_period_subus;
      this->in_event = true;
      nhw_pwm_period_end(inst);
      this->in_event = false;
    }
  }
  nhw_pwm_update_master_timer();
}

NSI_HW_EVENT(PWM_timer, nhw_pwm_timer_triggered, 50);

void nhw_pwm_TASK_STOP(uint inst)
{
  struct pwm_status *this = &nhw_pwm_st[inst];
  if (!this->running) {
    nhw_pwm_signal_event(inst, 0);
  } else if (!this->sequence_active) {
    nhw_pwm_stop_now(inst);
  } else if (this->at_period_end) {
    nhw_pwm_stop_now(inst);
  } else {
    this->task_generation++;
    this->stop_pending = true;
    uint64_t now = pwm_now_subus();
    uint64_t period = pwm_period_subus(inst, this->countertop);
    if (this->next_period_subus <= now) {
      this->next_period_subus +=
          ((now - this->next_period_subus) / period + 1) * period;
    }
    nhw_pwm_update_master_timer();
  }
}

void nhw_pwm_TASK_SEQSTART(uint inst, uint seq)
{
  struct pwm_status *this = &nhw_pwm_st[inst];
  NRF_PWM_Type *regs = &NRF_PWM_regs[inst];
  uint32_t count;

  if (seq >= 2 || !(regs->ENABLE & PWM_ENABLE_ENABLE_Enabled)) {
    return;
  }
  count = pwm_sequence_step_count(inst, seq);
  if (count == 0) {
    return;
  }

  this->running = true;
  this->sequence_active = true;
  this->stop_pending = false;
  this->sequence = seq;
  this->sample = 0;
  this->periods_to_step = (regs->DECODER & PWM_DECODER_MODE_Msk)
                          ? UINT32_MAX
                          : (regs->SEQ[seq].REFRESH & 0xFFFFFFU) + 1;
  this->loops_left = (regs->LOOP & PWM_LOOP_CNT_Msk);
  if (this->loops_left == 0) {
    this->loops_left = 1;
  }
  this->enddelay_left = 0;
  this->countertop = nhw_pwm_sequence_top(inst, seq, 0);
  if (this->countertop == 0) {
    bs_trace_warning_time_line("PWM%u started with COUNTERTOP=0; no period can be generated\n", inst);
    this->running = false;
    this->sequence_active = false;
    return;
  }

  this->next_period_subus = pwm_task_time_subus(this) + pwm_period_subus(inst, this->countertop);
  this->task_generation++;
  pwm_output_begin_period(inst, pwm_task_time_subus(this));
  uint32_t generation = this->task_generation;
#if NHW_HAS_DPPI
  /* The sequence values are fetched by DMA before the sequence starts */
  regs->DMA.SEQ[seq].AMOUNT = regs->DMA.SEQ[seq].MAXCNT;
  regs->DMA.SEQ[seq].CURRENTAMOUNT = regs->DMA.SEQ[seq].MAXCNT;
  nhw_pwm_signal_event(inst, PWM_EV_DMA_SEQ0_READY + 2 * seq);
  if (this->task_generation != generation) {
    return;
  }
  nhw_pwm_signal_event(inst, PWM_EV_DMA_SEQ0_END + 2 * seq);
  if (!this->running || this->task_generation != generation) {
    return;
  }
#endif
  nhw_pwm_signal_event(inst, 1 + seq);
  if (!this->running || this->task_generation != generation) {
    return;
  }
  if (count == 1) {
    nhw_pwm_signal_event(inst, 3 + seq);
    if (!this->running || this->task_generation != generation) {
      return;
    }
    if (regs->SHORTS & (PWM_SHORTS_SEQEND0_STOP_Msk << seq)) {
      this->stop_pending = true;
    }
  }
  nhw_pwm_update_master_timer();
}

void nhw_pwm_TASK_NEXTSTEP(uint inst)
{
  struct pwm_status *this = &nhw_pwm_st[inst];
  NRF_PWM_Type *regs = &NRF_PWM_regs[inst];
  uint32_t count;
  if (!this->running || !this->sequence_active ||
      !(regs->DECODER & PWM_DECODER_MODE_Msk)) {
    return;
  }

  count = pwm_sequence_step_count(inst, this->sequence);
  if (this->sample + 1 < count) {
    this->sample++;
    this->countertop = nhw_pwm_sequence_top(inst, this->sequence, this->sample);
    if (this->countertop == 0) {
      bs_trace_warning_time_line("PWM%u waveform sequence produced COUNTERTOP=0\n", inst);
      this->countertop = 1;
    }
    this->task_generation++;
    uint32_t generation = this->task_generation;
    this->periods_to_step = 1;
    pwm_output_begin_period(inst, pwm_task_time_subus(this));
    if (this->sample + 1 == count) {
      nhw_pwm_signal_event(inst, 3 + this->sequence);
      if (!this->running || this->task_generation != generation) {
        return;
      }
      if (regs->SHORTS & (PWM_SHORTS_SEQEND0_STOP_Msk << this->sequence)) {
        this->stop_pending = true;
      }
    }
    if (!this->at_period_end) {
      uint64_t now = pwm_now_subus();
      uint64_t period = pwm_period_subus(inst, this->countertop);
      if (this->next_period_subus <= now) {
        this->next_period_subus +=
            ((now - this->next_period_subus) / period + 1) * period;
      }
    } else {
      this->next_period_subus = pwm_task_time_subus(this) +
                                pwm_period_subus(inst, this->countertop);
    }
    nhw_pwm_update_master_timer();
  }
}

void nhw_pwm_regw_sideeffects_TASKS_STOP(uint inst)
{
  if (NRF_PWM_regs[inst].TASKS_STOP) {
    NRF_PWM_regs[inst].TASKS_STOP = 0;
    nhw_pwm_TASK_STOP(inst);
  }
}

void nhw_pwm_regw_sideeffects_TASKS_SEQSTART(uint inst, uint seq)
{
#if NHW_HAS_DPPI
  volatile uint32_t *task_reg = &NRF_PWM_regs[inst].TASKS_DMA.SEQ[seq].START;
#else
  volatile uint32_t *task_reg = &NRF_PWM_regs[inst].TASKS_SEQSTART[seq];
#endif
  if (seq < 2 && *task_reg) {
    *task_reg = 0;
    nhw_pwm_TASK_SEQSTART(inst, seq);
  }
}

void nhw_pwm_regw_sideeffects_TASKS_NEXTSTEP(uint inst)
{
  if (NRF_PWM_regs[inst].TASKS_NEXTSTEP) {
    NRF_PWM_regs[inst].TASKS_NEXTSTEP = 0;
    nhw_pwm_TASK_NEXTSTEP(inst);
  }
}

void nhw_pwm_regw_sideeffects_INTENSET(uint inst)
{
  NRF_PWM_Type *regs = &NRF_PWM_regs[inst];
  if (regs->INTENSET) {
    nhw_pwm_st[inst].INTEN |= regs->INTENSET & PWM_EVENT_MASK;
    regs->INTENSET = nhw_pwm_st[inst].INTEN;
    nhw_pwm_eval_interrupt(inst);
  }
}

void nhw_pwm_regw_sideeffects_INTENCLR(uint inst)
{
  NRF_PWM_Type *regs = &NRF_PWM_regs[inst];
  if (regs->INTENCLR) {
    nhw_pwm_st[inst].INTEN &= ~(regs->INTENCLR & PWM_EVENT_MASK);
    regs->INTENSET = nhw_pwm_st[inst].INTEN;
    regs->INTENCLR = 0;
    nhw_pwm_eval_interrupt(inst);
  }
}

void nhw_pwm_regw_sideeffects_INTEN(uint inst)
{
  NRF_PWM_regs[inst].INTENSET = nhw_pwm_st[inst].INTEN =
      NRF_PWM_regs[inst].INTEN & PWM_EVENT_MASK;
  nhw_pwm_eval_interrupt(inst);
}

void nhw_pwm_regw_sideeffects_EVENTS_all(uint inst)
{
  nhw_pwm_eval_interrupt(inst);
  nhw_pwm_update_master_timer();
}

#if NHW_HAS_DPPI
void nhw_pwm_regw_sideeffects_ENABLE(uint inst)
{
  struct pwm_status *this = &nhw_pwm_st[inst];
  bool enabled = (NRF_PWM_regs[inst].ENABLE & PWM_ENABLE_ENABLE_Msk) != 0;

  if (enabled) {
    if (this->running) {
      return;
    }
    /* The pins are taken over by the PWM, and driven to their idle level */
    for (uint channel = 0; channel < 4; channel++) {
      if (!(NRF_PWM_regs[inst].PSEL.OUT[channel] & PWM_PSEL_OUT_CONNECT_Msk)) {
        pwm_output_set_pin(inst, channel, pwm_output_idle_level(inst, channel));
      }
    }
    return;
  }

  /* Disabling the PWM drops any ongoing playback and releases the pins */
  this->running = false;
  this->sequence_active = false;
  this->stop_pending = false;
  this->next_period_subus = UINT64_MAX;
  this->task_generation++;
  for (uint channel = 0; channel < 4; channel++) {
    struct pwm_output *output = &this->output[channel];
    output->next_edge_subus = PWM_NO_EDGE;
    if (output->owned) {
      nrf_gpio_peri_pin_control(output->port, output->pin, 0, -1, 0, NULL, NULL, -1);
      output->owned = false;
    }
  }
  nhw_pwm_update_master_timer();
}

static struct nhw_subsc_mem pwm_subscribed_STOP[NHW_PWM_TOTAL_INST];
static struct nhw_subsc_mem pwm_subscribed_NEXTSTEP[NHW_PWM_TOTAL_INST];
static struct nhw_subsc_mem pwm_subscribed_SEQSTART[NHW_PWM_TOTAL_INST][2];
static const uint pwm_dppi_map[NHW_PWM_TOTAL_INST] = NHW_PWM_DPPI_MAP;

static void nhw_pwm_stop_wrap(void *param)
{
  nhw_pwm_TASK_STOP((uintptr_t)param);
}

static void nhw_pwm_nextstep_wrap(void *param)
{
  nhw_pwm_TASK_NEXTSTEP((uintptr_t)param);
}

static void nhw_pwm_seqstart_wrap(void *param)
{
  nhw_pwm_TASK_SEQSTART((uintptr_t)param >> 16, (uintptr_t)param & 0xFFFF);
}

void nhw_pwm_regw_sideeffects_SUBSCRIBE_STOP(uint inst)
{
  nhw_dppi_common_subscribe_sideeffect(pwm_dppi_map[inst],
                                       NRF_PWM_regs[inst].SUBSCRIBE_STOP,
                                       &pwm_subscribed_STOP[inst],
                                       nhw_pwm_stop_wrap, (void *)(uintptr_t)inst);
}

void nhw_pwm_regw_sideeffects_SUBSCRIBE_NEXTSTEP(uint inst)
{
  nhw_dppi_common_subscribe_sideeffect(pwm_dppi_map[inst],
                                       NRF_PWM_regs[inst].SUBSCRIBE_NEXTSTEP,
                                       &pwm_subscribed_NEXTSTEP[inst],
                                       nhw_pwm_nextstep_wrap, (void *)(uintptr_t)inst);
}

void nhw_pwm_regw_sideeffects_SUBSCRIBE_SEQSTART(uint inst, uint seq)
{
  nhw_dppi_common_subscribe_sideeffect(pwm_dppi_map[inst],
                                       NRF_PWM_regs[inst].SUBSCRIBE_DMA.SEQ[seq].START,
                                       &pwm_subscribed_SEQSTART[inst][seq],
                                       nhw_pwm_seqstart_wrap,
                                       (void *)(uintptr_t)((inst << 16) | seq));
}
#endif /* NHW_HAS_DPPI */

#if NHW_HAS_PPI
#define PWM_PPI_TASKS(inst) \
  void nhw_pwm##inst##_TASK_STOP(void) { nhw_pwm_TASK_STOP(inst); } \
  void nhw_pwm##inst##_TASK_SEQSTART_0(void) { nhw_pwm_TASK_SEQSTART(inst, 0); } \
  void nhw_pwm##inst##_TASK_SEQSTART_1(void) { nhw_pwm_TASK_SEQSTART(inst, 1); } \
  void nhw_pwm##inst##_TASK_NEXTSTEP(void) { nhw_pwm_TASK_NEXTSTEP(inst); }

PWM_PPI_TASKS(0)
PWM_PPI_TASKS(1)
PWM_PPI_TASKS(2)
PWM_PPI_TASKS(3)
#undef PWM_PPI_TASKS
#endif

#endif /* NRF52833 || NRF54L15 */
