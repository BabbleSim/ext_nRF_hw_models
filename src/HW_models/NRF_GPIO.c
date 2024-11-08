/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * GPIO - General purpose input/output
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/gpio.html?cp=5_1_0_5_7
 *
 * This model has the following limitations:
 *  * Note a.1:
 *    PIN_CNF.PULL is ignored. If a pin input is disconnected and not driven externally it will
 *    just remain at zero/low.
 *  * Note a.2:
 *    PIN_CNF.DRIVE / drive strength is ignored
 *    It is not possible to connect two outputs together building a logical AND or OR
 *  * Note a.3:
 *    Reading OUTCLR & DIRCLR reads always 0
 *  * Note a.4: There is no modeling of system idle/off, and as such a DETECT raising edge from the
 *    GPIO will not cause the system to exit those states.
 *
 * This model has the following approximations:
 *  * Note b.1:
 *    If a DIR bit is cleared for a pin which was being driven as a high output,
 *    that pin will be lowered immediately (like if it was driven low)
 *  * Note b.2:
 *    All drives are instantaneous.
 *  * Note b.3:
 *    During pin reconfigurations, there can be spurious transitions in the pins outputs
 *    which may not correspond to how the real HW would behave
 *  * Note b.4:
 *    In real HW, after a write to LATCH, the DETECT event output to the GPIOTE is kept low for
 *    a few clocks, before being raised again (if it needs to), in the model the new pulse/raise
 *    is sent instantaneously to the GPIOTE
 *
 * 5340 notes:
 *  * Note 53.1: Unlike in real HW, the Net and App cores GPIO peripherals are mapped to different/separate
 *    simulated ports.
 *
 *  * Note 53.2: MCUSEL is ignored at this point. (see point above)
 *
 *  * Note 53.3: Like in real HW DETECTMODE & DETECTMODE_SEC control the generation of DETECT_SEC & DETEC_NONSEC,
 *    which are connected to GPIOTE0 (GPIOTESEC) and GPIOTE1 respectively.
 *    But, all pins sense output (be them secure or not) is used as input for *both*.
 *    Note: This only affects the app core GPIOs, the net core GPIOs do not have the DETECT_SEC signal
 *    connected to anything.
 *
 * 53 & 54 notes:
 *  * Note 534.1: Split security distinctions are ignored
 *    == there is no distinction for accesses from secure or non secure bus masters or the S/NS address ranges.
 *    Accessing either through the S or NS address range all registers are equally accessible.
 *
 *  * Note 534.2: A possible secure/non secure pin configuration in the SPU is ignored
 *
 * 54L notes:
 *  * Note 54.1: PIN_CNF[n].CTRLSEL is ignored by now, but other peripherals can still take over a pin (irrespectively of .CTRLSEL)
 *    If another peripheral HW model has taken ownership of a pin, you will get a warning if you try to drive it through
 *    the GPIO registers. You will be able to read a pin input level even if another peripheral has control over it,
 *    while the pin input driver is enabled.
 *
 *  * Note 54.2: As we do not check pins secure configuration, at this point both DETECT_SEC & DETEC_NONSEC are generated
 *    at the same time from all pins sense outputs
 *
 *  * Note 54.3: Considerations regarding the LATCH register split security (different behavior depending on S/NS master access)
 *    are ignored (due to Note 534.1)
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NRF_GPIO.h"
#include "NRF_GPIOTE.h"
#include "bs_tracing.h"
#include "nsi_tasks.h"

NRF_GPIO_Type NRF_GPIO_regs[NHW_GPIO_TOTAL_INST];

struct gpio_status {
  uint32_t IO_level; /* Actual levels in the pins */
  uint32_t O_level;

  uint32_t DETECT;   /* Sense output / unlatched/non-sticky detect */
  uint32_t LDETECT;  /* Latched sense output */
  bool DETECT_signal; /* Individual detect signal to the GPIOTE (for 5340 == DETECT_NSEC) */
  bool DETECT_SEC_signal; /* Only used for 5340 */

  uint32_t INPUT_mask; /* As a 32bit mask, PIN_CNF[*].INPUT (0: enabled; 1: disabled)*/
  uint32_t SENSE_mask; /* As a 32bit mask, PIN_CNF[*].SENSE.en (1: enabled; 0: disabled)*/
  uint32_t SENSE_inv;  /* As a 32bit mask, PIN_CNF[*].SENSE.inv (1: inverted;0: not inverted) */

