/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * Copyright (c) 2026 Demant A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * File-input backend for GPIO pins.
 * Drives GPIO inputs from stimuli files in CSV format.
 * Multiple instances can be configured, each reading from a separate file.
 *
 * Check docs/GPIO.md for more info.
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NRF_GPIO.h"
#include "NRF_GPIO_backend.h"
#include "bs_types.h"
#include "nsi_hw_scheduler.h"
#include "bs_tracing.h"
#include "bs_oswrap.h"
#include "bs_cmd_line.h"
#include "bs_dynargs.h"
#include "nsi_hws_models_if.h"
#include "nsi_tasks.h"

#define MAXLINESIZE 2048
#define NRF_GPIO_MAX_INPUT_FILES 4

struct gpio_input_file_inst {
	const char *path;
	FILE *fp;
	bs_time_t timer;
	unsigned int port;
	unsigned int pin;
	bool level;
};

static struct gpio_input_file_inst input_insts[NRF_GPIO_MAX_INPUT_FILES];
static int n_input_insts;
static bs_time_t Timer_GPIO_input = TIME_NEVER;

static char *gpio_in_file_path_cli;

void nrf_gpio_backend_file_add_instance(const char *path)
{
	if (n_input_insts >= NRF_GPIO_MAX_INPUT_FILES) {
		bs_trace_error_line("Too many GPIO input file instances (max %d)\n",
				    NRF_GPIO_MAX_INPUT_FILES);
	}
	input_insts[n_input_insts].path = path;
	input_insts[n_input_insts].timer = TIME_NEVER;
	input_insts[n_input_insts].fp = NULL;
	n_input_insts++;
}

static void nrf_gpio_backend_file_update_timer(void)
{
	bs_time_t min = TIME_NEVER;

	for (int i = 0; i < n_input_insts; i++) {
		if (input_insts[i].timer < min) {
			min = input_insts[i].timer;
		}
	}
	Timer_GPIO_input = min;
	nsi_hws_find_next_event();
}

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

static void nrf_gpio_input_file_process_next(struct gpio_input_file_inst *inst, char *buf)
{
	bs_time_t time;
	unsigned int port;
	unsigned int pin;
	unsigned int level;
	int n;

	n = sscanf(buf, "%"SCNtime",%u,%u,%u", &time, &port, &pin, &level);
	if (n > 0 && n < 4) {
		bs_trace_warning_time_line("File %s seems corrupted. Ignoring rest of file. "
					   "Expected \""
					   "<uint64_t time>,<uint port>,<uint pin>,<uint level>\". "
					   "Line was:%s\n",
					   inst->path, buf);
	}
	if (n < 4) {
		fclose(inst->fp);
		inst->fp = NULL;
		inst->timer = TIME_NEVER;
	} else {
		if (time < nsi_hws_get_time()) {
			bs_trace_error_time_line("%s: GPIO input file went back in time(%s)\n",
						__func__, buf);
		}
		if (port >= NHW_GPIO_TOTAL_INST) {
			bs_trace_error_time_line("%s: GPIO input file attempted to access not "
						"existing GPIO port (%u>=%u) (%s)\n",
						__func__, port, NHW_GPIO_TOTAL_INST, buf);
		}
		unsigned int max_pins = nrf_gpio_get_number_pins_in_port(port);
		if (pin >= max_pins) {
			bs_trace_error_time_line("%s: GPIO input file attempted to access not "
						"existing GPIO pin in port %i (%u>=%u) (%s)\n",
						__func__, port, pin, max_pins, buf);
		}
		if (level != 0 && level != 1) {
			bs_trace_error_time_line("%s: level can only be 0 (for low) or 1 (for high)"
						"(%u) (%s)\n",
						__func__, level, buf);
		}
		inst->level = (bool)level;
		inst->pin = pin;
		inst->port = port;
		inst->timer = time;
	}
}

static void nrf_gpio_input_file_open(struct gpio_input_file_inst *inst)
{
	char line_buf[MAXLINESIZE];
	int read;

	inst->fp = bs_fopen(inst->path, "r");

	read = readline(line_buf, MAXLINESIZE, inst->fp);
	if (strncmp(line_buf, "time", 4) == 0) { /* skip possible csv header */
		read = readline(line_buf, MAXLINESIZE, inst->fp);
	}
	if (read == 0) {
		bs_trace_warning_line("%s: Input file %s seems empty\n",
				      __func__, inst->path);
	}

	nrf_gpio_input_file_process_next(inst, line_buf);
}

static void nrf_gpio_backend_file_init(void)
{
	for (int i = 0; i < n_input_insts; i++) {
		nrf_gpio_input_file_open(&input_insts[i]);
	}
	nrf_gpio_backend_file_update_timer();
}

NSI_TASK(nrf_gpio_backend_file_init, HW_INIT, 150);

static void nrf_gpio_backend_file_cleanup(void)
{
	for (int i = 0; i < n_input_insts; i++) {
		if (input_insts[i].fp != NULL) {
			fclose(input_insts[i].fp);
			input_insts[i].fp = NULL;
		}
	}
}

NSI_TASK(nrf_gpio_backend_file_cleanup, ON_EXIT_PRE, 100);

static void nrf_gpio_input_event_triggered(void)
{
	bs_time_t current_time = Timer_GPIO_input;
	char line_buf[MAXLINESIZE];

	for (int i = 0; i < n_input_insts; i++) {
		struct gpio_input_file_inst *inst = &input_insts[i];

		if (inst->timer != current_time || inst->fp == NULL) {
			continue;
		}
		nrf_gpio_eval_input(inst->port, inst->pin, inst->level);
		(void)readline(line_buf, MAXLINESIZE, inst->fp);
		nrf_gpio_input_file_process_next(inst, line_buf);
	}
	nrf_gpio_backend_file_update_timer();
}

NSI_HW_EVENT(Timer_GPIO_input, nrf_gpio_input_event_triggered, 50);

static void nrf_gpio_backend_file_register_cmd_args(void)
{
	static bs_args_struct_t args[] = {
		{
			.option = "gpio_in_file",
			.name = "path",
			.type = 's',
			.dest = (void *)&gpio_in_file_path_cli,
			.descript = "Optional path to a file containing GPIOs inputs activity",
		},
		ARG_TABLE_ENDMARKER
	};

	bs_add_extra_dynargs(args);
}

NSI_TASK(nrf_gpio_backend_file_register_cmd_args, PRE_BOOT_1, 100);

static void nrf_gpio_backend_file_post_cmdline(void)
{
	if (gpio_in_file_path_cli != NULL) {
		nrf_gpio_backend_file_add_instance(gpio_in_file_path_cli);
	}
}

NSI_TASK(nrf_gpio_backend_file_post_cmdline, PRE_BOOT_2, 100);
