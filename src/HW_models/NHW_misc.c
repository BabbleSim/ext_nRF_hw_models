/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "bs_tracing.h"
#include "bs_oswrap.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_virt_RAM.h"
#include "nrf_bsim_redef.h"
#include "nsi_tasks.h"

#if defined(NRF5340)
/* The nrf_bsim_redef.h header which is meant to hack the
 * nrf definitions only provides declarations for the
 * registers structures for a given core. So we need to declare
 * the ones that are exclusive for either core here.
 *
 * If at some point we have more uses for a bulk peripheral definitions
 * of all peripherals in a SOC consider moving it to a separate header
 */
extern NRF_AAR_Type NRF_AAR_regs;
extern NRF_CCM_Type NRF_CCM_regs;
extern NRF_ECB_Type NRF_ECB_regs;
extern NRF_NFCT_Type NRF_NFCT_regs;
extern NRF_RADIO_Type NRF_RADIO_regs;
extern NRF_RNG_Type NRF_RNG_regs;
extern int NRF_SWI_regs[];
extern NRF_TEMP_Type NRF_TEMP_regs;
extern NRF_VREQCTRL_Type NRF_VREQCTRL_regs;
extern NRF_SPU_Type NRF_SPU_regs[];
#endif

/*
 * Get the name of a core/domain
 *
 * Only for debugging/logging/tracing purposes.
 */
const char *nhw_get_core_name(unsigned int core_n)
{
  static const char *corenames[NHW_INTCTRL_TOTAL_INST] = NHW_CORE_NAMES;

  if (core_n < sizeof(corenames)/sizeof(corenames[0])) {
    return corenames[core_n];
  } else {
    return NULL;
  }
}

/*
 * Convert an address to the real embedded device RAM into an addresses
 * in the models buffer which is used in place of the RAM.
 * Note the input is a pointer to the pointer/address, which will be modified in the call.
 *
 * This function will only modify the address if it was inside an embedded RAM known to it.
 * In that case it returns true.
 * Otherwise the address is unmodified and false is returned.
 */
bool nhw_convert_RAM_addr(void **addr)
{
#if defined(NRF5340) || defined(NRF5340_XXAA_NETWORK) || defined(NRF5340_XXAA_APPLICATION)
  if (((intptr_t)*addr >= NHW_APPCORE_RAM_ADDR) && ((intptr_t)*addr < (intptr_t)*addr + NHW_APPCORE_RAM_SIZE)) {
     *addr = (intptr_t)*addr - NHW_APPCORE_RAM_ADDR + NHW_appcore_RAM;
     return true;
  }
#else
  (void) addr;
  bs_trace_warning_time_line("%s not supported yet in this device\n", __func__);
#endif
  return false;
}


