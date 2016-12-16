/******************************************************************************
* Filename: GPIO_Interface.h
*
* Description: GPIO driver interface header file for ARM CORTEX-M4 processor
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

#ifndef _GPIO_INTERFACE_H_
#define _GPIO_INTERFACE_H_

#include "EmbeddedTypes.h"
#include "AppToPlatformConfig.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
 ******************************************************************************
 * Public macros
 ******************************************************************************
 *****************************************************************************/

/*
 * Name: gGPIO_Enabled_d
 * Description: enables / disables the GPIO module code generation
 */
#ifndef gGPIO_Enabled_d
#define gGPIO_Enabled_d    1
#endif

/*
 * Name: gGPIO_GetPinInterruptStatus
 * Description: macro used to get the pin interrupt status
 */
#define gGPIO_GetPinInterruptStatus(port, pin)  ((PORT_ISFR_REG(port) & (1<<pin)) ? (TRUE) : (FALSE))
#define gGPIO_ClearPinInterruptStatusFlag(port, pin)    (PORT_ISFR_REG(port) &= (1<<pin))

/*****************************************************************************
 ******************************************************************************
 * Public type definitions
 ******************************************************************************
 *****************************************************************************/

/*
 * Name: GpioErr_t
 * Description: GPIO module error codes
 */
typedef enum 
{
    gGpioErrNoError_c = 0,
    gGpioErrInvalidParamater_c,
    gGpioErrInvalidPortID_c,
    gGpioErrInvalidPin_c,
    gGpioErrInvalidDirection_c,
    gGpioErrInvalidPinLevel_c,    
    gGpioErrClkGatingDisabled,
    gGpioErrInvalidPinAttr,
    gGpioErrNULLPtr_c,    
} GpioErr_t;

/*
 * Name: GpioPortID_t
 * Description: GPIO ports ID
 */
typedef enum
{
    gGpioPortA_c = 0,
    gGpioPortB_c,
    gGpioPortC_c,
    gGpioPortD_c,
    gGpioPortE_c,
    gGpioPortF_c,
    gGpioPortMax_c
} GpioPortID_t;

/*
 * Name: GpioDirection_t
 * Description: GPIO port direction
 */
typedef enum 
{
    gGpioDirIn_c = 0,
    gGpioDirOut_c,
    gGpioDirMax_c
} GpioDirection_t;

/*
 * Name: GpioPinLevel_t
 * Description: GPIO pin levels
 */
typedef enum {
    gGpioPinLow_c = 0,
    gGpioPinHigh_c,
    gGpioPinLevelMax_c
} GpioPinLevel_t;

/*
 * Name: GpioPinInterruptStatus_t
 * Description: GPIO pin interrupt status
 */
typedef enum {
    gGpioPinNoInterruptSet_c = 0,
    gGpioPinInterruptSet_c    
} GpioPinInterruptStatus_t;

/*
 * Name: GpioPinFunctionMode_t
 * Description: GPIO pin function modes (MUX modes)
 */
typedef enum {
    gGpioAlternate0_c = 0,
    gGpioAlternate1_c,
    gGpioAlternate2_c,
    gGpioAlternate3_c,
    gGpioAlternate4_c,
    gGpioAlternate5_c,
    gGpioAlternate6_c,
    gGpioAlternate7_c,
    gGpioEzPortMode_c,
    gGpioFunctionModeMax_c
} GpioPinFunctionMode_t;

/*
 * Name: GpioPinInterruptConfig_t
 * Description: GPIO pin interrupt configuration
 */
typedef enum {
    gGPIOIntDisable_c = 0,
    gGPIODmaReqOnRisingEdge_c,
    gGPIODmaReqOnFallingEdge_c,
    gGPIODmaReqOnEitherEdge_c,
    gGPIOIrqLogicZero_c = 8,
    gGPIOIrqOnRisingEdge_c,
    gGPIOIrqOnFallingEdge_c,
    gGPIOIrqOnEitherEdge_c,
    gGPIOIrqLogicOne_c,
    gGPIOIntCfgMax_c
} GpioPinInterruptConfig_t;

