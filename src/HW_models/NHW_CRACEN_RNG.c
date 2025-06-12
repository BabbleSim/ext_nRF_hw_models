/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * CRACEN (T/ND)RNG - (True/Non-Deterministic) Random Number Generator
 *
 * Notes:
 *  * There is 2 versions of the RNG HW. v1 is the one included in the 54L15/L10/L05
 *    And v2 is the one included in newer devices (like the 54LM20)
 *
 *  * Conditioning is not modeled beyond the throughput difference.
 *    The model will produce pseudorandom data of the same quality with it enabled or not.
 *    The KEY registers are ignored.
 *
 *  * The test LFSR is not modeled. Setting CONTROL.LFSREn has no effect. The RNG produces the same data either way,
 *    which will not match what the HW LFSR would produce.
 *
 *  * DisableOsc has no effect, the produced entropy is the same either way
 *
 *  * The TestData register is ignored. It is not possible to hand feed to the logic entropy values with it. TestDataBusy is never set.
 *
 *  * CONFIG.ForceRun is ignored (assumed always off)
 *  * CONFIG.FifoWriteStartUp is ignored (assumed always off)
 *
 *  * The model does not include the real AIS31 test logic, and therefore its associated registers are ignored (AIS31*)
 *    Enabling or not the AIS31Bypass has no effect and so does CONTROL.AIS31TestSel.
 *    AIS31Status is never set.
 *
 *  * The model does not include the real "health" monitoring logic. CONTROL.HealthTestSel is ignored and so are the Rep & PropThresh
 *    registers.
 *
 *  * The model includes test logic to trigger a startup, AIS31 or Health monitoring failure.
 *    See nhw_CRACEN_RNG_fake_test_error()
 *
 *  * The model does not properly support changing configuration on the fly (you should disable the peripheral first)
 *
 *  * The model timing is a bit faster than the real v1 HW (up to 25% faster depending on configuration),
 *    and when conditioning is on, it pushes to the FIFO 4 words at a time, while the real HW pushes one at a time in sets of 4.
 *
 *  //TODO comment on model vs real HW timing for v2
 *
 *  * Regarding FIFOLEVEL, writing to this register (in HW or the model) only clears the full status flag.
 *    The register content itself remains untouched. note that if the level is still full while writing to this
 *    register, the status flag and interrupt will be set back right away => Fix spec.
 *
 *  * Regarding FIFOThresh, the spec seems to be incorrect, in that the actual threshold is
 *    FIFOLEVEL < (FIFOThresh + 1)*4 (note the +1) => Fix spec
 *
 *  Notes for v2:
 *  * The blending and its selected method is not modeled beyond the throughput difference.
 *
 *  * In the model, the STATUS PROPTESTFAILPERSHARE and REPTESTFAILPERSHARE fields are never set
 *    Neither is CONDITIONINGISTOOSLOW.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_templates.h"
#include "NHW_xPPI.h"
#include "NHW_CRACEN_RNG.h"
#include "NHW_CRACEN_wrap.h"
#include "irq_ctrl.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"
#include "bs_rand_main.h"
#include "bs_tracing.h"

extern NRF_CRACEN_Type NRF_CRACEN_regs;
extern NRF_CRACENCORE_Type NRF_CRACENCORE_regs;

static NRF_CRACENCORE_RNGCONTROL_Type *RNG_regs;

bs_time_t Timer_CRACEN_NDRNG;
static uint Timer_rem_clocks;

static struct rng_status {
  enum status_t {rng_reset = 0, rng_startup, rng_idle_ron /*reused as fifo_full for v2*/, rng_idle_roff, rng_filling, rng_error} status;
  bool enabled;
  uint fifo_size; //Size of FIFO in 32bit words
  uint fifo_rptr; //Offset of the first available word to read
  uint fifo_wptr; //Offset of the first available empty word
  uint fifo_level; //Number of words uses in the fifo
  uint32_t fifo[1 << NHW_CRACEN_RNG_G_log2fifodepth];
  uint queued_words; //Number of 32bit words ready to go into FIFO

  bool error_ais31_noise;
  bool error_ais31_prenoise;
  bool error_prop_test;
  bool error_rep_test;
  bool error_startup_test;

  bool pend_error_ais31_noise;
  bool pend_error_ais31_prenoise;
  bool pend_error_prop_test;
  bool pend_error_rep_test;
  bool pend_error_startup_test;
} rng_st;