  /*
   * Is the output driven by another peripheral (1) or the GPIO directly (0).
   * Note that we don't keep track of who "owns" a pin, only that somebody else does
   */
  uint32_t out_override;
  /* Out value provided by other peripherals */
  uint32_t external_OUT;

  /* Is the pin input controlled by a peripheral(1) or the GPIO(0) */
  uint32_t input_override;
  /* If input_override, is the peripheral configuring the input buffer as connected (1) or disconnected (0) */
  uint32_t input_override_connected;

  /* Is "dir" controlled by a peripheral(1) or the GPIO(0) */
  uint32_t dir_override;
  /* If dir_override is set, is the peripheral configuring the output as connected (1) or disconnected (0) */
  uint32_t dir_override_set;

  /* Callbacks for peripherals to be informed of input changes */
  nrf_gpio_input_callback_hw_t per_intoggle_callbacks[NHW_GPIO_MAX_PINS_PER_PORT];
  void *per_intoggle_cb_data[NHW_GPIO_MAX_PINS_PER_PORT];

  int nbr_pins;
  int partner_GPIOTE;
  int has_sense;
};

static struct gpio_status gpio_st[NHW_GPIO_TOTAL_INST];

/* Callbacks for test code to be informed of input/output changes: */
static nrf_gpio_input_callback_t test_intoggle_callback;
static nrf_gpio_input_callback_t test_outtoggle_callback;

/*
 * Initialize the GPIOs model
 */
static void nrf_gpio_init(void) {
  memset(NRF_GPIO_regs, 0, sizeof(NRF_GPIO_regs));

  int GPIO_n_ports_pins[NHW_GPIO_TOTAL_INST] = NHW_GPIO_NBR_PINS;
  int GPIOTE_partners[NHW_GPIO_TOTAL_INST] = NHW_GPIO_PARTNER_GPIOTE;
  int has_sense[NHW_GPIO_TOTAL_INST] = NHW_GPIO_HAS_PIN_SENSE;

  for (int p = 0; p < NHW_GPIO_TOTAL_INST; p ++) {
    for (int n = 0; n < GPIO_n_ports_pins[p]; n++) {
      NRF_GPIO_regs[p].PIN_CNF[n] = 0x2; /* Disconnected out of reset */
    }
    gpio_st[p].INPUT_mask = UINT32_MAX; /* All disconnected out of reset */
    gpio_st[p].nbr_pins = GPIO_n_ports_pins[p];
    gpio_st[p].partner_GPIOTE = GPIOTE_partners[p];
    gpio_st[p].has_sense = has_sense[p];
  }

  nrf_gpio_backend_init();
}

NSI_TASK(nrf_gpio_init, HW_INIT, 100);

unsigned int nrf_gpio_get_number_pins_in_port(int port) {
  return gpio_st[port].nbr_pins;
}

static void nrf_gpio_eval_outputs(unsigned int port);
static void nrf_gpio_eval_inputs(unsigned int port);
void nrf_gpio_eval_input(unsigned int port, unsigned int n, bool value);

/*
 * Register a test callback to be called whenever a pin IN register changes
 */
void nrf_gpio_test_register_in_callback(nrf_gpio_input_callback_t fptr) {
  test_intoggle_callback = fptr;
}

/*
 * Register a test callback to be called whenever an *output* pin changes
 */
void nrf_gpio_test_register_out_callback(nrf_gpio_input_callback_t fptr) {
  test_outtoggle_callback = fptr;
}

/*
 * Change a pin input value
 *
 * Note: The pin must not be currently driven by the SOC, or you will get an error
 *
 * Inputs:
 *  * port: Which GPIO port
 *  * n: which pin in that GPIO port
 *  * value: true: high, false: low
 */
void nrf_gpio_test_change_pin_level(unsigned int port, unsigned int n, bool value) {
  nrf_gpio_eval_input(port, n, value);
}

/*
 * Get a pin level
 *
 * Inputs:
 *  * port: Which GPIO port
 *  * n: which pin in that GPIO port
 * Return: true (high), false (low)
 */
bool nrf_gpio_get_pin_level(unsigned int port, unsigned int n) {
  return (gpio_st[port].IO_level >> n) & 0x1;
}

#define CHECK_PIN_EXISTS(port, n, dir) \
		if (port >= NHW_GPIO_TOTAL_INST || n >= gpio_st[port].nbr_pins) { \
			bs_trace_error_time_line("%s: Error, attempted to toggle "dir" for nonexistent " \
					"GPIO port %i, pin %i\n", \
					__func__, port, n); \
		}

