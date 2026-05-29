/*
 * Copyright (c) 2019 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "nrfx_common_int.h"

IRQn_Type nrfx_get_irq_number(void const * p_reg){
  /* 28..63 :not real */
  if (IS_PERIPHERAL_REG(p_reg, AAR, 00, )) {
    return AAR00_CCM00_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, CCM, 00, )) {
    return AAR00_CCM00_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, ECB, 00, )) {
    return ECB00_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RRAMC, , )) {
    return RRAMC_IRQn;
  /* CTRLAP */
  /* CM33S */
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 00, )) {
    return TIMER00_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 00, )) {
    return EGU00_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, CRACEN, , )) {
    return CRACEN_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 10, )) {
    return TIMER10_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 10, )) {
    return EGU10_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RADIO, , )) {
    return RADIO_0_IRQn;
    /* Note RADIO has 2 interrupts */
  } else if (IS_PERIPHERAL_REG(p_reg, UARTE, 20, )) {
    return SERIAL20_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, UARTE, 21, )) {
    return SERIAL21_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, UARTE, 22, )) {
    return SERIAL22_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 20, )) {
    return EGU20_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 20, )) {
    return TIMER20_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, PWM, 20, )) {
    return PWM20_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, SAADC, , )) {
    return SAADC_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TEMP, , )) {
    return TEMP_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, GPIOTE, 20, )) {
    return GPIOTE20_0_IRQn;
    /* Note GPIO20 has two interrupts */
  } else if (IS_PERIPHERAL_REG(p_reg, QDEC, 20, )) {
    return QDEC20_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, GRTC, , )) {
    return GRTC_0_IRQn;
    /* Note GRTC has 4 interrupts */
  } else if (IS_PERIPHERAL_REG(p_reg, TAMPC, , )) {
    return TAMPC_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, WDT, 30, )) {
    return WDT30_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, GPIOTE, 30, )) {
    return GPIOTE30_0_IRQn;
    /* Note GPIO20 has two interrupts */
  } else if (IS_PERIPHERAL_REG(p_reg, CLOCK, , )) {
    return CLOCK_POWER_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, POWER, , )) {
    return CLOCK_POWER_IRQn;
  } else {
    bs_trace_error_time_line("Tried to get the peripheral number of an address unknown to these HW models\n");
    return 0; /* unreachable */
  }
}
