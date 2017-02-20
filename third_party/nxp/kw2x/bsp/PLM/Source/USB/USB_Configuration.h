/*****************************************************************************
* Universal Serial Bus implementation 
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

#ifndef _USB_CONFIGURATION_H
#define _USB_CONFIGURATION_H

#include "EmbeddedTypes.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

/* Defines whether or not the USB module is included in the application */
#ifndef gUsbIncluded_d
  #define gUsbIncluded_d              TRUE
#endif


/* USB Task priority: should have a medium level */
#ifndef gTsUsbTaskPriority_c
  #define gTsUsbTaskPriority_c        0x04
#endif





/* --- Params for configuring the CDC --------- */

/* Selects whether the device will have the CDC class enabled */
#ifndef gUsbCdcEnabled_d                
#define gUsbCdcEnabled_d              TRUE
#endif

/* Configures the size of the control endpoint for the CDC class
   This define applies to both Control IN and OUT endpoints 
   This define is ignored if CDC class support is not enabled
   Valid values :8, 16, 32, 64 */
#ifndef gUsbCdcControlEpSize_d                
#define gUsbCdcControlEpSize_d        16
#endif

/* Configures the size of the bulk IN endpoint for the CDC class
   This define is ignored if CDC class support is not enabled
   Valid values :8, 16, 32, 64 */
#ifndef gUsbCdcBulkInEpSize_d                
#define gUsbCdcBulkInEpSize_d   16
#endif


/* Configures the size of the bulk OUT endpoint for the CDC class
   This define is ignored if CDC class support is not enabled
   Valid values :8, 16, 32, 64 */
#ifndef gUsbCdcBulkOutEpSize_d                
#define gUsbCdcBulkOutEpSize_d  16
#endif

/* Configures the size of the interrupt IN endpoint for the CDC class
   This define is ignored if CDC class support is not enabled
   Valid values :8, 16, 32, 64 */
#ifndef gUsbCdcInterruptInEpSize_d                
#define gUsbCdcInterruptInEpSize_d   16
#endif




/* --- Params for configuring the HID --------- */

/* Selects whether the device will have the HID class enabled */
#ifndef gUsbHidEnabled_d                
#define gUsbHidEnabled_d              FALSE
#endif


/* Configure the number of HID controllers that can be used for installing
USB HID class devices. 
*/



/* Configures the size of the control endpoint for the HID class
   This define applies to both Control IN and OUT endpoints 
   This define is ignored if HID class support is not enabled
   Valid values :8, 16, 32, 64 */
#ifndef gUsbHidControlEpSize_d                
#define gUsbHidControlEpSize_d        16
#endif


/* Configures the size of the interrupt IN endpoint for the HID class
   This define is ignored if HID class support is not enabled
   Valid values :8, 16, 32, 64 */
#ifndef gUsbHidInterruptInEpSize_d                
#define gUsbHidInterruptInEpSize_d   8
#endif


#define gUsbAppNotificationOnSuspend_d  FALSE


/* ------ Configurable parameters verification section ------- */



/* Selects whether the device will function as a USB HUB */
#if (gUsbCdcEnabled_d && gUsbHidEnabled_d)               
#error Only one of the CDC and HID classes can be enabled
#endif 


/* Size of CDC control endpoint buffer should be 8, 16, 32 or 64 */
#if (gUsbCdcEnabled_d && \
   !((gUsbCdcControlEpSize_d == 8)  || (gUsbCdcControlEpSize_d == 16) || \
     (gUsbCdcControlEpSize_d == 32) || (gUsbCdcControlEpSize_d == 64)))
  #error Invalid CDC control endpoint length. Should be 8, 16, 32 or 64
#endif 


/* Size of CDC bulk endpoint IN buffer should be 8, 16, 32 or 64 */
#if (gUsbCdcEnabled_d && \
   !((gUsbCdcBulkInEpSize_d == 8)  || (gUsbCdcBulkInEpSize_d == 16) || \
     (gUsbCdcBulkInEpSize_d == 32) || (gUsbCdcBulkInEpSize_d == 64)))
  #error Invalid CDC non-control IN endpoint length. Should be 8, 16, 32 or 64
#endif 


/* Size of CDC bulk endpoint OUT buffer should be 8, 16, 32 or 64 */
#if (gUsbCdcEnabled_d && \
   !((gUsbCdcBulkOutEpSize_d == 8)  || (gUsbCdcBulkOutEpSize_d == 16) || \
     (gUsbCdcBulkOutEpSize_d == 32) || (gUsbCdcBulkOutEpSize_d == 64)))
  #error Invalid CDC non-control OUT endpoint length. Should be 8, 16, 32 or 64
#endif 


/* Size of HID control endpoint buffer should be 8, 16, 32 or 64 */
#if (gUsbHidEnabled_d && \
   !((gUsbHidControlEpSize_d == 8)  || (gUsbHidControlEpSize_d == 16) || \
     (gUsbHidControlEpSize_d == 32) || (gUsbHidControlEpSize_d == 64)))
  #error Invalid HID control endpoint length. Should be 8, 16, 32 or 64
#endif 


/* Size of HID interrupt IN endpoint buffer should be 8, 16, 32 or 64 */
#if (gUsbHidEnabled_d && \
   !((gUsbHidInterruptInEpSize_d == 8)  || (gUsbHidInterruptInEpSize_d == 16) || \
     (gUsbHidInterruptInEpSize_d == 32) || (gUsbHidInterruptInEpSize_d == 64)))
  #error Invalid HID non-control endpoint length. Should be 8, 16, 32 or 64
#endif 

/*****************************************************************************
******************************************************************************
* Public type definitions
******************************************************************************
*****************************************************************************/


/*****************************************************************************
******************************************************************************
* Public memory declaration
******************************************************************************
*****************************************************************************/


/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _USB_CONFIGURATION_H */