/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_GPIO_BACKEND_H
#define _NRF_HW_MODEL_GPIO_BACKEND_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

/*
 * Overall GPIO_backend component interface towards the top of the GPIO HW model
 */
void nrf_gpio_backend_init(void);
void nrf_gpio_backend_short_propagate(unsigned int port, unsigned int n, bool value);
void nrf_gpio_backend_change_output(unsigned int port, unsigned int n, bool value);

void nrf_gpio_backend_register_short(uint8_t X, uint8_t x, uint8_t Y, uint8_t y);

/*
 * Interface between individual backends and the overall GPIO_backend component
 */

/*
 * Callback interface for the GPIO backends.
 * Note backends are not required to register any one of these,
 * or at all if they do not need any of these callbacks.
 */
struct nrf_gpio_backend_if {
 /* Will be called during nrf_gpio_init()->nrf_gpio_backend_init() */
 void (*init)(void *st);
 /* Will be called each time a GPIO output is changed */
 void (*change_output)(void *st, unsigned int port, unsigned int n, bool value);
 /* Will be called during nrf_gpio_backend_cleaup() */
 void (*cleanup)(void *st);
};

/**
 * Register a backend instance.
 * Note this must be called before the gpio initialization or at the latest during the
 * gpio configuration file parsing in nrf_gpio_load_config().
 * The st pointer will be just passed back to all calls to the backend
 */
void nrf_gpio_backend_register(const struct nrf_gpio_backend_if *backend_callbacks, void *st);


/*
 * Interface between the GPIO file backend and the GPIO_backend component
 */

int nhw_gpio_filebackend_process_config(char *path);

/*
 * Interface between the GPIO fifo backend and the GPIO_backend component
 */
int nhw_gpio_fifobackend_process_config(char *buf, char *line);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_GPIO_BACKEND_H */
