/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * CRACEN - Cryptographic accelerator engine
 *
 * This file contains the wrapping logic, for the underlying crypto IPs
 *
 * That includes the events and interrupt logic and registers
 *
 * Note:
 *  * Only the RNG IP is modeled at this point, so:
 *    * SEED is unused at this point and SEEDVALIND & SEEDLOCK ignored
 *    * PROTECTEDRAMLOCK is ignored
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
#include "irq_ctrl.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

NRF_CRACEN_Type NRF_CRACEN_regs;
NRF_CRACENCORE_Type NRF_CRACENCORE_regs;

static struct cracen_wrap_status {
  bool CRYPTOMASTER_int_line;
  bool RNG_int_line;
  bool PKEIKG_int_line;
} cracen_w_st;

/**
 * Initialize the RNG model
 */
static void nhw_CRACEN_init(void) {
  memset(&NRF_CRACEN_regs, 0, sizeof(NRF_CRACEN_regs));
  memset(&NRF_CRACENCORE_regs, 0, sizeof(NRF_CRACENCORE_regs));
  memset(&cracen_w_st, 0, sizeof(cracen_w_st));

  nhw_CRACEN_RNG_init();
}

NSI_TASK(nhw_CRACEN_init, HW_INIT, 100);

static void nhw_CRACEN_eval_interrupt(uint inst) {
  static bool CRACEN_int_line[NHW_CRACEN_TOTAL_INST]; /* Is the CRACEN currently driving its interrupt line high */
  /* Mapping of peripheral instance to {int controller instance, int number} */
  static struct nhw_irq_mapping nhw_cracen_irq_map[NHW_CRACEN_TOTAL_INST] = NHW_CRACEN_INT_MAP;
  bool new_int_line = false;

  NHW_CHECK_INTERRUPT_si(CRACEN, CRYPTOMASTER, NRF_CRACEN_regs.INTEN)
  NHW_CHECK_INTERRUPT_si(CRACEN, RNG, NRF_CRACEN_regs.INTEN)
  NHW_CHECK_INTERRUPT_si(CRACEN, PKEIKG, NRF_CRACEN_regs.INTEN)

  hw_irq_ctrl_toggle_level_irq_line_if(&CRACEN_int_line[inst],
                                       new_int_line,
                                       &nhw_cracen_irq_map[inst]);
}

#define NHW_CRACEN_SIGNAL_EVENT(event) \
  void nhw_CRACEN_signal_EVENTS_##event(void) \
  { \
    NRF_CRACEN_regs.EVENTS_##event = 1; \
    nhw_CRACEN_eval_interrupt(0); \
  }

#define NHW_CRACEN_TOGGLE_INTLINE(event) \
  void nhw_CRACEN_toggle_##event##_intline(bool level) { \
    if (level == cracen_w_st. event##_int_line){ \
      return; \
    } \
    cracen_w_st. event##_int_line = level; \
    if (level) { \
      nhw_CRACEN_signal_EVENTS_##event(); \
    } \
  }

#define NHW_CRACEN_REGW_SIDEEFFECTS_EVENT(event) \
  void nhw_CRACEN_regw_sideeffects_EVENTS_##event(void) { \
    if (cracen_w_st. event##_int_line) { \
      NRF_CRACEN_regs.EVENTS_##event = 1; \
    } \
    nhw_CRACEN_eval_interrupt(0); \
  }

#define NHW_CRACEN_EVENT_LOGIC(event) \
  NHW_CRACEN_SIGNAL_EVENT(event) \
  NHW_CRACEN_TOGGLE_INTLINE(event) \
  NHW_CRACEN_REGW_SIDEEFFECTS_EVENT(event) \

NHW_CRACEN_EVENT_LOGIC(CRYPTOMASTER)
NHW_CRACEN_EVENT_LOGIC(RNG)
NHW_CRACEN_EVENT_LOGIC(PKEIKG)

NHW_SIDEEFFECTS_INTEN(CRACEN, NRF_CRACEN_regs., NRF_CRACEN_regs.INTEN)
NHW_SIDEEFFECTS_INTSET(CRACEN, NRF_CRACEN_regs., NRF_CRACEN_regs.INTEN)
NHW_SIDEEFFECTS_INTCLR(CRACEN, NRF_CRACEN_regs., NRF_CRACEN_regs.INTEN)
