/*
 * Copyright (c) 2018 Oticon A/S
 * Copyright (c) 2020-2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file redefines macros from nrf that need to be different for simulated devices.
 */

#ifndef NRF_BSIM_REDEF_H
#define NRF_BSIM_REDEF_H

#include "NHW_config.h"

#ifdef __cplusplus
extern "C" {
#endif

void *nhw_convert_periph_base_addr(void *hw_addr);

#if defined(NRF52833_XXAA)

#define NRF_FICR_BASE_REAL_HW               0x10000000UL
#define NRF_UICR_BASE_REAL_HW               0x10001000UL
#define NRF_APPROTECT_BASE_REAL_HW          0x40000000UL
#define NRF_CLOCK_BASE_REAL_HW              0x40000000UL
#define NRF_POWER_BASE_REAL_HW              0x40000000UL
#define NRF_P0_BASE_REAL_HW                 0x50000000UL
#define NRF_P1_BASE_REAL_HW                 0x50000300UL
#define NRF_RADIO_BASE_REAL_HW              0x40001000UL
#define NRF_UART0_BASE_REAL_HW              0x40002000UL
#define NRF_UARTE0_BASE_REAL_HW             0x40002000UL
#define NRF_SPI0_BASE_REAL_HW               0x40003000UL
#define NRF_SPIM0_BASE_REAL_HW              0x40003000UL
#define NRF_SPIS0_BASE_REAL_HW              0x40003000UL
#define NRF_TWI0_BASE_REAL_HW               0x40003000UL
#define NRF_TWIM0_BASE_REAL_HW              0x40003000UL
#define NRF_TWIS0_BASE_REAL_HW              0x40003000UL
#define NRF_SPI1_BASE_REAL_HW               0x40004000UL
#define NRF_SPIM1_BASE_REAL_HW              0x40004000UL
#define NRF_SPIS1_BASE_REAL_HW              0x40004000UL
#define NRF_TWI1_BASE_REAL_HW               0x40004000UL
#define NRF_TWIM1_BASE_REAL_HW              0x40004000UL
#define NRF_TWIS1_BASE_REAL_HW              0x40004000UL
#define NRF_NFCT_BASE_REAL_HW               0x40005000UL
#define NRF_GPIOTE_BASE_REAL_HW             0x40006000UL
#define NRF_SAADC_BASE_REAL_HW              0x40007000UL
#define NRF_TIMER0_BASE_REAL_HW             0x40008000UL
#define NRF_TIMER1_BASE_REAL_HW             0x40009000UL
#define NRF_TIMER2_BASE_REAL_HW             0x4000A000UL
#define NRF_RTC0_BASE_REAL_HW               0x4000B000UL
#define NRF_TEMP_BASE_REAL_HW               0x4000C000UL
#define NRF_RNG_BASE_REAL_HW                0x4000D000UL
#define NRF_ECB_BASE_REAL_HW                0x4000E000UL
#define NRF_AAR_BASE_REAL_HW                0x4000F000UL
#define NRF_CCM_BASE_REAL_HW                0x4000F000UL
#define NRF_WDT_BASE_REAL_HW                0x40010000UL
#define NRF_RTC1_BASE_REAL_HW               0x40011000UL
#define NRF_QDEC_BASE_REAL_HW               0x40012000UL
#define NRF_COMP_BASE_REAL_HW               0x40013000UL
#define NRF_LPCOMP_BASE_REAL_HW             0x40013000UL
#define NRF_EGU0_BASE_REAL_HW               0x40014000UL
#define NRF_SWI0_BASE_REAL_HW               0x40014000UL
#define NRF_EGU1_BASE_REAL_HW               0x40015000UL
#define NRF_SWI1_BASE_REAL_HW               0x40015000UL
#define NRF_EGU2_BASE_REAL_HW               0x40016000UL
#define NRF_SWI2_BASE_REAL_HW               0x40016000UL
#define NRF_EGU3_BASE_REAL_HW               0x40017000UL
#define NRF_SWI3_BASE_REAL_HW               0x40017000UL
#define NRF_EGU4_BASE_REAL_HW               0x40018000UL
#define NRF_SWI4_BASE_REAL_HW               0x40018000UL
#define NRF_EGU5_BASE_REAL_HW               0x40019000UL
#define NRF_SWI5_BASE_REAL_HW               0x40019000UL
#define NRF_TIMER3_BASE_REAL_HW             0x4001A000UL
#define NRF_TIMER4_BASE_REAL_HW             0x4001B000UL
#define NRF_PWM0_BASE_REAL_HW               0x4001C000UL
#define NRF_PDM_BASE_REAL_HW                0x4001D000UL
#define NRF_ACL_BASE_REAL_HW                0x4001E000UL
#define NRF_NVMC_BASE_REAL_HW               0x4001E000UL
#define NRF_PPI_BASE_REAL_HW                0x4001F000UL
#define NRF_MWU_BASE_REAL_HW                0x40020000UL
#define NRF_PWM1_BASE_REAL_HW               0x40021000UL
#define NRF_PWM2_BASE_REAL_HW               0x40022000UL
#define NRF_SPI2_BASE_REAL_HW               0x40023000UL
#define NRF_SPIM2_BASE_REAL_HW              0x40023000UL
#define NRF_SPIS2_BASE_REAL_HW              0x40023000UL
#define NRF_RTC2_BASE_REAL_HW               0x40024000UL
#define NRF_I2S_BASE_REAL_HW                0x40025000UL
#define NRF_FPU_BASE_REAL_HW                0x40026000UL
#define NRF_USBD_BASE_REAL_HW               0x40027000UL
#define NRF_UARTE1_BASE_REAL_HW             0x40028000UL
#define NRF_PWM3_BASE_REAL_HW               0x4002D000UL
#define NRF_SPIM3_BASE_REAL_HW              0x4002F000UL

/*
 * Redefine the base addresses.
 */
extern NRF_AAR_Type NRF_AAR_regs;
#undef NRF_AAR_BASE
#define NRF_AAR_BASE                      (&NRF_AAR_regs)
extern NRF_RNG_Type NRF_RNG_regs;
#undef NRF_RNG_BASE
#define NRF_RNG_BASE                      (&NRF_RNG_regs)
extern NRF_TEMP_Type NRF_TEMP_regs;
#undef NRF_TEMP_BASE
#define NRF_TEMP_BASE                     (&NRF_TEMP_regs)
extern NRF_RTC_Type NRF_RTC_regs[];
#undef NRF_RTC0_BASE
#define NRF_RTC0_BASE                     (&NRF_RTC_regs[0])
#undef NRF_RTC1_BASE
#define NRF_RTC1_BASE                     (&NRF_RTC_regs[1])
#undef NRF_RTC2_BASE
#define NRF_RTC2_BASE                     (&NRF_RTC_regs[2])
extern NRF_ECB_Type NRF_ECB_regs;
#undef NRF_ECB_BASE
#define NRF_ECB_BASE                      (&NRF_ECB_regs)
extern NRF_CCM_Type NRF_CCM_regs;
#undef NRF_CCM_BASE
#define NRF_CCM_BASE                      (&NRF_CCM_regs)
extern NRF_RADIO_Type NRF_RADIO_regs;
#undef NRF_RADIO_BASE
#define NRF_RADIO_BASE                    (&NRF_RADIO_regs)
extern NRF_CLOCK_Type *NRF_CLOCK_regs[];
#undef NRF_CLOCK_BASE
#define NRF_CLOCK_BASE                    (NRF_CLOCK_regs[NHW_CLKPWR_0])
extern NRF_FICR_Type NRF_FICR_regs;
#undef NRF_FICR_BASE
#define NRF_FICR_BASE                     (&NRF_FICR_regs)
extern NRF_PPI_Type NRF_PPI_regs;
#undef NRF_PPI_BASE
#define NRF_PPI_BASE                      (&NRF_PPI_regs)
extern NRF_TIMER_Type NRF_TIMER_regs[];
#undef NRF_TIMER0_BASE
#define NRF_TIMER0_BASE                   (&NRF_TIMER_regs[0])
#undef NRF_TIMER1_BASE
#define NRF_TIMER1_BASE                   (&NRF_TIMER_regs[1])
#undef NRF_TIMER2_BASE
#define NRF_TIMER2_BASE                   (&NRF_TIMER_regs[2])
#undef NRF_TIMER3_BASE
#define NRF_TIMER3_BASE                   (&NRF_TIMER_regs[3])
#undef NRF_TIMER4_BASE
#define NRF_TIMER4_BASE                   (&NRF_TIMER_regs[4])
#undef NRF_POWER_BASE
extern NRF_POWER_Type *NRF_POWER_regs[];
#define NRF_POWER_BASE                    (NRF_POWER_regs[NHW_CLKPWR_0])
extern NRF_GPIO_Type NRF_GPIO_regs[];
#undef NRF_P0_BASE
#define NRF_P0_BASE                       (&NRF_GPIO_regs[0])
#undef NRF_P1_BASE
#define NRF_P1_BASE                       (&NRF_GPIO_regs[1])
extern NRF_GPIOTE_Type NRF_GPIOTE_regs[];
#undef NRF_GPIOTE_BASE
#define NRF_GPIOTE_BASE                   (&NRF_GPIOTE_regs[NHW_GPIOTE_0])
extern NRF_MWU_Type NRF_MWU_regs;
#undef NRF_MWU_BASE
#define NRF_MWU_BASE                      (&NRF_MWU_regs)
extern NRF_NFCT_Type NRF_NFCT_regs;
#undef NRF_NFCT_BASE
#define NRF_NFCT_BASE                     (&NRF_NFCT_regs)
extern NRF_NVMC_Type *NRF_NVMC_regs_p[];
#undef NRF_NVMC_BASE
#define NRF_NVMC_BASE                     (NRF_NVMC_regs_p[0])
extern NRF_EGU_Type NRF_EGU_regs[6];
#undef NRF_EGU0_BASE
#define NRF_EGU0_BASE                     (&NRF_EGU_regs[0])
#undef NRF_EGU1_BASE
#define NRF_EGU1_BASE                     (&NRF_EGU_regs[1])
#undef NRF_EGU2_BASE
#define NRF_EGU2_BASE                     (&NRF_EGU_regs[2])
#undef NRF_EGU3_BASE
#define NRF_EGU3_BASE                     (&NRF_EGU_regs[3])
#undef NRF_EGU4_BASE
#define NRF_EGU4_BASE                     (&NRF_EGU_regs[4])
#undef NRF_EGU5_BASE
#define NRF_EGU5_BASE                     (&NRF_EGU_regs[5])
#undef NRF_UICR_BASE
extern NRF_UICR_Type *NRF_UICR_regs_p[];
#define NRF_UICR_BASE                     (NRF_UICR_regs_p[0])
extern NRF_UARTE_Type NRF_UARTE_regs[];
#undef NRF_UART0_BASE
#define NRF_UART0_BASE                    (&NRF_UARTE_regs[NHW_UART_0])
#undef NRF_UARTE0_BASE
#define NRF_UARTE0_BASE                   (&NRF_UARTE_regs[NHW_UART_0])
#undef NRF_UARTE1_BASE
#define NRF_UARTE1_BASE                   (&NRF_UARTE_regs[NHW_UART_1])

/********************************************************************/
/********************************************************************/
/********************************************************************/
#elif defined(NRF5340_XXAA_NETWORK) || defined(NRF5340_XXAA_APPLICATION)

#define NRF_FICR_NS_NET_BASE_REAL_HW            0x01FF0000UL
#define NRF_UICR_NS_NET_BASE_REAL_HW            0x01FF8000UL
#define NRF_CTI_NS_NET_BASE_REAL_HW             0xE0042000UL
#define NRF_DCNF_NS_NET_BASE_REAL_HW            0x41000000UL
#define NRF_VREQCTRL_NS_NET_BASE_REAL_HW        0x41004000UL
#define NRF_CLOCK_NS_NET_BASE_REAL_HW           0x41005000UL
#define NRF_POWER_NS_NET_BASE_REAL_HW           0x41005000UL
#define NRF_RESET_NS_NET_BASE_REAL_HW           0x41005000UL
#define NRF_CTRLAP_NS_NET_BASE_REAL_HW          0x41006000UL
#define NRF_RADIO_NS_NET_BASE_REAL_HW           0x41008000UL
#define NRF_RNG_NS_NET_BASE_REAL_HW             0x41009000UL
#define NRF_GPIOTE_NS_NET_BASE_REAL_HW          0x4100A000UL
#define NRF_WDT_NS_NET_BASE_REAL_HW             0x4100B000UL
#define NRF_TIMER0_NS_NET_BASE_REAL_HW          0x4100C000UL
#define NRF_ECB_NS_NET_BASE_REAL_HW             0x4100D000UL
#define NRF_AAR_NS_NET_BASE_REAL_HW             0x4100E000UL
#define NRF_CCM_NS_NET_BASE_REAL_HW             0x4100E000UL
#define NRF_DPPIC_NS_NET_BASE_REAL_HW           0x4100F000UL
#define NRF_TEMP_NS_NET_BASE_REAL_HW            0x41010000UL
#define NRF_RTC0_NS_NET_BASE_REAL_HW            0x41011000UL
#define NRF_IPC_NS_NET_BASE_REAL_HW             0x41012000UL
#define NRF_SPIM0_NS_NET_BASE_REAL_HW           0x41013000UL
#define NRF_SPIS0_NS_NET_BASE_REAL_HW           0x41013000UL
#define NRF_TWIM0_NS_NET_BASE_REAL_HW           0x41013000UL
#define NRF_TWIS0_NS_NET_BASE_REAL_HW           0x41013000UL
#define NRF_UARTE0_NS_NET_BASE_REAL_HW          0x41013000UL
#define NRF_EGU0_NS_NET_BASE_REAL_HW            0x41014000UL
#define NRF_RTC1_NS_NET_BASE_REAL_HW            0x41016000UL
#define NRF_TIMER1_NS_NET_BASE_REAL_HW          0x41018000UL
#define NRF_TIMER2_NS_NET_BASE_REAL_HW          0x41019000UL
#define NRF_SWI0_NS_NET_BASE_REAL_HW            0x4101A000UL
#define NRF_SWI1_NS_NET_BASE_REAL_HW            0x4101B000UL
#define NRF_SWI2_NS_NET_BASE_REAL_HW            0x4101C000UL
#define NRF_SWI3_NS_NET_BASE_REAL_HW            0x4101D000UL
#define NRF_APPMUTEX_NS_NET_BASE_REAL_HW        0x40030000UL
#define NRF_APPMUTEX_S_NET_BASE_REAL_HW         0x50030000UL
#define NRF_ACL_NS_NET_BASE_REAL_HW             0x41080000UL
#define NRF_NVMC_NS_NET_BASE_REAL_HW            0x41080000UL
#define NRF_VMC_NS_NET_BASE_REAL_HW             0x41081000UL
#define NRF_P0_NS_NET_BASE_REAL_HW              0x418C0500UL
#define NRF_P1_NS_NET_BASE_REAL_HW              0x418C0800UL

#define NRF_CACHEDATA_S_APP_BASE_REAL_HW        0x00F00000UL
#define NRF_CACHEINFO_S_APP_BASE_REAL_HW        0x00F08000UL
#define NRF_FICR_S_APP_BASE_REAL_HW             0x00FF0000UL
#define NRF_UICR_S_APP_BASE_REAL_HW             0x00FF8000UL
#define NRF_CTI_S_APP_BASE_REAL_HW              0xE0042000UL
#define NRF_TAD_S_APP_BASE_REAL_HW              0xE0080000UL
#define NRF_DCNF_NS_APP_BASE_REAL_HW            0x40000000UL
#define NRF_FPU_NS_APP_BASE_REAL_HW             0x40000000UL
#define NRF_DCNF_S_APP_BASE_REAL_HW             0x50000000UL
#define NRF_FPU_S_APP_BASE_REAL_HW              0x50000000UL
#define NRF_CACHE_S_APP_BASE_REAL_HW            0x50001000UL
#define NRF_SPU_S_APP_BASE_REAL_HW              0x50003000UL
#define NRF_OSCILLATORS_NS_APP_BASE_REAL_HW     0x40004000UL
#define NRF_REGULATORS_NS_APP_BASE_REAL_HW      0x40004000UL
#define NRF_OSCILLATORS_S_APP_BASE_REAL_HW      0x50004000UL
#define NRF_REGULATORS_S_APP_BASE_REAL_HW       0x50004000UL
#define NRF_CLOCK_NS_APP_BASE_REAL_HW           0x40005000UL
#define NRF_POWER_NS_APP_BASE_REAL_HW           0x40005000UL
#define NRF_RESET_NS_APP_BASE_REAL_HW           0x40005000UL
#define NRF_CLOCK_S_APP_BASE_REAL_HW            0x50005000UL
#define NRF_POWER_S_APP_BASE_REAL_HW            0x50005000UL
#define NRF_RESET_S_APP_BASE_REAL_HW            0x50005000UL
#define NRF_CTRLAP_NS_APP_BASE_REAL_HW          0x40006000UL
#define NRF_CTRLAP_S_APP_BASE_REAL_HW           0x50006000UL
#define NRF_SPIM0_NS_APP_BASE_REAL_HW           0x40008000UL
#define NRF_SPIS0_NS_APP_BASE_REAL_HW           0x40008000UL
#define NRF_TWIM0_NS_APP_BASE_REAL_HW           0x40008000UL
#define NRF_TWIS0_NS_APP_BASE_REAL_HW           0x40008000UL
#define NRF_UARTE0_NS_APP_BASE_REAL_HW          0x40008000UL
#define NRF_SPIM0_S_APP_BASE_REAL_HW            0x50008000UL
#define NRF_SPIS0_S_APP_BASE_REAL_HW            0x50008000UL
#define NRF_TWIM0_S_APP_BASE_REAL_HW            0x50008000UL
#define NRF_TWIS0_S_APP_BASE_REAL_HW            0x50008000UL
#define NRF_UARTE0_S_APP_BASE_REAL_HW           0x50008000UL
#define NRF_SPIM1_NS_APP_BASE_REAL_HW           0x40009000UL
#define NRF_SPIS1_NS_APP_BASE_REAL_HW           0x40009000UL
#define NRF_TWIM1_NS_APP_BASE_REAL_HW           0x40009000UL
#define NRF_TWIS1_NS_APP_BASE_REAL_HW           0x40009000UL
#define NRF_UARTE1_NS_APP_BASE_REAL_HW          0x40009000UL
#define NRF_SPIM1_S_APP_BASE_REAL_HW            0x50009000UL
#define NRF_SPIS1_S_APP_BASE_REAL_HW            0x50009000UL
#define NRF_TWIM1_S_APP_BASE_REAL_HW            0x50009000UL
#define NRF_TWIS1_S_APP_BASE_REAL_HW            0x50009000UL
#define NRF_UARTE1_S_APP_BASE_REAL_HW           0x50009000UL
#define NRF_SPIM4_NS_APP_BASE_REAL_HW           0x4000A000UL
#define NRF_SPIM4_S_APP_BASE_REAL_HW            0x5000A000UL
#define NRF_SPIM2_NS_APP_BASE_REAL_HW           0x4000B000UL
#define NRF_SPIS2_NS_APP_BASE_REAL_HW           0x4000B000UL
#define NRF_TWIM2_NS_APP_BASE_REAL_HW           0x4000B000UL
#define NRF_TWIS2_NS_APP_BASE_REAL_HW           0x4000B000UL
#define NRF_UARTE2_NS_APP_BASE_REAL_HW          0x4000B000UL
#define NRF_SPIM2_S_APP_BASE_REAL_HW            0x5000B000UL
#define NRF_SPIS2_S_APP_BASE_REAL_HW            0x5000B000UL
#define NRF_TWIM2_S_APP_BASE_REAL_HW            0x5000B000UL
#define NRF_TWIS2_S_APP_BASE_REAL_HW            0x5000B000UL
#define NRF_UARTE2_S_APP_BASE_REAL_HW           0x5000B000UL
#define NRF_SPIM3_NS_APP_BASE_REAL_HW           0x4000C000UL
#define NRF_SPIS3_NS_APP_BASE_REAL_HW           0x4000C000UL
#define NRF_TWIM3_NS_APP_BASE_REAL_HW           0x4000C000UL
#define NRF_TWIS3_NS_APP_BASE_REAL_HW           0x4000C000UL
#define NRF_UARTE3_NS_APP_BASE_REAL_HW          0x4000C000UL
#define NRF_SPIM3_S_APP_BASE_REAL_HW            0x5000C000UL
#define NRF_SPIS3_S_APP_BASE_REAL_HW            0x5000C000UL
#define NRF_TWIM3_S_APP_BASE_REAL_HW            0x5000C000UL
#define NRF_TWIS3_S_APP_BASE_REAL_HW            0x5000C000UL
#define NRF_UARTE3_S_APP_BASE_REAL_HW           0x5000C000UL
#define NRF_GPIOTE0_S_APP_BASE_REAL_HW          0x5000D000UL
#define NRF_SAADC_NS_APP_BASE_REAL_HW           0x4000E000UL
#define NRF_SAADC_S_APP_BASE_REAL_HW            0x5000E000UL
#define NRF_TIMER0_NS_APP_BASE_REAL_HW          0x4000F000UL
#define NRF_TIMER0_S_APP_BASE_REAL_HW           0x5000F000UL
#define NRF_TIMER1_NS_APP_BASE_REAL_HW          0x40010000UL
#define NRF_TIMER1_S_APP_BASE_REAL_HW           0x50010000UL
#define NRF_TIMER2_NS_APP_BASE_REAL_HW          0x40011000UL
#define NRF_TIMER2_S_APP_BASE_REAL_HW           0x50011000UL
#define NRF_RTC0_NS_APP_BASE_REAL_HW            0x40014000UL
#define NRF_RTC0_S_APP_BASE_REAL_HW             0x50014000UL
#define NRF_RTC1_NS_APP_BASE_REAL_HW            0x40015000UL
#define NRF_RTC1_S_APP_BASE_REAL_HW             0x50015000UL
#define NRF_DPPIC_NS_APP_BASE_REAL_HW           0x40017000UL
#define NRF_DPPIC_S_APP_BASE_REAL_HW            0x50017000UL
#define NRF_WDT0_NS_APP_BASE_REAL_HW            0x40018000UL
#define NRF_WDT0_S_APP_BASE_REAL_HW             0x50018000UL
#define NRF_WDT1_NS_APP_BASE_REAL_HW            0x40019000UL
#define NRF_WDT1_S_APP_BASE_REAL_HW             0x50019000UL
#define NRF_COMP_NS_APP_BASE_REAL_HW            0x4001A000UL
#define NRF_LPCOMP_NS_APP_BASE_REAL_HW          0x4001A000UL
#define NRF_COMP_S_APP_BASE_REAL_HW             0x5001A000UL
#define NRF_LPCOMP_S_APP_BASE_REAL_HW           0x5001A000UL
#define NRF_EGU0_NS_APP_BASE_REAL_HW            0x4001B000UL
#define NRF_EGU0_S_APP_BASE_REAL_HW             0x5001B000UL
#define NRF_EGU1_NS_APP_BASE_REAL_HW            0x4001C000UL
#define NRF_EGU1_S_APP_BASE_REAL_HW             0x5001C000UL
#define NRF_EGU2_NS_APP_BASE_REAL_HW            0x4001D000UL
#define NRF_EGU2_S_APP_BASE_REAL_HW             0x5001D000UL
#define NRF_EGU3_NS_APP_BASE_REAL_HW            0x4001E000UL
#define NRF_EGU3_S_APP_BASE_REAL_HW             0x5001E000UL
#define NRF_EGU4_NS_APP_BASE_REAL_HW            0x4001F000UL
#define NRF_EGU4_S_APP_BASE_REAL_HW             0x5001F000UL
#define NRF_EGU5_NS_APP_BASE_REAL_HW            0x40020000UL
#define NRF_EGU5_S_APP_BASE_REAL_HW             0x50020000UL
#define NRF_PWM0_NS_APP_BASE_REAL_HW            0x40021000UL
#define NRF_PWM0_S_APP_BASE_REAL_HW             0x50021000UL
#define NRF_PWM1_NS_APP_BASE_REAL_HW            0x40022000UL
#define NRF_PWM1_S_APP_BASE_REAL_HW             0x50022000UL
#define NRF_PWM2_NS_APP_BASE_REAL_HW            0x40023000UL
#define NRF_PWM2_S_APP_BASE_REAL_HW             0x50023000UL
#define NRF_PWM3_NS_APP_BASE_REAL_HW            0x40024000UL
#define NRF_PWM3_S_APP_BASE_REAL_HW             0x50024000UL
#define NRF_PDM0_NS_APP_BASE_REAL_HW            0x40026000UL
#define NRF_PDM0_S_APP_BASE_REAL_HW             0x50026000UL
#define NRF_I2S0_NS_APP_BASE_REAL_HW            0x40028000UL
#define NRF_I2S0_S_APP_BASE_REAL_HW             0x50028000UL
#define NRF_IPC_NS_APP_BASE_REAL_HW             0x4002A000UL
#define NRF_IPC_S_APP_BASE_REAL_HW              0x5002A000UL
#define NRF_QSPI_NS_APP_BASE_REAL_HW            0x4002B000UL
#define NRF_QSPI_S_APP_BASE_REAL_HW             0x5002B000UL
#define NRF_NFCT_NS_APP_BASE_REAL_HW            0x4002D000UL
#define NRF_NFCT_S_APP_BASE_REAL_HW             0x5002D000UL
#define NRF_GPIOTE1_NS_APP_BASE_REAL_HW         0x4002F000UL
#define NRF_MUTEX_NS_APP_BASE_REAL_HW           0x40030000UL
#define NRF_MUTEX_S_APP_BASE_REAL_HW            0x50030000UL
#define NRF_QDEC0_NS_APP_BASE_REAL_HW           0x40033000UL
#define NRF_QDEC0_S_APP_BASE_REAL_HW            0x50033000UL
#define NRF_QDEC1_NS_APP_BASE_REAL_HW           0x40034000UL
#define NRF_QDEC1_S_APP_BASE_REAL_HW            0x50034000UL
#define NRF_USBD_NS_APP_BASE_REAL_HW            0x40036000UL
#define NRF_USBD_S_APP_BASE_REAL_HW             0x50036000UL
#define NRF_USBREGULATOR_NS_APP_BASE_REAL_HW    0x40037000UL
#define NRF_USBREGULATOR_S_APP_BASE_REAL_HW     0x50037000UL
#define NRF_KMU_NS_APP_BASE_REAL_HW             0x40039000UL
#define NRF_NVMC_NS_APP_BASE_REAL_HW            0x40039000UL
#define NRF_KMU_S_APP_BASE_REAL_HW              0x50039000UL
#define NRF_NVMC_S_APP_BASE_REAL_HW             0x50039000UL
#define NRF_P0_NS_APP_BASE_REAL_HW              0x40842500UL
#define NRF_P1_NS_APP_BASE_REAL_HW              0x40842800UL
#define NRF_P0_S_APP_BASE_REAL_HW               0x50842500UL
#define NRF_P1_S_APP_BASE_REAL_HW               0x50842800UL
#define NRF_CRYPTOCELL_S_APP_BASE_REAL_HW       0x50844000UL
#define NRF_CC_AES_S_APP_BASE_REAL_HW           0x50845000UL
#define NRF_CC_AHB_S_APP_BASE_REAL_HW           0x50845000UL
#define NRF_CC_AO_S_APP_BASE_REAL_HW            0x50845000UL
#define NRF_CC_CHACHA_S_APP_BASE_REAL_HW        0x50845000UL
#define NRF_CC_CTL_S_APP_BASE_REAL_HW           0x50845000UL
#define NRF_CC_DIN_S_APP_BASE_REAL_HW           0x50845000UL
#define NRF_CC_DOUT_S_APP_BASE_REAL_HW          0x50845000UL
#define NRF_CC_GHASH_S_APP_BASE_REAL_HW         0x50845000UL
#define NRF_CC_HASH_S_APP_BASE_REAL_HW          0x50845000UL
#define NRF_CC_HOST_RGF_S_APP_BASE_REAL_HW      0x50845000UL
#define NRF_CC_MISC_S_APP_BASE_REAL_HW          0x50845000UL
#define NRF_CC_PKA_S_APP_BASE_REAL_HW           0x50845000UL
#define NRF_CC_RNG_S_APP_BASE_REAL_HW           0x50845000UL
#define NRF_CC_RNG_SRAM_S_APP_BASE_REAL_HW      0x50845000UL
#define NRF_VMC_NS_APP_BASE_REAL_HW             0x40081000UL
#define NRF_VMC_S_APP_BASE_REAL_HW              0x50081000UL

#if defined(NRF5340_XXAA_NETWORK)

extern void *NRF_FICR_regs_p[];
#undef NRF_FICR_NS_BASE
#define NRF_FICR_NS_BASE           (NRF_FICR_regs_p[NHW_FICR_NET])
extern NRF_UICR_Type *NRF_UICR_regs_p[];
#undef NRF_UICR_NS_BASE
#define NRF_UICR_NS_BASE           (NRF_UICR_regs_p[NHW_UICR_NET0])
#undef NRF_CTI_NS_BASE
#define NRF_CTI_NS_BASE            NULL
#undef NRF_DCNF_NS_BASE
#define NRF_DCNF_NS_BASE           NULL
extern NRF_VREQCTRL_Type NRF_VREQCTRL_regs;
#undef NRF_VREQCTRL_NS_BASE
#define NRF_VREQCTRL_NS_BASE       (&NRF_VREQCTRL_regs)
extern NRF_CLOCK_Type *NRF_CLOCK_regs[];
#undef NRF_CLOCK_NS_BASE
#define NRF_CLOCK_NS_BASE          (NRF_CLOCK_regs[NHW_CLKPWR_NET0])
extern NRF_POWER_Type *NRF_POWER_regs[];
#undef NRF_POWER_NS_BASE
#define NRF_POWER_NS_BASE          (NRF_POWER_regs[NHW_CLKPWR_NET0])
extern NRF_RESET_Type *NRF_RESET_regs[];
#undef NRF_RESET_NS_BASE
#define NRF_RESET_NS_BASE          (NRF_RESET_regs[NHW_CLKPWR_NET0])
#undef NRF_CTRLAP_NS_BASE
#define NRF_CTRLAP_NS_BASE         NULL
extern NRF_RADIO_Type NRF_RADIO_regs;
#undef NRF_RADIO_NS_BASE
#define NRF_RADIO_NS_BASE          (&NRF_RADIO_regs)
extern NRF_RNG_Type NRF_RNG_regs;
#undef NRF_RNG_NS_BASE
#define NRF_RNG_NS_BASE            (&NRF_RNG_regs)
extern NRF_GPIOTE_Type NRF_GPIOTE_regs[];
#undef NRF_GPIOTE_NS_BASE
#define NRF_GPIOTE_NS_BASE         (&NRF_GPIOTE_regs[NHW_GPIOTE_NET])
#undef NRF_WDT_NS_BASE
#define NRF_WDT_NS_BASE            NULL
#undef NRF_ECB_NS_BASE
extern NRF_ECB_Type NRF_ECB_regs;
#define NRF_ECB_NS_BASE            (&NRF_ECB_regs)
extern NRF_AAR_Type NRF_AAR_regs;
#undef NRF_AAR_NS_BASE
#define NRF_AAR_NS_BASE            (&NRF_AAR_regs)
extern NRF_CCM_Type NRF_CCM_regs;
#undef NRF_CCM_NS_BASE
#define NRF_CCM_NS_BASE            (&NRF_CCM_regs)
extern NRF_DPPIC_Type NRF_DPPIC_regs[];
#undef NRF_DPPIC_NS_BASE
#define NRF_DPPIC_NS_BASE          (&NRF_DPPIC_regs[NHW_DPPI_NET_0])
extern NRF_TEMP_Type NRF_TEMP_regs;
#undef NRF_TEMP_NS_BASE
#define NRF_TEMP_NS_BASE           (&NRF_TEMP_regs)
extern NRF_RTC_Type NRF_RTC_regs[];
#undef NRF_RTC0_NS_BASE
#define NRF_RTC0_NS_BASE           (&NRF_RTC_regs[NHW_RTC_NET0])
#undef NRF_RTC1_NS_BASE
#define NRF_RTC1_NS_BASE           (&NRF_RTC_regs[NHW_RTC_NET1])
extern NRF_IPC_Type NRF_IPC_regs[NHW_IPC_TOTAL_INST];
#undef NRF_IPC_NS_BASE
#define NRF_IPC_NS_BASE            (&NRF_IPC_regs[NHW_IPC_NET0])
#undef NRF_SPIM0_NS_BASE
#define NRF_SPIM0_NS_BASE          NULL
#undef NRF_SPIS0_NS_BASE
#define NRF_SPIS0_NS_BASE          NULL
#undef NRF_TWIM0_NS_BASE
#define NRF_TWIM0_NS_BASE          NULL
#undef NRF_TWIS0_NS_BASE
#define NRF_TWIS0_NS_BASE          NULL
extern NRF_UARTE_Type NRF_UARTE_regs[];
#undef NRF_UARTE0_NS_BASE
#define NRF_UARTE0_NS_BASE         (&NRF_UARTE_regs[NHW_UARTE_NET0])
extern NRF_EGU_Type NRF_EGU_regs[];
#undef NRF_EGU0_NS_BASE
#define NRF_EGU0_NS_BASE           (&NRF_EGU_regs[NHW_EGU_NET0])
extern NRF_TIMER_Type NRF_TIMER_regs[];
#undef NRF_TIMER0_NS_BASE
#define NRF_TIMER0_NS_BASE         (&NRF_TIMER_regs[NHW_TIMER_NET0])
#undef NRF_TIMER1_NS_BASE
#define NRF_TIMER1_NS_BASE         (&NRF_TIMER_regs[NHW_TIMER_NET1])
#undef NRF_TIMER2_NS_BASE
#define NRF_TIMER2_NS_BASE         (&NRF_TIMER_regs[NHW_TIMER_NET2])
extern int NRF_SWI_regs[];
#undef NRF_SWI0_NS_BASE
#define NRF_SWI0_NS_BASE           (&NRF_SWI_regs[NHW_SWI_NET0])
#undef NRF_SWI1_NS_BASE
#define NRF_SWI1_NS_BASE            (&NRF_SWI_regs[NHW_SWI_NET1])
#undef NRF_SWI2_NS_BASE
#define NRF_SWI2_NS_BASE            (&NRF_SWI_regs[NHW_SWI_NET2])
#undef NRF_SWI3_NS_BASE
#define NRF_SWI3_NS_BASE            (&NRF_SWI_regs[NHW_SWI_NET3])
extern NRF_MUTEX_Type NRF_MUTEX_regs;
#undef NRF_APPMUTEX_NS_BASE
#define NRF_APPMUTEX_NS_BASE       (&NRF_MUTEX_regs)
#undef NRF_APPMUTEX_S_BASE
#define NRF_APPMUTEX_S_BASE        (&NRF_MUTEX_regs)
#undef NRF_ACL_NS_BASE
#define NRF_ACL_NS_BASE            NULL
extern NRF_NVMC_Type *NRF_NVMC_regs_p[];
#undef NRF_NVMC_NS_BASE
#define NRF_NVMC_NS_BASE           (NRF_NVMC_regs_p[NHW_NVMC_NET0])
#undef NRF_VMC_NS_BASE
#define NRF_VMC_NS_BASE            NULL
extern NRF_GPIO_Type NRF_GPIO_regs[];
#undef NRF_P0_NS_BASE
#define NRF_P0_NS_BASE             (&NRF_GPIO_regs[NHW_GPIO_NET_P0])
#undef NRF_P1_NS_BASE
#define NRF_P1_NS_BASE             (&NRF_GPIO_regs[NHW_GPIO_NET_P1])

/********************************************************************/
/********************************************************************/
/********************************************************************/
#elif defined(NRF5340_XXAA_APPLICATION)

#undef NRF_CACHEDATA_S_BASE
#define NRF_CACHEDATA_S_BASE        NULL
#undef NRF_CACHEINFO_S_BASE
#define NRF_CACHEINFO_S_BASE        NULL
extern void *NRF_FICR_regs_p[];
#undef NRF_FICR_S_BASE
#define NRF_FICR_S_BASE             (NRF_FICR_regs_p[NHW_FICR_APP])
extern NRF_UICR_Type *NRF_UICR_regs_p[];
#undef NRF_UICR_S_BASE
#define NRF_UICR_S_BASE             (NRF_UICR_regs_p[NHW_UICR_APP0])
#undef NRF_CTI_S_BASE
#define NRF_CTI_S_BASE              NULL
#undef NRF_TAD_S_BASE
#define NRF_TAD_S_BASE              NULL
#undef NRF_DCNF_NS_BASE
#define NRF_DCNF_NS_BASE            NULL
#undef NRF_FPU_NS_BASE
#define NRF_FPU_NS_BASE             NULL
#undef NRF_DCNF_S_BASE
#define NRF_DCNF_S_BASE             NULL
#undef NRF_FPU_S_BASE
#define NRF_FPU_S_BASE              NULL
#undef NRF_CACHE_S_BASE
#define NRF_CACHE_S_BASE            NULL
extern NRF_SPU_Type NRF_SPU_regs[];
#undef NRF_SPU_S_BASE
#define NRF_SPU_S_BASE              (&NRF_SPU_regs[NHW_SPU_APP0])
#undef NRF_OSCILLATORS_NS_BASE
#define NRF_OSCILLATORS_NS_BASE     NULL
#undef NRF_REGULATORS_NS_BASE
#define NRF_REGULATORS_NS_BASE      NULL
#undef NRF_OSCILLATORS_S_BASE
#define NRF_OSCILLATORS_S_BASE      NULL
#undef NRF_REGULATORS_S_BASE
#define NRF_REGULATORS_S_BASE       NULL
extern NRF_CLOCK_Type *NRF_CLOCK_regs[];
#undef NRF_CLOCK_NS_BASE
#define NRF_CLOCK_NS_BASE          (NRF_CLOCK_regs[NHW_CLKPWR_APP0])
extern NRF_POWER_Type *NRF_POWER_regs[];
#undef NRF_POWER_NS_BASE
#define NRF_POWER_NS_BASE          (NRF_POWER_regs[NHW_CLKPWR_APP0])
extern NRF_RESET_Type *NRF_RESET_regs[];
#undef NRF_RESET_NS_BASE
#define NRF_RESET_NS_BASE          (NRF_RESET_regs[NHW_CLKPWR_APP0])
#undef NRF_CLOCK_S_BASE
#define NRF_CLOCK_S_BASE           (NRF_CLOCK_regs[NHW_CLKPWR_APP0])
#undef NRF_POWER_S_BASE
#define NRF_POWER_S_BASE           (NRF_POWER_regs[NHW_CLKPWR_APP0])
#undef NRF_RESET_S_BASE
#define NRF_RESET_S_BASE           (NRF_RESET_regs[NHW_CLKPWR_APP0])
#undef NRF_CTRLAP_NS_BASE
#define NRF_CTRLAP_NS_BASE          NULL
#undef NRF_CTRLAP_S_BASE
#define NRF_CTRLAP_S_BASE           NULL
#undef NRF_SPIM0_NS_BASE
#define NRF_SPIM0_NS_BASE           NULL
#undef NRF_SPIS0_NS_BASE
#define NRF_SPIS0_NS_BASE           NULL
#undef NRF_TWIM0_NS_BASE
#define NRF_TWIM0_NS_BASE           NULL
#undef NRF_TWIS0_NS_BASE
#define NRF_TWIS0_NS_BASE           NULL
extern NRF_UARTE_Type NRF_UARTE_regs[];
#undef NRF_UARTE0_NS_BASE
#define NRF_UARTE0_NS_BASE          (&NRF_UARTE_regs[NHW_UARTE_APP0])
#undef NRF_SPIM0_S_BASE
#define NRF_SPIM0_S_BASE            NULL
#undef NRF_SPIS0_S_BASE
#define NRF_SPIS0_S_BASE            NULL
#undef NRF_TWIM0_S_BASE
#define NRF_TWIM0_S_BASE            NULL
#undef NRF_TWIS0_S_BASE
#define NRF_TWIS0_S_BASE            NULL
#undef NRF_UARTE0_S_BASE
#define NRF_UARTE0_S_BASE          (&NRF_UARTE_regs[NHW_UARTE_APP0])
#undef NRF_SPIM1_NS_BASE
#define NRF_SPIM1_NS_BASE           NULL
#undef NRF_SPIS1_NS_BASE
#define NRF_SPIS1_NS_BASE           NULL
#undef NRF_TWIM1_NS_BASE
#define NRF_TWIM1_NS_BASE           NULL
#undef NRF_TWIS1_NS_BASE
#define NRF_TWIS1_NS_BASE           NULL
#undef NRF_UARTE1_NS_BASE
#define NRF_UARTE1_NS_BASE          (&NRF_UARTE_regs[NHW_UARTE_APP1])
#undef NRF_SPIM1_S_BASE
#define NRF_SPIM1_S_BASE            NULL
#undef NRF_SPIS1_S_BASE
#define NRF_SPIS1_S_BASE            NULL
#undef NRF_TWIM1_S_BASE
#define NRF_TWIM1_S_BASE            NULL
#undef NRF_TWIS1_S_BASE
#define NRF_TWIS1_S_BASE            NULL
#undef NRF_UARTE1_S_BASE
#define NRF_UARTE1_S_BASE           (&NRF_UARTE_regs[NHW_UARTE_APP1])
#undef NRF_SPIM4_NS_BASE
#define NRF_SPIM4_NS_BASE           NULL
#undef NRF_SPIM4_S_BASE
#define NRF_SPIM4_S_BASE            NULL
#undef NRF_SPIM2_NS_BASE
#define NRF_SPIM2_NS_BASE           NULL
#undef NRF_SPIS2_NS_BASE
#define NRF_SPIS2_NS_BASE           NULL
#undef NRF_TWIM2_NS_BASE
#define NRF_TWIM2_NS_BASE           NULL
#undef NRF_TWIS2_NS_BASE
#define NRF_TWIS2_NS_BASE           NULL
#undef NRF_UARTE2_NS_BASE
#define NRF_UARTE2_NS_BASE          (&NRF_UARTE_regs[NHW_UARTE_APP2])
#undef NRF_SPIM2_S_BASE
#define NRF_SPIM2_S_BASE            NULL
#undef NRF_SPIS2_S_BASE
#define NRF_SPIS2_S_BASE            NULL
#undef NRF_TWIM2_S_BASE
#define NRF_TWIM2_S_BASE            NULL
#undef NRF_TWIS2_S_BASE
#define NRF_TWIS2_S_BASE            NULL
#undef NRF_UARTE2_S_BASE
#define NRF_UARTE2_S_BASE           (&NRF_UARTE_regs[NHW_UARTE_APP2])
#undef NRF_SPIM3_NS_BASE
#define NRF_SPIM3_NS_BASE           NULL
#undef NRF_SPIS3_NS_BASE
#define NRF_SPIS3_NS_BASE           NULL
#undef NRF_TWIM3_NS_BASE
#define NRF_TWIM3_NS_BASE           NULL
#undef NRF_TWIS3_NS_BASE
#define NRF_TWIS3_NS_BASE           NULL
#undef NRF_UARTE3_NS_BASE
#define NRF_UARTE3_NS_BASE          (&NRF_UARTE_regs[NHW_UARTE_APP3])
#undef NRF_SPIM3_S_BASE
#define NRF_SPIM3_S_BASE            NULL
#undef NRF_SPIS3_S_BASE
#define NRF_SPIS3_S_BASE            NULL
#undef NRF_TWIM3_S_BASE
#define NRF_TWIM3_S_BASE            NULL
#undef NRF_TWIS3_S_BASE
#define NRF_TWIS3_S_BASE            NULL
#undef NRF_UARTE3_S_BASE
#define NRF_UARTE3_S_BASE           (&NRF_UARTE_regs[NHW_UARTE_APP3])
extern NRF_GPIOTE_Type NRF_GPIOTE_regs[];
#undef NRF_GPIOTE0_S_BASE
#define NRF_GPIOTE0_S_BASE          (&NRF_GPIOTE_regs[NHW_GPIOTE_APP0])
#undef NRF_SAADC_NS_BASE
#define NRF_SAADC_NS_BASE           NULL
#undef NRF_SAADC_S_BASE
#define NRF_SAADC_S_BASE            NULL
extern NRF_TIMER_Type NRF_TIMER_regs[];
#undef NRF_TIMER0_NS_BASE
#define NRF_TIMER0_NS_BASE          (&NRF_TIMER_regs[NHW_TIMER_APP0])
#undef NRF_TIMER0_S_BASE
#define NRF_TIMER0_S_BASE           (&NRF_TIMER_regs[NHW_TIMER_APP0])
#undef NRF_TIMER1_NS_BASE
#define NRF_TIMER1_NS_BASE          (&NRF_TIMER_regs[NHW_TIMER_APP1])
#undef NRF_TIMER1_S_BASE
#define NRF_TIMER1_S_BASE           (&NRF_TIMER_regs[NHW_TIMER_APP1])
#undef NRF_TIMER2_NS_BASE
#define NRF_TIMER2_NS_BASE          (&NRF_TIMER_regs[NHW_TIMER_APP2])
#undef NRF_TIMER2_S_BASE
#define NRF_TIMER2_S_BASE           (&NRF_TIMER_regs[NHW_TIMER_APP2])
extern NRF_RTC_Type NRF_RTC_regs[];
#undef NRF_RTC0_NS_BASE
#define NRF_RTC0_NS_BASE            (&NRF_RTC_regs[NHW_RTC_APP0])
#undef NRF_RTC0_S_BASE
#define NRF_RTC0_S_BASE             (&NRF_RTC_regs[NHW_RTC_APP0])
#undef NRF_RTC1_NS_BASE
#define NRF_RTC1_NS_BASE            (&NRF_RTC_regs[NHW_RTC_APP1])
#undef NRF_RTC1_S_BASE
#define NRF_RTC1_S_BASE             (&NRF_RTC_regs[NHW_RTC_APP1])
extern NRF_DPPIC_Type NRF_DPPIC_regs[];
#undef NRF_DPPIC_NS_BASE
#define NRF_DPPIC_NS_BASE           (&NRF_DPPIC_regs[NHW_DPPI_APP_0])
#undef NRF_DPPIC_S_BASE
#define NRF_DPPIC_S_BASE            (&NRF_DPPIC_regs[NHW_DPPI_APP_0])
#undef NRF_WDT0_NS_BASE
#define NRF_WDT0_NS_BASE            NULL
#undef NRF_WDT0_S_BASE
#define NRF_WDT0_S_BASE             NULL
#undef NRF_WDT1_NS_BASE
#define NRF_WDT1_NS_BASE            NULL
#undef NRF_WDT1_S_BASE
#define NRF_WDT1_S_BASE             NULL
#undef NRF_COMP_NS_BASE
#define NRF_COMP_NS_BASE            NULL
#undef NRF_LPCOMP_NS_BASE
#define NRF_LPCOMP_NS_BASE          NULL
#undef NRF_COMP_S_BASE
#define NRF_COMP_S_BASE             NULL
#undef NRF_LPCOMP_S_BASE
#define NRF_LPCOMP_S_BASE           NULL
extern NRF_EGU_Type NRF_EGU_regs[];
#undef NRF_EGU0_NS_BASE
#define NRF_EGU0_NS_BASE           (&NRF_EGU_regs[NHW_EGU_APP0])
#undef NRF_EGU0_S_BASE
#define NRF_EGU0_S_BASE            (&NRF_EGU_regs[NHW_EGU_APP0])
#undef NRF_EGU1_NS_BASE
#define NRF_EGU1_NS_BASE           (&NRF_EGU_regs[NHW_EGU_APP1])
#undef NRF_EGU1_S_BASE
#define NRF_EGU1_S_BASE            (&NRF_EGU_regs[NHW_EGU_APP1])
#undef NRF_EGU2_NS_BASE
#define NRF_EGU2_NS_BASE           (&NRF_EGU_regs[NHW_EGU_APP2])
#undef NRF_EGU2_S_BASE
#define NRF_EGU2_S_BASE            (&NRF_EGU_regs[NHW_EGU_APP2])
#undef NRF_EGU3_NS_BASE
#define NRF_EGU3_NS_BASE           (&NRF_EGU_regs[NHW_EGU_APP3])
#undef NRF_EGU3_S_BASE
#define NRF_EGU3_S_BASE            (&NRF_EGU_regs[NHW_EGU_APP3])
#undef NRF_EGU4_NS_BASE
#define NRF_EGU4_NS_BASE           (&NRF_EGU_regs[NHW_EGU_APP4])
#undef NRF_EGU4_S_BASE
#define NRF_EGU4_S_BASE            (&NRF_EGU_regs[NHW_EGU_APP4])
#undef NRF_EGU5_NS_BASE
#define NRF_EGU5_NS_BASE           (&NRF_EGU_regs[NHW_EGU_APP5])
#undef NRF_EGU5_S_BASE
#define NRF_EGU5_S_BASE            (&NRF_EGU_regs[NHW_EGU_APP5])
#undef NRF_PWM0_NS_BASE
#define NRF_PWM0_NS_BASE            NULL
#undef NRF_PWM0_S_BASE
#define NRF_PWM0_S_BASE             NULL
#undef NRF_PWM1_NS_BASE
#define NRF_PWM1_NS_BASE            NULL
#undef NRF_PWM1_S_BASE
#define NRF_PWM1_S_BASE             NULL
#undef NRF_PWM2_NS_BASE
#define NRF_PWM2_NS_BASE            NULL
#undef NRF_PWM2_S_BASE
#define NRF_PWM2_S_BASE             NULL
#undef NRF_PWM3_NS_BASE
#define NRF_PWM3_NS_BASE            NULL
#undef NRF_PWM3_S_BASE
#define NRF_PWM3_S_BASE             NULL
#undef NRF_PDM0_NS_BASE
#define NRF_PDM0_NS_BASE            NULL
#undef NRF_PDM0_S_BASE
#define NRF_PDM0_S_BASE             NULL
#undef NRF_I2S0_NS_BASE
#define NRF_I2S0_NS_BASE            NULL
#undef NRF_I2S0_S_BASE
#define NRF_I2S0_S_BASE             NULL
extern NRF_IPC_Type NRF_IPC_regs[NHW_IPC_TOTAL_INST];
#undef NRF_IPC_NS_BASE
#define NRF_IPC_NS_BASE             (&NRF_IPC_regs[NHW_IPC_APP0])
#undef NRF_IPC_S_BASE
#define NRF_IPC_S_BASE              (&NRF_IPC_regs[NHW_IPC_APP0])
#undef NRF_QSPI_NS_BASE
#define NRF_QSPI_NS_BASE            NULL
#undef NRF_QSPI_S_BASE
#define NRF_QSPI_S_BASE             NULL
extern NRF_NFCT_Type NRF_NFCT_regs;
#undef NRF_NFCT_NS_BASE
#define NRF_NFCT_NS_BASE            (&NRF_NFCT_regs)
#undef NRF_NFCT_S_BASE
#define NRF_NFCT_S_BASE             (&NRF_NFCT_regs)
#undef NRF_GPIOTE1_NS_BASE
#define NRF_GPIOTE1_NS_BASE         (&NRF_GPIOTE_regs[NHW_GPIOTE_APP1])
extern NRF_MUTEX_Type NRF_MUTEX_regs;
#undef NRF_MUTEX_NS_BASE
#define NRF_MUTEX_NS_BASE           (&NRF_MUTEX_regs)
#undef NRF_MUTEX_S_BASE
#define NRF_MUTEX_S_BASE            (&NRF_MUTEX_regs)
#undef NRF_QDEC0_NS_BASE
#define NRF_QDEC0_NS_BASE           NULL
#undef NRF_QDEC0_S_BASE
#define NRF_QDEC0_S_BASE            NULL
#undef NRF_QDEC1_NS_BASE
#define NRF_QDEC1_NS_BASE           NULL
#undef NRF_QDEC1_S_BASE
#define NRF_QDEC1_S_BASE            NULL
#undef NRF_USBD_NS_BASE
#define NRF_USBD_NS_BASE            NULL
#undef NRF_USBD_S_BASE
#define NRF_USBD_S_BASE             NULL
#undef NRF_USBREGULATOR_NS_BASE
#define NRF_USBREGULATOR_NS_BASE    NULL
#undef NRF_USBREGULATOR_S_BASE
#define NRF_USBREGULATOR_S_BASE     NULL
#undef NRF_KMU_NS_BASE
#define NRF_KMU_NS_BASE             NULL
extern NRF_NVMC_Type *NRF_NVMC_regs_p[];
#undef NRF_NVMC_NS_BASE
#define NRF_NVMC_NS_BASE            (NRF_NVMC_regs_p[NHW_NVMC_APP0])
#undef NRF_KMU_S_BASE
#define NRF_KMU_S_BASE              NULL
#undef NRF_NVMC_S_BASE
#define NRF_NVMC_S_BASE             (NRF_NVMC_regs_p[NHW_NVMC_APP0])
extern NRF_GPIO_Type NRF_GPIO_regs[];
#undef NRF_P0_NS_BASE
#define NRF_P0_NS_BASE              (&NRF_GPIO_regs[NHW_GPIO_APP_P0])
#undef NRF_P1_NS_BASE
#define NRF_P1_NS_BASE              (&NRF_GPIO_regs[NHW_GPIO_APP_P1])
#undef NRF_P0_S_BASE
#define NRF_P0_S_BASE               (&NRF_GPIO_regs[NHW_GPIO_APP_P0])
#undef NRF_P1_S_BASE
#define NRF_P1_S_BASE               (&NRF_GPIO_regs[NHW_GPIO_APP_P1])
#undef NRF_CRYPTOCELL_S_BASE
#define NRF_CRYPTOCELL_S_BASE       NULL
#undef NRF_VMC_NS_BASE
#define NRF_VMC_NS_BASE             NULL
#undef NRF_VMC_S_BASE
#define NRF_VMC_S_BASE              NULL

#endif //defined(NRF5340_XXAA_NETWORK) || defined(NRF5340_XXAA_APPLICATION)

/********************************************************************/
/********************************************************************/
/********************************************************************/
#elif defined(NRF54L15_XXAA)

#define NRF_FICR_NS_BASE_REAL_HW                  0x00FFC000UL
#define NRF_UICR_S_BASE_REAL_HW                   0x00FFD000UL
#define NRF_SICR_S_BASE_REAL_HW                   0x00FFE000UL
#define NRF_CRACENCORE_S_BASE_REAL_HW             0x51800000UL
#define NRF_SPU00_S_BASE_REAL_HW                  0x50040000UL
#define NRF_MPC00_S_BASE_REAL_HW                  0x50041000UL
#define NRF_DPPIC00_NS_BASE_REAL_HW               0x40042000UL
#define NRF_DPPIC00_S_BASE_REAL_HW                0x50042000UL
#define NRF_PPIB00_NS_BASE_REAL_HW                0x40043000UL
#define NRF_PPIB00_S_BASE_REAL_HW                 0x50043000UL
#define NRF_PPIB01_NS_BASE_REAL_HW                0x40044000UL
#define NRF_PPIB01_S_BASE_REAL_HW                 0x50044000UL
#define NRF_KMU_S_BASE_REAL_HW                    0x50045000UL
#define NRF_AAR00_NS_BASE_REAL_HW                 0x40046000UL
#define NRF_CCM00_NS_BASE_REAL_HW                 0x40046000UL
#define NRF_AAR00_S_BASE_REAL_HW                  0x50046000UL
#define NRF_CCM00_S_BASE_REAL_HW                  0x50046000UL
#define NRF_ECB00_NS_BASE_REAL_HW                 0x40047000UL
#define NRF_ECB00_S_BASE_REAL_HW                  0x50047000UL
#define NRF_CRACEN_S_BASE_REAL_HW                 0x50048000UL
#define NRF_SPIM00_NS_BASE_REAL_HW                0x4004A000UL
#define NRF_SPIS00_NS_BASE_REAL_HW                0x4004A000UL
#define NRF_UARTE00_NS_BASE_REAL_HW               0x4004A000UL
#define NRF_SPIM00_S_BASE_REAL_HW                 0x5004A000UL
#define NRF_SPIS00_S_BASE_REAL_HW                 0x5004A000UL
#define NRF_UARTE00_S_BASE_REAL_HW                0x5004A000UL
#define NRF_GLITCHDET_S_BASE_REAL_HW              0x5004B000UL
#define NRF_RRAMC_S_BASE_REAL_HW                  0x5004B000UL
#define NRF_VPR00_NS_BASE_REAL_HW                 0x4004C000UL
#define NRF_VPR00_S_BASE_REAL_HW                  0x5004C000UL
#define NRF_P2_NS_BASE_REAL_HW                    0x40050400UL
#define NRF_GPIOHSPADCTRL_S_BASE_REAL_HW          0x50050400UL
#define NRF_P2_S_BASE_REAL_HW                     0x50050400UL
#define NRF_CTRLAP_NS_BASE_REAL_HW                0x40052000UL
#define NRF_CTRLAP_S_BASE_REAL_HW                 0x50052000UL
#define NRF_TAD_NS_BASE_REAL_HW                   0x40053000UL
#define NRF_TAD_S_BASE_REAL_HW                    0x50053000UL
#define NRF_TIMER00_NS_BASE_REAL_HW               0x40055000UL
#define NRF_TIMER00_S_BASE_REAL_HW                0x50055000UL
#define NRF_SPU10_S_BASE_REAL_HW                  0x50080000UL
#define NRF_DPPIC10_NS_BASE_REAL_HW               0x40082000UL
#define NRF_DPPIC10_S_BASE_REAL_HW                0x50082000UL
#define NRF_PPIB10_NS_BASE_REAL_HW                0x40083000UL
#define NRF_PPIB10_S_BASE_REAL_HW                 0x50083000UL
#define NRF_PPIB11_NS_BASE_REAL_HW                0x40084000UL
#define NRF_PPIB11_S_BASE_REAL_HW                 0x50084000UL
#define NRF_TIMER10_NS_BASE_REAL_HW               0x40085000UL
#define NRF_TIMER10_S_BASE_REAL_HW                0x50085000UL
#define NRF_EGU10_NS_BASE_REAL_HW                 0x40087000UL
#define NRF_EGU10_S_BASE_REAL_HW                  0x50087000UL
#define NRF_RADIO_NS_BASE_REAL_HW                 0x4008A000UL
#define NRF_RADIO_S_BASE_REAL_HW                  0x5008A000UL
#define NRF_SPU20_S_BASE_REAL_HW                  0x500C0000UL
#define NRF_DPPIC20_NS_BASE_REAL_HW               0x400C2000UL
#define NRF_DPPIC20_S_BASE_REAL_HW                0x500C2000UL
#define NRF_PPIB20_NS_BASE_REAL_HW                0x400C3000UL
#define NRF_PPIB20_S_BASE_REAL_HW                 0x500C3000UL
#define NRF_PPIB21_NS_BASE_REAL_HW                0x400C4000UL
#define NRF_PPIB21_S_BASE_REAL_HW                 0x500C4000UL
#define NRF_PPIB22_NS_BASE_REAL_HW                0x400C5000UL
#define NRF_PPIB22_S_BASE_REAL_HW                 0x500C5000UL
#define NRF_SPIM20_NS_BASE_REAL_HW                0x400C6000UL
#define NRF_SPIS20_NS_BASE_REAL_HW                0x400C6000UL
#define NRF_TWIM20_NS_BASE_REAL_HW                0x400C6000UL
#define NRF_TWIS20_NS_BASE_REAL_HW                0x400C6000UL
#define NRF_UARTE20_NS_BASE_REAL_HW               0x400C6000UL
#define NRF_SPIM20_S_BASE_REAL_HW                 0x500C6000UL
#define NRF_SPIS20_S_BASE_REAL_HW                 0x500C6000UL
#define NRF_TWIM20_S_BASE_REAL_HW                 0x500C6000UL
#define NRF_TWIS20_S_BASE_REAL_HW                 0x500C6000UL
#define NRF_UARTE20_S_BASE_REAL_HW                0x500C6000UL
#define NRF_SPIM21_NS_BASE_REAL_HW                0x400C7000UL
#define NRF_SPIS21_NS_BASE_REAL_HW                0x400C7000UL
#define NRF_TWIM21_NS_BASE_REAL_HW                0x400C7000UL
#define NRF_TWIS21_NS_BASE_REAL_HW                0x400C7000UL
#define NRF_UARTE21_NS_BASE_REAL_HW               0x400C7000UL
#define NRF_SPIM21_S_BASE_REAL_HW                 0x500C7000UL
#define NRF_SPIS21_S_BASE_REAL_HW                 0x500C7000UL
#define NRF_TWIM21_S_BASE_REAL_HW                 0x500C7000UL
#define NRF_TWIS21_S_BASE_REAL_HW                 0x500C7000UL
#define NRF_UARTE21_S_BASE_REAL_HW                0x500C7000UL
#define NRF_SPIM22_NS_BASE_REAL_HW                0x400C8000UL
#define NRF_SPIS22_NS_BASE_REAL_HW                0x400C8000UL
#define NRF_TWIM22_NS_BASE_REAL_HW                0x400C8000UL
#define NRF_TWIS22_NS_BASE_REAL_HW                0x400C8000UL
#define NRF_UARTE22_NS_BASE_REAL_HW               0x400C8000UL
#define NRF_SPIM22_S_BASE_REAL_HW                 0x500C8000UL
#define NRF_SPIS22_S_BASE_REAL_HW                 0x500C8000UL
#define NRF_TWIM22_S_BASE_REAL_HW                 0x500C8000UL
#define NRF_TWIS22_S_BASE_REAL_HW                 0x500C8000UL
#define NRF_UARTE22_S_BASE_REAL_HW                0x500C8000UL
#define NRF_EGU20_NS_BASE_REAL_HW                 0x400C9000UL
#define NRF_EGU20_S_BASE_REAL_HW                  0x500C9000UL
#define NRF_TIMER20_NS_BASE_REAL_HW               0x400CA000UL
#define NRF_TIMER20_S_BASE_REAL_HW                0x500CA000UL
#define NRF_TIMER21_NS_BASE_REAL_HW               0x400CB000UL
#define NRF_TIMER21_S_BASE_REAL_HW                0x500CB000UL
#define NRF_TIMER22_NS_BASE_REAL_HW               0x400CC000UL
#define NRF_TIMER22_S_BASE_REAL_HW                0x500CC000UL
#define NRF_TIMER23_NS_BASE_REAL_HW               0x400CD000UL
#define NRF_TIMER23_S_BASE_REAL_HW                0x500CD000UL
#define NRF_TIMER24_NS_BASE_REAL_HW               0x400CE000UL
#define NRF_TIMER24_S_BASE_REAL_HW                0x500CE000UL
#define NRF_MEMCONF_NS_BASE_REAL_HW               0x400CF000UL
#define NRF_MEMCONF_S_BASE_REAL_HW                0x500CF000UL
#define NRF_PDM20_NS_BASE_REAL_HW                 0x400D0000UL
#define NRF_PDM20_S_BASE_REAL_HW                  0x500D0000UL
#define NRF_PDM21_NS_BASE_REAL_HW                 0x400D1000UL
#define NRF_PDM21_S_BASE_REAL_HW                  0x500D1000UL
#define NRF_PWM20_NS_BASE_REAL_HW                 0x400D2000UL
#define NRF_PWM20_S_BASE_REAL_HW                  0x500D2000UL
#define NRF_PWM21_NS_BASE_REAL_HW                 0x400D3000UL
#define NRF_PWM21_S_BASE_REAL_HW                  0x500D3000UL
#define NRF_PWM22_NS_BASE_REAL_HW                 0x400D4000UL
#define NRF_PWM22_S_BASE_REAL_HW                  0x500D4000UL
#define NRF_SAADC_NS_BASE_REAL_HW                 0x400D5000UL
#define NRF_SAADC_S_BASE_REAL_HW                  0x500D5000UL
#define NRF_NFCT_NS_BASE_REAL_HW                  0x400D6000UL
#define NRF_NFCT_S_BASE_REAL_HW                   0x500D6000UL
#define NRF_TEMP_NS_BASE_REAL_HW                  0x400D7000UL
#define NRF_TEMP_S_BASE_REAL_HW                   0x500D7000UL
#define NRF_P1_NS_BASE_REAL_HW                    0x400D8200UL
#define NRF_P1_S_BASE_REAL_HW                     0x500D8200UL
#define NRF_GPIOTE20_NS_BASE_REAL_HW              0x400DA000UL
#define NRF_GPIOTE20_S_BASE_REAL_HW               0x500DA000UL
#define NRF_TAMPC_S_BASE_REAL_HW                  0x500DC000UL
#define NRF_I2S20_NS_BASE_REAL_HW                 0x400DD000UL
#define NRF_I2S20_S_BASE_REAL_HW                  0x500DD000UL
#define NRF_QDEC20_NS_BASE_REAL_HW                0x400E0000UL
#define NRF_QDEC20_S_BASE_REAL_HW                 0x500E0000UL
#define NRF_QDEC21_NS_BASE_REAL_HW                0x400E1000UL
#define NRF_QDEC21_S_BASE_REAL_HW                 0x500E1000UL
#define NRF_GRTC_NS_BASE_REAL_HW                  0x400E2000UL
#define NRF_GRTC_S_BASE_REAL_HW                   0x500E2000UL
#define NRF_SPU30_S_BASE_REAL_HW                  0x50100000UL
#define NRF_DPPIC30_NS_BASE_REAL_HW               0x40102000UL
#define NRF_DPPIC30_S_BASE_REAL_HW                0x50102000UL
#define NRF_PPIB30_NS_BASE_REAL_HW                0x40103000UL
#define NRF_PPIB30_S_BASE_REAL_HW                 0x50103000UL
#define NRF_SPIM30_NS_BASE_REAL_HW                0x40104000UL
#define NRF_SPIS30_NS_BASE_REAL_HW                0x40104000UL
#define NRF_TWIM30_NS_BASE_REAL_HW                0x40104000UL
#define NRF_TWIS30_NS_BASE_REAL_HW                0x40104000UL
#define NRF_UARTE30_NS_BASE_REAL_HW               0x40104000UL
#define NRF_SPIM30_S_BASE_REAL_HW                 0x50104000UL
#define NRF_SPIS30_S_BASE_REAL_HW                 0x50104000UL
#define NRF_TWIM30_S_BASE_REAL_HW                 0x50104000UL
#define NRF_TWIS30_S_BASE_REAL_HW                 0x50104000UL
#define NRF_UARTE30_S_BASE_REAL_HW                0x50104000UL
#define NRF_COMP_NS_BASE_REAL_HW                  0x40106000UL
#define NRF_LPCOMP_NS_BASE_REAL_HW                0x40106000UL
#define NRF_COMP_S_BASE_REAL_HW                   0x50106000UL
#define NRF_LPCOMP_S_BASE_REAL_HW                 0x50106000UL
#define NRF_WDT30_S_BASE_REAL_HW                  0x50108000UL
#define NRF_WDT31_NS_BASE_REAL_HW                 0x40109000UL
#define NRF_WDT31_S_BASE_REAL_HW                  0x50109000UL
#define NRF_P0_NS_BASE_REAL_HW                    0x4010A000UL
#define NRF_P0_S_BASE_REAL_HW                     0x5010A000UL
#define NRF_GPIOTE30_NS_BASE_REAL_HW              0x4010C000UL
#define NRF_GPIOTE30_S_BASE_REAL_HW               0x5010C000UL
#define NRF_CLOCK_NS_BASE_REAL_HW                 0x4010E000UL
#define NRF_POWER_NS_BASE_REAL_HW                 0x4010E000UL
#define NRF_RESET_NS_BASE_REAL_HW                 0x4010E000UL
#define NRF_CLOCK_S_BASE_REAL_HW                  0x5010E000UL
#define NRF_POWER_S_BASE_REAL_HW                  0x5010E000UL
#define NRF_RESET_S_BASE_REAL_HW                  0x5010E000UL
#define NRF_OSCILLATORS_NS_BASE_REAL_HW           0x40120000UL
#define NRF_REGULATORS_NS_BASE_REAL_HW            0x40120000UL
#define NRF_OSCILLATORS_S_BASE_REAL_HW            0x50120000UL
#define NRF_REGULATORS_S_BASE_REAL_HW             0x50120000UL

extern NRF_FICR_Type NRF_FICR_regs;
#undef NRF_FICR_NS_BASE
#define NRF_FICR_NS_BASE (&NRF_FICR_regs)
extern NRF_UICR_Type *NRF_UICR_regs_p[];
#undef NRF_UICR_S_BASE
#define NRF_UICR_S_BASE (NRF_UICR_regs_p[0])
#undef NRF_SICR_S_BASE
#define NRF_SICR_S_BASE NULL
extern NRF_CRACENCORE_Type NRF_CRACENCORE_regs;
#undef NRF_CRACENCORE_S_BASE
#define NRF_CRACENCORE_S_BASE (&NRF_CRACENCORE_regs)
extern NRF_SPU_Type NRF_SPU_regs[];
#undef NRF_SPU00_S_BASE
#define NRF_SPU00_S_BASE (&NRF_SPU_regs[NHW_SPU_00])
#undef NRF_MPC00_S_BASE
#define NRF_MPC00_S_BASE NULL
extern NRF_DPPIC_Type NRF_DPPIC_regs[];
#undef NRF_DPPIC00_NS_BASE
#define NRF_DPPIC00_NS_BASE (&NRF_DPPIC_regs[NHW_DPPI_00])
#undef NRF_DPPIC00_S_BASE
#define NRF_DPPIC00_S_BASE (&NRF_DPPIC_regs[NHW_DPPI_00])
extern NRF_PPIB_Type NRF_PPIB_regs[];
#undef NRF_PPIB00_NS_BASE
#define NRF_PPIB00_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_00])
#undef NRF_PPIB00_S_BASE
#define NRF_PPIB00_S_BASE (&NRF_PPIB_regs[NHW_PPIB_00])
#undef NRF_PPIB01_NS_BASE
#define NRF_PPIB01_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_01])
#undef NRF_PPIB01_S_BASE
#define NRF_PPIB01_S_BASE (&NRF_PPIB_regs[NHW_PPIB_01])
#undef NRF_KMU_S_BASE
#define NRF_KMU_S_BASE NULL
extern NRF_AAR_Type *NRF_AAR_regs[];
extern NRF_CCM_Type *NRF_CCM_regs[];
extern NRF_ECB_Type NRF_ECB_regs[];
#undef NRF_AAR00_NS_BASE
#define NRF_AAR00_NS_BASE (NRF_AAR_regs[NHW_AARCCMECB_00])
#undef NRF_CCM00_NS_BASE
#define NRF_CCM00_NS_BASE (NRF_CCM_regs[NHW_AARCCMECB_00])
#undef NRF_AAR00_S_BASE
#define NRF_AAR00_S_BASE (NRF_AAR_regs[NHW_AARCCMECB_00])
#undef NRF_CCM00_S_BASE
#define NRF_CCM00_S_BASE (NRF_CCM_regs[NHW_AARCCMECB_00])
#undef NRF_ECB00_NS_BASE
#define NRF_ECB00_NS_BASE (&NRF_ECB_regs[NHW_AARCCMECB_00])
#undef NRF_ECB00_S_BASE
#define NRF_ECB00_S_BASE (&NRF_ECB_regs[NHW_AARCCMECB_00])
extern NRF_CRACEN_Type NRF_CRACEN_regs;
#undef NRF_CRACEN_S_BASE
#define NRF_CRACEN_S_BASE (&NRF_CRACEN_regs)
#undef NRF_SPIM00_NS_BASE
#define NRF_SPIM00_NS_BASE NULL
#undef NRF_SPIS00_NS_BASE
#define NRF_SPIS00_NS_BASE NULL
extern NRF_UARTE_Type NRF_UARTE_regs[];
#undef NRF_UARTE00_NS_BASE
#define NRF_UARTE00_NS_BASE (&NRF_UARTE_regs[NHW_UARTE00])
#undef NRF_SPIM00_S_BASE
#define NRF_SPIM00_S_BASE NULL
#undef NRF_SPIS00_S_BASE
#define NRF_SPIS00_S_BASE NULL
#undef NRF_UARTE00_S_BASE
#define NRF_UARTE00_S_BASE (&NRF_UARTE_regs[NHW_UARTE00])
#undef NRF_GLITCHDET_S_BASE
#define NRF_GLITCHDET_S_BASE NULL
extern void* nhw_RRAMC_get_RRAM_base_address(unsigned int inst);
#define NRF_RRAM_BASE_ADDR ((uintptr_t)nhw_RRAMC_get_RRAM_base_address(0))
extern NRF_RRAMC_Type *NRF_RRAMC_regs_p[];
#undef NRF_RRAMC_S_BASE
#define NRF_RRAMC_S_BASE (NRF_RRAMC_regs_p[0])
#undef NRF_VPR00_NS_BASE
#define NRF_VPR00_NS_BASE NULL
#undef NRF_VPR00_S_BASE
#define NRF_VPR00_S_BASE NULL
extern NRF_GPIO_Type NRF_GPIO_regs[];
#undef NRF_P2_NS_BASE
#define NRF_P2_NS_BASE (&NRF_GPIO_regs[NHW_GPIO_P2])
#undef NRF_P2_S_BASE
#define NRF_P2_S_BASE (&NRF_GPIO_regs[NHW_GPIO_P2])
#undef NRF_CTRLAP_NS_BASE
#define NRF_CTRLAP_NS_BASE NULL
#undef NRF_CTRLAP_S_BASE
#define NRF_CTRLAP_S_BASE NULL
#undef NRF_TAD_NS_BASE
#define NRF_TAD_NS_BASE NULL
#undef NRF_TAD_S_BASE
#define NRF_TAD_S_BASE NULL
extern NRF_TIMER_Type NRF_TIMER_regs[];
#undef NRF_TIMER00_NS_BASE
#define NRF_TIMER00_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_00])
#undef NRF_TIMER00_S_BASE
#define NRF_TIMER00_S_BASE (&NRF_TIMER_regs[NHW_TIMER_00])
#undef NRF_SPU10_S_BASE
#define NRF_SPU10_S_BASE  (&NRF_SPU_regs[NHW_SPU_10])
#undef NRF_DPPIC10_NS_BASE
#define NRF_DPPIC10_NS_BASE (&NRF_DPPIC_regs[NHW_DPPI_10])
#undef NRF_DPPIC10_S_BASE
#define NRF_DPPIC10_S_BASE (&NRF_DPPIC_regs[NHW_DPPI_10])
#undef NRF_PPIB10_NS_BASE
#define NRF_PPIB10_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_10])
#undef NRF_PPIB10_S_BASE
#define NRF_PPIB10_S_BASE (&NRF_PPIB_regs[NHW_PPIB_10])
#undef NRF_PPIB11_NS_BASE
#define NRF_PPIB11_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_11])
#undef NRF_PPIB11_S_BASE
#define NRF_PPIB11_S_BASE (&NRF_PPIB_regs[NHW_PPIB_11])
#undef NRF_TIMER10_NS_BASE
#define NRF_TIMER10_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_10])
#undef NRF_TIMER10_S_BASE
#define NRF_TIMER10_S_BASE (&NRF_TIMER_regs[NHW_TIMER_10])
extern NRF_EGU_Type NRF_EGU_regs[];
#undef NRF_EGU10_NS_BASE
#define NRF_EGU10_NS_BASE (&NRF_EGU_regs[NHW_EGU_10])
#undef NRF_EGU10_S_BASE
#define NRF_EGU10_S_BASE (&NRF_EGU_regs[NHW_EGU_10])
extern NRF_RADIO_Type NRF_RADIO_regs;
#undef NRF_RADIO_NS_BASE
#define NRF_RADIO_NS_BASE (&NRF_RADIO_regs)
#undef NRF_RADIO_S_BASE
#define NRF_RADIO_S_BASE (&NRF_RADIO_regs)
#undef NRF_SPU20_S_BASE
#define NRF_SPU20_S_BASE (&NRF_SPU_regs[NHW_SPU_20])
#undef NRF_DPPIC20_NS_BASE
#define NRF_DPPIC20_NS_BASE (&NRF_DPPIC_regs[NHW_DPPI_20])
#undef NRF_DPPIC20_S_BASE
#define NRF_DPPIC20_S_BASE (&NRF_DPPIC_regs[NHW_DPPI_20])
#undef NRF_PPIB20_NS_BASE
#define NRF_PPIB20_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_20])
#undef NRF_PPIB20_S_BASE
#define NRF_PPIB20_S_BASE (&NRF_PPIB_regs[NHW_PPIB_20])
#undef NRF_PPIB21_NS_BASE
#define NRF_PPIB21_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_21])
#undef NRF_PPIB21_S_BASE
#define NRF_PPIB21_S_BASE (&NRF_PPIB_regs[NHW_PPIB_21])
#undef NRF_PPIB22_NS_BASE
#define NRF_PPIB22_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_22])
#undef NRF_PPIB22_S_BASE
#define NRF_PPIB22_S_BASE (&NRF_PPIB_regs[NHW_PPIB_22])
#undef NRF_SPIM20_NS_BASE
#define NRF_SPIM20_NS_BASE NULL
#undef NRF_SPIS20_NS_BASE
#define NRF_SPIS20_NS_BASE NULL
#undef NRF_TWIM20_NS_BASE
#define NRF_TWIM20_NS_BASE NULL
#undef NRF_TWIS20_NS_BASE
#define NRF_TWIS20_NS_BASE NULL
#undef NRF_UARTE20_NS_BASE
#define NRF_UARTE20_NS_BASE (&NRF_UARTE_regs[NHW_UARTE20])
#undef NRF_SPIM20_S_BASE
#define NRF_SPIM20_S_BASE NULL
#undef NRF_SPIS20_S_BASE
#define NRF_SPIS20_S_BASE NULL
#undef NRF_TWIM20_S_BASE
#define NRF_TWIM20_S_BASE NULL
#undef NRF_TWIS20_S_BASE
#define NRF_TWIS20_S_BASE NULL
#undef NRF_UARTE20_S_BASE
#define NRF_UARTE20_S_BASE (&NRF_UARTE_regs[NHW_UARTE20])
#undef NRF_SPIM21_NS_BASE
#define NRF_SPIM21_NS_BASE NULL
#undef NRF_SPIS21_NS_BASE
#define NRF_SPIS21_NS_BASE NULL
#undef NRF_TWIM21_NS_BASE
#define NRF_TWIM21_NS_BASE NULL
#undef NRF_TWIS21_NS_BASE
#define NRF_TWIS21_NS_BASE NULL
#undef NRF_UARTE21_NS_BASE
#define NRF_UARTE21_NS_BASE (&NRF_UARTE_regs[NHW_UARTE21])
#undef NRF_SPIM21_S_BASE
#define NRF_SPIM21_S_BASE NULL
#undef NRF_SPIS21_S_BASE
#define NRF_SPIS21_S_BASE NULL
#undef NRF_TWIM21_S_BASE
#define NRF_TWIM21_S_BASE NULL
#undef NRF_TWIS21_S_BASE
#define NRF_TWIS21_S_BASE NULL
#undef NRF_UARTE21_S_BASE
#define NRF_UARTE21_S_BASE (&NRF_UARTE_regs[NHW_UARTE21])
#undef NRF_SPIM22_NS_BASE
#define NRF_SPIM22_NS_BASE NULL
#undef NRF_SPIS22_NS_BASE
#define NRF_SPIS22_NS_BASE NULL
#undef NRF_TWIM22_NS_BASE
#define NRF_TWIM22_NS_BASE NULL
#undef NRF_TWIS22_NS_BASE
#define NRF_TWIS22_NS_BASE NULL
#undef NRF_UARTE22_NS_BASE
#define NRF_UARTE22_NS_BASE (&NRF_UARTE_regs[NHW_UARTE22])
#undef NRF_SPIM22_S_BASE
#define NRF_SPIM22_S_BASE NULL
#undef NRF_SPIS22_S_BASE
#define NRF_SPIS22_S_BASE NULL
#undef NRF_TWIM22_S_BASE
#define NRF_TWIM22_S_BASE NULL
#undef NRF_TWIS22_S_BASE
#define NRF_TWIS22_S_BASE NULL
#undef NRF_UARTE22_S_BASE
#define NRF_UARTE22_S_BASE (&NRF_UARTE_regs[NHW_UARTE22])
#undef NRF_EGU20_NS_BASE
#define NRF_EGU20_NS_BASE (&NRF_EGU_regs[NHW_EGU_20])
#undef NRF_EGU20_S_BASE
#define NRF_EGU20_S_BASE (&NRF_EGU_regs[NHW_EGU_20])
#undef NRF_TIMER20_NS_BASE
#define NRF_TIMER20_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_20])
#undef NRF_TIMER20_S_BASE
#define NRF_TIMER20_S_BASE (&NRF_TIMER_regs[NHW_TIMER_20])
#undef NRF_TIMER21_NS_BASE
#define NRF_TIMER21_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_21])
#undef NRF_TIMER21_S_BASE
#define NRF_TIMER21_S_BASE (&NRF_TIMER_regs[NHW_TIMER_21])
#undef NRF_TIMER22_NS_BASE
#define NRF_TIMER22_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_22])
#undef NRF_TIMER22_S_BASE
#define NRF_TIMER22_S_BASE (&NRF_TIMER_regs[NHW_TIMER_22])
#undef NRF_TIMER23_NS_BASE
#define NRF_TIMER23_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_23])
#undef NRF_TIMER23_S_BASE
#define NRF_TIMER23_S_BASE (&NRF_TIMER_regs[NHW_TIMER_23])
#undef NRF_TIMER24_NS_BASE
#define NRF_TIMER24_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_24])
#undef NRF_TIMER24_S_BASE
#define NRF_TIMER24_S_BASE (&NRF_TIMER_regs[NHW_TIMER_24])
#undef NRF_MEMCONF_NS_BASE
#define NRF_MEMCONF_NS_BASE NULL
#undef NRF_MEMCONF_S_BASE
#define NRF_MEMCONF_S_BASE NULL
#undef NRF_PDM20_NS_BASE
#define NRF_PDM20_NS_BASE NULL
#undef NRF_PDM20_S_BASE
#define NRF_PDM20_S_BASE NULL
#undef NRF_PDM21_NS_BASE
#define NRF_PDM21_NS_BASE NULL
#undef NRF_PDM21_S_BASE
#define NRF_PDM21_S_BASE NULL
#undef NRF_PWM20_NS_BASE
#define NRF_PWM20_NS_BASE NULL
#undef NRF_PWM20_S_BASE
#define NRF_PWM20_S_BASE NULL
#undef NRF_PWM21_NS_BASE
#define NRF_PWM21_NS_BASE NULL
#undef NRF_PWM21_S_BASE
#define NRF_PWM21_S_BASE NULL
#undef NRF_PWM22_NS_BASE
#define NRF_PWM22_NS_BASE NULL
#undef NRF_PWM22_S_BASE
#define NRF_PWM22_S_BASE NULL
#undef NRF_SAADC_NS_BASE
#define NRF_SAADC_NS_BASE NULL
#undef NRF_SAADC_S_BASE
#define NRF_SAADC_S_BASE NULL
extern NRF_NFCT_Type NRF_NFCT_regs;
#undef NRF_NFCT_NS_BASE
#define NRF_NFCT_NS_BASE (&NRF_NFCT_regs)
#undef NRF_NFCT_S_BASE
#define NRF_NFCT_S_BASE (&NRF_NFCT_regs)
extern NRF_TEMP_Type NRF_TEMP_regs;
#undef NRF_TEMP_NS_BASE
#define NRF_TEMP_NS_BASE (&NRF_TEMP_regs)
#undef NRF_TEMP_S_BASE
#define NRF_TEMP_S_BASE (&NRF_TEMP_regs)
#undef NRF_P1_NS_BASE
#define NRF_P1_NS_BASE (&NRF_GPIO_regs[NHW_GPIO_P1])
#undef NRF_P1_S_BASE
#define NRF_P1_S_BASE (&NRF_GPIO_regs[NHW_GPIO_P1])
extern NRF_GPIOTE_Type NRF_GPIOTE_regs[];
#undef NRF_GPIOTE20_NS_BASE
#define NRF_GPIOTE20_NS_BASE (&NRF_GPIOTE_regs[NHW_GPIOTE_20])
#undef NRF_GPIOTE20_S_BASE
#define NRF_GPIOTE20_S_BASE (&NRF_GPIOTE_regs[NHW_GPIOTE_20])
#undef NRF_TAMPC_S_BASE
#define NRF_TAMPC_S_BASE NULL
#undef NRF_I2S20_NS_BASE
#define NRF_I2S20_NS_BASE NULL
#undef NRF_I2S20_S_BASE
#define NRF_I2S20_S_BASE NULL
#undef NRF_QDEC20_NS_BASE
#define NRF_QDEC20_NS_BASE NULL
#undef NRF_QDEC20_S_BASE
#define NRF_QDEC20_S_BASE NULL
#undef NRF_QDEC21_NS_BASE
#define NRF_QDEC21_NS_BASE NULL
#undef NRF_QDEC21_S_BASE
#define NRF_QDEC21_S_BASE NULL
extern NRF_GRTC_Type NRF_GRTC_regs;
#undef NRF_GRTC_NS_BASE
#define NRF_GRTC_NS_BASE (&NRF_GRTC_regs)
#undef NRF_GRTC_S_BASE
#define NRF_GRTC_S_BASE (&NRF_GRTC_regs)
#undef NRF_SPU30_S_BASE
#define NRF_SPU30_S_BASE (&NRF_SPU_regs[NHW_SPU_30])
#undef NRF_DPPIC30_NS_BASE
#define NRF_DPPIC30_NS_BASE (&NRF_DPPIC_regs[NHW_DPPI_30])
#undef NRF_DPPIC30_S_BASE
#define NRF_DPPIC30_S_BASE (&NRF_DPPIC_regs[NHW_DPPI_30])
#undef NRF_PPIB30_NS_BASE
#define NRF_PPIB30_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_30])
#undef NRF_PPIB30_S_BASE
#define NRF_PPIB30_S_BASE (&NRF_PPIB_regs[NHW_PPIB_30])
#undef NRF_SPIM30_NS_BASE
#define NRF_SPIM30_NS_BASE NULL
#undef NRF_SPIS30_NS_BASE
#define NRF_SPIS30_NS_BASE NULL
#undef NRF_TWIM30_NS_BASE
#define NRF_TWIM30_NS_BASE NULL
#undef NRF_TWIS30_NS_BASE
#define NRF_TWIS30_NS_BASE NULL
#undef NRF_UARTE30_NS_BASE
#define NRF_UARTE30_NS_BASE (&NRF_UARTE_regs[NHW_UARTE20])
#undef NRF_SPIM30_S_BASE
#define NRF_SPIM30_S_BASE NULL
#undef NRF_SPIS30_S_BASE
#define NRF_SPIS30_S_BASE NULL
#undef NRF_TWIM30_S_BASE
#define NRF_TWIM30_S_BASE NULL
#undef NRF_TWIS30_S_BASE
#define NRF_TWIS30_S_BASE NULL
#undef NRF_UARTE30_S_BASE
#define NRF_UARTE30_S_BASE (&NRF_UARTE_regs[NHW_UARTE30])
#undef NRF_COMP_NS_BASE
#define NRF_COMP_NS_BASE NULL
#undef NRF_LPCOMP_NS_BASE
#define NRF_LPCOMP_NS_BASE NULL
#undef NRF_COMP_S_BASE
#define NRF_COMP_S_BASE NULL
#undef NRF_LPCOMP_S_BASE
#define NRF_LPCOMP_S_BASE NULL
#undef NRF_WDT30_S_BASE
#define NRF_WDT30_S_BASE NULL
#undef NRF_WDT31_NS_BASE
#define NRF_WDT31_NS_BASE NULL
#undef NRF_WDT31_S_BASE
#define NRF_WDT31_S_BASE NULL
#undef NRF_P0_NS_BASE
#define NRF_P0_NS_BASE (&NRF_GPIO_regs[NHW_GPIO_P0])
#undef NRF_P0_S_BASE
#define NRF_P0_S_BASE (&NRF_GPIO_regs[NHW_GPIO_P0])
#undef NRF_GPIOTE30_NS_BASE
#define NRF_GPIOTE30_NS_BASE (&NRF_GPIOTE_regs[NHW_GPIOTE_30])
#undef NRF_GPIOTE30_S_BASE
#define NRF_GPIOTE30_S_BASE (&NRF_GPIOTE_regs[NHW_GPIOTE_30])
extern NRF_CLOCK_Type *NRF_CLOCK_regs[];
#undef NRF_CLOCK_NS_BASE
#define NRF_CLOCK_NS_BASE (NRF_CLOCK_regs[NHW_CLKPWR_0])
extern NRF_POWER_Type *NRF_POWER_regs[];
#undef NRF_POWER_NS_BASE
#define NRF_POWER_NS_BASE (NRF_POWER_regs[NHW_CLKPWR_0])
extern NRF_RESET_Type *NRF_RESET_regs[];
#undef NRF_RESET_NS_BASE
#define NRF_RESET_NS_BASE (NRF_RESET_regs[NHW_CLKPWR_0])
#undef NRF_CLOCK_S_BASE
#define NRF_CLOCK_S_BASE (NRF_CLOCK_regs[NHW_CLKPWR_0])
#undef NRF_POWER_S_BASE
#define NRF_POWER_S_BASE (NRF_POWER_regs[NHW_CLKPWR_0])
#undef NRF_RESET_S_BASE
#define NRF_RESET_S_BASE (NRF_RESET_regs[NHW_CLKPWR_0])
#undef NRF_OSCILLATORS_NS_BASE
#define NRF_OSCILLATORS_NS_BASE NULL
#undef NRF_REGULATORS_NS_BASE
#define NRF_REGULATORS_NS_BASE NULL
#undef NRF_OSCILLATORS_S_BASE
#define NRF_OSCILLATORS_S_BASE NULL
#undef NRF_REGULATORS_S_BASE
#define NRF_REGULATORS_S_BASE NULL

/********************************************************************/
/********************************************************************/
/********************************************************************/
#elif defined(NRF54LM20A_XXAA) || defined(NRF54LM20A_ENGA_XXAA)

#define NRF_FICR_NS_BASE_REAL_HW                  0x00FFC000UL
#define NRF_UICR_S_BASE_REAL_HW                   0x00FFD000UL
#define NRF_SICR_S_BASE_REAL_HW                   0x00FFE000UL
#define NRF_CRACENCORE_S_BASE_REAL_HW             0x50010000UL
#define NRF_USBHSCORE_NS_BASE_REAL_HW             0x40020000UL
#define NRF_USBHSCORE_S_BASE_REAL_HW              0x50020000UL
#define NRF_SPU00_S_BASE_REAL_HW                  0x50040000UL
#define NRF_MPC00_S_BASE_REAL_HW                  0x50041000UL
#define NRF_DPPIC00_NS_BASE_REAL_HW               0x40042000UL
#define NRF_DPPIC00_S_BASE_REAL_HW                0x50042000UL
#define NRF_PPIB00_NS_BASE_REAL_HW                0x40044000UL
#define NRF_PPIB00_S_BASE_REAL_HW                 0x50044000UL
#define NRF_PPIB01_NS_BASE_REAL_HW                0x40045000UL
#define NRF_PPIB01_S_BASE_REAL_HW                 0x50045000UL
#define NRF_KMU_S_BASE_REAL_HW                    0x50049000UL
#define NRF_AAR00_NS_BASE_REAL_HW                 0x4004A000UL
#define NRF_CCM00_NS_BASE_REAL_HW                 0x4004A000UL
#define NRF_AAR00_S_BASE_REAL_HW                  0x5004A000UL
#define NRF_CCM00_S_BASE_REAL_HW                  0x5004A000UL
#define NRF_ECB00_NS_BASE_REAL_HW                 0x4004B000UL
#define NRF_ECB00_S_BASE_REAL_HW                  0x5004B000UL
#define NRF_VPR00_NS_BASE_REAL_HW                 0x4004C000UL
#define NRF_VPR00_S_BASE_REAL_HW                  0x5004C000UL
#define NRF_SPIM00_NS_BASE_REAL_HW                0x4004D000UL
#define NRF_SPIS00_NS_BASE_REAL_HW                0x4004D000UL
#define NRF_UARTE00_NS_BASE_REAL_HW               0x4004D000UL
#define NRF_SPIM00_S_BASE_REAL_HW                 0x5004D000UL
#define NRF_SPIS00_S_BASE_REAL_HW                 0x5004D000UL
#define NRF_UARTE00_S_BASE_REAL_HW                0x5004D000UL
#define NRF_GLITCHDET_S_BASE_REAL_HW              0x5004E000UL
#define NRF_RRAMC_S_BASE_REAL_HW                  0x5004E000UL
#define NRF_P2_NS_BASE_REAL_HW                    0x40050400UL
#define NRF_GPIOHSPADCTRL_S_BASE_REAL_HW          0x50050400UL
#define NRF_P2_S_BASE_REAL_HW                     0x50050400UL
#define NRF_CTRLAP_NS_BASE_REAL_HW                0x40052000UL
#define NRF_CTRLAP_S_BASE_REAL_HW                 0x50052000UL
#define NRF_TAD_NS_BASE_REAL_HW                   0x40053000UL
#define NRF_TAD_S_BASE_REAL_HW                    0x50053000UL
#define NRF_TIMER00_NS_BASE_REAL_HW               0x40055000UL
#define NRF_TIMER00_S_BASE_REAL_HW                0x50055000UL
#define NRF_EGU00_NS_BASE_REAL_HW                 0x40058000UL
#define NRF_EGU00_S_BASE_REAL_HW                  0x50058000UL
#define NRF_CRACEN_S_BASE_REAL_HW                 0x50059000UL
#define NRF_USBHS_NS_BASE_REAL_HW                 0x4005A000UL
#define NRF_USBHS_S_BASE_REAL_HW                  0x5005A000UL
#define NRF_SPU10_S_BASE_REAL_HW                  0x50080000UL
#define NRF_DPPIC10_NS_BASE_REAL_HW               0x40082000UL
#define NRF_DPPIC10_S_BASE_REAL_HW                0x50082000UL
#define NRF_PPIB10_NS_BASE_REAL_HW                0x40083000UL
#define NRF_PPIB10_S_BASE_REAL_HW                 0x50083000UL
#define NRF_PPIB11_NS_BASE_REAL_HW                0x40084000UL
#define NRF_PPIB11_S_BASE_REAL_HW                 0x50084000UL
#define NRF_TIMER10_NS_BASE_REAL_HW               0x40085000UL
#define NRF_TIMER10_S_BASE_REAL_HW                0x50085000UL
#define NRF_EGU10_NS_BASE_REAL_HW                 0x40087000UL
#define NRF_EGU10_S_BASE_REAL_HW                  0x50087000UL
#define NRF_RADIO_NS_BASE_REAL_HW                 0x4008A000UL
#define NRF_RADIO_S_BASE_REAL_HW                  0x5008A000UL
#define NRF_SPU20_S_BASE_REAL_HW                  0x500C0000UL
#define NRF_DPPIC20_NS_BASE_REAL_HW               0x400C2000UL
#define NRF_DPPIC20_S_BASE_REAL_HW                0x500C2000UL
#define NRF_PPIB20_NS_BASE_REAL_HW                0x400C3000UL
#define NRF_PPIB20_S_BASE_REAL_HW                 0x500C3000UL
#define NRF_PPIB21_NS_BASE_REAL_HW                0x400C4000UL
#define NRF_PPIB21_S_BASE_REAL_HW                 0x500C4000UL
#define NRF_PPIB22_NS_BASE_REAL_HW                0x400C5000UL
#define NRF_PPIB22_S_BASE_REAL_HW                 0x500C5000UL
#define NRF_SPIM20_NS_BASE_REAL_HW                0x400C6000UL
#define NRF_SPIS20_NS_BASE_REAL_HW                0x400C6000UL
#define NRF_TWIM20_NS_BASE_REAL_HW                0x400C6000UL
#define NRF_TWIS20_NS_BASE_REAL_HW                0x400C6000UL
#define NRF_UARTE20_NS_BASE_REAL_HW               0x400C6000UL
#define NRF_SPIM20_S_BASE_REAL_HW                 0x500C6000UL
#define NRF_SPIS20_S_BASE_REAL_HW                 0x500C6000UL
#define NRF_TWIM20_S_BASE_REAL_HW                 0x500C6000UL
#define NRF_TWIS20_S_BASE_REAL_HW                 0x500C6000UL
#define NRF_UARTE20_S_BASE_REAL_HW                0x500C6000UL
#define NRF_SPIM21_NS_BASE_REAL_HW                0x400C7000UL
#define NRF_SPIS21_NS_BASE_REAL_HW                0x400C7000UL
#define NRF_TWIM21_NS_BASE_REAL_HW                0x400C7000UL
#define NRF_TWIS21_NS_BASE_REAL_HW                0x400C7000UL
#define NRF_UARTE21_NS_BASE_REAL_HW               0x400C7000UL
#define NRF_SPIM21_S_BASE_REAL_HW                 0x500C7000UL
#define NRF_SPIS21_S_BASE_REAL_HW                 0x500C7000UL
#define NRF_TWIM21_S_BASE_REAL_HW                 0x500C7000UL
#define NRF_TWIS21_S_BASE_REAL_HW                 0x500C7000UL
#define NRF_UARTE21_S_BASE_REAL_HW                0x500C7000UL
#define NRF_SPIM22_NS_BASE_REAL_HW                0x400C8000UL
#define NRF_SPIS22_NS_BASE_REAL_HW                0x400C8000UL
#define NRF_TWIM22_NS_BASE_REAL_HW                0x400C8000UL
#define NRF_TWIS22_NS_BASE_REAL_HW                0x400C8000UL
#define NRF_UARTE22_NS_BASE_REAL_HW               0x400C8000UL
#define NRF_SPIM22_S_BASE_REAL_HW                 0x500C8000UL
#define NRF_SPIS22_S_BASE_REAL_HW                 0x500C8000UL
#define NRF_TWIM22_S_BASE_REAL_HW                 0x500C8000UL
#define NRF_TWIS22_S_BASE_REAL_HW                 0x500C8000UL
#define NRF_UARTE22_S_BASE_REAL_HW                0x500C8000UL
#define NRF_EGU20_NS_BASE_REAL_HW                 0x400C9000UL
#define NRF_EGU20_S_BASE_REAL_HW                  0x500C9000UL
#define NRF_TIMER20_NS_BASE_REAL_HW               0x400CA000UL
#define NRF_TIMER20_S_BASE_REAL_HW                0x500CA000UL
#define NRF_TIMER21_NS_BASE_REAL_HW               0x400CB000UL
#define NRF_TIMER21_S_BASE_REAL_HW                0x500CB000UL
#define NRF_TIMER22_NS_BASE_REAL_HW               0x400CC000UL
#define NRF_TIMER22_S_BASE_REAL_HW                0x500CC000UL
#define NRF_TIMER23_NS_BASE_REAL_HW               0x400CD000UL
#define NRF_TIMER23_S_BASE_REAL_HW                0x500CD000UL
#define NRF_TIMER24_NS_BASE_REAL_HW               0x400CE000UL
#define NRF_TIMER24_S_BASE_REAL_HW                0x500CE000UL
#define NRF_MEMCONF_NS_BASE_REAL_HW               0x400CF000UL
#define NRF_MEMCONF_S_BASE_REAL_HW                0x500CF000UL
#define NRF_PDM20_NS_BASE_REAL_HW                 0x400D0000UL
#define NRF_PDM20_S_BASE_REAL_HW                  0x500D0000UL
#define NRF_PDM21_NS_BASE_REAL_HW                 0x400D1000UL
#define NRF_PDM21_S_BASE_REAL_HW                  0x500D1000UL
#define NRF_PWM20_NS_BASE_REAL_HW                 0x400D2000UL
#define NRF_PWM20_S_BASE_REAL_HW                  0x500D2000UL
#define NRF_PWM21_NS_BASE_REAL_HW                 0x400D3000UL
#define NRF_PWM21_S_BASE_REAL_HW                  0x500D3000UL
#define NRF_PWM22_NS_BASE_REAL_HW                 0x400D4000UL
#define NRF_PWM22_S_BASE_REAL_HW                  0x500D4000UL
#define NRF_SAADC_NS_BASE_REAL_HW                 0x400D5000UL
#define NRF_SAADC_S_BASE_REAL_HW                  0x500D5000UL
#define NRF_NFCT_NS_BASE_REAL_HW                  0x400D6000UL
#define NRF_NFCT_S_BASE_REAL_HW                   0x500D6000UL
#define NRF_TEMP_NS_BASE_REAL_HW                  0x400D7000UL
#define NRF_TEMP_S_BASE_REAL_HW                   0x500D7000UL
#define NRF_P1_NS_BASE_REAL_HW                    0x400D8200UL
#define NRF_P3_NS_BASE_REAL_HW                    0x400D8600UL
#define NRF_P1_S_BASE_REAL_HW                     0x500D8200UL
#define NRF_P3_S_BASE_REAL_HW                     0x500D8600UL
#define NRF_GPIOTE20_NS_BASE_REAL_HW              0x400DA000UL
#define NRF_GPIOTE20_S_BASE_REAL_HW               0x500DA000UL
#define NRF_QDEC20_NS_BASE_REAL_HW                0x400E0000UL
#define NRF_QDEC20_S_BASE_REAL_HW                 0x500E0000UL
#define NRF_QDEC21_NS_BASE_REAL_HW                0x400E1000UL
#define NRF_QDEC21_S_BASE_REAL_HW                 0x500E1000UL
#define NRF_GRTC_NS_BASE_REAL_HW                  0x400E2000UL
#define NRF_GRTC_S_BASE_REAL_HW                   0x500E2000UL
#define NRF_TDM_NS_BASE_REAL_HW                   0x400E8000UL
#define NRF_TDM_S_BASE_REAL_HW                    0x500E8000UL
#define NRF_SPIM23_NS_BASE_REAL_HW                0x400ED000UL
#define NRF_SPIS23_NS_BASE_REAL_HW                0x400ED000UL
#define NRF_TWIM23_NS_BASE_REAL_HW                0x400ED000UL
#define NRF_TWIS23_NS_BASE_REAL_HW                0x400ED000UL
#define NRF_UARTE23_NS_BASE_REAL_HW               0x400ED000UL
#define NRF_SPIM23_S_BASE_REAL_HW                 0x500ED000UL
#define NRF_SPIS23_S_BASE_REAL_HW                 0x500ED000UL
#define NRF_TWIM23_S_BASE_REAL_HW                 0x500ED000UL
#define NRF_TWIS23_S_BASE_REAL_HW                 0x500ED000UL
#define NRF_UARTE23_S_BASE_REAL_HW                0x500ED000UL
#define NRF_SPIM24_NS_BASE_REAL_HW                0x400EE000UL
#define NRF_SPIS24_NS_BASE_REAL_HW                0x400EE000UL
#define NRF_TWIM24_NS_BASE_REAL_HW                0x400EE000UL
#define NRF_TWIS24_NS_BASE_REAL_HW                0x400EE000UL
#define NRF_UARTE24_NS_BASE_REAL_HW               0x400EE000UL
#define NRF_SPIM24_S_BASE_REAL_HW                 0x500EE000UL
#define NRF_SPIS24_S_BASE_REAL_HW                 0x500EE000UL
#define NRF_TWIM24_S_BASE_REAL_HW                 0x500EE000UL
#define NRF_TWIS24_S_BASE_REAL_HW                 0x500EE000UL
#define NRF_UARTE24_S_BASE_REAL_HW                0x500EE000UL
#define NRF_TAMPC_S_BASE_REAL_HW                  0x500EF000UL
#define NRF_SPU30_S_BASE_REAL_HW                  0x50100000UL
#define NRF_DPPIC30_NS_BASE_REAL_HW               0x40102000UL
#define NRF_DPPIC30_S_BASE_REAL_HW                0x50102000UL
#define NRF_PPIB30_NS_BASE_REAL_HW                0x40103000UL
#define NRF_PPIB30_S_BASE_REAL_HW                 0x50103000UL
#define NRF_SPIM30_NS_BASE_REAL_HW                0x40104000UL
#define NRF_SPIS30_NS_BASE_REAL_HW                0x40104000UL
#define NRF_TWIM30_NS_BASE_REAL_HW                0x40104000UL
#define NRF_TWIS30_NS_BASE_REAL_HW                0x40104000UL
#define NRF_UARTE30_NS_BASE_REAL_HW               0x40104000UL
#define NRF_SPIM30_S_BASE_REAL_HW                 0x50104000UL
#define NRF_SPIS30_S_BASE_REAL_HW                 0x50104000UL
#define NRF_TWIM30_S_BASE_REAL_HW                 0x50104000UL
#define NRF_TWIS30_S_BASE_REAL_HW                 0x50104000UL
#define NRF_UARTE30_S_BASE_REAL_HW                0x50104000UL
#define NRF_COMP_NS_BASE_REAL_HW                  0x40106000UL
#define NRF_LPCOMP_NS_BASE_REAL_HW                0x40106000UL
#define NRF_COMP_S_BASE_REAL_HW                   0x50106000UL
#define NRF_LPCOMP_S_BASE_REAL_HW                 0x50106000UL
#define NRF_WDT30_S_BASE_REAL_HW                  0x50108000UL
#define NRF_WDT31_NS_BASE_REAL_HW                 0x40109000UL
#define NRF_WDT31_S_BASE_REAL_HW                  0x50109000UL
#define NRF_P0_NS_BASE_REAL_HW                    0x4010A000UL
#define NRF_P0_S_BASE_REAL_HW                     0x5010A000UL
#define NRF_GPIOTE30_NS_BASE_REAL_HW              0x4010C000UL
#define NRF_GPIOTE30_S_BASE_REAL_HW               0x5010C000UL
#define NRF_CLOCK_NS_BASE_REAL_HW                 0x4010E000UL
#define NRF_POWER_NS_BASE_REAL_HW                 0x4010E000UL
#define NRF_RESET_NS_BASE_REAL_HW                 0x4010E000UL
#define NRF_CLOCK_S_BASE_REAL_HW                  0x5010E000UL
#define NRF_POWER_S_BASE_REAL_HW                  0x5010E000UL
#define NRF_RESET_S_BASE_REAL_HW                  0x5010E000UL
#define NRF_OSCILLATORS_NS_BASE_REAL_HW           0x40120000UL
#define NRF_REGULATORS_NS_BASE_REAL_HW            0x40120000UL
#define NRF_OSCILLATORS_S_BASE_REAL_HW            0x50120000UL
#define NRF_REGULATORS_S_BASE_REAL_HW             0x50120000UL
#define NRF_VREGUSB_NS_BASE_REAL_HW               0x40121000UL
#define NRF_VREGUSB_S_BASE_REAL_HW                0x50121000UL


extern NRF_FICR_Type NRF_FICR_regs;
#undef NRF_FICR_NS_BASE
#define NRF_FICR_NS_BASE (&NRF_FICR_regs)
extern NRF_UICR_Type *NRF_UICR_regs_p[];
#undef NRF_UICR_S_BASE
#define NRF_UICR_S_BASE (NRF_UICR_regs_p[0])
#undef NRF_SICR_S_BASE
#define NRF_SICR_S_BASE NULL
extern NRF_CRACENCORE_Type NRF_CRACENCORE_regs;
#undef NRF_CRACENCORE_S_BASE
#define NRF_CRACENCORE_S_BASE (&NRF_CRACENCORE_regs)
#undef NRF_USBHSCORE_NS_BASE
#define NRF_USBHSCORE_NS_BASE NULL
#undef NRF_USBHSCORE_S_BASE
#define NRF_USBHSCORE_S_BASE NULL
extern NRF_SPU_Type NRF_SPU_regs[];
#undef NRF_SPU00_S_BASE
#define NRF_SPU00_S_BASE (&NRF_SPU_regs[NHW_SPU_00])
#undef NRF_MPC00_S_BASE
#define NRF_MPC00_S_BASE NULL
extern NRF_DPPIC_Type NRF_DPPIC_regs[];
#undef NRF_DPPIC00_NS_BASE
#define NRF_DPPIC00_NS_BASE (&NRF_DPPIC_regs[NHW_DPPI_00])
#undef NRF_DPPIC00_S_BASE
#define NRF_DPPIC00_S_BASE (&NRF_DPPIC_regs[NHW_DPPI_00])
extern NRF_PPIB_Type NRF_PPIB_regs[];
#undef NRF_PPIB00_NS_BASE
#define NRF_PPIB00_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_00])
#undef NRF_PPIB00_S_BASE
#define NRF_PPIB00_S_BASE (&NRF_PPIB_regs[NHW_PPIB_00])
#undef NRF_PPIB01_NS_BASE
#define NRF_PPIB01_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_01])
#undef NRF_PPIB01_S_BASE
#define NRF_PPIB01_S_BASE (&NRF_PPIB_regs[NHW_PPIB_01])
#undef NRF_KMU_S_BASE
#define NRF_KMU_S_BASE NULL
extern NRF_AAR_Type *NRF_AAR_regs[];
extern NRF_CCM_Type *NRF_CCM_regs[];
extern NRF_ECB_Type NRF_ECB_regs[];
#undef NRF_AAR00_NS_BASE
#define NRF_AAR00_NS_BASE (NRF_AAR_regs[NHW_AARCCMECB_00])
#undef NRF_CCM00_NS_BASE
#define NRF_CCM00_NS_BASE (NRF_CCM_regs[NHW_AARCCMECB_00])
#undef NRF_AAR00_S_BASE
#define NRF_AAR00_S_BASE (NRF_AAR_regs[NHW_AARCCMECB_00])
#undef NRF_CCM00_S_BASE
#define NRF_CCM00_S_BASE (NRF_CCM_regs[NHW_AARCCMECB_00])
#undef NRF_ECB00_NS_BASE
#define NRF_ECB00_NS_BASE (&NRF_ECB_regs[NHW_AARCCMECB_00])
#undef NRF_ECB00_S_BASE
#define NRF_ECB00_S_BASE (&NRF_ECB_regs[NHW_AARCCMECB_00])
#undef NRF_VPR00_NS_BASE
#define NRF_VPR00_NS_BASE NULL
#undef NRF_VPR00_S_BASE
#define NRF_VPR00_S_BASE NULL
#undef NRF_SPIM00_NS_BASE
#define NRF_SPIM00_NS_BASE NULL
#undef NRF_SPIS00_NS_BASE
#define NRF_SPIS00_NS_BASE NULL
extern NRF_UARTE_Type NRF_UARTE_regs[];
#undef NRF_UARTE00_NS_BASE
#define NRF_UARTE00_NS_BASE (&NRF_UARTE_regs[NHW_UARTE00])
#undef NRF_SPIM00_S_BASE
#define NRF_SPIM00_S_BASE NULL
#undef NRF_SPIS00_S_BASE
#define NRF_SPIS00_S_BASE NULL
#undef NRF_UARTE00_S_BASE
#define NRF_UARTE00_S_BASE (&NRF_UARTE_regs[NHW_UARTE00])
#undef NRF_GLITCHDET_S_BASE
#define NRF_GLITCHDET_S_BASE NULL
extern void* nhw_RRAMC_get_RRAM_base_address(unsigned int inst);
#define NRF_RRAM_BASE_ADDR ((uintptr_t)nhw_RRAMC_get_RRAM_base_address(0))
extern NRF_RRAMC_Type *NRF_RRAMC_regs_p[];
#undef NRF_RRAMC_S_BASE
#define NRF_RRAMC_S_BASE (NRF_RRAMC_regs_p[0])
extern NRF_GPIO_Type NRF_GPIO_regs[];
#undef NRF_P2_NS_BASE
#define NRF_P2_NS_BASE (&NRF_GPIO_regs[NHW_GPIO_P2])
#undef NRF_P2_S_BASE
#define NRF_P2_S_BASE (&NRF_GPIO_regs[NHW_GPIO_P2])
#undef NRF_CTRLAP_NS_BASE
#define NRF_CTRLAP_NS_BASE NULL
#undef NRF_CTRLAP_S_BASE
#define NRF_CTRLAP_S_BASE NULL
#undef NRF_TAD_NS_BASE
#define NRF_TAD_NS_BASE NULL
#undef NRF_TAD_S_BASE
#define NRF_TAD_S_BASE NULL
extern NRF_TIMER_Type NRF_TIMER_regs[];
#undef NRF_TIMER00_NS_BASE
#define NRF_TIMER00_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_00])
#undef NRF_TIMER00_S_BASE
#define NRF_TIMER00_S_BASE (&NRF_TIMER_regs[NHW_TIMER_00])
extern NRF_EGU_Type NRF_EGU_regs[];
#undef NRF_EGU00_NS_BASE
#define NRF_EGU00_NS_BASE (&NRF_EGU_regs[NHW_EGU_00])
#undef NRF_EGU00_S_BASE
#define NRF_EGU00_S_BASE (&NRF_EGU_regs[NHW_EGU_00])
extern NRF_CRACEN_Type NRF_CRACEN_regs;
#undef NRF_CRACEN_S_BASE
#define NRF_CRACEN_S_BASE (&NRF_CRACEN_regs)
#undef NRF_USBHS_NS_BASE
#define NRF_USBHS_NS_BASE NULL
#undef NRF_USBHS_S_BASE
#define NRF_USBHS_S_BASE NULL
#undef NRF_SPU10_S_BASE
#define NRF_SPU10_S_BASE  (&NRF_SPU_regs[NHW_SPU_10])
#undef NRF_DPPIC10_NS_BASE
#define NRF_DPPIC10_NS_BASE (&NRF_DPPIC_regs[NHW_DPPI_10])
#undef NRF_DPPIC10_S_BASE
#define NRF_DPPIC10_S_BASE (&NRF_DPPIC_regs[NHW_DPPI_10])
#undef NRF_PPIB10_NS_BASE
#define NRF_PPIB10_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_10])
#undef NRF_PPIB10_S_BASE
#define NRF_PPIB10_S_BASE (&NRF_PPIB_regs[NHW_PPIB_10])
#undef NRF_PPIB11_NS_BASE
#define NRF_PPIB11_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_11])
#undef NRF_PPIB11_S_BASE
#define NRF_PPIB11_S_BASE (&NRF_PPIB_regs[NHW_PPIB_11])
#undef NRF_TIMER10_NS_BASE
#define NRF_TIMER10_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_10])
#undef NRF_TIMER10_S_BASE
#define NRF_TIMER10_S_BASE (&NRF_TIMER_regs[NHW_TIMER_10])
#undef NRF_EGU10_NS_BASE
#define NRF_EGU10_NS_BASE (&NRF_EGU_regs[NHW_EGU_10])
#undef NRF_EGU10_S_BASE
#define NRF_EGU10_S_BASE (&NRF_EGU_regs[NHW_EGU_10])
extern NRF_RADIO_Type NRF_RADIO_regs;
#undef NRF_RADIO_NS_BASE
#define NRF_RADIO_NS_BASE (&NRF_RADIO_regs)
#undef NRF_RADIO_S_BASE
#define NRF_RADIO_S_BASE (&NRF_RADIO_regs)
#undef NRF_SPU20_S_BASE
#define NRF_SPU20_S_BASE (&NRF_SPU_regs[NHW_SPU_20])
#undef NRF_DPPIC20_NS_BASE
#define NRF_DPPIC20_NS_BASE (&NRF_DPPIC_regs[NHW_DPPI_20])
#undef NRF_DPPIC20_S_BASE
#define NRF_DPPIC20_S_BASE (&NRF_DPPIC_regs[NHW_DPPI_20])
#undef NRF_PPIB20_NS_BASE
#define NRF_PPIB20_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_20])
#undef NRF_PPIB20_S_BASE
#define NRF_PPIB20_S_BASE (&NRF_PPIB_regs[NHW_PPIB_20])
#undef NRF_PPIB21_NS_BASE
#define NRF_PPIB21_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_21])
#undef NRF_PPIB21_S_BASE
#define NRF_PPIB21_S_BASE (&NRF_PPIB_regs[NHW_PPIB_21])
#undef NRF_PPIB22_NS_BASE
#define NRF_PPIB22_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_22])
#undef NRF_PPIB22_S_BASE
#define NRF_PPIB22_S_BASE (&NRF_PPIB_regs[NHW_PPIB_22])
#undef NRF_SPIM20_NS_BASE
#define NRF_SPIM20_NS_BASE NULL
#undef NRF_SPIS20_NS_BASE
#define NRF_SPIS20_NS_BASE NULL
#undef NRF_TWIM20_NS_BASE
#define NRF_TWIM20_NS_BASE NULL
#undef NRF_TWIS20_NS_BASE
#define NRF_TWIS20_NS_BASE NULL
#undef NRF_UARTE20_NS_BASE
#define NRF_UARTE20_NS_BASE (&NRF_UARTE_regs[NHW_UARTE20])
#undef NRF_SPIM20_S_BASE
#define NRF_SPIM20_S_BASE NULL
#undef NRF_SPIS20_S_BASE
#define NRF_SPIS20_S_BASE NULL
#undef NRF_TWIM20_S_BASE
#define NRF_TWIM20_S_BASE NULL
#undef NRF_TWIS20_S_BASE
#define NRF_TWIS20_S_BASE NULL
#undef NRF_UARTE20_S_BASE
#define NRF_UARTE20_S_BASE (&NRF_UARTE_regs[NHW_UARTE20])
#undef NRF_SPIM21_NS_BASE
#define NRF_SPIM21_NS_BASE NULL
#undef NRF_SPIS21_NS_BASE
#define NRF_SPIS21_NS_BASE NULL
#undef NRF_TWIM21_NS_BASE
#define NRF_TWIM21_NS_BASE NULL
#undef NRF_TWIS21_NS_BASE
#define NRF_TWIS21_NS_BASE NULL
#undef NRF_UARTE21_NS_BASE
#define NRF_UARTE21_NS_BASE (&NRF_UARTE_regs[NHW_UARTE21])
#undef NRF_SPIM21_S_BASE
#define NRF_SPIM21_S_BASE NULL
#undef NRF_SPIS21_S_BASE
#define NRF_SPIS21_S_BASE NULL
#undef NRF_TWIM21_S_BASE
#define NRF_TWIM21_S_BASE NULL
#undef NRF_TWIS21_S_BASE
#define NRF_TWIS21_S_BASE NULL
#undef NRF_UARTE21_S_BASE
#define NRF_UARTE21_S_BASE (&NRF_UARTE_regs[NHW_UARTE21])
#undef NRF_SPIM22_NS_BASE
#define NRF_SPIM22_NS_BASE NULL
#undef NRF_SPIS22_NS_BASE
#define NRF_SPIS22_NS_BASE NULL
#undef NRF_TWIM22_NS_BASE
#define NRF_TWIM22_NS_BASE NULL
#undef NRF_TWIS22_NS_BASE
#define NRF_TWIS22_NS_BASE NULL
#undef NRF_UARTE22_NS_BASE
#define NRF_UARTE22_NS_BASE (&NRF_UARTE_regs[NHW_UARTE22])
#undef NRF_SPIM22_S_BASE
#define NRF_SPIM22_S_BASE NULL
#undef NRF_SPIS22_S_BASE
#define NRF_SPIS22_S_BASE NULL
#undef NRF_TWIM22_S_BASE
#define NRF_TWIM22_S_BASE NULL
#undef NRF_TWIS22_S_BASE
#define NRF_TWIS22_S_BASE NULL
#undef NRF_UARTE22_S_BASE
#define NRF_UARTE22_S_BASE (&NRF_UARTE_regs[NHW_UARTE22])
#undef NRF_EGU20_NS_BASE
#define NRF_EGU20_NS_BASE (&NRF_EGU_regs[NHW_EGU_20])
#undef NRF_EGU20_S_BASE
#define NRF_EGU20_S_BASE (&NRF_EGU_regs[NHW_EGU_20])
#undef NRF_TIMER20_NS_BASE
#define NRF_TIMER20_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_20])
#undef NRF_TIMER20_S_BASE
#define NRF_TIMER20_S_BASE (&NRF_TIMER_regs[NHW_TIMER_20])
#undef NRF_TIMER21_NS_BASE
#define NRF_TIMER21_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_21])
#undef NRF_TIMER21_S_BASE
#define NRF_TIMER21_S_BASE (&NRF_TIMER_regs[NHW_TIMER_21])
#undef NRF_TIMER22_NS_BASE
#define NRF_TIMER22_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_22])
#undef NRF_TIMER22_S_BASE
#define NRF_TIMER22_S_BASE (&NRF_TIMER_regs[NHW_TIMER_22])
#undef NRF_TIMER23_NS_BASE
#define NRF_TIMER23_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_23])
#undef NRF_TIMER23_S_BASE
#define NRF_TIMER23_S_BASE (&NRF_TIMER_regs[NHW_TIMER_23])
#undef NRF_TIMER24_NS_BASE
#define NRF_TIMER24_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_24])
#undef NRF_TIMER24_S_BASE
#define NRF_TIMER24_S_BASE (&NRF_TIMER_regs[NHW_TIMER_24])
#undef NRF_MEMCONF_NS_BASE
#define NRF_MEMCONF_NS_BASE NULL
#undef NRF_MEMCONF_S_BASE
#define NRF_MEMCONF_S_BASE NULL
#undef NRF_PDM20_NS_BASE
#define NRF_PDM20_NS_BASE NULL
#undef NRF_PDM20_S_BASE
#define NRF_PDM20_S_BASE NULL
#undef NRF_PDM21_NS_BASE
#define NRF_PDM21_NS_BASE NULL
#undef NRF_PDM21_S_BASE
#define NRF_PDM21_S_BASE NULL
#undef NRF_PWM20_NS_BASE
#define NRF_PWM20_NS_BASE NULL
#undef NRF_PWM20_S_BASE
#define NRF_PWM20_S_BASE NULL
#undef NRF_PWM21_NS_BASE
#define NRF_PWM21_NS_BASE NULL
#undef NRF_PWM21_S_BASE
#define NRF_PWM21_S_BASE NULL
#undef NRF_PWM22_NS_BASE
#define NRF_PWM22_NS_BASE NULL
#undef NRF_PWM22_S_BASE
#define NRF_PWM22_S_BASE NULL
#undef NRF_SAADC_NS_BASE
#define NRF_SAADC_NS_BASE NULL
#undef NRF_SAADC_S_BASE
#define NRF_SAADC_S_BASE NULL
extern NRF_NFCT_Type NRF_NFCT_regs;
#undef NRF_NFCT_NS_BASE
#define NRF_NFCT_NS_BASE (&NRF_NFCT_regs)
#undef NRF_NFCT_S_BASE
#define NRF_NFCT_S_BASE (&NRF_NFCT_regs)
extern NRF_TEMP_Type NRF_TEMP_regs;
#undef NRF_TEMP_NS_BASE
#define NRF_TEMP_NS_BASE (&NRF_TEMP_regs)
#undef NRF_TEMP_S_BASE
#define NRF_TEMP_S_BASE (&NRF_TEMP_regs)
#undef NRF_P1_NS_BASE
#define NRF_P1_NS_BASE (&NRF_GPIO_regs[NHW_GPIO_P1])
#undef NRF_P1_S_BASE
#define NRF_P1_S_BASE (&NRF_GPIO_regs[NHW_GPIO_P1])
#undef NRF_P3_NS_BASE
#define NRF_P3_NS_BASE (&NRF_GPIO_regs[NHW_GPIO_P3])
#undef NRF_P3_S_BASE
#define NRF_P3_S_BASE (&NRF_GPIO_regs[NHW_GPIO_P3])
extern NRF_GPIOTE_Type NRF_GPIOTE_regs[];
#undef NRF_GPIOTE20_NS_BASE
#define NRF_GPIOTE20_NS_BASE (&NRF_GPIOTE_regs[NHW_GPIOTE_20])
#undef NRF_GPIOTE20_S_BASE
#define NRF_GPIOTE20_S_BASE (&NRF_GPIOTE_regs[NHW_GPIOTE_20])
#undef NRF_QDEC20_NS_BASE
#define NRF_QDEC20_NS_BASE NULL
#undef NRF_QDEC20_S_BASE
#define NRF_QDEC20_S_BASE NULL
#undef NRF_QDEC21_NS_BASE
#define NRF_QDEC21_NS_BASE NULL
#undef NRF_QDEC21_S_BASE
#define NRF_QDEC21_S_BASE NULL
extern NRF_GRTC_Type NRF_GRTC_regs;
#undef NRF_GRTC_NS_BASE
#define NRF_GRTC_NS_BASE (&NRF_GRTC_regs)
#undef NRF_GRTC_S_BASE
#define NRF_GRTC_S_BASE (&NRF_GRTC_regs)
#undef NRF_TDM_NS_BASE
#define NRF_TDM_NS_BASE NULL
#undef NRF_TDM_S_BASE
#define NRF_TDM_S_BASE NULL
#undef NRF_SPIM23_NS_BASE
#define NRF_SPIM23_NS_BASE NULL
#undef NRF_SPIS23_NS_BASE
#define NRF_SPIS23_NS_BASE NULL
#undef NRF_TWIM23_NS_BASE
#define NRF_TWIM23_NS_BASE NULL
#undef NRF_TWIS23_NS_BASE
#define NRF_TWIS23_NS_BASE NULL
#undef NRF_UARTE23_NS_BASE
#define NRF_UARTE23_NS_BASE (&NRF_UARTE_regs[NHW_UARTE23])
#undef NRF_SPIM23_S_BASE
#define NRF_SPIM23_S_BASE NULL
#undef NRF_SPIS23_S_BASE
#define NRF_SPIS23_S_BASE NULL
#undef NRF_TWIM23_S_BASE
#define NRF_TWIM23_S_BASE NULL
#undef NRF_TWIS23_S_BASE
#define NRF_TWIS23_S_BASE NULL
#undef NRF_UARTE23_S_BASE
#define NRF_UARTE23_S_BASE (&NRF_UARTE_regs[NHW_UARTE23])
#undef NRF_SPIM24_NS_BASE
#define NRF_SPIM24_NS_BASE NULL
#undef NRF_SPIS24_NS_BASE
#define NRF_SPIS24_NS_BASE NULL
#undef NRF_TWIM24_NS_BASE
#define NRF_TWIM24_NS_BASE NULL
#undef NRF_TWIS24_NS_BASE
#define NRF_TWIS24_NS_BASE NULL
#undef NRF_UARTE24_NS_BASE
#define NRF_UARTE24_NS_BASE (&NRF_UARTE_regs[NHW_UARTE24])
#undef NRF_SPIM24_S_BASE
#define NRF_SPIM24_S_BASE NULL
#undef NRF_SPIS24_S_BASE
#define NRF_SPIS24_S_BASE NULL
#undef NRF_TWIM24_S_BASE
#define NRF_TWIM24_S_BASE NULL
#undef NRF_TWIS24_S_BASE
#define NRF_TWIS24_S_BASE NULL
#undef NRF_UARTE24_S_BASE
#define NRF_UARTE24_S_BASE (&NRF_UARTE_regs[NHW_UARTE24])
#undef NRF_TAMPC_S_BASE
#define NRF_TAMPC_S_BASE NULL
#undef NRF_SPU30_S_BASE
#define NRF_SPU30_S_BASE (&NRF_SPU_regs[NHW_SPU_30])
#undef NRF_DPPIC30_NS_BASE
#define NRF_DPPIC30_NS_BASE (&NRF_DPPIC_regs[NHW_DPPI_30])
#undef NRF_DPPIC30_S_BASE
#define NRF_DPPIC30_S_BASE (&NRF_DPPIC_regs[NHW_DPPI_30])
#undef NRF_PPIB30_NS_BASE
#define NRF_PPIB30_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_30])
#undef NRF_PPIB30_S_BASE
#define NRF_PPIB30_S_BASE (&NRF_PPIB_regs[NHW_PPIB_30])
#undef NRF_SPIM30_NS_BASE
#define NRF_SPIM30_NS_BASE NULL
#undef NRF_SPIS30_NS_BASE
#define NRF_SPIS30_NS_BASE NULL
#undef NRF_TWIM30_NS_BASE
#define NRF_TWIM30_NS_BASE NULL
#undef NRF_TWIS30_NS_BASE
#define NRF_TWIS30_NS_BASE NULL
#undef NRF_UARTE30_NS_BASE
#define NRF_UARTE30_NS_BASE (&NRF_UARTE_regs[NHW_UARTE20])
#undef NRF_SPIM30_S_BASE
#define NRF_SPIM30_S_BASE NULL
#undef NRF_SPIS30_S_BASE
#define NRF_SPIS30_S_BASE NULL
#undef NRF_TWIM30_S_BASE
#define NRF_TWIM30_S_BASE NULL
#undef NRF_TWIS30_S_BASE
#define NRF_TWIS30_S_BASE NULL
#undef NRF_UARTE30_S_BASE
#define NRF_UARTE30_S_BASE (&NRF_UARTE_regs[NHW_UARTE30])
#undef NRF_COMP_NS_BASE
#define NRF_COMP_NS_BASE NULL
#undef NRF_LPCOMP_NS_BASE
#define NRF_LPCOMP_NS_BASE NULL
#undef NRF_COMP_S_BASE
#define NRF_COMP_S_BASE NULL
#undef NRF_LPCOMP_S_BASE
#define NRF_LPCOMP_S_BASE NULL
#undef NRF_WDT30_S_BASE
#define NRF_WDT30_S_BASE NULL
#undef NRF_WDT31_NS_BASE
#define NRF_WDT31_NS_BASE NULL
#undef NRF_WDT31_S_BASE
#define NRF_WDT31_S_BASE NULL
#undef NRF_P0_NS_BASE
#define NRF_P0_NS_BASE (&NRF_GPIO_regs[NHW_GPIO_P0])
#undef NRF_P0_S_BASE
#define NRF_P0_S_BASE (&NRF_GPIO_regs[NHW_GPIO_P0])
#undef NRF_GPIOTE30_NS_BASE
#define NRF_GPIOTE30_NS_BASE (&NRF_GPIOTE_regs[NHW_GPIOTE_30])
#undef NRF_GPIOTE30_S_BASE
#define NRF_GPIOTE30_S_BASE (&NRF_GPIOTE_regs[NHW_GPIOTE_30])
extern NRF_CLOCK_Type *NRF_CLOCK_regs[];
#undef NRF_CLOCK_NS_BASE
#define NRF_CLOCK_NS_BASE (NRF_CLOCK_regs[NHW_CLKPWR_0])
extern NRF_POWER_Type *NRF_POWER_regs[];
#undef NRF_POWER_NS_BASE
#define NRF_POWER_NS_BASE (NRF_POWER_regs[NHW_CLKPWR_0])
extern NRF_RESET_Type *NRF_RESET_regs[];
#undef NRF_RESET_NS_BASE
#define NRF_RESET_NS_BASE (NRF_RESET_regs[NHW_CLKPWR_0])
#undef NRF_CLOCK_S_BASE
#define NRF_CLOCK_S_BASE (NRF_CLOCK_regs[NHW_CLKPWR_0])
#undef NRF_POWER_S_BASE
#define NRF_POWER_S_BASE (NRF_POWER_regs[NHW_CLKPWR_0])
#undef NRF_RESET_S_BASE
#define NRF_RESET_S_BASE (NRF_RESET_regs[NHW_CLKPWR_0])
#undef NRF_OSCILLATORS_NS_BASE
#define NRF_OSCILLATORS_NS_BASE NULL
#undef NRF_REGULATORS_NS_BASE
#define NRF_REGULATORS_NS_BASE NULL
#undef NRF_OSCILLATORS_S_BASE
#define NRF_OSCILLATORS_S_BASE NULL
#undef NRF_REGULATORS_S_BASE
#define NRF_REGULATORS_S_BASE NULL
#undef NRF_VREGUSB_NS_BASE
#define NRF_VREGUSB_NS_BASE NULL
#undef NRF_VREGUSB_S_BASE
#define NRF_VREGUSB_S_BASE NULL

#else
#error "Platform not supported"
# endif

#ifdef __cplusplus
}
#endif

#endif /* NRF_BSIM_REDEF_H */
