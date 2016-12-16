/******************************************************************************
* The file contains USB stack class layer API header function.
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

#ifndef _USB_CLASS_H
#define _USB_CLASS_H



/******************************************************************************
 * Includes
 *****************************************************************************/
#include "EmbeddedTypes.h"
#include "USB_Interface.h"
#include "usb_devapi.h"

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define gUsbSOFHighByteShift_d                 (8)
#define gUsbGetStatusDeviceMask_d              (0x0003)
#define gUsbRemoteWakeupStatusMask_d           (0x0002)
#define gbmUsbAttributes_BusPowered_d          (0x80)
#define gbmUsbAttributes_SelfPowered_d         (0x40)
#define gUsbSelfPoweredBitShift_d                    (6)

/* max packet size for the control endpoint */

/* USB Specs define CONTROL_MAX_PACKET_SIZE for High Speed device as only 64,
   whereas for FS its allowed to be 8, 16, 32 or 64 */
   





/* identification values and masks to identify request types  */
#define gUsbRequestClassMask_d              (0x60)
#define gUsbRequestClass_Strd_d             (0x00)
#define gUsbRequestClass_Class_d            (0x20)
#define gUsbRequestClass_Vendor_d           (0x40)
#define gUsbRequestRecipientMask_d          (0x03)
#define gUsbRequestRecipient_Device_d        (0x00)
#define gUsbRequestRecipient_Other_d         (0x03)

#define USB_ClassSendData( controller_ID , ep_num , buff_ptr , size) \
       USB_ClassSendData_( controller_ID , ep_num , buff_ptr , size , FALSE)
#define USB_ClassSendShort( controller_ID , ep_num , buff_ptr , size) \
       USB_ClassSendData_( controller_ID , ep_num , buff_ptr , size , TRUE)


/******************************************************************************
 * Types
 *****************************************************************************/

/******************************************************************************
 * Global Functions
 *****************************************************************************/
#if gUsbIncluded_d 
 extern uint8_t USB_ClassInit 
 (
    uint8_t    controller_ID,
    pfUsbToAppEventHandler_t class_callback,
    pfusbReqHandler_t       other_req_callback
 );

 extern uint8_t USB_ClassDeInit 
 (
    uint8_t    controller_ID      
 );

 extern uint8_t USB_ClassSendData_ (
    uint8_t           controller_ID,
    uint8_t           ep_num,
    uint8_t*       buff_ptr,
    usbPacketSize_t  size,
    bool_t shortSend
 );
#else
 #define USB_ClassInit(controller_ID, class_callback, other_req_callback)      gUsbErr_NoError_c
 #define USB_ClassDeInit(controller_ID)                                        gUsbErr_NoError_c   
 #define USB_ClassSendData_(controller_ID, ep_num, buff_ptr, size, shortSend)  gUsbErr_NoError_c
#endif

#ifdef __cplusplus
}
#endif

#endif
