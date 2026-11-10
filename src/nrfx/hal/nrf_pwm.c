/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL.
 */

#include <stdint.h>
#include "NHW_PWM.h"
#include "hal/nrf_pwm.h"
#include "bs_tracing.h"

static uint pwm_number_from_ptr(NRF_PWM_Type const *p_reg)
{
  uintptr_t base = (uintptr_t)&NRF_PWM_regs[0];
  uintptr_t address = (uintptr_t)p_reg;
  if (address < base || address >= base + sizeof(NRF_PWM_regs) ||
      ((address - base) % sizeof(NRF_PWM_Type)) != 0) {
    bs_trace_error_line_time("Invalid PWM register block passed to the HAL\n");
    return NHW_PWM_TOTAL_INST;
  }
  return (uint)((address - base) / sizeof(NRF_PWM_Type));
}

void nrf_pwm_task_trigger(NRF_PWM_Type *p_reg, nrf_pwm_task_t task)
{
  uint inst = pwm_number_from_ptr(p_reg);
  if (inst >= NHW_PWM_TOTAL_INST) {
    return;
  }
  if (task != NRF_PWM_TASK_STOP && task != NRF_PWM_TASK_SEQSTART0 &&
      task != NRF_PWM_TASK_SEQSTART1 && task != NRF_PWM_TASK_NEXTSTEP) {
    bs_trace_error_line_time("Unsupported PWM task offset 0x%x\n", (unsigned int)task);
    return;
  }
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 1;
  if (task == NRF_PWM_TASK_STOP) {
    nhw_pwm_regw_sideeffects_TASKS_STOP(inst);
  } else if (task == NRF_PWM_TASK_SEQSTART0) {
    nhw_pwm_regw_sideeffects_TASKS_SEQSTART(inst, 0);
  } else if (task == NRF_PWM_TASK_SEQSTART1) {
    nhw_pwm_regw_sideeffects_TASKS_SEQSTART(inst, 1);
  } else if (task == NRF_PWM_TASK_NEXTSTEP) {
    nhw_pwm_regw_sideeffects_TASKS_NEXTSTEP(inst);
  }
}

void nrf_pwm_event_clear(NRF_PWM_Type *p_reg, nrf_pwm_event_t event)
{
  uint inst = pwm_number_from_ptr(p_reg);
  if (inst >= NHW_PWM_TOTAL_INST) {
    return;
  }
  if (event != NRF_PWM_EVENT_STOPPED && event != NRF_PWM_EVENT_SEQSTARTED0 &&
      event != NRF_PWM_EVENT_SEQSTARTED1 && event != NRF_PWM_EVENT_SEQEND0 &&
      event != NRF_PWM_EVENT_SEQEND1 && event != NRF_PWM_EVENT_PWMPERIODEND &&
      event != NRF_PWM_EVENT_LOOPSDONE) {
    bs_trace_error_line_time("Unsupported PWM event offset 0x%x\n", (unsigned int)event);
    return;
  }
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0;
  nhw_pwm_regw_sideeffects_EVENTS_all(inst);
}

#if NHW_HAS_DPPI

void nrf_pwm_enable(NRF_PWM_Type *p_reg)
{
  uint inst = pwm_number_from_ptr(p_reg);
  if (inst < NHW_PWM_TOTAL_INST) {
    p_reg->ENABLE = PWM_ENABLE_ENABLE_Enabled;
    nhw_pwm_regw_sideeffects_ENABLE(inst);
  }
}

void nrf_pwm_disable(NRF_PWM_Type *p_reg)
{
  uint inst = pwm_number_from_ptr(p_reg);
  if (inst < NHW_PWM_TOTAL_INST) {
    p_reg->ENABLE = 0;
    nhw_pwm_regw_sideeffects_ENABLE(inst);
  }
}

static void nrf_pwm_subscribe_common(NRF_PWM_Type *p_reg, nrf_pwm_task_t task)
{
  uint inst = pwm_number_from_ptr(p_reg);
  if (inst >= NHW_PWM_TOTAL_INST) {
    return;
  }
  if (task == NRF_PWM_TASK_STOP) {
    nhw_pwm_regw_sideeffects_SUBSCRIBE_STOP(inst);
  } else if (task == NRF_PWM_TASK_SEQSTART0) {
    nhw_pwm_regw_sideeffects_SUBSCRIBE_SEQSTART(inst, 0);
  } else if (task == NRF_PWM_TASK_SEQSTART1) {
    nhw_pwm_regw_sideeffects_SUBSCRIBE_SEQSTART(inst, 1);
  } else if (task == NRF_PWM_TASK_NEXTSTEP) {
    nhw_pwm_regw_sideeffects_SUBSCRIBE_NEXTSTEP(inst);
  } else {
    bs_trace_error_line_time("Unsupported PWM task offset 0x%x\n", (unsigned int)task);
  }
}

void nrf_pwm_subscribe_set(NRF_PWM_Type *p_reg, nrf_pwm_task_t task, uint8_t channel)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task + 0x80uL)) =
      ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
  nrf_pwm_subscribe_common(p_reg, task);
}

void nrf_pwm_subscribe_clear(NRF_PWM_Type *p_reg, nrf_pwm_task_t task)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task + 0x80uL)) = 0;
  nrf_pwm_subscribe_common(p_reg, task);
}

/* The model needs to know when the PWMPERIODEND event starts or stops being published */
void nrf_pwm_publish_set(NRF_PWM_Type *p_reg, nrf_pwm_event_t event, uint8_t channel)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event + 0x80uL)) =
      ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

void nrf_pwm_publish_clear(NRF_PWM_Type *p_reg, nrf_pwm_event_t event)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event + 0x80uL)) = 0;
}

#endif /* NHW_HAS_DPPI */

void nrf_pwm_int_enable(NRF_PWM_Type *p_reg, uint32_t mask)
{
  uint inst = pwm_number_from_ptr(p_reg);
  if (inst < NHW_PWM_TOTAL_INST) {
    p_reg->INTENSET = mask;
    nhw_pwm_regw_sideeffects_INTENSET(inst);
  }
}

void nrf_pwm_int_disable(NRF_PWM_Type *p_reg, uint32_t mask)
{
  uint inst = pwm_number_from_ptr(p_reg);
  if (inst < NHW_PWM_TOTAL_INST) {
    p_reg->INTENCLR = mask;
    nhw_pwm_regw_sideeffects_INTENCLR(inst);
  }
}

void nrf_pwm_int_set(NRF_PWM_Type *p_reg, uint32_t mask)
{
  uint inst = pwm_number_from_ptr(p_reg);
  if (inst < NHW_PWM_TOTAL_INST) {
    p_reg->INTEN = mask;
    nhw_pwm_regw_sideeffects_INTEN(inst);
  }
}
