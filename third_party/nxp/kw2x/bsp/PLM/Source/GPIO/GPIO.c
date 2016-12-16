/******************************************************************************
* Filename: GPIO.c
*
* Description: GPIO driver implementation file for ARM CORTEX-M4 processor
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

#include "PortConfig.h"
#include "GPIO.h"

#if gGPIO_Enabled_d

/*****************************************************************************
 *****************************************************************************
 * Private memory definitions
 *****************************************************************************
 *****************************************************************************/

/* Array initializer of GPIO peripheral base pointers */
#ifndef GPIO_BASE_PTRS
#define GPIO_BASE_PTRS   { PTA_BASE_PTR,\
                           PTB_BASE_PTR,\
                           PTC_BASE_PTR,\
                           PTD_BASE_PTR,\
                           PTE_BASE_PTR }
#endif

/* Array initializer of PORT peripheral base pointers */
#ifndef PORT_BASE_PTRS
#define PORT_BASE_PTRS   { PORTA_BASE_PTR,\
                           PORTB_BASE_PTR,\
                           PORTC_BASE_PTR,\
                           PORTD_BASE_PTR,\
                           PORTE_BASE_PTR }
#endif

/*
 * Name: mGPIO_MemPtrs
 * Description: GPIO memory map pointers
 */
GPIO_MemMapPtr mGPIO_MemPtrs[] = GPIO_BASE_PTRS;

/*
 * Name: mGPIO_PortMemPtrs
 * Description: GPIO PORT memory map pointers
 */
PORT_MemMapPtr mGPIO_PortMemPtrs[] = PORT_BASE_PTRS;

/*****************************************************************************
 *****************************************************************************
 * Private prototypes
 *****************************************************************************
 *****************************************************************************/

/* NONE */

/*****************************************************************************
 *****************************************************************************
 * Private functions
 *****************************************************************************
 *****************************************************************************/

/* NONE */

/*****************************************************************************
 ******************************************************************************
 * Public functions
 ******************************************************************************
 *****************************************************************************/