/*
 * Name: GpioPin_t
 * Description: GPIO port pins enumeration
 */
typedef enum {
    gGpioPin0_c = 0,  gGpioPin1_c,  gGpioPin2_c,  gGpioPin3_c,
    gGpioPin4_c,  gGpioPin5_c,  gGpioPin6_c,  gGpioPin7_c,
    gGpioPin8_c,  gGpioPin9_c,  gGpioPin10_c, gGpioPin11_c,
    gGpioPin12_c, gGpioPin13_c, gGpioPin14_c, gGpioPin15_c,
    gGpioPin16_c, gGpioPin17_c, gGpioPin18_c, gGpioPin19_c,
    gGpioPin20_c, gGpioPin21_c, gGpioPin22_c, gGpioPin23_c,
    gGpioPin24_c, gGpioPin25_c, gGpioPin26_c, gGpioPin27_c,
    gGpioPin28_c, gGpioPin29_c, gGpioPin30_c, gGpioPin31_c, 
    gGpioPinMax_c
} GpioPin_t;

/*
 * Name: GpioDrvStrength_t
 * Description: GPIO pin drive strength
 */
typedef enum {
    gGPIOLowDriveStrength_c = 0,
    gGPIOHighDriveStrength_c,
    gGPIODriveStrengthMax_c,
} GpioDrvStrength_t;

/*
 * Name: GpioPullSelect_t
 * Description: GPIO pull select
 */
typedef enum {
    gGPIOInternalPulldown_c = 0,
    gGPIOInternalPullup_c,
    gGPIOInternalPullMax_c,
} GpioPullSelect_t;

/*
 * Name: GpioPinSlewRate_t
 * Description: GPIO slew rate
 */
typedef enum {
    gGPIOSlowSlewRate_c = 0,
    gGPIOFastSlewRate_c,
    gGPIOSlewRateMax_c
} GpioPinSlewRate_t;

/*
 * Name: GpioPortConfig_t
 * Description: GPIO port configuration structure
 */
typedef struct
{
    uint32_t portDir;    
    uint32_t portData;
    uint32_t portMask;
} GpioPortConfig_t;

/*
 * Name: GpioPinConfigAttr_t
 * Description: GPIO pin configuration data structure
 */
typedef struct 
{
    GpioPinInterruptConfig_t intCtrl;
    bool_t lockReg;
    GpioPinFunctionMode_t muxCtrl;
    GpioDrvStrength_t driveStrength;
    bool_t openDrainEnable;
    bool_t passiveFilterEnable;
    GpioPinSlewRate_t slewRateEnable;
    bool_t pullEnable;
    GpioPullSelect_t pullSelect;
} GpioPinConfigAttr_t;

/*****************************************************************************
 ******************************************************************************
 * Public memory declarations
 ******************************************************************************
 *****************************************************************************/

/* none */

/*****************************************************************************
 *****************************************************************************
 * Public prototypes
 *****************************************************************************
 *****************************************************************************/

#if gGPIO_Enabled_d

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
extern GpioErr_t GPIO_InitPort
(
        GpioPortID_t portID, 
        GpioPortConfig_t* pInitData
);

/*---------------------------------------------------------------------------
 * NAME: GPIO_DeInitPort
 * DESCRIPTION: Resets the specified port and disables the port clock gating  
 * PARAMETERS: [IN] portID
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrInvalidPortID_c in case of invalid port ID   
 *---------------------------------------------------------------------------*/
extern GpioErr_t GPIO_DeInitPort
(
        GpioPortID_t portID
);

