/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * nRF54L
 * CLOCK - Clock control
 * POWER - POWER control
 * RESET - RESET control
 *
 * Note: CLOCK:
 * 1. This model ignores the system ON/OFF status
 *
 * 2. We assume the 32.768 KHz clock does not drift relative to the 64MHz one.
 *    This would only be the case if they had the same source or there was a tracking
 *    and adjustment loop of one based on the other.
 *
 * 3. By default all tasks complete in 1 delta cycle, but it is possible to
 *    cause them to take time by using the nhw_clock_cheat_* APIs
 *
 * 4. LFCLK.SRC is mostly ignored (beyond copying LFCLK.SRC to LFCLK.SRCCOPY and LFCLK.STAT)
 *    and optionally taking a different amount of time to start.
 *
 * 5. Turning the clocks on/off has no effect on other peripherals models. Other peripherals
 *    do not request clocks to this model.
 *    So triggering a clock STOP task will always be done right away, without needing
 *    to wait for a peripheral which is using that clock to stop requesting it.
 *
 *    There is no relationship to the OSCILLATORS peripheral yet.
 *
 * 6. From the spec, it is unclear if the *.RUN.STATUS register fields are kept on even
 *    after the operation is completed or they are cleared.
 *    The model just leaves them at 1, unless the STOP task is triggered.
 *
 * 7. XOTUNE does nothing more than generate the XOTUNED/XOTUNEFAILED event. It will only fail
 *    if set to do so with the nhw_clock_cheat_set_xotune_fail() interface.
 *
 * 8. The models do not check the requirement of having the HFXO clock running to be
 *    able to run the RADIO. The RADIO models will run just fine without it.
 *
 * Notes: POWER & RESET
 * * Only a register stub without functionality is provided
 */

#include <string.h>
#include <stdint.h>
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_54L_CLOCK.h"
#include "NHW_templates.h"
#include "NHW_xPPI.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"
#include "bs_utils.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

union NRF_CLKPWR_Type NRF_CLKPWR_regs[NHW_CLKPWR_TOTAL_INST];

NRF_CLOCK_Type *NRF_CLOCK_regs[NHW_CLKPWR_TOTAL_INST];
#if (NHW_CLKPWR_HAS_POWER)
NRF_POWER_Type *NRF_POWER_regs[NHW_CLKPWR_TOTAL_INST];
#endif
NRF_RESET_Type *NRF_RESET_regs[NHW_CLKPWR_TOTAL_INST];

enum clock_states {Stopped = 0, Starting, Started, Stopping};
enum tuning_states {Tunning_stopped = 0, Tuning_ok, Tuning_fail};

struct clkpwr_status {
  bs_time_t Timer_XO;
  bs_time_t Timer_PLL;
  bs_time_t Timer_LFCLK;
  bs_time_t Timer_CAL;
  bs_time_t Timer_XOTUNE;

  enum clock_states XO_state;
  enum clock_states LFCLK_state;
  enum clock_states PLL_state;
  enum clock_states CAL_state;
  enum tuning_states XOTUNE_state;

  bs_time_t CLOCK_start_times[NHW_CLKPWR_N_CLKS][NHW_CLKPWR_CLK_MAX_N_SRCS];
  bs_time_t XOtuning_durations[2]; /* 0: Success, 1: Fail */
  bs_time_t XOtuning_pending_fails;
  bs_time_t CAL_duration;
};

static bs_time_t Timer_PWRCLK = TIME_NEVER;
static struct clkpwr_status nhw_clkpwr_st;
static uint nhw_CLOCK_dppi_map[] = NHW_CLKPWR_DPPI_MAP;

static void nhw_CLOCK_XOTimer_triggered(void);
static void nhw_CLOCK_LFCLK_triggered(void);
static void nhw_CLOCK_PLLTimer_triggered(void);

