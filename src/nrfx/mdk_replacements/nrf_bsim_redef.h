/*
 * Copyright (c) 2018 Oticon A/S
 * Copyright (c) 2020-2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file redefines macros from nrf that need to be different for simulated devices.
 */

#ifndef NRF_BSIM_REDEF_H
#define NRF_BSIM_REDEF_H

#include "NHW_config.h"

#ifdef __cplusplus
extern "C" {
#endif

void *nhw_convert_periph_base_addr(void *hw_addr);

#if defined(NRF52833_XXAA)
#include "nrf_bsim_redef.52833.h"

#elif defined(NRF5340_XXAA_NETWORK) || defined(NRF5340_XXAA_APPLICATION)
#include "nrf_bsim_redef.5340.h"

#elif defined(NRF54L15_XXAA)
#include "nrf_bsim_redef.54L15.h"

#elif defined(NRF54LS05A_XXAA)
#include "nrf_bsim_redef.54LS05.h"

#elif defined(NRF54LM20A_XXAA) || defined(NRF54LM20A_ENGA_XXAA)
#include "nrf_bsim_redef.54LM20.h"

#else
#error "Platform not supported"
#endif

#ifdef __cplusplus
}
#endif

#endif /* NRF_BSIM_REDEF_H */
