/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * The Application core/domain is indexed as domain 0
 * The Network core/domain is indexed as domain 1
 */

#define NHW_HAS_PPI  0
#define NHW_HAS_DPPI 1
#define NHW_USE_MDK_TYPES 0

#define NHW_AAR_TOTAL_INST 1
#define NHW_AAR_NET0 0
#define NHW_AAR_INT_MAP {{1 , 14}} /*Net core,AAR_CCM */
#define NHW_AAR_DPPI_MAP {1}
#define NHW_AAR_t_AAR    6

#define NHW_CCM_TOTAL_INST 1
#define NHW_CCM_NET0 0
#define NHW_CCM_INT_MAP {{1 , 14}} /*Net core,AAR_CCM*/
#define NHW_CCM_DPPI_MAP {1}

#define NHW_CLKPWR_TOTAL_INST 2
#define NHW_CLKPWR_APP0 0
#define NHW_CLKPWR_NET0 1
#define NHW_CLKPWR_INT_MAP {{0 , 5}, \
                            {1 , 5}  \
                           } /* {App, CLOCK_POWER}
                              * {Net, CLOCK_POWER}
                              */
#define NHW_CLKPWR_DPPI_MAP {0, 1}
#define NHW_CLKPWR_HAS_RESET 1
#define NHW_CLKPWR_HAS_CALTIMER 0
#define NHW_CLKPWR_HAS_HFCLKAUDIOCLK 1
#define NHW_CLKPWR_HAS_HFCLKAUDIOCLK_I {1, 0}
#define NHW_CLKPWR_HAS_HFCLK192MCLK 1
#define NHW_CLKPWR_HAS_HFCLK192MCLK_I  {1, 0}
#define NHW_CLKPWR_HAS_LFCLK 1
#define NHW_CLKPWR_CLK_MAX_N_SRCS 3
#define NHW_CLKPWR_N_CLKS 4
#define NHW_CLKPWR_CLK_IDX_LF      0
#define NHW_CLKPWR_CLK_IDX_HF      1
#define NHW_CLKPWR_CLK_IDX_HFAUDIO 2
#define NHW_CLKPWR_CLK_IDX_192     3
#define NHW_CLKPWR_CLK_START_TIMES {\
                                    {0, 0, 0},\
                                    {0},\
                                    {0},\
                                    {0}}
                                    /* LFCLK,
                                       HFCLK,
                                       HFAUDIO,
                                       HF192*/
#define NHW_CLKPWR_CLK_CAL_TIME 0

#define NHW_ECB_TOTAL_INST 1
#define NHW_ECB_NET0 0
#define NHW_ECB_INT_MAP {{1 , 13}} /*Net core, ECB_IRQn*/
#define NHW_ECB_DPPI_MAP {1}
#define NHW_ECB_t_ECB 6 /* 6.2 */

#define NHW_EGU_TOTAL_INST 7
#define NHW_EGU_APP0 0
#define NHW_EGU_APP1 1
#define NHW_EGU_APP2 2
#define NHW_EGU_APP3 3
#define NHW_EGU_APP4 4
#define NHW_EGU_APP5 5
#define NHW_EGU_NET0 6
#define NHW_EGU_INT_MAP {{0 , 27}, \
                         {0 , 28}, \
                         {0 , 29}, \
                         {0 , 30}, \
                         {0 , 31}, \
                         {0 , 32}, \
                         {1 , 20}, \
                        }
                        /* {App, EGU0}
                         * ..
                         * {App, EGU5}
                         * {Network, EGU0}
                         * */
#define NHW_EGU_DPPI_MAP {0, 0, 0, 0, 0, 0,\
                          1}
#define NHW_EGU_N_EVENTS {16, 16, 16, 16, 16, 16,\
                          16}

