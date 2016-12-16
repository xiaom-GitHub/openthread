/******************************************************************************
* The file contains USB stack HID class layer API header function.
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

#ifndef _USB_HID_H
#define _USB_HID_H

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "EmbeddedTypes.h"
#include "usb_class.h"
#include "USB_Configuration.h"

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
 /* class specific requests */

#define gUSB_HID_ClassRequest_GetReport_d      (0x01)
#define gUSB_HID_ClassRequest_GetIdle_d        (0x02)
#define gUSB_HID_ClassRequest_GetProtocol_d    (0x03)
#define gUSB_HID_ClassRequest_SetReport_d      (0x09)
#define gUSB_HID_ClassRequest_SetIdle_d        (0x0A)
#define gUSB_HID_ClassRequest_SetProtocol_d    (0x0B)

/* for class specific requests */

#define gUSB_HID_HighByteShift_d                 (8)
#define gUSB_HID_HighByteMask                  (0xFF00)
#define gUSB_HID_RequestDirMask_d               (0x08)
#define gUSB_HID_RequestTypeMask_d              (0x01)
#define gUSB_HID_ReportSize_d                     (4)
#define gUSB_HID_ClassReqDataSize_d             (0x01)

/******************************************************************************
 * Types
 *****************************************************************************/

/******************************************************************************
 * Global Functions
 *****************************************************************************/

#if gUsbHidEnabled_d 

extern uint8_t USB_Class_HID_Init(uint8_t controllerId);
extern uint8_t USB_Class_HID_DeInit(uint8_t controllerId);
#define USB_Class_HID_SendData( controllerId, epNum, pBuff, size)  USB_ClassSendData( controllerId, epNum, pBuff, size)

#else

#define USB_Class_HID_Init(controllerId)                            gUsbErr_NoError_c
#define USB_Class_HID_DeInit(controllerId)                          gUsbErr_NoError_c
#define USB_Class_HID_SendData( controllerId, epNum, pBuff, size)   gUsbErr_NoError_c

#endif // gUsbHidEnabled_d

#ifdef __cplusplus
}
#endif

#endif // _USB_HID_H