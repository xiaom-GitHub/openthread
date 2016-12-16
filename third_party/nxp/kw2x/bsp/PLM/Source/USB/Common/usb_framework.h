/******************************************************************************
* The file contains USB Framework module API header function.
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

#ifndef _USB_FRAMEWORK_H
#define _USB_FRAMEWORK_H

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "EmbeddedTypes.h"
#include "usb_class.h"
#include "usb_descriptor.h"

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define gMaxStrdReq_d                (13)  /* Max value of standard request */
/* size of data to be returned for various Get Desc calls */
#define gDeviceStatusSize_d          (2)
#define gUsbInterfaceStatusSize_d        (1)
#define gUsbConfigSize_d                 (1)
#define gUsbFrameSize_d                  (2)
#define gUsbEndpStatusSize_d             (2)

#define gUsbDeviceFeatureRemoteWakeup_d      (0x0001)
#define gUsbDeviceSetFeatureMask_d           ((uint16_t)(1<<(gUsbDeviceFeatureRemoteWakeup_d)))
#define gUsbDeviceClearFeatureMask_d         ((uint16_t)(1<<(gUsbDeviceFeatureRemoteWakeup_d)))

#define gUsbReportDescriptorType_d     (0x22)
#define gUsbStringDescriptorType_d     (0x03)

/* masks and values for provides of Get Desc information */
#define gUsbRequestSrcMask_d         (0x03)
#define gUsbRequestSrc_Device_d      (0x00)
#define gUsbRequestSrc_Interface_d   (0x01)
#define gUsbRequestSrc_Endpoint_d    (0x02)

#define  gUsbSetRequestMask_d       (0x02)

/* for transfer direction check */
#define gUsbDataDir_ToHost_d            (0x80)
#define gUsbDataDir_ToDevice_d          (0x00)
#define gUsbDataDirMask_d               (0x80)

#define USB_UInt16Low(x)  ((uint8_t)x)       /* lsb byte */
#define USB_UInt16High(x) ((uint8_t)(x>>8))  /* msb byte */
#define USB_ByteSwap16(a) (uint16_t)((((uint16_t)(a)&0xFF00)>>8) | (((uint16_t)(a)&0x00FF)<<8))

#define gLittle_Endian_d  (!gBigEndian_c)

/******************************************************************************
 * Types
 *****************************************************************************/

/******************************************************************************
 * Global Functions
 *****************************************************************************/
extern bool_t USB_Framework_RemoteWakeup(uint8_t controllerId);

#define USB_Framework_RemoteWakeup USB_Desc_RemoteWakeup
extern uint8_t USB_Framework_Init (
    uint8_t              controllerId,
    pfUsbToAppEventHandler_t  pfClassCallback,
    pfusbReqHandler_t        pfOtherReqCallback
);

extern uint8_t USB_Framework_DeInit
(
    uint8_t controllerId
);

#ifdef __cplusplus
}
#endif

#endif
