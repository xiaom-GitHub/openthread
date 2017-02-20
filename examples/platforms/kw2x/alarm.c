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
 *   This file implements the OpenThread platform abstraction for the alarm.
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "platform-kw2x.h"
#include "atomic.h"
#include "led.h"

#include <platform/alarm.h>
#include <Interrupt.h>

enum
{
    MAX_DELAY = 32768,
};

static void kw2xSetAlarm(void);
static void kw2xSetHardwareAlarm(uint16_t t0, uint16_t dt);

static uint16_t sTimerHi = 0;
static uint16_t sTimerLo = 0;
static uint32_t sAlarmT0 = 0;
static uint32_t sAlarmDt = 0;
static bool sIsRunning = false;
static bool sIsFired = false;

void delayMs(uint16_t val)
{
    uint32_t now = otPlatAlarmGetNow();

    while (otPlatAlarmGetNow() - now <= val) {}
}

void kw2xAlarmInit(void)
{
    /* turn on the LPTMR clock */
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;
    /* disable LPTMR */
    LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;
    /* 1ms tick period */
    LPTMR0_PSR = (LPTMR_PSR_PBYP_MASK | LPTMR_PSR_PCS(1));
    /* enable LPTMR IRQ */
    NVICICPR1 = 1 << 26;
    NVICISER1 = 1 << 26;

    LPTMR0_CMR = MAX_DELAY;

    sTimerHi = 0;
    sTimerLo = 0;
    sIsRunning = false;

    /* enable LPTMR */
    LPTMR0_CSR = LPTMR_CSR_TCF_MASK | LPTMR_CSR_TIE_MASK | LPTMR_CSR_TFC_MASK;
    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;
}

uint32_t otPlatAlarmGetNow(void)
{
    uint32_t intState = otPlatAtomicBegin();
    uint16_t timerLo;

    LPTMR0_CNR = LPTMR0_CNR;
    timerLo = LPTMR0_CNR;

    if (timerLo < sTimerLo)
    {
        sTimerHi++;
    }

    sTimerLo = timerLo;
    otPlatAtomicEnd(intState);

    return ((uint32_t)sTimerHi << 16) | sTimerLo;
}

void kw2xAlarmProcess(otInstance *aInstance)
{
    if (sIsFired)
    {
        otPlatAlarmFired(aInstance);
        sIsFired = false;
    }
}

void kw2xSetAlarm(void)
{
    uint32_t now = otPlatAlarmGetNow();
    uint32_t remaining = MAX_DELAY;
    uint32_t expires;

    if (sIsRunning)
    {
        expires = sAlarmT0 + sAlarmDt;
        remaining = expires - now;

        if (sAlarmT0 <= now)
        {
            if (expires >= sAlarmT0 && expires <= now)
            {
                remaining = 0;
            }
        }
        else
        {
            if (expires >= sAlarmT0 || expires <= now)
            {
                remaining = 0;
            }
        }

        if (remaining > MAX_DELAY)
        {
            sAlarmT0 = now + MAX_DELAY;
            sAlarmDt = remaining - MAX_DELAY;
            remaining = MAX_DELAY;
        }
        else
        {
            sAlarmT0 += sAlarmDt;
            sAlarmDt = 0;
        }
    }

    kw2xSetHardwareAlarm(now, remaining);
}

void kw2xSetHardwareAlarm(uint16_t t0, uint16_t dt)
{
    uint32_t intState = otPlatAtomicBegin();
    uint16_t now, elapsed;
    uint16_t remaining;

    LPTMR0_CNR = LPTMR0_CNR;
    now = LPTMR0_CNR;
    elapsed = now - t0;

    if (elapsed >= dt)
    {
        LPTMR0_CMR = now + 2;
    }
    else
    {
        remaining = dt - elapsed;

        if (remaining <= 2)
        {
            LPTMR0_CMR = now + 2;
        }
        else
        {
            LPTMR0_CMR = now + remaining;
        }
    }

    LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;
    otPlatAtomicEnd(intState);
}

void otPlatAlarmStartAt(otInstance *aInstance, uint32_t t0, uint32_t dt)
{
    uint32_t intState = otPlatAtomicBegin();
    sInstance = aInstance;

    sAlarmT0 = t0;
    sAlarmDt = dt;
    sIsRunning = true;
    kw2xSetAlarm();
    otPlatAtomicEnd(intState);
}

void otPlatAlarmStop(otInstance *aInstance)
{
    (void)aInstance;

    if (sIsRunning)
    {
        sIsRunning = false;
        kw2xSetAlarm();
    }
}

void LPTMR_IrqHandler()
{
    uint32_t intState = otPlatAtomicBegin();

    LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;
    otPlatAlarmGetNow();

    if (sIsRunning && sAlarmDt == 0)
    {
        sIsRunning = false;
        sIsFired = true;
    }
    else
    {
        kw2xSetAlarm();
    }

    // toggle LED
    kw2xLedToggle(LED_2);

    otPlatAtomicEnd(intState);
}
