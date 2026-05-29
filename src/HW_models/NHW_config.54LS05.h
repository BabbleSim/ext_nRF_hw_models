/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define NHW_HAS_PPI  0
#define NHW_HAS_DPPI 1
#define NHW_USE_MDK_TYPES 1 /* The HW registers layout types are taken from the MDK */
#define NHW_CORE_NAMES {"Application"}

#define NHW_AARCCMECB_TOTAL_INST 1
#define NHW_AARCCMECB_00 0
#define NHW_AARCCMECB_DPPI_MAP {0} /* MCU */
#define NHW_AARCCM_INTMAP {{0, 74}}
                         /* AAR00_CCM00_IRQn */
#define NHW_ECB_INTMAP {{0, 75}}
                        /* ECB00_IRQn */
#define NHW_AARCCMECB_CLOCK {128} /* MHz */
#define NHW_ECB_t_ECB {1} /* In micros (~73cc/128MHz in real HW)*/

#define NHW_CLKPWR_TOTAL_INST 1
#define NHW_CLKPWR_54L_MODEL 1
#define NHW_CLKPWR_0 0
#define NHW_CLKPWR_DPPI_MAP {3} /* LP */
#define NHW_CLKPWR_INT_MAP {{0, 270}} /* {App, CLOCK_POWER_IRQn} */
#define NHW_CLKPWR_HAS_POWER 1
#define NHW_CLKPWR_HAS_LFCLK 1
#define NHW_CLKPWR_HAS_PLL_24M 0
#define NHW_CLKPWR_CLK_MAX_N_SRCS 3 /* RF, XO, SYNT */
#define NHW_CLKPWR_N_CLKS 2
#define NHW_CLKPWR_CLK_IDX_LF 0
#define NHW_CLKPWR_CLK_IDX_XO 1
#define NHW_CLKPWR_CLK_START_TIMES {\
                                    {0, 0, 0},\
                                    {0},\
                                   }
                                    /* LFCLK,
                                       XO*/
#define NHW_CLKPWR_CLK_XOTUNE_TIMES {0, 16300000} /* Duration for a successful and failed tuning */
#define NHW_CLKPWR_CLK_CAL_TIME 0

#define NHW_CRACEN_TOTAL_INST 1
/* #define NHW_CRACEN_DDPI_MAP no PPI events from CRACEN */
#define NHW_CRACEN_INT_MAP {{0, 89}} /* {App, CRACEN_IRQn} */
#define NHW_CRACEN_RNG_G_log2fifodepth 4 /* log2 of the FIFO depth in 32bit words */
#define NHW_CRACEN_RNG_V 2 /* v2 = the CRACEN IP found in the 54LM20 or newer devices */
#define NHW_CRACEN_FREQ_MHZ 128
#define NHW_CRACEN_STARTUPTEST_DUR 4 /*In microseconds duration of the startup tests and other start overhead */
#define NHW_CRACEN_HAS_CM 0
#define NHW_CRACEN_HAS_PKEIG 0

#define NHW_DPPI_TOTAL_INST 4
#define NHW_DPPI_00 0 /* MCU */
#define NHW_DPPI_10 1 /* Radio */
#define NHW_DPPI_20 2 /* Peri */
#define NHW_DPPI_30 3 /* LP */
/* The DPPI does not generate interrupts */
#define NHW_DPPI_DPPI_MAP {0, 1, 2, 3} /* DPPI connect to themselves */
#define NHW_DPPI_N_CH {16, 24, 16, 4} /* Number of channels in each DPPI */
#define NHW_DPPI_N_CHG {2, 6, 6, 2}  /* Number of channel groups in each DPPI */

#define NHW_EGU_TOTAL_INST 3
#define NHW_EGU_00 0
#define NHW_EGU_10 1
#define NHW_EGU_20 2
#define NHW_EGU_INT_MAP {{0 , 88}, \
                         {0 , 135}, \
                         {0 , 201}, \
                        }
                        /* {App, EGU00}
                         * {App, EGU10}
                         * {App, EGU20}
                         * */
#define NHW_EGU_DPPI_MAP {0, 1, 2} /* MCU, Radio, Peri */
#define NHW_EGU_N_EVENTS {6, 16, 6}

