/******************************************************************************
* Filename: Timer.c
*
* Description: TIMER implementation file for ARM CORTEX-M4 processor
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
#include "TS_Interface.h"
#include "Timer.h"
#include "NVIC.h"

#if gTMR_EnableHWLowPowerTimers_d
#include "PWR_Configuration.h"
#include "PWR_Interface.h"
#endif

#if gTMR_Enabled_d

/*****************************************************************************
 *****************************************************************************
 * Private memory definitions
 *****************************************************************************
 *****************************************************************************/

/*
 * NAME: mPreviousTimeInTicks
 * DESCRIPTION: The previous time in ticks when the counter register was read
 * VALUES: 0..65535
 */
static tmrTimerTicks16_t mPreviousTimeInTicks;

/*
 * NAME: mMaxToCountDown
 * DESCRIPTION:  Count to maximum (0xffff - 2*4ms(in ticks)), to be sure that 
 * the currentTimeInTicks will never roll over mPreviousTimeInTicks in the 
 * TMR_Task(); A task have to be executed at most in 4ms.
 * VALUES: 0..65535
 */
static uint16_t mMaxToCountDown; 

/*
 * NAME: mTicksFor4ms
 * DESCRIPTION:  Ticks for 4ms. The TMR_Task()event will not be issued faster than 4ms
 * VALUES: uint32_t range
 */
static uint32_t mTicksFor4ms;

/*
 * NAME: mClkSourceKhz
 * DESCRIPTION:  The source clock in Khz
 * VALUES: see definition
 */
static uint32_t mClkSourceKhz = (uint32_t)(gTmrSourceClkKHz_c);

/*
 * NAME: maTmrTimerTable
 * DESCRIPTION:  Main timer table. All allocated timers are stored here.
 *               A timer's ID is it's index in this table.
 * VALUES: see definition
 */
static tmrTimerTableEntry_t maTmrTimerTable[gTmrTotalTimers_c];

/*
 * NAME: maTmrTimerStatusTable
 * DESCRIPTION: timer status stable. Making the single-byte-per-timer status
 *              table a separate array saves a bit of code space.
 *              If an entry is == 0, the timer is not allocated.
 * VALUES: see definition
 */
static tmrStatus_t maTmrTimerStatusTable[gTmrTotalTimers_c];

/*
 * NAME: mNumberOfActiveTimers
 * DESCRIPTION: Number of Active timers (without low power capability)
 *              the MCU can not enter low power if mNumberOfActiveTimers!=0
 * VALUES: 0..255
 */
static uint8_t mNumberOfActiveTimers = 0;

/*
 * NAME: mNumberOfLowPowerActiveTimers
 * DESCRIPTION: Number of low power active timer.
 *              The MCU can enter in low power if more low power timers are active
 * VALUES:
 */
static uint8_t mNumberOfLowPowerActiveTimers = 0;

#define IncrementActiveTimerNumber(type)  (((type) & gTmrLowPowerTimer_c) \
                                          ?(++mNumberOfLowPowerActiveTimers) \
                                          :(++mNumberOfActiveTimers) )                                   
#define DecrementActiveTimerNumber(type)  (((type) & gTmrLowPowerTimer_c) \
                                          ?(--mNumberOfLowPowerActiveTimers) \
                                          :(--mNumberOfActiveTimers) ) 
                                          
/*
 * NAME: mTimerTaskID
 * DESCRIPTION: timer task ID. No other code should ever post an event to the timer task.
 * VALUES: see definition
 */
static tsTaskID_t mTimerTaskID;

/*
 * NAME: mTimerHardwareIsRunning
 * DESCRIPTION: Flag if the hardware timer is running or not
 * VALUES: TRUE/FALSE
 */
static bool_t mTimerHardwareIsRunning = FALSE;


/*
 * LPTMR-based software timers local variables
 */
#if gTMR_EnableHWLowPowerTimers_d

/*
 * NAME: lpTmrPreviousTimeInTicks
 * DESCRIPTION: The previous time in ticks when the counter register was read
 * VALUES: 0..65535
 */
static tmrTimerTicks16_t mLpTmrPreviousTimeInTicks;

/*
 * NAME: mLpTmrMaxToCountDown
 * DESCRIPTION:  Count to maximum (0xffff - 2*4ms(in ticks)), to be sure that 
 * the currentTimeInTicks will never roll over mLpTmrPreviousTimeInTicks in the 
 * TMR_Task(); A task have to be executed at most in 4ms.
 * VALUES: 0..65535
 */
static uint16_t mLpTmrMaxToCountDown; 

/*
 * NAME: mLpTmrTicksFor4ms
 * DESCRIPTION:  Ticks for 4ms. The TMR_Task()event will not be issued faster than 4ms
 * VALUES: uint32_t range
 */
static uint32_t mLpTmrTicksFor4ms;

/*
 * NAME: maTmrTimerTable
 * DESCRIPTION:  Main timer table. All allocated timers are stored here.
 *               A timer's ID is it's index in this table.
 * VALUES: see definition
 */
static tmrTimerTableEntry_t maLpTmrTimerTable[gLpTmrTotalTimers_c];

/*
 * NAME: maLpTmrTimerStatusTable
 * DESCRIPTION: timer status stable. Making the single-byte-per-timer status
 *              table a separate array saves a bit of code space.
 *              If an entry is == 0, the timer is not allocated.
 * VALUES: see definition
 */
static tmrStatus_t maLpTmrTimerStatusTable[gLpTmrTotalTimers_c];

/*
 * NAME: mLpTmrNumberOfActiveTimers
 * DESCRIPTION: Number of active LPTMR-based software timers              
 * VALUES: 0..255
 */
static uint8_t mLpTmrNumberOfActiveTimers = 0;


#define IncrementActiveLpTmrTimerNumber()    (++mLpTmrNumberOfActiveTimers)
                                                                             
#define DecrementActiveLpTmrTimerNumber()    (--mLpTmrNumberOfActiveTimers)

/*
 * NAME: mLpTmrHardwareIsRunning
 * DESCRIPTION: Flag if the LPTMR hardware timer is running or not
 * VALUES: TRUE/FALSE
 */
static bool_t mLpTmrHardwareIsRunning = FALSE;

#endif /* gTMR_EnableHWLowPowerTimers_d */

/*****************************************************************************
 *****************************************************************************
 * Private prototypes
 *****************************************************************************
 *****************************************************************************/

/*---------------------------------------------------------------------------
 * NAME: TMR_GetTimerStatus
 * DESCRIPTION: RETURNs the timer status
 * PARAMETERS:  IN: timerID - the timer ID
 *              IN: hwTmrType - FTM or LPTMR
 * RETURN: see definition of tmrStatus_t
 * NOTES: none
 *---------------------------------------------------------------------------*/
static tmrStatus_t TMR_GetTimerStatus 
( 
    tmrTimerID_t timerID
#if gTMR_EnableHWLowPowerTimers_d
    ,tmrHwSourceType_t hwTmrType
#endif
);

/*---------------------------------------------------------------------------
 * NAME: TMR_SetTimerStatus
 * DESCRIPTION: Set the timer status
 * PARAMETERS:  IN: timerID - the timer ID
 * 			    IN: status - the status of the timer
 * 			    IN: hwTmrType - FTM or LPTMR
 * RETURN: None
 * NOTES: none
 *---------------------------------------------------------------------------*/
static void TMR_SetTimerStatus
( 
    tmrTimerID_t timerID,
    tmrStatus_t status
#if gTMR_EnableHWLowPowerTimers_d
    ,tmrHwSourceType_t hwTmrType
#endif
);

/*---------------------------------------------------------------------------
 * NAME: TMR_GetTimerType
 * DESCRIPTION: RETURNs the timer type
 * PARAMETERS:  IN: timerID - the timer ID
 *              IN: hwTmrType - FTM or LPTMR
 * RETURN: see definition of tmrTimerType_t
 * NOTES: none
 *---------------------------------------------------------------------------*/
static tmrTimerType_t TMR_GetTimerType 
( 
    tmrTimerID_t timerID
#if gTMR_EnableHWLowPowerTimers_d
    ,tmrHwSourceType_t hwTmrType
#endif
);

