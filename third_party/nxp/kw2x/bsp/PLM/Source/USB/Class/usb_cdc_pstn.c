/******************************************************************************
* The file contains USB CDC_PSTN Sub Class implementation.
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

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "USB.h"
#include "usb_cdc_pstn.h"  /* USB CDC PSTN Sub Class Header File */
#if gUsbCdcEnabled_d

#if (gUsbIncluded_d == FALSE)
#error If gUsbCdcEnabled_d == TRUE, gUsbIncluded_d must be TRUE
#endif
/*****************************************************************************
* Constant and Macro's
*****************************************************************************/
#define USB_Class_CDC_Interface_CIC_Send_Data(controllerId, pBuff,size)  \
        USB_ClassSendData( controllerId , gNotifyEndpoint , pBuff, size)

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/
#if gUSB_CDC_CIC_NotifElemSupport_d
uint8_t gNotifyEndpoint;
#endif
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Local Variables
 *****************************************************************************/
/* data terminal equipment present or not */
uint8_t gDTEStatus = (uint8_t)gUninitialisedVal_d; /* Status of DATA TERMINAL EQUIPMENT */
void (*pfCDCCtrlLineStateCallback)(uint8_t val);
static uint8_t gCurrentInterface = 0;
static uartBitmap_t gUARTBitmap;

#if gUSB_CDC_CIC_NotifElemSupport_d
static uint8_t gSerialState[gUSB_CDC_Size_NotifPacketSize_d + gUSB_CDC_UARTBitmapSize_d] =
{
    gUSB_CDC_NotifRequestType_d, gUSB_CDC_ClassNotifCodes_SerialState_d,
    0x00, 0x00, /*wValue*/
    0x00, 0x00, /*interface - modifies*/
    gUSB_CDC_UARTBitmapSize_d, 0x00, /* wlength*/
    0x00, 0x00 /*data initialized - modifies*/
};/*uart bitmap*/
#endif
/*****************************************************************************
* Local Functions
*****************************************************************************/

/*****************************************************************************
 * Global Functions
 *****************************************************************************/

/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_PSTN_GetLineCoding
 *
 * @brief  This function is called in response to GetLineCoding request
 *
 * @param controller_ID   : Controller ID
 * @param setup_packet    : Pointer to setup packet
 * @param data            : Pointer to Data to be send
 * @param size            : Pointer to Size of Data
 *
 * @return status:
 *          gUsbErr_NoError_c : When Successfull
 *          gUsbErr_InvalidReqType_c: When  request for
 *                                   invalid Interface is presented
 ******************************************************************************
 * Calls application to receive Line Coding Information
 *****************************************************************************/
