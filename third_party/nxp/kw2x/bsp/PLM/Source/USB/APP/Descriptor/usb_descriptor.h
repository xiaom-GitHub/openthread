/******************************************************************************
* This is the generic descriptor file interface
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

#ifndef _USB_DESCRIPTOR_H
#define _USB_DESCRIPTOR_H

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "EmbeddedTypes.h"
#include "USB_Interface.h"

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define gUSB_DescSize_DeviceDescriptorSize_d            (18)
#define gUSB_DescSize_StringDescriptorSize_d            (2)
#define gUSB_DescSize_DeviceQualifierDescriptorSize_d   (10)
#define gUSB_DescSize_ConfigOnlyDescriptorSize_d        (9)
#define gUSB_DescSize_InterfaceOnlyDescriptorSize_d     (9)
#define gUSB_DescSize_EndpOnlyDescriptorSize_d          (7)
 
#define gUSB_DescType_DeviceDescriptor_d                (1)
#define gUSB_DescType_ConfigDescriptor_d                (2)
#define gUSB_DescType_StringDescriptor_d                (3)
#define gUSB_DescType_InterfaceDescriptor_d             (4)
#define gUSB_DescType_EndpointDescriptor_d              (5)
#define gUSB_DescType_DevQualDescriptor_d               (6)

#define gUSB_RemoteWakeupShift_d                        (5)
 
/******************************************************************************
 * Types
 *****************************************************************************/
 
typedef const struct usbLanguage_tag
{
  uint16_t const languageId;      /* Language ID */
  uint8_t const ** pLangDesc;      /* Language Descriptor String */
  uint8_t const * pLangDescSize;  /* Language Descriptor Size */
} usbLanguage_t; 



/******************************************************************************
 * Global variables
 *****************************************************************************/


/******************************************************************************
 * Global Functions
 *****************************************************************************/
 
/*****************************************************************************
 * This function is used to pass the pointer to the requested descriptor
 *****************************************************************************/ 
extern uint8_t USB_Desc_GetDescriptor
(
  uint8_t controllerId,
  uint8_t type,
  uint8_t str_num,
  uint16_t index,
  uint8_t* *descriptor,
  usbPacketSize_t *size
);

/*****************************************************************************
 * This function is called by the framework module to get the current alternete interface
 *****************************************************************************/
extern uint8_t USB_Desc_GetInterface
(
  uint8_t controllerId,
  uint8_t interface,
  uint8_t* altInterface
);

/*****************************************************************************
 * This function is called by the framework module to set the alternate interface
 *****************************************************************************/
extern uint8_t USB_Desc_SetInterface
(
  uint8_t controllerId,
  uint8_t interface,
  uint8_t altInterface
);

/*****************************************************************************
 * This function checks whether the configuration is valid or not
 *****************************************************************************/
extern bool_t USB_Desc_ValidConfiguration
(
  uint8_t controllerId,
  uint16_t configVal
);

/*****************************************************************************
 * This function checks whether the interface is valid or not
 *****************************************************************************/
extern bool_t USB_Desc_ValidInterface
(
uint8_t controllerId,
uint8_t interface
);

/*****************************************************************************
 * The function checks whether the remote wakeup is supported or not
 *****************************************************************************/
extern bool_t USB_Desc_RemoteWakeup(uint8_t controllerId);

#ifdef __cplusplus
}
#endif

#endif