static void generate_more_data(void);
static void startup(void);
static inline void update_state(int state);
static void check_interrupts(void);
static void check_errors(void);

static void nhw_CRACEN_soft_reset(void) {
  rng_st.queued_words = 0;
  rng_st.fifo_level = 0;
  RNG_regs->FIFOLEVEL = 0;
  rng_st.fifo_wptr = 0;
  rng_st.fifo_rptr = 0;

  rng_st.status = rng_reset;
  update_state(CRACENCORE_RNGCONTROL_STATUS_STATE_RESET);

  rng_st.error_ais31_noise = false;
  rng_st.error_ais31_prenoise = false;
  rng_st.error_prop_test = false;
  rng_st.error_rep_test = false;
  rng_st.error_startup_test = false;
  rng_st.pend_error_ais31_noise = false;
  rng_st.pend_error_ais31_prenoise = false;
  rng_st.pend_error_prop_test = false;
  rng_st.pend_error_rep_test = false;
  rng_st.pend_error_startup_test = false;

  check_interrupts();
}

void nhw_CRACEN_RNG_init(void) {
  RNG_regs = (NRF_CRACENCORE_RNGCONTROL_Type *)&NRF_CRACENCORE_regs.RNGCONTROL;

  RNG_regs->CONTROL = CRACENCORE_RNGCONTROL_CONTROL_ResetValue;
  RNG_regs->FIFOTHRESHOLD = CRACENCORE_RNGCONTROL_FIFOTHRESHOLD_ResetValue;
  RNG_regs->FIFODEPTH = CRACENCORE_RNGCONTROL_FIFODEPTH_ResetValue;
  RNG_regs->REPEATTHRESHOLD = CRACENCORE_RNGCONTROL_REPEATTHRESHOLD_ResetValue;

#if NHW_CRACEN_RNG_V < 2
  RNG_regs->PROPTHRESHOLD = CRACENCORE_RNGCONTROL_PROPTHRESHOLD_ResetValue;
  RNG_regs->INITWAITVAL = CRACENCORE_RNGCONTROL_INITWAITVAL_ResetValue;
  RNG_regs->SWOFFTMRVAL = CRACENCORE_RNGCONTROL_SWOFFTMRVAL_ResetValue;
  RNG_regs->AIS31CONF0 = CRACENCORE_RNGCONTROL_AIS31CONF0_ResetValue;
  RNG_regs->AIS31CONF1 = CRACENCORE_RNGCONTROL_AIS31CONF1_ResetValue;
  RNG_regs->AIS31CONF2 = CRACENCORE_RNGCONTROL_AIS31CONF2_ResetValue;
#else
  RNG_regs->PROPTESTCUTOFF = CRACENCORE_RNGCONTROL_PROPTESTCUTOFF_ResetValue;
  RNG_regs->WARMUPPERIOD = CRACENCORE_RNGCONTROL_WARMUPPERIOD_ResetValue;
  RNG_regs->SAMPLINGPERIOD = CRACENCORE_RNGCONTROL_SAMPLINGPERIOD_ResetValue;
  RNG_regs->AUTOCORRTESTCUTOFF0 = CRACENCORE_RNGCONTROL_AUTOCORRTESTCUTOFF0_ResetValue;
  RNG_regs->AUTOCORRTESTCUTOFF1 = CRACENCORE_RNGCONTROL_AUTOCORRTESTCUTOFF1_ResetValue;
  RNG_regs->CORRTESTCUTOFF0 = CRACENCORE_RNGCONTROL_CORRTESTCUTOFF0_ResetValue;
  RNG_regs->CORRTESTCUTOFF1 = CRACENCORE_RNGCONTROL_CORRTESTCUTOFF1_ResetValue;
  RNG_regs->HWVERSION = CRACENCORE_RNGCONTROL_HWVERSION_ResetValue;
#endif
  RNG_regs->HWCONFIG = CRACENCORE_RNGCONTROL_HWCONFIG_ResetValue;

  rng_st.fifo_size = 1 << NHW_CRACEN_RNG_G_log2fifodepth;

  Timer_CRACEN_NDRNG = TIME_NEVER;

  nhw_CRACEN_soft_reset();
}

