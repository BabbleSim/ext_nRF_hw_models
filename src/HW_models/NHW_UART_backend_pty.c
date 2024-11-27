/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Backend for the UART(E) which connects to a pseudoterminal
 * It can be used to drive and monitor the UART interactively
 *
 * Note that using this you lose the simulation determinism you would have otherwise
 *
 * This backend sends characters directly to the PTY and checks it for new received characters
 * periodically
 * While receiving, it will receive at max the configured UART baudrate.
 *
 * The other side CTS is always assumed enabled(clear to send)
 *
 * When the (own) RTS pin is raised (not ready to receive), and if the command line
 * respect_RTS is set, input data in the PTY will be held until RTS is lowered.
 * Otherwise, data will be fed right as soon as it is polled.
 */

#include <unistd.h>
#include "bs_types.h"
#include "bs_tracing.h"
#include "bs_oswrap.h"
#include "bs_cmd_line.h"
#include "bs_dynargs.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_UART_backend_if.h"
#include "NHW_UART_backend_pty_int.h"
#include "nsi_hw_scheduler.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

static bs_time_t Timer_UPTY = TIME_NEVER;

#define DEFAULT_CMD "xterm -e screen %s &"

static bool wait_for_pty;
static bs_time_t poll_period = 50000;

struct upty_st_t {
  bool enabled;
  bool auto_attach;
  char *attach_cmd;
  bool respect_RTS;

  int out_fd; /* File descriptor used for output */
  int in_fd; /* File descriptor used for input */

  bs_time_t Rx_timer;

  bool rx_on;
  bool RTS;

  bool pty_connected;
} upty_st[NHW_UARTE_TOTAL_INST];

static void nhw_upty_tx_byte(uint inst, uint16_t data);
static void nhw_upty_RTS_pin_toggle(uint inst, bool new_level);
static void nhw_upty_enable_notify(uint inst, uint8_t tx_enabled, uint8_t rx_enabled);
static void nhw_upty_update_timer(void);

static void nhw_upty_init(void) {
  char *uart_names[NHW_UARTE_TOTAL_INST] = NHW_UARTE_NAMES;

  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {
    struct upty_st_t *u_el = &upty_st[i];

    u_el->out_fd = -1;
    u_el->in_fd = -1;
    u_el->Rx_timer = TIME_NEVER;

    if (u_el->attach_cmd != NULL) {
      u_el->auto_attach = true;
    }
    if (u_el->auto_attach) {
      u_el->enabled = true;
    }
    if (!u_el->enabled) {
      continue;
    }
    if (u_el->attach_cmd == NULL) {
      u_el->attach_cmd = DEFAULT_CMD;
    }

    //Connect to pty
    char uart_name[50];
    snprintf(uart_name, 50, "UART %i (%s)", i, uart_names[i]);
    int pty_fd = nhw_upty_open_ptty(uart_name, u_el->attach_cmd, u_el->auto_attach, wait_for_pty);
    u_el->in_fd = pty_fd;
    u_el->out_fd = pty_fd;

    struct backend_if st;
    st.tx_byte_f = nhw_upty_tx_byte;
    st.RTS_pin_toggle_f = nhw_upty_RTS_pin_toggle;
    st.uart_enable_notify_f = nhw_upty_enable_notify;
    nhw_UARTE_backend_register(i, &st);
    nhw_UARTE_CTS_lowered(i); //We behave as if the other side was always ready to receive

    if (!u_el->respect_RTS) {
      u_el->Rx_timer = poll_period;
    }
  }
  nhw_upty_update_timer();
}

NSI_TASK(nhw_upty_init, HW_INIT, 100); /* this must be before the uart itself */

static void nhw_upty_tx_byte(uint inst, uint16_t data) {
  struct upty_st_t *u_el = &upty_st[inst];
  int ret;

  if (!u_el->enabled) {
    bs_trace_error_time_line("Programming error\n");
  }

  if (wait_for_pty & (u_el->pty_connected == false)) {
    nhw_upty_wait_for_pty(u_el->out_fd, 100e3);
    u_el->pty_connected = true;
  }

  /* The return value of write() cannot be ignored (there is a warning)
   * but we do not need the return value for anything.
   */
  ret = write(u_el->out_fd, &data, 1);
  (void) ret;
}

static void nhw_upty_RTS_pin_toggle(uint inst, bool new_level) {
  struct upty_st_t *u_el = &upty_st[inst];

  if (!u_el->enabled) {
    bs_trace_error_time_line("Programming error\n");
  }
  if ((u_el->RTS == new_level) || !(u_el->respect_RTS)) {
    return;
  }
  u_el->RTS = new_level;
  if (u_el->RTS) { //Not ready to receive
    u_el->Rx_timer = TIME_NEVER;
  } else {
    u_el->Rx_timer = nsi_hws_get_time() + nhw_uarte_one_byte_time(inst);
  }
  nhw_upty_update_timer();
}

static void nhw_upty_enable_notify(uint inst, uint8_t tx_enabled, uint8_t rx_enabled) {
  (void) tx_enabled;
  struct upty_st_t *u_el = &upty_st[inst];

  if (!u_el->enabled) {
    bs_trace_error_time_line("Programming error\n");
  }
  u_el->rx_on = rx_enabled;
}

static void nhw_upty_update_timer(void) {
  Timer_UPTY = TIME_NEVER;
  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {
    struct upty_st_t * u_el = &upty_st[i];
    if (!u_el->enabled) {
      continue;
    }
    Timer_UPTY = BS_MIN(u_el->Rx_timer, Timer_UPTY);
  }
  nsi_hws_find_next_event();
}

