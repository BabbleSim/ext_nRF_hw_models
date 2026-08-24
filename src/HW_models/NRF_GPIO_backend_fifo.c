/*
 * Copyright (c) 2026 Demant A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * FIFO backend for GPIO pin changes.
 * Exchanges GPIO pin state changes between two simulated devices.
 * NOP messages are used as keepalive/timing markers so both sides
 * keep advancing their simulated time.
 * Multiple instances can be configured, each with its own FIFO pair.
 */

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "bs_cmd_line.h"
#include "bs_dynargs.h"
#include "bs_oswrap.h"
#include "bs_pc_base_fifo_user.h"
#include "bs_tracing.h"
#include "bs_types.h"
#include "NHW_config.h"
#include "NHW_common_types.h"
#include "NRF_GPIO.h"
#include "NRF_GPIO_backend.h"
#include "nsi_hws_models_if.h"
#include "nsi_hw_scheduler.h"
#include "nsi_tasks.h"

/*
 * FIFO protocol used to exchange GPIO pin changes between two simulated devices.
 */
struct gpfifo_msg_header {
	bs_time_t time;
	enum {
		gpfifo_NOP = 0,
		gpfifo_PIN_CHANGE,
		gpfifo_DISCONNECT,
	} msg_type;
	uint16_t size;
} __attribute__ ((packed));

struct gpfifo_msg_pin_change {
	struct gpfifo_msg_header header;
	uint8_t port;
	uint8_t pin;
	uint8_t level;
} __attribute__ ((packed));

#define GPFIFO_BIGGEST_MSG (sizeof(struct gpfifo_msg_pin_change))

struct gpfifo_st_t {
	bs_time_t Rx_timer;
	bs_time_t Tx_timer;

	char *fifo_Tx_path;
	char *fifo_Rx_path;

	int fifo_tx;
	int fifo_rx;

	bs_time_t mdt;

	bool enabled;
	bool disconnected;
	bool last_rx_msg_pending;

	char last_rx_msg[GPFIFO_BIGGEST_MSG];
};

#define GPFIFO_MSG_HEADER_SIZE (sizeof(struct gpfifo_msg_header))
#define GPFIFO_MSG_PIN_CHANGE_BODY_SIZE \
	(sizeof(struct gpfifo_msg_pin_change) - sizeof(struct gpfifo_msg_header))
#define GPFIFO_DEFAULT_MDT 10000
#define NRF_GPIO_MAX_FIFO_INSTS 4

static bs_time_t Timer_GPIOFIFO = TIME_NEVER;

static struct gpfifo_st_t gpfifo_insts[NRF_GPIO_MAX_FIFO_INSTS];
static int n_fifo_insts;
static bool gpio_fifo_no_terminate;

/* CLI-parsed paths for the first (backward-compat) instance */
static char *gpio_fifob_txfile_cli;
static char *gpio_fifob_rxfile_cli;
static double gpio_fifo_mdt_cli = NAN;

static void nrf_gpio_backend_fifo_init(void);
static void nrf_gpio_backend_fifo_change_output(unsigned int port, unsigned int n, bool value);

static const struct nrf_gpio_backend_if gpfifo_backend = {
	.init = nrf_gpio_backend_fifo_init,
	.change_output = nrf_gpio_backend_fifo_change_output,
};

static void nrf_gpio_backend_fifo_update_timer(void);
static void nrf_gpio_backend_fifo_create_fifos(struct gpfifo_st_t *inst);
static void nrf_gpio_backend_fifo_tx_nop(struct gpfifo_st_t *inst, bs_time_t t);
static void nrf_gpio_backend_fifo_tx_pin_change(struct gpfifo_st_t *inst,
						unsigned int port, unsigned int pin, bool level);
static void nrf_gpio_backend_fifo_handle_rx_timer(struct gpfifo_st_t *inst);

void nrf_gpio_backend_fifo_add_instance(const char *tx_path, const char *rx_path, double mdt)
{
	if (n_fifo_insts >= NRF_GPIO_MAX_FIFO_INSTS) {
		bs_trace_error_line("Too many GPIO FIFO backend instances (max %d)\n",
				    NRF_GPIO_MAX_FIFO_INSTS);
	}

	struct gpfifo_st_t *inst = &gpfifo_insts[n_fifo_insts];

	inst->fifo_Tx_path = (char *)tx_path;
	inst->fifo_Rx_path = (char *)rx_path;
	inst->mdt = isnan(mdt) ? GPFIFO_DEFAULT_MDT : (bs_time_t)mdt;
	inst->enabled = true;

	if (n_fifo_insts == 0) {
		nrf_gpio_backend_register(&gpfifo_backend);
	}
	n_fifo_insts++;
}

