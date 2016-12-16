/******************************************************************************
* The file contains USB CDC_PSTN Sub Class API header function
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

#ifndef _USB_CDC_PSTN_H
#define _USB_CDC_PSTN_H

/******************************************************************************
 * Includes
 *****************************************************************************/
 #include "usb_cdc.h" /* USB CDC Class Header File */
 
#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
 * Constants - None
 *****************************************************************************/
typedef struct bitmapUart_tag
{
  uint8_t bRxCarrier   : 1;    /* Receive Carrier Activation Flag */
  uint8_t bTxCarrier   : 1;    /* Transmit Carrier Activation Flag */
  uint8_t bBreak       : 1;    /* Break Flag */
  uint8_t bRingSignal  : 1;    /* Ring Signal Flag */
  uint8_t bFraming     : 1;    /* Frame Flag */
  uint8_t bParity      : 1;    /* Parity Flag */
  uint8_t bOverRun     : 1;    /* OverRun Flag */
  uint8_t reserved1    : 1;    /* Reserved */
}bitmapUart_t;
typedef union uartBitmap_tag
{
  uint8_t byte;
  bitmapUart_t bitmapUart;
}uartBitmap_t;   /* UART STATE BITMAP */


/******************************************************************************
 * Macro's
 *****************************************************************************/
#define gUSB_CDC_UARTBitmapSize_d                    (0x02)
#define gPSTN_CommFeature_AbstractStateFeature_d     (0x01)
#define gPSTN_CommFeature_CountrySettingFeature_d    (0x02)
#define gDTEStatus_CarrierActivation_d               (0x02)
#define gDTEStatus_DTEPresence_d                     (0x01)
//#define gDTEStatus_COM_Open_d                        (gDTEStatus_CarrierActivation_d | gDTEStatus_DTEPresence_d)
#define gDTEStatus_COM_Open_d                        (gDTEStatus_DTEPresence_d)
#define gUSB_CDC_CommFeatureDataSize_d               (0x02)

extern uint8_t USB_Class_CDC_PSTN_GetLineCoding
(
  uint8_t controllerId,               /* [IN] Controller ID */
  usbSetupPacket_t * pSetupPacket,    /* [IN] Pointer to setup packet */
  uint8_t* *pData,                   /* [OUT] Pointer to Data to be send */
  usbPacketSize_t *pSize               /* [OUT] Pointer to Size of Data */
);

extern uint8_t USB_Class_CDC_PSTN_SetLineCoding
(
  uint8_t controllerId,               /* [IN] Controller ID */
  usbSetupPacket_t * pSetupPacket,    /* [IN] Pointer to setup packet */
  uint8_t* *pData,                   /* [OUT] Pointer to Data */
  usbPacketSize_t *pSize               /* [OUT] Pointer to Size of Data */
);

extern uint8_t USB_Class_CDC_PSTN_SetCtrlLineState 
(
  uint8_t controllerId,               /* [IN] Controller ID */
  usbSetupPacket_t * pSetupPacket,    /* [IN] Pointer to setup packet */
  uint8_t* *pData,                   /* [OUT] Pointer to Data  */
  usbPacketSize_t *pSize               /* [OUT] Pointer to Size of Data */
);

extern uint8_t USB_Class_CDC_PSTN_SendBreak
(
  uint8_t controllerId,               /* [IN] Controller ID */
  usbSetupPacket_t * pSetupPacket,    /* [IN] Pointer to setup packet */
  uint8_t* *pData,                   /* [OUT] Pointer to Data  */
  usbPacketSize_t *pSize               /* [OUT] Pointer to Size of Data */
);

extern uint8_t USB_Class_CDC_PSTN_GetCommFeature 
(
  uint8_t controllerId,               /* [IN] Controller ID */
  usbSetupPacket_t * pSetupPacket,    /* [IN] Pointer to setup packet */
  uint8_t* *pData,                   /* [OUT] Pointer to Data to send */
  usbPacketSize_t *pSize               /* [OUT] Pointer to Size of Data */
);

extern uint8_t USB_Class_CDC_PSTN_SetCommFeature
(
  uint8_t controllerId,               /* [IN] Controller ID */
  usbSetupPacket_t * pSetupPacket,    /* [IN] Pointer to setup packet */
  uint8_t* *pData,                   /* [OUT] Pointer to Data */
  usbPacketSize_t *pSize               /* [OUT] Pointer to Size of Data */
);

#if gUSB_CDC_CIC_NotifElemSupport_d
extern void USB_Class_CDC_PSTN_SendSerialState (uint8_t controllerId);
#endif

#ifdef __cplusplus
}
#endif

#endif