#define DEF_ENTRY(peri, suf) \
		{(void *)NRF_##peri##suf##_BASE,  NRF_##peri##suf##_BASE_REAL_HW, sizeof(NRF_##peri##_Type)}

struct simu_real_conv_table_t {
  void* simu_addr;
  uint32_t real_add;
  uint32_t size;
};
static struct simu_real_conv_table_t *simu_real_conv_table;
static int simu_real_conv_table_size;

static void init_simu_real_conv_table(void) {
  struct simu_real_conv_table_t content[] = {
#if defined(NRF52833)
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
    //DEF_ENTRY(PWM, 0),
    //DEF_ENTRY(PDM,),
    //DEF_ENTRY(ACL,),
    DEF_ENTRY(NVMC,),
    DEF_ENTRY(PPI,),
    DEF_ENTRY(MWU,),
    //DEF_ENTRY(PWM, 1),
    //DEF_ENTRY(PWM, 2),
    //SPI2
    DEF_ENTRY(RTC, 2),
    //DEF_ENTRY(I2S,),
    //DEF_ENTRY(FPU,),
    //DEF_ENTRY(USBD,),
    DEF_ENTRY(UARTE, 1),
    //DEF_ENTRY(PWM, 3),
    //SPIM3
#elif defined(NRF5340)
    /* Note for the 5340 we cannot use the HAL macros, as these
     * macros exist only for either the app or net core */

    /***** Application core *****/
    //{(void *)NRF_CACHEDATA_S_BASE    ,NRF_CACHEDATA_S_APP_BASE_REAL_HW, sizeof(NRF_CACHEDATA_Type) },
    //{(void *)NRF_CACHEINFO_S_BASE    ,NRF_CACHEINFO_S_APP_BASE_REAL_HW, sizeof(NRF_CACHEINFO_Type) },
    {(void *)(NRF_FICR_regs_p[NHW_FICR_APP])  ,0x00FF0000UL, sizeof(NRF_FICR_APP_Type)},
    {(void *)(NRF_UICR_regs_p[NHW_UICR_APP0]) ,0x00FF8000UL, sizeof(NRF_UICR_Type)},
    //{(void *)NRF_CTI_S_BASE          ,0xE0042000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_TAD_S_BASE          ,0xE0080000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_DCNF_NS_BASE        ,0x40000000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_FPU_NS_BASE         ,0x40000000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_DCNF_S_BASE         ,0x50000000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_FPU_S_BASE          ,0x50000000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_CACHE_S_BASE        ,0x50001000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_SPU_S_BASE          ,0x50003000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_OSCILLATORS_NS_BASE ,0x40004000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_REGULATORS_NS_BASE  ,0x40004000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_OSCILLATORS_S_BASE  ,0x50004000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_REGULATORS_S_BASE   ,0x50004000UL, sizeof(NRF_*_Type)},
    {(void *)(NRF_CLOCK_regs[NHW_CLKPWR_APP0]) ,0x40005000UL, sizeof(NRF_CLOCK_Type)},
    {(void *)(NRF_POWER_regs[NHW_CLKPWR_APP0]) ,0x40005000UL, sizeof(NRF_POWER_Type)},
    {(void *)(NRF_RESET_regs[NHW_CLKPWR_APP0]) ,0x40005000UL, sizeof(NRF_RESET_Type)},
    {(void *)(NRF_CLOCK_regs[NHW_CLKPWR_APP0]) ,0x50005000UL, sizeof(NRF_CLOCK_Type)},
    {(void *)(NRF_POWER_regs[NHW_CLKPWR_APP0]) ,0x50005000UL, sizeof(NRF_POWER_Type)},
    {(void *)(NRF_RESET_regs[NHW_CLKPWR_APP0]) ,0x50005000UL, sizeof(NRF_RESET_Type)},
    //{(void *)NRF_CTRLAP_NS_BASE      ,0x40006000U, sizeof(NRF_*_Type)},
    //{(void *)NRF_CTRLAP_S_BASE       ,0x50006000U, sizeof(NRF_*_Type)},
    //{(void *)NRF_SPIM0_NS_BASE       ,0x40008000U, sizeof(NRF_*_Type)},
    //{(void *)NRF_SPIS0_NS_BASE       ,0x40008000U, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIM0_NS_BASE       ,0x40008000U, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIS0_NS_BASE       ,0x40008000U, sizeof(NRF_*_Type)},
    {(void *)(&NRF_UARTE_regs[NHW_UARTE_APP0]) ,0x40008000UL, sizeof(NRF_UARTE_Type)},
    //{(void *)NRF_SPIM0_S_BASE        ,0x50008000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_SPIS0_S_BASE        ,0x50008000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIM0_S_BASE        ,0x50008000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIS0_S_BASE        ,0x50008000UL, sizeof(NRF_*_Type)},
    {(void *)(&NRF_UARTE_regs[NHW_UARTE_APP0]) ,0x50008000UL, sizeof(NRF_UARTE_Type)},
    //{(void *)NRF_SPIM1_NS_BASE       ,0x40009000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_SPIS1_NS_BASE       ,0x40009000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIM1_NS_BASE       ,0x40009000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIS1_NS_BASE       ,0x40009000UL, sizeof(NRF_*_Type)},
    {(void *)(&NRF_UARTE_regs[NHW_UARTE_APP1]) ,0x40009000UL, sizeof(NRF_UARTE_Type)},
    //{(void *)NRF_SPIM1_S_BASE        ,0x50009000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_SPIS1_S_BASE        ,0x50009000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIM1_S_BASE        ,0x50009000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIS1_S_BASE        ,0x50009000UL, sizeof(NRF_*_Type)},
    {(void *)(&NRF_UARTE_regs[NHW_UARTE_APP1]) ,0x50009000UL, sizeof(NRF_UARTE_Type)},
    //{(void *)NRF_SPIM4_NS_BASE       ,0x4000A000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_SPIM4_S_BASE        ,0x5000A000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_SPIM2_NS_BASE       ,0x4000B000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_SPIS2_NS_BASE       ,0x4000B000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIM2_NS_BASE       ,0x4000B000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIS2_NS_BASE       ,0x4000B000UL, sizeof(NRF_*_Type)},
    {(void *)(&NRF_UARTE_regs[NHW_UARTE_APP2]) ,0x4000B000UL, sizeof(NRF_UARTE_Type)},
    //{(void *)NRF_SPIM2_S_BASE        ,0x5000B000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_SPIS2_S_BASE        ,0x5000B000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIM2_S_BASE        ,0x5000B000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIS2_S_BASE        ,0x5000B000UL, sizeof(NRF_*_Type)},
    {(void *)(&NRF_UARTE_regs[NHW_UARTE_APP2]) ,0x5000B000UL, sizeof(NRF_UARTE_Type)},
    //{(void *)NRF_SPIM3_NS_BASE       ,0x4000C000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_SPIS3_NS_BASE       ,0x4000C000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIM3_NS_BASE       ,0x4000C000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIS3_NS_BASE       ,0x4000C000UL, sizeof(NRF_*_Type)},
    {(void *)(&NRF_UARTE_regs[NHW_UARTE_APP3]) ,0x4000C000UL, sizeof(NRF_UARTE_Type)},
    //{(void *)NRF_SPIM3_S_BASE        ,0x5000C000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_SPIS3_S_BASE        ,0x5000C000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIM3_S_BASE        ,0x5000C000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIS3_S_BASE        ,0x5000C000UL, sizeof(NRF_*_Type)},
    {(void *)(&NRF_UARTE_regs[NHW_UARTE_APP3])   ,0x5000C000UL, sizeof(NRF_UARTE_Type)},
    {(void *)(&NRF_GPIOTE_regs[NHW_GPIOTE_APP0]) ,0x5000D000UL, sizeof(NRF_GPIOTE_Type)},
    //{(void *)NRF_SAADC_NS_BASE       ,0x4000E000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_SAADC_S_BASE        ,0x5000E000UL, sizeof(NRF_*_Type)},
    {(void *)(&NRF_TIMER_regs[NHW_TIMER_APP0]) ,0x4000F000UL, sizeof(NRF_TIMER_Type)},
    {(void *)(&NRF_TIMER_regs[NHW_TIMER_APP0]) ,0x5000F000UL, sizeof(NRF_TIMER_Type)},
    {(void *)(&NRF_TIMER_regs[NHW_TIMER_APP1]) ,0x40010000UL, sizeof(NRF_TIMER_Type)},
    {(void *)(&NRF_TIMER_regs[NHW_TIMER_APP1]) ,0x50010000UL, sizeof(NRF_TIMER_Type)},
    {(void *)(&NRF_TIMER_regs[NHW_TIMER_APP2]) ,0x40011000UL, sizeof(NRF_TIMER_Type)},
    {(void *)(&NRF_TIMER_regs[NHW_TIMER_APP2]) ,0x50011000UL, sizeof(NRF_TIMER_Type)},
    {(void *)(&NRF_RTC_regs[NHW_RTC_APP0])     ,0x40014000UL, sizeof(NRF_RTC_Type)},
    {(void *)(&NRF_RTC_regs[NHW_RTC_APP0])     ,0x50014000UL, sizeof(NRF_RTC_Type)},
    {(void *)(&NRF_RTC_regs[NHW_RTC_APP1])     ,0x40015000UL, sizeof(NRF_RTC_Type)},
    {(void *)(&NRF_RTC_regs[NHW_RTC_APP1])     ,0x50015000UL, sizeof(NRF_RTC_Type)},
    {(void *)(&NRF_DPPIC_regs[NHW_DPPI_APP_0]) ,0x40017000UL, sizeof(NRF_DPPIC_Type)},
    {(void *)(&NRF_DPPIC_regs[NHW_DPPI_APP_0]) ,0x50017000UL, sizeof(NRF_DPPIC_Type)},
    //{(void *)NRF_WDT0_NS_BASE        ,0x40018000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_WDT0_S_BASE         ,0x50018000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_WDT1_NS_BASE        ,0x40019000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_WDT1_S_BASE         ,0x50019000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_COMP_NS_BASE        ,0x4001A000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_LPCOMP_NS_BASE      ,0x4001A000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_COMP_S_BASE         ,0x5001A000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_LPCOMP_S_BASE       ,0x5001A000UL, sizeof(NRF_*_Type)},
    {(void *)(&NRF_EGU_regs[NHW_EGU_APP0]) ,0x4001B000UL, sizeof(NRF_EGU_Type)},
    {(void *)(&NRF_EGU_regs[NHW_EGU_APP0]) ,0x5001B000UL, sizeof(NRF_EGU_Type)},
    {(void *)(&NRF_EGU_regs[NHW_EGU_APP1]) ,0x4001C000UL, sizeof(NRF_EGU_Type)},
    {(void *)(&NRF_EGU_regs[NHW_EGU_APP1]) ,0x5001C000UL, sizeof(NRF_EGU_Type)},
    {(void *)(&NRF_EGU_regs[NHW_EGU_APP2]) ,0x4001D000UL, sizeof(NRF_EGU_Type)},
    {(void *)(&NRF_EGU_regs[NHW_EGU_APP2]) ,0x5001D000UL, sizeof(NRF_EGU_Type)},
    {(void *)(&NRF_EGU_regs[NHW_EGU_APP3]) ,0x4001E000UL, sizeof(NRF_EGU_Type)},
    {(void *)(&NRF_EGU_regs[NHW_EGU_APP3]) ,0x5001E000UL, sizeof(NRF_EGU_Type)},
    {(void *)(&NRF_EGU_regs[NHW_EGU_APP4]) ,0x4001F000UL, sizeof(NRF_EGU_Type)},
    {(void *)(&NRF_EGU_regs[NHW_EGU_APP4]) ,0x5001F000UL, sizeof(NRF_EGU_Type)},
    {(void *)(&NRF_EGU_regs[NHW_EGU_APP5]) ,0x40020000UL, sizeof(NRF_EGU_Type)},
    {(void *)(&NRF_EGU_regs[NHW_EGU_APP5]) ,0x50020000UL, sizeof(NRF_EGU_Type)},
    //{(void *)NRF_PWM0_NS_BASE        ,0x40021000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_PWM0_S_BASE         ,0x50021000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_PWM1_NS_BASE        ,0x40022000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_PWM1_S_BASE         ,0x50022000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_PWM2_NS_BASE        ,0x40023000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_PWM2_S_BASE         ,0x50023000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_PWM3_NS_BASE        ,0x40024000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_PWM3_S_BASE         ,0x50024000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_PDM0_NS_BASE        ,0x40026000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_PDM0_S_BASE         ,0x50026000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_I2S0_NS_BASE        ,0x40028000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_I2S0_S_BASE         ,0x50028000UL, sizeof(NRF_*_Type)},
    {(void *)(&NRF_IPC_regs[NHW_IPC_APP0]) ,0x4002A000UL, sizeof(NRF_IPC_Type)},
    {(void *)(&NRF_IPC_regs[NHW_IPC_APP0]) ,0x5002A000UL, sizeof(NRF_IPC_Type)},
    //{(void *)NRF_QSPI_NS_BASE        ,0x4002B000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_QSPI_S_BASE         ,0x5002B000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_NFCT_NS_BASE        ,0x4002D000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_NFCT_S_BASE         ,0x5002D000UL, sizeof(NRF_*_Type)},
    {(void *)(&NRF_GPIOTE_regs[NHW_GPIOTE_APP1]) ,0x4002F000UL, sizeof(NRF_GPIOTE_Type)},
    {(void *)(&NRF_MUTEX_regs)                   ,0x40030000UL, sizeof(NRF_MUTEX_Type)},
    {(void *)(&NRF_MUTEX_regs)                   ,0x50030000UL, sizeof(NRF_MUTEX_Type)},
    //{(void *)NRF_QDEC0_NS_BASE       ,0x40033000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_QDEC0_S_BASE        ,0x50033000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_QDEC1_NS_BASE       ,0x40034000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_QDEC1_S_BASE        ,0x50034000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_USBD_NS_BASE        ,0x40036000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_USBD_S_BASE         ,0x50036000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_USBREGULATOR_NS_BASE,0x40037000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_USBREGULATOR_S_BASE ,0x50037000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_KMU_NS_BASE         ,0x40039000UL, sizeof(NRF_*_Type)},
    {(void *)(NRF_NVMC_regs_p[NHW_NVMC_APP0]) ,0x40039000UL, sizeof(NRF_NVMC_Type)},
    //{(void *)NRF_KMU_S_BASE          ,0x50039000UL, sizeof(NRF_*_Type)},
    {(void *)(NRF_NVMC_regs_p[NHW_NVMC_APP0])  ,0x50039000UL, sizeof(NRF_NVMC_Type)},
    {(void *)(&NRF_GPIO_regs[NHW_GPIO_APP_P0]) ,0x40842500UL, sizeof(NRF_GPIO_Type)},
    {(void *)(&NRF_GPIO_regs[NHW_GPIO_APP_P1]) ,0x40842800UL, sizeof(NRF_GPIO_Type)},
    {(void *)(&NRF_GPIO_regs[NHW_GPIO_APP_P0]) ,0x50842500UL, sizeof(NRF_GPIO_Type)},
    {(void *)(&NRF_GPIO_regs[NHW_GPIO_APP_P1]) ,0x50842800UL, sizeof(NRF_GPIO_Type)},
    //{(void *)NRF_CRYPTOCELL_S_BASE   ,0x50844000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_VMC_NS_BASE         ,0x40081000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_VMC_S_BASE          ,0x50081000UL, sizeof(NRF_*_Type)},

    /***** Network core: *****/
    {(void *)(NRF_FICR_regs_p[NHW_FICR_NET])  ,0x01FF0000UL, sizeof(NRF_FICR_NET_Type)},
    {(void *)(NRF_UICR_regs_p[NHW_UICR_NET0]) ,0x01FF8000UL, sizeof(NRF_UICR_Type)},
    //{(void *)NRF_CTI_NS_BASE         ,0xE0042000UL, sizeof(NRF_*_Type)},
    //{(void *)NRF_DCNF_NS_BASE        ,0x41000000UL, sizeof(NRF_*_Type)},
    {(void *)(&NRF_VREQCTRL_regs)              ,0x41004000UL, sizeof(NRF_VREQCTRL_Type)},
    {(void *)(NRF_CLOCK_regs[NHW_CLKPWR_NET0]) ,0x41005000UL, sizeof(NRF_CLOCK_Type)},
    {(void *)(NRF_POWER_regs[NHW_CLKPWR_NET0]) ,0x41005000UL, sizeof(NRF_POWER_Type)},
    {(void *)(NRF_RESET_regs[NHW_CLKPWR_NET0]) ,0x41005000UL, sizeof(NRF_RESET_Type)},
    //{(void *)NRF_CTRLAP_NS_BASE      ,0x41006000UL, sizeof(NRF_*_Type)},
    {(void *)(&NRF_RADIO_regs)                  ,0x41008000UL, sizeof(NRF_RADIO_Type)},
    {(void *)(&NRF_RNG_regs)                    ,0x41009000UL, sizeof(NRF_RNG_Type)},
    {(void *)(&NRF_GPIOTE_regs[NHW_GPIOTE_NET]) ,0x4100A000UL, sizeof(NRF_GPIOTE_Type)},
    //{(void *)NRF_WDT_NS_BASE         ,0x4100B000UL, sizeof(NRF_*_Type)},
    {(void *)(&NRF_TIMER_regs[NHW_TIMER_NET0]) ,0x4100C000UL, sizeof(NRF_TIMER_Type)},
    {(void *)(&NRF_ECB_regs)                   ,0x4100D000UL, sizeof(NRF_ECB_Type)},
    {(void *)(&NRF_AAR_regs)                   ,0x4100E000UL, sizeof(NRF_AAR_Type)},
    {(void *)(&NRF_CCM_regs)                   ,0x4100E000UL, sizeof(NRF_CCM_Type)},
    {(void *)(&NRF_DPPIC_regs[NHW_DPPI_NET_0]) ,0x4100F000UL, sizeof(NRF_DPPIC_Type)},
    {(void *)(&NRF_TEMP_regs)                  ,0x41010000UL, sizeof(NRF_TEMP_Type)},
    {(void *)(&NRF_RTC_regs[NHW_RTC_NET0])     ,0x41011000UL, sizeof(NRF_RTC_Type)},
    {(void *)(&NRF_IPC_regs[NHW_IPC_NET0])     ,0x41012000UL, sizeof(NRF_IPC_Type)},
    //{(void *)NRF_SPIM0_NS_BASE       ,0x41013000UL, sizeof(NRF_*_Type), sizeof(NRF_*_Type)},
    //{(void *)NRF_SPIS0_NS_BASE       ,0x41013000UL, sizeof(NRF_*_Type), sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIM0_NS_BASE       ,0x41013000UL, sizeof(NRF_*_Type), sizeof(NRF_*_Type)},
    //{(void *)NRF_TWIS0_NS_BASE       ,0x41013000UL, sizeof(NRF_*_Type), sizeof(NRF_*_Type)},
    {(void *)(&NRF_UARTE_regs[NHW_UARTE_NET0]) ,0x41013000UL, sizeof(NRF_UARTE_Type)},
    {(void *)(&NRF_EGU_regs[NHW_EGU_NET0])     ,0x41014000UL, sizeof(NRF_EGU_Type)},
    {(void *)(&NRF_RTC_regs[NHW_RTC_NET1])     ,0x41016000UL, sizeof(NRF_RTC_Type)},
    {(void *)(&NRF_TIMER_regs[NHW_TIMER_NET1]) ,0x41018000UL, sizeof(NRF_TIMER_Type)},
    {(void *)(&NRF_TIMER_regs[NHW_TIMER_NET2]) ,0x41019000UL, sizeof(NRF_TIMER_Type)},
    {(void *)(&NRF_SWI_regs[NHW_SWI_NET0])     ,0x4101A000UL, sizeof(int)},
    {(void *)(&NRF_SWI_regs[NHW_SWI_NET1])     ,0x4101B000UL, sizeof(int)},
    {(void *)(&NRF_SWI_regs[NHW_SWI_NET2])     ,0x4101C000UL, sizeof(int)},
    {(void *)(&NRF_SWI_regs[NHW_SWI_NET3])     ,0x4101D000UL, sizeof(int)},
    {(void *)(&NRF_MUTEX_regs)                 ,0x40030000UL, sizeof(NRF_MUTEX_Type)},
    {(void *)(&NRF_MUTEX_regs)                 ,0x50030000UL, sizeof(NRF_MUTEX_Type)},
    //{(void *)NRF_ACL_NS_BASE         ,0x41080000UL, sizeof(NRF_*_Type)},
    {(void *)(NRF_NVMC_regs_p[NHW_NVMC_NET0])  ,0x41080000UL, sizeof(NRF_NVMC_Type)},
    //{(void *)NRF_VMC_NS_BASE         ,0x41081000UL, sizeof(NRF_*_Type)},
    {(void *)(&NRF_GPIO_regs[NHW_GPIO_NET_P0]) ,0x418C0500UL, sizeof(NRF_GPIO_Type)},
    {(void *)(&NRF_GPIO_regs[NHW_GPIO_NET_P1]) ,0x418C0800UL, sizeof(NRF_GPIO_Type)},
#elif defined(NRF54L15)
    DEF_ENTRY(FICR, _NS),
    DEF_ENTRY(UICR, _S),
    //DEF_ENTRY(SICR, _S),
    DEF_ENTRY(CRACENCORE, _S),
    DEF_ENTRY(SPU, 00_S),
    //DEF_ENTRY(MPC, 00_S),
    DEF_ENTRY(DPPIC, 00_S),
    DEF_ENTRY(DPPIC, 00_NS),
    DEF_ENTRY(PPIB, 00_S),
    DEF_ENTRY(PPIB, 00_NS),
    DEF_ENTRY(PPIB, 01_S),
    DEF_ENTRY(PPIB, 01_NS),
    //DEF_ENTRY(KMU, _S),
    DEF_ENTRY(AAR, 00_NS),
    DEF_ENTRY(AAR, 00_S),
    DEF_ENTRY(CCM, 00_NS),
    DEF_ENTRY(CCM, 00_S),
    DEF_ENTRY(ECB, 00_NS),
    DEF_ENTRY(ECB, 00_S),
    DEF_ENTRY(CRACEN, _S),
    //DEF_ENTRY(SPIM, 00_NS),
    //DEF_ENTRY(SPIM, 00_S),
    //DEF_ENTRY(SPIS, 00_NS),
    //DEF_ENTRY(SPIS, 00_S),
    DEF_ENTRY(UARTE, 00_NS),
    DEF_ENTRY(UARTE, 00_S),
    //DEF_ENTRY(GLITCHDET, _S),
    DEF_ENTRY(RRAMC, _S),
    //DEF_ENTRY(VPR, 00_NS),
    //DEF_ENTRY(VPR, 00_S),
    {(void *)NRF_P2_NS_BASE, NRF_P2_NS_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    {(void *)NRF_P2_S_BASE, NRF_P2_S_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    //DEF_ENTRY(CTRLA, _NS),
    //DEF_ENTRY(CTRLA, _S),
    //DEF_ENTRY(TAD, _NS),
    //DEF_ENTRY(TAD, _S),
    DEF_ENTRY(TIMER, 00_NS),
    DEF_ENTRY(TIMER, 00_S),
    DEF_ENTRY(SPU, 10_S),
    DEF_ENTRY(DPPIC, 10_S),
    DEF_ENTRY(DPPIC, 10_NS),
    DEF_ENTRY(PPIB, 10_S),
    DEF_ENTRY(PPIB, 10_NS),
    DEF_ENTRY(PPIB, 11_S),
    DEF_ENTRY(PPIB, 11_NS),
    DEF_ENTRY(TIMER, 10_NS),
    DEF_ENTRY(TIMER, 10_S),
    DEF_ENTRY(EGU, 10_NS),
    DEF_ENTRY(EGU, 10_S),
    DEF_ENTRY(RADIO, _NS),
    DEF_ENTRY(RADIO, _S),
    DEF_ENTRY(SPU, 20_S),
    DEF_ENTRY(DPPIC, 20_S),
    DEF_ENTRY(DPPIC, 20_NS),
    DEF_ENTRY(PPIB, 20_S),
    DEF_ENTRY(PPIB, 20_NS),
    DEF_ENTRY(PPIB, 21_S),
    DEF_ENTRY(PPIB, 21_NS),
    DEF_ENTRY(PPIB, 22_S),
    DEF_ENTRY(PPIB, 22_NS),
    //DEF_ENTRY(SPIM, 20_NS),
    //DEF_ENTRY(SPIM, 20_S),
    //DEF_ENTRY(SPIS, 20_NS),
    //DEF_ENTRY(SPIS, 20_S),
    //DEF_ENTRY(TWIM, 20_NS),
    //DEF_ENTRY(TWIM, 20_S),
    //DEF_ENTRY(TWIS, 20_NS),
    //DEF_ENTRY(TWIS, 20_S),
    DEF_ENTRY(UARTE, 20_NS),
    DEF_ENTRY(UARTE, 20_S),
    //DEF_ENTRY(SPIM, 21_NS),
    //DEF_ENTRY(SPIM, 21_S),
    //DEF_ENTRY(SPIS, 21_NS),
    //DEF_ENTRY(SPIS, 21_S),
    //DEF_ENTRY(TWIM, 21_NS),
    //DEF_ENTRY(TWIM, 21_S),
    //DEF_ENTRY(TWIS, 21_NS),
    //DEF_ENTRY(TWIS, 21_S),
    DEF_ENTRY(UARTE, 21_NS),
    DEF_ENTRY(UARTE, 21_S),
    //DEF_ENTRY(SPIM, 22_NS),
    //DEF_ENTRY(SPIM, 22_S),
    //DEF_ENTRY(SPIS, 22_NS),
    //DEF_ENTRY(SPIS, 22_S),
    //DEF_ENTRY(TWIM, 22_NS),
    //DEF_ENTRY(TWIM, 22_S),
    //DEF_ENTRY(TWIS, 22_NS),
    //DEF_ENTRY(TWIS, 22_S),
    DEF_ENTRY(UARTE, 22_NS),
    DEF_ENTRY(UARTE, 22_S),
    DEF_ENTRY(EGU, 20_NS),
    DEF_ENTRY(EGU, 20_S),
    DEF_ENTRY(TIMER, 20_NS),
    DEF_ENTRY(TIMER, 20_S),
    DEF_ENTRY(TIMER, 21_NS),
    DEF_ENTRY(TIMER, 21_S),
    DEF_ENTRY(TIMER, 22_NS),
    DEF_ENTRY(TIMER, 22_S),
    DEF_ENTRY(TIMER, 23_NS),
    DEF_ENTRY(TIMER, 23_S),
    DEF_ENTRY(TIMER, 24_NS),
    DEF_ENTRY(TIMER, 24_S),
    //DEF_ENTRY(MEMCONF, _NS),
    //DEF_ENTRY(MEMCONF, _S),
    //DEF_ENTRY(PDM, 20_NS),
    //DEF_ENTRY(PDM, 20_S),
    //DEF_ENTRY(PDM, 21_NS),
    //DEF_ENTRY(PDM, 21_S),
    //DEF_ENTRY(PWM, 20_NS),
    //DEF_ENTRY(PWM, 20_S),
    //DEF_ENTRY(PWM, 21_NS),
    //DEF_ENTRY(PWM, 21_S),
    //DEF_ENTRY(PWM, 22_NS),
    //DEF_ENTRY(PWM, 22_S),
    //DEF_ENTRY(SAADC, _NS),
    //DEF_ENTRY(SAADC, _S),
    DEF_ENTRY(NFCT, _NS),
    DEF_ENTRY(NFCT, _S),
    DEF_ENTRY(TEMP, _NS),
    DEF_ENTRY(TEMP, _S),
    {(void *)NRF_P1_NS_BASE, NRF_P1_NS_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    {(void *)NRF_P1_S_BASE, NRF_P1_S_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    DEF_ENTRY(GPIOTE, 20_NS),
    DEF_ENTRY(GPIOTE, 20_S),
    //DEF_ENTRY(TAMPC, _S),
    //DEF_ENTRY(I2S, 20_NS),
    //DEF_ENTRY(I2S, 20_S),
    //DEF_ENTRY(QDEC, 20_NS),
    //DEF_ENTRY(QDEC, 20_S),
    //DEF_ENTRY(QDEC, 21_NS),
    //DEF_ENTRY(QDEC, 21_S),
    DEF_ENTRY(GRTC, _NS),
    DEF_ENTRY(GRTC, _S),
    DEF_ENTRY(SPU, 30_S),
    DEF_ENTRY(DPPIC, 30_S),
    DEF_ENTRY(DPPIC, 30_NS),
    DEF_ENTRY(PPIB, 30_S),
    DEF_ENTRY(PPIB, 30_NS),
    //DEF_ENTRY(SPIM, 30_NS),
    //DEF_ENTRY(SPIM, 30_S),
    //DEF_ENTRY(SPIS, 30_NS),
    //DEF_ENTRY(SPIS, 30_S),
    //DEF_ENTRY(TWIM, 30_NS),
    //DEF_ENTRY(TWIM, 30_S),
    //DEF_ENTRY(TWIS, 30_NS),
    //DEF_ENTRY(TWIS, 30_S),
    DEF_ENTRY(UARTE, 30_NS),
    DEF_ENTRY(UARTE, 30_S),
    //DEF_ENTRY(COMP, _NS),
    //DEF_ENTRY(COMP, _S),
    //DEF_ENTRY(LPCOMP, _NS),
    //DEF_ENTRY(LPCOMP, _S),
    //DEF_ENTRY(WDT, 30_S),
    //DEF_ENTRY(WDT, 31_NS),
    //DEF_ENTRY(WDT, 31_S),
    {(void *)NRF_P0_NS_BASE, NRF_P0_NS_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    {(void *)NRF_P0_S_BASE, NRF_P0_S_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    DEF_ENTRY(GPIOTE, 30_NS),
    DEF_ENTRY(GPIOTE, 30_S),
    DEF_ENTRY(CLOCK, _NS),
    DEF_ENTRY(CLOCK, _S),
    DEF_ENTRY(POWER, _NS),
    DEF_ENTRY(POWER, _S),
    DEF_ENTRY(RESET, _NS),
    DEF_ENTRY(RESET, _S),
    //DEF_ENTRY(OSCILLATORS, _NS),
    //DEF_ENTRY(OSCILLATORS, _S),
    //DEF_ENTRY(REGULATORS, _NS),
    //DEF_ENTRY(REGULATORS, _S),

#elif defined(NRF54LM20)
    DEF_ENTRY(FICR, _NS),
    DEF_ENTRY(UICR, _S),
    //DEF_ENTRY(SICR, _S),
    DEF_ENTRY(CRACENCORE, _S),
    //DEF_ENTRY(USBHSCORE, _NS),
    //DEF_ENTRY(USBHSCORE, _S),
    DEF_ENTRY(SPU, 00_S),
    //DEF_ENTRY(MPC, 00_S),
    DEF_ENTRY(DPPIC, 00_S),
    DEF_ENTRY(DPPIC, 00_NS),
    DEF_ENTRY(PPIB, 00_S),
    DEF_ENTRY(PPIB, 00_NS),
    DEF_ENTRY(PPIB, 01_S),
    DEF_ENTRY(PPIB, 01_NS),
    //DEF_ENTRY(KMU, _S),
    DEF_ENTRY(AAR, 00_NS),
    DEF_ENTRY(AAR, 00_S),
    DEF_ENTRY(CCM, 00_NS),
    DEF_ENTRY(CCM, 00_S),
    DEF_ENTRY(ECB, 00_NS),
    DEF_ENTRY(ECB, 00_S),
    //DEF_ENTRY(VPR, 00_NS),
    //DEF_ENTRY(VPR, 00_S),
    //DEF_ENTRY(SPIM, 00_NS),
    //DEF_ENTRY(SPIM, 00_S),
    //DEF_ENTRY(SPIS, 00_NS),
    //DEF_ENTRY(SPIS, 00_S),
    DEF_ENTRY(UARTE, 00_NS),
    DEF_ENTRY(UARTE, 00_S),
    //DEF_ENTRY(GLITCHDET, _S),
    DEF_ENTRY(RRAMC, _S),
    {(void *)NRF_P2_NS_BASE, NRF_P2_NS_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    {(void *)NRF_P2_S_BASE, NRF_P2_S_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    //DEF_ENTRY(CTRLA, _NS),
    //DEF_ENTRY(CTRLA, _S),
    //DEF_ENTRY(TAD, _NS),
    //DEF_ENTRY(TAD, _S),
    DEF_ENTRY(TIMER, 00_NS),
    DEF_ENTRY(TIMER, 00_S),
    DEF_ENTRY(EGU, 00_NS),
    DEF_ENTRY(EGU, 00_S),
    DEF_ENTRY(CRACEN, _S),
    //DEF_ENTRY(USBHS, _NS),
    //DEF_ENTRY(USBHS, _S),
    DEF_ENTRY(SPU, 10_S),
    DEF_ENTRY(DPPIC, 10_S),
    DEF_ENTRY(DPPIC, 10_NS),
    DEF_ENTRY(PPIB, 10_S),
    DEF_ENTRY(PPIB, 10_NS),
    DEF_ENTRY(PPIB, 11_S),
    DEF_ENTRY(PPIB, 11_NS),
    DEF_ENTRY(TIMER, 10_NS),
    DEF_ENTRY(TIMER, 10_S),
    DEF_ENTRY(EGU, 10_NS),
    DEF_ENTRY(EGU, 10_S),
    DEF_ENTRY(RADIO, _NS),
    DEF_ENTRY(RADIO, _S),
    DEF_ENTRY(SPU, 20_S),
    DEF_ENTRY(DPPIC, 20_S),
    DEF_ENTRY(DPPIC, 20_NS),
    DEF_ENTRY(PPIB, 20_S),
    DEF_ENTRY(PPIB, 20_NS),
    DEF_ENTRY(PPIB, 21_S),
    DEF_ENTRY(PPIB, 21_NS),
    DEF_ENTRY(PPIB, 22_S),
    DEF_ENTRY(PPIB, 22_NS),
    //DEF_ENTRY(SPIM, 20_NS),
    //DEF_ENTRY(SPIM, 20_S),
    //DEF_ENTRY(SPIS, 20_NS),
    //DEF_ENTRY(SPIS, 20_S),
    //DEF_ENTRY(TWIM, 20_NS),
    //DEF_ENTRY(TWIM, 20_S),
    //DEF_ENTRY(TWIS, 20_NS),
    //DEF_ENTRY(TWIS, 20_S),
    DEF_ENTRY(UARTE, 20_NS),
    DEF_ENTRY(UARTE, 20_S),
    //DEF_ENTRY(SPIM, 21_NS),
    //DEF_ENTRY(SPIM, 21_S),
    //DEF_ENTRY(SPIS, 21_NS),
    //DEF_ENTRY(SPIS, 21_S),
    //DEF_ENTRY(TWIM, 21_NS),
    //DEF_ENTRY(TWIM, 21_S),
    //DEF_ENTRY(TWIS, 21_NS),
    //DEF_ENTRY(TWIS, 21_S),
    DEF_ENTRY(UARTE, 21_NS),
    DEF_ENTRY(UARTE, 21_S),
    //DEF_ENTRY(SPIM, 22_NS),
    //DEF_ENTRY(SPIM, 22_S),
    //DEF_ENTRY(SPIS, 22_NS),
    //DEF_ENTRY(SPIS, 22_S),
    //DEF_ENTRY(TWIM, 22_NS),
    //DEF_ENTRY(TWIM, 22_S),
    //DEF_ENTRY(TWIS, 22_NS),
    //DEF_ENTRY(TWIS, 22_S),
    DEF_ENTRY(UARTE, 22_NS),
    DEF_ENTRY(UARTE, 22_S),
    DEF_ENTRY(EGU, 20_NS),
    DEF_ENTRY(EGU, 20_S),
    DEF_ENTRY(TIMER, 20_NS),
    DEF_ENTRY(TIMER, 20_S),
    DEF_ENTRY(TIMER, 21_NS),
    DEF_ENTRY(TIMER, 21_S),
    DEF_ENTRY(TIMER, 22_NS),
    DEF_ENTRY(TIMER, 22_S),
    DEF_ENTRY(TIMER, 23_NS),
    DEF_ENTRY(TIMER, 23_S),
    DEF_ENTRY(TIMER, 24_NS),
    DEF_ENTRY(TIMER, 24_S),
    //DEF_ENTRY(MEMCONF, _NS),
    //DEF_ENTRY(MEMCONF, _S),
    //DEF_ENTRY(PDM, 20_NS),
    //DEF_ENTRY(PDM, 20_S),
    //DEF_ENTRY(PDM, 21_NS),
    //DEF_ENTRY(PDM, 21_S),
    //DEF_ENTRY(PWM, 20_NS),
    //DEF_ENTRY(PWM, 20_S),
    //DEF_ENTRY(PWM, 21_NS),
    //DEF_ENTRY(PWM, 21_S),
    //DEF_ENTRY(PWM, 22_NS),
    //DEF_ENTRY(PWM, 22_S),
    //DEF_ENTRY(SAADC, _NS),
    //DEF_ENTRY(SAADC, _S),
    DEF_ENTRY(NFCT, _NS),
    DEF_ENTRY(NFCT, _S),
    DEF_ENTRY(TEMP, _NS),
    DEF_ENTRY(TEMP, _S),
    {(void *)NRF_P1_NS_BASE, NRF_P1_NS_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    {(void *)NRF_P1_S_BASE, NRF_P1_S_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    {(void *)NRF_P3_NS_BASE, NRF_P3_NS_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    {(void *)NRF_P3_S_BASE, NRF_P3_S_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    DEF_ENTRY(GPIOTE, 20_NS),
    DEF_ENTRY(GPIOTE, 20_S),
    //DEF_ENTRY(QDEC, 20_NS),
    //DEF_ENTRY(QDEC, 20_S),
    //DEF_ENTRY(QDEC, 21_NS),
    //DEF_ENTRY(QDEC, 21_S),
    DEF_ENTRY(GRTC, _NS),
    DEF_ENTRY(GRTC, _S),
    //DEF_ENTRY(TDM, _NS),
    //DEF_ENTRY(TDM, _S),
    //DEF_ENTRY(SPIM, 23_NS),
    //DEF_ENTRY(SPIM, 23_S),
    //DEF_ENTRY(SPIS, 23_NS),
    //DEF_ENTRY(SPIS, 23_S),
    //DEF_ENTRY(TWIM, 23_NS),
    //DEF_ENTRY(TWIM, 23_S),
    //DEF_ENTRY(TWIS, 23_NS),
    //DEF_ENTRY(TWIS, 23_S),
    DEF_ENTRY(UARTE, 23_NS),
    DEF_ENTRY(UARTE, 23_S),
    //DEF_ENTRY(SPIM, 24_NS),
    //DEF_ENTRY(SPIM, 24_S),
    //DEF_ENTRY(SPIS, 24_NS),
    //DEF_ENTRY(SPIS, 24_S),
    //DEF_ENTRY(TWIM, 24_NS),
    //DEF_ENTRY(TWIM, 24_S),
    //DEF_ENTRY(TWIS, 24_NS),
    //DEF_ENTRY(TWIS, 24_S),
    DEF_ENTRY(UARTE, 24_NS),
    DEF_ENTRY(UARTE, 24_S),
    //DEF_ENTRY(TAMPC, _S),
    DEF_ENTRY(SPU, 30_S),
    DEF_ENTRY(DPPIC, 30_S),
    DEF_ENTRY(DPPIC, 30_NS),
    DEF_ENTRY(PPIB, 30_S),
    DEF_ENTRY(PPIB, 30_NS),
    //DEF_ENTRY(SPIM, 30_NS),
    //DEF_ENTRY(SPIM, 30_S),
    //DEF_ENTRY(SPIS, 30_NS),
    //DEF_ENTRY(SPIS, 30_S),
    //DEF_ENTRY(TWIM, 30_NS),
    //DEF_ENTRY(TWIM, 30_S),
    //DEF_ENTRY(TWIS, 30_NS),
    //DEF_ENTRY(TWIS, 30_S),
    DEF_ENTRY(UARTE, 30_NS),
    DEF_ENTRY(UARTE, 30_S),
    //DEF_ENTRY(COMP, _NS),
    //DEF_ENTRY(COMP, _S),
    //DEF_ENTRY(LPCOMP, _NS),
    //DEF_ENTRY(LPCOMP, _S),
    //DEF_ENTRY(WDT, 30_S),
    //DEF_ENTRY(WDT, 31_NS),
    //DEF_ENTRY(WDT, 31_S),
    {(void *)NRF_P0_NS_BASE, NRF_P0_NS_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    {(void *)NRF_P0_S_BASE, NRF_P0_S_BASE_REAL_HW, sizeof(NRF_GPIO_Type)},
    DEF_ENTRY(GPIOTE, 30_NS),
    DEF_ENTRY(GPIOTE, 30_S),
    DEF_ENTRY(CLOCK, _NS),
    DEF_ENTRY(CLOCK, _S),
    DEF_ENTRY(POWER, _NS),
    DEF_ENTRY(POWER, _S),
    DEF_ENTRY(RESET, _NS),
    DEF_ENTRY(RESET, _S),
    //DEF_ENTRY(OSCILLATORS, _NS),
    //DEF_ENTRY(OSCILLATORS, _S),
    //DEF_ENTRY(REGULATORS, _NS),
    //DEF_ENTRY(REGULATORS, _S),
    //DEF_ENTRY(VREGUSB, _NS),
    //DEF_ENTRY(VREGUSB, _S),
#else
  #error "No valid platform was selected"
#endif
  };

  simu_real_conv_table = bs_malloc(sizeof(content));
  memcpy(simu_real_conv_table, content, sizeof(content));
  simu_real_conv_table_size = sizeof(content)/sizeof(content[0]);
}

NSI_TASK(init_simu_real_conv_table, HW_INIT, 999);

/**
 * Convert a real peripheral base address (like 0x4000F000UL)
 * into the corresponding simulation peripheral base address
 *
 * Note: In general it is recommended to refer to peripherals
 * using the MDK macros (like NRF_TIMER0), this function
 * exists to cover the cases in which this is not possible.
 */
void *nhw_convert_periph_base_addr(void *hw_addr) {
  for (unsigned int i = 0; i < simu_real_conv_table_size; i++) {
    if (simu_real_conv_table[i].real_add == (uint32_t)hw_addr) {
      return simu_real_conv_table[i].simu_addr;
    }
  }
  bs_trace_error_time_line("Could not find real peripheral addr %p\n", hw_addr);
  return NULL;
}

/**
 * Convert a simulated peripheral address into the real
 * HW peripheral address (like 0x4000F000UL)
 *
 * Note that if there is both secure and non secure it will just return whichever was
 * earlier in simu_real_conv_table[].
 */
void *nhw_convert_per_addr_sim_to_hw(void *sim_addr) {
  for (unsigned int i = 0; i < simu_real_conv_table_size; i++) {
    intptr_t start = (intptr_t)simu_real_conv_table[i].simu_addr;
    intptr_t end = start + (intptr_t)simu_real_conv_table[i].size ;
    if (((intptr_t)sim_addr >= start) && ((intptr_t)sim_addr < end)) {
      return (void *)((intptr_t)sim_addr - start + (intptr_t)simu_real_conv_table[i].real_add);
    }
  }
  bs_trace_error_time_line("%s could not find %p in between the simulated peripherals\n", __func__, sim_addr); \
  return 0;
}

/**
 * Convert a real peripheral address (like 0x4000F000UL)
 * into the equivalent simulated HW peripheral address
 *
 * Note that if there is both secure and non secure it will always return the secure real HW address
 */
void *nhw_convert_per_addr_hw_to_sim(void *real_addr) {
  for (unsigned int i = 0; i < simu_real_conv_table_size; i++) {
    intptr_t start = (intptr_t)simu_real_conv_table[i].real_add;
    intptr_t end = start + (intptr_t)simu_real_conv_table[i].size ;
    if (((intptr_t)real_addr >= start) && ((intptr_t)real_addr < end)) {
      return (void *)((intptr_t)real_addr - start + (intptr_t)simu_real_conv_table[i].simu_addr);
    }
  }
  bs_trace_error_time_line("%s could not find %p in between the simulated peripherals\n", __func__, real_addr); \
  return 0;
}

