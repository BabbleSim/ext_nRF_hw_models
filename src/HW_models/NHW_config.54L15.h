/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define NHW_HAS_PPI  0
#define NHW_HAS_DPPI 1
#define NHW_USE_MDK_TYPES 1 /* The HW registers layout types are taken from the MDK */
#define NHW_CORE_NAMES {"Application", "Flipper"}

#define NHW_AARCCMECB_TOTAL_INST 1
#define NHW_AARCCMECB_00 0
#define NHW_AARCCMECB_DPPI_MAP {0} /* Global */
#define NHW_AARCCM_INTMAP {{-1, 70}}
                         /* Router, AAR00_CCM00_IRQn */
#define NHW_ECB_INTMAP {{-1, 71}}
                        /* ECB00_IRQn */
#define NHW_AARCCMECB_CLOCK {128} /* MHz */
#define NHW_ECB_t_ECB {1} /* In micros (~73cc/128MHz in real HW)*/

#define NHW_CLKPWR_TOTAL_INST 1
#define NHW_CLKPWR_54L_MODEL 1
#define NHW_CLKPWR_0 0
#define NHW_CLKPWR_DPPI_MAP {0} /* Global */
#define NHW_CLKPWR_INT_MAP {{-1, 261}} /* {Router, CLOCK_POWER_IRQn} */
#define NHW_CLKPWR_HAS_POWER 1
#define NHW_CLKPWR_HAS_LFCLK 1
#define NHW_CLKPWR_CLK_MAX_N_SRCS 3
#define NHW_CLKPWR_N_CLKS 2
#define NHW_CLKPWR_CLK_IDX_LF 0
#define NHW_CLKPWR_CLK_IDX_XO 1
#define NHW_CLKPWR_CLK_START_TIMES {\
                                    {0, 0, 0},\
                                    {0}}
                                    /* LFCLK,
                                       XO*/
#define NHW_CLKPWR_CLK_XOTUNE_TIMES {0, 16300000} /* Duration for a successful and failed tuning */
#define NHW_CLKPWR_CLK_CAL_TIME 0

#define NHW_CRACEN_TOTAL_INST 1
/* #define NHW_CRACEN_DDPI_MAP no PPI events from CRACEN */
#define NHW_CRACEN_INT_MAP {{-1, 72}} /* {Router, CRACEN_IRQn} */
#define NHW_CRACEN_RNG_G_log2fifodepth 4 /* log2 of the FIFO depth in 32bit words */
#define NHW_CRACEN_RNG_V 1
#define NHW_CRACEN_FREQ_MHZ 128
#define NHW_CRACEN_STARTUPTEST_DUR 4 /*In microseconds duration of the startup tests and other start overhead */
#define NHW_CRACEN_CM_AES_t_ECB {1, 1, 1} /* In microseconds time it takes for key size of 128, 192 and 256 */
                                          /* The real time would be < ~0.25 micros for the ECB itself and another so much for the DMA */
#define NHW_CRACEN_HAS_CM 1
#define NHW_CRACEN_HAS_PKEIG 1

#define NHW_DPPI_TOTAL_INST 4
#define NHW_DPPI_00 0 /* Global */
#define NHW_DPPI_10 1 /* Radio */
#define NHW_DPPI_20 2 /* Peri */
#define NHW_DPPI_30 3 /* LP */
/* The DPPI does not generate interrupts */
#define NHW_DPPI_DPPI_MAP {0, 1, 2, 3} /* DPPI connect to themselves */
#define NHW_DPPI_N_CH {8, 24, 16, 4} /* Number of channels in each DPPI */
#define NHW_DPPI_N_CHG {2, 6, 6, 2}  /* Number of channel groups in each DPPI */

#define NHW_EGU_TOTAL_INST 2
#define NHW_EGU_10 0
#define NHW_EGU_20 1
#define NHW_EGU_INT_MAP {{-1 , 135}, \
                         {-1 , 201}, \
                        }
                        /* {Router, EGU10}
                         * {Router, EGU20}
                         * */
#define NHW_EGU_DPPI_MAP {1, 2} /* Radio, Peri */
#define NHW_EGU_N_EVENTS {16, 6}