static inline uint32_t get_enabled_inputs(unsigned int port){
  struct gpio_status *st = &gpio_st[port];
  return (~st->input_override & ~st->INPUT_mask)
      | (st->input_override & st->input_override_connected);
}

static inline uint32_t get_dir(unsigned int port){
  struct gpio_status *st = &gpio_st[port];
  return (~st->dir_override & NRF_GPIO_regs[port].DIR)
      | (st->dir_override & st->dir_override_set);
}

/*
 * Function with which another peripheral can claim configuration control of a pin.
 *
 * Inputs:
 *	* port: Which GPIO port
 *	* n : which pin in that GPIO port
 *	* override_output:
 *		* -1 : Don't change
 *		*  0 : Leave for GPIO control (GPIO OUT register sets the output value)
 *		*  1 : Take external control of pin output value (peripheral sets the output value
 *		       with nrf_gpio_peri_change_output() )
 *	* override_input:
 *		* -1 : Don't change
 *		*  0 : Leave input to be controlled by the GPIO module
 *		*  2 : Take external control of input, and disconnect
 *		*  3 : Take external control of input, and connect
 *	* override_dir:
 *		* -1 : Don't change
 *		*  0 : Leave DIR to be controlled by the GPIO module
 *		*  2 : Take external control of DIR, and disconnect (not driving output)
 *		*  3 : Take external control of DIR, and connect (driving output)
 *      * fptr: Function to be called whenever that input toggles (if enabled).
 *              Set to NULL if not needed.
 *      * new_level:
 *              * -1: Don't change
 *              *  0: low
 *              *  1: high
 */
void nrf_gpio_peri_pin_control(unsigned int port, unsigned int n,
    int override_output, int override_input, int override_dir,
    nrf_gpio_input_callback_hw_t fptr, void *fptr_data, int new_level) {

  struct gpio_status *st = &gpio_st[port];

  if (port >= NHW_GPIO_TOTAL_INST || n >= st->nbr_pins) { /* LCOV_EXCL_BR_LINE */
    bs_trace_error_time_line("Programming error\n"); /* LCOV_EXCL_LINE */
  }

  uint32_t mask = 1<<n;
  bool need_output_eval = false;
  bool need_input_eval = false;

  if (override_output >= 0) {
    st->out_override &= ~mask;
    st->out_override |= (uint32_t)(override_output?1:0) << n;
    need_output_eval = true;
  }
  if (override_input >= 0) {
    st->input_override &= ~mask;
    st->input_override |= (uint32_t)(override_input?1:0) << n;

    st->input_override_connected &= ~mask;
    st->input_override_connected |= (uint32_t)(override_input==3?1:0) << n;

    need_input_eval = true;
  }
  if (override_dir >= 0) {
    st->dir_override &= ~mask;
    st->dir_override |= (uint32_t)(override_dir?1:0) << n;

    st->dir_override_set &= ~mask;
    st->dir_override_set |= (uint32_t)(override_dir==3?1:0) << n;

    need_output_eval = true;
  }
  st->per_intoggle_callbacks[n] = fptr;
  st->per_intoggle_cb_data[n] = fptr_data;
  if (new_level >= 0) {
    st->external_OUT &= ~((uint32_t)1 << n);
    st->external_OUT |= (uint32_t)(new_level?1:0) << n;
    need_output_eval = true;
  }

  if (need_output_eval) {
    nrf_gpio_eval_outputs(port);
  }
  if (need_input_eval) {
    nrf_gpio_eval_inputs(port);
  }
}

/*
 * A peripheral wants to toggle a GPIO output to a new value <value>.
 * Note: The value may be the same as it was, in which case nothing will happen.
 *
 * Inputs:
 *  * port is the GPIO port
 *  * n is the pin number inside that GPIO port (0..31)
 *  * value is the new output value high (true) or low (false)
 */
void nrf_gpio_peri_change_output(unsigned int port, unsigned int n, bool value)
{
  CHECK_PIN_EXISTS(port, n, "output"); /* LCOV_EXCL_BR_LINE */

  struct gpio_status *st = &gpio_st[port];

  if (((st->out_override >> n) & 0x1) != 1) { /* LCOV_EXCL_START */
    bs_trace_error_time_line("%s: Programming error, a peripheral is trying to toggle "
        "a GPIO output it does not own, GPIO port %i, pin %i\n",
        __func__, port, n);
  }

  if (((get_dir(port) >> n) & 0x1) != 1) {
    bs_trace_warning_time_line("%s: A peripheral is trying to toggle "
        "a GPIO output but the output is disabled, "
        "GPIO port %i, pin %i\n",
        __func__, port, n);
  } /* LCOV_EXCL_STOP */

  st->external_OUT &= ~((uint32_t)1 << n);
  st->external_OUT |= (uint32_t)value << n;
  nrf_gpio_eval_outputs(port);
}

