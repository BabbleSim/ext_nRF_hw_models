/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */

#include <stdint.h>
#include "hal/nrf_rramc.h"
#include "NHW_RRAMC.h"

void nrf_rramc_erase_all_set(NRF_RRAMC_Type * p_reg)
{
    p_reg->ERASE.ERASEALL = RRAMC_ERASE_ERASEALL_ERASE_Erase;
    nhw_RRAMC_regw_sideeffects_ERASEALL(0);
}

/* Note that tasks and events are not implemented in the HW model */


void nrf_rramc_byte_write(uint32_t address, uint8_t value)
{
    nhw_RRAMC_write_byte(address, value);
}

void nrf_rramc_halfword_write(uint32_t address, uint16_t value)
{
    nhw_RRAMC_write_halfword(address,value);
}

void nrf_rramc_word_write(uint32_t address, uint32_t value)
{
    nhw_RRAMC_write_word(address,value);
}

void nrf_rramc_buffer_write(uint32_t address, void * src, uint32_t num_bytes)
{
    nhw_RRAMC_write_buffer(address, src, num_bytes);
}

uint8_t nrf_rramc_byte_read(uint32_t address)
{
    return nhw_RRAMC_read_byte(address);
}

uint16_t nrf_rramc_halfword_read(uint32_t address)
{
    return nhw_RRAMC_read_halfword(address);
}

uint32_t nrf_rramc_word_read(uint32_t address)
{
    return nhw_RRAMC_read_word(address);
}

void nrf_rramc_buffer_read(void * dst, uint32_t address, uint32_t num_bytes)
{
    nhw_RRAMC_read_buffer(dst, address, num_bytes);
}