/*---------------------------------------------------------------------------
 * NAME: TMR_SetTimerType
 * DESCRIPTION: Set the timer type
 * PARAMETERS:  IN: timerID - the timer ID
 * 			    IN: type - timer type
 * 			    IN: hwTmrType - FTM or LPTMR
 * RETURN: none
 * NOTES: none
 *---------------------------------------------------------------------------*/
static void TMR_SetTimerType
(
    tmrTimerID_t timerID,
    tmrTimerType_t type
#if gTMR_EnableHWLowPowerTimers_d
    ,tmrHwSourceType_t hwTmrType
#endif
);

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_StartLpTimer (BeeStack or application)
 * DESCRIPTION: Start a specified LPTMR-based timer
 * PARAMETERS: IN: timerId - the ID of the timer
 *             IN: timerType - the type of the timer
 *             IN: timeInMilliseconds - time expressed in millisecond units
 *             IN: pfTmrCallBack - callback function
 * RETURN: -
 * NOTES: When the timer expires, the callback function is called in
 *        non-interrupt context. If the timer is already running when
 *        this function is called, it will be stopped and restarted.
 *---------------------------------------------------------------------------*/
static void TMR_StartLpTimer
(
    tmrTimerID_t timerID,                       
    tmrTimerType_t timerType,                   
    tmrTimeInMilliseconds_t timeInMilliseconds, 
    void (*pfTimerCallBack)(tmrTimerID_t)       
);
#endif

/*****************************************************************************
******************************************************************************
* Private functions
******************************************************************************
*****************************************************************************/

/*---------------------------------------------------------------------------
* NAME: TMR_GetTimerStatus
* DESCRIPTION: Returns the timer status
* PARAMETERS:  IN: timerID - the timer ID
*              IN: hwTmrType - FTM or LPTMR
* RETURN: see definition of tmrStatus_t
* NOTES: none
*---------------------------------------------------------------------------*/
static tmrStatus_t TMR_GetTimerStatus
(
    tmrTimerID_t timerID
#if gTMR_EnableHWLowPowerTimers_d
    ,tmrHwSourceType_t hwTmrType
#endif    
)
{
#if gTMR_EnableHWLowPowerTimers_d
	
	if(hwTmrType == tmrFTMbased_c)
	{
		return maTmrTimerStatusTable[timerID] & mTimerStatusMask_c;
	}
	
	return maLpTmrTimerStatusTable[timerID] & mTimerStatusMask_c;
	
#else
    return maTmrTimerStatusTable[timerID] & mTimerStatusMask_c;
#endif
}

/*---------------------------------------------------------------------------
* NAME: TMR_SetTimerStatus
* DESCRIPTION: Set the timer status
* PARAMETERS:  IN: timerID - the timer ID
* 			   IN: status - the status of the timer	
* 			   IN: hwTmrType - FTM or LPTMR
* RETURN: None
* NOTES: none
*---------------------------------------------------------------------------*/
static void TMR_SetTimerStatus
(
    tmrTimerID_t timerID, 
    tmrStatus_t status
#if gTMR_EnableHWLowPowerTimers_d
    ,tmrHwSourceType_t hwTmrType
#endif    
)
{
#if gTMR_EnableHWLowPowerTimers_d
	
	if(hwTmrType == tmrFTMbased_c)
	{
		maTmrTimerStatusTable[timerID] = (maTmrTimerStatusTable[timerID] & ~mTimerStatusMask_c) | status;
	}
	else
	{
		maLpTmrTimerStatusTable[timerID] = (maLpTmrTimerStatusTable[timerID] & ~mTimerStatusMask_c) | status;
	}
		
#else
	maTmrTimerStatusTable[timerID] = (maTmrTimerStatusTable[timerID] & ~mTimerStatusMask_c) | status;
#endif
}

/*---------------------------------------------------------------------------
* NAME: TMR_GetTimerType
* DESCRIPTION: Returns the timer type
* PARAMETERS:  IN: timerID - the timer ID
*              IN: hwTmrType - FTM or LPTMR
* RETURN: see definition of tmrTimerType_t
* NOTES: none
*---------------------------------------------------------------------------*/
static tmrTimerType_t TMR_GetTimerType
(
    tmrTimerID_t timerID
#if gTMR_EnableHWLowPowerTimers_d
    ,tmrHwSourceType_t hwTmrType
#endif    
)
{
#if gTMR_EnableHWLowPowerTimers_d
	
	if(hwTmrType == tmrFTMbased_c)
	{
		return maTmrTimerStatusTable[timerID] & mTimerType_c;
	}
	
	return maLpTmrTimerStatusTable[timerID] & mTimerType_c;
    
#else
    return maTmrTimerStatusTable[timerID] & mTimerType_c;
#endif
}

/*---------------------------------------------------------------------------
* NAME: TMR_SetTimerType
* DESCRIPTION: Set the timer type
* PARAMETERS:  IN: timerID - the timer ID
* 			   IN: type - timer type
* 			   IN: hwTmrType - FTM or LPTMR	
* RETURN: none
* NOTES: none
*---------------------------------------------------------------------------*/
static void TMR_SetTimerType
(
    tmrTimerID_t timerID, 
    tmrTimerType_t type
#if gTMR_EnableHWLowPowerTimers_d
    ,tmrHwSourceType_t hwTmrType
#endif    
)
{
#if gTMR_EnableHWLowPowerTimers_d	
	if(hwTmrType == tmrFTMbased_c)
	{
		maTmrTimerStatusTable[timerID] = (maTmrTimerStatusTable[timerID] & ~mTimerType_c) | type;
	}
	else
	{
		/*
		 * gTmrLowPowerTimer_c is used only by the normal (FTM-based) timers and
		 * this macro indicates a timer that will be synchronised after the MCU
		 * will wake-up from low power mode. The macro has no mean for LPTMR-based
		 * timers.
		 */  
		type &= ~gTmrLowPowerTimer_c;
		maLpTmrTimerStatusTable[timerID] = (maLpTmrTimerStatusTable[timerID] & ~mTimerType_c) | type;		
	}		    
#else
    maTmrTimerStatusTable[timerID] = (maTmrTimerStatusTable[timerID] & ~mTimerType_c) | type;
#endif
} 

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_StartLpTimer (BeeStack or application)
 * DESCRIPTION: Start a specified LPTMR-based timer
 * PARAMETERS: IN: timerId - the ID of the timer
 *             IN: timerType - the type of the timer
 *             IN: timeInMilliseconds - time expressed in millisecond units
 *             IN: pfTmrCallBack - callback function
 * RETURN: -
 * NOTES: When the timer expires, the callback function is called in
 *        non-interrupt context. If the timer is already running when
 *        this function is called, it will be stopped and restarted.
 *---------------------------------------------------------------------------*/
static void TMR_StartLpTimer
(
    tmrTimerID_t timerID,                       
    tmrTimerType_t timerType,                   
    tmrTimeInMilliseconds_t timeInMilliseconds, 
    void (*pfTimerCallBack)(tmrTimerID_t)       
)
{
    tmrTimerTicks32_t intervalInTicks;
    
    /* check if timer is not allocated or if it has an invalid ID (fix@ENGR223389) */
    if (!TMR_IsLpTimerAllocated(timerID) || (gTmrInvalidTimerID_c == timerID) || 
    		(timerType == gTmrLowPowerTimer_c)) 
      return;

    /* Stopping an already stopped timer is harmless. */
    TMR_StopLpTimer(timerID);
    
    intervalInTicks = timeInMilliseconds;
    if (!intervalInTicks) 
    {
        intervalInTicks = 1;
    }

    TMR_SetTimerType(timerID, timerType, tmrLPTMRbased_c);
    maLpTmrTimerTable[timerID].intervalInTicks = intervalInTicks;
    maLpTmrTimerTable[timerID].remainingTicks = intervalInTicks;
    LPTMRReadCounterRegister(maLpTmrTimerTable[timerID].timestamp);
    maLpTmrTimerTable[timerID].pfCallBack = pfTimerCallBack;

    /* Enable timer, the timer task will do the rest of the work. */
    TMR_EnableLpTimer(timerID);
}
#endif /* gTMR_EnableHWLowPowerTimers_d */


