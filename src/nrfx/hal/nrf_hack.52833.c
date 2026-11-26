/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "nrfx.h"
#include "hal/nrf_ccm.h"
#include "hal/nrf_rtc.h"
#include "hal/nrf_gpio.h"
#include "hal/nrf_gpiote.h"
#include "hal/nrf_temp.h"
#include "hal/nrf_radio.h"
#include "hal/nrf_egu.h"
#include "hal/nrf_timer.h"
#include "hal/nrf_clock.h"
#include "hal/nrf_pwm.h"
#include "hal/nrf_rng.h"
#include "hal/nrf_aar.h"
#include "hal/nrf_ppi.h"
#include "hal/nrf_ecb.h"
#include "hal/nrf_uart.h"
#include "hal/nrf_uarte.h"

#include "nrf_hack_int.h"

void nrf_hack_get_task_from_ptr(void *task_reg,
                                void **p_reg,
                                subscribe_set_f *set_f,
                                subscribe_clear_f *clear_f,
                                task_trigger_f *trigger_f,
                                int *task)
{
  IF_PER(CLOCK, , , clock)
  } else IF_PER(RADIO, , , radio)
  } else IF_PER(UART, 0, , uart)
  } else IF_PER(UARTE, 0, , uarte)
  } else IF_PER(UARTE, 1, , uarte)
  } else IF_PER(RNG, , , rng)
  } else IF_PER(GPIOTE, , , gpiote)
  } else IF_PER(TIMER, 0, , timer)
  } else IF_PER(TIMER, 1, , timer)
  } else IF_PER(TIMER, 2, , timer)
  } else IF_PER(TIMER, 3, , timer)
  } else IF_PER(TIMER, 4, , timer)
  } else IF_PER(PWM, 0, , pwm)
  } else IF_PER(PWM, 1, , pwm)
  } else IF_PER(PWM, 2, , pwm)
  } else IF_PER(PWM, 3, , pwm)
  } else IF_PER(ECB, , , ecb)
  } else IF_PER(AAR, , , aar)
  } else IF_PER(CCM, , , ccm)
  } else IF_PER(PPI, , , ppi)
  } else IF_PER(EGU, 0, , egu)
  } else IF_PER(EGU, 1, , egu)
  } else IF_PER(EGU, 2, , egu)
  } else IF_PER(EGU, 3, , egu)
  } else IF_PER(EGU, 4, , egu)
  } else IF_PER(EGU, 5, , egu)
  } else IF_PER(RTC, 0, , rtc)
  } else IF_PER(RTC, 1, , rtc)
  } else IF_PER(RTC, 2, , rtc)
  } else IF_PER(TEMP, , , temp)
  } else {
    NOT_KNOWN_TASK_ERROR;
    return; /* unreachable */
  }
}
