/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file specifies a set of interfaces meant to allow tests
 * to modify the HW behavior.
 * This APIs should only be used for test purposes, to facilitate testing.
 *
 * This API is specified in a relatively HW agnostic way, so as to facilitate
 * other vendors providing an implementation for it, but
 * it should be used with extra care as the functionality is likely to not be
 * provided by other build targets than the ones based on this HW models.
 */

#ifndef _NRF_HWMODELS_HW_TESTCHEAT_IF_H
#define _NRF_HWMODELS_HW_TESTCHEAT_IF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * Offset the Tx power by <power_offset_i> dBs
 *
 * For example, set to -20 to lower the Tx power by 20dBs compared
 * to what it would have been otherwise.
 */
void hw_radio_testcheat_set_tx_power_gain(double power_offset);

/**
 * Offset the measured Rx power (RSSI) by <power_offset_i> dBs
 *
 * For example, set to 20 to raise the Rx power by 20dBs compared
 * to what it would have been otherwise.
 */
void hw_radio_testcheat_set_rx_power_gain(double power_offset);

/**
 * Enable/disable the RADIO Tx at the lowest level for a number of packets.
 * Even though from the SW point of view it will appear as if packets are sent,
 * after calling this function with <count> set, <count> packets won't be sent out.
 * Calling it again with <count> set to 0 will re-enable the radio for the following packets.
 *
 * If you want it to keep failing until stopped, set <count> to -1.
 */
void hw_radio_testcheat_disable_tx(int64_t count);

/**
 * Enable/disable the RADIO Rx at the lowest level for a number of packets.
 * If called with <count_dont_sync> set, the following <count_dont_sync> receptions will be
 * configured with a bogus address, and therefore nothing will be received.
 * If called with <count_fail_CRC> set, the following <count_fail_CRC> receptions which otherwise
 * would have had a correct CRC, will be have their CRC fail.
 * Call it setting both to 0 to restore normal behavior.
 *
 * If you want it to keep failing until stopped, set the corresponding count_ to -1.
 */
void hw_radio_testcheat_disable_rx(int64_t count_dont_sync, int64_t count_fail_crc);

/*
 * Cheat interface to adjust the time in microseconds it takes
 * for a 16byte AES ECB block to be computed
 */
void nhw_aes_ecb_cheat_set_t_ecb(unsigned int new_t);
/*
 * Cheat interface to reset the time it takes
 * for a 16byte AES ECB block to be computed
 * to the value specified in the infocenter spec.
 */
void nhw_aes_ecb_cheat_reset_t_ecb(void);

#ifdef __cplusplus
}
#endif

#endif
