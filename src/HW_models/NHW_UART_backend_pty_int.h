/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_UART_BACKEND_PTY_INT_H
#define _NRF_HW_MODEL_UART_BACKEND_PTY_INT_H

#ifdef __cplusplus
extern "C"{
#endif

int nhw_upty_open_ptty(const char *uart_name, const char *auto_attach_cmd,
                       bool do_auto_attach, bool wait_pts);
void nhw_upty_wait_for_pty(int fd, uint64_t microsec);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_UART_BACKEND_PTY_INT_H */
