/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "nrfx.h"
#include "nrf_hack_int.h"

/*
 * Given a peripheral task/event (task_event) value (A value of a nrf_<peri>_task_t or nrf_<peri>_event_t)
 * return true if it is a task, or false if it is an event
 */
static bool nrf_hack_is_task(int task_event) {
  /*
   * In the NRF peripherals:
   *  [0x000..0x080) are tasks
   *  [0x080..0x100) are subscribe registers
   *  [0x100..0x180) are events
   *  [0x180..0x200) are publish registers
   *  Note: For the nrf54 RADIO, each group is twice as big, but tasks are still < 0x100
   */
  if (task_event < 0x100) {
    return true;
  } else {
    return false;
  }
}

#if defined(DPPI_PRESENT)
/*
 * Given a *<subscribe>* register (NOT a task register),
 * set it to be enabled and connected to <channel>, by calling the appropriate
 * HAL set replacement function.
 *
 * This function provides the backend for the NRF_DPPI_ENDPOINT_SETUP HAL macro
 * for simulation.
 *
 * Note: This function also accepts publish registers, but ignores them
 * (so as to be callable from NRF_DPPI_ENDPOINT_SETUP() without any check)
 */
void nrf_dppi_hack_subscribe_set(void *sub_reg, unsigned int channel)
{
  void *p_reg;
  int task;
  subscribe_set_f set_f;
  subscribe_clear_f clear_f;
  task_trigger_f task_f;

  intptr_t task_reg_pr = (intptr_t)sub_reg - NRF_SUBSCRIBE_PUBLISH_OFFSET((uintptr_t)sub_reg);

  nrf_hack_get_task_from_ptr((void *)task_reg_pr, &p_reg, &set_f, &clear_f, &task_f, &task);
  if (nrf_hack_is_task(task)) {
    set_f(p_reg, task, channel);
  }
}

/*
 * Given a *<subscribe>* register (NOT a task register),
 * clear it/set it to be disabled, by calling the appropriate
 * HAL clear replacement function.
 *
 * This function provides the backend for the NRF_DPPI_ENDPOINT_CLEAR HAL macro
 * for simulation.
 *
 * Note: This function also accepts publish registers, but ignores them
 * (so as to be callable from NRF_DPPI_ENDPOINT_SETUP() without any check)
 */
void nrf_dppi_hack_subscribe_clear(void *sub_reg)
{
  void *p_reg;
  int task;
  subscribe_set_f set_f;
  subscribe_clear_f clear_f;
  task_trigger_f trigger_f;

  intptr_t task_reg_pr = (intptr_t)sub_reg - NRF_SUBSCRIBE_PUBLISH_OFFSET((uintptr_t)sub_reg);

  nrf_hack_get_task_from_ptr((void *)task_reg_pr, &p_reg, &set_f, &clear_f, &trigger_f, &task);
  if (nrf_hack_is_task(task)) {
    clear_f(p_reg, task);
  }
}

#endif /* DPPI_PRESENT */

/*
 * Given a task register address for an unspecified peripheral,
 * trigger the corresponding task in the hardware model.
 */
void nrf_hack_trigger_task_address(void *task_reg)
{
  void *p_reg;
  int task;
  subscribe_set_f set_f;
  subscribe_clear_f clear_f;
  task_trigger_f trigger_f;

  nrf_hack_get_task_from_ptr(task_reg, &p_reg, &set_f, &clear_f, &trigger_f, &task);

  if (nrf_hack_is_task(task)) {
    if (trigger_f != NULL) {
      trigger_f(p_reg, task);
    } else {
      /* Reached when application code attempts to software-trigger a task on a
       * peripheral whose tasks can only fire via DPPI subscription (e.g. PPIB).
       */
      bs_trace_error_time_line(
        "%s: peripheral has no software-trigger HAL function (subscribe-only)\n",
        __func__);
    }
  }
}