static void check_interrupts(void) {
  bool new_int_line = false;

#if NHW_CRACEN_RNG_V < 2
uint32_t status_int_mask =   CRACENCORE_RNGCONTROL_STATUS_REPFAIL_Msk
                           | CRACENCORE_RNGCONTROL_STATUS_PROPFAIL_Msk
                           | CRACENCORE_RNGCONTROL_STATUS_FULLINT_Msk
                           | CRACENCORE_RNGCONTROL_STATUS_PREINT_Msk
                           | CRACENCORE_RNGCONTROL_STATUS_ALMINT_Msk;
#else
uint32_t status_int_mask =   CRACENCORE_RNGCONTROL_STATUS_REPFAIL_Msk
                           | CRACENCORE_RNGCONTROL_STATUS_PROPFAIL_Msk
                           | CRACENCORE_RNGCONTROL_STATUS_ANYHEALTHTESTFAIL_Msk
                           | CRACENCORE_RNGCONTROL_STATUS_FULLINT_Msk;
#endif

  RNG_regs->STATUS &= ~status_int_mask;

#define CHECK_SET_FAILINT(statusm, intm) \
  RNG_regs->STATUS |= CRACENCORE_RNGCONTROL_STATUS_##statusm##_Msk; \
  if (RNG_regs->CONTROL & CRACENCORE_RNGCONTROL_CONTROL_INTEN##intm##_Msk) { \
    new_int_line = true; \
  }

  /*
   * Note: v1 of the IP has interrupt causes for FULLINT, REPFAIL, PROPFAIL, PREINT, and ALMINT
   * Note: v2 of the IP has interrupt causes for FULLINT and ANYHEALTHTESTFAIL and a bit different STATUS register layout
   */

  if (rng_st.error_startup_test) { //Does not have an int line, only status bit
    RNG_regs->STATUS |= CRACENCORE_RNGCONTROL_STATUS_STARTUPFAIL_Msk;
  }
  if (rng_st.fifo_level == rng_st.fifo_size) {
    CHECK_SET_FAILINT(FULLINT, FULL)
  }
#if NHW_CRACEN_RNG_V < 2
  if (rng_st.error_rep_test) {
    CHECK_SET_FAILINT(REPFAIL, REP)
  }
  if (rng_st.error_prop_test) {
    CHECK_SET_FAILINT(PROPFAIL, PROP)
  }
  if (rng_st.error_ais31_prenoise) {
    CHECK_SET_FAILINT(PREINT, PRE)
  }
  if (rng_st.error_ais31_noise) {
    CHECK_SET_FAILINT(ALMINT, ALM)
  }
#else //v2
  if (rng_st.error_rep_test) {
    RNG_regs->STATUS |= CRACENCORE_RNGCONTROL_STATUS_REPFAIL_Msk;
  }
  if (rng_st.error_prop_test) {
    RNG_regs->STATUS |= CRACENCORE_RNGCONTROL_STATUS_PROPFAIL_Msk;
  }
  if (rng_st.error_rep_test || rng_st.error_prop_test || rng_st.error_ais31_prenoise || rng_st.error_ais31_noise)  {
    CHECK_SET_FAILINT(ANYHEALTHTESTFAIL, REP)
  }
#endif
  nhw_CRACEN_toggle_RNG_intline(new_int_line);
}

/**
 * Check if an error has "occurred"
 */
void check_errors(void) {
  bool error = false;

#define CHECK_SET_PEND_ERROR(err_n) \
  if (rng_st.pend_error_##err_n) { \
    rng_st.error_##err_n = true; \
    error = true; \
  }

  if (rng_st.status == rng_startup) {
    CHECK_SET_PEND_ERROR(startup_test)
  }
  if (rng_st.status == rng_filling) {
    CHECK_SET_PEND_ERROR(rep_test)
    CHECK_SET_PEND_ERROR(prop_test)
    CHECK_SET_PEND_ERROR(ais31_prenoise)
    CHECK_SET_PEND_ERROR(ais31_noise)
  }

  if (error) {
    rng_st.status = rng_error;
    update_state(CRACENCORE_RNGCONTROL_STATUS_STATE_ERROR);
    Timer_CRACEN_NDRNG = TIME_NEVER;
    nhw_CRACEN_update_timer();
    check_interrupts();
  }
}

static void fifo_push(uint32_t data) {
  rng_st.fifo[rng_st.fifo_wptr] = data;
  rng_st.fifo_level +=1; /* no need to check for overflow */
  RNG_regs->FIFOLEVEL = rng_st.fifo_level;
  rng_st.fifo_wptr = (rng_st.fifo_wptr + 1) % rng_st.fifo_size;

  check_interrupts();
}

