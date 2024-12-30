/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_CRACEN_CM_AES_H
#define _NRF_HW_MODEL_CRACEN_CM_AES_H

#include <stdint.h>
#include "NHW_CRACEN_CM.h"

#ifdef __cplusplus
extern "C"{
#endif

void nhw_CRACEN_CM_AES_init(void);
bool nhw_CRACEN_CM_AES_feed_data(struct CM_tag *tag_st, char* buf, size_t len);
void nhw_CRACEN_CM_AES_hard_stop(void);
void nhw_CRACEN_CM_AES_timer_triggered(void);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_CRACEN_CM_AES_H */
