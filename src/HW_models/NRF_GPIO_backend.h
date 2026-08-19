/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * Copyright (c) 2026 Demant A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_GPIO_BACKEND_H
#define _NRF_HW_MODEL_GPIO_BACKEND_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct nrf_gpio_backend_if {
	void (*init)(void);
	void (*change_output)(unsigned int port, unsigned int n, bool value);
};

void nrf_gpio_backend_register(const struct nrf_gpio_backend_if *backend);
void nrf_gpio_backend_init(void);
void nrf_gpio_backend_short_propagate(unsigned int port, unsigned int n, bool value);
void nrf_gpio_backend_change_output(unsigned int port, unsigned int n, bool value);

void nrf_gpio_backend_register_short(uint8_t X, uint8_t x, uint8_t Y, uint8_t y);

void nrf_gpio_backend_file_add_instance(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_GPIO_BACKEND_H */
