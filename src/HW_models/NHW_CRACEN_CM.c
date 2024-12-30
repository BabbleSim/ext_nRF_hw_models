/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * CRACEN CryptoMaster
 *
 * This file implements the DMA and wrap logic of the cryptomaster
 * Each cryptomaster engine is implemented in its own file
 *
 * Notes:
 *  * [Note1] Writing to PUSH_ADDR_MSB or FETCH_ADDR_MSB has no effect
 *
 *  * [Note2] The DMA does not model the interrupt back-preasure mechanism described in the spec.
 *    Meaning, it will not hold fetching the next descriptor and updating the addr registers if
 *    the corresponding end of block interrupt status bit is high.
 *
 *  * [Note3] This model assumes the CONFIG.software reset bit is not self clearing though this is not clear from the spec.
 *    STATUS.SoftRst_busy is never set.
 *
 *  * [Note4] The fetcher and pusher DMAs are instantaneous and transfer instantaneously to/from the crypto engines
 *    as soon as the data can be feed to/is provided by the crypto engine.
 *  * [Note4b] For the fetcher
 *    This model fetcher always feeds the crypto engines in full fetcher blocks/one descriptor at a time,
 *    so, for data, they better match the cryptoengines processing size
 *    Data is read and fed in one go, so:
 *    STATUS.{Not empty flag from input FIFO (fetcher)} is never set
 *    STATUS.{Number of data in output FIFO} is also not changed (just 0).
 *
 *    STATUS.{Fetch busy} is set if the cryptoengine applies back pressure while processing data.
 *  * [Note4c] For the pusher:
 *    STATUS.{Pusher busy} and {Pusher Waiting FIFO} are set until the engine has provided enough data.
 *
 *  * [Note7] The descriptors {Constant address}, and Realign, bits are ignored/not supported.
 *    (Realign is not needed as effectively all accesses in the model are properly "aligned"
 *    independently of the size of the previous access)
 *
 *  * [Note8]: Only the AES crypto engine, and bypass, are implemented at this point
 *
 *  * [Note9]: Error interrupts (in both fetcher and pusher) are not generated
 *
 *  * [Note10]: From the spec there is many opens about how the pusher would handle descriptors which are
 *    shorter or longer than the data produced by the engines, or several blocks. The model just copies data
 *    until there is no more left configured in the pusher at which points it raises the Stopped interrupt.
 *    So the amount of produced data better match the pusher configuration.
 *
 *  * [Note11]: During pushes, the model just ignores the tag.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_templates.h"
#include "NHW_CRACEN_CM.h"
#include "NHW_CRACEN_CM.AES.h"
#include "NHW_CRACEN_wrap.h"
#include "bs_tracing.h"

extern NRF_CRACEN_Type NRF_CRACEN_regs;
extern NRF_CRACENCORE_Type NRF_CRACENCORE_regs;

bs_time_t Timer_CRACEN_CM;
extern bs_time_t Timer_CRACEN_CM_AES;

static NRF_CRACENCORE_CRYPTMSTRDMA_Type volatile *CMDMA_regs;

struct CM_descr {
  char *Address;
  union {
    uint32_t Stop:1;
    void *Next;
  };
  struct {
    uint32_t Length:28;
    uint32_t CstAddr:1;
    uint32_t Realign:1;
    uint32_t Discard :1;
    uint32_t IntEn :1;
  };
  struct CM_tag tag;
} __attribute__ ((packed));

struct fetcher_pusher_st {
  bool processing;
  struct CM_tag tag;
  bool Discard;
  bool Stop;
  bool IntEn;
  char *current_address;
  size_t current_len;
  struct CM_descr *next;
};

static struct fetcher_pusher_st CM_pusher_st;
static struct fetcher_pusher_st CM_fetcher_st;

enum NRF_CRACENCORE_CMDMA_Int_t {
  Fetch_EndBlock_int = 1,
  Fetch_Stopped_int = 2,
  Fetch_Error_int = 4,
  Push_EndBlock_int = 8,
  Push_Stopped_int = 0x10,
  Push_Error_int = 0x20,
};

static void nhw_CRACEN_CM_set_int(enum NRF_CRACENCORE_CMDMA_Int_t interrupt);
static void nhw_CRACEN_CM_load_pusher_descr(struct CM_descr *descr);
static void nhw_CRACEN_CM_load_fetcher_descr(struct CM_descr *descr);

