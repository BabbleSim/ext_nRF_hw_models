/*
 * Copyright (c) 2018 Oticon A/S
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define NRF_FICR_BASE_REAL_HW                     0x00FFC000UL
#define NRF_UICR_BASE_REAL_HW                     0x00FFD000UL
#define NRF_DPPIC00_BASE_REAL_HW                  0x40042000UL
#define NRF_PPIB00_BASE_REAL_HW                   0x40044000UL
#define NRF_PPIB01_BASE_REAL_HW                   0x40045000UL
#define NRF_AAR00_BASE_REAL_HW                    0x4004A000UL
#define NRF_CCM00_BASE_REAL_HW                    0x4004A000UL
#define NRF_ECB00_BASE_REAL_HW                    0x4004B000UL
#define NRF_RRAMC_BASE_REAL_HW                    0x4004E000UL
#define NRF_CTRLAP_BASE_REAL_HW                   0x40052000UL
#define NRF_TAD_BASE_REAL_HW                      0x40053000UL
#define NRF_TIMER00_BASE_REAL_HW                  0x40055000UL
#define NRF_EGU00_BASE_REAL_HW                    0x40058000UL
#define NRF_CRACEN_BASE_REAL_HW                   0x40059000UL
#define NRF_CRACENCORE_BASE_REAL_HW               0x40059000UL
#define NRF_DPPIC10_BASE_REAL_HW                  0x40082000UL
#define NRF_PPIB10_BASE_REAL_HW                   0x40083000UL
#define NRF_PPIB11_BASE_REAL_HW                   0x40084000UL
#define NRF_TIMER10_BASE_REAL_HW                  0x40085000UL
#define NRF_EGU10_BASE_REAL_HW                    0x40087000UL
#define NRF_RADIO_BASE_REAL_HW                    0x4008A000UL
#define NRF_DPPIC20_BASE_REAL_HW                  0x400C2000UL
#define NRF_PPIB20_BASE_REAL_HW                   0x400C3000UL
#define NRF_PPIB21_BASE_REAL_HW                   0x400C4000UL
#define NRF_PPIB22_BASE_REAL_HW                   0x400C5000UL
#define NRF_SPIM20_BASE_REAL_HW                   0x400C6000UL
#define NRF_SPIS20_BASE_REAL_HW                   0x400C6000UL
#define NRF_TWIM20_BASE_REAL_HW                   0x400C6000UL
#define NRF_TWIS20_BASE_REAL_HW                   0x400C6000UL
#define NRF_UARTE20_BASE_REAL_HW                  0x400C6000UL
#define NRF_SPIM21_BASE_REAL_HW                   0x400C7000UL
#define NRF_SPIS21_BASE_REAL_HW                   0x400C7000UL
#define NRF_TWIM21_BASE_REAL_HW                   0x400C7000UL
#define NRF_TWIS21_BASE_REAL_HW                   0x400C7000UL
#define NRF_UARTE21_BASE_REAL_HW                  0x400C7000UL
#define NRF_SPIM22_BASE_REAL_HW                   0x400C8000UL
#define NRF_SPIS22_BASE_REAL_HW                   0x400C8000UL
#define NRF_TWIM22_BASE_REAL_HW                   0x400C8000UL
#define NRF_TWIS22_BASE_REAL_HW                   0x400C8000UL
#define NRF_UARTE22_BASE_REAL_HW                  0x400C8000UL
#define NRF_EGU20_BASE_REAL_HW                    0x400C9000UL
#define NRF_TIMER20_BASE_REAL_HW                  0x400CA000UL
#define NRF_MEMCONF_BASE_REAL_HW                  0x400CF000UL
#define NRF_PWM20_BASE_REAL_HW                    0x400D2000UL
#define NRF_SAADC_BASE_REAL_HW                    0x400D5000UL
#define NRF_TEMP_BASE_REAL_HW                     0x400D7000UL
#define NRF_P1_BASE_REAL_HW                       0x400D8200UL
#define NRF_GPIOTE20_BASE_REAL_HW                 0x400DA000UL
#define NRF_QDEC20_BASE_REAL_HW                   0x400E0000UL
#define NRF_GRTC_BASE_REAL_HW                     0x400E2000UL
#define NRF_TAMPC_BASE_REAL_HW                    0x400EF000UL
#define NRF_DPPIC30_BASE_REAL_HW                  0x40102000UL
#define NRF_PPIB30_BASE_REAL_HW                   0x40103000UL
#define NRF_WDT30_BASE_REAL_HW                    0x40108000UL
#define NRF_P0_BASE_REAL_HW                       0x4010A000UL
#define NRF_GPIOTE30_BASE_REAL_HW                 0x4010C000UL
#define NRF_CLOCK_BASE_REAL_HW                    0x4010E000UL
#define NRF_POWER_BASE_REAL_HW                    0x4010E000UL
#define NRF_RESET_BASE_REAL_HW                    0x4010E000UL
#define NRF_OSCILLATORS_BASE_REAL_HW              0x40120000UL
#define NRF_REGULATORS_BASE_REAL_HW               0x40120000UL

extern void* nhw_RRAMC_get_RRAM_base_address(unsigned int inst);
#define NRF_RRAM_BASE_ADDR ((uintptr_t)nhw_RRAMC_get_RRAM_base_address(0))
extern NRF_FICR_Type NRF_FICR_regs;
#undef NRF_FICR_BASE
#define NRF_FICR_BASE (&NRF_FICR_regs)
extern NRF_UICR_Type *NRF_UICR_regs_p[];
#undef NRF_UICR_BASE
#define NRF_UICR_BASE (NRF_UICR_regs_p[0])
extern NRF_DPPIC_Type NRF_DPPIC_regs[];
#undef NRF_DPPIC00_BASE
#define NRF_DPPIC00_BASE (&NRF_DPPIC_regs[NHW_DPPI_00])
extern NRF_PPIB_Type NRF_PPIB_regs[];
#undef NRF_PPIB00_BASE
#define NRF_PPIB00_BASE (&NRF_PPIB_regs[NHW_PPIB_00])
#undef NRF_PPIB01_BASE
#define NRF_PPIB01_BASE (&NRF_PPIB_regs[NHW_PPIB_01])
extern NRF_AAR_Type *NRF_AAR_regs[];
extern NRF_CCM_Type *NRF_CCM_regs[];
extern NRF_ECB_Type NRF_ECB_regs[];
#undef NRF_AAR00_BASE
#define NRF_AAR00_BASE (NRF_AAR_regs[NHW_AARCCMECB_00])
#undef NRF_CCM00_BASE
#define NRF_CCM00_BASE (NRF_CCM_regs[NHW_AARCCMECB_00])
#undef NRF_ECB00_BASE
#define NRF_ECB00_BASE (&NRF_ECB_regs[NHW_AARCCMECB_00])
extern NRF_RRAMC_Type *NRF_RRAMC_regs_p[];
#undef NRF_RRAMC_BASE
#define NRF_RRAMC_BASE (NRF_RRAMC_regs_p[0])
#undef NRF_CTRLAP_BASE
#define NRF_CTRLAP_BASE NULL
#undef NRF_TAD_BASE
#define NRF_TAD_BASE NULL
extern NRF_TIMER_Type NRF_TIMER_regs[];
#undef NRF_TIMER00_BASE
#define NRF_TIMER00_BASE (&NRF_TIMER_regs[NHW_TIMER_00])
extern NRF_EGU_Type NRF_EGU_regs[];
#undef NRF_EGU00_BASE
#define NRF_EGU00_BASE (&NRF_EGU_regs[NHW_EGU_00])
extern NRF_CRACEN_Type NRF_CRACEN_regs;
#undef NRF_CRACEN_BASE
#define NRF_CRACEN_BASE (&NRF_CRACEN_regs)
extern NRF_CRACENCORE_Type NRF_CRACENCORE_regs;
#undef NRF_CRACENCORE_BASE
#define NRF_CRACENCORE_BASE (&NRF_CRACENCORE_regs)
#undef NRF_DPPIC10_BASE
#define NRF_DPPIC10_BASE (&NRF_DPPIC_regs[NHW_DPPI_10])
#undef NRF_PPIB10_BASE
#define NRF_PPIB10_BASE (&NRF_PPIB_regs[NHW_PPIB_10])
#undef NRF_PPIB11_BASE
#define NRF_PPIB11_BASE (&NRF_PPIB_regs[NHW_PPIB_11])
#undef NRF_TIMER10_BASE
#define NRF_TIMER10_BASE (&NRF_TIMER_regs[NHW_TIMER_10])
#undef NRF_EGU10_BASE
#define NRF_EGU10_BASE (&NRF_EGU_regs[NHW_EGU_10])
extern NRF_RADIO_Type NRF_RADIO_regs;
#undef NRF_RADIO_BASE
#define NRF_RADIO_BASE (&NRF_RADIO_regs)
#undef NRF_DPPIC20_BASE
#define NRF_DPPIC20_BASE (&NRF_DPPIC_regs[NHW_DPPI_20])
#undef NRF_PPIB20_BASE
#define NRF_PPIB20_BASE (&NRF_PPIB_regs[NHW_PPIB_20])
#undef NRF_PPIB21_BASE
#define NRF_PPIB21_BASE (&NRF_PPIB_regs[NHW_PPIB_21])
#undef NRF_PPIB22_BASE
#define NRF_PPIB22_BASE (&NRF_PPIB_regs[NHW_PPIB_22])
#undef NRF_SPIM20_BASE
#define NRF_SPIM20_BASE NULL
#undef NRF_SPIS20_BASE
#define NRF_SPIS20_BASE NULL
#undef NRF_TWIM20_BASE
#define NRF_TWIM20_BASE NULL
#undef NRF_TWIS20_BASE
#define NRF_TWIS20_BASE NULL
extern NRF_UARTE_Type NRF_UARTE_regs[];
#undef NRF_UARTE20_BASE
#define NRF_UARTE20_BASE (&NRF_UARTE_regs[NHW_UARTE20])
#undef NRF_SPIM21_BASE
#define NRF_SPIM21_BASE NULL
#undef NRF_SPIS21_BASE
#define NRF_SPIS21_BASE NULL
#undef NRF_TWIM21_BASE
#define NRF_TWIM21_BASE NULL
#undef NRF_TWIS21_BASE
#define NRF_TWIS21_BASE NULL
#undef NRF_UARTE21_BASE
#define NRF_UARTE21_BASE (&NRF_UARTE_regs[NHW_UARTE21])
#undef NRF_SPIM22_BASE
#define NRF_SPIM22_BASE NULL
#undef NRF_SPIS22_BASE
#define NRF_SPIS22_BASE NULL
#undef NRF_TWIM22_BASE
#define NRF_TWIM22_BASE NULL
#undef NRF_TWIS22_BASE
#define NRF_TWIS22_BASE NULL
#undef NRF_UARTE22_BASE
#define NRF_UARTE22_BASE (&NRF_UARTE_regs[NHW_UARTE22])
#undef NRF_EGU20_BASE
#define NRF_EGU20_BASE (&NRF_EGU_regs[NHW_EGU_20])
#undef NRF_TIMER20_BASE
#define NRF_TIMER20_BASE (&NRF_TIMER_regs[NHW_TIMER_20])
#undef NRF_MEMCONF_BASE
#define NRF_MEMCONF_BASE NULL
#undef NRF_PWM20_BASE
#define NRF_PWM20_BASE NULL
#undef NRF_SAADC_BASE
#define NRF_SAADC_BASE NULL
extern NRF_TEMP_Type NRF_TEMP_regs;
#undef NRF_TEMP_BASE
#define NRF_TEMP_BASE (&NRF_TEMP_regs)
extern NRF_GPIO_Type NRF_GPIO_regs[];
#undef NRF_P1_BASE
#define NRF_P1_BASE (&NRF_GPIO_regs[NHW_GPIO_P1])
extern NRF_GPIOTE_Type NRF_GPIOTE_regs[];
#undef NRF_GPIOTE20_BASE
#define NRF_GPIOTE20_BASE (&NRF_GPIOTE_regs[NHW_GPIOTE_20])
#undef NRF_QDEC20_BASE
#define NRF_QDEC20_BASE NULL
extern NRF_GRTC_Type NRF_GRTC_regs;
#undef NRF_GRTC_BASE
#define NRF_GRTC_BASE (&NRF_GRTC_regs)
#undef NRF_TAMPC_BASE
#define NRF_TAMPC_BASE NULL
#undef NRF_DPPIC30_BASE
#define NRF_DPPIC30_BASE (&NRF_DPPIC_regs[NHW_DPPI_30])
#undef NRF_PPIB30_BASE
#define NRF_PPIB30_BASE (&NRF_PPIB_regs[NHW_PPIB_30])
#undef NRF_WDT30_BASE
#define NRF_WDT30_BASE NULL
#undef NRF_P0_BASE
#define NRF_P0_BASE (&NRF_GPIO_regs[NHW_GPIO_P0])
#undef NRF_GPIOTE30_BASE
#define NRF_GPIOTE30_BASE (&NRF_GPIOTE_regs[NHW_GPIOTE_30])
extern NRF_CLOCK_Type *NRF_CLOCK_regs[];
#undef NRF_CLOCK_BASE
#define NRF_CLOCK_BASE (NRF_CLOCK_regs[NHW_CLKPWR_0])
extern NRF_POWER_Type *NRF_POWER_regs[];
#undef NRF_POWER_BASE
#define NRF_POWER_BASE (NRF_POWER_regs[NHW_CLKPWR_0])
extern NRF_RESET_Type *NRF_RESET_regs[];
#undef NRF_RESET_BASE
#define NRF_RESET_BASE (NRF_RESET_regs[NHW_CLKPWR_0])
#undef NRF_OSCILLATORS_BASE
#define NRF_OSCILLATORS_BASE NULL
#undef NRF_REGULATORS_BASE
#define NRF_REGULATORS_BASE NULL