static uint32_t fifo_pop(void) {
  uint32_t value = rng_st.fifo[rng_st.fifo_rptr];
  if (rng_st.fifo_level > 0) {
    rng_st.fifo_level -=1;
    RNG_regs->FIFOLEVEL = rng_st.fifo_level;
    rng_st.fifo_rptr = (rng_st.fifo_rptr + 1) % rng_st.fifo_size;
  } else {
    bs_trace_warning_time_line("CRACEN RNG pop from empty FIFO\n");
  }

  if (rng_st.fifo_level < (RNG_regs->FIFOTHRESHOLD + 1)* 4) {
    if (rng_st.status == rng_idle_roff) { //Note in v2 there is no idle_roff
      startup();
    } else if (rng_st.status == rng_idle_ron) { //Note: The condition is the same from idle_roff or idle_ron
      generate_more_data();
    }
  }
  check_interrupts();

  return value;
}

static inline void update_state(int state) {
  RNG_regs->STATUS &= ~CRACENCORE_RNGCONTROL_STATUS_STATE_Msk;
  RNG_regs->STATUS |= state << CRACENCORE_RNGCONTROL_STATUS_STATE_Pos;
}

static inline int get_NB128BITBLOCKS(void) {
  return (RNG_regs->CONTROL & CRACENCORE_RNGCONTROL_CONTROL_NB128BITBLOCKS_Msk)
      >> CRACENCORE_RNGCONTROL_CONTROL_NB128BITBLOCKS_Pos;
}

#if NHW_CRACEN_RNG_V >= 2
static inline int get_blending_method(void) {
  return (RNG_regs->CONTROL & CRACENCORE_RNGCONTROL_CONTROL_BLENDINGMETHOD_Msk)
      >> CRACENCORE_RNGCONTROL_CONTROL_BLENDINGMETHOD_Pos;
}

/* Bits per *clock* */
static double raw_entropy_rate_v2(void) {
  uint32_t T = BS_MAX(RNG_regs->SAMPLINGPERIOD,1); //Setting the SAMPLINGPERIOD to 0 is not really correct, but this prevents a div by 0 in the model
  T += RNG_regs->COOLDOWNPERIOD;

  double bit_rate = 4*8 / (double)T; //4 sets(shares) of 8 ring oscillators each producing each 1 bit every T

  switch (get_blending_method()) {
    case 3: //Von Neumann
      bit_rate /=4;
    case 2: //XOR2
      bit_rate /=4;
    case 1: //XOR1
      bit_rate /=8;
    case 0: //Concat
    default:
      break;
  }
  return bit_rate;
}
#endif

static void generate_more_data(void) {
  bs_time_t clocks_next;

  rng_st.status = rng_filling;
  update_state(CRACENCORE_RNGCONTROL_STATUS_STATE_FILLFIFO);

#if NHW_CRACEN_RNG_V < 2
  if (RNG_regs->CONTROL & CRACENCORE_RNGCONTROL_CONTROL_CONDBYPASS_Msk) {
    clocks_next = 32*(RNG_regs->CLKDIV+1);
    rng_st.queued_words = 1;
  } else {
    clocks_next = 128*get_NB128BITBLOCKS()*(RNG_regs->CLKDIV+1);
    rng_st.queued_words = 4;
  }
#else
  if (RNG_regs->CONTROL & CRACENCORE_RNGCONTROL_CONTROL_CONDBYPASS_Msk) {
    clocks_next = 32/raw_entropy_rate_v2();
    rng_st.queued_words = 1;
  } else {
    clocks_next = 128*get_NB128BITBLOCKS()/raw_entropy_rate_v2();
    rng_st.queued_words = 4;
  }
#endif

  clocks_next+=Timer_rem_clocks;
  Timer_CRACEN_NDRNG = clocks_next/NHW_CRACEN_FREQ_MHZ;
  Timer_rem_clocks = clocks_next - (Timer_CRACEN_NDRNG*NHW_CRACEN_FREQ_MHZ);
  Timer_CRACEN_NDRNG += nsi_hws_get_time();
  nhw_CRACEN_update_timer();
  check_errors();
}