#define NHW_GPIO_TOTAL_INST 3
#define NHW_GPIO_P0 0
#define NHW_GPIO_P1 1
#define NHW_GPIO_P2 2
#define NHW_GPIO_MAX_PINS_PER_PORT 32
#define NHW_GPIO_NBR_PINS {7, 17, 11} /* Number of IOs per port */
#define NHW_GPIO_PARTNER_GPIOTE {1, 0, -1} /* P0->GPIOTE30, P1->GPIOTE20, P2->nothing */
#define NHW_GPIO_HAS_PIN_SENSE {1, 1, 0} /* Per instance, does it have pin sense/detect mechanism */

#define NHW_GPIOTE_TOTAL_INST 2
#define NHW_GPIOTE_20 0
#define NHW_GPIOTE_30 1
#define NHW_GPIOTE_MAX_CHANNELS 8    /* Maximum number of channels in any instance */
#define NHW_GPIOTE_CHANNELS {8, 4}   /* Number of channels per instance */
#define NHW_GPIOTE_N_INT 2 /* Number of interrupts lines, common for all instances */
#define NHW_GPIOTE_INT_MAP {{{-1, 218},  \
                             {-1, 219}}, \
                            {{-1, 268},  \
                             {-1, 269}}}
                            /* Router, GPIOTE20_0_IRQn */
                            /* Router, GPIOTE20_1_IRQn */
                            /* Router, GPIOTE30_0_IRQn */
                            /* Router, GPIOTE30_1_IRQn */
#define NHW_GPIOTE_DPPI_MAP {2, 3} /* GPIOTE20 in Peri , GPIOTE30 in LP */
#define NHW_GPIOTE_IS_54 1

#define NHW_GRTC_TOTAL_INST 1
#define NHW_GRTC_N_INT 4
#define NHW_GRTC_INT_MAP { \
                           {-1, 226}, \
                           {-1, 227}, \
                           {-1, 228}, \
                           {-1, 229}, \
                         }
                        /* {Router, GRTC_0..3_IRQn} */
#define NHW_GRTC_DPPI_MAP {2 /* Peripheral domain */}
#define NHW_GRTC_N_CC 12
#define NHW_GRTC_N_DOMAINS 4
#define NHW_GRTC_SYSCOUNTER_BW 52
#define NHW_GRTC_HAS_CLKOUT 0
#define NHW_GRTC_HAS_PWM 1


#define NHW_INTCTRL_TOTAL_INST 2
#define NHW_INTCTRL_MAX_INTLINES 271

/* IRQ Controllers are 0: M33 NVIC ; 1: FLPR */
#define NHW_HAS_INTROUTER 1
#define NHW_IRQRTR_NBR_GLB_LINES NHW_INTCTRL_MAX_INTLINES

/* Note: This router irq numbering matches the M33 numbering for simplicity */
#define NHW_IRQRTR_MAPPING { \
  /* Preset the whole table to -2 (HW_IRQR_DISCONNECTED), 0 */ \
  [0 ... NHW_IRQRTR_NBR_GLB_LINES - 1][0 ... NHW_INTCTRL_TOTAL_INST - 1] = {-2, 0}, \