static void nrf_gpio_backend_fifo_init(void)
{
	signal(SIGPIPE, SIG_IGN);

	for (int i = 0; i < n_fifo_insts; i++) {
		struct gpfifo_st_t *inst = &gpfifo_insts[i];

		inst->fifo_tx = -1;
		inst->fifo_rx = -1;
		inst->Rx_timer = TIME_NEVER;
		inst->Tx_timer = TIME_NEVER;
		inst->last_rx_msg_pending = false;

		nrf_gpio_backend_fifo_create_fifos(inst);

		inst->Tx_timer = 0;
		inst->Rx_timer = inst->mdt;
	}
	nrf_gpio_backend_fifo_update_timer();
}

static void nrf_gpio_backend_fifo_change_output(unsigned int port, unsigned int n, bool value)
{
	for (int i = 0; i < n_fifo_insts; i++) {
		struct gpfifo_st_t *inst = &gpfifo_insts[i];

		if (!inst->enabled || inst->disconnected) {
			continue;
		}
		nrf_gpio_backend_fifo_tx_pin_change(inst, port, n, value);
		inst->Tx_timer = nsi_hws_get_time() + inst->mdt;
	}
	nrf_gpio_backend_fifo_update_timer();
}

static void nrf_gpio_backend_fifo_cleanup(void)
{
	for (int i = 0; i < n_fifo_insts; i++) {
		struct gpfifo_st_t *inst = &gpfifo_insts[i];

		if (!inst->enabled) {
			continue;
		}

		if ((inst->fifo_tx != -1) && !inst->disconnected) {
			nrf_gpio_backend_fifo_tx_nop(inst, nsi_hws_get_time());
		}

		if (inst->fifo_tx != -1) {
			(void)close(inst->fifo_tx);
			inst->fifo_tx = -1;
			(void)remove(inst->fifo_Tx_path);
		}

		if (inst->fifo_rx != -1) {
			(void)close(inst->fifo_rx);
			inst->fifo_rx = -1;
			(void)remove(inst->fifo_Rx_path);
		}
	}
}

NSI_TASK(nrf_gpio_backend_fifo_cleanup, ON_EXIT_PRE, 100);

static void nrf_gpio_backend_fifo_register_cmdline(void)
{
	static bs_args_struct_t args2[] = {
		{
			.option = "gpio_fifob_txfile",
			.name = "path",
			.type = 's',
			.dest = (void *)&gpio_fifob_txfile_cli,
			.descript = "Path to the FIFO to be used for GPIO Tx (it will be created automatically)."
		},
		{
			.option = "gpio_fifob_rxfile",
			.name = "path",
			.type = 's',
			.dest = (void *)&gpio_fifob_rxfile_cli,
			.descript = "Path to the FIFO to be used for GPIO Rx (it will be created automatically)."
		},
		ARG_TABLE_ENDMARKER
	};

	static bs_args_struct_t args1[] = {
		{
			.is_switch = true,
			.option = "gpio_fifob_no_terminate",
			.type = 'b',
			.dest = (void *)&gpio_fifo_no_terminate,
			.descript = "Do NOT terminate execution when the other end disconnects gracefully"
		},
		{
			.option = "gpio_fifob_mdt",
			.type = 'd',
			.dest = (void *)&gpio_fifo_mdt_cli,
			.descript = "Maximum amount of time the GPIO FIFO backend can stay without sending a NOP keepalive."
		},
		ARG_TABLE_ENDMARKER
	};

	bs_add_extra_dynargs(args2);
	bs_add_extra_dynargs(args1);
}

NSI_TASK(nrf_gpio_backend_fifo_register_cmdline, PRE_BOOT_1, 50);

static void nrf_gpio_backend_fifo_post_cmdline(void)
{
	if ((gpio_fifob_txfile_cli == NULL) && (gpio_fifob_rxfile_cli == NULL)) {
		return;
	}

	if ((gpio_fifob_txfile_cli == NULL) || (gpio_fifob_rxfile_cli == NULL)) {
		bs_trace_error_line("gpio_fifob_txfile and gpio_fifob_rxfile must both be provided\n");
	}

	nrf_gpio_backend_fifo_add_instance(gpio_fifob_txfile_cli, gpio_fifob_rxfile_cli,
					   gpio_fifo_mdt_cli);
}

NSI_TASK(nrf_gpio_backend_fifo_post_cmdline, PRE_BOOT_2, 50);

