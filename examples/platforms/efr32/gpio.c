/*
 *  Copyright (c) 2017, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for GPIO.
 *
 */

#ifdef OPENTHREAD_CONFIG_FILE
#include OPENTHREAD_CONFIG_FILE
#else
#include <openthread-config-generic.h>
#endif

#include <openthread/types.h>
#include <openthread/platform/gpio.h>

#include "em_gpio.h"
#include "em_cmu.h"

#ifdef OPENTHREAD_ENABLE_JOINER
#include "platform-efr32.h"
#include "bspconfig.h"
#include "gpiointerrupt.h"
#include <openthread/instance.h>

void gpioCallback(uint8_t pin);
#endif

void otPlatGpioInit(void)
{
    // enable GPIO clock first if not
    CMU_ClockEnable(cmuClock_GPIO, true);

    // configure the gpio mode: output or input
    GPIO_PinModeSet(gpioPortD, RED_LED_PIN, gpioModePushPull, GPIO_LOGIC_LOW);
    GPIO_PinModeSet(gpioPortD, GREEN_LED_PIN, gpioModePushPull, GPIO_LOGIC_LOW);
    GPIO_PinModeSet(gpioPortD, BLUE_LED_PIN, gpioModePushPull, GPIO_LOGIC_LOW);

#ifdef OPENTHREAD_ENABLE_JOINER
    // configure PF6 as button interrupt
    GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);

    GPIOINT_Init();
    GPIOINT_CallbackRegister(BSP_GPIO_PB0_PIN, gpioCallback);
    GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);
#endif
}

#ifdef OPENTHREAD_ENABLE_JOINER
void gpioCallback(uint8_t pin)
{
    // factoryreset
    otInstanceFactoryReset(sInstance);
}
#endif

void otPlatGpioOutSet(uint32_t port, uint8_t pin)
{
    // must configure mode first

#if (OPENTHREAD_GPIO_LOGIC_LEVEL == 1)
    GPIO_PinOutSet(port, pin);
#else
    GPIO_PinOutClear(port, pin);
#endif
}

void otPlatGpioOutClear(uint32_t port, uint8_t pin)
{
    // must configure mode first

#if (OPENTHREAD_GPIO_LOGIC_LEVEL == 1)
    GPIO_PinOutClear(port, pin);
#else
    GPIO_PinOutSet(port, pin);
#endif
}

void otPlatGpioOutToggle(uint32_t port, uint8_t pin)
{
    // must configure mode filst
    GPIO_PinOutToggle(port, pin);
}

uint8_t otPlatGpioOutGet(uint32_t port, uint8_t pin)
{
    return GPIO_PinOutGet(port, pin);
}

void otPlatGpioIntEnable(uint32_t port, uint8_t pin)
{
    // configure PF6 as button interrupt
    GPIO_PinModeSet(port, pin, gpioModeInputPull, 1);
    
    GPIOINT_Init();
    //GPIOINT_CallbackRegister(pin, gpioCallback);
    GPIO_IntConfig(port, pin, false, true, true);
}

void otPlatGpioRegisterCallback(uint32_t port, otPlatGpioIntCallback aCallback)
{
    (void)port;
    GPIOINT_CallbackRegister(BSP_GPIO_PB0_PIN, aCallback);
}

void otPlatGpioIntClear(uint32_t port, uint8_t pin)
{
    (void)port;
    GPIOINT_CallbackUnRegister(pin);
}
