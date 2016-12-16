/**************************************************************************
* Filename: WDOG_Interface.h
*
* Description: WDOG export interface file for ARM CORTEX-M4 processor
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

#ifndef _WDOG_INTERFACE_H_
#define _WDOG_INTERFACE_H_

#include "EmbeddedTypes.h"
#include "AppToPlatformConfig.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
 *****************************************************************************
 * Public macros
 *****************************************************************************
 *****************************************************************************/
/*
* Name: gWDOG_Enabled_d
* Description: WDOG module enable / disable compiler switch
*/
#ifndef gWDOG_Enabled_d
#define gWDOG_Enabled_d    FALSE
#endif

/*
* Name: gWDOGClkSrcLPO_c
* Description: WDT clock source is LPO (1KHz)
*/
#define gWDOGClkSrcLPO_c           0

/*
* Name: wWDOGClkSrcALT_c
* Description: WDT clock source is alternate clock
*/
#define wWDOGClkSrcALT_c           1

/*
* Name: wWDOGClkSrcMax_c
* Description: WDT clock source max value (guard define)
*/
#define wWDOGClkSrcMax_c           2

/*
* Name: wDOGByteSelMax_c
* Description: WDT test mode byte max value (guard define)
*/
#define wDOGByteSelMax_c           3

/*
* Name: wDOGPrescalerMax_c
* Description: WDT source clock prescaler max value (guard define)
*/
#define wDOGPrescalerMax_c         7

/* WDOG default configuration */
#define gWDOGAllowUpdate_c         TRUE
#define gWDOGClkSource_c           gWDOGClkSrcLPO_c
#define gWDOGPrescaler_c           0
#define gWDOGTimeoutVal_c          5000 /* [ms] */
#define gWDOGWaitModeEnable_c      TRUE
#define gWDOGwdogStopModeEnable_c  TRUE


/*****************************************************************************
 ******************************************************************************
 * Public type definitions
 ******************************************************************************
 *****************************************************************************/

/*
* Name: wdogConfig_t
* Description: WDT configuration structure
*/
typedef struct wdogConfig_tag {
    bool_t wdogTestModeEnable;
    uint8_t wdogByteSel;
    bool_t wdogTestSel;
    bool_t wdogStartTest;
    bool_t wdogWaitModeEnable;
    bool_t wdogStopModeEnable;
    bool_t wdogDbgModeEnable;
    bool_t wdogAllowUpdate;
    bool_t wdogWindowModeEnable;
    bool_t wdogIrqRstEnable;
    bool_t wdogClkSource;
    bool_t wdogEnable;
    uint16_t wdogPrescaler;    
    uint32_t wdogTimeoutVal;
    uint32_t wdogWindowVal;
} wdogConfig_t;


/*****************************************************************************
 *****************************************************************************
 * Public prototypes
 *****************************************************************************
 *****************************************************************************/

#if (gWDOG_Enabled_d == FALSE)
/* Stubs */
#define WDOG_Init()
#define WDOG_SetConfig(pConfig)    FALSE
#define WDOG_GetConfig(pConfig)    FALSE
#define WDOG_Refresh()
#define WDOG_Enable()
#define WDOG_Disable()

#else

/******************************************************************************
 * Name: WDOG_Init
 * Description: Initialize the watchdog timer (WDOG)
 * Parameters: none
 * Return: none
 ******************************************************************************/
extern void WDOG_Init
(
    void
);


/******************************************************************************
 * Name: WDOG_SetConfig
 * Description: Setup a new configuration for the WDOG
 * Parameters: [IN] pConfig - a pointer to the memory location where the  
 *                            configuration is defined and stored
 * Return: TRUE if the WDOG has been successfully updated / FALSE otherwise
 ******************************************************************************/
extern bool_t WDOG_SetConfig
(
    wdogConfig_t* pConfig
);

/******************************************************************************
 * Name: WDOG_GetConfig
 * Description: Retrieve the current configuration of the WDOG
 * Parameters: [OUT] pConfig - a pointer to a memory location where the  
 *                             current WDOG configuration will be stored
 * Return: TRUE if the WDOG configuration was successfully retrieved & stored  
 *         FALSE otherwise
 ******************************************************************************/
extern bool_t WDOG_GetConfig
(
    wdogConfig_t* pConfig
);

/******************************************************************************
 * Name: WDOG_Refresh
 * Description: Refresh the WDOG to avoid MCU reset. Shall be called 
 *              periodically at intervals < WDOG timeout period
 * Parameters: none
 * Return: none
 ******************************************************************************/
extern void WDOG_Refresh
(
    void
);

/******************************************************************************
 * Name: WDOG_Enable
 * Description: Enables the WDOG
 * Parameters: none
 * Return: none
 ******************************************************************************/
extern void WDOG_Enable
(
    void
);

/******************************************************************************
 * Name: WDOG_Disable
 * Description: Disables the WDOG
 * Parameters: none
 * Return: none
 ******************************************************************************/
extern void WDOG_Disable
(
    void
);

#endif /* gWDOG_Enabled_d */

#ifdef __cplusplus
}
#endif

#endif /* _WDOG_INTERFACE_H_ */
