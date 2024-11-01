/*
 * Copyright (c) 2015 - 2023, Nordic Semiconductor ASA
 *
 * Modifications to the GPIOTE HAL to use with simulation models
 *
 * Code which is not copied from the nRFx HAL is licensed as:
 * SPDX-License-Identifier: Apache-2.0
 *
 * Most of the code below is taken from the NRFx HAL with minor
 * modifications. For that code, the original license applies:
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include "NHW_config.h"
#include "NRF_GPIOTE.h"
#include "hal/nrf_gpiote.h"
#include "bs_tracing.h"

static int gpiote_number_from_ptr(NRF_GPIOTE_Type const * p_reg){
  int i = ( (int)p_reg - (int)&NRF_GPIOTE_regs[0] ) / sizeof(NRF_GPIOTE_Type);
  return i;
}

void nrf_gpiote_task_trigger(NRF_GPIOTE_Type * p_reg, nrf_gpiote_task_t task)
{
  uint32_t *reg = (uint32_t *)((uintptr_t)p_reg + task);
  *(volatile uint32_t *)reg = 0x1UL;

  uint inst = gpiote_number_from_ptr(p_reg);

  if ((reg >= &NRF_GPIOTE_regs[inst].TASKS_OUT[0])
      && (reg<= &NRF_GPIOTE_regs[inst].TASKS_OUT[NHW_GPIOTE_MAX_CHANNELS])) {
    nrf_gpiote_regw_sideeffects_TASKS_OUT(inst, reg - &NRF_GPIOTE_regs[inst].TASKS_OUT[0]);
  } else if ((reg >= &NRF_GPIOTE_regs[inst].TASKS_SET[0])
             && (reg<= &NRF_GPIOTE_regs[inst].TASKS_SET[NHW_GPIOTE_MAX_CHANNELS])) {
    nrf_gpiote_regw_sideeffects_TASKS_SET(inst, reg - &NRF_GPIOTE_regs[inst].TASKS_SET[0]);
  } else if ((reg >= &NRF_GPIOTE_regs[inst].TASKS_CLR[0])
             && (reg<= &NRF_GPIOTE_regs[inst].TASKS_CLR[NHW_GPIOTE_MAX_CHANNELS])) {
    nrf_gpiote_regw_sideeffects_TASKS_CLR(inst, reg - &NRF_GPIOTE_regs[inst].TASKS_CLR[0]);
  } else {
    bs_trace_error_time_line("%s: Unknown GPIOTE tasks %i\n",task); /* LCOV_EXCL_LINE */
  }
}

void nrf_gpiote_event_clear(NRF_GPIOTE_Type * p_reg, nrf_gpiote_event_t event)
{
  uint32_t *reg = (uint32_t *)((uintptr_t)p_reg + event);
  *(volatile uint32_t *)reg = 0;

  uint inst = gpiote_number_from_ptr(p_reg);

  if ((reg >= &NRF_GPIOTE_regs[inst].EVENTS_IN[0])
      && (reg <= &NRF_GPIOTE_regs[inst].EVENTS_IN[NHW_GPIOTE_MAX_CHANNELS])) {
    nrf_gpiote_regw_sideeffects_EVENTS_IN(inst, reg - &NRF_GPIOTE_regs[inst].EVENTS_IN[0]);
#if !NHW_GPIOTE_IS_54
  } else if (reg == &NRF_GPIOTE_regs[inst].EVENTS_PORT) {
    nrf_gpiote_regw_sideeffects_EVENTS_PORT(inst);
#else
  } else if ((reg == &NRF_GPIOTE_regs[inst].EVENTS_PORT[0].NONSECURE)
             || (reg == &NRF_GPIOTE_regs[inst].EVENTS_PORT[0].SECURE)) {
    nrf_gpiote_regw_sideeffects_EVENTS_PORT(inst);
#endif
  } else {
    bs_trace_error_time_line("%s: Unknown GPIOTE event %i\n",event); /* LCOV_EXCL_LINE */
  }
}

#if defined(GPIOTE_IRQ_GROUP)
#define DEFAULT_IRQ_LINE NRF_GPIOTE_IRQ_GROUP
#else
#define DEFAULT_IRQ_LINE 0
#endif