/* Table format : */ \
/* [Router global interrupt nbr] = { {ctrl,line}, {ctrl,line}..} \ */ \
  [64  /* SPU00        */] = {{0, 64 }, {1, 64 }}, \
  [65  /* MPC00        */] = {{0, 65 }, {1, 65 }}, \
  [70  /* AAR00_CCM00  */] = {{0, 70 }, {1, 70 }}, \
  [71  /* ECB00        */] = {{0, 71 }, {1, 71 }}, \
  [72  /* CRACEN       */] = {{0, 72 }, {1, 72 }}, \
  [74  /* SERIAL00     */] = {{0, 74 }, {1, 74 }}, \
  [75  /* RRAMC        */] = {{0, 75 }, {1, 75 }}, \
  [76  /* VPR00        */] = {{0, 76 }, {1, 76 }}, \
  [82  /* CTRLAP       */] = {{0, 82 }, {1, 82 }}, \
  [84  /* CM33SS       */] = {{0, 84 }, {1, 84 }}, \
  [85  /* TIMER00      */] = {{0, 85 }, {1, 85 }}, \
  [128 /*  SPU10       */] = {{0, 128}, {1, 128}}, \
  [133 /*  TIMER10     */] = {{0, 133}, {1, 133}}, \
  [135 /*  EGU10       */] = {{0, 135}, {1, 135}}, \
  [138 /*  RADIO_0     */] = {{0, 138}, {1, 138}}, \
  [139 /*  RADIO_1     */] = {{0, 139}, {1, 139}}, \
  [192 /*  SPU20       */] = {{0, 192}, {1, 192}}, \
  [198 /*  SERIAL20    */] = {{0, 198}, {1, 198}}, \
  [199 /*  SERIAL21    */] = {{0, 199}, {1, 199}}, \
  [200 /*  SERIAL22    */] = {{0, 200}, {1, 200}}, \
  [201 /*  EGU20       */] = {{0, 201}, {1, 201}}, \
  [202 /*  TIMER20     */] = {{0, 202}, {1, 202}}, \
  [203 /*  TIMER21     */] = {{0, 203}, {1, 203}}, \
  [204 /*  TIMER22     */] = {{0, 204}, {1, 204}}, \
  [205 /*  TIMER23     */] = {{0, 205}, {1, 205}}, \
  [206 /*  TIMER24     */] = {{0, 206}, {1, 206}}, \
  [208 /*  PDM20       */] = {{0, 208}, {1, 208}}, \
  [209 /*  PDM21       */] = {{0, 209}, {1, 209}}, \
  [210 /*  PWM20       */] = {{0, 210}, {1, 210}}, \
  [211 /*  PWM21       */] = {{0, 211}, {1, 211}}, \
  [212 /*  PWM22       */] = {{0, 212}, {1, 212}}, \
  [213 /*  SAADC       */] = {{0, 213}, {1, 213}}, \
  [214 /*  NFCT        */] = {{0, 214}, {1, 214}}, \
  [215 /*  TEMP        */] = {{0, 215}, {1, 215}}, \
  [218 /*  GPIOTE20_0  */] = {{0, 218}, {1, 218}}, \
  [219 /*  GPIOTE20_1  */] = {{0, 219}, {1, 219}}, \
  [220 /*  TAMPC       */] = {{0, 220}, {1, 220}}, \
  [221 /*  I2S20       */] = {{0, 221}, {1, 221}}, \
  [224 /*  QDEC20      */] = {{0, 224}, {1, 224}}, \
  [225 /*  QDEC21      */] = {{0, 225}, {1, 225}}, \
  [226 /*  GRTC_0      */] = {{0, 226}, {1, 226}}, \
  [227 /*  GRTC_1      */] = {{0, 227}, {1, 227}}, \
  [228 /*  GRTC_2      */] = {{0, 228}, {1, 228}}, \
  [229 /*  GRTC_3      */] = {{0, 229}, {1, 229}}, \
  [256 /*  SPU30       */] = {{0, 256}, {1, 256}}, \
  [260 /*  SERIAL30    */] = {{0, 260}, {1, 260}}, \
  [261 /*  CLOCK_POWER */] = {{0, 261}, {1, 261}}, \
  [262 /*  COMP_LPCOMP */] = {{0, 262}, {1, 262}}, \
  [264 /*  WDT30       */] = {{0, 264}, {1, 264}}, \
  [265 /*  WDT31       */] = {{0, 265}, {1, 265}}, \
  [268 /*  GPIOTE30_0  */] = {{0, 268}, {1, 268}}, \
  [269 /*  GPIOTE30_1  */] = {{0, 269}, {1, 269}}, \
}

