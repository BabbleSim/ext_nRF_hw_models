/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_CLOCK_H
#define _NRF_HW_MODEL_CLOCK_H

#ifdef __cplusplus
extern "C"{
#endif

void nrf_clock_TASKS_LFCLKSTART(void);
void nrf_clock_TASKS_LFCLKSTOP(void);
void nrf_clock_TASKS_HFCLKSTART(void);
void nrf_clock_TASKS_HFCLKSTOP(void);
void nrf_clock_TASKS_CAL(void);
void nrf_clock_TASKS_CTSTART(void);
void nrf_clock_TASKS_CTSTOP(void);
void nrf_clock_reqw_sideeffects_INTENSET(void);
void nrf_clock_reqw_sideeffects_INTENCLR(void);
void nrf_clock_reqw_sideeffects_TASKS_LFCLKSTART(void);
void nrf_clock_reqw_sideeffects_TASKS_LFCLKSTOP(void);
void nrf_clock_reqw_sideeffects_TASKS_HFCLKSTART(void);
void nrf_clock_reqw_sideeffects_TASKS_HFCLKSTOP(void);
void nrf_clock_reqw_sideeffects_TASKS_CAL(void);
void nrf_clock_reqw_sideeffects_TASKS_CTSTART(void);
void nrf_clock_reqw_sideeffects_TASKS_CTSTOP(void);
/* Side-effecting function when any event register is written: */
void nrf_clock_regw_sideeffects_EVENTS_all(void);

#ifdef __cplusplus
}
#endif

#endif
