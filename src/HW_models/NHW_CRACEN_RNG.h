/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_CRACEN_RNG_H
#define _NRF_HW_MODEL_CRACEN_RNG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

void nhw_CRACEN_RNG_init(void);
void nhw_CRACEN_RNG_regw_sideeffects_CONTROL(void);
void nhw_CRACEN_RNG_regw_sideeffects_FIFOLEVEL(void);
uint32_t nhw_CRACEN_RNG_regr_sideeffects_FIFO(void);
void nhw_CRACEN_RNG_timer_triggered(void);

#define CRACEN_RNG_FAKE_AIS31_NOISE_ERROR 0x1
#define CRACEN_RNG_FAKE_AIS31_PRENOISE_ERROR 0x2
#define CRACEN_RNG_FAKE_PROP_TEST_ERROR 0x4
#define CRACEN_RNG_FAKE_REP_TEST_ERROR 0x8
#define CRACEN_RNG_FAKE_STARTUP_ERROR 0x10
/*
 * Trigger (or queue) errors in the CRACEN RNG
 * If an error cannot occur now, it will be "pended", and whenever the FSM
 * is in a state in which it can happen it will be triggered
 *
 * Note: pended or triggered errors can only be cleared with a softreset of the RNG
 */
void nhw_CRACEN_RNG_fake_test_error(uint32_t mask);

#ifdef __cplusplus
}
#endif

#endif
