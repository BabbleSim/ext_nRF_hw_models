/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_CRACEN_WRAP_H
#define _NRF_HW_MODEL_CRACEN_WRAP_H

#ifdef __cplusplus
extern "C"{
#endif

/* SW interface */
void nhw_CRACEN_regw_sideeffects_EVENTS_CRYPTOMASTER(void);
void nhw_CRACEN_regw_sideeffects_EVENTS_RNG(void);
void nhw_CRACEN_regw_sideeffects_EVENTS_PKEIKG(void);
void nhw_CRACEN_regw_sideeffects_INTEN(unsigned int inst);
void nhw_CRACEN_regw_sideeffects_INTENSET(unsigned int inst);
void nhw_CRACEN_regw_sideeffects_INTENCLR(unsigned int inst);

/* Interface for the subcomponents */
void nhw_CRACEN_toggle_CRYPTOMASTER_intline(bool level);
void nhw_CRACEN_toggle_RNG_intline(bool level);
void nhw_CRACEN_toggle_PKEIKG_intline(bool level);
void nhw_CRACEN_update_timer(void);

#ifdef __cplusplus
}
#endif

#endif