/* These names are taken from the IRQn_Type in the MDK header */
#define NHW_INT_NAMES { [0 /*Application core*/] = {\
[28 ]="SWI00",\
[29 ]="SWI01",\
[30 ]="SWI02",\
[31 ]="SWI03",\
[64 ]="SPU00",\
[65 ]="MPC00",\
[70 ]="AAR00_CCM00",\
[71 ]="ECB00",\
[72 ]="CRACEN",\
[74 ]="SERIAL00",\
[75 ]="RRAMC",\
[76 ]="VPR00",\
[82 ]="CTRLAP",\
[83 ]="CM33SS",\
[85 ]="TIMER00",\
[128]="SPU10",\
[133]="TIMER10",\
[134]="-",\
[135]="EGU10",\
[138]="RADIO_0",\
[139]="RADIO_1",\
[192]="SPU20",\
[198]="SERIAL20",\
[199]="SERIAL21",\
[200]="SERIAL22",\
[201]="EGU20",\
[202]="TIMER20",\
[203]="TIMER21",\
[204]="TIMER22",\
[205]="TIMER23",\
[206]="TIMER24",\
[208]="PDM20",\
[209]="PDM21",\
[210]="PWM20",\
[211]="PWM21",\
[212]="PWM22",\
[213]="SAADC",\
[214]="NFCT",\
[215]="TEMP",\
[218]="GPIOTE20_0",\
[219]="GPIOTE20_1",\
[220]="TAMPC",\
[221]="I2S20",\
[224]="QDEC20",\
[225]="QDEC21",\
[226]="GRTC_0",\
[227]="GRTC_1",\
[228]="GRTC_2",\
[229]="GRTC_3",\
[256]="SPU30",\
[260]="SERIAL30",\
[261]="CLOCK_POWER",\
[262]="COMP_LPCOMP",\
[264]="WDT30",\
[265]="WDT31",\
[268]="GPIOTE30_0",\
[269]="GPIOTE30_1",\
[270]="CLOCK_POWER",\
}, [1 /*Flipper core*/] = {\
[0  ]="VPRCLIC_0",\
[1  ]="VPRCLIC_1",\
[2  ]="VPRCLIC_2",\
[3  ]="VPRCLIC_3",\
[4  ]="VPRCLIC_4",\
[5  ]="VPRCLIC_5",\
[6  ]="VPRCLIC_6",\
[7  ]="VPRCLIC_7",\
[8  ]="VPRCLIC_8",\
[9  ]="VPRCLIC_9",\
[10 ]="VPRCLIC_10",\
[11 ]="VPRCLIC_11",\
[12 ]="VPRCLIC_12",\
[13 ]="VPRCLIC_13",\
[14 ]="VPRCLIC_14",\
[15 ]="VPRCLIC_15",\
[16 ]="VPRCLIC_16",\
[17 ]="VPRCLIC_17",\
[18 ]="VPRCLIC_18",\
[19 ]="VPRCLIC_19",\
[20 ]="VPRCLIC_20",\
[21 ]="VPRCLIC_21",\
[22 ]="VPRCLIC_22",\
[23 ]="VPRCLIC_23",\
[24 ]="VPRCLIC_24",\
[25 ]="VPRCLIC_25",\
[26 ]="VPRCLIC_26",\
[27 ]="VPRCLIC_27",\
[28 ]="VPRCLIC_28",\
[29 ]="VPRCLIC_29",\
[30 ]="VPRCLIC_30",\
[31 ]="VPRCLIC_31",\
[64 ]="SPU00",\
[65 ]="MPC00",\
[70 ]="AAR00_CCM00",\
[71 ]="ECB00",\
[72 ]="CRACEN",\
[74 ]="SERIAL00",\
[75 ]="RRAMC",\
[76 ]="VPR00",\
[82 ]="CTRLAP",\
[85 ]="TIMER00",\
[128]="SPU10",\
[133]="TIMER10",\
[134]="-",\
[135]="EGU10",\
[138]="RADIO_0",\
[139]="RADIO_1",\
[192]="SPU20",\
[198]="SERIAL20",\
[199]="SERIAL21",\
[200]="SERIAL22",\
[201]="EGU20",\
[202]="TIMER20",\
[203]="TIMER21",\
[204]="TIMER22",\
[205]="TIMER23",\
[206]="TIMER24",\
[208]="PDM20",\
[209]="PDM21",\
[210]="PWM20",\
[211]="PWM21",\
[212]="PWM22",\
[213]="SAADC",\
[214]="NFCT",\
[215]="TEMP",\
[218]="GPIOTE20_0",\
[219]="GPIOTE20_1",\
[220]="TAMPC",\
[221]="I2S20",\
[224]="QDEC20",\
[225]="QDEC21",\
[226]="GRTC_0",\
[227]="GRTC_1",\
[228]="GRTC_2",\
[229]="GRTC_3",\
[256]="SPU30",\
[260]="SERIAL30",\
[261]="CLOCK_POWER",\
[262]="COMP_LPCOMP",\
[264]="WDT30",\
[265]="WDT31",\
[268]="GPIOTE30_0",\
[269]="GPIOTE30_1",\
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
#define NHW_PPIB_N_CH {8, 8, 8, 16, 8, 16, 4, 4}
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
#define NHW_RADIO_INT_MAP {{-1, 138}, \
                           {-1, 139}}
                          /*{Router, RADIO_0_IRQn},
                           *{Router, RADIO_1_IRQn} */