#define NHW_DPPI_TOTAL_INST 2
#define NHW_DPPI_APP_0 0
#define NHW_DPPI_NET_0 1
/* The DPPI does not generate interrupts */
#define NHW_DPPI_DPPI_MAP {0,1} /*App DPPI connects to itself, network DPPI to itself*/
#define NHW_DPPI_N_CH {32, 32} /* Number of channels in each DPPI */
#define NHW_DPPI_N_CHG {6, 6}  /* Number of channel groups in each DPPI */

#define NHW_IPC_TOTAL_INST 2
#define NHW_IPC_APP0 0
#define NHW_IPC_NET0 1
#define NHW_IPC_INT_MAP {{0 , 42}, \
                         {1 , 18}}
                        /* {App, IPC}
                         * {Network, IPC}
                         * */
#define NHW_IPC_DPPI_MAP {0, 1} /*App, network */
#define NHW_IPC_N_CH     {16, 16}

#define NHW_FICR_APP 0
#define NHW_FICR_NET 1

/* Note the net and app core ports are fully separate */
#define NHW_GPIO_TOTAL_INST 4
#define NHW_GPIO_NET_P0 0
#define NHW_GPIO_NET_P1 1
#define NHW_GPIO_APP_P0 2
#define NHW_GPIO_APP_P1 3
#define NHW_GPIO_MAX_PINS_PER_PORT 32
#define NHW_GPIO_NBR_PINS {32, 16, 32, 16} /* Number of IOs per port */
#define NHW_GPIO_PARTNER_GPIOTE {0, 0, 2, 2} /* NET_P0->GPIOT_NET, NET_P1->GPIOTE_NET */
                                             /* APP_P0/1->APP GPIOTE1 (NS)
                                              * (secure connection handled in code as special case)*/
#define NHW_GPIO_HAS_PIN_SENSE {1, 1, 1, 1} /* Per instance, does it have pin sense/detect mechanism */

#define NHW_GPIOTE_TOTAL_INST 3
#define NHW_GPIOTE_NET 0
#define NHW_GPIOTE_APP0 1 /* AKA GPIOTESEC */
#define NHW_GPIOTE_APP1 2
#define NHW_GPIOTE_MAX_CHANNELS 8    /* Maximum number of channels in any instance */
#define NHW_GPIOTE_CHANNELS {8, 8, 8}   /* Number of channels per instance */
#define NHW_GPIOTE_N_INT 1 /* Number of interrupts lines, common for all instances */
#define NHW_GPIOTE_INT_MAP {{{1, 10}},\
                            {{0, 13}},\
                            {{0, 47}}}
                            /* Net, GPIOTE_IRQn */
                            /* App, GPIOTE0_IRQn */
                            /* App, GPIOTE1_IRQn */
#define NHW_GPIOTE_DPPI_MAP {1, 0 ,0} /* Net, App, App */
#define NHW_GPIOTE_IS_54 0 /* Multiple int. lines and PORTxSECURE/NONSECURE */

#define NHW_INTCTRL_TOTAL_INST 2
#define NHW_INTCTRL_MAX_INTLINES 58