static void nhw_upty_check_for_input(uint inst, struct upty_st_t *u_el) {
  unsigned char byte;
  int ret;

  if (wait_for_pty & (u_el->pty_connected == false)) {
    nhw_upty_wait_for_pty(u_el->in_fd, 100e3);
    u_el->pty_connected = true;
  }

  ret = read(u_el->in_fd, &byte, 1);
  if (ret != -1) {
    if (!u_el->rx_on) {
      bs_trace_info_time(3, "UART%i: Received byte (0x%02X) while Rx is off => ignored\n", inst, byte);
    } else {
      nhw_UARTE_digest_Rx_byte(inst, byte);
    }
    u_el->Rx_timer += nhw_uarte_one_byte_time(inst);
  } else {
    u_el->Rx_timer += poll_period;
  }
}

static void nhw_upty_timer_triggered(void) {
  bs_time_t current_time = Timer_UPTY;

  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {
    struct upty_st_t *u_el = &upty_st[i];
    if (u_el->Rx_timer == current_time) {
      nhw_upty_check_for_input(i, u_el);
    }
  }
  nhw_upty_update_timer();
}

NSI_HW_EVENT(Timer_UPTY, nhw_upty_timer_triggered, 900); /* Let's let as many timers as possible evaluate before this one */

static void nhw_upty_cleanup(void) {
  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {
    struct upty_st_t *u_el = &upty_st[i];

    if (u_el->in_fd != -1) {
      close(u_el->in_fd);
      u_el->in_fd = -1;
    }
    if (u_el->out_fd != -1) {
      close(u_el->out_fd);
      u_el->out_fd = -1;
    }
  }
}

NSI_TASK(nhw_upty_cleanup, ON_EXIT_PRE, 100);

static double poll_period_f;

static void parse_poll_period(char *argv, int offset) {
  (void) offset;
  if (poll_period_f < 1 || poll_period_f > 10e6) {
    bs_trace_error_line("uart_pty_pollT must be set to a value between 1 and 10e6 (%s)\n", argv);
  }
  poll_period = poll_period_f;
}

static void nhw_upty_backend_register_cmdline(void) {
#define OPT_PER_UART 4
  static bs_args_struct_t args[OPT_PER_UART*NHW_UARTE_TOTAL_INST + 1 /* End marker */];
  static char descr_connect[] = "Connect this UART to a pseudoterminal";
  static char descr_auto[] = "Automatically attach to the UART terminal (implies uartx_pty)";
  static char descr_cmd[] = "Command used to automatically attach to the terminal (implies "
                            "uartx_pty_attach), by default: '" DEFAULT_CMD "'";
  static char descr_ignoreRTS[] = "Hold feeding data from the PTY if RTS is high (note: "
                                  "If HW flow control is disabled the UART never lowers RTS)";
#define OPTION_LEN (4 + 2 + 15 + 1)
  static char options[NHW_UARTE_TOTAL_INST][OPT_PER_UART][OPTION_LEN];
  static char opt_cmd[]= "cmd";

  for (int i = 0 ; i < NHW_UARTE_TOTAL_INST; i++) {
    snprintf(options[i][0], OPTION_LEN, "uart%i_pty", i);
    snprintf(options[i][1], OPTION_LEN, "uart%i_pty_attach", i);
    snprintf(options[i][2], OPTION_LEN, "uart%i_pty_attach_cmd", i);
    snprintf(options[i][3], OPTION_LEN, "uart%i_pty_respect_RTS", i);

    args[OPT_PER_UART*i].option = options[i][0];
    args[OPT_PER_UART*i].is_switch = true;
    args[OPT_PER_UART*i].type = 'b';
    args[OPT_PER_UART*i].dest = &upty_st[i].enabled;
    args[OPT_PER_UART*i].descript = descr_connect;

    args[OPT_PER_UART*i + 1].option = options[i][1];
    args[OPT_PER_UART*i + 1].is_switch = true;
    args[OPT_PER_UART*i + 1].type = 'b';
    args[OPT_PER_UART*i + 1].dest = &upty_st[i].auto_attach;
    args[OPT_PER_UART*i + 1].descript = descr_auto;

    args[OPT_PER_UART*i + 2].option = options[i][2];
    args[OPT_PER_UART*i + 2].name = opt_cmd;
    args[OPT_PER_UART*i + 2].type = 's';
    args[OPT_PER_UART*i + 2].dest = &upty_st[i].attach_cmd;
    args[OPT_PER_UART*i + 2].descript = descr_cmd;

    args[OPT_PER_UART*i + 3].option = options[i][3];
    args[OPT_PER_UART*i + 3].is_switch = true;
    args[OPT_PER_UART*i + 3].type = 'b';
    args[OPT_PER_UART*i + 3].dest = &upty_st[i].respect_RTS;
    args[OPT_PER_UART*i + 3].descript = descr_ignoreRTS;
  }

  bs_add_extra_dynargs(args);

  static bs_args_struct_t args_b[] = {
      { .is_switch = true,
          .option = "uart_pty_wait",
          .type = 'b',
          .dest = (void *)&wait_for_pty,
          .descript = "Hold writes to the uart/pts (and therefore the simulation) until a client is connected/ready"
      },
      { .option = "uart_pty_pollT",
          .type = 'd',
          .name = "period",
          .call_when_found = parse_poll_period,
          .dest = (void *)&poll_period_f,
          .descript = "(By default 50e3=50ms) simulated polling period for received bytes from the pseudoterminal"
                      "The smaller this value the higher the overhead"
      },
      ARG_TABLE_ENDMARKER
  };

  bs_add_extra_dynargs(args_b);
}

NSI_TASK(nhw_upty_backend_register_cmdline, PRE_BOOT_1, 200);