/*****************************************************************************
******************************************************************************
* Public functions
******************************************************************************
*****************************************************************************/

/*---------------------------------------------------------------------------
 * NAME: TmrTicksFromMilliseconds
 * DESCRIPTION: Convert milliseconds to ticks
 * PARAMETERS:  IN: milliseconds
 * RETURN: tmrTimerTicks32_t - ticks number
 * NOTES: none
 *---------------------------------------------------------------------------*/
tmrTimerTicks32_t TmrTicksFromMilliseconds
(
    tmrTimeInMilliseconds_t milliseconds   
)
{
    return (milliseconds * (mClkSourceKhz/(0x01<<gFTMxSC_PrescaleCount_c)));
}

/*---------------------------------------------------------------------------
 * NAME: TMR_Init
 * DESCRIPTION: initialize the timer module
 * PARAMETERS: -
 * RETURN: -
 *---------------------------------------------------------------------------*/
void TMR_Init 
(
    void
)
{
    /* Configure a FTM channel: enable interrupts; set output compare mode. */

    /* Enable timer IRQ */
    NVIC_EnableIRQ(gTMR_FTMIrqNo);
    /* Set timer IRQ priority */
    NVIC_SetPriority(gTMR_FTMIrqNo, gTMR_FTMInterruptPriority);

    /* Setup the system clock gating */
    gTMR_FTM_SIM_SCGC_REG_c |= (1<<gTMR_FTM_SIM_SCGC_BIT_c);

    /* Dummy read of the FTMx_SC register to clear the interrupt flag */                       
    (void)(gFTMxSC_c == 0U);
    /* Stop the counter */
    gFTMxSC_c = (uint32_t)0x00UL;
    /* Dummy read of the FTMx_CnSC register to clear the interrupt flag */
    (void)(gFTMxCnSC_c == 0U);             

    /* Disable write protection */
    /* FTMx_MODE: WPDIS=1 */
    gFTMxMODE_c |= (uint32_t)0x04UL;       
    /* FTMx_MODE: FTMEN=0 */
    gFTMxMODE_c &= (uint32_t)~0x01UL;    

    /* Clear modulo register */
    gFTMxMOD_c = (uint32_t)0x00UL;	

    gFTMxCnSC_c = (uint32_t)0x50UL; 

    mTimerTaskID = TS_CreateTask(gTsTimerTaskPriority_c, TMR_Task);    
    
    /* Count to maximum (0xffff - 2*4ms(in ticks)), to be sure that the currentTimeInTicks 
    will never roll over mPreviousTimeInTicks in the TMR_Task() */
    mMaxToCountDown = 0xFFFF - TmrTicksFromMilliseconds(8); 
    /* The TMR_Task()event will not be issued faster than 4ms*/
    mTicksFor4ms = TmrTicksFromMilliseconds(4);
        
/* Low-Power Timer default configuration */
#if gTMR_EnableHWLowPowerTimers_d
    /* turn on the LPTMR clock */
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;
    /* disable LPTMR */
    LPTMR0_CSR &= ~(LPTMR_CSR_TEN_MASK);
#if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz)
    /* 1ms tick period */
    LPTMR0_PSR = (cLPTMR_PRS_00001ms | cPWR_LPTMRClockSource);
#elif (cPWR_LPTMRClockSource == cLPTMR_Source_Ext_ERCLK32K)    
  #if (MCU_MK60N512VMD100 == 1)	
    SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL_MASK;
  #elif ((MCU_MK60D10 == 1)  || (MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
    SIM_SOPT1 &= ~SIM_SOPT1_OSC32KSEL_MASK;
    SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL(2);
  #endif // (MCU_MK60N512VMD100 == 1)
    /* init 32.768 KHz RTC OSC */    
    SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;
    RTC_CR |= RTC_CR_SWR_MASK;
    RTC_CR &= ~(RTC_CR_SWR_MASK);
    RTC_SR &= ~(RTC_SR_TCE_MASK);
    RTC_CR |= RTC_CR_SUP_MASK;
    RTC_CR |= RTC_CR_OSCE_MASK;
    /* ~1ms (0.9765625ms) tick period */
    LPTMR0_PSR = (cLPTMR_PRS_125_div_by_128ms | cPWR_LPTMRClockSource);
#else
#error "*** ERROR: Illegal value in cPWR_LPTMRClockSource"
#endif
            
    /* Enable LPTMR IRQ */
    NVIC_EnableIRQ(gTMR_LPTMR_IRQ_c);
    /* Set LPTMR IRQ priority */
    NVIC_SetPriority(gTMR_LPTMR_IRQ_c, gTMR_LPTMRInterruptPriority);
           
    /* Count to maximum (0xffff - 2*4ms(in ticks)), to be sure that the currentTimeInTicks 
    will never roll over mPreviousTimeInTicks in the TMR_Task() */
    mLpTmrMaxToCountDown = 0xFFF7; /* 1 tick = 1 ms */
    /* The TMR_Task()event will not be issued faster than 4ms*/
    mLpTmrTicksFor4ms = 4; /* 1 tick = 1 ms */
#endif /* gTMR_EnableHWLowPowerTimers_d */
}

/*---------------------------------------------------------------------------
 * NAME: TMR_NotifyClkChanged
 * DESCRIPTION: This function is called when the clock is changed
 * PARAMETERS: IN: clkKhz (uint32_t) - new clock
 * RETURN: -
 *---------------------------------------------------------------------------*/
void TMR_NotifyClkChanged
(
    uint32_t clkKhz
)
{
    mClkSourceKhz = clkKhz;
    /* Clock was changed, so calculate again  mMaxToCountDown.
    Count to maximum (0xffff - 2*4ms(in ticks)), to be sure that the currentTimeInTicks 
    will never roll over mPreviousTimeInTicks in the TMR_Task() */
    mMaxToCountDown = 0xFFFF - TmrTicksFromMilliseconds(8); 
    /* The TMR_Task()event will not be issued faster than 4ms*/
    mTicksFor4ms = TmrTicksFromMilliseconds(4);
}

/*---------------------------------------------------------------------------
 * NAME: TMR_AllocateTimer
 * DESCRIPTION: allocate a timer
 * PARAMETERS: -
 * RETURN: timer ID
 *---------------------------------------------------------------------------*/
tmrTimerID_t TMR_AllocateTimer
(
    void
)
{
    uint32_t i;
  
    for (i = 0; i < NumberOfElements(maTmrTimerTable); ++i) 
    {
        if (!TMR_IsTimerAllocated(i)) 
        {
#if gTMR_EnableHWLowPowerTimers_d        	
            TMR_SetTimerStatus(i, mTmrStatusInactive_c, tmrFTMbased_c);
#else
            TMR_SetTimerStatus(i, mTmrStatusInactive_c);
#endif
            return i;
        }
   }
  
   return gTmrInvalidTimerID_c;
}   

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_AllocateLpTimer
 * DESCRIPTION: allocate a LPTMR-based timer
 * PARAMETERS: -
 * RETURN: timer ID
 *---------------------------------------------------------------------------*/
tmrTimerID_t TMR_AllocateLpTimer
(
    void 
)
{
    uint32_t i;   
  
    for (i = 0; i < NumberOfElements(maLpTmrTimerTable); ++i) 
    {
        if (!TMR_IsLpTimerAllocated(i)) 
        {
            TMR_SetTimerStatus(i, mTmrStatusInactive_c, tmrLPTMRbased_c);
            return i;
        }
   }
  
   return gTmrInvalidTimerID_c;
}
#endif /* gTMR_EnableHWLowPowerTimers_d */

/*---------------------------------------------------------------------------
 * NAME: TMR_AreAllTimersOff
 * DESCRIPTION: Check if all timers except the LP timers are OFF.
 * PARAMETERS: -
 * RETURN: TRUE if there are no active non-low power timers, FALSE otherwise
 *---------------------------------------------------------------------------*/
