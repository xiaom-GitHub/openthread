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
 *   This file includes the OpenThread platform abstraction for USB CDC implementation.
 *
 */

#ifndef USBCDC_H_
#define USBCDC_H_

#include <CDC_Interface.h>
#include <USB_Interface.h>
#include <openthread-types.h>

extern void cdcReceiveDone(void);
extern void cdcTransmitDone(const uint8_t *aBuf);

/**
 * Enable USB communication device class(CDC) module.
 *
 */
ThreadError kw2xUsbCdcEnable(void);

/**
 * Disable USB communication device class(CDC) module.
 *
 */
ThreadError kw2xUsbCdcDisable(void);

/**
 * Transmit aBufLength bytes of data from aBuf over the virtual com port.
 *
 * @param[in]  aBuf        A pointer to the data buffer.
 * @param[in]  aBufLength  Number of bytes to transmit.
 */
void kw2xUsbCdcTransmit(const uint8_t *aBuf, uint16_t aBufLength);

#endif // USBCDC_H
