/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * Copyright (c) 2026 Demant A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "bs_tracing.h"
#include "bs_types.h"
#include "bs_oswrap.h"
#include "bs_cmd_line.h"
#include "bs_dynargs.h"
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NRF_GPIO.h"
#include "NRF_GPIO_backend.h"
#include "nsi_hw_scheduler.h"
#include "nsi_tasks.h"

#define MAXLINESIZE 2048
#define MAX_SHORTS 8
#define NRF_GPIO_MAX_OUTPUT_BACKENDS 8

static char *gpio_out_file_path;
static char *gpio_conf_file_path;

static FILE *output_file_ptr;

static struct {
	uint8_t port;
	uint8_t pin;
} shorts[NHW_GPIO_TOTAL_INST][NHW_GPIO_MAX_PINS_PER_PORT][MAX_SHORTS];

static const struct nrf_gpio_backend_if *backends[NRF_GPIO_MAX_OUTPUT_BACKENDS];
static int n_backends;

void nrf_gpio_backend_register(const struct nrf_gpio_backend_if *backend)
{
	if (backend == NULL) {
		bs_trace_error_line("Programming error\n");
	}
	if (n_backends >= NRF_GPIO_MAX_OUTPUT_BACKENDS) {
		bs_trace_error_line("Too many GPIO output backends\n");
	}
	backends[n_backends++] = backend;
}

void nrf_gpio_backend_short_propagate(unsigned int port, unsigned int n, bool value)
{
	for (int i = 0; i < MAX_SHORTS; i++) {
		if (shorts[port][n][i].port == UINT8_MAX) {
			break;
		}
		nrf_gpio_eval_input(shorts[port][n][i].port, shorts[port][n][i].pin, value);
	}
}

void nrf_gpio_backend_change_output(unsigned int port, unsigned int n, bool value)
{
	if (output_file_ptr != NULL) {
		fprintf(output_file_ptr, "%"PRItime",%u,%u,%u\n",
			nsi_hws_get_time(), port, n, value);
	}

	for (int i = 0; i < n_backends; i++) {
		if (backends[i]->change_output != NULL) {
			backends[i]->change_output(port, n, value);
		}
	}
}

static void nrf_gpio_init_output_file(void)
{
	if (gpio_out_file_path == NULL) {
		return;
	}
	bs_create_folders_in_path(gpio_out_file_path);
	output_file_ptr = bs_fopen(gpio_out_file_path, "w");
	fprintf(output_file_ptr, "time(microsecond),port,pin,level\n");
}

static void nrf_gpio_backend_cleanup(void)
{
	if (output_file_ptr != NULL) {
		fclose(output_file_ptr);
		output_file_ptr = NULL;
	}
}

NSI_TASK(nrf_gpio_backend_cleanup, ON_EXIT_PRE, 100);

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

/*
 * Read a line from a file into a buffer (s), while
 * skipping duplicate spaces (unless they are quoted), comments (#...),
 * and empty lines.
 * The string will be null terminated (even if nothing is copied in).
 *
 * Return: The number of characters copied into s (apart from the termination 0 byte)
 */
static int readline(char *s, int size, FILE *stream)
{
	int c = 0, i = 0;
	bool was_a_space = true;
	bool in_a_string = false;

	while ((i == 0) && (c != EOF)) {
		while ((i < size - 1) && ((c = getc(stream)) != EOF) && c != '\n') {
			if (isspace(c) && (!in_a_string)) {
				if (was_a_space) {
					continue;
				}
				was_a_space = true;
			} else {
				was_a_space = false;
			}
			if (c == '"') {
				in_a_string = !in_a_string;
			}
			if (c == '#') {
				bs_skipline(stream);
				break;
			}
			s[i++] = c;
		}
	}
	s[i] = 0;

	if (i >= size - 1) {
		bs_trace_warning_line("Truncated line while reading from file after %i chars\n",
				      size - 1);
	}
	return i;
}

static int process_config_line(char *s)
{
	char *buf = s;

	if (strncmp(s, "short", 5) == 0) {
		buf += 5;
	} else if (strncmp(s, "s", 1) == 0) {
		buf += 1;
	} else if (strncmp(s, "input_file ", 11) == 0) {
		nrf_gpio_backend_file_add_instance(s + 11);
		return 0;
	} else {
		bs_trace_error_line("%s: Unknown command in GPIO config file: \"%s\"\n",
				    __func__, s);
	}

	/* Parse X.x Y.y for short/s commands */
	{
		unsigned long X, x, Y, y;
		char *endp;
		const char error_msg[] = "%s: Corrupted GPIO configuration file, the valid format is "
				"\"short X.x Y.y\"\nLine was:%s\n";

		X = strtoul(buf, &endp, 0);
		if ((endp == buf) || (*endp != '.')) {
			bs_trace_error_line(error_msg, __func__, s);
		}
		buf = endp + 1;
		x = strtoul(buf, &endp, 0);
		if ((endp == buf) || (*endp != ' ')) {
			bs_trace_error_line(error_msg, __func__, s);
		}
		buf = endp + 1;
		Y = strtoul(buf, &endp, 0);
		if ((endp == buf) || (*endp != '.')) {
			bs_trace_error_line(error_msg, __func__, s);
		}
		buf = endp + 1;
		y = strtoul(buf, &endp, 0);
		if (endp == buf) {
			bs_trace_error_line(error_msg, __func__, s);
		}
		bs_trace_info_time(4, "Short-circuiting GPIO port %lu pin %lu to GPIO port %lu pin %lu\n",
				X, x, Y, y);
		nrf_gpio_backend_register_short(X, x, Y, y);
	}
	return 0;
}

static void nrf_gpio_load_config(void)
{
	if (gpio_conf_file_path == NULL) {
		return;
	}

	FILE *fileptr = bs_fopen(gpio_conf_file_path, "r");
	char line_buf[MAXLINESIZE];
	int rc;

	while (true) {
		rc = readline(line_buf, MAXLINESIZE, fileptr);
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

void nrf_gpio_backend_init(void)
{
	memset(shorts, UINT8_MAX, sizeof(shorts));
	nrf_gpio_load_config();
	nrf_gpio_init_output_file();

	for (int i = 0; i < n_backends; i++) {
		if (backends[i]->init != NULL) {
			backends[i]->init();
		}
	}
}

static void nrf_gpio_backend_register_cmd_args(void)
{
	static bs_args_struct_t args[] = {
		{
			.option = "gpio_out_file",
			.name = "path",
			.type = 's',
			.dest = (void *)&gpio_out_file_path,
			.descript = "Optional path to a file where GPIOs output activity will be saved",
		},
		{
			.option = "gpio_conf_file",
			.name = "path",
			.type = 's',
			.dest = (void *)&gpio_conf_file_path,
			.descript = "Optional path to a file where the GPIOs configuration will be found.",
		},
		ARG_TABLE_ENDMARKER
	};

	bs_add_extra_dynargs(args);
}

NSI_TASK(nrf_gpio_backend_register_cmd_args, PRE_BOOT_1, 100);
