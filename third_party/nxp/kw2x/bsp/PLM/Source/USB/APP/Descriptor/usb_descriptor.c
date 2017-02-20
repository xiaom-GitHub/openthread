/******************************************************************************
* This is the generic descriptor file
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
#include "EmbeddedTypes.h"
#include "usb_class.h"
#include "usb_descriptor.h"
#include "USB.h"
#include "USB_Configuration.h"

#if gUsbIncluded_d
/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/


/****************************************************************************
 * Global Variables
 ****************************************************************************/

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Local Variables - None
 *****************************************************************************/

/*****************************************************************************
* Local Functions - None
*****************************************************************************/

/*****************************************************************************
 * Global Functions
 *****************************************************************************/

/*****************************************************************************/
uint8_t USB_Desc_GetDescriptor(
    uint8_t controllerId,   /* [IN]  Controller ID */
    uint8_t type,            /* [IN]  Type of descriptor requested */
    uint8_t strNum,         /* [IN]  String index for string descriptor */
    uint16_t index,          /* [IN]  String descriptor language Id */
    uint8_t * *pDescriptor, /* [OUT] Output descriptor pointer */
    usbPacketSize_t *pSize   /* [OUT] Size of descriptor returned */
)
{
    uint8_t status = gUsbErr_InvalidReqType_c;
    pf_USB_Desc_GetDescriptor_t pfGD;
    pfGD = gpaUsbClassDescriptor[controllerId]->pfUSB_Desc_GetDescriptor;

    if (pfGD != NULL)
    {
        status = (*pfGD)(controllerId, type, strNum, index, pDescriptor, pSize);
    }

    return status;

}

/*****************************************************************************/
uint8_t USB_Desc_GetInterface
(
    uint8_t controllerId,     /* [IN] Controller ID */
    uint8_t interface,         /* [IN] Interface number */
    uint8_t *altInterface  /* [OUT] Output alternate interface */
)
{
    uint8_t status = gUsbErr_InvalidReqType_c;
    pf_USB_Desc_GetInterface_t pfGI;
    pfGI = gpaUsbClassDescriptor[controllerId]->pf_USB_Desc_GetInterface;

    if (pfGI != NULL)
    {
        status = (*pfGI)(controllerId, interface, altInterface);
    }

    return status;
}

/*****************************************************************************/
uint8_t USB_Desc_SetInterface
(
    uint8_t controllerId, /* [IN] Controller ID */
    uint8_t interface,     /* [IN] Interface number */
    uint8_t altInterface  /* [IN] Input alternate interface */
)
{
    uint8_t status = gUsbErr_InvalidReqType_c;
    pf_USB_Desc_SetInterface_t pfSI;
    pfSI = gpaUsbClassDescriptor[controllerId]->pf_USB_Desc_SetInterface;

    if (pfSI != NULL)
    {
        status = (*pfSI)(controllerId, interface, altInterface);
    }

    /* if interface valid */
    return status;
}

/*****************************************************************************/
bool_t USB_Desc_ValidConfiguration
(
    uint8_t controllerId,/*[IN] Controller ID */
    uint16_t configVal   /*[IN] Configuration value */
)
{

    bool_t status = FALSE;
    pf_USB_Desc_ValidConfiguration_t pfVC;
    pfVC = gpaUsbClassDescriptor[controllerId]->pf_USB_Desc_ValidConfiguration;

    if (pfVC != NULL)
    {
        status = (*pfVC)(controllerId, configVal);
    }

    return status;
}

/*****************************************************************************/
bool_t USB_Desc_ValidInterface
(
    uint8_t controllerId, /*[IN] Controller ID */
    uint8_t interface      /*[IN] Target interface */
)
{
    bool_t status = FALSE;
    pf_USB_Desc_ValidInterface_t pfVI;
    pfVI = gpaUsbClassDescriptor[controllerId]->pf_USB_Desc_ValidInterface;

    if (pfVI != NULL)
    {
        status = (*pfVI)(controllerId, interface);
    }

    return status;
}

/*****************************************************************************/
bool_t USB_Desc_RemoteWakeup
(
    uint8_t controllerId      /* [IN] Controller ID */
)
{
    bool_t status = FALSE;
    pf_USB_Desc_RemoteWakeup_t  pfRW;
    pfRW = gpaUsbClassDescriptor[controllerId]->pf_USB_Desc_RemoteWakeup;

    if (pfRW != NULL)
    {
        status = (*pfRW)(controllerId);
    }

    return status;
}

#endif