static void nrf_gpio_backend_fifo_update_timer(void)
{
	Timer_GPIOFIFO = TIME_NEVER;

	for (int i = 0; i < n_fifo_insts; i++) {
		struct gpfifo_st_t *inst = &gpfifo_insts[i];

		if (!inst->enabled || inst->disconnected) {
			continue;
		}
		if (inst->Rx_timer < Timer_GPIOFIFO) {
			Timer_GPIOFIFO = inst->Rx_timer;
		}
		if (inst->Tx_timer < Timer_GPIOFIFO) {
			Timer_GPIOFIFO = inst->Tx_timer;
		}
	}
	nsi_hws_find_next_event();
}

static void nrf_gpio_backend_fifo_tx_msg(struct gpfifo_st_t *inst, void *ptr, size_t size)
{
	int res;

	res = write(inst->fifo_tx, ptr, size);
	if (res != (int)size) {
		inst->disconnected = true;
		if ((res == -1) && (errno == EPIPE)) {
			if (gpio_fifo_no_terminate) {
				bs_trace_warning_time_line("GPIO FIFO other end disconnected\n");
				return;
			}
			bs_trace_exit_time_line("GPIO FIFO other end disconnected, terminating\n");
		}
		bs_trace_error_time_line("GPIO FIFO write error (%i, %i)\n", res, errno);
	}
}

static void nrf_gpio_backend_fifo_tx_nop(struct gpfifo_st_t *inst, bs_time_t t)
{
	struct gpfifo_msg_header msg;

	memset(&msg, 0, sizeof(msg));
	msg.time = t;
	msg.msg_type = gpfifo_NOP;
	msg.size = sizeof(msg);
	nrf_gpio_backend_fifo_tx_msg(inst, &msg, sizeof(msg));
}

static void nrf_gpio_backend_fifo_tx_pin_change(struct gpfifo_st_t *inst,
						unsigned int port, unsigned int pin, bool level)
{
	struct gpfifo_msg_pin_change msg;

	memset(&msg, 0, sizeof(msg));
	msg.header.time = nsi_hws_get_time();
	msg.header.msg_type = gpfifo_PIN_CHANGE;
	msg.header.size = sizeof(msg);
	msg.port = (uint8_t)port;
	msg.pin = (uint8_t)pin;
	msg.level = level ? 1U : 0U;
	nrf_gpio_backend_fifo_tx_msg(inst, &msg, sizeof(msg));
}

static int nrf_gpio_backend_fifo_read(struct gpfifo_st_t *inst, void *ptr, size_t size)
{
	int res = read(inst->fifo_rx, ptr, size);

	if (res != (int)size) {
		inst->disconnected = true;
		if (res == 0) {
			if (gpio_fifo_no_terminate) {
				bs_trace_warning_line("GPIO FIFO other end disconnected\n");
				return res;
			}
			bs_trace_exit_time_line("GPIO FIFO other end disconnected unexpectedly\n");
		}
		bs_trace_error_time_line("GPIO FIFO read error (%i, %s)\n", res, strerror(errno));
	}

	return res;
}

static void nrf_gpio_backend_fifo_rx_get_one_msg(struct gpfifo_st_t *inst)
{
	struct gpfifo_msg_header *buf = (struct gpfifo_msg_header *)inst->last_rx_msg;
	int ret;

	ret = nrf_gpio_backend_fifo_read(inst, buf, GPFIFO_MSG_HEADER_SIZE);
	if (ret != GPFIFO_MSG_HEADER_SIZE) {
		return;
	}

	switch (buf->msg_type) {
	case gpfifo_NOP:
		break;
	case gpfifo_PIN_CHANGE:
		ret = nrf_gpio_backend_fifo_read(inst,
						 (char *)buf + GPFIFO_MSG_HEADER_SIZE,
						 GPFIFO_MSG_PIN_CHANGE_BODY_SIZE);
		if (ret != GPFIFO_MSG_PIN_CHANGE_BODY_SIZE) {
			return;
		}
		break;
	case gpfifo_DISCONNECT:
		inst->disconnected = true;
		if (gpio_fifo_no_terminate) {
			inst->enabled = false;
			inst->Rx_timer = TIME_NEVER;
			inst->Tx_timer = TIME_NEVER;
			nrf_gpio_backend_fifo_update_timer();
			return;
		}
		bs_trace_exit_line_time("GPIO FIFO other end disconnected. Terminating\n");
		break;
	default:
		bs_trace_error_time_line("Corrupted GPIO FIFO stream\n");
		break;
	}
}