static void nhw_CLOCK_update_master_timer(void) {

  Timer_PWRCLK = TIME_NEVER;

  bs_time_t t1 = BS_MIN(BS_MIN(nhw_clkpwr_st.Timer_XO, nhw_clkpwr_st.Timer_PLL), nhw_clkpwr_st.Timer_XOTUNE);
  bs_time_t t2 = BS_MIN(nhw_clkpwr_st.Timer_LFCLK, nhw_clkpwr_st.Timer_CAL);

  bs_time_t el_min = BS_MIN(t1, t2);
  if (el_min < Timer_PWRCLK) {
    Timer_PWRCLK = el_min;
  }
  nsi_hws_find_next_event();
}

static void nhw_CLOCK_init(void) {
  NRF_CLOCK_regs[0] = (NRF_CLOCK_Type *)&NRF_CLKPWR_regs[0];
#if (NHW_CLKPWR_HAS_POWER)
  NRF_POWER_regs[0] = (NRF_POWER_Type *)&NRF_CLKPWR_regs[0];
#endif
  NRF_RESET_regs[0] = (NRF_RESET_Type *)&NRF_CLKPWR_regs[0];

  memset(NRF_CLKPWR_regs, 0, sizeof(NRF_CLKPWR_regs));

  nhw_clkpwr_st.Timer_XO    = TIME_NEVER;
  nhw_clkpwr_st.Timer_PLL   = TIME_NEVER;
  nhw_clkpwr_st.Timer_LFCLK = TIME_NEVER;
  nhw_clkpwr_st.Timer_CAL   = TIME_NEVER;
  nhw_clkpwr_st.Timer_XOTUNE= TIME_NEVER;

  nhw_clkpwr_st.XO_state    = Stopped;
  nhw_clkpwr_st.LFCLK_state = Stopped;
  nhw_clkpwr_st.PLL_state   = Stopped;
  nhw_clkpwr_st.CAL_state   = Stopped;
  nhw_clkpwr_st.XOTUNE_state= Tunning_stopped;

  nhw_CLOCK_update_master_timer();

  bs_time_t nhw_clock_start_times[NHW_CLKPWR_N_CLKS][NHW_CLKPWR_CLK_MAX_N_SRCS] = NHW_CLKPWR_CLK_START_TIMES;
  memcpy(nhw_clkpwr_st.CLOCK_start_times, nhw_clock_start_times, sizeof(nhw_clock_start_times));

  bs_time_t nhw_clock_tune_durs[2] = NHW_CLKPWR_CLK_XOTUNE_TIMES;
  memcpy(nhw_clkpwr_st.XOtuning_durations, nhw_clock_tune_durs, sizeof(nhw_clock_tune_durs));

  nhw_clkpwr_st.CAL_duration = NHW_CLKPWR_CLK_CAL_TIME;
}

NSI_TASK(nhw_CLOCK_init, HW_INIT, 100);

static void nhw_CLOCK_eval_interrupt(uint inst) {
  (void) inst;
  static struct nhw_irq_mapping nhw_CLOCK_irq_map[] = NHW_CLKPWR_INT_MAP;
  static bool clock_int_line; /* Is the CLOCK currently driving its interrupt line high */
  bool new_int_line = false;

  NRF_CLOCK_regs[0]->INTPEND = 0;

  #define check_interrupt(x) \
		if (NRF_CLOCK_regs[0]->EVENTS_ ##x && (NRF_CLOCK_regs[0]->INTEN & CLOCK_INTENSET_## x ##_Msk)){ \
		    new_int_line = 1; \
		    NRF_CLOCK_regs[0]->INTPEND |= CLOCK_INTENSET_## x ##_Msk; \
		}

  check_interrupt(XOSTARTED)
  check_interrupt(PLLSTARTED)
#if (NHW_CLKPWR_HAS_LFCLK)
  check_interrupt(LFCLKSTARTED)
#endif
  check_interrupt(DONE)
  check_interrupt(XOTUNED)
  check_interrupt(XOTUNEERROR)
  check_interrupt(XOTUNEFAILED)

  hw_irq_ctrl_toggle_level_irq_line_if(&clock_int_line,
                                        new_int_line,
                                        &nhw_CLOCK_irq_map[0]);
}

