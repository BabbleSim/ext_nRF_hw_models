/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>

bool nrfx_is_in_ram(void const * p_object) {
  (void)p_object;
  return true;
}