void nhw_CRACEN_RNG_timer_triggered(void) {

#if NHW_CRACEN_RNG_V < 2
  if (rng_st.status == rng_idle_ron) {
    rng_st.status = rng_idle_roff;
    update_state(CRACENCORE_RNGCONTROL_STATUS_STATE_IDLEROFF);
    Timer_CRACEN_NDRNG = TIME_NEVER;
    nhw_CRACEN_update_timer();
    return;

  } else
#endif
  if ((rng_st.status == rng_filling)) {
    while ((rng_st.queued_words > 0) && (rng_st.fifo_level < rng_st.fifo_size)) {
      fifo_push(bs_random_uint32());
      rng_st.queued_words--;
    }
    rng_st.queued_words = 0; //Discard a possible remainder
  }

  if (rng_st.fifo_level < rng_st.fifo_size) {
    generate_more_data();
  } else {
    rng_st.status = rng_idle_ron;
    update_state(CRACENCORE_RNGCONTROL_STATUS_STATE_IDLERON);
#if NHW_CRACEN_RNG_V < 2
    Timer_CRACEN_NDRNG = nsi_hws_get_time() + RNG_regs->SWOFFTMRVAL/NHW_CRACEN_FREQ_MHZ;
#else
    Timer_CRACEN_NDRNG = TIME_NEVER;
#endif
    nhw_CRACEN_update_timer();
  }
}

static void startup(void) {
  rng_st.status = rng_startup;
  update_state(CRACENCORE_RNGCONTROL_STATUS_STATE_STARTUP);

#if NHW_CRACEN_RNG_V >= 2
#define INITWAITVAL WARMUPPERIOD
#endif
  Timer_CRACEN_NDRNG = nsi_hws_get_time() + RNG_regs->INITWAITVAL/NHW_CRACEN_FREQ_MHZ + NHW_CRACEN_STARTUPTEST_DUR; //TODO: do start up tests add any delay?
#undef INITWAITVAL
  nhw_CRACEN_update_timer();
  check_errors();
}

void nhw_CRACEN_RNG_regw_sideeffects_CONTROL(void) {
  bool enabled = RNG_regs->CONTROL & CRACENCORE_RNGCONTROL_CONTROL_ENABLE_Msk;

  if (RNG_regs->CONTROL & CRACENCORE_RNGCONTROL_CONTROL_SOFTRST_Msk) {
    nhw_CRACEN_soft_reset();
    enabled = false;
  }

  if (!enabled) {
    rng_st.enabled = false;
    Timer_CRACEN_NDRNG = TIME_NEVER;
    nhw_CRACEN_update_timer();
    rng_st.status = rng_reset;
    update_state(CRACENCORE_RNGCONTROL_STATUS_STATE_RESET);
    return;
  }

  if ((NRF_CRACEN_regs.ENABLE & CRACEN_ENABLE_RNG_Msk) == 0) {
    bs_trace_warning_time_line("Attempting to enable CRACEN RNG while the CRACEN wrap logic is off\n");
  }

  check_interrupts();

  if (rng_st.enabled == true) { //Already enabled
    return; //Nothing more to do (we don't handle too properly on the fly CONFIG changes)
  }

  rng_st.enabled = true;

  startup();
}

uint32_t nhw_CRACEN_RNG_regr_sideeffects_FIFO(void) {
#if NHW_CRACEN_RNG_V < 2
  if ((rng_st.status == rng_reset) && (rng_st.fifo_level == 0)) {
    RNG_regs->STATUS |= CRACENCORE_RNGCONTROL_STATUS_FIFOACCFAIL_Msk;
  }
#endif

  RNG_regs->FIFO[0] = fifo_pop();
  return RNG_regs->FIFO[0];
}

void nhw_CRACEN_RNG_regw_sideeffects_FIFOLEVEL(void) {
  RNG_regs->STATUS &= ~CRACENCORE_RNGCONTROL_STATUS_FULLINT_Msk;
  RNG_regs->FIFOLEVEL = rng_st.fifo_level;
  check_interrupts();
}

void nhw_CRACEN_RNG_fake_test_error(uint32_t mask) {
  if (mask & CRACEN_RNG_FAKE_STARTUP_ERROR) {
    rng_st.pend_error_startup_test = true;
  }
  if (mask & CRACEN_RNG_FAKE_REP_TEST_ERROR) {
    rng_st.pend_error_rep_test = true;
  }
  if (mask & CRACEN_RNG_FAKE_PROP_TEST_ERROR) {
    rng_st.pend_error_prop_test = true;
  }
  if (mask & CRACEN_RNG_FAKE_AIS31_PRENOISE_ERROR) {
    rng_st.pend_error_ais31_prenoise = true;
  }
  if (mask & CRACEN_RNG_FAKE_AIS31_NOISE_ERROR) {
    rng_st.pend_error_ais31_noise = true;
  }
  check_errors();
}
