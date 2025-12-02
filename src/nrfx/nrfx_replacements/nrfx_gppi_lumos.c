/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <NHW_misc.h>

uint32_t nrfx_gppi_domain_id_get(uint32_t addr)
{
  addr = (intptr_t)nhw_convert_per_addr_sim_to_hw((void *)addr);
  return ((addr >> 18) & 0x7) - 1;
}
