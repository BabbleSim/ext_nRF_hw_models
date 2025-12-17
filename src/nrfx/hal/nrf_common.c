/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */

#include <stdbool.h>
#include <stdint.h>
#include <nrfx.h>
#include "NHW_misc.h"

#if defined(ADDRESS_BUS_Msk)
uint8_t nrf_address_bus_get(uint32_t addr, size_t size)
{
  uintptr_t real_hw_addr = (uintptr_t)nhw_convert_per_addr_sim_to_hw((void *)addr);
  return (uint8_t)((real_hw_addr & ADDRESS_BUS_Msk & ~(size - 1)) >> ADDRESS_BUS_Pos);
}
#endif

#if defined(ADDRESS_SLAVE_Msk)
uint8_t nrf_address_slave_get(uint32_t addr)
{
  uintptr_t real_hw_addr = (uintptr_t)nhw_convert_per_addr_sim_to_hw((void *)addr);
  return (uint8_t)((real_hw_addr & ADDRESS_SLAVE_Msk) >> ADDRESS_SLAVE_Pos);
}
#endif

bool nrf_dma_accessible_check(void const * p_reg, void const * p_object)
{
  (void)p_reg;
  (void)p_object;
  return true; /* No restrictions in simulation */
}
