/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define NHW_CORE_NAMES {""}

#define NHW_HAS_PPI  1
#define NHW_HAS_DPPI 0
#define NHW_USE_MDK_TYPES 1 /* The HW registers layout types are taken from the MDK */

#define NHW_AAR_TOTAL_INST 1
#define NHW_AAR_0 0
#define NHW_AAR_INT_MAP {{0 , 15}} /*Only core,CCM_AAR_IRQn*/
#define NHW_AAR_t_AAR    6

#define NHW_CCM_TOTAL_INST 1
#define NHW_CCM_0 0
#define NHW_CCM_INT_MAP {{0 , 15}} /*Only core,CCM_AAR_IRQn*/

#define NHW_CLKPWR_TOTAL_INST 1
#define NHW_CLKPWR_0 0
#define NHW_CLKPWR_INT_MAP {{0 , 0}} /*Only core, POWER_CLOCK_IRQn*/
#define NHW_CLKPWR_HAS_RESET 0
#define NHW_CLKPWR_HAS_CALTIMER 1
#define NHW_CLKPWR_HAS_HFCLKAUDIOCLK 0
#define NHW_CLKPWR_HAS_HFCLKAUDIOCLK_I {0}
#define NHW_CLKPWR_HAS_HFCLK192MCLK 0
#define NHW_CLKPWR_HAS_HFCLK192MCLK_I  {0}
#define NHW_CLKPWR_HAS_LFCLK 1
#define NHW_CLKPWR_CLK_MAX_N_SRCS 3
#define NHW_CLKPWR_N_CLKS 2
#define NHW_CLKPWR_CLK_IDX_LF 0
#define NHW_CLKPWR_CLK_IDX_HF 1
#define NHW_CLKPWR_CLK_START_TIMES {\
                                    {0, 0, 0},\
                                    {0}}
                                    /* LFCLK,
                                       HFCLK*/
#define NHW_CLKPWR_CLK_CAL_TIME 0

#define NHW_ECB_TOTAL_INST 1
#define NHW_ECB_0 0
#define NHW_ECB_INT_MAP {{0 , 14}} /*Only core,ECB_IRQn*/
#define NHW_ECB_t_ECB 7 /* 7.2 */

#define NHW_EGU_TOTAL_INST 6
#define NHW_EGU_0 0
#define NHW_EGU_1 1
#define NHW_EGU_2 2
#define NHW_EGU_3 3
#define NHW_EGU_4 4
#define NHW_EGU_5 5
#define NHW_EGU_INT_MAP {{0 , 20}, \
                         {0 , 21}, \
                         {0 , 22}, \
                         {0 , 23}, \
                         {0 , 24}, \
                         {0 , 25}, \
                        } /*Only core,SWI0..5_EGU0_IRQn*/
#define NHW_EGU_N_EVENTS {16, 16, 16, 16, 16, 16}

#define NHW_GPIO_TOTAL_INST 2
#define NHW_GPIO_P0 0
#define NHW_GPIO_P1 1
#define NHW_GPIO_MAX_PINS_PER_PORT 32
#define NHW_GPIO_NBR_PINS {32, 10} /* Number of IOs per port */
#define NHW_GPIO_PARTNER_GPIOTE {0, 0} /* GPIOTE nbr to which the DETECT signal(s) will be sent */
#define NHW_GPIO_HAS_PIN_SENSE {1} /* Per instance, does it have pin sense/detect mechanism */

#define NHW_GPIOTE_TOTAL_INST 1
#define NHW_GPIOTE_0 0
#define NHW_GPIOTE_N_INT 1 /* Number of interrupts lines, common for all instances */
#define NHW_GPIOTE_INT_MAP {{{0 , 6}}} /* Only core, GPIOTE_IRQn */
#define NHW_GPIOTE_MAX_CHANNELS 8    /* Maximum number of channels in any instance */
#define NHW_GPIOTE_CHANNELS {8} /* Number of channels per instance */
#define NHW_GPIOTE_IS_54 0

#define NHW_INTCTRL_TOTAL_INST 1
#define NHW_INTCTRL_MAX_INTLINES 48

/* These names are taken from the IRQn_Type in the MDK header */
#define NHW_INT_NAMES { [0] = {\
[0 ] = "POWER_CLOCK",\
[1 ] = "RADIO",\
[2 ] = "UARTE0_UART0",\
[3 ] = "SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0",\
[4 ] = "SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1",\
[5 ] = "NFCT",\
[6 ] = "GPIOTE",\
[7 ] = "SAADC",\
[8 ] = "TIMER0",\
[9 ] = "TIMER1",\
[10] = "TIMER2",\
[11] = "RTC0",\
[12] = "TEMP",\
[13] = "RNG",\
[14] = "ECB",\
[15] = "CCM_AAR",\
[16] = "WDT",\
[17] = "RTC1",\
[18] = "QDEC",\
[19] = "COMP_LPCOMP",\
[20] = "SWI0_EGU0",\
[21] = "SWI1_EGU1",\
[22] = "SWI2_EGU2",\
[23] = "SWI3_EGU3",\
[24] = "SWI4_EGU4",\
[25] = "SWI5_EGU5",\
[26] = "TIMER3",\
[27] = "TIMER4",\
[28] = "PWM0",\
[29] = "PDM",\
[32] = "MWU",\
[33] = "PWM1",\
[34] = "PWM2",\
[35] = "SPIM2_SPIS2_SPI2",\
[36] = "RTC2",\
[37] = "I2S",\
[38] = "FPU",\
[39] = "USBD",\
[40] = "UARTE1",\
[45] = "PWM3",\
[47] = "SPIM3",\
}}

