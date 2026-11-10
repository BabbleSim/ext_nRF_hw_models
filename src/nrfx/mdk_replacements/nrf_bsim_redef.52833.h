/*
 * Copyright (c) 2018 Oticon A/S
 * Copyright (c) 2020-2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
extern NRF_PWM_Type NRF_PWM_regs[];
#undef NRF_PWM0_BASE
#define NRF_PWM0_BASE                     (&NRF_PWM_regs[0])
#undef NRF_PWM1_BASE
#define NRF_PWM1_BASE                     (&NRF_PWM_regs[1])
#undef NRF_PWM2_BASE
#define NRF_PWM2_BASE                     (&NRF_PWM_regs[2])
#undef NRF_PWM3_BASE
#define NRF_PWM3_BASE                     (&NRF_PWM_regs[3])
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