void nrf_gpiote_int_enable(NRF_GPIOTE_Type * p_reg, uint32_t mask)
{
    uint inst = gpiote_number_from_ptr(p_reg);
    p_reg->NRFX_CONCAT_2(INTENSET, NRF_GPIOTE_IRQ_GROUP) = mask;
    nrf_gpiote_regw_sideeffects_INTENSET(inst, DEFAULT_IRQ_LINE);

}

void nrf_gpiote_int_disable(NRF_GPIOTE_Type * p_reg, uint32_t mask)
{
    uint inst = gpiote_number_from_ptr(p_reg);
    p_reg->NRFX_CONCAT_2(INTENCLR, NRF_GPIOTE_IRQ_GROUP) = mask;
    nrf_gpiote_regw_sideeffects_INTENCLR(inst, DEFAULT_IRQ_LINE);
}

void nrf_gpiote_event_enable(NRF_GPIOTE_Type * p_reg, uint32_t idx)
{
   uint inst = gpiote_number_from_ptr(p_reg);
   p_reg->CONFIG[idx] |= GPIOTE_CONFIG_MODE_Event;
   nrf_gpiote_regw_sideeffects_CONFIG(inst, idx);
}

void nrf_gpiote_event_disable(NRF_GPIOTE_Type * p_reg, uint32_t idx)
{
   uint inst = gpiote_number_from_ptr(p_reg);
   p_reg->CONFIG[idx] &= ~GPIOTE_CONFIG_MODE_Msk;
   nrf_gpiote_regw_sideeffects_CONFIG(inst, idx);
}

#if NRF_GPIOTE_HAS_INT_GROUPS
void nrf_gpiote_int_group_enable(NRF_GPIOTE_Type * p_reg,
                                 uint8_t           group_idx,
                                 uint32_t          mask)
{
    switch (group_idx)
    {
        case 0:
            p_reg->INTENSET0 = mask;
            break;
        case 1:
            p_reg->INTENSET1 = mask;
            break;
#if defined(GPIOTE_INTENSET2_IN0_Msk)
        case 2:
            p_reg->INTENSET2 = mask;
            break;
#endif
#if defined(GPIOTE_INTENSET3_IN0_Msk)
        case 3:
            p_reg->INTENSET3 = mask;
            break;
#endif
#if defined(GPIOTE_INTENSET4_IN0_Msk)
        case 4:
            p_reg->INTENSET4 = mask;
            break;
#endif
#if defined(GPIOTE_INTENSET5_IN0_Msk)
        case 5:
            p_reg->INTENSET5 = mask;
            break;
#endif
#if defined(GPIOTE_INTENSET6_IN0_Msk)
        case 6:
            p_reg->INTENSET6 = mask;
            break;
#endif
       default:
            NRFX_ASSERT(false);
            break;
    }
    uint inst = gpiote_number_from_ptr(p_reg);
    nrf_gpiote_regw_sideeffects_INTENSET(inst, group_idx);
}

void nrf_gpiote_int_group_disable(NRF_GPIOTE_Type * p_reg,
                                  uint8_t           group_idx,
                                  uint32_t          mask)
{
    switch (group_idx)
    {
        case 0:
            p_reg->INTENCLR0 = mask;
            break;
        case 1:
            p_reg->INTENCLR1 = mask;
            break;
#if defined(GPIOTE_INTENCLR2_IN0_Msk)
        case 2:
            p_reg->INTENCLR2 = mask;
            break;
#endif
#if defined(GPIOTE_INTENCLR3_IN0_Msk)
        case 3:
            p_reg->INTENCLR3 = mask;
            break;
#endif
#if defined(GPIOTE_INTENCLR4_IN0_Msk)
        case 4:
            p_reg->INTENCLR4 = mask;
            break;
#endif
#if defined(GPIOTE_INTENCLR5_IN0_Msk)
        case 5:
            p_reg->INTENCLR5 = mask;
            break;
#endif
#if defined(GPIOTE_INTENCLR6_IN0_Msk)
        case 6:
            p_reg->INTENCLR6 = mask;
            break;
#endif
        default:
            NRFX_ASSERT(false);
            break;
    }
    uint inst = gpiote_number_from_ptr(p_reg);
    nrf_gpiote_regw_sideeffects_INTENCLR(inst, group_idx);
}
#endif

