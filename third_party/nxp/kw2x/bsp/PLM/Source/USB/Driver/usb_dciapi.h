/******************************************************************************
* The file contains DCI api function definetions .
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

#ifndef _USB_DCIAPI_H
#define _USB_DCIAPI_H

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "EmbeddedTypes.h"
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
#define gNumMaxEnpNumber_d     16 
#define gNumMinEnpNumber_d     1

/******************************************************************************
 * Types
 *****************************************************************************/


 /*****************************************************************************
 * Global Functions
 *****************************************************************************/
extern uint8_t USB_DCI_Init (uint8_t    controllerId);
extern uint8_t USB_DCI_DeInit(uint8_t    controllerId);
extern uint8_t USB_DCI_InitEndpoint(uint8_t controller_ID, usbEpStruct_t* pEpStruct, bool_t flag);
extern uint8_t USB_DCI_CancelTransfer( uint8_t controllerId, uint8_t endpNumber, uint8_t direction);
extern uint8_t USB_DCI_DeinitEndpoint(uint8_t controllerId, uint8_t epNum, uint8_t direction);
extern void USB_DCI_StallEndpoint( uint8_t controllerId, uint8_t endpNum, uint8_t direction);
extern void USB_DCI_UnstallEndpoint( uint8_t controllerId, uint8_t endpNumber, uint8_t direction);
extern void USB_DCI_GetSetupData(uint8_t controllerId, uint8_t  endpNumber, uint8_t* pBuff);
extern uint8_t USB_DCI_GetTransferStatus( uint8_t controllerId, uint8_t endpNumber, uint8_t direction);
extern uint8_t USB_DCI_RecvData(uint8_t controllerId, uint8_t endpNumber, uint8_t* pBuff, usbPacketSize_t size);
extern uint8_t USB_DCI_SendData(uint8_t controllerId, uint8_t endpNumber, uint8_t* pBuff, usbPacketSize_t size, bool_t sendShort);
extern void  USB_DCI_SetAddress(uint8_t controllerId, uint8_t address);
extern void USB_DCI_AssertResume(uint8_t controllerId);


#define USB_DCI_CancelTransfer USB_DeviceCancelTransfer  

#define USB_DCI_RecvData USB_DeviceRecvData  

#define USB_DCI_SendData USB_DeviceSendData    

#define USB_DCI_StallEndpoint USB_DeviceStallEndpoint  

#define USB_DCI_UnstallEndpoint USB_DeviceUnstallEndpoint  

#define USB_DCI_GetTransferStatus USB_Device_GetTransferStatus       

#define USB_DCI_GetSetupData USB_Device_ReadSetupData 

#define USB_DCI_SetAddress USB_DeviceSetAddress  

#define USB_DCI_AssertResume USB_DeviceAssertResume

#ifdef __cplusplus
}
#endif

#endif
