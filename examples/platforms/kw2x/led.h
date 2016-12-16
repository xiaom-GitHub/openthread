/*
 *  Copyright (c) 2016, The OpenThread Authors.
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
 *   This file includes the OpenThread platform abstraction for LED.
 *
 */

#ifndef LED_H_
#define LED_H_

#include <Led.h>
#include <openthread-types.h>

enum
{
    LED_1 = 0x01,
    LED_2 = 0x02,
    LED_3 = 0x04,
    LED_4 = 0x08,
};

/**
 * Initialize the LED module.
 *
 */
void kw2xLedInit(void);

/**
 * Turns off all the LEDs and disables clock gating for LED port.
 *
 */
void kw2xLedUninit(void);

/**
 * Toggle the specified LED(s).
 *
 * @param[in]  aLEDNr  LED number(s) to be toggled.
 */
void kw2xLedToggle(uint8_t aLEDNr);

/**
 * Turn on the specified LED(s).
 *
 * @param[in]  aLEDNr  LED number(s) to be turned on.
 */
void kw2xLedTurnOn(uint8_t aLEDNr);

/**
 * Turn off the specified LED(s).
 *
 * @param[in]  aLEDNr  LED number(s) to be turned off.
 */
void kw2xLedTurnOff(uint8_t aLEDNr);

#endif // LED_H
