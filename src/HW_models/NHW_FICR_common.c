/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Command line options to set some FICR registers instead of their default values:
 *   -ficr_deviceaddr=XX:XX:XX:XX:XX:XX  DEVICEADDR (48 bits, hex, MSB first)
 *   -ficr_deviceaddrtype_random         DEVICEADDRTYPE = random (1)
 *
 * Without these options the FICR models keep their defaults
 * (random DEVICEADDR, public DEVICEADDRTYPE).
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "bs_tracing.h"
#include "bs_cmd_line.h"
#include "nsi_tasks.h"
#include "weak_stubs.h"
#include "NHW_FICR_common.h"

static struct {
  bool deviceaddr_set;
  bool deviceaddrtype_random;
  uint64_t deviceaddr; /* 48 bits */
} ficr_args;

static void arg_deviceaddr_found(char *argv, int offset) {
  const char *str = &argv[offset];
  unsigned int b[6];
  char extra;

  if (sscanf(str, "%2x:%2x:%2x:%2x:%2x:%2x%c",
             &b[5], &b[4], &b[3], &b[2], &b[1], &b[0], &extra) != 6) {
    bs_trace_error_line("-ficr_deviceaddr: expected XX:XX:XX:XX:XX:XX (hex, MSB first), got \"%s\"\n",
                        str);
  }

  ficr_args.deviceaddr = 0;
  for (int i = 5; i >= 0; i--) {
    if (b[i] > 0xFF) {
      bs_trace_error_line("-ficr_deviceaddr: invalid byte in \"%s\"\n", str);
    }
    ficr_args.deviceaddr = (ficr_args.deviceaddr << 8) | b[i];
  }
  ficr_args.deviceaddr_set = true;
}

static void nhw_ficr_register_cmd_args(void) {
  static bs_args_struct_t args_struct_toadd[] = {
    {
      .option = "ficr_deviceaddr",
      .name = "XX:XX:XX:XX:XX:XX",
      .type = 's',
      .call_when_found = arg_deviceaddr_found,
      .descript = "Set the FICR DEVICEADDR (48 bits, hex, MSB first). By default it is random. "
                  "Note some controllers, like Zephyr's open source one will only use DEVICEADDR if DEVICEADDRTYPE=1 (random) "
                  "(see -ficr_deviceaddrtype_random)"
    },
    {
      .is_switch = true,
      .option = "ficr_deviceaddrtype_random",
      .type = 'b',
      .dest = (void *)&ficr_args.deviceaddrtype_random,
      .descript = "Set the FICR DEVICEADDRTYPE to random (1). By default it is public (0)"
    },
    ARG_TABLE_ENDMARKER
  };

  bs_add_extra_dynargs(args_struct_toadd);
}

NSI_TASK(nhw_ficr_register_cmd_args, PRE_BOOT_1, 100);

void nhw_ficr_apply_addr_args(uint32_t deviceaddr[2], uint32_t *deviceaddrtype) {
  if (ficr_args.deviceaddr_set) {
    deviceaddr[0] = (uint32_t)ficr_args.deviceaddr;
    /* Only the 16 LSbits of DEVICEADDR[1] are part of the address, the rest is left erased */
    deviceaddr[1] = 0xFFFF0000 | (uint32_t)(ficr_args.deviceaddr >> 32);
  }
  if (ficr_args.deviceaddrtype_random) {
    *deviceaddrtype = 1;
  }
}
