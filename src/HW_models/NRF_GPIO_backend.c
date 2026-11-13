/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * "Backends" for the GPIO pins.
 * Currently there is 3 parts to this:
 *   * Outputs changes can be recorded in a file
 *   * Outputs can be short-circuited to inputs thru a configuration file
 *   * Inputs can be driven from backends
 *
 * Check docs/GPIO.md for more info.
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "bs_cmd_line.h"
#include "bs_dynargs.h"
#include "bs_oswrap.h"
#include "bs_types.h"
#include "bs_tracing.h"
#include "HW_utils.h"
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NRF_GPIO.h"
#include "nsi_hw_scheduler.h"
#include "nsi_tasks.h"

#define MAXLINESIZE 2048
#define MAX_SHORTS 8

static char *gpio_out_file_path; /* Possible file for dumping output toggles */
static char *gpio_conf_file_path; /* Possible file for configuration (short-circuits) */

/* Table keeping all configured short-circuits */
static struct {
  uint8_t port;
  uint8_t pin;
} shorts[NHW_GPIO_TOTAL_INST][NHW_GPIO_MAX_PINS_PER_PORT][MAX_SHORTS];

static FILE *output_file_ptr; /* File pointer for gpio_out_file_path */

static struct backends_st_t {
  const struct nrf_gpio_backend_if *c;
  void *st;
} *backends_st;

static int n_backends;

static void nrf_gpio_load_config(void);
static void nrf_gpio_init_output_file(void);

