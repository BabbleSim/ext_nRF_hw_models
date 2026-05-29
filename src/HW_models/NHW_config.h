/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file configures the HW models based on the variant being built
 */
#ifndef _NRF_HW_CONFIG_H
#define _NRF_HW_CONFIG_H

/*
 * For each peripheral the following may be defined
 * NHW_<PERIPH_TYPE>_TOTAL_INST <val> : Total number of instances of the peripheral in the whole SOC
 * NHW_<PERIPH_TYPE>_<CORE>_<INST> <val> : Index of that peripheral instant in list of peripherals
 *
 * NHW_<PERIPH_TYPE>_INT_MAP : List of irq controller mapping, for each instante: {irq ctrl number, irq line}
 */

#if defined(NRF52833) || defined(NRF52833_XXAA)
#include "NHW_config.52833.h"

#elif defined(NRF5340) || defined(NRF5340_XXAA_NETWORK) || defined(NRF5340_XXAA_APPLICATION)
#include "NHW_config.5340.h"

#elif defined(NRF54L15) || defined(NRF54L15_XXAA)
#include "NHW_config.54L15.h"

#elif defined(NRF54LS05) || defined(NRF54LS05A_XXAA)
#include "NHW_config.54LS05.h"

#elif defined(NRF54LM20) || defined(NRF54LM20A_XXAA) || defined(NRF54LM20A_ENGA_XXAA)
#include "NHW_config.54LM20.h"

#else
#error "No valid platform was selected"
#endif

#endif /* _NRF_HW_CONFIG_H */