/*---------------------------------------------------------------------------
 * NAME: GPIO_InitPort
 * DESCRIPTION: Initialize the specified GPIO port with initial data
 * PARAMETERS: [IN] portID - the port ID to be initialized
 *             [IN] pInitData - the initialization data
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrNULLPtr_c - in case of null pointer
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_InitPort
(
    GpioPortID_t portID,
    GpioPortConfig_t *pInitData
)
{
    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (NULL == pInitData)
    {
        return gGpioErrNULLPtr_c;
    }

    /* enable clock gating for the specified port */
    gGPIOClockGatingReg_c |= (1 << (gGPIOPortAClockGatingBit_c + portID));

    /* setup port direction and port data, preserving the unaffected bits */
    GPIO_PDDR_REG(mGPIO_MemPtrs[portID]) &= (pInitData->portDir | (~pInitData->portMask));
    GPIO_PDOR_REG(mGPIO_MemPtrs[portID]) &= (pInitData->portData | (~pInitData->portMask));

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_DeInitPort
 * DESCRIPTION: Resets the specified port and disables the port clock gating
 * PARAMETERS: [IN] portID
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_DeInitPort
(
    GpioPortID_t portID
)
{
    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    /* clear all GPIO port registers */
    GPIO_PDOR_REG(mGPIO_MemPtrs[portID]) = 0;
    GPIO_PSOR_REG(mGPIO_MemPtrs[portID]) = 0;
    GPIO_PCOR_REG(mGPIO_MemPtrs[portID]) = 0;
    GPIO_PTOR_REG(mGPIO_MemPtrs[portID]) = 0;
    GPIO_PDIR_REG(mGPIO_MemPtrs[portID]) = 0;
    GPIO_PDDR_REG(mGPIO_MemPtrs[portID]) = 0;

    /* disable clock gating for the specified port */
    gGPIOClockGatingReg_c &= ~(1 << (gGPIOPortAClockGatingBit_c + portID));

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_EnablePortClockGating
 * DESCRIPTION: Enable the clock gating for the selected port
 * PARAMETERS: [IN] portID - the ID of the port
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_EnablePortClockGating
(
    GpioPortID_t portID
)
{
    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        /* enable clock gating for the specified port */
        gGPIOClockGatingReg_c |= (1 << (gGPIOPortAClockGatingBit_c + portID));
    }

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_DisablePortClockGating
 * DESCRIPTION: Disables the clock gating for the selected port
 * PARAMETERS: [IN] portID - the ID of the port
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_DisablePortClockGating
(
    GpioPortID_t portID
)
{
    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if ((gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        /* disable clock gating for the specified port */
        gGPIOClockGatingReg_c &= ~(1 << (gGPIOPortAClockGatingBit_c + portID));
    }

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_SetPortDir
 * DESCRIPTION: Set port data direction
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] mask - the port mask (affected pins)
 *             [IN] value - port direction value to be set
 *                          (bit=1 means OUTPUT, bit=0 means INPUT)
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_SetPortDir
(
    GpioPortID_t portID,
    uint32_t mask,
    uint32_t value
)
{
    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    /* setup port direction and port data, preserving the unaffected bits */
    GPIO_PDDR_REG(mGPIO_MemPtrs[portID]) &= (value | (~mask));

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_GetPortDir
 * DESCRIPTION: Get the specified port data direction
 * PARAMETERS: [IN] portID - the ID of the port
 *             [OUT] pData - pointer to a memory location where the port data
 *                           direction will be stored
 *
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrNULLPtr_c in case of null pointer provided
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_GetPortDir
(
    GpioPortID_t portID,
    uint32_t *pData
)
{
    if (NULL == pData)
    {
        return gGpioErrNULLPtr_c;
    }

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    *pData = GPIO_PDDR_REG(mGPIO_MemPtrs[portID]);

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_SetPinDir
 * DESCRIPTION: Set pin port data direction
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [IN] pinDir - the pin data direction to be set
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrInvalidDirection_c in case of invalid direction
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_SetPinDir
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    GpioDirection_t pinDir
)
{
    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (pinDir >= gGpioDirMax_c)
    {
        return gGpioErrInvalidDirection_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    if (pinDir == gGpioDirIn_c)
    {
        /* setup pin direction [INPUT], preserving the unaffected bits */
        GPIO_PDDR_REG(mGPIO_MemPtrs[portID]) &= (uint32_t)(~(1 << pinNo));
    }
    else
    {
        /* setup pin direction [OUTPUT], preserving the unaffected bits */
        GPIO_PDDR_REG(mGPIO_MemPtrs[portID]) |= (uint32_t)(1 << pinNo);
    }

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_GetPinDir
 * DESCRIPTION: Get pin port data direction
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [OUT] pData - pointer to a memory location where the pin
 *                           direction will be stored
 *
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrNULLPtr_c in case of null pointer provided
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_GetPinDir
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    GpioDirection_t *pData
)
{
    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (NULL == pData)
    {
        return gGpioErrNULLPtr_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    if ((GPIO_PDDR_REG(mGPIO_MemPtrs[portID]) & (1 << pinNo)))
    {
        *pData = gGpioDirOut_c;
    }
    else
    {
        *pData = gGpioDirIn_c;
    }

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_SetPinPortAttr
 * DESCRIPTION: Set pin port attributes (see GpioPinConfigAttr_t for details)
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [IN] pConfig - pointer to a memory location where the
 *                            configuration to be applied is stored
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrNULLPtr_c in case of null pointer provided
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *         - gGpioErrInvalidPinAttr in case of an invalid pin attribute
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_SetPinPortAttr
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    GpioPinConfigAttr_t *pConfig
)
{
    GpioPinCtrlRegMapping_t pinCtrlRegValue;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (NULL == pConfig)
    {
        return gGpioErrNULLPtr_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    if ((pConfig->intCtrl >= gGPIOIntCfgMax_c) ||
        (pConfig->muxCtrl >= gGpioFunctionModeMax_c) ||
        (pConfig->driveStrength >= gGPIODriveStrengthMax_c) ||
        (pConfig->pullSelect >= gGPIOInternalPullMax_c))
    {
        return gGpioErrInvalidPinAttr;
    }

    pinCtrlRegValue.bits.IRQC = ((pConfig->intCtrl) & gGPIO_IRQC_Mask_c);
    pinCtrlRegValue.bits.LK = ((pConfig->lockReg) & gGPIO_LK_Mask_c);
    pinCtrlRegValue.bits.MUX = ((pConfig->muxCtrl) & gGPIO_MUX_Mask_c);
    pinCtrlRegValue.bits.DSE = ((pConfig->driveStrength) & gGPIO_DSE_Mask_c);
    pinCtrlRegValue.bits.ODE = ((pConfig->openDrainEnable) & gGPIO_ODE_Mask_c);
    pinCtrlRegValue.bits.PFE = ((pConfig->passiveFilterEnable) & gGPIO_PFE_Mask_c);
    pinCtrlRegValue.bits.SRE = ((pConfig->slewRateEnable) & gGPIO_SRE_Mask_c);
    pinCtrlRegValue.bits.PE = ((pConfig->pullEnable) & gGPIO_PE_Mask_c);
    pinCtrlRegValue.bits.PS = ((pConfig->pullSelect) & gGPIO_PS_Mask_c);

    PORT_PCR_REG(mGPIO_PortMemPtrs[portID], pinNo) = pinCtrlRegValue.regValue;

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_GetPinPortAttr
 * DESCRIPTION: Get pin port attributes (see GpioPinConfigAttr_t for details)
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [OUT] pConfig - pointer to a memory location where the
 *                             configuration will be stored
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrNULLPtr_c in case of null pointer provided
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_GetPinPortAttr
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    GpioPinConfigAttr_t *pConfig
)
{
    GpioPinCtrlRegMapping_t pinCtrlRegValue;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (NULL == pConfig)
    {
        return gGpioErrNULLPtr_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    pinCtrlRegValue.regValue = PORT_PCR_REG(mGPIO_PortMemPtrs[portID], pinNo);

    pConfig->intCtrl = (GpioPinInterruptConfig_t)pinCtrlRegValue.bits.IRQC ;
    pConfig->lockReg = pinCtrlRegValue.bits.LK ;
    pConfig->muxCtrl = (GpioPinFunctionMode_t)pinCtrlRegValue.bits.MUX ;
    pConfig->driveStrength = (GpioDrvStrength_t)pinCtrlRegValue.bits.DSE;
    pConfig->openDrainEnable = pinCtrlRegValue.bits.ODE;
    pConfig->passiveFilterEnable = pinCtrlRegValue.bits.PFE;
    pConfig->slewRateEnable = (GpioPinSlewRate_t)pinCtrlRegValue.bits.SRE;
    pConfig->pullEnable = pinCtrlRegValue.bits.PE;
    pConfig->pullSelect = (GpioPullSelect_t)pinCtrlRegValue.bits.PS;

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_SetPortData
 * DESCRIPTION: Self explanatory
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] portData - the data to be written to the port
 *             [IN] mask - port mask (affected bits)
 * RETURN:  One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_SetPortData
(
    GpioPortID_t portID,
    uint32_t portData,
    uint32_t mask
)
{
    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_PSOR_REG(mGPIO_MemPtrs[portID]) = (portData & mask);
    GPIO_PCOR_REG(mGPIO_MemPtrs[portID]) = ((~portData) & mask);

    return gGpioErrNoError_c;
}


/*---------------------------------------------------------------------------
 * NAME: GPIO_GetPortData
 * DESCRIPTION: self explanatory
 * PARAMETERS: [IN] portID - the ID of the port
 *             [OUT] portData - a pointer to a memory location where port
 *                              port data will be stored
 * RETURN:  One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrNULLPtr_c in case of null pointer provided
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_GetPortData
(
    GpioPortID_t portID,
    uint32_t *portData
)
{
    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (NULL == portData)
    {
        return gGpioErrNULLPtr_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    (*portData) = GPIO_PDIR_REG(mGPIO_MemPtrs[portID]);

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_SetPinData
 * DESCRIPTION: self explanatory
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [IN] pinLevel - the level (low/high) of the pin to be set
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_SetPinData
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    GpioPinLevel_t pinLevel
)
{
    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (pinLevel >= gGpioPinLevelMax_c)
    {
        return gGpioErrInvalidPinLevel_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    if (pinLevel == gGpioPinLow_c)
    {
        GPIO_PCOR_REG(mGPIO_MemPtrs[portID]) |= (1 << pinNo);
    }
    else
    {
        GPIO_PSOR_REG(mGPIO_MemPtrs[portID]) |= (1 << pinNo);
    }

    return gGpioErrNoError_c;
}


/*---------------------------------------------------------------------------
 * NAME: GPIO_GetPinData
 * DESCRIPTION: self explanatory
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [OUT] pinLevel - pointer to a memory location where the pin
 *                              level will be stored
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrNULLPtr_c in case of null pointer provided
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_GetPinData
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    GpioPinLevel_t *pinLevel
)
{
    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (NULL == pinLevel)
    {
        return gGpioErrNULLPtr_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    if (GPIO_PDIR_REG(mGPIO_MemPtrs[portID]) & (1 << pinNo))
    {
        (*pinLevel) = gGpioPinHigh_c;
    }
    else
    {
        (*pinLevel) = gGpioPinLow_c;
    }

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_TogglePortData
 * DESCRIPTION: self explanatory
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] mask - the mask of the port (affected bits)
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_TogglePortData
(
    GpioPortID_t portID,
    uint32_t mask
)
{
    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_PTOR_REG(mGPIO_MemPtrs[portID]) = mask;

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_TogglePinData
 * DESCRIPTION: self explanatory
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_TogglePinData
(
    GpioPortID_t portID,
    GpioPin_t pinNo
)
{
    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    /* toggle pin */
    GPIO_PTOR_REG(mGPIO_MemPtrs[portID]) = (1 << pinNo);

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_EnablePinPullup
 * DESCRIPTION: enables or disables the pin pull (up/down)
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [IN] enable - if TRUE enables the pin pull, if FALSE disables
 *                           the pin pull
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_EnablePinPull
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    bool_t enable
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    config.pullEnable = enable;
    GPIO_SetPinPortAttr(portID, pinNo, &config);

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_IsPinPullupEnabled
 * DESCRIPTION: Checks if the pin pull is enabled for the specified pin port
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [IN] state - pointer to a memory location where the pin pull
 *                          state will be stored
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *         - gGpioErrNULLPtr_c in case of null pointer provided
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_IsPinPullupEnabled
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    bool_t *state
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (NULL == state)
    {
        return gGpioErrNULLPtr_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    (*state) = config.pullEnable;

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_SelectPinPull
 * DESCRIPTION: select pin pull mode: pull-up / pull-down
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [IN] pullSel - the pin pull selection value
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *         - gGpioErrInvalidPinAttr in case of invalid pin attribute
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_SelectPinPull
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    GpioPullSelect_t pullSel
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (pullSel >= gGPIOInternalPullMax_c)
    {
        return gGpioErrInvalidPinAttr;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    config.pullSelect = pullSel;
    GPIO_SetPinPortAttr(portID, pinNo, &config);

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_GetPinPullSelect
 * DESCRIPTION: get the pin pull selected mode: pull-up / pull-down
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [IN] pullSel - pointer to a memory location where the the pin
 *                            pull select will be stored
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *         - gGpioErrNULLPtr_c in case of null pointer provided
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_GetPinPullSelect
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    GpioPullSelect_t *pullSel
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (NULL == pullSel)
    {
        return gGpioErrNULLPtr_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    (*pullSel) = config.pullSelect;

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_SetPinFunction
 * DESCRIPTION: Set pin function (configures pin MUXes)
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [IN] pinFunc - the pin function to be set
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *         - gGpioErrInvalidPinAttr in case of invalid pin attribute
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_SetPinFunction
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    GpioPinFunctionMode_t pinFunc
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (pinFunc >= gGpioFunctionModeMax_c)
    {
        return gGpioErrInvalidPinAttr;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    config.muxCtrl = pinFunc;
    GPIO_SetPinPortAttr(portID, pinNo, &config);

    return gGpioErrNoError_c;

}

/*---------------------------------------------------------------------------
 * NAME: GPIO_GetPinFunction
 * DESCRIPTION: get pin function (MUX mode)
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [IN] pinFunc - pointer to a memory location where the pin
 *                            function will be stored
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *         - gGpioErrNULLPtr_c in case of null pointer provided
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_GetPinFunction
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    GpioPinFunctionMode_t *pinFunc
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (NULL == pinFunc)
    {
        return gGpioErrNULLPtr_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    (*pinFunc) = config.muxCtrl;

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_SetPinInterruptControl
 * DESCRIPTION: Setup the pin interrupt control
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [IN] pinIRQC - the pin interrupt control to be set
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *         - gGpioErrInvalidPinAttr in case of invalid pin attribute
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_SetPinInterruptControl
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    GpioPinInterruptConfig_t pinIRQC
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (pinIRQC >= gGPIOIntCfgMax_c)
    {
        return gGpioErrInvalidPinAttr;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    config.intCtrl = pinIRQC;
    GPIO_SetPinPortAttr(portID, pinNo, &config);

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_GetPinInterruptControl
 * DESCRIPTION: get the pin interrupt control value
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [OUT] pinIRQC - pointer to a memory location where the pin
 *                            interrupt control value will be stored
 * RETURN:  One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *         - gGpioErrNULLPtr_c in case of null pointer provided
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_GetPinInterruptControl
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    GpioPinInterruptConfig_t *pinIRQC
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (NULL == pinIRQC)
    {
        return gGpioErrNULLPtr_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    (*pinIRQC) = config.intCtrl;

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_LockPinCtrlReg
 * DESCRIPTION: lock the pin control register [bits 15..0] of the specified
 *              pin port
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_LockPinCtrlReg
(
    GpioPortID_t portID,
    GpioPin_t pinNo
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    config.lockReg = TRUE;
    GPIO_SetPinPortAttr(portID, pinNo, &config);

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_IsPinCtrlRegLocked
 * DESCRIPTION: checks if the pin control register of the specified port is
 *              locked
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [OUT] pLock - pointer to a memory location where the lock value
 *                           will be stored
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *         - gGpioErrNULLPtr_c in case of null pointer provided
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_IsPinCtrlRegLocked
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    bool_t *pLock
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (NULL == pLock)
    {
        return gGpioErrNULLPtr_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    *pLock = config.lockReg;

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_EnableDriveStrength
 * DESCRIPTION: enables or disables the pin drive strength (when configured
 *              as digital output)
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [IN] enable - if TRUE enables the pin drive strength
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_EnableDriveStrength
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    bool_t enable
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    config.driveStrength = (GpioDrvStrength_t)enable;
    GPIO_SetPinPortAttr(portID, pinNo, &config);

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_IsDriveStrengthEnabled
 * DESCRIPTION: checks if the pin drive strength is enabled
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [OUT] enable - pointer to a memory location where the pin
 *                            drive strength value (TRUE/FALSE)  will be
 *                            stored
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *         - gGpioErrNULLPtr_c in case of null pointer provided
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_IsDriveStrengthEnabled
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    bool_t *enable
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (NULL == enable)
    {
        return gGpioErrNULLPtr_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    (*enable) = config.driveStrength;

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_EnablePassiveFilter
 * DESCRIPTION: enables or disables the passive filter (when configured
 *              as digital input)
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [IN] enable - if TRUE enables the pin passive filter
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_EnablePassiveFilter
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    bool_t enable
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    config.passiveFilterEnable = enable;
    GPIO_SetPinPortAttr(portID, pinNo, &config);

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_IsPassiveFilterEnabled
 * DESCRIPTION: checks if the pin passive filter is enabled
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [OUT] enable - pointer to a memory location where the pin
 *                            passive filter value (TRUE/FALSE) will be
 *                            stored
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *         - gGpioErrNULLPtr_c in case of null pointer provided
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_IsPassiveFilterEnabled
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    bool_t *enable
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (NULL == enable)
    {
        return gGpioErrNULLPtr_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    (*enable) = config.passiveFilterEnable;

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_EnableSlewRate
 * DESCRIPTION: enables or disables the slew rate
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [IN] slrValue - slew rate value
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *         - gGpioErrInvalidPinAttr in case of invalid pin attribute
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_EnableSlewRate
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    GpioPinSlewRate_t slrValue
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (slrValue >= gGPIOSlewRateMax_c)
    {
        return gGpioErrInvalidPinAttr;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    config.slewRateEnable = slrValue;
    GPIO_SetPinPortAttr(portID, pinNo, &config);

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_IsSlewRateEnabled
 * DESCRIPTION: checks if the pin slew rate is enabled
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [OUT] slrValue - pointer to a memory location where the
 *                              slew rate value (TRUE/FALSE) will be stored
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *         - gGpioErrNULLPtr_c in case of null pointer provided
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_IsSlewRateEnabled
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    GpioPinSlewRate_t *slrPtr
)
{
    GpioPinConfigAttr_t config;

    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (NULL == slrPtr)
    {
        return gGpioErrNULLPtr_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    GPIO_GetPinPortAttr(portID, pinNo, &config);
    (*slrPtr) = config.slewRateEnable;

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_GetPinInterruptStatus
 * DESCRIPTION: Get pin interrupt status
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 *             [IN] pStatus - pointer to a memory location where the
 *                            pin interrupt status will be stored
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrNULLPtr_c in case of null pointer provided
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_GetPinInterruptStatus
(
    GpioPortID_t portID,
    GpioPin_t pinNo,
    GpioPinInterruptStatus_t *pStatus
)
{
    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    if (NULL == pStatus)
    {
        return gGpioErrNULLPtr_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    if (PORT_ISFR_REG(mGPIO_PortMemPtrs[portID]) & (1 << pinNo))
    {
        *pStatus = gGpioPinInterruptSet_c;
    }
    else
    {
        *pStatus = gGpioPinNoInterruptSet_c;
    }

    return gGpioErrNoError_c;
}

/*---------------------------------------------------------------------------
 * NAME: GPIO_ClearPinInterruptFlag
 * DESCRIPTION: Clear the pin interrupt status
 * PARAMETERS: [IN] portID - the ID of the port
 *             [IN] pinNo - the pin number, valid range [0..31]
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrClkGatingDisabled if port has clock gating disabled
 *         - gGpioErrInvalidPortID_c in case of invalid port ID
 *         - gGpioErrInvalidPin_c in case of pin number out of range
 *---------------------------------------------------------------------------*/
GpioErr_t GPIO_ClearPinInterruptFlag
(
    GpioPortID_t portID,
    GpioPin_t pinNo
)
{
    if (portID >= gGpioPortMax_c)
    {
        return gGpioErrInvalidPortID_c;
    }

    if (pinNo >= gGpioPinMax_c)
    {
        return gGpioErrInvalidPin_c;
    }

    /* check if clock gating for the specified port is enabled */
    if (!(gGPIOClockGatingReg_c & (1 << (gGPIOPortAClockGatingBit_c + portID))))
    {
        return gGpioErrClkGatingDisabled;
    }

    PORT_ISFR_REG(mGPIO_PortMemPtrs[portID]) |= (1 << pinNo);

    return gGpioErrNoError_c;
}

#endif