#define FOR_EACH_BACKEND_call(f, ...) \
  do { \
    if (backends_st) { \
      for (int i = 0; i < n_backends; i++) { \
        if (backends_st[i].c && backends_st[i].c->f) { \
          backends_st[i].c->f(backends_st[i].st, ##__VA_ARGS__); \
        } \
      } \
    } \
  } while(0)

/*
 * Initialize the GPIO backends
 */
void nrf_gpio_backend_init(void)
{
  memset(shorts, UINT8_MAX, sizeof(shorts));

  nrf_gpio_load_config();
  nrf_gpio_init_output_file();

  FOR_EACH_BACKEND_call(init);
}

/*
 * Cleanup before exit
 */
static void nrf_gpio_backend_cleaup(void)
{
  if (output_file_ptr != NULL) {
    fclose(output_file_ptr);
    output_file_ptr = NULL;
  }

  if (backends_st) {
    FOR_EACH_BACKEND_call(cleanup);
    free(backends_st);
    backends_st = NULL;
  }
}

NSI_TASK(nrf_gpio_backend_cleaup, ON_EXIT_PRE, 100);

void nrf_gpio_backend_register(const struct nrf_gpio_backend_if *backend_callbacks, void *st)
{
  int n = n_backends;

  n_backends++;
  backends_st = bs_realloc(backends_st, n_backends*sizeof(struct backends_st_t));
  backends_st[n].c = backend_callbacks;
  backends_st[n].st = st;
}

static void nrf_gpio_register_cmd_args(void) {

  static bs_args_struct_t args_struct_toadd[] = {
      {
          .option="gpio_out_file",
          .name="path",
          .type='s',
          .dest=(void *)&gpio_out_file_path,
          .descript="Optional path to a file where GPIOs output activity will be saved",
      },
      {
          .option="gpio_conf_file",
          .name="path",
          .type='s',
          .dest=(void *)&gpio_conf_file_path,
          .descript="Optional path to a file where the GPIOs configuration will be found.",
      },
      ARG_TABLE_ENDMARKER
  };

  bs_add_extra_dynargs(args_struct_toadd);
}

NSI_TASK(nrf_gpio_register_cmd_args, PRE_BOOT_1, 100);

/*
 * Propagate an output change thru its external short-circuits
 */
void nrf_gpio_backend_short_propagate(unsigned int port, unsigned int n, bool value)
{
  for (int i = 0; i < MAX_SHORTS; i++) {
    if (shorts[port][n][i].port == UINT8_MAX) {
      break;
    }
    nrf_gpio_eval_input(shorts[port][n][i].port, shorts[port][n][i].pin, value);
  }
}

/*
 * Initialize the GPIO output activity file
 * (open and write csv header)
 */
static void nrf_gpio_init_output_file(void)
{
  if (gpio_out_file_path == NULL) {
    return;
  }

  bs_create_folders_in_path(gpio_out_file_path);
  output_file_ptr = bs_fopen(gpio_out_file_path, "w");
  fprintf(output_file_ptr, "time(microsecond),port,pin,level\n");
}

/*
 * Register an output pin change in the gpio output file
 */
void nrf_gpio_backend_change_output(unsigned int port, unsigned int n, bool value)
{
  if (output_file_ptr != NULL) {
    fprintf(output_file_ptr, "%"PRItime",%u,%u,%u\n",
        nsi_hws_get_time(), port, n, value);
  }

  FOR_EACH_BACKEND_call(change_output, port, n, value);
}

/*
 * Register an external GPIO output -> input short-circuit
 * Normally this is automatically called when a gpio configuration file
 * defines a short-circuit, but it can also be called from test code.
 */
void nrf_gpio_backend_register_short(uint8_t Port_out, uint8_t Pin_out,
    uint8_t Port_in, uint8_t Pin_in)
{
  int i;
  unsigned int max_pins;

  for (i = 0; i < MAX_SHORTS; i++) {
    if (shorts[Port_out][Pin_out][i].port == UINT8_MAX)
      break;
  }
  if (i == MAX_SHORTS) {
    bs_trace_error_line("%s: Number of supported shorts per output (%i) exceeded\n",
        __func__, MAX_SHORTS);
  }
  if (Port_out >= NHW_GPIO_TOTAL_INST) {
    bs_trace_error_time_line("%s: GPIO configuration file attempted to set short from "
        "non existing GPIO port (%u>=%u)\n",
        __func__, Port_out, NHW_GPIO_TOTAL_INST);
  }
  if (Port_in >= NHW_GPIO_TOTAL_INST) {
    bs_trace_error_time_line("%s: GPIO configuration file attempted to set short to "
        "non existing GPIO port (%u>=%u)\n",
        __func__, Port_in, NHW_GPIO_TOTAL_INST);
  }
  max_pins = nrf_gpio_get_number_pins_in_port(Port_out);
  if (Pin_out >= max_pins) {
    bs_trace_error_time_line("%s: GPIO configuration file attempted to set short from "
        "non existing GPIO pin in port %i (%u>=%u)\n",
        __func__, Port_out, Pin_out, max_pins);
  }
  max_pins = nrf_gpio_get_number_pins_in_port(Port_in);
  if (Pin_in >= max_pins) {
    bs_trace_error_time_line("%s: GPIO configuration file attempted to set short to "
        "non existing GPIO pin in port %i (%u>=%u)\n",
        __func__, Port_in, Pin_in, max_pins);
  }
  shorts[Port_out][Pin_out][i].port = Port_in;
  shorts[Port_out][Pin_out][i].pin  = Pin_in;
}

static int process_config_short(char *buf, char *line) {
  char *endp;
  unsigned long X,x,Y,y;
  const char error_msg[] = "%s: Corrupted GPIO configuration file short line."
      "Valid format is \"shortcut X.x Y.y\". Check docs/GPIO.md\n"
      "Line was:%s\n";

  X = strtoul(buf, &endp, 0);
  if ((endp == buf) || (*endp!='.')) {
    bs_trace_error_line(error_msg, __func__, line);
  }
  buf = endp + 1;
  x = strtoul(buf, &endp, 0);
  if ((endp == buf) || (*endp!=' ')) {
    bs_trace_error_line(error_msg, __func__, line);
  }
  buf = endp + 1;
  Y = strtoul(buf, &endp, 0);
  if ((endp == buf) || (*endp!='.')) {
    bs_trace_error_line(error_msg, __func__, line);
  }
  buf = endp + 1;
  y = strtoul(buf, &endp, 0);
  if (endp == buf) {
    bs_trace_error_line(error_msg, __func__, line);
  }
  bs_trace_info_time(4, "Short-circuiting GPIO port %li pin %li to GPIO port %li pin %li\n",
      X,x,Y,y);
  nrf_gpio_backend_register_short(X, x, Y, y);
  return 0;
}

static int process_config_line(char *line)
{
  if (strncmp(line, "input_file ", 11) == 0) {
    return nhw_gpio_filebackend_process_config(line + 11);
  } else if (strncmp(line, "fifo ", 5) == 0) {
    return nhw_gpio_fifobackend_process_config(line + 5, line);
  } else if (strncmp(line, "short", 5) == 0) {
    return process_config_short(line + 5, line);
  } else if (strncmp(line, "s", 1) == 0) {
    return process_config_short(line + 1, line);
  } else {
    bs_trace_error_line("%s: Configuration file command not understood at this point, "
        "Line was \"%s\"\n", __func__, line);
  }

  return 0;
}

/*
 * Load GPIO configuration file (short-circuits)
 */
static void nrf_gpio_load_config(void)
{
  if (gpio_conf_file_path == NULL) {
    return;
  }

  FILE *fileptr = bs_fopen(gpio_conf_file_path, "r");
  char line_buf[MAXLINESIZE];
  int rc;

  while (true) {
    rc = hwu_readline(line_buf, MAXLINESIZE, fileptr);
    if (rc == 0) {
      break;
    }
    rc = process_config_line(line_buf);
    if (rc != 0) {
      break;
    }
  }
  fclose(fileptr);
}