#define NHW_NVMC_UICR_TOTAL_INST 1
#define NHW_NVMC_HAS_ERASEREGS 1
#define NHW_FLASH_START_ADDR {0x00000000}
#define NHW_FLASH_PAGESIZE {(4*1024)}
#define NHW_FLASH_N_PAGES {128}
#define NHW_UICR_SIZE {776 /*64*4*/ /*bytes*/}
        /* In case somebody tries to access the UICR registers, we book
         * more space than its actual flash area (64*4)*/
#define NHW_NVMC_FLASH_T_ERASEALL (173000)
#define NHW_NVMC_FLASH_T_ERASEPAGE (87500)
#define NHW_NVMC_FLASH_T_WRITE        (42)
#define NHW_NVMC_FLASH_PARTIAL_ERASE_FACTOR (1.0)

#define NHW_RADIO_TOTAL_INST 1
#define NHW_RADIO_0 0
#define NHW_RADIO_N_INT 1
#define NHW_RADIO_INT_MAP {{0 , 1}} /*Only core,RADIO_IRQn*/
#define NHW_RADIO_ED_RSSIOFFS (-93)
#define NHW_RADIO_IS_54 0
#define NHW_RADIO_HAS_15_4 1
#define NHW_RADIO_HAS_BLECODED 1
#define NHW_RADIO_HAS_DF 1
#define NHW_RADIO_HAS_CS 0

#define NHW_RNG_TOTAL_INST 1
#define NHW_RNG_0 0
#define NHW_RNG_INT_MAP {{0 , 13}} /*Only core,RNG_IRQn*/
#define NHW_RNG_tRNG_START 128
#define NHW_RNG_tRNG_RAW    30
#define NHW_RNG_tRNG_BC    120

#define NHW_RTC_TOTAL_INST 3
#define NHW_RTC_0 0
#define NHW_RTC_1 1
#define NHW_RTC_2 2
#define NHW_RTC_INT_MAP {{0 , 11}, \
                         {0 , 17}, \
                         {0 , 36}, \
                         } /*Only core,RTC0..2_IRQn*/
#define NHW_RTC_HAS_CAPTURE 0
#define NHW_RTC_HAS_SHORT_COMP_CLEAR 0
#define NHW_RTC_N_CC {3, 4, 4}

#define NHW_TEMP_TOTAL_INST 1
#define NHW_TEMP_0 0
#define NHW_TEMP_INT_MAP {{0 , 12}} /*Only core,TEMP_IRQn*/
#define NHW_TEMP_t_TEMP 36 /* microseconds */
#define NHW_TEMP_FBITS  2 /* fractional bits => 0.25C resolution */

#define NHW_TIMER_TOTAL_INST 5
#define NHW_TIMER_0 0
#define NHW_TIMER_1 1
#define NHW_TIMER_2 2
#define NHW_TIMER_3 3
#define NHW_TIMER_4 4
#define NHW_TIMER_INT_MAP {{0 , 8}, \
                           {0 , 9}, \
                           {0 , 10}, \
                           {0 , 26}, \
                           {0 , 27}, \
                          } /* Only core, TIMER0..4_IRQn */
#define NHW_TIMER_HAS_ONE_SHOT 0
#define NHW_TIMER_N_CC {4, 4, 4, 6, 6}
#define NHW_TIMER_MAX_N_CC 6
#define NHW_TIMER_FREQ {16, 16, 16, 16, 16}

#define NHW_UARTE_TOTAL_INST 2
#define NHW_UART_0 0
#define NHW_UART_1 1
#define NHW_UARTE_INT_MAP {{0 , 2}, \
                          {0 , 40}, \
                          } /* Only core, UARTE0_UART0_IRQn, UARTE1_IRQn */
#define NHW_UARTE_HAS_UART 1
#define NHW_UARTE_NAMES {"UARTE0", \
                         "UARTE1"}
#define NHW_UARTE_54NAMING 0
#define NHW_UARTE_FLUSH_AMOUNT_BUG 0
#define NHW_UARTE_HAS_FRAMETIMEOUT 0
#define NHW_UARTE_HAS_MATCH 0
#define NHW_UARTE_CLOCKS {16, 16}

#define NHW_BSTICKER_TOTAL_INST 1
#define NHW_BSTICKER_TIMER_INT_MAP {{0 , 0}} /*Only core, -*/

#define NHW_FAKE_TIMER_TOTAL_INST 1
#define NHW_FAKE_TIMER_INT_MAP {{0 , 0}} /*Only core, -*/
