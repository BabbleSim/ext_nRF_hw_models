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
//#include "hal/nrf_pdm.h" //Pending support in real HAL
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
  /*IF_PER(USBHSCORE, , _S, usbhscore)
  } else IF_PER(SPU, 00, _S, spu)
  } else IF_PER(MPC, 00, _S, mpc)
  } else*/ IF_PER(DPPIC, 00, _S, dppi)
  } else IF_PER_SUBSCRIBE_ONLY(PPIB, 00, _S, ppib)
  } else IF_PER_SUBSCRIBE_ONLY(PPIB, 01, _S, ppib)
  //} else IF_PER(KMU, , _S, kmu)
  } else IF_PER(AAR, 00, _S, aar)
  } else IF_PER(CCM, 00, _S, ccm)
  } else IF_PER(ECB, 00, _S, ecb)
  //} else IF_PER(VPR, 00, _0, vpr)
  //} else IF_PER(SPIM, 00, _S, spi)
  } else IF_PER(UARTE, 00, _S, uarte)
  //} else IF_PER(GLITCHDET, , _S, glitchdet)
  //} else IF_PER(RRAMC, , _S, rramc)
  //} else IF_PER(GPIOHSPADCTRL, , _S, gpiohspadctrl)
  //} else IF_PER(CTRLAP, , _S, ctrlap)
  } else IF_PER(TIMER, 00, _S, timer)
  } else IF_PER(EGU, 00, _S, egu)
  //} else IF_PER(CRACEN, , _S, cracen) //No tasks
  //} else IF_PER(USBHS, , _S, usbhs)
  //} else IF_PER(SPU, 10, _S, spu)
  } else IF_PER(DPPIC, 10, _S, dppi)
  } else IF_PER_SUBSCRIBE_ONLY(PPIB, 10, _S, ppib)
  } else IF_PER_SUBSCRIBE_ONLY(PPIB, 11, _S, ppib)
  } else IF_PER(TIMER, 10, _S, timer)
  } else IF_PER(EGU, 10, _S, egu)
  } else IF_PER(RADIO, , _S, radio)
  //} else IF_PER(SPU, 20, _S, spu)
  } else IF_PER(DPPIC, 20, _S, dppi)
  } else IF_PER_SUBSCRIBE_ONLY(PPIB, 20, _S, ppib)
  } else IF_PER_SUBSCRIBE_ONLY(PPIB, 21, _S, ppib)
  } else IF_PER_SUBSCRIBE_ONLY(PPIB, 22, _S, ppib)
  //} else IF_PER(SPIM, 20, _S, spi)
  //} else IF_PER(TWIM, 20, _S, twi)
  } else IF_PER(UARTE, 20, _S, uarte)
  //} else IF_PER(SPIM, 21, _S, spi)
  //} else IF_PER(TWIM, 21, _S, twi)
  } else IF_PER(UARTE, 21, _S, uarte)
  //} else IF_PER(SPIM, 22, _S, spi)
  //} else IF_PER(TWIM, 22, _S, twi)
  } else IF_PER(UARTE, 22, _S, uarte)
  } else IF_PER(EGU, 20, _S, egu)
  } else IF_PER(TIMER, 20, _S, timer)
  } else IF_PER(TIMER, 21, _S, timer)
  } else IF_PER(TIMER, 22, _S, timer)
  } else IF_PER(TIMER, 23, _S, timer)
  } else IF_PER(TIMER, 24, _S, timer)
  //} else IF_PER(MEMCONF, , _S, memconf)
  //} else IF_PER(PDM, 20, _S, pdm)
  //} else IF_PER(PDM, 21, _S, pdm)
  //} else IF_PER(PWM, 20, _S, pwm)
  //} else IF_PER(PWM, 21, _S, pwm)
  //} else IF_PER(PWM, 22, _S, pwm)
  //} else IF_PER(SAADC, , _S, saadc)
  //} else IF_PER(NFCT, , _S, nfct)
  } else IF_PER(TEMP, , _S, temp)
  } else IF_PER(GPIOTE, 20, _S, gpiote)
  //} else IF_PER(QDEC, 20, _S, qdec)
  //} else IF_PER(QDEC, 21, _S, qdec)
  } else IF_PER(GRTC, , _S, grtc)
  //} else IF_PER(TDM, , _S, tdm)
  //} else IF_PER(SPIM, 23, _S, spi)
  //} else IF_PER(TWIM, 23, _S, twi)
  } else IF_PER(UARTE, 23, _S, uarte)
  //} else IF_PER(SPIM, 24, _S, spi)
  //} else IF_PER(TWIM, 24, _S, twi)
  } else IF_PER(UARTE, 24, _S, uarte)
  //} else IF_PER(TAMPC, , _S, tamp)
  //} else IF_PER(SPU, 30, _S, spu)
  } else IF_PER(DPPIC, 30, _S, dppi)
  } else IF_PER_SUBSCRIBE_ONLY(PPIB, 30, _S, ppib)
  //} else IF_PER(SPIM, 30, _S, spi)
  //} else IF_PER(TWIM, 30, _S, twi)
  } else IF_PER(UARTE, 30, _S, uarte)
  //} else IF_PER(COMP, , _S, comp)
  //} else IF_PER(LPCOMP, , _S, lpcomp)
  //} else IF_PER(WDT, 30, _S, wdt)
  //} else IF_PER(WDT, 31, _S, wdt)
  } else IF_PER(GPIOTE, 30, _S, gpiote)
  } else IF_PER(CLOCK, , _NS, clock)
  //} else IF_PER(POWER, , _NS, power)
  //} else IF_PER(RESET, , _NS, reset)
  //} else IF_PER(OSCILLARTORS, , _NS, oscillators)
  //} else IF_PER(REGULATORS, , _NS, regulators)
  //} else IF_PER(VREGUSB, , _S, vregusb)
  } else {
    NOT_KNOWN_TASK_ERROR;
    return; /* unreachable */
  }
}
