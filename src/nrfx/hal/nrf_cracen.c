/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */

#include <stdint.h>
#include "hal/nrf_cracen.h"
#include "bs_tracing.h"
#include "NHW_CRACEN_wrap.h"

void nrf_cracen_int_enable(NRF_CRACEN_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;
  nhw_CRACEN_regw_sideeffects_INTENSET(0);
}

void nrf_cracen_int_disable(NRF_CRACEN_Type * p_reg, uint32_t mask)
{
  p_reg->INTENCLR = mask;
  nhw_CRACEN_regw_sideeffects_INTENCLR(0);
}

void nrf_cracen_event_clear(NRF_CRACEN_Type *  p_reg,
                            nrf_cracen_event_t event)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0;
  if (event == NRF_CRACEN_EVENT_CRYPTOMASTER) {
    nhw_CRACEN_regw_sideeffects_EVENTS_CRYPTOMASTER();
  } else if (event == NRF_CRACEN_EVENT_RNG) {
    nhw_CRACEN_regw_sideeffects_EVENTS_RNG();
  } else if (event == NRF_CRACEN_EVENT_PKE_IKG) {
    nhw_CRACEN_regw_sideeffects_EVENTS_PKEIKG();
  } else {
    bs_trace_error_time_line("Attempted to clear unknown event %i\n", event);
  }
}