#define NHW_RADIO_DPPI_MAP {1} /* Radio domain */
#define NHW_RADIO_ED_RSSIOFFS (-92)
#define NHW_RADIO_IS_54 1
#define NHW_RADIO_HAS_PLLEN 1
#define NHW_RADIO_HAS_15_4 1
#define NHW_RADIO_HAS_BLECODED 1
#define NHW_RADIO_HAS_DF 1
#define NHW_RADIO_HAS_CS 1

#define NHW_RRAMC_UICR_TOTAL_INST 1
#define NHW_RRAM_START_ADDR {0x00000000}
#define NHW_RRAM_SIZE       {(1524*1024)}
#define NHW_UICR_SIZE {2560 /*bytes*/}

#define NHW_FICR_PART 0x00054B15
#define NHW_FICR_RRAM 1524 /* KB */

#define NHW_SPU_TOTAL_INST 4
#define NHW_SPU_00 0
#define NHW_SPU_10 1
#define NHW_SPU_20 2
#define NHW_SPU_30 3

#define NHW_SWI_TOTAL_INST 4

#define NHW_TEMP_TOTAL_INST 1
#define NHW_TEMP_APP0 0
#define NHW_TEMP_INT_MAP {{-1, 215}} /*Router,TEMP_IRQn*/
#define NHW_TEMP_DPPI_MAP {2} /*Peri domain*/
#define NHW_TEMP_t_TEMP 36 /* microseconds, Unknown, assuming by now same as 52833 & 5340 */
#define NHW_TEMP_FBITS  2 /* fractional bits => 0.25C resolution */

#define NHW_TIMER_TOTAL_INST 7
#define NHW_TIMER_00 0
#define NHW_TIMER_10 1
#define NHW_TIMER_20 2
#define NHW_TIMER_21 3
#define NHW_TIMER_22 4
#define NHW_TIMER_23 5
#define NHW_TIMER_24 6
#define NHW_TIMER_INT_MAP {{-1, 85}, \
                           {-1, 133}, \
                           {-1, 202}, \
                           {-1, 203}, \
                           {-1, 204}, \
                           {-1, 205}, \
                           {-1, 206}, \
                          } /* Router, TIMER00..24_IRQn */
#define NHW_TIMER_HAS_ONE_SHOT 1
#define NHW_TIMER_N_CC {6, 8, 6, 6, 6, 6, 6}
#define NHW_TIMER_MAX_N_CC 8
#define NHW_TIMER_FREQ {128, 32, 16, 16, 16, 16, 16}
#define NHW_TIMER_DPPI_MAP {0, 1, 2, 2, 2, 2, 2}

#define NHW_PWM_TOTAL_INST 3
#define NHW_PWM_20 0
#define NHW_PWM_21 1
#define NHW_PWM_22 2
#define NHW_PWM_INT_MAP {{0, 210}, {0, 211}, {0, 212}} /* PWM20..22_IRQn */
#define NHW_PWM_DPPI_MAP {2, 2, 2} /* All in the Peripheral domain => DPPIC20 */
#define NHW_PWM_GPIO_PORT {NHW_GPIO_P1, NHW_GPIO_P1, NHW_GPIO_P1} /* The PWM pins must be in the same domain: P1 */