/* These names are taken from the IRQn_Type in the MDK header */
#define NHW_INT_NAMES { [0 /*Application core*/] = {\
[0 ]= "FPU",\
[1 ]= "CACHE",\
[3 ]= "SPU",\
[5 ]= "CLOCK_POWER",\
[8 ]= "SERIAL0",\
[9 ]= "SERIAL1",\
[10]= "SPIM4",\
[11]= "SERIAL2",\
[12]= "SERIAL3",\
[13]= "GPIOTE0",\
[14]= "SAADC",\
[15]= "TIMER0",\
[16]= "TIMER1",\
[17]= "TIMER2",\
[20]= "RTC0",\
[21]= "RTC1",\
[24]= "WDT0",\
[25]= "WDT1",\
[26]= "COMP_LPCOMP",\
[27]= "EGU0",\
[28]= "EGU1",\
[29]= "EGU2",\
[30]= "EGU3",\
[31]= "EGU4",\
[32]= "EGU5",\
[33]= "PWM0",\
[34]= "PWM1",\
[35]= "PWM2",\
[36]= "PWM3",\
[38]= "PDM0",\
[40]= "I2S0",\
[42]= "IPC",\
[43]= "QSPI",\
[45]= "NFCT",\
[47]= "GPIOTE1",\
[51]= "QDEC0",\
[52]= "QDEC1",\
[54]= "USBD",\
[55]= "USBREGULATOR",\
[57]= "KMU",\
/*[68]= "CRYPTOCELL",*/\
}, [1 /*Network core*/] = {\
[5 ] = "CLOCK_POWER",\
[8 ] = "RADIO",\
[9 ] = "RNG",\
[10] = "GPIOTE",\
[11] = "WDT",\
[12] = "TIMER0",\
[13] = "ECB",\
[14] = "AAR_CCM",\
[16] = "TEMP",\
[17] = "RTC0",\
[18] = "IPC",\
[19] = "SERIAL0",\
[20] = "EGU0",\
[22] = "RTC1",\
[24] = "TIMER1",\
[25] = "TIMER2",\
[26] = "SWI0",\
[27] = "SWI1",\
[28] = "SWI2",\
[29] = "SWI3",\
}}

#define NHW_CORE_NAMES {"Application", "Network"}

#define NHW_NVMC_UICR_TOTAL_INST 2
#define NHW_NVMC_APP0 0
#define NHW_NVMC_NET0 1
#define NHW_UICR_APP0 0
#define NHW_UICR_NET0 1
#define NHW_NVMC_HAS_ERASEREGS 0
#define NHW_FLASH_START_ADDR {0x00000000, 0x01000000}
#define NHW_FLASH_PAGESIZE {(4*1024), (2*1024)}
#define NHW_FLASH_N_PAGES {256, 128}
#define NHW_UICR_SIZE {4096, 800 /*bytes*/}
         //App UICR size including the KEYSLOT
#define NHW_NVMC_FLASH_T_ERASEALL (173000)
#define NHW_NVMC_FLASH_T_ERASEPAGE (87500)
#define NHW_NVMC_FLASH_T_WRITE        (43)
#define NHW_NVMC_FLASH_PARTIAL_ERASE_FACTOR (1.0)

#define NHW_RADIO_TOTAL_INST 1
#define NHW_RADIO_NET0 0
#define NHW_RADIO_N_INT 1
#define NHW_RADIO_INT_MAP {{1 , 8}} /*Net core,RADIO_IRQn*/
#define NHW_RADIO_DPPI_MAP {1} /*Network core*/
#define NHW_RADIO_ED_RSSIOFFS (-93)
#define NHW_RADIO_IS_54 0
#define NHW_RADIO_HAS_15_4 1
#define NHW_RADIO_HAS_BLECODED 1
#define NHW_RADIO_HAS_DF 1
#define NHW_RADIO_HAS_CS 0

#define NHW_RNG_TOTAL_INST 1
#define NHW_RNG_NET_0 0
#define NHW_RNG_INT_MAP  {{1, 9}} /*Network core, "RNG_IRQn"*/
#define NHW_RNG_DPPI_MAP {1} /*Network core*/
#define NHW_RNG_tRNG_START 128
#define NHW_RNG_tRNG_RAW 32
#define NHW_RNG_tRNG_BC 122

#define NHW_RTC_TOTAL_INST 4
#define NHW_RTC_APP0 0
#define NHW_RTC_APP1 1
#define NHW_RTC_NET0 2
#define NHW_RTC_NET1 3
#define NHW_RTC_INT_MAP {{0 , 20}, \
                         {0 , 21}, \
                         {1 , 17}, \
                         {1 , 22}, \
                         } /*App core,RTC0..1_IRQn*/
                           /*Net core,RTC0..1_IRQn*/