static void nhw_CRACEN_CM_soft_reset(void) {
  memset(&CM_pusher_st, 0, sizeof(CM_pusher_st));
  memset(&CM_fetcher_st, 0, sizeof(CM_fetcher_st));
  CMDMA_regs->STATUS = 0;
}

void nhw_CRACEN_CM_init(void) {

  CMDMA_regs = &NRF_CRACENCORE_regs.CRYPTMSTRDMA;

  NRF_CRACENCORE_CRYPTMSTRHW_Type volatile *HWconf_regs;

  HWconf_regs = &NRF_CRACENCORE_regs.CRYPTMSTRHW;
  HWconf_regs->INCLIPSHWCFG = CRACENCORE_CRYPTMSTRHW_INCLIPSHWCFG_ResetValue;
  HWconf_regs->BA411EAESHWCFG1 = CRACENCORE_CRYPTMSTRHW_BA411EAESHWCFG1_ResetValue;
  HWconf_regs->BA411EAESHWCFG2 = CRACENCORE_CRYPTMSTRHW_BA411EAESHWCFG2_ResetValue;
  HWconf_regs->BA413HASHHWCFG = CRACENCORE_CRYPTMSTRHW_BA413HASHHWCFG_ResetValue;
  HWconf_regs->BA418SHA3HWCFG = CRACENCORE_CRYPTMSTRHW_BA418SHA3HWCFG_ResetValue;
  HWconf_regs->BA419SM4HWCFG = CRACENCORE_CRYPTMSTRHW_BA419SM4HWCFG_ResetValue;
  HWconf_regs->BA424ARIAHWCFG = CRACENCORE_CRYPTMSTRHW_BA424ARIAHWCFG_ResetValue;

  Timer_CRACEN_CM = TIME_NEVER;

  nhw_CRACEN_CM_soft_reset();
  nhw_CRACEN_CM_AES_init();
}

static void nhw_CRACEN_CM_eval_interrupt(uint inst) {
  (void) inst;

  static bool int_level = false;

  CMDMA_regs->INTSTAT = CMDMA_regs->INTSTATRAW & CMDMA_regs->INTEN;

  bool new_int_level = (CMDMA_regs->INTSTAT != 0);

  if (new_int_level != int_level) {
    int_level = new_int_level;
    nhw_CRACEN_toggle_CRYPTOMASTER_intline(int_level);
  }
}

/*
 * Interface for the cryptoengine to provide output data to the Cryptomaster pusher
 */
void nhw_CRACEN_CM_give_pusher_data(char *data, size_t len) {
  if (CM_pusher_st.processing == 0) {
    bs_trace_warning_time_line("A CM engine provided data but the pusher was not expecting it => discarded\n");
    return;
  }

  while (len > 0) {
    unsigned int to_copy = BS_MIN(len, CM_pusher_st.current_len);

    if (!CM_pusher_st.Discard) {
      memcpy(CM_pusher_st.current_address, data, to_copy);
    }
    len -= to_copy;
    data += to_copy;
    CM_pusher_st.current_len -= to_copy;
    CM_pusher_st.current_address += to_copy;

    if (CM_pusher_st.current_len == 0) { //End of pusher block
      if (CM_pusher_st.IntEn) {
        nhw_CRACEN_CM_set_int(Push_EndBlock_int);
      }
      if (CM_pusher_st.Stop) { //No more blocks
        nhw_CRACEN_CM_set_int(Push_Stopped_int);
        CM_pusher_st.processing = false;
        CMDMA_regs->STATUS &= ~(CRACENCORE_CRYPTMSTRDMA_STATUS_PUSHBUSY_Msk
                                | CRACENCORE_CRYPTMSTRDMA_STATUS_PUSHWAITINGFIFO_Msk);
        break;
      } else {
        nhw_CRACEN_CM_load_pusher_descr(CM_pusher_st.next);
      }
    }
  }
  if (len > 0) {
    bs_trace_warning_time_line("Crypto engine provided more data than pusher configuration had space for\n");
  }
}