static void nhw_CLOCK_TASK_XOSTART(uint inst) {
  (void) inst;
  if ((nhw_clkpwr_st.XO_state == Stopped ) || (nhw_clkpwr_st.XO_state == Stopping)) {
    nhw_clkpwr_st.XO_state = Starting;
    NRF_CLOCK_regs[0]->XO.RUN = CLOCK_XO_RUN_STATUS_Msk;
    nhw_clkpwr_st.Timer_XO = nsi_hws_get_time() + nhw_clkpwr_st.CLOCK_start_times[NHW_CLKPWR_CLK_IDX_XO][0];
    nhw_CLOCK_update_master_timer();
  }
}

static void nhw_CLOCK_TASK_XOSTOP(uint inst) {
  (void) inst;
  if ((nhw_clkpwr_st.XO_state != Stopping) && (nhw_clkpwr_st.XO_state != Stopped)) {
    nhw_clkpwr_st.XO_state = Stopping;
    NRF_CLOCK_regs[0]->XO.RUN = 0;
    /* Instantaneous stop */
    nhw_CLOCK_XOTimer_triggered();
  }
}

static void nhw_CLOCK_TASK_PLLSTART(uint inst) {
  (void) inst;
  if ((nhw_clkpwr_st.PLL_state == Stopped ) || (nhw_clkpwr_st.PLL_state == Stopping)) {
    nhw_clkpwr_st.PLL_state = Starting;
    NRF_CLOCK_regs[0]->PLL.RUN = CLOCK_PLL_RUN_STATUS_Msk;
    nhw_clkpwr_st.Timer_PLL = nsi_hws_get_time();
    nhw_CLOCK_update_master_timer();
  }
}

static void nhw_CLOCK_TASK_PLLSTOP(uint inst) {
  (void) inst;
  if ((nhw_clkpwr_st.PLL_state == Started) || (nhw_clkpwr_st.PLL_state == Starting)) {
    nhw_clkpwr_st.PLL_state = Stopping;
    NRF_CLOCK_regs[0]->PLL.RUN = 0;
    /* Instantaneous stop */
    nhw_CLOCK_PLLTimer_triggered();
  }
}

#if (NHW_CLKPWR_HAS_LFCLK)
static void nhw_CLOCK_TASK_LFCLKSTART(uint inst) {
  (void) inst;
  if ((nhw_clkpwr_st.LFCLK_state == Stopped ) || (nhw_clkpwr_st.LFCLK_state == Stopping)) {
    uint src = NRF_CLOCK_regs[0]->LFCLK.SRC & CLOCK_LFCLK_SRC_SRC_Msk;

    nhw_clkpwr_st.LFCLK_state = Starting;
    NRF_CLOCK_regs[0]->LFCLK.RUN = CLOCK_LFCLK_RUN_STATUS_Msk;
    NRF_CLOCK_regs[0]->LFCLK.SRCCOPY = src;
    nhw_clkpwr_st.Timer_LFCLK = nsi_hws_get_time() + nhw_clkpwr_st.CLOCK_start_times[NHW_CLKPWR_CLK_IDX_LF][src];
    nhw_CLOCK_update_master_timer();
  }
}

static void nhw_CLOCK_TASK_LFCLKSTOP(uint inst) {
  (void) inst;
  if ((nhw_clkpwr_st.LFCLK_state == Started) || (nhw_clkpwr_st.LFCLK_state == Starting)) {
    nhw_clkpwr_st.LFCLK_state = Stopping;
    NRF_CLOCK_regs[0]->LFCLK.RUN = 0;
    /* Instantaneous stop */
    nhw_CLOCK_LFCLK_triggered();
  }
}
#endif