/*---------------------------------------------------------------------------
 * NAME: GPIO_EnablePortClockGating
 * DESCRIPTION: Enable the clock gating for the selected port
 * PARAMETERS: [IN] portID - the ID of the port
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrInvalidPortID_c in case of invalid port ID  
 *---------------------------------------------------------------------------*/
extern GpioErr_t GPIO_EnablePortClockGating
(
        GpioPortID_t portID
);

/*---------------------------------------------------------------------------
 * NAME: GPIO_DisablePortClockGating
 * DESCRIPTION: Disables the clock gating for the selected port
 * PARAMETERS: [IN] portID - the ID of the port
 * RETURN: One of the following:
 *         - gGpioErrNoError_c in case of NO ERROR
 *         - gGpioErrInvalidPortID_c in case of invalid port ID  
 *---------------------------------------------------------------------------*/
extern GpioErr_t GPIO_DisablePortClockGating
(
        GpioPortID_t portID
);

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
extern GpioErr_t GPIO_SetPortDir
(
        GpioPortID_t portID,
        uint32_t mask,
        uint32_t value
);

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
extern GpioErr_t GPIO_GetPortDir
(
        GpioPortID_t portID,        
        uint32_t* pData
);

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
extern GpioErr_t GPIO_SetPinDir
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        GpioDirection_t pinDir
);

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
extern GpioErr_t GPIO_GetPinDir
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        GpioDirection_t* pData
);

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
extern GpioErr_t GPIO_SetPinPortAttr
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        GpioPinConfigAttr_t* pConfig 
);

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
extern GpioErr_t GPIO_GetPinPortAttr
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        GpioPinConfigAttr_t* pConfig 
);


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
extern GpioErr_t GPIO_SetPortData
(
        GpioPortID_t portID,
        uint32_t portData,
        uint32_t mask
);

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
extern GpioErr_t GPIO_GetPortData
(
        GpioPortID_t portID,
        uint32_t* portData        
);

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
extern GpioErr_t GPIO_SetPinData
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        GpioPinLevel_t pinLevel
);

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
extern GpioErr_t GPIO_GetPinData
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        GpioPinLevel_t* pinLevel
);

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
extern GpioErr_t GPIO_TogglePortData
(
        GpioPortID_t portID,
        uint32_t mask
);

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
extern GpioErr_t GPIO_TogglePinData
(
        GpioPortID_t portID,
        GpioPin_t pinNo
);


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
extern GpioErr_t GPIO_EnablePinPull
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        bool_t enable
);

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
extern GpioErr_t GPIO_IsPinPullupEnabled
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        bool_t* state
);


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
extern GpioErr_t GPIO_SelectPinPull
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        GpioPullSelect_t pullSel
);


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
extern GpioErr_t GPIO_GetPinPullSelect
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        GpioPullSelect_t* pullSel
);


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
extern GpioErr_t GPIO_SetPinFunction
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        GpioPinFunctionMode_t pinFunc
);

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
extern GpioErr_t GPIO_GetPinFunction
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        GpioPinFunctionMode_t* pinFunc
);

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
extern GpioErr_t GPIO_SetPinInterruptControl
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        GpioPinInterruptConfig_t pinIRQC
);

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
extern GpioErr_t GPIO_GetPinInterruptControl
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        GpioPinInterruptConfig_t* pinIRQC
);

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
extern GpioErr_t GPIO_LockPinCtrlReg
(
        GpioPortID_t portID,
        GpioPin_t pinNo    
);


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
extern GpioErr_t GPIO_IsPinCtrlRegLocked
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        bool_t* pLock 
);

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
extern GpioErr_t GPIO_EnableDriveStrength
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        bool_t enable 
);

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
extern GpioErr_t GPIO_IsDriveStrengthEnabled
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        bool_t* enable 
);

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
extern GpioErr_t GPIO_EnablePassiveFilter
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        bool_t enable 
);

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
extern GpioErr_t GPIO_IsPassiveFilterEnabled
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        bool_t* enable 
);

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
extern GpioErr_t GPIO_EnableSlewRate
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        GpioPinSlewRate_t slrValue 
);

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
extern GpioErr_t GPIO_IsSlewRateEnabled
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        GpioPinSlewRate_t* slrPtr 
);

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
extern GpioErr_t GPIO_GetPinInterruptStatus
(
        GpioPortID_t portID,
        GpioPin_t pinNo,
        GpioPinInterruptStatus_t* pStatus
);

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
extern GpioErr_t GPIO_ClearPinInterruptFlag
(
        GpioPortID_t portID,
        GpioPin_t pinNo
);