bool_t TMR_AreAllTimersOff
(
    void
)
{
    return !mNumberOfActiveTimers;
} 

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_AreAllLpTimersOff
 * DESCRIPTION: Check if all LPTMR-based timers are OFF.
 * PARAMETERS: -
 * RETURN: TRUE if there are no active non-low power timers, FALSE otherwise
 *---------------------------------------------------------------------------*/
bool_t TMR_AreAllLpTimersOff
(
    void
)
{
    return !mLpTmrNumberOfActiveTimers;
}
#endif /* gTMR_EnableHWLowPowerTimers_d */

/*---------------------------------------------------------------------------
 * NAME: TMR_FreeTimer
 * DESCRIPTION: Free a timer
 * PARAMETERS:  IN: timerID - the ID of the timer
 * RETURN: -
 * NOTES: Safe to call even if the timer is running.
 *        Harmless if the timer is already free.
 *---------------------------------------------------------------------------*/
void TMR_FreeTimer
(
    tmrTimerID_t timerID
)
{
    TMR_StopTimer(timerID);
    TMR_MarkTimerFree(timerID);
}

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_FreeLpTimer
 * DESCRIPTION: Free a LPTMR-based timer
 * PARAMETERS:  IN: timerID - the ID of the timer
 * RETURN: -
 * NOTES: Safe to call even if the timer is running.
 *        Harmless if the timer is already free.
 *---------------------------------------------------------------------------*/
void TMR_FreeLpTimer
(
    tmrTimerID_t timerID
)
{
	TMR_StopLpTimer(timerID);
	TMR_MarkLpTimerFree(timerID);
}
#endif

/*---------------------------------------------------------------------------
 * NAME: TMR_InterruptHandler
 * DESCRIPTION: Timer Module Interrupt Service Routine
 * PARAMETERS: -
 * RETURN: -
 * NOTES: This function have to be added to Interrupt Vector Table
 *---------------------------------------------------------------------------*/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
void TMR_InterruptHandler
(
    void
) 
{
    /* Clearing the overflow flag requires reading it and then writing it. */

    if(gFTMxSC_c & gFTMxSC_TOF_c) 
    {
        gFTMxSC_c  &= ~ gFTMxSC_TOF_c;
    }

    if ( gFTMxCnSC_c & gFTMxCnSC_CHF_c ) 
    {
        gFTMxCnSC_c &= ~gFTMxCnSC_CHF_c;
        TS_SendEvent(mTimerTaskID, gTMR_Event_c);    
    }
}

/*---------------------------------------------------------------------------
 * NAME: PWRLib_LPTMR_Isr
 * DESCRIPTION: LPTMR Interrupt Service Routine. Exists only when PWR module
 *              is used and deep sleep mode is 14. The name is the same as
 *              in PWRLib in order to keep the vector table unmodified.
 * PARAMETERS: -
 * RETURN: -
 *---------------------------------------------------------------------------*/
#if gTMR_EnableHWLowPowerTimers_d
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
void PWRLib_LPTMR_Isr
(
    void
)
{
	LPTMR0_CSR |= LPTMR_CSR_TCF_MASK; /* w1c */
	TS_SendEvent(mTimerTaskID, gLPTMR_Event_c); 
}
#endif /* #if gTMR_EnableHWLowPowerTimers_d */

/*---------------------------------------------------------------------------
 * NAME: TMR_IsTimerActive
 * DESCRIPTION: Check if a specified timer is active
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: TRUE if the timer (specified by the timerID) is active,
 *         FALSE otherwise
 *---------------------------------------------------------------------------*/
bool_t TMR_IsTimerActive
(
    tmrTimerID_t timerID
)
{
#if gTMR_EnableHWLowPowerTimers_d  
    return (TMR_GetTimerStatus(timerID, tmrLPTMRbased_c) == mTmrStatusActive_c);
#else
    return (TMR_GetTimerStatus(timerID) == mTmrStatusActive_c);
#endif
}

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_IsLpTimerActive
 * DESCRIPTION: Check if a specified LPTMR-based timer is active
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: TRUE if the timer (specified by the timerID) is active,
 *         FALSE otherwise
 *---------------------------------------------------------------------------*/
bool_t TMR_IsLpTimerActive
(
    tmrTimerID_t timerID
)
{
    return (TMR_GetTimerStatus(timerID, tmrLPTMRbased_c) == mTmrStatusActive_c);
}
#endif /* gTMR_EnableHWLowPowerTimers_d */

/*---------------------------------------------------------------------------
 * NAME: TMR_IsTimerReady
 * DESCRIPTION: Check if a specified timer is ready
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: TRUE if the timer (specified by the timerID) is ready,
 *         FALSE otherwise
 *---------------------------------------------------------------------------*/
bool_t TMR_IsTimerReady
(
    tmrTimerID_t timerID
)
{
#if gTMR_EnableHWLowPowerTimers_d	
    return (TMR_GetTimerStatus(timerID, tmrFTMbased_c) == mTmrStatusReady_c);
#else
    return (TMR_GetTimerStatus(timerID) == mTmrStatusReady_c);
#endif
} 

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_IsLpTimerReady
 * DESCRIPTION: Check if a specified LPTMR-based timer is ready
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: TRUE if the timer (specified by the timerID) is ready,
 *         FALSE otherwise
 *---------------------------------------------------------------------------*/
bool_t TMR_IsLpTimerReady
(
    tmrTimerID_t timerID
)
{
    return (TMR_GetTimerStatus(timerID, tmrLPTMRbased_c) == mTmrStatusReady_c);
}
#endif /* gTMR_EnableHWLowPowerTimers_d */


/*---------------------------------------------------------------------------
 * NAME: TMR_GetRemainingTime
 * DESCRIPTION: Returns the remaining time until timeout, for the specified
 *              timer
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: remaining time in milliseconds until specified timer timeout
 *---------------------------------------------------------------------------*/
uint32_t TMR_GetRemainingTime
(
    tmrTimerID_t tmrID
)
{
  tmrTimerTicks32_t remainingTime, currentTime, elapsedTicks;
  unsigned int saveInt;
  
  if(gTmrInvalidTimerID_c == tmrID)
    return 0;
  
  if(!maTmrTimerTable[tmrID].remainingTicks)
    return 0;
  
  saveInt = IntDisableAll(); 
  
  FTMReadCNTRegister(currentTime);
  
  if(currentTime < maTmrTimerTable[tmrID].timestamp)
  {
    currentTime += (uint32_t)gFTM_CNT_OVERFLOW_VALUE_c;
  }
  
  elapsedTicks = currentTime - maTmrTimerTable[tmrID].timestamp;
  
  if(elapsedTicks > maTmrTimerTable[tmrID].remainingTicks)
  {
    IntRestoreAll(saveInt);
    return 1;
  }
  
  if((maTmrTimerTable[tmrID].remainingTicks - elapsedTicks) > mClkSourceKhz)
    remainingTime = ((maTmrTimerTable[tmrID].remainingTicks - elapsedTicks)/mClkSourceKhz) * (1 << gFTMxSC_PrescaleCount_c);
  else  
    remainingTime = ((maTmrTimerTable[tmrID].remainingTicks - elapsedTicks) * (1 << gFTMxSC_PrescaleCount_c))/mClkSourceKhz;
  IntRestoreAll(saveInt);

  return remainingTime;
}

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_GetLpRemainingTime
 * DESCRIPTION: Returns the remaining time until timeout, for the specified
 *              timer
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: remaining time in milliseconds until specified timer timeout
 *---------------------------------------------------------------------------*/
