/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "NHW_misc_int.h"

uint nhw_get_simu_real_conv_table(struct simu_real_conv_table_t **table) {
  struct simu_real_conv_table_t simu_real_conv_init[] = {
    DEF_ENTRY(FICR,),
    DEF_ENTRY(UICR,),
    //DEF_ENTRY(APPROTECT),
    DEF_ENTRY(CLOCK,),
    DEF_ENTRY(POWER,),
    {(void *)NRF_P0_BASE , NRF_P0_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    {(void *)NRF_P1_BASE , NRF_P1_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    DEF_ENTRY(RADIO,),
    DEF_ENTRY(UARTE, 0),
    //SPI0-1
    DEF_ENTRY(NFCT,),
    DEF_ENTRY(GPIOTE,),
    //DEF_ENTRY(SAADC,)
    DEF_ENTRY(TIMER, 0),
    DEF_ENTRY(TIMER, 1),
    DEF_ENTRY(TIMER, 2),
    DEF_ENTRY(RTC, 0),
    DEF_ENTRY(TEMP,),
    DEF_ENTRY(RNG,),
    DEF_ENTRY(ECB,),
    DEF_ENTRY(AAR,),
    DEF_ENTRY(CCM,),
    //DEF_ENTRY(WDT,),
    DEF_ENTRY(RTC, 1),
    //DEF_ENTRY(QDEC,),
    //DEF_ENTRY(COMP,),
    //DEF_ENTRY(LPCOMP,),
    DEF_ENTRY(EGU, 0),
    DEF_ENTRY(EGU, 1),
    DEF_ENTRY(EGU, 2),
    DEF_ENTRY(EGU, 3),
    DEF_ENTRY(EGU, 4),
    DEF_ENTRY(EGU, 5),
    DEF_ENTRY(TIMER, 3),
    DEF_ENTRY(TIMER, 4),
    DEF_ENTRY(PWM, 0),
    //DEF_ENTRY(PDM,),
    //DEF_ENTRY(ACL,),
    DEF_ENTRY(NVMC,),
    DEF_ENTRY(PPI,),
    DEF_ENTRY(MWU,),
    DEF_ENTRY(PWM, 1),
    DEF_ENTRY(PWM, 2),
    //SPI2
    DEF_ENTRY(RTC, 2),
    //DEF_ENTRY(I2S,),
    //DEF_ENTRY(FPU,),
    //DEF_ENTRY(USBD,),
    DEF_ENTRY(UARTE, 1),
    DEF_ENTRY(PWM, 3),
    //SPIM3
  };

  *table = bs_malloc(sizeof(simu_real_conv_init));
  memcpy(*table, simu_real_conv_init, sizeof(simu_real_conv_init));
  return sizeof(simu_real_conv_init)/sizeof(simu_real_conv_init[0]);
}
