/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note: This header is private to the RADIO HW model
 */
#ifndef _NRF_RADIO_TIMINGS_H
#define _NRF_RADIO_TIMINGS_H

#include <stdint.h>
#include <stdbool.h>
#include "bs_types.h"
#include "NHW_RADIO_priv.h"

#ifdef __cplusplus
extern "C"{
#endif

enum nhwra_tim_condition {NHWRA_NONE, NHWRA_FROM_HW_TIFS, NHWRA_FROM_PLL};

void nrfra_timings_init(void);
bs_time_t nhwra_timings_get_rampup_time(bool TxNotRx, enum nhwra_tim_condition cond);
bs_time_t nhwra_timings_get_Rx_chain_delay(void);
bs_time_t nhwra_timings_get_RX_rampdown_time(void);
bs_time_t nhwra_timings_get_TX_rampdown_time(void);
bs_time_t nhwra_timings_get_TX_chain_delay(void);
bs_time_t nhwra_timings_get_PLL_settle_time(nrfra_state_t radio_state, bool freq_change);

#ifdef __cplusplus
}
#endif

#endif