#define NHW_UARTE_TOTAL_INST 5
#define NHW_UARTE00 0
#define NHW_UARTE20 1
#define NHW_UARTE21 2
#define NHW_UARTE22 3
#define NHW_UARTE30 4
#define NHW_UARTE_INT_MAP {{-1, 74}, \
                          {-1, 198}, \
                          {-1, 199}, \
                          {-1, 200}, \
                          {-1, 260}, \
                          } /* Router, SERIAL00_IRQn,
                               Router, SERIAL20_IRQn,
                               Router, SERIAL21_IRQn,
                               Router, SERIAL22_IRQn,
                               Router, SERIAL30_IRQn,
                               */
#define NHW_UARTE_DPPI_MAP {0, 2, 2, 2, 3} /* Global, 4xPeri, LP */
#define NHW_UARTE_HAS_UART 0
#define NHW_UARTE_HAS_FRAMETIMEOUT 1
#define NHW_UARTE_HAS_MATCH 1
#define NHW_UARTE_N_MATCH {4, 4, 4, 4, 4}
#define NHW_UARTE_MAX_N_MATCH 4
#define NHW_UARTE_NAMES {"UARTE00", \
                         "UARTE20", \
                         "UARTE21", \
                         "UARTE22", \
                         "UARTE30"}
#define NHW_UARTE_54NAMING 1
#define NHW_UARTE_FLUSH_AMOUNT_BUG 0
#define NHW_UARTE_CLOCKS {128, 16, 16, 16, 16}

#define NHW_VPR_TOTAL_INST 1
#define NHW_VPR00 0
#define NHW_VPR_CPU_NBRS {1} /* VPR00 is expected to be the CPU nbr 1 */
#define NHW_VPR_VEIF_MAX_TASKSEVENTS 32 /* For all instances */
#define NHW_VPR_VEIF_BASE_TASKSEVENTS_MASK {0x007F0000} /* Overall available TASKS/EVENTS */
#define NHW_VPR_VEIF_DPPI_MASK {0x000F0000} /* Which are connected to the DPPI */
#define NHW_VPR_VEIF_EXTIRQ_MASK {0x00100000}  /* Which EVENTS drive the external IRQ line */
#define NHW_VPR_VEIF_CLICIRQ_MASK {0x007F0000} /* Which TASKS are connected to the CLIC */

#define NHW_VPR_INT_MAP {{-1, 76}, \
                        } /* Router, VPR00_IRQn */
#define NHW_VPR_CLIC_INT_MAP {{ \
 {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, \
 {1, 5}, {1, 6}, {1, 7}, {1, 8}, {1, 9}, \
 {1, 10}, {1, 11}, {1, 12}, {1, 13}, {1, 14}, \
 {1, 15}, {1, 16}, {1, 17}, {1, 18}, {1, 19}, \
 {1, 20}, {1, 21}, {1, 22}, {1, 23}, {1, 24}, \
 {1, 25}, {1, 26}, {1, 27}, {1, 28}, {1, 29}, \
 {1, 30}, {1, 31}, \
}} /* CLIC, VPRCLIC_0_IRQn..VPRCLIC_31_IRQn */

#define NHW_VPR_DPPI_MAP {0} /* Global */
#define NHW_VPR_DPPI_CHANNELS_TASKS { \
  [0 ... NHW_VPR_TOTAL_INST - 1][0 ... NHW_VPR_VEIF_MAX_TASKSEVENTS - 1] = -1, \
  [0][16] = 0,\
  [0][17] = 1,\
  [0][18] = 2,\
  [0][19] = 3,\
}
#define NHW_VPR_DPPI_CHANNELS_EVENTS { \
  [0 ... NHW_VPR_TOTAL_INST - 1][0 ... NHW_VPR_VEIF_MAX_TASKSEVENTS - 1] = -1, \
  [0][16] = 0,\
  [0][17] = 1,\
  [0][18] = 2,\
  [0][19] = 3,\
}

#define NHW_FAKE_TIMER_TOTAL_INST 2
#define NHW_FAKE_TIMER_INT_MAP {{0 , 0}, \
                                {1 , 0}} /*App core & flpr core, -*/

#define NHW_BSTICKER_TOTAL_INST 2
#define NHW_BSTICKER_TIMER_INT_MAP {{0 , 0}, \
                                    {1 , 0}} /*App core & flpr core, -*/
