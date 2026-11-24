/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_FICR_COMMON_H
#define _NRF_HW_MODEL_FICR_COMMON_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

/*
 * Override the FICR DEVICEADDR and DEVICEADDRTYPE registers with the values
 * given in the command line (if any).
 * To be called by each FICR model at the end of its initialization.
 */
void nhw_ficr_apply_addr_args(uint32_t deviceaddr[2], uint32_t *deviceaddrtype);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_FICR_COMMON_H */
