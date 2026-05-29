/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "NHW_misc_int.h"

uint nhw_get_simu_real_conv_table(struct simu_real_conv_table_t **table) {
  struct simu_real_conv_table_t simu_real_conv_init[] = {
    DEF_ENTRY(FICR, ),
    DEF_ENTRY(UICR, ),
    DEF_ENTRY(CRACENCORE, ),
    DEF_ENTRY(DPPIC, 00),
    DEF_ENTRY(PPIB, 00),
    DEF_ENTRY(PPIB, 01),
    DEF_ENTRY(AAR, 00),
    DEF_ENTRY(CCM, 00),
    DEF_ENTRY(ECB, 00),
    DEF_ENTRY(RRAMC, ),
    //DEF_ENTRY(CTRLA, ),
    //DEF_ENTRY(TAD, ),
    DEF_ENTRY(TIMER, 00),
    DEF_ENTRY(EGU, 00),
    DEF_ENTRY(CRACEN, ),
    DEF_ENTRY(DPPIC, 10),
    DEF_ENTRY(PPIB, 10),
    DEF_ENTRY(PPIB, 11),
    DEF_ENTRY(TIMER, 10),
    DEF_ENTRY(EGU, 10),
    DEF_ENTRY(RADIO, ),
    DEF_ENTRY(DPPIC, 20),
    DEF_ENTRY(PPIB, 20),
    DEF_ENTRY(PPIB, 21),
    DEF_ENTRY(PPIB, 22),
    //DEF_ENTRY(SPIM, 20),
    //DEF_ENTRY(SPIS, 20),
    //DEF_ENTRY(TWIM, 20),
    //DEF_ENTRY(TWIS, 20),
    DEF_ENTRY(UARTE, 20),
    //DEF_ENTRY(SPIM, 21),
    //DEF_ENTRY(SPIS, 21),
    //DEF_ENTRY(TWIM, 21),
    //DEF_ENTRY(TWIS, 21),
    DEF_ENTRY(UARTE, 21),
    //DEF_ENTRY(SPIM, 22),
    //DEF_ENTRY(SPIS, 22),
    //DEF_ENTRY(TWIM, 22),
    //DEF_ENTRY(TWIS, 22),
    DEF_ENTRY(UARTE, 22),
    DEF_ENTRY(EGU, 20),
    DEF_ENTRY(TIMER, 20),
    //DEF_ENTRY(MEMCONF, ),
    //DEF_ENTRY(PWM, 20),
    //DEF_ENTRY(SAADC, ),
    DEF_ENTRY(TEMP, ),
    {(void *)NRF_P1_BASE, NRF_P1_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    DEF_ENTRY(GPIOTE, 20),
    //DEF_ENTRY(QDEC, 20),
    DEF_ENTRY(GRTC, ),
    //DEF_ENTRY(TAMPC, ),
    DEF_ENTRY(DPPIC, 30),
    DEF_ENTRY(PPIB, 30),
    //DEF_ENTRY(WDT, 30),
    {(void *)NRF_P0_BASE, NRF_P0_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    DEF_ENTRY(GPIOTE, 30),
    DEF_ENTRY(CLOCK, ),
    DEF_ENTRY(POWER, ),
    DEF_ENTRY(RESET, ),
    //DEF_ENTRY(OSCILLATORS, ),
    //DEF_ENTRY(REGULATORS, ),
    //DEF_ENTRY(VREGUSB, ),
  };

  *table = bs_malloc(sizeof(simu_real_conv_init));
  memcpy(*table, simu_real_conv_init, sizeof(simu_real_conv_init));
  return sizeof(simu_real_conv_init)/sizeof(simu_real_conv_init[0]);
}
