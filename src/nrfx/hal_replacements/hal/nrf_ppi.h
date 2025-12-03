/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * The integrator should ensure this header is in
 * the include path before the real hal/nrf_ppi.h
 */

#ifndef HAL_REPLACEMENTS_HAL_NRF_PPI_H
#define HAL_REPLACEMENTS_HAL_NRF_PPI_H

#include <stdint.h>
#include <NHW_misc.h>
#include_next "hal/nrf_ppi.h"

static inline bool _nrf_ppi_ep_is_evt(uint32_t ep) {
  ep = (uintptr_t)nhw_convert_per_addr_sim_to_hw((void *)ep);
  return (ep & NRFX_BIT(8));
}

#undef NRF_PPI_ENDPOINT_IS_EVENT
#define NRF_PPI_ENDPOINT_IS_EVENT(_ep) _nrf_ppi_ep_is_evt((uint32_t)_ep)

#endif /* HAL_REPLACEMENTS_HAL_NRF_PPI_H */