uint8_t USB_Class_CDC_PSTN_GetLineCoding
(
    uint8_t controllerId,               /* [IN] Controller ID */
    usbSetupPacket_t *pSetupPacket,     /* [IN] Pointer to setup packet */
    uint8_t * *pData,                  /* [OUT] Pointer to Data to be send */
    usbPacketSize_t *pSize               /* [OUT] Pointer to Size of Data */
)
{
    uint8_t status;
    (void)(pSize);
    gCurrentInterface = (uint8_t)pSetupPacket->index ;
    status = USB_CDC_Desc_GetLineCoding(controllerId, gCurrentInterface, pData);
    return status;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_PSTN_SetLineCoding
 *
 * @brief  This function is called in response to SetLineCoding request
 *
 * @param controller_ID   : Controller ID
 * @param setup_packet    : Pointer to setup packet
 * @param data            : Pointer to Data
 * @param size            : Pointer to Size of Data
 *
 * @return status:
 *      gUsbErr_NoError_c                  : When Successfull
 *      gUsbErr_InvalidReqType_c : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Calls Applciation to update Line Coding Information
 *****************************************************************************/
uint8_t USB_Class_CDC_PSTN_SetLineCoding
(
    uint8_t controllerId,               /* [IN] Controller ID */
    usbSetupPacket_t *pSetupPacket,     /* [IN] Pointer to setup packet */
    uint8_t * *pData,                  /* [OUT] Pointer to Data */
    usbPacketSize_t *pSize               /* [OUT] Pointer to Size of Data */
)
{
    uint8_t status;
    *pData = (uint8_t *)pSetupPacket + gUsbSetupPacketSize_d;
    *pSize = 0;
    gCurrentInterface = (uint8_t)pSetupPacket->index ;
    status = USB_CDC_Desc_SetLineCoding(controllerId, gCurrentInterface, pData);
    return status;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_PSTN_SetCtrlLineState
 *
 * @brief  This function is called in response to Set Control Line State
 *
 * @param controller_ID   : Controller ID
 * @param setup_packet    : Pointer to setup packet
 * @param data            : Pointer to Data
 * @param size            : Pointer to Size of Data
 *
 * @return status:
 *                        gUsbErr_NoError_c : Always
 ******************************************************************************
 * Updates Control Line State
 *****************************************************************************/
uint8_t USB_Class_CDC_PSTN_SetCtrlLineState
(
    uint8_t controllerId,               /* [IN] Controller ID */
    usbSetupPacket_t *pSetupPacket,     /* [IN] Pointer to setup packet */
    uint8_t * *pData,                  /* [OUT] Pointer to Data  */
    usbPacketSize_t *pSize               /* [OUT] Pointer to Size of Data */
)
{

    (void)(pData);
    *pSize = 0;

    if (gDTEStatus == (uint8_t)pSetupPacket->value)
    {
        return gUsbErr_NoError_c;
    }

    gDTEStatus = (uint8_t)pSetupPacket->value ;
    gCurrentInterface = (uint8_t)pSetupPacket->index ;
    gUARTBitmap.byte = 0x00; /* initialize */
    /* activate/deactivate Tx carrier */
    gUARTBitmap.bitmapUart.bTxCarrier = (gDTEStatus & gDTEStatus_CarrierActivation_d) ? 1 : 0 ;
    /* activate carrier and DTE */
    gUARTBitmap.bitmapUart.bRxCarrier = 1;

#if gUSB_CDC_CIC_NotifElemSupport_d
    /* Send Notification to Host - Parameter on Device side has changed */
    USB_Class_CDC_PSTN_SendSerialState(controllerId);
#endif


    if (pfCDCCtrlLineStateCallback != NULL)
    {
        pfCDCCtrlLineStateCallback(gDTEStatus);
    }

    return gUsbErr_NoError_c;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_PSTN_SendBreak
 *
 * @brief  This function is called in response to Set Config request
 *
 * @param controller_ID   : Controller ID
 * @param setup_packet    : Pointer to setup packet
 * @param data            : Pointer to Data
 * @param size            : Pointer to Size of Data
 *
 * @return status:
 *                        gUsbErr_NoError_c : Always
 ******************************************************************************
 * Updates Break Duration Information from Host
 *****************************************************************************/
uint8_t USB_Class_CDC_PSTN_SendBreak
(
    uint8_t controllerId,               /* [IN] Controller ID */
    usbSetupPacket_t *pSetupPacket,     /* [IN] Pointer to setup packet */
    uint8_t * *pData,                  /* [OUT] Pointer to Data  */
    usbPacketSize_t *pSize               /* [OUT] Pointer to Size of Data */
)
{
    (void)(controllerId);
    (void)(pData);
    *pSize = 0;
    return gUsbErr_NoError_c;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_PSTN_GetCommFeature
 *
 * @brief  This function is called in response to GetCommFeature request
 *
 * @param controller_ID   : Controller ID
 * @param setup_packet    : Pointer to setup packet
 * @param data            : Pointer to Data to be send
 * @param size            : Pointer to Size of Data
 *
 * @return status:
 *      gUsbErr_NoError_c                  : When Successfull
 *      gUsbErr_InvalidReqType_c : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns the status of the get comm feature request
 *****************************************************************************/
uint8_t USB_Class_CDC_PSTN_GetCommFeature
(
    uint8_t controllerId,               /* [IN] Controller ID */
    usbSetupPacket_t *pSetupPacket,     /* [IN] Pointer to setup packet */
    uint8_t * *pData,                  /* [OUT] Pointer to Data to send */
    usbPacketSize_t *pSize               /* [OUT] Pointer to Size of Data */
)
{
    uint8_t status;
    status = gUsbErr_InvalidReqType_c;
    *pSize = gUSB_CDC_CommFeatureDataSize_d;
    gCurrentInterface = (uint8_t)pSetupPacket->index ;

    if (pSetupPacket->value == gPSTN_CommFeature_AbstractStateFeature_d)
    {
        status = USB_CDC_Desc_GetAbstractState(controllerId, gCurrentInterface, pData);
    }
    else if (pSetupPacket->value == gPSTN_CommFeature_CountrySettingFeature_d)
    {
        status = USB_CDC_Desc_GetCountrySetting(controllerId, gCurrentInterface, pData);
    }

    return status;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_PSTN_SetCommFeature
 *
 * @brief  This function is called in response to SetCommFeature request
 *
 * @param controller_ID   : Controller ID
 * @param setup_packet    : Pointer to setup packet
 * @param data            : Pointer to Data
 * @param size            : Pointer to Size of Data
 *
 * @return status:
 *      gUsbErr_NoError_c                  : When Successfull
 *      gUsbErr_InvalidReqType_c : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets the comm feature specified by Host
 *****************************************************************************/
uint8_t USB_Class_CDC_PSTN_SetCommFeature
(
    uint8_t controllerId,               /* [IN] Controller ID */
    usbSetupPacket_t *pSetupPacket,     /* [IN] Pointer to setup packet */
    uint8_t * *pData,                  /* [OUT] Pointer to Data */
    usbPacketSize_t *pSize               /* [OUT] Pointer to Size of Data */
)
{
    uint8_t status;
    status = gUsbErr_InvalidReqType_c;
    *pSize = 0;
    gCurrentInterface = (uint8_t)pSetupPacket->index ;

    if (pSetupPacket->value == gPSTN_CommFeature_AbstractStateFeature_d)
    {
        status = USB_CDC_Desc_SetAbstractState(controllerId, gCurrentInterface, pData);
    }
    else if (pSetupPacket->value == gPSTN_CommFeature_CountrySettingFeature_d)
    {
        status = USB_CDC_Desc_SetCountrySetting(controllerId, gCurrentInterface, pData);
    }

    return status;
}

#if gUSB_CDC_CIC_NotifElemSupport_d
/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_PSTN_SendSerialState
 *
 * @brief  This function is called to send serial state notification
 *
 * @param controller_ID : Controller ID
 *
 * @return NONE
 ******************************************************************************
 * Returns the Serial State
 *****************************************************************************/
void USB_Class_CDC_PSTN_SendSerialState
(
    uint8_t controllerId               /* [IN] Controller ID */
)
{
    /* update array for current interface */
    gSerialState[4] = gCurrentInterface;
    /* Lower byte of UART BITMAP */
    gSerialState[gUSB_CDC_Size_NotifPacketSize_d + gUSB_CDC_UARTBitmapSize_d - 2] = gUARTBitmap.byte;

    if (gNotifyEndpoint)
    {
        (void)USB_DeviceCancelTransfer(controllerId, gNotifyEndpoint, gUsbEpDirection_In_c);
        (void)USB_Class_CDC_Interface_CIC_Send_Data(controllerId, gSerialState,
                                                    (gUSB_CDC_Size_NotifPacketSize_d + gUSB_CDC_UARTBitmapSize_d));
    }

}
#endif

#endif//gUsbCdcEnabled_d