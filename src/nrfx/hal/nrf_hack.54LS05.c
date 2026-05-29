/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "nrfx.h"
#include "hal/nrf_aar.h"
#include "hal/nrf_ccm.h"
#include "hal/nrf_clock.h"
#include "hal/nrf_ecb.h"
#include "hal/nrf_egu.h"
#include "hal/nrf_dppi.h"
#include "hal/nrf_gpiote.h"
#include "hal/nrf_grtc.h"
#include "hal/nrf_ppib.h"
#include "hal/nrf_pwm.h"
#include "hal/nrf_radio.h"
#include "hal/nrf_timer.h"
#include "hal/nrf_temp.h"
#include "hal/nrf_uarte.h"

#include "nrf_hack_int.h"

void nrf_hack_get_task_from_ptr(void *task_reg,
                                void **p_reg,
                                subscribe_set_f *set_f,
                                subscribe_clear_f *clear_f,
                                task_trigger_f *trigger_f,
                                int *task)
{
  IF_PER(DPPIC, 00, , dppi)
  //} else IF_PER(PPIB, 00, , ppib)
  //} else IF_PER(PPIB, 01, , ppib)
  } else IF_PER(AAR, 00, , aar)
  } else IF_PER(CCM, 00, , ccm)
  } else IF_PER(ECB, 00, , ecb)
  //} else IF_PER(RRAMC, , , rramc)
  //} else IF_PER(CTRLAP, , , ctrlap)
  } else IF_PER(TIMER, 00, , timer)
  } else IF_PER(EGU, 00, , egu)
  //} else IF_PER(CRACEN, , , cracen) //No tasks
  } else IF_PER(DPPIC, 10, , dppi)
  //} else IF_PER(PPIB, 10, , ppib)
  //} else IF_PER(PPIB, 11, , ppib)
  } else IF_PER(TIMER, 10, , timer)
  } else IF_PER(EGU, 10, , egu)
  } else IF_PER(RADIO, , , radio)
  } else IF_PER(DPPIC, 20, , dppi)
  //} else IF_PER(PPIB, 20, , ppib)
  //} else IF_PER(PPIB, 21, , ppib)
  //} else IF_PER(PPIB, 22, , ppib)
  //} else IF_PER(SPIM, 20, , spi)
  //} else IF_PER(TWIM, 20, , twi)
  } else IF_PER(UARTE, 20, , uarte)
  //} else IF_PER(SPIM, 21, , spi)
  //} else IF_PER(TWIM, 21, , twi)
  } else IF_PER(UARTE, 21, , uarte)
  //} else IF_PER(SPIM, 22, , spi)
  //} else IF_PER(TWIM, 22, , twi)
  } else IF_PER(UARTE, 22, , uarte)
  } else IF_PER(EGU, 20, , egu)
  } else IF_PER(TIMER, 20, , timer)
  //} else IF_PER(MEMCONF, , , memconf)
  //} else IF_PER(PWM, 20, , pwm)
  //} else IF_PER(SAADC, , , saadc)
  } else IF_PER(TEMP, , , temp)
  } else IF_PER(GPIOTE, 20, , gpiote)
  //} else IF_PER(QDEC, 20, , qdec)
  } else IF_PER(GRTC, , , grtc)
  //} else IF_PER(TAMPC, , , tamp)
  } else IF_PER(DPPIC, 30, , dppi)
  //} else IF_PER(PPIB, 30, , ppib)
  //} else IF_PER(WDT, 30, , wdt)
  } else IF_PER(GPIOTE, 30, , gpiote)
  } else IF_PER(CLOCK, , , clock)
  //} else IF_PER(POWER, , , power)
  //} else IF_PER(RESET, , , reset)
  //} else IF_PER(OSCILLARTORS, , , oscillators)
  //} else IF_PER(REGULATORS, , , regulators)
  } else {
    NOT_KNOWN_TASK_ERROR;
    return; /* unreachable */
  }
}
