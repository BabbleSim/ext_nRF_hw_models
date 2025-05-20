/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */

#include <stdint.h>
#include <nrfx.h>
#if ( (defined(NRFX_RELEASE_VER_AT_LEAST) && NRFX_RELEASE_VER_AT_LEAST(3,10,0)) )
#include "hal/nrf_cracen_rng.h"
#include "bs_tracing.h"
#include "NHW_CRACEN_RNG.h"

void nrf_cracen_rng_control_set(NRF_CRACENCORE_Type *            p_reg,
                                nrf_cracen_rng_control_t const * p_config)
{
    p_reg->RNGCONTROL.CONTROL =
          ((p_config->enable << CRACENCORE_RNGCONTROL_CONTROL_ENABLE_Pos)
           & CRACENCORE_RNGCONTROL_CONTROL_ENABLE_Msk)
        | ((p_config->fifo_full_int_en << CRACENCORE_RNGCONTROL_CONTROL_INTENFULL_Pos)
            & CRACENCORE_RNGCONTROL_CONTROL_INTENFULL_Msk)
        | ((p_config->soft_reset << CRACENCORE_RNGCONTROL_CONTROL_SOFTRST_Pos)
            & CRACENCORE_RNGCONTROL_CONTROL_SOFTRST_Msk)
#if (NRF_CRACEN_RNG_VERSION >= 2)
        | ((p_config->blending_method << CRACENCORE_RNGCONTROL_CONTROL_BLENDINGMETHOD_Pos)
            & CRACENCORE_RNGCONTROL_CONTROL_BLENDINGMETHOD_Msk)
#endif
        | ((p_config->number_128_blocks << CRACENCORE_RNGCONTROL_CONTROL_NB128BITBLOCKS_Pos)
            & CRACENCORE_RNGCONTROL_CONTROL_NB128BITBLOCKS_Msk);
    nhw_CRACEN_RNG_regw_sideeffects_CONTROL();
}

uint32_t nrf_cracen_rng_fifo_get(NRF_CRACENCORE_Type const * p_reg)
{
    nhw_CRACEN_RNG_regr_sideeffects_FIFO();
    return p_reg->RNGCONTROL.FIFO[0];
}

#endif
