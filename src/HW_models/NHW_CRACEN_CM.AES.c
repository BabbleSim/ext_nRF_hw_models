/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * CRACEN CryptoMaster, AES engine model
 *
 * Notes:
 *  * Only ECB mode is supported by now
 *
 *  * Only SW programmed key is supported by now
 *
 *  * This AES model does not bother clearing the AES Keys if they are incorrectly programmed
 *    or between runs
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "bs_tracing.h"
#include "NHW_config.h"
#include "NHW_CRACEN_CM.h"
#include "BLECrypt_if.h"
#include "nsi_hws_models_if.h"

#define CONFIG_ENCORDEC_MASK 0x1
#define CONFIG_MODE_MASK 0x1FF00
#define BLOCK_SIZE 128/8

static struct CM_AES_regs_t {
  uint32_t CONFIG;
  uint32_t reserved;
  uint32_t KEY[8];
  uint32_t IV[4];
  uint32_t IV2[4];
  uint32_t KEY2[8];
  uint32_t MASK;
} CM_AES_regs;

static int t_ecb[] = NHW_CRACEN_CM_AES_t_ECB;

static struct CM_AES_st_t {
  int AES_KEY_size;
  char data_out[BLOCK_SIZE];
} CM_AES_st;

bs_time_t Timer_CRACEN_CM_AES;

void nhw_CRACEN_CM_AES_init(void) {
  memset(&CM_AES_regs, 0, sizeof(CM_AES_regs));
  memset(&CM_AES_st, 0, sizeof(CM_AES_st));

  Timer_CRACEN_CM_AES = TIME_NEVER;
}

static int nhw_CRACEN_CM_AES_get_mode(void) {
  return (CM_AES_regs.CONFIG & CONFIG_MODE_MASK) >> 8;
}

static int nhw_CRACEN_CM_AES_get_KeySel(void) {
  return ((CM_AES_regs.CONFIG >> 6) & 0x3) | ((CM_AES_regs.CONFIG >> 26) & 0x7);;
}

static void nhw_CRACEN_CM_AES_process_data(struct CM_tag *tag_st, char* buf, size_t len) {
  (void)tag_st;
  if (nhw_CRACEN_CM_AES_get_mode() != 1) {
    bs_trace_error_time_line("%s: Only ECB mode supported by now\n",
                             __func__);
  }
  if (nhw_CRACEN_CM_AES_get_KeySel() != 0) {
    bs_trace_error_time_line("%s: Only SW programmed key supported by now\n",
                             __func__);
  }
  if (len != BLOCK_SIZE) {
    bs_trace_error_time_line("Block size of 128b excedded for payload\n");
  }
  BLECrypt_if_aes_ecb((uint8_t *)CM_AES_regs.KEY, CM_AES_st.AES_KEY_size,
                      (uint8_t *)buf, (uint8_t *)CM_AES_st.data_out);

  Timer_CRACEN_CM_AES = nsi_hws_get_time() + t_ecb[(CM_AES_st.AES_KEY_size - 128)/64];
  nhw_CRACEN_CM_update_timer();
}

bool nhw_CRACEN_CM_AES_feed_data(struct CM_tag *tag_st, char* buf, size_t len) {

  if (tag_st->DataOrConf == 1) { // Config
    int OffsetStartAddr = tag_st->OffsetStartAddr;
    if (OffsetStartAddr + len > sizeof(CM_AES_regs)) {
      bs_trace_error_time_line("%s: Attempted to write into non existent CONFIG register (%i..%i)\n",
                               __func__, OffsetStartAddr, OffsetStartAddr + len);
    }
    memcpy(&((char *)&CM_AES_regs)[OffsetStartAddr], buf, len);
    if (OffsetStartAddr == offsetof(struct CM_AES_regs_t , KEY)) {
      if ((len*8 != 128) && (len*8 != 192) && (len*8 != 256)) {
        bs_trace_warning_time_line("%s: Invalid key length (%i)\n", __func__, len*8);
      } else {
        CM_AES_st.AES_KEY_size = len*8;
      }
    }
    return false;
  } else { // Data
    if (tag_st->DataType == 0) { // Payload
      if (tag_st->Invalid_bytes != 0) {
        bs_trace_error_time_line("%s: Invalid_bytes != 0 not supported\n", __func__);
      }
      nhw_CRACEN_CM_AES_process_data(tag_st, buf, len);
    } else { // Header
      bs_trace_error_time_line("Only ECB mode implemented by now => DataType %i not supported\n",
                               tag_st->DataType);
    }
    return false;
  }
}

void nhw_CRACEN_CM_AES_timer_triggered(void) {
  Timer_CRACEN_CM_AES = TIME_NEVER;
  nhw_CRACEN_CM_update_timer();
  nhw_CRACEN_CM_give_pusher_data(CM_AES_st.data_out, BLOCK_SIZE);
}

void nhw_CRACEN_CM_AES_hard_stop(void) {
  nhw_CRACEN_CM_AES_init();
  nhw_CRACEN_CM_update_timer();
}