uint32_t TMR_GetLpRemainingTime
(
    tmrTimerID_t tmrID
)
{
  tmrTimerTicks32_t remainingTime, currentTime, elapsedTicks;
  unsigned int saveInt;
  
  if(gTmrInvalidTimerID_c == tmrID)
    return 0;
  
  if(!maLpTmrTimerTable[tmrID].remainingTicks)
    return 0;
  
  saveInt = IntDisableAll();
  
  LPTMRReadCounterRegister(currentTime);
  
  if(currentTime < maLpTmrTimerTable[tmrID].timestamp)
  {
    currentTime += (uint32_t)gLPTMR_OVERFLOW_VALUE_c;
  }
  
  elapsedTicks = currentTime - maLpTmrTimerTable[tmrID].timestamp;
  
  if(elapsedTicks > maLpTmrTimerTable[tmrID].remainingTicks)
  {
    IntRestoreAll(saveInt);
    return 1;
  }
  
  /* 1 tick period = 1 ms */
  remainingTime = (maLpTmrTimerTable[tmrID].remainingTicks - elapsedTicks);
  
  IntRestoreAll(saveInt);
  
  return remainingTime;
}
#endif /* gTMR_EnableHWLowPowerTimers_d */

/*---------------------------------------------------------------------------
 * NAME: TMR_StartTimer (BeeStack or application)
 * DESCRIPTION: Start a specified timer
 * PARAMETERS: IN: timerId - the ID of the timer
 *             IN: timerType - the type of the timer
 *             IN: timeInMilliseconds - time expressed in millisecond units
 *             IN: pfTmrCallBack - callback function
 * RETURN: -
 * NOTES: When the timer expires, the callback function is called in
 *        non-interrupt context. If the timer is already running when
 *        this function is called, it will be stopped and restarted.
 *---------------------------------------------------------------------------*/
void TMR_StartTimer
(
    tmrTimerID_t timerID,
    tmrTimerType_t timerType,
    tmrTimeInMilliseconds_t timeInMilliseconds,
    void (*pfTimerCallBack)(tmrTimerID_t)
)
{
    tmrTimerTicks32_t intervalInTicks;
    
    /* check if timer is not allocated or if it has an invalid ID (fix@ENGR223389) */
    if (!TMR_IsTimerAllocated(timerID) || (gTmrInvalidTimerID_c == timerID)) 
      return;

    /* Stopping an already stopped timer is harmless. */
    TMR_StopTimer(timerID);

    intervalInTicks = TmrTicksFromMilliseconds(timeInMilliseconds);
    if (!intervalInTicks) 
    {
        intervalInTicks = 1;
    }
    
#if gTMR_EnableHWLowPowerTimers_d
    TMR_SetTimerType(timerID, timerType, tmrFTMbased_c);
#else
    TMR_SetTimerType(timerID, timerType);
#endif
    maTmrTimerTable[timerID].intervalInTicks = intervalInTicks;
    maTmrTimerTable[timerID].remainingTicks = intervalInTicks;
    FTMReadCNTRegister(maTmrTimerTable[timerID].timestamp);
    maTmrTimerTable[timerID].pfCallBack = pfTimerCallBack;

    /* Enable timer, the timer task will do the rest of the work. */
    TMR_EnableTimer(timerID);
}

/*---------------------------------------------------------------------------
 * NAME: TMR_StartLowPowerTimer
 * DESCRIPTION: Start a low power timer. When the timer goes off, call the 
 *              callback function in non-interrupt context. 
 *              If the timer is running when this function is called, it will 
 *              be stopped and restarted. 
 *              Start the timer with the following timer types:
 *                          - gTmrLowPowerMinuteTimer_c
 *                          - gTmrLowPowerSecondTimer_c
 *                          - gTmrLowPowerSingleShotMillisTimer_c
 *                          - gTmrLowPowerIntervalMillisTimer_c
 *              The MCU can enter in low power if there are only active low 
 *              power timers.
 * PARAMETERS: IN: timerId - the ID of the timer
 *             IN: timerType - the type of the timer
 *             IN: timeIn - time in ticks
 *             IN: pfTmrCallBack - callback function
 * RETURN: type/DESCRIPTION
 *---------------------------------------------------------------------------*/
void TMR_StartLowPowerTimer
(
    tmrTimerID_t timerId,
    tmrTimerType_t timerType,
    uint32_t timeIn,
    void (*pfTmrCallBack)(tmrTimerID_t)
) 
{
#if(gTMR_EnableLowPowerTimers_d) 
	/* FTM-based timers that are sync. after system wake-up */
    TMR_StartTimer(timerId, timerType | gTmrLowPowerTimer_c, timeIn, 
                   pfTmrCallBack);
#elif gTMR_EnableHWLowPowerTimers_d
    /* LPTMR-based timers */
     TMR_StartLpTimer(timerId, timerType, timeIn, pfTmrCallBack);
#else
    (void)timerId;
    (void)timerType;
    (void)timeIn;
	(void)pfTmrCallBack;
#endif
}

/*---------------------------------------------------------------------------
 * NAME: TMR_StartMinuteTimer
 * DESCRIPTION: Starts a minutes timer
 * PARAMETERS:  IN: timerId - the ID of the timer
 *              IN: timeInMinutes - time expressed in minutes
 *              IN: pfTmrCallBack - callback function
 * RETURN: None
 * NOTES: Customized form of TMR_StartTimer(). This is a single shot timer.
 *        There are no interval minute timers.
 *---------------------------------------------------------------------------*/
 #if gTMR_EnableMinutesSecondsTimers_d
void TMR_StartMinuteTimer
(
    tmrTimerID_t timerId, 
    tmrTimeInMinutes_t timeInMinutes, 
    void (*pfTmrCallBack)(tmrTimerID_t)
)
{
    TMR_StartTimer(timerId, gTmrMinuteTimer_c, TmrMinutes(timeInMinutes), pfTmrCallBack);   
}

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_StartLpMinuteTimer
 * DESCRIPTION: Starts a minutes LPTMR-timer
 * PARAMETERS:  IN: timerId - the ID of the timer
 *              IN: timeInMinutes - time expressed in minutes
 *              IN: pfTmrCallBack - callback function
 * RETURN: None
 * NOTES: Customized form of TMR_StartTimer(). This is a single shot timer.
 *        There are no interval minute timers.
 *---------------------------------------------------------------------------*/
void TMR_StartLpMinuteTimer
(
    tmrTimerID_t timerId, 
    tmrTimeInMinutes_t timeInMinutes, 
    void (*pfTmrCallBack)(tmrTimerID_t)
)
{
    TMR_StartLowPowerTimer(timerId, gTmrMinuteTimer_c, TmrMinutes(timeInMinutes), pfTmrCallBack);    
}

#endif /* gTMR_EnableHWLowPowerTimers_d */
#endif /* gTMR_EnableMinutesSecondsTimers_d */
  
/*---------------------------------------------------------------------------
 * NAME: TMR_StartSecondTimer
 * DESCRIPTION: Starts a seconds timer
 * PARAMETERS:  IN: timerId - the ID of the timer
 *              IN: timeInSeconds - time expressed in seconds
 *              IN: pfTmrCallBack - callback function
 * RETURN: None
 * NOTES: Customized form of TMR_StartTimer(). This is a single shot timer.
 *        There are no interval seconds timers.
 *---------------------------------------------------------------------------*/
#if gTMR_EnableMinutesSecondsTimers_d
void TMR_StartSecondTimer
(
    tmrTimerID_t timerId, 
    tmrTimeInSeconds_t timeInSeconds, 
    void (*pfTmrCallBack)(tmrTimerID_t)
) 
{
    TMR_StartTimer(timerId, gTmrSecondTimer_c, TmrSeconds(timeInSeconds), pfTmrCallBack);
}
#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_StartLpSecondTimer
 * DESCRIPTION: Starts a seconds LPTMR-based timer
 * PARAMETERS:  IN: timerId - the ID of the timer
 *              IN: timeInSeconds - time expressed in seconds
 *              IN: pfTmrCallBack - callback function
 * RETURN: None
 * NOTES: Customized form of TMR_StartTimer(). This is a single shot timer.
 *        There are no interval seconds timers.
 *---------------------------------------------------------------------------*/
void TMR_StartLpSecondTimer
(
    tmrTimerID_t timerId, 
    tmrTimeInSeconds_t timeInSeconds, 
    void (*pfTmrCallBack)(tmrTimerID_t)
) 
{
    TMR_StartLowPowerTimer(timerId, gTmrSecondTimer_c, TmrSeconds(timeInSeconds), pfTmrCallBack);
}
#endif /* gTMR_EnableHWLowPowerTimers_d */
#endif /* gTMR_EnableMinutesSecondsTimers_d */
 
