/*
 *  Copyright (c) 2018, The OpenThread Authors.
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
 *   This file implements the OpenThread platform abstraction for SPI slave communication.
 *
 */

#include <openthread/platform/spi-slave.h>

#include "em_gpio.h"
#include "em_core.h"
#include "spidrv.h"
#include "gpiointerrupt.h"

static SPIDRV_HandleData_t sHandleData;
static SPIDRV_Handle_t sHandle = &sHandleData;
static SPIDRV_Init_t sInitData = SPIDRV_SLAVE_USART0;

static void *sContext = NULL;
static otPlatSpiSlaveTransactionCompleteCallback sTransactionCompleteCallback = NULL;
static otPlatSpiSlaveTransactionProcessCallback sTransactionProcessCallback = NULL;

enum
{
    DISABLE,
    IDLE,
    BUSY,
} spi_slave_state_t;

static typedef struct spi_slave_context_s {
    uint8_t *sInputBuf;
    uint16_t sInputBufLen;
    uint8_t *sOutputBuf;
    uint16_t sOutputBufLen;
    spi_slave_state_t state;
    uint16_t sRxCount;
    uint16_t sTxCount;
} spi_slave_context_t;

static spi_slave_context_t ssContext = {
    NULL,
    0,
    NULL,
    0,
    DISABLE,
    0,
    0,
};

static void transmitComplete(SPIDRV_Handle_t aHandle, Ecode_t aTransferStatus,
                             int aItemsTransferred)
{
    (void)aHandle;
    assert(aTransferStatus == ECODE_EMDRV_SPIDRV_OK);
    ssContext.sTxCount = aItemsTransferred;
}

static void receiveComplete(SPIDRV_Handle_t aHandle, Ecode_t aTransferStatus,
                            int aItemsTransferred)
{
    (void)aHandle;
    assert(aTransferStatus == ECODE_EMDRV_SPIDRV_OK);
    ssContext.sRxCount = aItemsTransferred;
}

static otError returnTypeConvert(uint32_t aStatus)
{
    otError error = OT_ERROR_NONE;
    
    switch (aStatus)
    {
        case ECODE_EMDRV_SPIDRV_OK:
            error = OT_ERROR_NONE;
            break;

        case ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE:
            error = OT_ERROR_INVALID_ARGS;
            break;
        
        case ECODE_EMDRV_SPIDRV_PARAM_ERROR:
            error = OT_ERROR_INVALID_ARGS;
            break;
        
        case ECODE_EMDRV_SPIDRV_DMA_ALLOC_ERROR:
        default:
            error = OT_ERROR_FAILED;
    }
    
    return error;
}

static void gpioInterruptHandler(uint8_t aPin)
{
    uint32_t retval;
    uint16_t rx_count = ssContext.RxCount;
    uint16_t tx_count = ssContext.TxCount;
    uint8_t *rx_buf = ssContext.sInputBuf;
    uint16_t rx_buf_len = ssContext.sInputBufLen;
    uint8_t *tx_buf = ssContext.sOutputBuf;
    uint16_t tx_buf_len = ssContext.sOutputBufLen;
    bool shouldProcess = false;

    if (GPIO_PinInGet(port, aPin) == 1)
    {
        otEXPECT(ssContext.state == BUSY);

        if (controller->callback)
        {
            shouldProcess = sTransactionCompleteCallback(sContext, tx_buf, tx_buf_len, rx_buf, rx_buf_len,
                                                         (tx_count - rx_count) ? tx_count: rx_count);
            if (shouldProcess)
            {
                sTransactionProcessCallback(sContext);
            }

            // reset context
            ssContext.sInputBuf = NULL;
            ssContext.sInputBufLen = 0;
            ssContext.sOutputBuf = NULL;
            ssContext.sOutputBufLen = 0;
        }

        ssContext.state = IDLE;
    }
    else
    {
        ssContext.state = BUSY;
        retval = SPIDRV_STransmit(sHandle, aInputBuf, aInputBufLen, transmitComplete, 0);
        otEXPECT_ACTION(returnTypeConvert(retval) == OT_ERROR_NONE, error = OT_ERROR_FAILED);
    }

exit:
    return;
}

otError otPlatSpiSlaveEnable(otPlatSpiSlaveTransactionCompleteCallback aCompleteCallback,
                             otPlatSpiSlaveTransactionProcessCallback aProcessCallback, void *aContext)
{
    otError error = OT_ERROR_NONE;
    uint32_t retval;

    otEXPECT_ACTION(ssContext.state == DISABLE, error = OT_ERROR_INVALID_ARGS);
    otEXPECT_ACTION(aCompleteCallback != NULL, error = OT_ERROR_INVALID_ARGS);
    otEXPECT_ACTION(aProcessCallback != NULL, error = OT_ERROR_INVALID_ARGS);

    sTransactionCompleteCallback = aCompleteCallback;
    sTransactionProcessCallback = aProcessCallback;
    sContext = aContext;

    retval = SPIDRV_Init(sHandle, &sInitData);
    otEXPECT_ACTION(returnTypeConvert(retval) == OT_ERROR_NONE, error = OT_ERROR_FAILED);

    // configure the CS pin interrupt
    CMU_ClockEnable(cmuClock_GPIO, true);
    GPIOINT_Init();

    GPIO_ExtIntConfig(PORT, PIN, PIN, true, true, true);
    GPIOINT_CallbackRegister(1<<_USART_ROUTELOC0_CSLOC_LOC1, gpioInterruptHandler);

    ssContext.state = IDLE;

exit:
    return error;
}

void otPlatSpiSlaveDisable(void)
{
    asset(ssContext.state != DISBALE);

    SPIDRV_DeInit(sHandle);
    ssContext.state = DISABLE;
}

otError otPlatSpiSlavePrepareTransaction(uint8_t *aOutputBuf, uint16_t aOutputBufLen, uint8_t *aInputBuf,
                                         uint16_t aInputBufLen, bool aRequestTransactionFlag)
{
    otError error = OT_ERROR_NONE;
    uint32_t retval;

    otEXPECT_ACTION(sHandle->state == spidrvStateIdle, error = OT_ERROR_BUSY);

    if (aOutputBuf != NULL)
    {
        // transmit should wait until cs pin actives
        ssContext.sOutputBuf = aOutputBuf;
        ssContext.sOutputBufLen = aOutputBufLen;
    }

    if (aInputBuf != NULL)
    {
        // receive
        ssContext.sInputBuf = aInputBuf;
        ssContext.sInputBufLen = aInputBufLen;

        retval = SPIDRV_SReceive(sHandle, aInputBuf, aInputBufLen, receiveComplete, 0);
        otEXPECT_ACTION(returnTypeConvert(retval) == OT_ERROR_NONE, error = OT_ERROR_FAILED);
    }

    // set host interrupt pin
    GPIO_PinModeSet();
    GPIO_PinOutSet(pin, aRequestTransactionFlag ? 0 : 1);

exit:
    return error;
}