#define NHW_RTC_DPPI_MAP {0, 0, 1, 1} /*2xApp core, 2xNetwork core*/
#define NHW_RTC_HAS_CAPTURE 1
#define NHW_RTC_HAS_SHORT_COMP_CLEAR 1
#define NHW_RTC_N_CC {4, 4, 4, 4}

#define NHW_SPU_TOTAL_INST 1
#define NHW_SPU_APP0 0

#define NHW_SWI_TOTAL_INST 4
#define NHW_SWI_NET0 0
#define NHW_SWI_NET1 1
#define NHW_SWI_NET2 2
#define NHW_SWI_NET3 3

#define NHW_TEMP_TOTAL_INST 1
#define NHW_TEMP_NET0 0
#define NHW_TEMP_INT_MAP {{1 , 16}} /*Net core,TEMP_IRQn*/
#define NHW_TEMP_DPPI_MAP {1} /*Network core*/
#define NHW_TEMP_t_TEMP 36 /* microseconds */
#define NHW_TEMP_FBITS  2 /* fractional bits => 0.25C resolution */

#define NHW_TIMER_TOTAL_INST 6
#define NHW_TIMER_APP0 0
#define NHW_TIMER_APP1 1
#define NHW_TIMER_APP2 2
#define NHW_TIMER_NET0 3
#define NHW_TIMER_NET1 4
#define NHW_TIMER_NET2 5
#define NHW_TIMER_INT_MAP {{0 , 15}, \
                           {0 , 16}, \
                           {0 , 17}, \
                           {1 , 12}, \
                           {1 , 24}, \
                           {1 , 25}, \
                          } /* AppCore, TIMER0..2_IRQn */
                            /* NetCore, TIMER0..2_IRQn */
#define NHW_TIMER_HAS_ONE_SHOT 1
#define NHW_TIMER_N_CC {6, 6, 6, 8, 8, 8}
#define NHW_TIMER_MAX_N_CC 8
#define NHW_TIMER_FREQ {16, 16, 16, 16, 16, 16}
#define NHW_TIMER_DPPI_MAP {0, 0, 0, 1, 1, 1} /*3xApp core, 3xNetwork core*/

#define NHW_UARTE_TOTAL_INST 5
#define NHW_UARTE_APP0 0
#define NHW_UARTE_APP1 1
#define NHW_UARTE_APP2 2
#define NHW_UARTE_APP3 3
#define NHW_UARTE_NET0 4
#define NHW_UARTE_INT_MAP {{0 , 8}, \
                          {0 , 9}, \
                          {0 , 11}, \
                          {0 , 12}, \
                          {1 , 19}, \
                          } /* App core, SERIAL0..3_IRQn,
                               Net core, SERIAL0_IRQn */
#define NHW_UARTE_DPPI_MAP {0, 0, 0, 0, 1} /*4xApp core, 1xNetwork core*/
#define NHW_UARTE_HAS_UART 0
#define NHW_UARTE_NAMES {"App UARTE0", \
                         "App UARTE1", \
                         "App UARTE2", \
                         "App UARTE3", \
                         "Net UARTE0"}
#define NHW_UARTE_54NAMING 0
#define NHW_UARTE_FLUSH_AMOUNT_BUG 0
#define NHW_UARTE_HAS_FRAMETIMEOUT 0
#define NHW_UARTE_HAS_MATCH 0
#define NHW_UARTE_CLOCKS {16, 16, 16, 16, 16}

#define NHW_FAKE_TIMER_TOTAL_INST 2
#define NHW_FAKE_TIMER_INT_MAP {{0 , 0}, \
                                {1 , 0}} /*App core & Net core, -*/

#define NHW_BSTICKER_TOTAL_INST 2
#define NHW_BSTICKER_TIMER_INT_MAP {{0 , 0}, \
                                    {1 , 0}} /*App core & Net core, -*/

#define NHW_APPCORE_RAM_ADDR 0x20000000
#define NHW_APPCORE_RAM_SIZE 0x80000 /*512KiB*/
