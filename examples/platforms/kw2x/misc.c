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
 *   This file implements the OpenThread platform abstraction for miscellaneous behaviors.
 */

#include "platform-kw2x.h"
#include <PWR_Interface.h>
#include <openthread-types.h>
#include <platform/misc.h>

void otPlatReset(otInstance *aInstance)
{
    (void)aInstance;

    SCB_AIRCR = (uint32_t)(SCB_AIRCR_VECTKEY(0x5FA) | SCB_AIRCR_SYSRESETREQ_MASK);

    while (1);
}

otPlatResetReason otPlatGetResetReason(otInstance *aInstance)
{
    (void)aInstance;
    otPlatResetReason reason;
    uint16_t status = (uint16_t)(RCM_SRS0) | (uint16_t)(RCM_SRS1 << 8);

    if (status & gPWRLib_ResetStatus_POR)
    {
        reason = kPlatResetReason_PowerOn;
    }
    else if (status & gPWRLib_ResetStatus_SW)
    {
        reason = kPlatResetReason_Software;
    }
    else if (status & gPWRLib_ResetStatus_COP)
    {
        reason = kPlatResetReason_Watchdog;
    }
    else if ((status & gPWRLib_ResetStatus_PIN)     ||
             (status & gPWRLib_ResetStatus_EZPT))
    {
        reason = kPlatResetReason_External;
    }
    else if ((status & gPWRLib_ResetStatus_LOL)     ||
             (status & gPWRLib_ResetStatus_LOC)     ||
             (status & gPWRLib_ResetStatus_SACKERR) ||
             (status & gPWRLib_ResetStatus_LOCKUP))
    {
        reason = kPlatResetReason_Fault;
    }
    else if ((status & gPWRLib_ResetStatus_WAKEUP)  ||
             (status & gPWRLib_ResetStatus_LVD)     ||
             (status & gPWRLib_ResetStatus_LOC)     ||
             (status & gPWRLib_ResetStatus_TAMPER))
    {
        reason = kPlatResetReason_Assert;
    }
    else
    {
        reason = kPlatResetReason_Other;
    }

    return reason;
}
