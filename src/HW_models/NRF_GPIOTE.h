/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_GPIOTE_H
#define _NRF_HW_MODEL_GPIOTE_H

#ifdef __cplusplus
extern "C"{
#endif

void nrf_gpiote_port_detect_raise(unsigned int inst, unsigned int port);

void nrf_gpiote_regw_sideeffects_TASKS_OUT(unsigned int inst, unsigned int n);
void nrf_gpiote_regw_sideeffects_TASKS_SET(unsigned int inst, unsigned int n);
void nrf_gpiote_regw_sideeffects_TASKS_CLR(unsigned int inst, unsigned int n);
void nrf_gpiote_regw_sideeffects_EVENTS_IN(unsigned int inst, unsigned int n);
void nrf_gpiote_regw_sideeffects_EVENTS_PORT(unsigned int inst);
void nrf_gpiote_regw_sideeffects_INTENSET(unsigned int inst, unsigned int interrupt_nbr);
void nrf_gpiote_regw_sideeffects_INTENCLR(unsigned int inst, unsigned int interrupt_nbr);
void nrf_gpiote_regw_sideeffects_CONFIG(unsigned int inst, unsigned int n);

#if NHW_HAS_PPI
void nrf_gpiote_TASKS_SET_0(void);
void nrf_gpiote_TASKS_SET_1(void);
void nrf_gpiote_TASKS_SET_2(void);
void nrf_gpiote_TASKS_SET_3(void);
void nrf_gpiote_TASKS_SET_4(void);
void nrf_gpiote_TASKS_SET_5(void);
void nrf_gpiote_TASKS_SET_6(void);
void nrf_gpiote_TASKS_SET_7(void);
void nrf_gpiote_TASKS_CLR_0(void);
void nrf_gpiote_TASKS_CLR_1(void);
void nrf_gpiote_TASKS_CLR_2(void);
void nrf_gpiote_TASKS_CLR_3(void);
void nrf_gpiote_TASKS_CLR_4(void);
void nrf_gpiote_TASKS_CLR_5(void);
void nrf_gpiote_TASKS_CLR_6(void);
void nrf_gpiote_TASKS_CLR_7(void);
void nrf_gpiote_TASKS_OUT_0(void);
void nrf_gpiote_TASKS_OUT_1(void);
void nrf_gpiote_TASKS_OUT_2(void);
void nrf_gpiote_TASKS_OUT_3(void);
void nrf_gpiote_TASKS_OUT_4(void);
void nrf_gpiote_TASKS_OUT_5(void);
void nrf_gpiote_TASKS_OUT_6(void);
void nrf_gpiote_TASKS_OUT_7(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
