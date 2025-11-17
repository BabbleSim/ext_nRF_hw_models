/**
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * We do not have any errata we want to run in simulation yet,
 * so in general we do not define any NRF<xxx>_ERRATA_<yy>_ENABLE_WORKAROUND or nrf<xxx>_errata_<yy>
 */

#ifndef NRF_ERRATAS_H
#define NRF_ERRATAS_H

#include <stdbool.h>
#include "nrf.h"

/* Users of nrf_errata.h should guard calls to nrf<xxx>_errata_<yy> with an #ifdef NRF<xxx>_ERRATA_<yy>_ENABLE_WORKAROUND
 * But in some places this was not done, so let's provide a working replacement for those to avoid breaking that SW */
#define _BSIM_NRF_N_ERRATA_M(n,m) static inline bool nrf##n##_errata_##m(void) { return false; }

_BSIM_NRF_N_ERRATA_M(52,122)
_BSIM_NRF_N_ERRATA_M(53,4)
_BSIM_NRF_N_ERRATA_M(53,161)

#undef _BSIM_NRF_N_ERRATA_M

#endif // NRF_ERRATAS_H