#define NHW_GPIO_TOTAL_INST 2
#define NHW_GPIO_P0 0
#define NHW_GPIO_P1 1
#define NHW_GPIO_MAX_PINS_PER_PORT 32
#define NHW_GPIO_NBR_PINS {5, 32} /* Number of IOs per port */
#define NHW_GPIO_PARTNER_GPIOTE {1, 0} /* P0->GPIOTE30, P1->GPIOTE20 */
#define NHW_GPIO_HAS_PIN_SENSE {1, 1} /* Per instance, does it have pin sense/detect mechanism */

#define NHW_GPIOTE_TOTAL_INST 2
#define NHW_GPIOTE_20 0
#define NHW_GPIOTE_30 1
#define NHW_GPIOTE_MAX_CHANNELS 8    /* Maximum number of channels in any instance */
#define NHW_GPIOTE_CHANNELS {8, 4}   /* Number of channels per instance */
#define NHW_GPIOTE_N_INT 1 /* Number of interrupts lines, common for all instances */
#define NHW_GPIOTE_INT_MAP {{{0, 218}}, \
                            {{0, 268}}}
                            /* App, GPIOTE20_0_IRQn */
                            /* App, GPIOTE30_0_IRQn */
#define NHW_GPIOTE_DPPI_MAP {2, 3} /* GPIOTE20 in Peri , GPIOTE30 in LP */
#define NHW_GPIOTE_IS_54 1

#define NHW_GRTC_TOTAL_INST 1
#define NHW_GRTC_N_INT 4
#define NHW_GRTC_INT_MAP { \
                           {0 , 226}, \
                           {0 , 227}, \
                           {0 , 228}, \
                           {0 , 229}, \
                         }
                        /* {App, GRTC_0..3_IRQn} */
#define NHW_GRTC_DPPI_MAP {2 /* Peripheral domain */}
#define NHW_GRTC_N_CC 12
#define NHW_GRTC_N_DOMAINS 4
#define NHW_GRTC_SYSCOUNTER_BW 52
#define NHW_GRTC_HAS_CLKOUT 1
#define NHW_GRTC_HAS_PWM 1
#define NHW_GRTC_HAS_PASTCC 1


#define NHW_INTCTRL_TOTAL_INST 1
#define NHW_INTCTRL_MAX_INTLINES 271

/* These names are taken from the IRQn_Type in the MDK header */
#define NHW_INT_NAMES { [0 /*Application core*/] = { \
[28]="SWI00",\
[29]="SWI01",\
[30]="SWI02",\
[31]="SWI03",\
[74]="AAR00_CCM00",\
[75]="ECB00",\
[78]="RRAMC",\
[82]="CTRLAP",\
[84]="CM33SS",\
[85]="TIMER00",\
[88]="EGU00",\
[89]="CRACEN",\
[133]="TIMER10",\
[135]="EGU10",\
[138]="RADIO_0",\
[139]="RADIO_1",\
[198]="SERIAL20",\
[199]="SERIAL21",\
[200]="SERIAL22",\
[201]="EGU20",\
[202]="TIMER20",\
[210]="PWM20",\
[213]="SAADC",\
[215]="TEMP",\
[218]="GPIOTE20_0",\
[224]="QDEC20",\
[226]="GRTC_0",\
[227]="GRTC_1",\
[228]="GRTC_2",\
[229]="GRTC_3",\
[239]="TAMPC",\
[264]="WDT30",\
[268]="GPIOTE30_0",\
[270]="CLOCK_POWER",\
}}

#define NHW_PPIB_TOTAL_INST 8
#define NHW_PPIB_00 0
#define NHW_PPIB_01 1
#define NHW_PPIB_10 2
#define NHW_PPIB_11 3
#define NHW_PPIB_20 4
#define NHW_PPIB_21 5
#define NHW_PPIB_22 6
#define NHW_PPIB_30 7
#define NHW_PPIB_DPPI_MAP {0, 0, 1, 1, 2, 2, 2, 3}
#define NHW_PPIB_N_CH {12, 8, 12, 16, 8, 16, 4, 4}
#define NHW_PPIB_MATE {2,/*00->10*/ \
                       4,/*01->20*/ \
                       0,/*10->00*/ \
                       5,/*11->21*/ \
                       1,/*20->01*/ \
                       3,/*21->11*/ \
                       7,/*22->30*/ \
                       6,/*30->22*/ \
                       }
