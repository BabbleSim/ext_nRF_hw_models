/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "bs_tracing.h"

#define PERIPHERAL_REG_BASE(per, nbr, post) \
    (void*)NRF_##per##nbr##post##_BASE

#define IS_PERIPHERAL_REG(p, per, nbr, post) \
    (p >= PERIPHERAL_REG_BASE(per, nbr, post)) && \
    ((uintptr_t)p < (uintptr_t)PERIPHERAL_REG_BASE(per, nbr, post) + sizeof(NRF_##per##_Type))

#if defined(DPPI_PRESENT)
#define IF_PER(per, nbr, post, lname)                            \
  if (IS_PERIPHERAL_REG(task_reg, per, nbr, post)) {             \
    *p_reg = PERIPHERAL_REG_BASE(per, nbr, post);                \
    *task = (intptr_t)task_reg - (intptr_t)*p_reg;               \
    *set_f = (subscribe_set_f)nrf_##lname##_subscribe_set;       \
    *clear_f = (subscribe_clear_f)nrf_##lname##_subscribe_clear; \
    *trigger_f = (task_trigger_f)nrf_##lname##_task_trigger;\
    return;

/* Variant for peripherals whose tasks fire only via DPPI subscription (e.g.
 * PPIB, whose TASKS_SEND[n] has no corresponding software-trigger HAL function
 * in nrfx).
 */
#define IF_PER_SUBSCRIBE_ONLY(per, nbr, post, lname)             \
  if (IS_PERIPHERAL_REG(task_reg, per, nbr, post)) {             \
    *p_reg = PERIPHERAL_REG_BASE(per, nbr, post);                \
    *task = (intptr_t)task_reg - (intptr_t)*p_reg;               \
    *set_f = (subscribe_set_f)nrf_##lname##_subscribe_set;       \
    *clear_f = (subscribe_clear_f)nrf_##lname##_subscribe_clear; \
    *trigger_f = NULL;                                           \
    return;
#else
#define IF_PER(per, nbr, post, lname)                            \
  if (IS_PERIPHERAL_REG(task_reg, per, nbr, post)) {             \
    *p_reg = PERIPHERAL_REG_BASE(per, nbr, post);                \
    *task = (intptr_t)task_reg - (intptr_t)*p_reg;               \
    *set_f = NULL;                                               \
    *clear_f = NULL;                                             \
    *trigger_f = (task_trigger_f)nrf_##lname##_task_trigger;\
    return;
#endif

#define NOT_KNOWN_TASK_ERROR \
  bs_trace_error_time_line("Tried to look for a task register not known to these HW models\n")

typedef void (*subscribe_set_f)(void *, int , uint8_t);
typedef void (*subscribe_clear_f)(void *, int);
typedef void (*task_trigger_f)(void *, int);

/*
 * Given a pointer to a task or event register in an unknown peripheral
 * Get
 *  **p_reg: A pointer to the register base address
 *  *set_f/clear_f: pointers to the HAL subscribe set and clear functions that correspond to that peripheral
 *  *trigger_f: pointer to the HAL task trigger function corresponding to that peripheral
 * The task/event *offset* corresponding to that subscribe register (input to most nrf HAL functions)
 */
void nrf_hack_get_task_from_ptr(void *task_reg,
                                void **p_reg,
                                subscribe_set_f *set_f,
                                subscribe_clear_f *clear_f,
                                task_trigger_f *trigger_f,
                                int *task);