static void nhw_CLOCK_TASK_CAL(uint inst) {
  (void) inst;
  if (nhw_clkpwr_st.XO_state != Started) { /* LCOV_EXCL_START */
    bs_trace_warning_line("%s: Triggered RC oscillator calibration with the HFXO CLK stopped "
                          "(the model does not have a problem with this, but this is against "
                          "the spec)\n", __func__);
  } /* LCOV_EXCL_STOP */

  if ((nhw_clkpwr_st.CAL_state == Stopped ) || (nhw_clkpwr_st.CAL_state == Stopping)) {
    nhw_clkpwr_st.CAL_state = Starting;
    nhw_clkpwr_st.Timer_CAL = nsi_hws_get_time() + nhw_clkpwr_st.CAL_duration;
    nhw_CLOCK_update_master_timer();
  }
}

static void nhw_CLOCK_TASK_XOTUNE(uint inst) {
  (void)inst;

  if (nhw_clkpwr_st.XO_state != Started) {
    bs_trace_warning_time_line("TASK XOTUNE triggered but XO was not started\n");
  }

  if (nhw_clkpwr_st.XOTUNE_state == Tunning_stopped) {
    if (nhw_clkpwr_st.XOtuning_pending_fails > 0) {
      nhw_clkpwr_st.XOtuning_pending_fails--;
      nhw_clkpwr_st.XOTUNE_state = Tuning_fail;
      nhw_clkpwr_st.Timer_XOTUNE = nsi_hws_get_time() + nhw_clkpwr_st.XOtuning_durations[1];
    } else {
      nhw_clkpwr_st.XOTUNE_state = Tuning_ok;
      nhw_clkpwr_st.Timer_XOTUNE = nsi_hws_get_time() + nhw_clkpwr_st.XOtuning_durations[0];
    }
    nhw_CLOCK_update_master_timer();
  }
}

static void nhw_CLOCK_TASK_XOTUNEABORT(uint inst) {
  (void) inst;
  nhw_clkpwr_st.XOTUNE_state = Tunning_stopped;
  nhw_clkpwr_st.Timer_XOTUNE = TIME_NEVER;
  nhw_CLOCK_update_master_timer();
}

NHW_SIDEEFFECTS_INTSET(CLOCK, NRF_CLOCK_regs[0]->, NRF_CLOCK_regs[0]->INTEN)
NHW_SIDEEFFECTS_INTCLR(CLOCK, NRF_CLOCK_regs[0]->, NRF_CLOCK_regs[0]->INTEN)
NHW_SIDEEFFECTS_INTEN(CLOCK, NRF_CLOCK_regs[0]->, NRF_CLOCK_regs[0]->INTEN)

NHW_SIDEEFFECTS_EVENTS(CLOCK)

void nhw_pwrclk_regw_sideeffects_EVENTS_all(uint inst) {
  nhw_CLOCK_eval_interrupt(inst);
}

#if !defined(CLOCK_PUBLISH_XOSTARTED_ResetValue) /* no PUBLISH registers in MDK */
#undef NHW_SIGNAL_EVENT
#define NHW_SIGNAL_EVENT(peri, peri_regs, event) \
  void nhw_##peri##_signal_EVENTS_##event(unsigned int inst) \
  { \
      peri_regs EVENTS_##event = 1; \
      nhw_##peri##_eval_interrupt(inst); \
  }
#endif

NHW_SIGNAL_EVENT(CLOCK, NRF_CLOCK_regs[0]->, XOSTARTED)
NHW_SIGNAL_EVENT(CLOCK, NRF_CLOCK_regs[0]->, PLLSTARTED)
#if (NHW_CLKPWR_HAS_LFCLK)
NHW_SIGNAL_EVENT(CLOCK, NRF_CLOCK_regs[0]->, LFCLKSTARTED)
#endif
NHW_SIGNAL_EVENT(CLOCK, NRF_CLOCK_regs[0]->, DONE)
NHW_SIGNAL_EVENT(CLOCK, NRF_CLOCK_regs[0]->, XOTUNED)
NHW_SIGNAL_EVENT(CLOCK, NRF_CLOCK_regs[0]->, XOTUNEERROR)
NHW_SIGNAL_EVENT(CLOCK, NRF_CLOCK_regs[0]->, XOTUNEFAILED)

NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, XOSTART)
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, XOSTOP)
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, PLLSTART)
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, PLLSTOP)
#if (NHW_CLKPWR_HAS_LFCLK)
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, LFCLKSTART)
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, LFCLKSTOP)
#endif
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, CAL)
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, XOTUNE)
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, XOTUNEABORT)

#define NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(task) \
  static void nhw_CLOCK_TASK_##task##_wrap(void *param) { \
    nhw_CLOCK_TASK_##task((int) param); \
  } \
  void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_##task(unsigned int inst) { \
    static struct nhw_subsc_mem task##_subscribed[NHW_CLKPWR_TOTAL_INST]; \
    nhw_dppi_common_subscribe_sideeffect(nhw_CLOCK_dppi_map[inst], \
        NRF_CLOCK_regs[0]->SUBSCRIBE_##task, \
        &task##_subscribed[inst], \
        nhw_CLOCK_TASK_##task##_wrap, \
        (void*) inst); \
  }

NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(XOSTART)
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(XOSTOP)
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(PLLSTART)
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(PLLSTOP)
#if (NHW_CLKPWR_HAS_LFCLK)
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(LFCLKSTART)
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(LFCLKSTOP)
#endif
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(CAL)
#if defined(CLOCK_SUBSCRIBE_XOTUNE_EN_Msk)
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(XOTUNE)
#endif
#if defined(CLOCK_SUBSCRIBE_XOTUNEABORT_EN_Msk)
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(XOTUNEABORT)
#endif

static void nhw_CLOCK_XOTUNEtimer_triggered(void);

static void nhw_CLOCK_XOTimer_triggered(void) {
  nhw_clkpwr_st.Timer_XO = TIME_NEVER;
  nhw_CLOCK_update_master_timer();

  if ( nhw_clkpwr_st.XO_state == Starting ){
    nhw_clkpwr_st.XO_state = Started;

    NRF_CLOCK_regs[0]->XO.STAT = CLOCK_XO_STAT_STATE_Msk;

    nhw_CLOCK_signal_EVENTS_XOSTARTED(0);
    if ((nhw_clkpwr_st.XOtuning_durations[0] == 0) && (nhw_clkpwr_st.XOtuning_pending_fails == 0)) {
      //Let's raise the event in this same delta cycle
      nhw_CLOCK_XOTUNEtimer_triggered();
    } else {
      nhw_CLOCK_TASK_XOTUNE(0);
    }
  } else if ( nhw_clkpwr_st.XO_state == Stopping ){
    nhw_clkpwr_st.XO_state = Stopped;
    NRF_CLOCK_regs[0]->XO.STAT = 0;
  }
}

static void nhw_CLOCK_PLLTimer_triggered(void) {
  nhw_clkpwr_st.Timer_PLL = TIME_NEVER;
  nhw_CLOCK_update_master_timer();

  if ( nhw_clkpwr_st.PLL_state == Starting ){
    nhw_clkpwr_st.PLL_state = Started;

    NRF_CLOCK_regs[0]->PLL.STAT = CLOCK_PLL_STAT_STATE_Msk;

    nhw_CLOCK_signal_EVENTS_PLLSTARTED(0);

  } else if ( nhw_clkpwr_st.PLL_state == Stopping ){
    nhw_clkpwr_st.PLL_state = Stopped;
    NRF_CLOCK_regs[0]->PLL.STAT = 0;
  }
}

static void nhw_CLOCK_LFCLK_triggered(void) {
#if (NHW_CLKPWR_HAS_LFCLK)
  nhw_clkpwr_st.Timer_LFCLK = TIME_NEVER;
  nhw_CLOCK_update_master_timer();

  if ( nhw_clkpwr_st.LFCLK_state == Starting ){
    nhw_clkpwr_st.LFCLK_state = Started;

    NRF_CLOCK_regs[0]->LFCLK.STAT = CLOCK_LFCLK_STAT_STATE_Msk
                               | (NRF_CLOCK_regs[0]->LFCLK.SRCCOPY << CLOCK_LFCLK_STAT_SRC_Pos);

    nhw_CLOCK_signal_EVENTS_LFCLKSTARTED(0);

  } else if ( nhw_clkpwr_st.LFCLK_state == Stopping ){
    nhw_clkpwr_st.LFCLK_state = Stopped;
    NRF_CLOCK_regs[0]->LFCLK.STAT = 0;
  }
#endif
}

