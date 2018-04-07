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
 * @file:This file implements the OpenThread platform abstraction for GPIO.
 *
 */

#include <openthread/types.h>
#include <openthread/platform/gpio.h>

#include "hal/nrf_gpio.h"

void otPlatGpioInit(void)
{
    // configure GPIO mode: output
    nrf_gpio_cfg_output(RED_LED_PIN);
    nrf_gpio_cfg_output(GREEN_LED_PIN);
    nrf_gpio_cfg_output(BLUE_LED_PIN);

    // clear all output first
    nrf_gpio_pin_write(RED_LED_PIN, GPIO_LOGIC_LOW);
    nrf_gpio_pin_write(GREEN_LED_PIN, GPIO_LOGIC_LOW);
    nrf_gpio_pin_write(BLUE_LED_PIN, GPIO_LOGIC_LOW);
}

void otPlatGpioOutSet(uint32_t port, uint8_t pin)
{
    (void)port;

    nrf_gpio_pin_write(pin, GPIO_LOGIC_HIGH);
}

void otPlatGpioOutClear(uint32_t port, uint8_t pin)
{
    (void)port;

    nrf_gpio_pin_write(pin, GPIO_LOGIC_LOW);
}

void otPlatGpioOutToggle(uint32_t port, uint8_t pin)
{
    (void)port;

    nrf_gpio_pin_toggle((uint32_t)pin);
}

uint8_t otPlatGpioOutGet(uint32_t port, uint8_t pin)
{
    (void)port;
    uint8_t rval = nrf_gpio_pin_out_read((uint32_t)pin);

#if (OPENTHREAD_GPIO_LOGIC_LEVEL == 1)
    return rval > 0 ? GPIO_LOGIC_HIGH : GPIO_LOGIC_LOW;
#else
    return rval > 0 ? GPIO_LOGIC_LOW : GPIO_LOGIC_HIGH;
#endif
}