#define HWH_PPIB_HARDWIRESCHANNELS 0

#define NHW_RADIO_TOTAL_INST 1
#define NHW_RADIO_0 0
#define NHW_RADIO_N_INT 2
#define NHW_RADIO_INT_MAP {{0 , 138}, \
                           {0 , 139}}
                          /*{App, RADIO_0_IRQn},
                           *{App, RADIO_1_IRQn} */
#define NHW_RADIO_DPPI_MAP {1} /* Radio domain */
#define NHW_RADIO_ED_RSSIOFFS (-92)
#define NHW_RADIO_IS_54 1
#define NHW_RADIO_HAS_PLLEN 1
#define NHW_RADIO_HAS_15_4 0
#define NHW_RADIO_HAS_BLECODED 0
#define NHW_RADIO_HAS_DF 0
#define NHW_RADIO_HAS_CS 0

#define NHW_RRAMC_UICR_TOTAL_INST 1
#define NHW_RRAM_START_ADDR {0x00000000}
#define NHW_RRAM_SIZE       {(508*1024)}
#define NHW_UICR_SIZE {2560 /*bytes*/}

#define NHW_FICR_PART 0x054BE10B /* N54LS05B */
#define NHW_FICR_RRAM 508 /* KB */

#define NHW_SWI_TOTAL_INST 4

#define NHW_TEMP_TOTAL_INST 1
#define NHW_TEMP_APP0 0
#define NHW_TEMP_INT_MAP {{0 , 215}} /*App core,TEMP_IRQn*/
#define NHW_TEMP_DPPI_MAP {2} /*Peri domain*/
#define NHW_TEMP_t_TEMP 36 /* microseconds, Unknown, assuming by now same as 52833 & 5340 */
#define NHW_TEMP_FBITS  2 /* fractional bits => 0.25C resolution */

#define NHW_TIMER_TOTAL_INST 3
#define NHW_TIMER_00 0
#define NHW_TIMER_10 1
#define NHW_TIMER_20 2
#define NHW_TIMER_INT_MAP {{0 , 85}, \
                           {0 , 133}, \
                           {0 , 202}, \
                          } /* App, TIMER00..20_IRQn */
#define NHW_TIMER_HAS_ONE_SHOT 1
#define NHW_TIMER_N_CC {6, 8, 6}
#define NHW_TIMER_MAX_N_CC 8
#define NHW_TIMER_FREQ {128, 32, 16}
#define NHW_TIMER_DPPI_MAP {0, 1, 2}

#define NHW_UARTE_TOTAL_INST 3
#define NHW_UARTE20 0
#define NHW_UARTE21 1
#define NHW_UARTE22 2
#define NHW_UARTE_INT_MAP {{0 , 198}, \
                          {0 , 199}, \
                          {0 , 200}, \
                          } /* App, SERIAL20..22_IRQn,
                               */
#define NHW_UARTE_DPPI_MAP {2, 2, 2} /* 3xPeri */
#define NHW_UARTE_HAS_UART 0
#define NHW_UARTE_HAS_FRAMETIMEOUT 1
#define NHW_UARTE_HAS_MATCH 1
#define NHW_UARTE_N_MATCH {4, 4, 4}
#define NHW_UARTE_MAX_N_MATCH 4
#define NHW_UARTE_NAMES {"UARTE20", \
                         "UARTE21", \
                         "UARTE22", \
                        }
#define NHW_UARTE_54NAMING 1
#define NHW_UARTE_FLUSH_AMOUNT_BUG 0
#define NHW_UARTE_CLOCKS {16, 16, 16}

#define NHW_FAKE_TIMER_TOTAL_INST 1
#define NHW_FAKE_TIMER_INT_MAP {{0 , 0}} /*App core*/

#define NHW_BSTICKER_TOTAL_INST 1
#define NHW_BSTICKER_TIMER_INT_MAP {{0 , 0}} /*App core*/
