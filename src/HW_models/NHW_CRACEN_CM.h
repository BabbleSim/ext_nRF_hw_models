/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_CRACEN_CM_H
#define _NRF_HW_MODEL_CRACEN_CM_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"{
#endif

void nhw_CRACEN_CM_regw_sideeffects_CONFIG(void);
void nhw_CRACEN_CM_regw_sideeffects_START(void);

void nhw_CRACEN_CM_regw_sideeffects_INTEN(unsigned int inst);
void nhw_CRACEN_CM_regw_sideeffects_INTENSET(unsigned int inst);
void nhw_CRACEN_CM_regw_sideeffects_INTENCLR(unsigned int inst);

void nhw_CRACEN_CM_regw_sideeffects_INTSTARTCLR(unsigned int inst);

//Interface towards CRACEN wrap
void nhw_CRACEN_CM_init(void);
void nhw_CRACEN_CM_timer_triggered(void);

//Interface towards the CM crypto engines
void nhw_CRACEN_CM_update_timer(void);
void nhw_CRACEN_CM_give_pusher_data(char *data, size_t len);
void nhw_CRACEN_CM_fetcher_feed(void);

struct CM_tag {
  uint8_t EngineSelect:4;
  uint8_t DataOrConf:1;
  uint8_t Last:1;
  uint8_t DataType:2;
  union {
     uint8_t Invalid_bytes:6;
     uint8_t OffsetStartAddr;
  };
} __attribute__ ((packed));

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_CRACEN_CM_H */