static void nrf_gpio_backend_fifo_rx_process_last_msg_post(struct gpfifo_st_t *inst)
{
	struct gpfifo_msg_header *buf = (struct gpfifo_msg_header *)inst->last_rx_msg;

	if (!inst->last_rx_msg_pending) {
		return;
	}

	inst->last_rx_msg_pending = false;

	switch (buf->msg_type) {
	case gpfifo_NOP:
		break;
	case gpfifo_PIN_CHANGE:
	{
		struct gpfifo_msg_pin_change *msg = (struct gpfifo_msg_pin_change *)buf;
		nrf_gpio_eval_input(msg->port, msg->pin, msg->level != 0U);
		break;
	}
	case gpfifo_DISCONNECT:
	default:
		break;
	}
}

static void nrf_gpio_backend_fifo_rx_process_last_msg_pre(struct gpfifo_st_t *inst)
{
	struct gpfifo_msg_header *buf = (struct gpfifo_msg_header *)inst->last_rx_msg;

	switch (buf->msg_type) {
	case gpfifo_NOP:
	case gpfifo_PIN_CHANGE:
		inst->last_rx_msg_pending = true;
		inst->Rx_timer = BS_MAX(buf->time, nsi_hws_get_time());
		nrf_gpio_backend_fifo_update_timer();
		break;
	case gpfifo_DISCONNECT:
		break;
	default:
		bs_trace_error_time_line("Programming error\n");
		break;
	}
}

static void nrf_gpio_backend_fifo_handle_rx_timer(struct gpfifo_st_t *inst)
{
	nrf_gpio_backend_fifo_rx_process_last_msg_post(inst);

	if (inst->enabled && !inst->disconnected) {
		nrf_gpio_backend_fifo_rx_get_one_msg(inst);
		nrf_gpio_backend_fifo_rx_process_last_msg_pre(inst);
	}
}

static void nrf_gpio_backend_fifo_handle_tx_timer(struct gpfifo_st_t *inst)
{
	if (inst->enabled && !inst->disconnected) {
		bs_time_t next_tx = nsi_hws_get_time() + inst->mdt;

		inst->Tx_timer = next_tx;
		nrf_gpio_backend_fifo_tx_nop(inst, next_tx);
	}
}

static void nrf_gpio_backend_fifo_timer_triggered(void)
{
	bs_time_t current_time = Timer_GPIOFIFO;

	for (int i = 0; i < n_fifo_insts; i++) {
		struct gpfifo_st_t *inst = &gpfifo_insts[i];

		if (!inst->enabled || inst->disconnected) {
			continue;
		}
		if (inst->Tx_timer == current_time) {
			nrf_gpio_backend_fifo_handle_tx_timer(inst);
		}
		if (inst->Rx_timer == current_time) {
			nrf_gpio_backend_fifo_handle_rx_timer(inst);
		}
	}
	nrf_gpio_backend_fifo_update_timer();
}

NSI_HW_EVENT(Timer_GPIOFIFO, nrf_gpio_backend_fifo_timer_triggered, 900);

static void nrf_gpio_backend_fifo_create_fifos(struct gpfifo_st_t *inst)
{
	int prov_descr;

	bs_create_folders_in_path(inst->fifo_Tx_path);
	bs_create_folders_in_path(inst->fifo_Rx_path);

	if (pb_create_fifo_if_not_there(inst->fifo_Tx_path) != 0) {
		bs_trace_error_line("Couldn't create GPIO FIFO Tx FIFO\n");
	}
	if (pb_create_fifo_if_not_there(inst->fifo_Rx_path) != 0) {
		bs_trace_error_line("Couldn't create GPIO FIFO Rx FIFO\n");
	}

	prov_descr = open(inst->fifo_Rx_path, O_RDONLY | O_NONBLOCK);
	if (prov_descr == -1) {
		bs_trace_error_line("Couldn't open GPIO FIFO Rx FIFO (%i, %s)\n",
				    errno, strerror(errno));
	}

	inst->fifo_tx = open(inst->fifo_Tx_path, O_WRONLY);
	if (inst->fifo_tx == -1) {
		bs_trace_error_line("Couldn't open GPIO FIFO Tx FIFO (%i, %s)\n",
				    errno, strerror(errno));
	}

	inst->fifo_rx = open(inst->fifo_Rx_path, O_RDONLY);
	if (inst->fifo_rx == -1) {
		bs_trace_error_line("Couldn't open GPIO FIFO Rx FIFO (%i, %s)\n",
				    errno, strerror(errno));
	}

	(void)close(prov_descr);
}