static bool nhw_CRACEN_CM_fetcher_demux(struct CM_tag *tag, char* buf, size_t len) {

  if ((len != 0) && (buf == NULL)) {
    bs_trace_error_time_line("Attempting to access NULL buffer\n");
  }
  switch (tag->EngineSelect) {
    case 0x0: //Bypass
      nhw_CRACEN_CM_give_pusher_data(buf, len);
      return false;
      break;
    case 0x1:
      return nhw_CRACEN_CM_AES_feed_data(tag, buf, len);
      break;
    default:
      bs_trace_error_time_line("Engine %i not yet supported\n", tag->EngineSelect);
      break;
  }
  return true;
}

/*
 * Interface for the cryptoengines to request new data/indicate that is ready for more data
 * from the cryptomaster fetcher
 */
void nhw_CRACEN_CM_fetcher_feed(void) {
  bool hold = false;

  while ((CM_fetcher_st.processing == true) && (hold == false)) {

    hold = nhw_CRACEN_CM_fetcher_demux(&CM_fetcher_st.tag, CM_fetcher_st.current_address, CM_fetcher_st.current_len);

    if (CM_fetcher_st.IntEn) {
      nhw_CRACEN_CM_set_int(Fetch_EndBlock_int);
    }
    if (CM_fetcher_st.Stop) {
      nhw_CRACEN_CM_set_int(Fetch_Stopped_int);
      CM_fetcher_st.processing = false;
      CMDMA_regs->STATUS &= ~CRACENCORE_CRYPTMSTRDMA_STATUS_FETCHBUSY_Msk;
      break;
    } else {
      nhw_CRACEN_CM_load_fetcher_descr(CM_fetcher_st.next);
    }
  }
}

static void nhw_CRACEN_CM_load_descr(struct fetcher_pusher_st *st, struct CM_descr *descr) {
  if (descr == NULL) {
    bs_trace_error_time_line("Descriptor in address 0\n");
  }
  st->tag = descr->tag;
  st->Discard = descr->Discard;
  st->current_len = descr->Length;
  st->current_address = descr->Address;
  st->Stop = descr->Stop;
  st->IntEn = descr->IntEn;
  st->next = (struct CM_descr *)((uintptr_t)descr->Next & ~0x3);
}

static void nhw_CRACEN_CM_load_pusher_descr(struct CM_descr *descr) {
  nhw_CRACEN_CM_load_descr(&CM_pusher_st, descr);

  CMDMA_regs->PUSHADDRLSB = (uintptr_t)descr;
  CMDMA_regs->PUSHADDRMSB = (uintptr_t)descr;
}

static void nhw_CRACEN_CM_load_fetcher_descr(struct CM_descr *descr) {
  nhw_CRACEN_CM_load_descr(&CM_fetcher_st, descr);

  CMDMA_regs->FETCHADDRLSB = (uintptr_t)descr;
  CMDMA_regs->FETCHADDRMSB = (uintptr_t)descr;
}

static void nhw_CRACEN_CM_start(int start) {
  if (start & 0x2) { //Pusher start
    CM_pusher_st.processing = true;
    if ((CMDMA_regs->CONFIG & CRACENCORE_CRYPTMSTRDMA_CONFIG_PUSHCTRLINDIRECT_Msk) == 0) { //Direct mode
      CM_pusher_st.Discard = (CMDMA_regs->PUSHLEN >> CRACENCORE_CRYPTMSTRDMA_PUSHLEN_PUSHDISCARD_Pos) & 0x1;
      CM_pusher_st.current_len = CMDMA_regs->PUSHLEN & CRACENCORE_CRYPTMSTRDMA_PUSHLEN_PUSHLEN_Msk;
      CM_pusher_st.current_address = (char *)CMDMA_regs->PUSHADDRLSB;
      CM_pusher_st.Stop = true;
      CM_pusher_st.IntEn = false;
      CM_pusher_st.next = NULL;
      memset(&CM_pusher_st.tag, 0 , sizeof(CM_pusher_st.tag));
    } else { //Scatter mode
      nhw_CRACEN_CM_load_pusher_descr((struct CM_descr *)CMDMA_regs->PUSHADDRLSB);
    }
    CMDMA_regs->STATUS |= CRACENCORE_CRYPTMSTRDMA_STATUS_PUSHBUSY_Msk
                        | CRACENCORE_CRYPTMSTRDMA_STATUS_PUSHWAITINGFIFO_Msk;

  }
  if (start & 0x1) { //Fetcher start
    CM_fetcher_st.processing = true;
    if ((CMDMA_regs->CONFIG & CRACENCORE_CRYPTMSTRDMA_CONFIG_FETCHCTRLINDIRECT_Msk) == 0) { //Direct mode
      CM_fetcher_st.Discard = (CMDMA_regs->FETCHLEN >> CRACENCORE_CRYPTMSTRDMA_PUSHLEN_PUSHDISCARD_Pos) & 0x1;
      CM_fetcher_st.current_len = CMDMA_regs->FETCHLEN & CRACENCORE_CRYPTMSTRDMA_FETCHLEN_FETCHLEN_Msk;
      CM_fetcher_st.current_address = (char *)CMDMA_regs->FETCHADDRLSB;
      CM_fetcher_st.Stop = true;
      CM_fetcher_st.IntEn = false;
      CM_fetcher_st.next = NULL;
      memcpy(&CM_fetcher_st.tag, (char *)&CMDMA_regs->FETCHTAG , sizeof(uint32_t));
    } else {
      nhw_CRACEN_CM_load_fetcher_descr((struct CM_descr *)CMDMA_regs->FETCHADDRLSB);
    }
    CMDMA_regs->STATUS |= CRACENCORE_CRYPTMSTRDMA_STATUS_FETCHBUSY_Msk;
    nhw_CRACEN_CM_fetcher_feed();
  }
}