#else
/* Stubs */
#define GpioErr_t GPIO_InitPort(portID, pInitData)    0
#define GpioErr_t GPIO_DeInitPort(portID)    0
#define GpioErr_t GPIO_EnablePortClockGating(portID)   0
#define GpioErr_t GPIO_DisablePortClockGating(portID)   0
#define GpioErr_t GPIO_SetPortDir(portID, mask, value)    0
#define GpioErr_t GPIO_GetPortDir(portID, pData)    0
#define GpioErr_t GPIO_SetPinDir(portID, pinNo, pinDir)    0
#define GpioErr_t GPIO_GetPinDir(portID, pinNo, pData)    0
#define GpioErr_t GPIO_SetPinPortAttr(portID, pinNo, pConfig)    0
#define GpioErr_t GPIO_GetPinPortAttr(portID, pinNo, pConfig)    0
#define GpioErr_t GPIO_SetPortData(portID, portData, mask)    0
#define GpioErr_t GPIO_GetPortData(portID, portData)    0
#define GpioErr_t GPIO_SetPinData(portID, pinNo, pinLevel)    0
#define GpioErr_t GPIO_GetPinData(portID,pinNo,pinLevel)    0
#define GpioErr_t GPIO_TogglePortData(portID, mask)    0
#define GpioErr_t GPIO_TogglePinData(portID, pinNo)    0
#define GpioErr_t GPIO_EnablePinPull(portID, pinNo, enable)    0
#define GpioErr_t GPIO_IsPinPullupEnabled(portID, pinNo, state)    0
#define GpioErr_t GPIO_SelectPinPull(portID, pinNo, pullSel)    0
#define GpioErr_t GPIO_GetPinPullupSelect(portID, pinNo, pullSel)    0
#define GpioErr_t GPIO_SetPinFunction(portID, pinNo, pinFunc)    0
#define GpioErr_t GPIO_GetPinFunction(portID, pinNo, pinFunc)    0
#define GpioErr_t GPIO_SetPinInterruptControl(portID, pinNo, pinIRQC)    0
#define GpioErr_t GPIO_GetPinInterruptControl(portID, pinNo, pinIRQC)    0
#define GpioErr_t GPIO_LockPinCtrlReg(portID, pinNo)    0
#define GpioErr_t GPIO_IsPinCtrlRegLocked(portID, pinNo, pLock)    0
#define GpioErr_t GPIO_EnableDriveStrength(portID, pinNo, enable)    0
#define GpioErr_t GPIO_IsDriveStrengthEnabled(portID, pinNo, enable) 0
#define GpioErr_t GPIO_EnablePassiveFilter(portID, pinNo, enable)    0
#define GpioErr_t GPIO_IsPassiveFilterEnabled(portID, pinNo, enable)    0
#define GpioErr_t GPIO_EnableSlewRate(portID, pinNo, slrValue)    0
#define GpioErr_t GPIO_IsSlewRateEnabled(portID, pinNo, slrPtr)    0
#define GpioErr_t GPIO_GetPinInterruptStatus(portID, pinNo, pStatus)    0
#define GpioErr_t GPIO_ClearPinInterruptFlag(portID, pinNo)     0
#endif /* gGPIO_Enabled_d */

#ifdef __cplusplus
}
#endif

#endif /* _GPIO_INTERFACE_H_ */
