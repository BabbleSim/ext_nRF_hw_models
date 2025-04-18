/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_radio.h"
#include "bs_tracing.h"
#include "NHW_RADIO.h"

void nrf_radio_task_trigger(NRF_RADIO_Type * p_reg, nrf_radio_task_t task)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;

#define CASE_CALL_SIDEEFFECT(x) \
  case NRF_RADIO_TASK_##x :\
    nhw_RADIO_regw_sideeffects_TASKS_##x();\
    break

  switch (task) {
    CASE_CALL_SIDEEFFECT(TXEN);
    CASE_CALL_SIDEEFFECT(RXEN);
    CASE_CALL_SIDEEFFECT(START);
    CASE_CALL_SIDEEFFECT(STOP);
    CASE_CALL_SIDEEFFECT(DISABLE);
    CASE_CALL_SIDEEFFECT(RSSISTART);
#if defined(RADIO_TASKS_RSSISTOP_TASKS_RSSISTOP_Msk)
    CASE_CALL_SIDEEFFECT(RSSISTOP);
#endif
    CASE_CALL_SIDEEFFECT(BCSTART);
    CASE_CALL_SIDEEFFECT(BCSTOP);
    CASE_CALL_SIDEEFFECT(EDSTART);
    CASE_CALL_SIDEEFFECT(EDSTOP);
    CASE_CALL_SIDEEFFECT(CCASTART);
    CASE_CALL_SIDEEFFECT(CCASTOP);
#if defined(RADIO_TASKS_SOFTRESET_TASKS_SOFTRESET_Msk)
    //To be enabled once NRF_RADIO_TASK_SOFTRESET is added to the HAL
    CASE_CALL_SIDEEFFECT(SOFTRESET);
#endif
    default:
      bs_trace_error_line_time("%s: Not supported task %i started\n", __func__, task);
      break;
  }
#undef CASE_CALL_SIDEEFFECT
}

void nrf_radio_int_enable(NRF_RADIO_Type * p_reg, uint32_t mask)
{
#if defined(RADIO_INTENSET_READY_Msk)
    p_reg->INTENSET = mask;
#elif defined(RADIO_INTENSET00_READY_Msk)
    p_reg->INTENSET00 = mask;
#endif
    nhw_RADIO_regw_sideeffects_INTENSET(0);
}

void nrf_radio_int_disable(NRF_RADIO_Type * p_reg, uint32_t mask)
{
#if defined(RADIO_INTENCLR_READY_Msk)
    p_reg->INTENCLR = mask;
#elif defined(RADIO_INTENCLR00_READY_Msk)
    p_reg->INTENCLR00 = mask;
#endif
    nhw_RADIO_regw_sideeffects_INTENCLR(0);
}

void nrf_radio_bcc_set(NRF_RADIO_Type * p_reg, uint32_t radio_bcc)
{
  p_reg->BCC = radio_bcc;
  nhw_RADIO_regw_sideeffects_BCC();
}

#if defined(RADIO_POWER_POWER_Msk)
void nrf_radio_power_set(NRF_RADIO_Type * p_reg, bool radio_power)
{
  p_reg->POWER = (uint32_t) radio_power;
  nhw_RADIO_regw_sideeffects_POWER();
}
#endif

void nrf_radio_event_clear(NRF_RADIO_Type * p_reg, nrf_radio_event_t event)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
  nhw_RADIO_regw_sideeffects_EVENTS_all(0);
}

#if defined(DPPI_PRESENT)

static void nrf_radio_subscribe_common(NRF_RADIO_Type * p_reg,
                                       nrf_radio_task_t task)
{
  (void) p_reg;

#define CASE_CALL_SIDEEFFECT(x) \
  case NRF_RADIO_TASK_##x :\
    nhw_RADIO_regw_sideeffects_SUBSCRIBE_##x(0);\
    break

  switch (task) {
    CASE_CALL_SIDEEFFECT(TXEN);
    CASE_CALL_SIDEEFFECT(RXEN);
    CASE_CALL_SIDEEFFECT(START);
    CASE_CALL_SIDEEFFECT(STOP);
    CASE_CALL_SIDEEFFECT(DISABLE);
    CASE_CALL_SIDEEFFECT(RSSISTART);
#if defined(RADIO_TASKS_RSSISTOP_TASKS_RSSISTOP_Msk)
    CASE_CALL_SIDEEFFECT(RSSISTOP);
#endif
    CASE_CALL_SIDEEFFECT(BCSTART);
    CASE_CALL_SIDEEFFECT(BCSTOP);
    CASE_CALL_SIDEEFFECT(EDSTART);
    CASE_CALL_SIDEEFFECT(EDSTOP);
    CASE_CALL_SIDEEFFECT(CCASTART);
    CASE_CALL_SIDEEFFECT(CCASTOP);
#if 0 /* defined(RADIO_TASKS_SOFTRESET_TASKS_SOFTRESET_Msk) */
    CASE_CALL_SIDEEFFECT(SOFTRESET); //TODO missing in HAL
#endif
    default:
      bs_trace_error_line_time("%s: Attempted to subscribe to a not-supported task in the nrf_radio (%i)\n",
                               __func__, task);
      break;
  }
#undef CASE_CALL_SIDEEFFECT
}

void nrf_radio_subscribe_set(NRF_RADIO_Type * p_reg,
                             nrf_radio_task_t task,
                             uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + NRF_RADIO_DPPI_OFFSET)) =
          ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
    nrf_radio_subscribe_common(p_reg, task);
}

void nrf_radio_subscribe_clear(NRF_RADIO_Type * p_reg,
                               nrf_radio_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + NRF_RADIO_DPPI_OFFSET)) = 0;
    nrf_radio_subscribe_common(p_reg, task);
}

#endif /* defined(DPPI_PRESENT) */
