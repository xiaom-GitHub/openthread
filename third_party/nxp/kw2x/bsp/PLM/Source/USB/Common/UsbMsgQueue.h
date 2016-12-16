/*****************************************************************************
* USB Mem System declarations.
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

#ifndef _USB_MSG_QUEUE_H_
#define _USB_MSG_QUEUE_H_

#include "EmbeddedTypes.h"
#include "USB.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/


/*****************************************************************************
******************************************************************************
* Public type declarations
******************************************************************************
*****************************************************************************/
typedef enum 
  {
    usbMsgQueueErr_NoError,
    usbMsgQueueErr_QueueFull,
    usbMsgQueueErr_QueueEmpty
  }usbMsgQueueErr_t;


/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/
/******************************************************************************
* UsbMsgQueueInit
******************************************************************************/
extern void UsbMsgQueue_Init(void);

/******************************************************************************
* UsbMsgQueue_Push
******************************************************************************/
extern usbMsgQueueErr_t UsbMsgQueue_Push(usbMsg_t* usbMsg);


/******************************************************************************
* UsbMem_Free
******************************************************************************/
extern usbMsgQueueErr_t UsbMsgQueue_Pop(void);

/******************************************************************************
* UsbMsgQueue_GetFirstMsg
******************************************************************************/
extern usbMsg_t* UsbMsgQueue_GetFirstMsg(void);

/******************************************************************************
* UsbMsgQueue_Empty
******************************************************************************/
extern uint8_t UsbMsgQueue_MsgNo(void);

/******************************************************************************
* UsbMsgQueue_MsgPending
******************************************************************************/
extern bool_t UsbMsgQueue_MsgPending(void);

/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _USB_MSG_QUEUE_H_ */
