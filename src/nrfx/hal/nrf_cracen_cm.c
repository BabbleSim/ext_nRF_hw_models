/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */

#include <stdint.h>
#include <nrfx.h>
#if 1 //( (defined(NRFX_RELEASE_VER_AT_LEAST) && NRFX_RELEASE_VER_AT_LEAST(3,10,0)) )
#include "hal/nrf_cracen_cm.h"
#include "bs_tracing.h"
#include "NHW_CRACEN_CM.h"

void nrf_cracen_cm_config_indirect_set(NRF_CRACENCORE_Type *           p_reg,
                                       nrf_cracen_cm_config_indirect_mask_t mask)
{
    p_reg->CRYPTMSTRDMA.CONFIG = (uint32_t)mask;
    nhw_CRACEN_CM_regw_sideeffects_CONFIG();
}

void nrf_cracen_cm_softreset(NRF_CRACENCORE_Type * p_reg)
{
    p_reg->CRYPTMSTRDMA.CONFIG = CRACENCORE_CRYPTMSTRDMA_CONFIG_SOFTRST_Msk;
    nhw_CRACEN_CM_regw_sideeffects_CONFIG();
    p_reg->CRYPTMSTRDMA.CONFIG = 0;
    nhw_CRACEN_CM_regw_sideeffects_CONFIG();
}

void nrf_cracen_cm_start(NRF_CRACENCORE_Type * p_reg)
{
    p_reg->CRYPTMSTRDMA.START = CRACENCORE_CRYPTMSTRDMA_START_STARTFETCH_Msk
                               | CRACENCORE_CRYPTMSTRDMA_START_STARTPUSH_Msk;
    nhw_CRACEN_CM_regw_sideeffects_START();
}

void nrf_cracen_cm_int_enable(NRF_CRACENCORE_Type * p_reg, uint32_t mask)
{
    p_reg->CRYPTMSTRDMA.INTENSET = mask;
    nhw_CRACEN_CM_regw_sideeffects_INTENSET(0);
}

void nrf_cracen_cm_int_disable(NRF_CRACENCORE_Type * p_reg, uint32_t mask)
{
    p_reg->CRYPTMSTRDMA.INTENCLR = mask;
    nhw_CRACEN_CM_regw_sideeffects_INTENCLR(0);
}

void nrf_cracen_cm_int_clear(NRF_CRACENCORE_Type * p_reg, uint32_t mask)
{
    p_reg->CRYPTMSTRDMA.INTSTATCLR = mask;
    nhw_CRACEN_CM_regw_sideeffects_INTSTARTCLR(0);
}

#endif