/*---------------------------------------------------------------------------
 * NAME: TMR_StartIntervalTimer
 * DESCRIPTION: Starts an interval count timer
 * PARAMETERS:  IN: timerId - the ID of the timer
 *              IN: timeInMilliseconds - time expressed in milliseconds
 *              IN: pfTmrCallBack - callback function
 * RETURN: None
 * NOTES: Customized form of TMR_StartTimer()
 *---------------------------------------------------------------------------*/
void TMR_StartIntervalTimer
(
    tmrTimerID_t timerID,
    tmrTimeInMilliseconds_t timeInMilliseconds,
    void (*pfTimerCallBack)(tmrTimerID_t)
)
{
    TMR_StartTimer(timerID, gTmrIntervalTimer_c, timeInMilliseconds, pfTimerCallBack);
}

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_StartLpIntervalTimer
 * DESCRIPTION: Starts an interval count LPTMR-based timer
 * PARAMETERS:  IN: timerId - the ID of the timer
 *              IN: timeInMilliseconds - time expressed in milliseconds
 *              IN: pfTmrCallBack - callback function
 * RETURN: None
 * NOTES: Customized form of TMR_StartTimer()
 *---------------------------------------------------------------------------*/
void TMR_StartLpIntervalTimer
(
    tmrTimerID_t timerID,
    tmrTimeInMilliseconds_t timeInMilliseconds,
    void (*pfTimerCallBack)(tmrTimerID_t)
)
{
    TMR_StartLowPowerTimer(timerID, gTmrIntervalTimer_c, timeInMilliseconds, pfTimerCallBack);
}
#endif /* gTMR_EnableHWLowPowerTimers_d */

/*---------------------------------------------------------------------------
 * NAME: TMR_StartSingleShotTimer
 * DESCRIPTION: Starts an single-shot timer
 * PARAMETERS:  IN: timerId - the ID of the timer
 *              IN: timeInMilliseconds - time expressed in milliseconds
 *              IN: pfTmrCallBack - callback function
 * RETURN: None
 * NOTES: Customized form of TMR_StartTimer()
 *---------------------------------------------------------------------------*/
void TMR_StartSingleShotTimer
(
    tmrTimerID_t timerID,
    tmrTimeInMilliseconds_t timeInMilliseconds,
    void (*pfTimerCallBack)(tmrTimerID_t)
)
{
    TMR_StartTimer(timerID, gTmrSingleShotTimer_c, timeInMilliseconds, pfTimerCallBack);
}

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_StartLpSingleShotTimer
 * DESCRIPTION: Starts an single-shot timer
 * PARAMETERS:  IN: timerId - the ID of the timer
 *              IN: timeInMilliseconds - time expressed in milliseconds
 *              IN: pfTmrCallBack - callback function
 * RETURN: None
 * NOTES: Customized form of TMR_StartTimer()
 *---------------------------------------------------------------------------*/
void TMR_StartLpSingleShotTimer
(
    tmrTimerID_t timerID,
    tmrTimeInMilliseconds_t timeInMilliseconds,
    void (*pfTimerCallBack)(tmrTimerID_t)
)
{
    TMR_StartLowPowerTimer(timerID, gTmrSingleShotTimer_c, timeInMilliseconds, pfTimerCallBack);
}
#endif /* gTMR_EnableHWLowPowerTimers_d */

/*---------------------------------------------------------------------------
 * NAME: TMR_StopTimer
 * DESCRIPTION: Stop a timer
 * PARAMETERS:  IN: timerID - the ID of the timer
 * RETURN: None
 * NOTES: Associated timer callback function is not called, even if the timer
 *        expires. Does not frees the timer. Safe to call anytime, regardless
 *        of the state of the timer.
 *---------------------------------------------------------------------------*/
void TMR_StopTimer
(
    tmrTimerID_t timerID
)
{
    tmrStatus_t status;
    unsigned int saveInt;
    
    saveInt = IntDisableAll();
    
#if gTMR_EnableHWLowPowerTimers_d    
    status = TMR_GetTimerStatus(timerID, tmrFTMbased_c);
#else
    status = TMR_GetTimerStatus(timerID);
#endif
    
    if ( (status == mTmrStatusActive_c) || (status == mTmrStatusReady_c) ) 
    {
#if gTMR_EnableHWLowPowerTimers_d    	
        TMR_SetTimerStatus(timerID, mTmrStatusInactive_c, tmrFTMbased_c);
#else
        TMR_SetTimerStatus(timerID, mTmrStatusInactive_c);
#endif
        
#if gTMR_EnableHWLowPowerTimers_d        
        DecrementActiveTimerNumber(TMR_GetTimerType(timerID, tmrFTMbased_c));
#else
        DecrementActiveTimerNumber(TMR_GetTimerType(timerID));
#endif
        /* if no sw active timers are enabled, */
        /* call the TMR_Task() to countdown the ticks and stop the hw timer*/
        if (!mNumberOfActiveTimers && !mNumberOfLowPowerActiveTimers) 
            TS_SendEvent(mTimerTaskID, gTMR_Event_c);
    }	

    IntRestoreAll(saveInt);
}

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_StopLpTimer
 * DESCRIPTION: Stop a LPTMR-based timer
 * PARAMETERS:  IN: timerID - the ID of the timer
 * RETURN: None
 *---------------------------------------------------------------------------*/
void TMR_StopLpTimer
(
    tmrTimerID_t timerID
)
{
    tmrStatus_t status;
    unsigned int saveInt;
    
    saveInt = IntDisableAll();
    status = TMR_GetTimerStatus(timerID, tmrLPTMRbased_c);
    
    if ( (status == mTmrStatusActive_c) || (status == mTmrStatusReady_c) ) 
    {
        TMR_SetTimerStatus(timerID, mTmrStatusInactive_c, tmrLPTMRbased_c);
        DecrementActiveLpTmrTimerNumber();
        /* if no sw active timers are enabled, */
        /* call the TMR_Task() to countdown the ticks and stop the hw timer*/
        if (!mLpTmrNumberOfActiveTimers) 
            TS_SendEvent(mTimerTaskID, gLPTMR_Event_c);
    }	

    IntRestoreAll(saveInt);
}
#endif /* gTMR_EnableHWLowPowerTimers_d */

/*---------------------------------------------------------------------------
 * NAME: TMR_Task
 * DESCRIPTION: Timer task. 
 *              Called by the kernel when the timer ISR posts a timer event.
 * PARAMETERS:  IN: events - timer events mask
 * RETURN: None
 * NOTES: none
 *---------------------------------------------------------------------------*/
