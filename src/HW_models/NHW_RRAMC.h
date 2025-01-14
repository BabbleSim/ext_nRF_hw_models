/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_NHW_RRAMC_H
#define _NRF_HW_MODEL_NHW_RRAMC_H

#include <stdint.h>
#include "bs_types.h"

#ifdef __cplusplus
extern "C"{
#endif

void nhw_RRAMC_regw_sideeffects_ERASEALL(uint inst);
void* nhw_RRAMC_get_RRAM_base_address(uint inst);

void nhw_RRAMC_write_word(uint32_t address, uint32_t value);
void nhw_RRAMC_write_halfword(uint32_t address, uint16_t value);
void nhw_RRAMC_write_byte(uint32_t address, uint8_t value);
uint32_t nhw_RRAMC_read_word(uint32_t address);
uint16_t nhw_RRAMC_read_halfword(uint32_t address);
uint8_t nhw_RRAMC_read_byte(uint32_t address);
void nhw_RRAMC_write_buffer(uint32_t address, void *src, size_t size);
void nhw_RRAMC_read_buffer(void *dest, uint32_t address, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_NHW_RRAMC_H */
