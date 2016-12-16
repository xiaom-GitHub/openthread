/*****************************************************************************
* Aplication utilities declarations.
*
* (c) Copyright 2010, Freescale, Inc. All rights reserved.
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

#ifndef _USB_H_
#define _USB_H_

#include "EmbeddedTypes.h"
#include "USB_Interface.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

/* Define the controller that will be used for the CDC.*/

#define gUsbCdcControllerId_d                    0
/* Get the number of required CDC controllers. 1 if CDC support is enabled, 0 otherwise */
#define gNumUsbCdcControllers_d                    ((gUsbCdcEnabled_d)?1:0)

/* Define the controller that will be used for the HID.*/
#define gUsbHidControllerId_d             0
#define gNumUsbHidControllers_d                    ((gUsbHidEnabled_d)?1:0)

/* Define that gets the total number of endpoints (both control and non-control) 
used by the CDC. If CDC is enabled, it will require
   1 control IN endpoint
   1 control OUT endpoint
   1 bulk IN endpoint 
   1 bulk OUT endpoint */
#define gNumUsbCdcNonControlEp_d              (3)
#define gNumUsbCdcEp_d                        (gNumUsbCdcNonControlEp_d + 2)    
#define gNumUsbEpUsedByCdc_d                  ((gUsbCdcEnabled_d)?(gNumUsbCdcControllers_d * gNumUsbCdcEp_d):0)

/* Define that gets the total number of endpoints (both control and non-control) 
used by the HID. If HID is enabled, it will require
   1 control IN endpoint
   1 control OUT endpoint
   1 bulk IN endpoint */  
#define gNumUsbHidNonControlEp_d              (1)
#define gNumUsbHidEp_d                        (gNumUsbHidNonControlEp_d + 2) 
#define gNumUsbEpUsedByHid_d                  ((gUsbHidEnabled_d)?(gNumUsbHidControllers_d * gNumUsbHidEp_d):0)


/* Get the amount of RAM required to store the buffers for all endpoints of the CDC */
#define gUsbRamUsedByOneCdc_d                 ((2 * gUsbCdcControlEpSize_d) + gUsbCdcBulkInEpSize_d + gUsbCdcBulkOutEpSize_d + gUsbCdcInterruptInEpSize_d)
#define gUsbRamUsedByCdc_d                    (gNumUsbCdcControllers_d * gUsbRamUsedByOneCdc_d)

/* Get the amount of RAM required to store the buffers for all endpoints of the HID */
#define gUsbRamUsedByOneHid_d                 ((2 * gUsbHidControlEpSize_d) + gUsbHidInterruptInEpSize_d)
#define gUsbRamUsedByHid_d                    (gNumUsbHidControllers_d * gUsbRamUsedByOneHid_d)




  

/*  --- TOTALS ----- */


/* Get total number of controllers that will be enabled on the device */
#define gNumUsbControllers_d                    (gNumUsbCdcControllers_d + gNumUsbHidControllers_d)

/* Get total number of endpoints that will be required for the USB controllers enabled on the device */
#define gNumUsbEp_d                           (gNumUsbEpUsedByCdc_d + gNumUsbEpUsedByHid_d)

///* Get total amount of RAM required for all endpoints that will be required for the USB controllers enabled on the device */
#define gSizeOfUsbRam_d                       (gUsbRamUsedByCdc_d + gUsbRamUsedByHid_d)





/*****************************************************************************
******************************************************************************
* Public type declarations
******************************************************************************
*****************************************************************************/
typedef enum
{
  gUsbStdRequest_GetStatus_c,
  gUsbStdRequest_ClearFeature_c, 
  gUsbStdRequest_Reserved2_c,
  gUsbStdRequest_SetFeature_c,  
  gUsbStdRequest_Reserved4_c, 
  gUsbStdRequest_SetAddress_c, 
  gUsbStdRequest_GetDescriptor_c, 
  gUsbStdRequest_SetDescriptor_c, 
  gUsbStdRequest_GetConfiguration_c, 
  gUsbStdRequest_SetConfiguration_c, 
  gUsbStdRequest_GetInterface_c, 
  gUsbStdRequest_SetInterface_c, 
  gUsbStdRequest_SynchFrame_c 
}usbStdRequest_t;


typedef struct stallEvent_t_tag
{
  uint8_t stallEp;    
}stallEvent_t;
typedef struct configEvent_t_tag
{
  uint16_t configValue;    
}configEvent_t;

typedef struct commEvent_t_tag
{
  uint8_t epNum;
  uint8_t* pBuffer;
  usbPacketSize_t len;                /* buffer size of endpoint */
}sendCompleteEvent_t,dataReceivedEvent_t;

typedef struct shortSendCompleteEvent_t_tag
{
  uint8_t epNum;
  uint8_t* pBuffer;
  usbPacketSize_t len;       
}shortSendCompleteEvent_t;

typedef struct requestPendingEvent_t_tag
{
  usbSetupPacket_t * pSetupPacket;    
}requestPendingEvent_t;

typedef struct errorEvent_t_tag
{
  uint8_t errors;    
}errorEvent_t;


typedef struct usbMsg_t_tag
{
  uint8_t controllerId;
  gUsbToAppEvent_t  eventType;
  union 
  {
    stallEvent_t stallEvent;
    configEvent_t configEvent;
    sendCompleteEvent_t sendCompleteEvent;
    dataReceivedEvent_t dataReceivedEvent;
    shortSendCompleteEvent_t shortSendCompleteEvent;
    requestPendingEvent_t requestPendingEvent;
    errorEvent_t errorEvent;
  }eventParam;
}usbMsg_t;


typedef struct
{
  uint8_t           controllerId;
  uint8_t           epNum;
  usbEpDirection_t  direction;
}usbIsrEvent_t;


/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/
#if gUsbIncluded_d

#if (gNumUsbControllers_d == 0)
 #error If gUsbIncluded_d == TRUE, you must enable a device in USB_Configuration.h
#endif 
 extern const usbClassDescriptor_t* gpaUsbClassDescriptor[gNumUsbControllers_d];

 extern pfUsbToAppEventHandler_t gpfClassFrameworkCallback;

#endif//gUsbIncluded_d




/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/
#if gUsbIncluded_d 
 extern void USB_IsrEvent(usbMsg_t* pUsbMsg);
 extern void USB_Lock(void);
 extern void USB_UnLock(void);
 #else
 #define USB_IsrEvent(pUsbMsg)  
 #define USB_Lock()  
 #define USB_UnLock()  
 
#endif // gUsbIncluded_d 

#ifdef __cplusplus
}
#endif

#endif /* _USB_H_ */