void TMR_Task
(
    event_t events
)
{
    tmrTimerTicks16_t nextInterruptTime;
    pfTmrCallBack_t pfCallBack;
    tmrTimerTicks16_t currentTimeInTicks;
    tmrTimerStatus_t status;
    tmrTimerTicks16_t ticksSinceLastHere, ticksdiff; 
    uint8_t timerID;
    unsigned int saveInt;
    tmrTimerType_t timerType;
    
#if gTMR_EnableHWLowPowerTimers_d
    tmrTimerTicks16_t lptmr_nextInterruptTime;    
    tmrTimerTicks16_t lptmr_currentTimeInTicks;    
    tmrTimerTicks16_t lptmr_ticksSinceLastHere, lptmr_ticksdiff; 
#endif
    
    /* sample the current ticks */    
    saveInt = IntDisableAll();
    FTMReadCNTRegister(currentTimeInTicks);
#if gTMR_EnableHWLowPowerTimers_d
    LPTMRReadCounterRegister(lptmr_currentTimeInTicks);
#endif
    IntRestoreAll(saveInt);
    
#if gTMR_EnableHWLowPowerTimers_d    
    if(events & gTMR_Event_c)
    {
#else
    	(void)events;	
#endif    	
    	/* calculate difference between current and previous.  */
    	ticksSinceLastHere = (currentTimeInTicks - mPreviousTimeInTicks);
    	/* remember for next time */
    	mPreviousTimeInTicks = currentTimeInTicks;

    	for (timerID = 0; timerID < NumberOfElements(maTmrTimerTable); ++timerID) 
    	{
    		saveInt = IntDisableAll();
#if gTMR_EnableHWLowPowerTimers_d    	
    		status = TMR_GetTimerStatus(timerID, tmrFTMbased_c);
#else
    		status = TMR_GetTimerStatus(timerID);
#endif /* gTMR_EnableHWLowPowerTimers_d */
    		/* If TMR_StartTimer() has been called for this timer, start it's count */
    		/* down as of now. */
    		if (status == mTmrStatusReady_c) 
    		{
#if gTMR_EnableHWLowPowerTimers_d    		
    			TMR_SetTimerStatus(timerID, mTmrStatusActive_c, tmrFTMbased_c);
#else
    			TMR_SetTimerStatus(timerID, mTmrStatusActive_c);
#endif
    			IntRestoreAll(saveInt);
    			continue;
    		}
    		IntRestoreAll(saveInt);

    		/* Ignore any timer that is not active. */
    		if (status != mTmrStatusActive_c) 
    		{
    			continue;
    		}

    		/* This timer is active. Decrement it's countdown.. */
    		if (maTmrTimerTable[timerID].remainingTicks > ticksSinceLastHere) 
    		{
    			maTmrTimerTable[timerID].remainingTicks -= ticksSinceLastHere;
                        FTMReadCNTRegister(maTmrTimerTable[timerID].timestamp);
    			continue;
    		}
#if gTMR_EnableHWLowPowerTimers_d
    		timerType = TMR_GetTimerType(timerID, tmrFTMbased_c);
#else
    		timerType = TMR_GetTimerType(timerID);
#endif
    		/* If this is an interval timer, restart it. Otherwise, mark it as inactive. */
    		if ( (timerType & gTmrSingleShotTimer_c) ||
    				(timerType & gTmrSetMinuteTimer_c) ||
    				(timerType & gTmrSetSecondTimer_c)  ) 
    		{
    			TMR_StopTimer(timerID);
    		} 
    		else 
    		{
    			maTmrTimerTable[timerID].remainingTicks = maTmrTimerTable[timerID].intervalInTicks;
                        FTMReadCNTRegister(maTmrTimerTable[timerID].timestamp);
    		}
    		/* This timer has expired. */
    		pfCallBack = maTmrTimerTable[timerID].pfCallBack;
    		/*Call callback if it is not NULL
    	    	        This is done after the timer got updated,
    	    	        in case the timer gets stopped or restarted in the callback*/
    		if (pfCallBack) 
    		{
    			pfCallBack(timerID);
    		}
    	}  /* for (timerID = 0; timerID < ... */

    	/* Find the shortest active timer. */
    	nextInterruptTime = mMaxToCountDown;

    	for (timerID = 0; timerID < NumberOfElements(maTmrTimerTable); ++timerID) 
    	{
#if gTMR_EnableHWLowPowerTimers_d    	
    		if (TMR_GetTimerStatus(timerID, tmrFTMbased_c) == mTmrStatusActive_c)
#else
    			if (TMR_GetTimerStatus(timerID) == mTmrStatusActive_c)
#endif
    			{
    				if (nextInterruptTime > maTmrTimerTable[timerID].remainingTicks) 
    				{
    					nextInterruptTime = maTmrTimerTable[timerID].remainingTicks;
    				}
    			}
    	}

    	saveInt = IntDisableAll();

    	/* Check to be sure that the timer was not programmed in the past for different source clocks.
    	 * The interrupts are now disabled.
    	 */
    	FTMReadCNTRegister(ticksdiff);  

    	/* Number of ticks to be here */
    	ticksdiff = (uint16_t)(ticksdiff - currentTimeInTicks); 

    	/* Next ticks to count already expired?? */
    	if(ticksdiff >= nextInterruptTime)
    	{  
    		/* Is assumed that a task has to be executed in 4ms...
    	    	    so if the ticks already expired enter in TMR_Task() after 4ms*/
    		nextInterruptTime = ticksdiff + mTicksFor4ms;
    	} 
    	else 
    	{
    		/* Time reference is 4ms, so be sure that won't be loaded 
    	    	           in Cmp Reg. less that 4ms in ticks */
    		if((nextInterruptTime - ticksdiff) < mTicksFor4ms) 
    		{
    			nextInterruptTime = ticksdiff + mTicksFor4ms;
    		}
    	}
    	/* Update the compare register */
    	nextInterruptTime += currentTimeInTicks;
    	gFTMxCnV_c = nextInterruptTime;
    	IntRestoreAll(saveInt);

    	if (!mNumberOfActiveTimers && !mNumberOfLowPowerActiveTimers) 
    	{
    		FTMStopTimerHardware();
    		mTimerHardwareIsRunning = FALSE;
    	} 
    	else if (!mTimerHardwareIsRunning) 
    	{
    		FTMStartTimerHardware();
    		mTimerHardwareIsRunning = TRUE;
    	}    	
#if gTMR_EnableHWLowPowerTimers_d    	
    }
#endif
    
/*
 * handle LPTMR-based software timers
 */    
#if gTMR_EnableHWLowPowerTimers_d    
    if(events & gLPTMR_Event_c)
    {    	
    	/* calculate difference between current and previous.  */
    	lptmr_ticksSinceLastHere = (lptmr_currentTimeInTicks - mLpTmrPreviousTimeInTicks);
    	/* remember for next time */
    	mLpTmrPreviousTimeInTicks = lptmr_currentTimeInTicks;

    	for (timerID = 0; timerID < NumberOfElements(maLpTmrTimerTable); ++timerID) 
    	{
    		saveInt = IntDisableAll();
    		status = TMR_GetTimerStatus(timerID, tmrLPTMRbased_c);
    		/* If TMR_StartTimer() has been called for this timer, start it's count */
    		/* down as of now. */
    		if (status == mTmrStatusReady_c) 
    		{
    			TMR_SetTimerStatus(timerID, mTmrStatusActive_c, tmrLPTMRbased_c);
    			IntRestoreAll(saveInt);
    			continue;
    		}
    		IntRestoreAll(saveInt);

    		/* Ignore any timer that is not active. */
    		if (status != mTmrStatusActive_c) 
    		{
    			continue;
    		}

    		/* This timer is active. Decrement it's countdown.. */
    		if (maLpTmrTimerTable[timerID].remainingTicks > lptmr_ticksSinceLastHere) 
    		{
    			maLpTmrTimerTable[timerID].remainingTicks -= lptmr_ticksSinceLastHere;
                        LPTMRReadCounterRegister(maLpTmrTimerTable[timerID].timestamp);
    			continue;
    		}

    		timerType = TMR_GetTimerType(timerID, tmrLPTMRbased_c);
    		/* If this is an interval timer, restart it. Otherwise, mark it as inactive. */
    		if ( (timerType & gTmrSingleShotTimer_c) ||
    				(timerType & gTmrSetMinuteTimer_c) ||
    				(timerType & gTmrSetSecondTimer_c)  ) 
    		{
    			TMR_StopLpTimer(timerID);
    		} 
    		else 
    		{
    			/* reload */
    			maLpTmrTimerTable[timerID].remainingTicks = maLpTmrTimerTable[timerID].intervalInTicks;
                        LPTMRReadCounterRegister(maLpTmrTimerTable[timerID].timestamp);
    		}
    		/* This timer has expired. */
    		pfCallBack = maLpTmrTimerTable[timerID].pfCallBack;
    		/*Call callback if it is not NULL
    	    	        This is done after the timer got updated,
    	    	        in case the timer gets stopped or restarted in the callback*/
    		if (pfCallBack) 
    		{
    			pfCallBack(timerID);
    		}
    	}  /* for (timerID = 0; timerID < ... */

    	/* Find the shortest active timer. */
    	lptmr_nextInterruptTime = mLpTmrMaxToCountDown;

    	for (timerID = 0; timerID < NumberOfElements(maLpTmrTimerTable); ++timerID) 
    	{
    		if (TMR_GetTimerStatus(timerID, tmrLPTMRbased_c) == mTmrStatusActive_c) 
    		{
    			if (lptmr_nextInterruptTime > maLpTmrTimerTable[timerID].remainingTicks) 
    			{
    				lptmr_nextInterruptTime = maLpTmrTimerTable[timerID].remainingTicks;
    			}
    		}
    	}

    	saveInt = IntDisableAll();

    	/* Check to be sure that the timer was not programmed in the past for different source clocks.
    	 * The interrupts are now disabled.
    	 */
    	LPTMRReadCounterRegister(lptmr_ticksdiff);  

    	/* Number of ticks to be here */
    	lptmr_ticksdiff = (uint16_t)(lptmr_ticksdiff - lptmr_currentTimeInTicks); 

    	/* Next ticks to count already expired?? */
    	if(lptmr_ticksdiff >= lptmr_nextInterruptTime)
    	{  
    		/* Is assumed that a task has to be executed in 4ms...
    	    	    so if the ticks already expired enter in TMR_Task() after 4ms*/
    		lptmr_nextInterruptTime = lptmr_ticksdiff + mLpTmrTicksFor4ms;
    	} 
    	else 
    	{
    		/* Time reference is 4ms, so be sure that won't be loaded 
    	    	           in Cmp Reg. less that 4ms in ticks */
    		if((lptmr_nextInterruptTime - lptmr_ticksdiff) < mLpTmrTicksFor4ms) 
    		{
    			lptmr_nextInterruptTime = lptmr_ticksdiff + mLpTmrTicksFor4ms;
    		}
    	}
    	/* Update the compare register */
    	lptmr_nextInterruptTime += lptmr_currentTimeInTicks;
    	LPTMR0_CMR = lptmr_nextInterruptTime;
    	IntRestoreAll(saveInt);

    	if (!mLpTmrNumberOfActiveTimers) 
    	{
    		LPTMRStopTimerHardware();
    		mLpTmrHardwareIsRunning = FALSE;
    	} 
    	else if (!mLpTmrHardwareIsRunning) 
    	{
    		LPTMRStartTimerHardware();
    		mLpTmrHardwareIsRunning = TRUE;
    	}
    }
#endif    
}