void nrf_gpiote_event_configure(NRF_GPIOTE_Type *     p_reg,
                                uint32_t              idx,
                                uint32_t              pin,
                                nrf_gpiote_polarity_t polarity)
{
  uint inst = gpiote_number_from_ptr(p_reg);
  p_reg->CONFIG[idx] &= ~(GPIOTE_CONFIG_PORT_PIN_Msk | GPIOTE_CONFIG_POLARITY_Msk);
  p_reg->CONFIG[idx] |= ((pin << GPIOTE_CONFIG_PSEL_Pos) & GPIOTE_CONFIG_PORT_PIN_Msk) |
                        ((polarity << GPIOTE_CONFIG_POLARITY_Pos) & GPIOTE_CONFIG_POLARITY_Msk);
  nrf_gpiote_regw_sideeffects_CONFIG(inst, idx);
}

void nrf_gpiote_task_enable(NRF_GPIOTE_Type * p_reg, uint32_t idx)
{
    uint inst = gpiote_number_from_ptr(p_reg);
    uint32_t final_config = p_reg->CONFIG[idx] | GPIOTE_CONFIG_MODE_Task;
    p_reg->CONFIG[idx] = final_config;
    nrf_gpiote_regw_sideeffects_CONFIG(inst, idx);
}

void nrf_gpiote_task_disable(NRF_GPIOTE_Type * p_reg, uint32_t idx)
{
    uint inst = gpiote_number_from_ptr(p_reg);
    p_reg->CONFIG[idx] &= ~GPIOTE_CONFIG_MODE_Msk;
    nrf_gpiote_regw_sideeffects_CONFIG(inst, idx);
}

void nrf_gpiote_task_configure(NRF_GPIOTE_Type *     p_reg,
                               uint32_t              idx,
                               uint32_t              pin,
                               nrf_gpiote_polarity_t polarity,
                               nrf_gpiote_outinit_t  init_val)
{
  uint inst = gpiote_number_from_ptr(p_reg);
  p_reg->CONFIG[idx] &= ~(GPIOTE_CONFIG_PORT_PIN_Msk |
                          GPIOTE_CONFIG_POLARITY_Msk |
                          GPIOTE_CONFIG_OUTINIT_Msk);

  p_reg->CONFIG[idx] |= ((pin << GPIOTE_CONFIG_PSEL_Pos) & GPIOTE_CONFIG_PORT_PIN_Msk) |
                        ((polarity << GPIOTE_CONFIG_POLARITY_Pos) & GPIOTE_CONFIG_POLARITY_Msk) |
                        ((init_val << GPIOTE_CONFIG_OUTINIT_Pos) & GPIOTE_CONFIG_OUTINIT_Msk);

  nrf_gpiote_regw_sideeffects_CONFIG(inst, idx);
}

void nrf_gpiote_task_force(NRF_GPIOTE_Type *    p_reg,
                           uint32_t             idx,
                           nrf_gpiote_outinit_t init_val)
{
    uint inst = gpiote_number_from_ptr(p_reg);
    p_reg->CONFIG[idx] = (p_reg->CONFIG[idx] & ~GPIOTE_CONFIG_OUTINIT_Msk) |
                         ((init_val << GPIOTE_CONFIG_OUTINIT_Pos) & GPIOTE_CONFIG_OUTINIT_Msk);
    nrf_gpiote_regw_sideeffects_CONFIG(inst, idx);
}

void nrf_gpiote_te_default(NRF_GPIOTE_Type * p_reg, uint32_t idx)
{
    uint inst = gpiote_number_from_ptr(p_reg);
    p_reg->CONFIG[idx] = 0;
#if !defined(NRF51_SERIES) && !defined(NRF52_SERIES)
    p_reg->CONFIG[idx] = 0;
#endif
    nrf_gpiote_regw_sideeffects_CONFIG(inst, idx);
}