static void nrf_gpio_update_detect_signal(unsigned int port) {
  struct gpio_status *st = &gpio_st[port];

  if (NRF_GPIO_regs[port].DETECTMODE == 0){ //gpio.detect signal from not latched detect
    st->DETECT_signal = (st->DETECT != 0);
  } else {//gpio.detect signal from latched detect
    st->DETECT_signal = (st->LDETECT != 0);
  }

#if defined(NRF5340)
  if (NRF_GPIO_regs[port].DETECTMODE_SEC == 0){
    st->DETECT_SEC_signal = (st->DETECT != 0);
  } else {//gpio.detect signal from latched detect
    st->DETECT_SEC_signal = (st->LDETECT != 0);
  }
#endif
}

/*
 * Evaluate sense output (after a change of input or configuration)
 */
static void nrf_gpio_eval_sense(unsigned int port){
  struct gpio_status *st = &gpio_st[port];

  if (!st->has_sense) {
    return;
  }

  /* Note SENSE_dir inverts the output */
  st->DETECT = (NRF_GPIO_regs[port].IN ^ st->SENSE_inv) & st->SENSE_mask;
  st->LDETECT |= st->DETECT;
  NRF_GPIO_regs[port].LATCH = st->LDETECT;

  bool old_DETECT_signal = st->DETECT_signal;
#if defined(NRF5340)
  bool old_DETECT_SEC_signal = st->DETECT_SEC_signal;
#endif
  nrf_gpio_update_detect_signal(port);

  if ((st->DETECT_signal == true) && (old_DETECT_signal==false) && (st->partner_GPIOTE>=0)) {
    nrf_gpiote_port_detect_raise(st->partner_GPIOTE, port);
  }

#if defined(NRF5340)
  if ((port == NHW_GPIO_APP_P0) || (port == NHW_GPIO_APP_P1)) {
    if ((st->DETECT_SEC_signal == true) && (old_DETECT_SEC_signal==false)) {
      nrf_gpiote_port_detect_raise(NHW_GPIOTE_APP0, port);
    }
  }
#endif
}

/*
 * Return the level of the DETECT output signal for a GPIO instance
 *
 * input: port: The GPIO instance number
 */
bool nrf_gpio_get_detect_level(unsigned int port){
  return gpio_st[port].DETECT_signal;
}

/*
 * Get the level of the IN signal for GPIO <port> pin <n>
 */
bool nrf_gpio_get_IN(unsigned int port, unsigned int n) {
  return (NRF_GPIO_regs[port].IN >> n) & 0x1;
}

/*
 * The input has changed and the driver is connected,
 * notify as necessary
 */
static void nrf_gpio_input_change_sideeffects(unsigned int port, unsigned int n)
{
  struct gpio_status *st = &gpio_st[port];
  bool level = nrf_gpio_get_IN(port,n);

  if (st->per_intoggle_callbacks[n] != NULL) {
    st->per_intoggle_callbacks[n](port, n, level, st->per_intoggle_cb_data[n]);
  }
  if (test_intoggle_callback != NULL) {
    test_intoggle_callback(port, n, level);
  }
}

/*
 * An input pin has toggled or the input configuration has changed,
 * propagate it
 */
static void nrf_gpio_eval_inputs(unsigned int port)
{
  uint32_t new_IN = gpio_st[port].IO_level & get_enabled_inputs(port);

  uint32_t diff = new_IN ^ NRF_GPIO_regs[port].IN;

  NRF_GPIO_regs[port].IN = new_IN;

  for (int n = __builtin_ffs(diff) - 1; n >= 0; n = __builtin_ffs(diff) - 1) {
    nrf_gpio_input_change_sideeffects(port, n);
    diff &= ~(1 << n);
  }

  nrf_gpio_eval_sense(port);
}

/*
 * An input *may* be changing to a new value <value>.
 * Note: The value may be the same as it was, in which case nothing will happen.
 *
 * This function is meant to be called from something which drives the input
 * **externally**
 *
 * Inputs:
 *  * port is the GPIO port
 *  * n is the pin number inside that GPIO port (0..31)
 *  * value: is the input high (true) or low (false)
 */
