/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_PWM_H
#define _NRF_HW_MODEL_PWM_H

#include "bs_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"

#if defined(NRF52833) || defined(NRF52833_XXAA) || \
    defined(NRF54L15) || defined(NRF54L15_XXAA)

extern NRF_PWM_Type NRF_PWM_regs[NHW_PWM_TOTAL_INST];

void nhw_pwm_regw_sideeffects_TASKS_STOP(uint inst);
void nhw_pwm_regw_sideeffects_TASKS_SEQSTART(uint inst, uint seq);
void nhw_pwm_regw_sideeffects_TASKS_NEXTSTEP(uint inst);
void nhw_pwm_regw_sideeffects_INTENSET(uint inst);
void nhw_pwm_regw_sideeffects_INTENCLR(uint inst);
void nhw_pwm_regw_sideeffects_INTEN(uint inst);
void nhw_pwm_regw_sideeffects_EVENTS_all(uint inst);

#if NHW_HAS_DPPI
void nhw_pwm_regw_sideeffects_ENABLE(uint inst);
void nhw_pwm_regw_sideeffects_SUBSCRIBE_STOP(uint inst);
void nhw_pwm_regw_sideeffects_SUBSCRIBE_NEXTSTEP(uint inst);
void nhw_pwm_regw_sideeffects_SUBSCRIBE_SEQSTART(uint inst, uint seq);
#endif

void nhw_pwm_TASK_STOP(uint inst);
void nhw_pwm_TASK_SEQSTART(uint inst, uint seq);
void nhw_pwm_TASK_NEXTSTEP(uint inst);

#if NHW_HAS_PPI
#define NHW_PWM_PPI_TASKS(inst) \
  void nhw_pwm##inst##_TASK_STOP(void); \
  void nhw_pwm##inst##_TASK_SEQSTART_0(void); \
  void nhw_pwm##inst##_TASK_SEQSTART_1(void); \
  void nhw_pwm##inst##_TASK_NEXTSTEP(void)

NHW_PWM_PPI_TASKS(0);
NHW_PWM_PPI_TASKS(1);
NHW_PWM_PPI_TASKS(2);
NHW_PWM_PPI_TASKS(3);
#undef NHW_PWM_PPI_TASKS
#endif

#endif /* NRF52833 || NRF54L15 */

#endif /* _NRF_HW_MODEL_PWM_H */
