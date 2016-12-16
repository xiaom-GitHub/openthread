/************************************************************************************
* Source file of the application specific memory system
*
* Copyright (c) 2014, Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of Freescale Semiconductor, Inc. nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************/

#include "EmbeddedTypes.h"
#include "Interrupt.h"
#include "UsbMsgQueue.h"
#include "USB.h"
#include "USB_Configuration.h"

#if gUsbIncluded_d

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/
/* The USB module uses one single pool of messages containing messages used
from the Interrupt handler to signal events to the USB task */

#define mUsbMsgQueueSize (6*gNumUsbControllers_d + gNumUsbEp_d )
/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/
static uint8_t usbMsgQueue_head;
static uint8_t usbMsgQueue_tail;
static uint8_t usbMsgQueue_msgNo;
static usbMsg_t maUsbMsgQueue[mUsbMsgQueueSize];

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/******************************************************************************
* UsbMsgQueueInit
This function suppose to be used in the app init when interrupts are disabled
******************************************************************************/
void UsbMsgQueue_Init(void)
{
    usbMsgQueue_head = usbMsgQueue_tail = usbMsgQueue_msgNo = 0;
}

/******************************************************************************
* UsbMsgQueue_Push
To be used only from the USB interrupt.
******************************************************************************/
usbMsgQueueErr_t UsbMsgQueue_Push(usbMsg_t *pUsbMsg)
{
    if (usbMsgQueue_msgNo == mUsbMsgQueueSize)
    {
        return usbMsgQueueErr_QueueFull;
    }

    maUsbMsgQueue[usbMsgQueue_head] = *pUsbMsg;

    if (++usbMsgQueue_head == mUsbMsgQueueSize)
    {
        usbMsgQueue_head = 0;
    }

    usbMsgQueue_msgNo++;
    return usbMsgQueueErr_NoError;
}


/******************************************************************************
* UsbMsgQueue_Pop
To be used only from USB task.
******************************************************************************/
usbMsgQueueErr_t UsbMsgQueue_Pop(void)
{
    usbMsgQueueErr_t usbMsgQueueErr = usbMsgQueueErr_NoError;
    USB_Lock();

    if (usbMsgQueue_msgNo == 0)
    {
        usbMsgQueueErr = usbMsgQueueErr_QueueEmpty;
    }

    else
    {
        if (++usbMsgQueue_tail == mUsbMsgQueueSize)
        {
            usbMsgQueue_tail = 0;
        }

        usbMsgQueue_msgNo--;
    }

    USB_UnLock();
    return usbMsgQueueErr;
}

/******************************************************************************
* UsbMsgQueue_GetFirstMsg
******************************************************************************/
usbMsg_t *UsbMsgQueue_GetFirstMsg(void)
{
    return (maUsbMsgQueue + usbMsgQueue_tail);
}


/******************************************************************************
* UsbMsgQueue_MsgNo
******************************************************************************/
uint8_t UsbMsgQueue_MsgNo()
{
    return usbMsgQueue_msgNo;
}

/******************************************************************************
* UsbMsgQueue_MsgPending
******************************************************************************/
bool_t UsbMsgQueue_MsgPending()
{
    return (usbMsgQueue_msgNo != 0);
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
*******************************************************************************/

#endif// gUsbIncluded_d