void nrf_gpio_eval_input(unsigned int port, unsigned int n, bool value)
{
  CHECK_PIN_EXISTS(port, n, "input"); /* LCOV_EXCL_BR_LINE */

  uint32_t dir = get_dir(port);

  if ((dir >> n) & 0x1) { /* LCOV_EXCL_START */
    bs_trace_warning_time_line("%s: Attempted to drive externally a pin which is "
        "currently being driven by the SOC. It will be ignored."
        "GPIO port %i, pin %i\n",
        __func__, port, n);
    return;
  }			/* LCOV_EXCL_STOP */

  int diff = ((gpio_st[port].IO_level >> n) & 0x1) ^ (uint32_t)value;

  if (diff == 0) {
    /* No toggle */
    return;
  }

  gpio_st[port].IO_level ^= (uint32_t)1 << n;

  nrf_gpio_eval_inputs(port);
}

/*
 * The output is being changed, propagate it as necessary and/or record it.
 */
static void nrf_gpio_output_change_sideeffects(unsigned int port,unsigned  int n, bool value)
{
  nrf_gpio_backend_write_output_change(port, n, value);
  if (test_outtoggle_callback != NULL) {
    test_outtoggle_callback(port, n, value);
  }
  nrf_gpio_backend_short_propagate(port, n, value);
}

/*
 * Reevaluate outputs after a configuration or OUT/external_OUT change
 */
static void nrf_gpio_eval_outputs(unsigned int port)
{
  /* Actual level in the pin, but only of the bits driven by output: */
  struct gpio_status *st = &gpio_st[port];

  uint32_t dir = get_dir(port); /* Which pins are driven by output */

  uint32_t out = (~st->out_override & NRF_GPIO_regs[port].OUT)
			    | (st->out_override & st->external_OUT);

  uint32_t new_output = dir & out;

  uint32_t diff = new_output ^ st->O_level;

  if (diff == 0) {
    return;
  }

  st->O_level = new_output;

  st->IO_level &= ~dir;
  st->IO_level |= st->O_level;

  for (int n = __builtin_ffs(diff) - 1; n >= 0; n = __builtin_ffs(diff) - 1) {
    nrf_gpio_output_change_sideeffects(port, n, (new_output >> n) & 0x1);
    diff &= ~(1 << n);
  }

  /* Inputs may be connected to pins driven by outputs, let's check */
  nrf_gpio_eval_inputs(port);
}


/*
 * Register write side-effecting functions:
 */

void nrf_gpio_regw_sideeffects_OUT(unsigned int port) {
  nrf_gpio_eval_outputs(port);
}

void nrf_gpio_regw_sideeffects_OUTSET(unsigned int port) {
  if (NRF_GPIO_regs[port].OUTSET) {
    NRF_GPIO_regs[port].OUT |= NRF_GPIO_regs[port].OUTSET;
    nrf_gpio_eval_outputs(port);
  }
  NRF_GPIO_regs[port].OUTSET = NRF_GPIO_regs[port].OUT;
}

void nrf_gpio_regw_sideeffects_OUTCLR(unsigned int port) {
  if (NRF_GPIO_regs[port].OUTCLR) {
    NRF_GPIO_regs[port].OUT &= ~NRF_GPIO_regs[port].OUTCLR;
    NRF_GPIO_regs[port].OUTCLR = 0;
    nrf_gpio_eval_outputs(port);
  }
}

void nrf_gpio_regw_sideeffects_DIR(unsigned int port) {
  /* Mirror change into PIN_CNF[*].DIR */
  for (int n = 0; n < gpio_st[port].nbr_pins; n++ ) {
    NRF_GPIO_regs[port].PIN_CNF[n] &= ~GPIO_PIN_CNF_DIR_Msk;
    NRF_GPIO_regs[port].PIN_CNF[n] |= (NRF_GPIO_regs[port].DIR >> n) & 0x1;
  }

  nrf_gpio_eval_outputs(port);
}

void nrf_gpio_regw_sideeffects_DIRSET(unsigned int port) {
  if (NRF_GPIO_regs[port].DIRSET) {
    NRF_GPIO_regs[port].DIR |= NRF_GPIO_regs[port].DIRSET;
    nrf_gpio_regw_sideeffects_DIR(port);
  }
  NRF_GPIO_regs[port].DIRSET = NRF_GPIO_regs[port].DIR;
}

