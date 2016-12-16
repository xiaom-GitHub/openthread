/******************************************************************************
* Filename: WDOG.c
*
* Description: Watchdog Timer functions set for ARM CORTEX-M4 processor
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

#include "EmbeddedTypes.h"
#include "Interrupt.h"
#include "WDOG.h"

#if (gWDOG_Enabled_d == TRUE)

/******************************************************************************
 ******************************************************************************
 * Private macros
 ******************************************************************************
 ******************************************************************************/

/******************************************************************************
 ******************************************************************************
 * Private type definitions
 ******************************************************************************
 ******************************************************************************/

/******************************************************************************
 ******************************************************************************
 * Public memory definitions
 ******************************************************************************
 ******************************************************************************/

/*
 * Name: mWdogConfig
 * Description: WDOG configuration
 * Valid ranges: see wdogConfig_t description
 */
wdogConfig_t mWdogConfig;

/******************************************************************************
 ******************************************************************************
 * Private function prototypes
 ******************************************************************************
 ******************************************************************************/

/* NONE */

/******************************************************************************
 ******************************************************************************
 * Private function
 ******************************************************************************
 ******************************************************************************/

/* NONE */

/******************************************************************************
 ******************************************************************************
 * Public functions
 ******************************************************************************
 *****************************************************************************/

/******************************************************************************
 * Name: WDOG_Init
 * Description: Initialize the watchdog timer (WDOG)
 * Parameters: none
 * Return: none
 ******************************************************************************/
void WDOG_Init
(
    void
)
{
    wdogConfig_t config;

    /* Configure WDOG */
	config.wdogEnable           = TRUE;
	config.wdogAllowUpdate      = gWDOGAllowUpdate_c;
    config.wdogClkSource        = gWDOGClkSource_c;
    config.wdogPrescaler        = gWDOGPrescaler_c;
    config.wdogTimeoutVal       = gWDOGTimeoutVal_c;
    config.wdogWaitModeEnable   = gWDOGWaitModeEnable_c;
    config.wdogStopModeEnable   = gWDOGwdogStopModeEnable_c;
    config.wdogByteSel          = 0;
    config.wdogTestModeEnable   = FALSE;
    config.wdogTestSel          = FALSE;
    config.wdogStartTest        = FALSE;
    config.wdogDbgModeEnable    = FALSE;
    config.wdogIrqRstEnable     = FALSE;
    config.wdogWindowModeEnable = FALSE;
    config.wdogWindowVal        = 0;

    WDOG_SetConfig(&config);
}


/******************************************************************************
 * Name: WDOG_SetConfig
 * Description: Setup a new configuration for the WDOG
 * Parameters: [IN] pConfig - a pointer to the memory location where the
 *                            configuration is defined and stored
 * Return: TRUE if the WDOG has been successfully updated / FALSE otherwise
 ******************************************************************************/
