/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_NHW_MISC_H
#define _NRF_HW_MODEL_NHW_MISC_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

const char *nhw_get_core_name(unsigned int core_n);
void *nhw_convert_RAM_addr(void *ram_addr);

void *nhw_convert_per_addr_sim_to_hw(void *sim_addr);
void *nhw_convert_per_addr_hw_to_sim(void *real_addr);
void *nhw_convert_periph_base_addr(void *hw_addr);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_NHW_MISC_H */