void nhw_CRACEN_CM_regw_sideeffects_CONFIG(void) {
  if (CMDMA_regs->CONFIG & CRACENCORE_CRYPTMSTRDMA_CONFIG_SOFTRST_Msk) {
    nhw_CRACEN_CM_soft_reset();
    nhw_CRACEN_CM_AES_hard_stop();
  }
  if (CMDMA_regs->CONFIG & CRACENCORE_CRYPTMSTRDMA_CONFIG_PUSHSTOP_Msk) {
    CM_pusher_st.Stop = 1;
  }
  if (CMDMA_regs->CONFIG & CRACENCORE_CRYPTMSTRDMA_CONFIG_FETCHSTOP_Msk) {
    CM_fetcher_st.Stop = 1;
  }
}

void nhw_CRACEN_CM_regw_sideeffects_START(void) {
  if (CMDMA_regs->START) {
    if ((NRF_CRACEN_regs.ENABLE & CRACEN_ENABLE_CRYPTOMASTER_Msk) == 0) {
      bs_trace_warning_time_line("Attempting to enable CRACEN Cryptomaster while the CRACEN wrap logic is off\n");
    }

    nhw_CRACEN_CM_start(CMDMA_regs->START);
    CMDMA_regs->START = 0;
  }
}

void nhw_CRACEN_CM_regw_sideeffects_INTSTARTCLR(uint inst) {
  if (CMDMA_regs->INTSTATCLR) {
    CMDMA_regs->INTSTATRAW &= ~CMDMA_regs->INTSTATCLR;
    CMDMA_regs->INTSTATCLR = 0;
    nhw_CRACEN_CM_eval_interrupt(inst);
  }
}

NHW_SIDEEFFECTS_INTEN(CRACEN_CM, CMDMA_regs->, CMDMA_regs->INTEN)
NHW_SIDEEFFECTS_INTSET(CRACEN_CM, CMDMA_regs->, CMDMA_regs->INTEN)
NHW_SIDEEFFECTS_INTCLR(CRACEN_CM, CMDMA_regs->, CMDMA_regs->INTEN)

static void nhw_CRACEN_CM_set_int(enum NRF_CRACENCORE_CMDMA_Int_t interrupt) {
  CMDMA_regs->INTSTATRAW |= interrupt;
  nhw_CRACEN_CM_eval_interrupt(0);
}

void nhw_CRACEN_CM_update_timer(void) {
  if (Timer_CRACEN_CM_AES != Timer_CRACEN_CM) {
    Timer_CRACEN_CM = Timer_CRACEN_CM_AES;
    nhw_CRACEN_update_timer();
  }
}

void nhw_CRACEN_CM_timer_triggered(void) {
  if (Timer_CRACEN_CM == Timer_CRACEN_CM_AES) {
    nhw_CRACEN_CM_AES_timer_triggered();
  }
}
