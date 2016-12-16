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
 *   This file includes the platform-specific initializers.
 *
 */

#ifndef PLATFORM_KW2X_H_
#define PLATFORM_KW2X_H_

#include <stdint.h>

#include <mcg.h>
#include <MK21D5.h>
#include <PortConfig.h>
#include <EmbeddedTypes.h>
#include <openthread-types.h>

// Global OpenThread instance structure
extern otInstance *sInstance;

/**
 * This function initializes the alarm service used by OpenThread.
 *
 */
void kw2xAlarmInit(void);

/**
 * This function performs alarm driver processing.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 *
 */
void kw2xAlarmProcess(otInstance *aInstance);

/**
 * This function initializes the radio service used by OpenThread.
 *
 */
void kw2xRadioInit(void);

/**
 * This function performs radio driver processing.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 *
 */
void kw2xRadioProcess(otInstance *aInstance);

/**
 * This function initializes the random number service used by OpenThread.
 *
 */
void kw2xRandomInit(void);

/**
 * This function initializes the USB CDC class module used by OpenThread.
 *
 */
void kw2xUsbCdcInit(void);

/**
 * This function initializes the LED module used by OpenThread.
 *
 */
void kw2xLedInit(void);

/**
 * This function performs UART driver (virtual com) processing.
 *
 */
void kw2xUartProcess(void);

#endif  // PLATFORM_KW2X_H_
