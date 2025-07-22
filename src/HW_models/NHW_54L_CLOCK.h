/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_54L_CLOCK_H
#define _NRF_HW_MODEL_54L_CLOCK_H

#include "bs_types.h"
#include "NHW_config.h"

#ifdef __cplusplus
extern "C"{
#endif

void nhw_CLOCK_regw_sideeffects_INTENSET(uint i);
void nhw_CLOCK_regw_sideeffects_INTENCLR(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_XOSTART(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_XOSTOP(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_PLLSTART(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_PLLSTOP(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_LFCLKSTART(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_LFCLKSTOP(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_CAL(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_XOTUNE(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_XOTUNEABORT(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_XO24MSTART(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_XO24MSTOP(uint i);

/* Side-effecting function when any event register is written: */
void nhw_pwrclk_regw_sideeffects_EVENTS_all(uint i);

void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_XOSTART(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_XOSTOP(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_PLLSTART(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_PLLSTOP(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_LFCLKSTART(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_LFCLKSTOP(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_CAL(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_XOTUNE(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_XOTUNEABORT(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_XO24MSTART(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_XO24MSTOP(uint i);

/*
 * Set how long it will take for the clock to start after triggering its start task
 * Where <inst> is 0.
 * <clock> is one of NHW_CLKPWR_CLK_IDX_* (0 for the LFCLK and 1 for the HFCLK).
 *   You can set it to -1 to set the start time for all clocks.
 * <source> is the clock source for which that timings applies:
 *   For the LFCLK this has the same definition as the values set in the LFCLKSRC.SRC field
 *   For the XO it is 0
 *   You can set it to -1 to set the value for all sources
 * <time> is the start duration.
 */
void nhw_clock_cheat_set_start_time(uint inst, uint clock, uint source, bs_time_t time);

/*
 * Set how long it will take for the LF clock to calibrate after triggering its CAL task
 * Where <inst> is 0
 * <time> is the calibration duration in microseconds.
 */
void nhw_clock_cheat_set_calibrate_time(uint inst, bs_time_t time);

/*
 * Set how long it will take for the XO tuning to start after triggering the XOTUNE task
 * Where <inst> is 0
 * <success_time> is the tuning duration in microseconds for a successful tune
 * <fail_time> is the tuning duration in microseconds for a failed tune
 */
void nhw_clock_cheat_set_xotune_time(uint inst, bs_time_t success_time, bs_time_t fail_time);

/*
 * Set if and how many times the XO tuning will fail
 * Where <inst> is 0
 * <fail_count> is how many times the next tune attempts will fail. Set it to 0 for it to pass
 * in the next started attempt. (Note the decision about a failure is done during the XOTUNE task trigger.
 */
void nhw_clock_cheat_set_xotune_fail(uint inst, uint fail_count);

/*
 * Generate the XOTUNEERROR event
 * Where <inst> is 0
 */
void nhw_clock_cheat_trigger_xotune_error(uint inst);

union NRF_CLKPWR_Type {
  NRF_CLOCK_Type CLK;
#if (NHW_CLKPWR_HAS_POWER)
  NRF_POWER_Type PWR;
#endif
  NRF_RESET_Type RST;
};

extern union NRF_CLKPWR_Type NRF_CLKPWR_regs[];

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_54L_CLOCK_H */
