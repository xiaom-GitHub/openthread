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
 *   This file implements the OpenThread platform abstraction for UART communication
 *   (virtual com) based on USB CDC.
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "platform-kw2x.h"
#include <common/code_utils.hpp>
#include <platform/uart.h>
#include "usbcdc.h"

enum
{
    kReceiveBufferSize = 128,
};

static void processReceive();
static void processTransmit();

static const uint8_t *sTransmitBuffer = NULL;
static uint16_t sTransmitLength = 0;

typedef struct RecvBuffer
{
    // The data buffer
    uint8_t mBuffer[kReceiveBufferSize];
    // The offset of the first item written to the list.
    uint16_t mHead;
    // The offset of the next item to be written to the list.
    uint16_t mTail;
} RecvBuffer;

static RecvBuffer sReceive;

void cdcReceiveDone(void)
{
    uint8_t new_tail;
    uint8_t byte;
    uint8_t rval = TRUE;

    while (rval)
    {
        rval = CDC_GetByteFromRxBuffer(&byte);

        if (rval == TRUE)
        {
            new_tail = (sReceive.mTail + 1) % kReceiveBufferSize;

            if (new_tail != sReceive.mHead)
            {
                sReceive.mBuffer[sReceive.mTail] = byte;
                sReceive.mTail = new_tail;
            }
        }
    }
}

void cdcTransmitDone(unsigned char const *aBuf)
{
    (void)aBuf;
}

ThreadError otPlatUartEnable(void)
{
    kw2xUsbCdcEnable();

    return kThreadError_None;
}

ThreadError otPlatUartDisable(void)
{
    kw2xUsbCdcDisable();

    return kThreadError_None;
}

ThreadError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    ThreadError error = kThreadError_None;

    VerifyOrExit(sTransmitBuffer == NULL, error = kThreadError_Busy);

    sTransmitBuffer = aBuf;
    sTransmitLength = aBufLength;

exit:
    return error;
}

void processTransmit(void)
{
    VerifyOrExit(sTransmitBuffer != NULL, ;);

    kw2xUsbCdcTransmit(sTransmitBuffer, sTransmitLength);

    sTransmitBuffer = NULL;
    otPlatUartSendDone();

exit:
    return;
}

void kw2xUartProcess(void)
{
    processReceive();
    processTransmit();
}

void processReceive(void)
{
    // Copy tail to prevent multiple reads
    uint16_t tail = sReceive.mTail;

    if (sReceive.mHead > tail)
    {
        otPlatUartReceived(sReceive.mBuffer + sReceive.mHead, kReceiveBufferSize - sReceive.mHead);

        // Reset the buffer mHead back to zero
        sReceive.mHead = 0;
    }

    if (sReceive.mHead != tail)
    {
        otPlatUartReceived(sReceive.mBuffer + sReceive.mHead, tail - sReceive.mHead);

        // Set mHead to the local tail we have cached
        sReceive.mHead = tail;
    }
}