bool_t WDOG_SetConfig
(
    wdogConfig_t* pConfig
)
{
    uint16_t StatusCtrlRegValue = 0;

    /* Several checks */

    if(NULL == pConfig)
        return FALSE;

    if(pConfig->wdogClkSource >= wWDOGClkSrcMax_c)
        return FALSE;

    if(pConfig->wdogByteSel > wDOGByteSelMax_c)
        return FALSE;

    if(pConfig->wdogPrescaler > wDOGPrescalerMax_c)
        return FALSE;

    /* Compute the control register value */
    pConfig->wdogTestModeEnable ? (StatusCtrlRegValue = WDOG_STCTRLH_DISTESTWDOG_MASK) : (StatusCtrlRegValue = 0);
    StatusCtrlRegValue |= WDOG_STCTRLH_BYTESEL(pConfig->wdogByteSel);
    pConfig->wdogTestSel ? StatusCtrlRegValue |= WDOG_STCTRLH_TESTSEL_MASK : 0;
    pConfig->wdogStartTest ? StatusCtrlRegValue |= WDOG_STCTRLH_TESTWDOG_MASK : 0;
    pConfig->wdogWaitModeEnable ? StatusCtrlRegValue |= WDOG_STCTRLH_WAITEN_MASK : 0;
    pConfig->wdogStopModeEnable ? StatusCtrlRegValue |= WDOG_STCTRLH_STOPEN_MASK : 0;
    pConfig->wdogDbgModeEnable ? StatusCtrlRegValue |= WDOG_STCTRLH_DBGEN_MASK : 0;
    pConfig->wdogAllowUpdate ? StatusCtrlRegValue |= WDOG_STCTRLH_ALLOWUPDATE_MASK : 0;
    pConfig->wdogWindowModeEnable ? StatusCtrlRegValue |= WDOG_STCTRLH_WINEN_MASK : 0;
    pConfig->wdogIrqRstEnable ? StatusCtrlRegValue |= WDOG_STCTRLH_IRQRSTEN_MASK : 0;
    pConfig->wdogClkSource ? StatusCtrlRegValue |= WDOG_STCTRLH_CLKSRC_MASK : 0;
    pConfig->wdogEnable ? StatusCtrlRegValue |= WDOG_STCTRLH_WDOGEN_MASK : 0;

    /* Disable interrupts while WDOG is unlocked */
    DisableInterrupts();
    /* Unlock WDOG */
    gWDOG_UNLOCK();
    /* Write WDOG registers */
    gWDOGWindowRegHi_c     = (uint16_t)((pConfig->wdogWindowVal >> 16) & gWDOGWindowRegMask_c);
    gWDOGWindowRegLo_c     = (uint16_t)(pConfig->wdogWindowVal & gWDOGWindowRegMask_c);
    gWDOGTimeoutValRegHi_c = (uint16_t)((pConfig->wdogTimeoutVal >> 16) & gWDOGTimeoutRegMask_c);
    gWDOGTimeoutValRegLo_c = (uint16_t)(pConfig->wdogTimeoutVal & gWDOGTimeoutRegMask_c);
    gWDOGPrescalerReg_c    = WDOG_PRESC_PRESCVAL(pConfig->wdogPrescaler);
    gWDOGStatusCtrlRegHi_c = StatusCtrlRegValue;
    /* Enable interrupts */
    EnableInterrupts();

       /* store configuration */
    mWdogConfig.wdogTestModeEnable   = pConfig->wdogTestModeEnable;
    mWdogConfig.wdogByteSel          = pConfig->wdogByteSel;
    mWdogConfig.wdogTestSel          = pConfig->wdogTestSel;
    mWdogConfig.wdogStartTest        = pConfig->wdogStartTest;
    mWdogConfig.wdogWaitModeEnable   = pConfig->wdogWaitModeEnable;
    mWdogConfig.wdogStopModeEnable   = pConfig->wdogStopModeEnable;
    mWdogConfig.wdogDbgModeEnable    = pConfig->wdogDbgModeEnable;
    mWdogConfig.wdogAllowUpdate      = pConfig->wdogAllowUpdate;
    mWdogConfig.wdogWindowModeEnable = pConfig->wdogWindowModeEnable;
    mWdogConfig.wdogIrqRstEnable     = pConfig->wdogIrqRstEnable;
    mWdogConfig.wdogClkSource        = pConfig->wdogClkSource;
    mWdogConfig.wdogEnable           = pConfig->wdogEnable;
    mWdogConfig.wdogPrescaler        = pConfig->wdogPrescaler;
    mWdogConfig.wdogTimeoutVal       = pConfig->wdogTimeoutVal;
    mWdogConfig.wdogWindowVal        = pConfig->wdogWindowVal;

    return TRUE;
}

/******************************************************************************
 * Name: WDOG_GetConfig
 * Description: Retrieve the current configuration of the WDOG
 * Parameters: [OUT] pConfig - a pointer to a memory location where the
 *                             current WDOG configuration will be stored
 * Return: TRUE if the WDOG configuration was successfully retrieved & stored
 *         FALSE otherwise
 ******************************************************************************/
bool_t WDOG_GetConfig
(
    wdogConfig_t* pConfig
)
{
    if(NULL == pConfig)
        return FALSE;

    pConfig = &mWdogConfig;
    return TRUE;
}

/******************************************************************************
 * Name: gWDOG_REFRESH
 * Description: Refresh the WDOG to avoid MCU reset. Shall be called
 *              periodically at intervals < WDOG timeout period
 * Parameters: none
 * Return: none
 ******************************************************************************/
void WDOG_Refresh
(
    void
)
{
    if(gWDOGTmrOutputRegLo_c > 1)
    {
        /* Disable interrupts while WDOG is refreshed */
        DisableInterrupts();
        /* Feed the dog */
        gWDOG_REFRESH();
        /* Enable interrupts */
        EnableInterrupts();
    }
}

/******************************************************************************
 * Name: WDOG_Enable
 * Description: Enables the WDOG
 * Parameters: none
 * Return: none
 ******************************************************************************/
void WDOG_Enable
(
    void
)
{
        /* Disable interrupts while WDOG is unlocked */
       DisableInterrupts();
       /* Unlock WDOG */
       gWDOG_UNLOCK();
       /* Enable WDOG */
       gWDOGStatusCtrlRegHi_c |= WDOG_STCTRLH_WDOGEN_MASK;
       /* Enable interrupts */
       EnableInterrupts();
}

/******************************************************************************
 * Name: WDOG_Disable
 * Description: Disables the WDOG
 * Parameters: none
 * Return: none
 ******************************************************************************/
void WDOG_Disable
(
    void
)
{
    /* Disable interrupts while WDOG is unlocked */
    DisableInterrupts();
    /* Unlock WDOG */
    gWDOG_UNLOCK();
    /* Disable WDOG */
    gWDOGStatusCtrlRegHi_c &= ~WDOG_STCTRLH_WDOGEN_MASK;
    /* Enable interrupts */
    EnableInterrupts();
}

#endif /* gWDOG_Enabled_d */