static void nhw_CLOCK_CALtimer_triggered(void) {
  nhw_clkpwr_st.CAL_state = Stopped;
  nhw_clkpwr_st.Timer_CAL = TIME_NEVER;
  nhw_CLOCK_update_master_timer();
  nhw_CLOCK_signal_EVENTS_DONE(0);
}

static void nhw_CLOCK_XOTUNEtimer_triggered(void) {

  bool failed = (nhw_clkpwr_st.XOTUNE_state == Tuning_fail);

  nhw_clkpwr_st.XOTUNE_state = Tunning_stopped;
  nhw_clkpwr_st.Timer_XOTUNE = TIME_NEVER;
  nhw_CLOCK_update_master_timer();

  if (failed) {
    nhw_CLOCK_signal_EVENTS_XOTUNEFAILED(0);
  } else {
    nhw_CLOCK_signal_EVENTS_XOTUNED(0);
  }

}

static void nhw_pwrclk_timer_triggered(void) {
  if (Timer_PWRCLK == nhw_clkpwr_st.Timer_XO) {
    nhw_CLOCK_XOTimer_triggered();
  } else if (Timer_PWRCLK == nhw_clkpwr_st.Timer_PLL) {
    nhw_CLOCK_PLLTimer_triggered();
  } else if (Timer_PWRCLK == nhw_clkpwr_st.Timer_LFCLK) {
    nhw_CLOCK_LFCLK_triggered();
  } else if (Timer_PWRCLK == nhw_clkpwr_st.Timer_CAL) {
    nhw_CLOCK_CALtimer_triggered();
  } else if (Timer_PWRCLK == nhw_clkpwr_st.Timer_XOTUNE) {
    nhw_CLOCK_XOTUNEtimer_triggered();
  } else { /* LCOV_EXCL_START */
    bs_trace_error_time_line("%s programming error\n", __func__);
  } /* LCOV_EXCL_STOP */
}

NSI_HW_EVENT(Timer_PWRCLK, nhw_pwrclk_timer_triggered, 50);

void nhw_clock_cheat_set_start_time(uint inst, uint clock, uint source, bs_time_t time) {
  (void)inst;
  for (int c = 0; c < NHW_CLKPWR_N_CLKS; c++) {
    if ((clock == -1) || (c == clock)) {
      for (int s = 0; s < NHW_CLKPWR_CLK_MAX_N_SRCS; s++) {
        if ((source == -1) || (s == source)) {
          nhw_clkpwr_st.CLOCK_start_times[c][s] = time;
        }
      }
    }
  }
}

void nhw_clock_cheat_set_xotune_time(uint inst, bs_time_t success_time, bs_time_t fail_time) {
  (void)inst;
  nhw_clkpwr_st.XOtuning_durations[0] = success_time;
  nhw_clkpwr_st.XOtuning_durations[1] = fail_time;
}

void nhw_clock_cheat_set_xotune_fail(uint inst, uint fail_count) {
  (void)inst;
  nhw_clkpwr_st.XOtuning_pending_fails = fail_count;
}

void nhw_clock_cheat_trigger_xotune_error(uint inst)
{
  if (nhw_clkpwr_st.XO_state != Started) {
    bs_trace_warning_line("TUNEERROR event can only be generated when running");
  }
  nhw_CLOCK_signal_EVENTS_XOTUNEERROR(0);
}

void nhw_clock_cheat_set_calibrate_time(uint inst, bs_time_t time) {
  (void)inst;
  nhw_clkpwr_st.CAL_duration = time;
}