void nrf_gpio_regw_sideeffects_DIRCLR(unsigned int port) {
  if (NRF_GPIO_regs[port].DIRCLR) {
    NRF_GPIO_regs[port].DIR &= ~NRF_GPIO_regs[port].DIRCLR;
    NRF_GPIO_regs[port].DIRCLR = 0;
    nrf_gpio_regw_sideeffects_DIR(port);
  }
}

void nrf_gpio_regw_sideeffects_LATCH(unsigned int port) {
  struct gpio_status *st = &gpio_st[port];

  if (!st->has_sense) {
    return;
  }

  /* LATCH contains what SW wrote: */
  uint32_t sw_input = NRF_GPIO_regs[port].LATCH;

  /* Whatever bits SW set to 1, it is trying to clear: */
  st->LDETECT &= ~sw_input;

  /* But where the sense output is high, the bits are kept high: */
  st->LDETECT |= st->DETECT;

  NRF_GPIO_regs[port].LATCH = st->LDETECT;

  nrf_gpio_update_detect_signal(port);

  /*
   * Note the text from the spec:
   *   If one or more bits in the LATCH register are '1' after the CPU has
   *   performed a clear operation on the LATCH registers, a rising edge will be generated
   *   on the LDETECT signal.
   * "the CPU has performed a clear operation" == after writing LATCH with any bit to 1
   */
  if (sw_input != 0 && st->LDETECT != 0 && NRF_GPIO_regs[port].DETECTMODE == 1 && (st->partner_GPIOTE>=0)) {
    nrf_gpiote_port_detect_raise(st->partner_GPIOTE, port);
  }

#if defined(NRF5340)
  if ((port == NHW_GPIO_APP_P0) || (port == NHW_GPIO_APP_P1)) {
    if ((sw_input != 0) && (st->LDETECT != 0) && (NRF_GPIO_regs[port].DETECTMODE_SEC == 1)) {
      nrf_gpiote_port_detect_raise(NHW_GPIOTE_APP0, port);
    }
  }
#endif
}

/* For 5340: To be called for either DETECTMODE or DETECTMODE_SEC */
void nrf_gpio_regw_sideeffects_DETECTMODE(unsigned int port) {
  nrf_gpio_eval_sense(port);
}

void nrf_gpio_regw_sideeffects_PIN_CNF(unsigned int port, unsigned int n) {

  struct gpio_status *st = &gpio_st[port];

  bool need_output_eval = false;
  bool need_input_eval = false;
  bool need_sense_eval = false;

  int dir = NRF_GPIO_regs[port].PIN_CNF[n] & GPIO_PIN_CNF_DIR_Msk;

  if (dir != ((NRF_GPIO_regs[port].DIR >> n) & 0x1)) {
    NRF_GPIO_regs[port].DIR ^= 1 << n;
    need_output_eval = true;
  }

  /* Note: DRIVE and PULL are not yet used in this model
	int pull = (NRF_GPIO_regs[port].PIN_CNF[n] & GPIO_PIN_CNF_PULL_Msk)
					>> GPIO_PIN_CNF_PULL_Pos;

	int drive = (NRF_GPIO_regs[port].PIN_CNF[n] & GPIO_PIN_CNF_DRIVE_Msk)
					>> GPIO_PIN_CNF_DRIVE_Pos;
   */

  int input = (NRF_GPIO_regs[port].PIN_CNF[n] & GPIO_PIN_CNF_INPUT_Msk)
			    >> GPIO_PIN_CNF_INPUT_Pos;
  if (input != ((st->INPUT_mask >> n) & 0x1)) {
    st->INPUT_mask ^= 1 << n;
    need_input_eval = true;
  }

  int sense = (NRF_GPIO_regs[port].PIN_CNF[n] & GPIO_PIN_CNF_SENSE_Msk)
			    >> GPIO_PIN_CNF_SENSE_Pos;
  if (((sense >> 1) & 0x1) != ((st->SENSE_mask >> n) & 0x1)) {
    st->SENSE_mask ^= 1 << n;
    need_sense_eval = true;
  }
  if ((sense & 0x1) != ((st->SENSE_inv >> n) & 0x1)) {
    st->SENSE_inv ^= 1 << n;
    need_sense_eval = true;
  }

  if (need_output_eval) {
    nrf_gpio_eval_outputs(port);
  }
  if (need_input_eval) {
    nrf_gpio_eval_inputs(port);
  }
  if (need_sense_eval) {
    nrf_gpio_eval_sense(port);
  }
}