/*---------------------------------------------------------------------------
 * NAME: TMR_EnableTimer
 * DESCRIPTION: Enable the specified timer
 * PARAMETERS:  IN: tmrID - the timer ID
 * RETURN: None
 * NOTES: none
 *---------------------------------------------------------------------------*/
void TMR_EnableTimer
(
    tmrTimerID_t tmrID
)
{    		
    unsigned int saveInt;
    saveInt = IntDisableAll();
    
#if gTMR_EnableHWLowPowerTimers_d    
    if (TMR_GetTimerStatus(tmrID, tmrFTMbased_c) == mTmrStatusInactive_c)
    {      
        IncrementActiveTimerNumber(TMR_GetTimerType(tmrID, tmrFTMbased_c));    
        TMR_SetTimerStatus(tmrID, mTmrStatusReady_c, tmrFTMbased_c);
        TS_SendEvent(mTimerTaskID, gTMR_Event_c);
    }  	    
#else
    if (TMR_GetTimerStatus(tmrID) == mTmrStatusInactive_c)
    {      
    	IncrementActiveTimerNumber(TMR_GetTimerType(tmrID));    
    	TMR_SetTimerStatus(tmrID, mTmrStatusReady_c);
    	TS_SendEvent(mTimerTaskID, gTMR_Event_c);
    }  	   
#endif
    IntRestoreAll(saveInt);
}

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_EnableLpTimer
 * DESCRIPTION: Enable the specified LPTMR-based timer
 * PARAMETERS:  IN: tmrID - the timer ID
 * RETURN: None
 * NOTES: none
 *---------------------------------------------------------------------------*/
void TMR_EnableLpTimer
(
    tmrTimerID_t tmrID
)
{    		
    unsigned int saveInt;
    saveInt = IntDisableAll();

    if (TMR_GetTimerStatus(tmrID, tmrLPTMRbased_c) == mTmrStatusInactive_c)
    {      
    	IncrementActiveLpTmrTimerNumber();    
        TMR_SetTimerStatus(tmrID, mTmrStatusReady_c, tmrLPTMRbased_c);
        TS_SendEvent(mTimerTaskID, gLPTMR_Event_c);
    }  	

    IntRestoreAll(saveInt);
}
#endif /* gTMR_EnableHWLowPowerTimers_d */

/*---------------------------------------------------------------------------
 * NAME: TMR_NotCountedMillisTimeBeforeSleep
 * DESCRIPTION: This function is called by Low Power module;
 * Also this function stops the hardware timer.
 * PARAMETERS:  none
 * RETURN: uint32 - time in millisecond that wasn't counted before
 *		  entering in sleep
 * NOTES: none
 *---------------------------------------------------------------------------*/
uint16_t TMR_NotCountedTicksBeforeSleep
(
    void
)
{
#if (gTMR_EnableLowPowerTimers_d)  
    uint16_t currentTimeInTicks;
  
    if (!mNumberOfLowPowerActiveTimers)
        return 0;

    FTMReadCNTRegister(currentTimeInTicks);
    FTMStopTimerHardware();
    mTimerHardwareIsRunning = FALSE; 

    /* The hw timer is stopped but keep mTimerHardwareIsRunning = TRUE...*/ 
    /* The Lpm timers are considered as being in running mode, so that  */
    /* not to start the hw timer if a TMR event occurs (this shouldn't happen) */ 
     
    return  (uint16_t)(currentTimeInTicks - mPreviousTimeInTicks);
#else  
    return 0;
#endif
} 

/*---------------------------------------------------------------------------
 * NAME: TMR_SyncLpmTimers
 * DESCRIPTION: This function is called by the Low Power module
 * each time the MCU wakes up.
 * PARAMETERS:  sleep duration in milliseconds
 * RETURN: none
 * NOTES: none
 *---------------------------------------------------------------------------*/                             
void TMR_SyncLpmTimers
(
    uint32_t sleepDurationTmrTicks
)
{
#if (gTMR_EnableLowPowerTimers_d) 
    index_t  timerID;
    tmrTimerType_t timerType;

    /* Check if there are low power active timer */
    if (!mNumberOfLowPowerActiveTimers)
        return;          

    /* For each timer, detect the timer type and count down the spent duration in sleep */  
    for (timerID = 0; timerID < NumberOfElements(maTmrTimerTable); ++timerID) 
    {    	
        /* Detect the timer type and count down the spent duration in sleep */
#if gTMR_EnableHWLowPowerTimers_d
        timerType = TMR_GetTimerType(timerID, tmrFTMbased_c);
#else
    	timerType = TMR_GetTimerType(timerID);
#endif
        /* Sync. only the low power timers that are active */
#if gTMR_EnableHWLowPowerTimers_d
        if ( (TMR_GetTimerStatus(timerID, tmrFTMbased_c) == mTmrStatusActive_c)
                && (IsLowPowerTimer(timerType)) )
#else
        if ( (TMR_GetTimerStatus(timerID) == mTmrStatusActive_c)
        		&& (IsLowPowerTimer(timerType)) )
#endif
        {
            /* Timer expired when MCU was in sleep mode??? */
            if( maTmrTimerTable[timerID].remainingTicks > sleepDurationTmrTicks) 
            {
                maTmrTimerTable[timerID].remainingTicks -= sleepDurationTmrTicks;

            } 
            else 
            {
                maTmrTimerTable[timerID].remainingTicks = 0;           
            }

        }

    }/* end for (timerID = 0;.... */ 

    FTMStartTimerHardware();
    FTMReadCNTRegister(mPreviousTimeInTicks);

    TS_SendEvent(mTimerTaskID, gTMR_Event_c);
#else
    sleepDurationTmrTicks = sleepDurationTmrTicks;
#endif /* #if (gTMR_EnableLowPowerTimers_d) */ 
}

#endif /* #if gTMR_Enabled_d */

/*****************************************************************************
 *                               <<< EOF >>>                                 *
 *****************************************************************************/
