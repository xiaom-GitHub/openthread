/**************************************************************************
* Filename: TMR_Interface.h
*
* Description: TIMER interface file for ARM CORTEX-M4 processor
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

#ifndef __TMR_INTERFACE_H__
#define __TMR_INTERFACE_H__

#include "EmbeddedTypes.h"
#include "AppToPlatformConfig.h"
#include "TS_Interface.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

/*
 * NAME: gTMR_Enabled_d
 * DESCRIPTION: Enables / Disables the Timer platform component
 * VALID RANGE: TRUE/FALSE
 */
#ifndef gTMR_Enabled_d
#define gTMR_Enabled_d    TRUE
#endif
    
/*
 * NAME: gTMR_EnableHWLowPowerTimers_d
 * DESCRIPTION: Enables the low power timers based on the 
 * LPTMR hardware module. Setting, available ONLY in 
 * deep sleep mode 14.
 */

#ifndef gTMR_EnableHWLowPowerTimers_d
  #define gTMR_EnableHWLowPowerTimers_d    FALSE
#endif

/*
 * NAME: gTMR_FTMx
 * DESCRIPTION: Specifies which FTM hardware module is used
 * VALID RANGE: 0..2
 */
#ifndef gTMR_FTMx
#define gTMR_FTMx	0
#endif

/*
 * NAME: gTMR_FTM_CNx
 * DESCRIPTION: Specifies which FTM CHANNEL is used
 * VALID RANGE: 0..7
 */
#ifndef gTMR_FTM_CNx
#define gTMR_FTM_CNx	0
#endif

/*
 * NAME: gTMR_EnableLowPowerTimers_d
 * DESCRIPTION: Enable/Disable Low Power Timer
 * WARNING: if PWRLib is enabled and is using deep sleep mode 14, then 
 *          the LPTMR-based software timers are used and therefore this macro 
 *          shall be set to FALSE.
 */
#ifndef gTMR_EnableLowPowerTimers_d
#define gTMR_EnableLowPowerTimers_d    (TRUE)
#endif
    
#if ((gTMR_EnableHWLowPowerTimers_d == TRUE) && (gTMR_EnableLowPowerTimers_d == TRUE))
#error "*** ERROR: gTMR_EnableLowPowerTimers_d needs to be set to FALSE if hardware low-power timers are enabled"
#endif

/*
 * NAME: gTMR_EnableMinutesSecondsTimers_d
 * DESCRIPTION:  Enable/Disable Minutes and Seconds Timers
 * VALID RANGE: TRUE/FALSE
 */
#ifndef gTMR_EnableMinutesSecondsTimers_d
#define gTMR_EnableMinutesSecondsTimers_d	TRUE
#endif

/*
 * NAME: gTmrApplicationTimers_c
 * DESCRIPTION: Number of timers needed by the application
 * VALID RANGE: user defined
 */
#ifndef gTmrApplicationTimers_c
#define gTmrApplicationTimers_c 4
#endif

/*
 * NAME: gTmrStackTimers_c
 * DESCRIPTION: Number of timers needed by the protocol stack
 * VALID RANGE: user defined
 */
#ifndef gTmrStackTimers_c
#define gTmrStackTimers_c	30
#endif

/*
 * NAME: gTmrTotalTimers_c
 * DESCRIPTION: Total number of timers
 * VALID RANGE: sum of application and stack timers
 */
#ifndef gTmrTotalTimers_c
#define gTmrTotalTimers_c	( gTmrApplicationTimers_c + gTmrStackTimers_c )
#endif


/*
 * LPTMR-based timers
 */
#if gTMR_EnableHWLowPowerTimers_d
/*
 * NAME: gLpTmrApplicationTimers_c
 * DESCRIPTION: Number of LPTMR-based timers needed by the application
 */
#ifndef gLpTmrApplicationTimers_c
#define gLpTmrApplicationTimers_c    5
#endif

/*
 * NAME: gLpTmrStackTimers_c
 * DESCRIPTION: Number of LPTMR-based timers needed by the protocol stack
 */
#ifndef gLpTmrStackTimers_c
#define gLpTmrStackTimers_c    5
#endif

/*
 * NAME: gLpTmrTotalTimers_c
 * DESCRIPTION: Total number of LPTMR-based timers
 */
#ifndef gLpTmrTotalTimers_c
#define gLpTmrTotalTimers_c	( gLpTmrApplicationTimers_c + gLpTmrStackTimers_c )
#endif

#endif /* gTMR_EnableHWLowPowerTimers_d */

/*
 * NAME: TmrMilliseconds()
 * DESCRIPTION: Typecast the macro argument into milliseconds
 * VALID RANGE: -
 */
#define TmrMilliseconds( n )	( (tmrTimeInMilliseconds_t) (n) )

/*
 * NAME: TmrSeconds()
 * DESCRIPTION: Converts the macro argument (i.e. seconds) into milliseconds
 * VALID RANGE: - 
 */
#define TmrSeconds( n )			( (tmrTimeInMilliseconds_t) (TmrMilliseconds(n) * 1000) )

/*
 * NAME: TmrMinutes()
 * DESCRIPTION: Converts the macro argument (i.e. minutes) into milliseconds
 * VALID RANGE: -
 */
#define TmrMinutes( n )			( (tmrTimeInMilliseconds_t) (TmrSeconds(n) * 60) )

/*
 * NAME: gTmrInvalidTimerID_c
 * DESCRIPTION: Reserved for invalid timer id
 * VALID RANGE: 0xFF
 */
#define gTmrInvalidTimerID_c	0xFF

/*
 * NAME: gTmrSingleShotTimer_c, gTmrIntervalTimer_c,
 *       gTmrSetMinuteTimer_c, gTmrSetSecondTimer_c,
 *       gTmrLowPowerTimer_c
 * DESCRIPTION: Timer types coded values
 * VALID RANGE: see definitions below
 */
#define gTmrSingleShotTimer_c	0x01
#define gTmrIntervalTimer_c     0x02
#define gTmrSetMinuteTimer_c	0x04
#define gTmrSetSecondTimer_c	0x08
#define gTmrLowPowerTimer_c     0x10

/*
 * NAME: gTmrMinuteTimer_c
 * DESCRIPTION: Minute timer definition
 * VALID RANGE: see definition below
 */
#define gTmrMinuteTimer_c       ( gTmrSetMinuteTimer_c )

/*
 * NAME: gTmrSecondTimer_c
 * DESCRIPTION: Second timer definition
 * VALID RANGE: see definition below
 */
#define gTmrSecondTimer_c       ( gTmrSetSecondTimer_c )

/*
 * NAME: See below
 * DESCRIPTION: LP minute/second/millisecond timer definitions
 * VALID VALUES: See definitions below
 */
#define gTmrLowPowerMinuteTimer_c			( gTmrMinuteTimer_c | gTmrLowPowerTimer_c )
#define gTmrLowPowerSecondTimer_c			( gTmrSecondTimer_c | gTmrLowPowerTimer_c )
#define gTmrLowPowerSingleShotMillisTimer_c ( gTmrSingleShotTimer_c | gTmrLowPowerTimer_c )
#define gTmrLowPowerIntervalMillisTimer_c	( gTmrIntervalTimer_c | gTmrLowPowerTimer_c )

/*****************************************************************************
******************************************************************************
* Public type definitions
******************************************************************************
*****************************************************************************/
 
/*
 * NAME: tmrTimerTicks_t
 * DESCRIPTION: 16-bit timer ticks type definition
 * VALID VALUES: see definition
 */
typedef uint16_t tmrTimerTicks16_t;

/*
 * NAME: tmrTimerTicks_t
 * DESCRIPTION: 32-bit timer ticks type definition
 * VALID VALUES: see definition
 */
typedef uint32_t tmrTimerTicks32_t;

/*
 * NAME: tmrTimeInMilliseconds_t
 * DESCRIPTION: Times specified in milliseconds (max 0x3ffff)
 */
typedef uint32_t	tmrTimeInMilliseconds_t;

/*
 * NAME: tmrTimeInMinutes_t
 * DESCRIPTION: Times specified in minutes (up to 40 days)
 */
typedef uint32_t	tmrTimeInMinutes_t;

/*
 * NAME: tmrTimeInSeconds_t
 * DESCRIPTION: Times specified in seconds (up to 65535)
 */
typedef uint32_t	tmrTimeInSeconds_t;

/*
 * NAME: tmrTimerType_t
 * DESCRIPTION: Timer type
 */
typedef uint8_t		tmrTimerID_t;

/*
 * NAME: tmrTimerType_t
 * DESCRIPTION: Timer type
 */
typedef uint8_t		tmrTimerType_t;

/*
 * NAME: pfTmrCallBack_t
 * DESCRIPTION: Timer callback function
 */
typedef void ( *pfTmrCallBack_t ) ( tmrTimerID_t );


/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/

/* none */

/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/

#if gTMR_Enabled_d
 
/*---------------------------------------------------------------------------
 * NAME: TMR_Init
 * DESCRIPTION: initialize the timer module
 * PARAMETERS: -
 * RETURN: -
 *---------------------------------------------------------------------------*/
extern void TMR_Init 
(
    void
);

/*---------------------------------------------------------------------------
 * NAME: TMR_NotifyClkChanged
 * DESCRIPTION: This function is called when the clock is changed
 * PARAMETERS: IN: clkKhz (uint32_t) - new clock
 * RETURN: -
 *---------------------------------------------------------------------------*/
extern void TMR_NotifyClkChanged
(
    uint32_t clkKhz
);

/*---------------------------------------------------------------------------
 * NAME: TMR_AllocateTimer
 * DESCRIPTION: allocate a timer
 * PARAMETERS: -
 * RETURN: timer ID
 *---------------------------------------------------------------------------*/
extern tmrTimerID_t TMR_AllocateTimer
(
    void
);

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_AllocateLpTimer
 * DESCRIPTION: allocate a LPTMR-based timer
 * PARAMETERS: -
 * RETURN: timer ID
 *---------------------------------------------------------------------------*/
extern tmrTimerID_t TMR_AllocateLpTimer
(
    void 
);
#endif
                                     
/*---------------------------------------------------------------------------
 * NAME: TMR_AreAllTimersOff
 * DESCRIPTION: Check if all timers except the LP timers are OFF.
 * PARAMETERS: -
 * RETURN: TRUE if there are no active non-low power timers, FALSE otherwise
 *---------------------------------------------------------------------------*/
extern bool_t TMR_AreAllTimersOff
(
    void
);

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_AreAllLpTimersOff
 * DESCRIPTION: Check if all LPTMR-based timers are OFF.
 * PARAMETERS: -
 * RETURN: TRUE if there are no active non-low power timers, FALSE otherwise
 *---------------------------------------------------------------------------*/
extern bool_t TMR_AreAllLpTimersOff
(
    void
);
#endif

/*---------------------------------------------------------------------------
 * NAME: TMR_FreeTimer
 * DESCRIPTION: Free a timer
 * PARAMETERS:  IN: timerID - the ID of the timer
 * RETURN: -
 * NOTES: Safe to call even if the timer is running.
 *        Harmless if the timer is already free.
 *---------------------------------------------------------------------------*/
extern void TMR_FreeTimer
(
    tmrTimerID_t timerID
);

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_FreeLpTimer
 * DESCRIPTION: Free a LPTMR-based timer
 * PARAMETERS:  IN: timerID - the ID of the timer
 * RETURN: -
 * NOTES: Safe to call even if the timer is running.
 *        Harmless if the timer is already free.
 *---------------------------------------------------------------------------*/
extern void TMR_FreeLpTimer
(
    tmrTimerID_t timerID
);
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
extern void TMR_InterruptHandler
(
    void
);

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
extern void PWRLib_LPTMR_Isr
(
    void
);
#endif

/*---------------------------------------------------------------------------
 * NAME: TMR_IsTimerActive
 * DESCRIPTION: Check if a specified timer is active
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: TRUE if the timer (specified by the timerID) is active,
 *         FALSE otherwise
 *---------------------------------------------------------------------------*/
extern bool_t TMR_IsTimerActive
(
    tmrTimerID_t timerID
);

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_IsLpTimerActive
 * DESCRIPTION: Check if a specified LPTMR-based timer is active
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: TRUE if the timer (specified by the timerID) is active,
 *         FALSE otherwise
 *---------------------------------------------------------------------------*/
extern bool_t TMR_IsLpTimerActive
(
    tmrTimerID_t timerID
);
#endif

/*---------------------------------------------------------------------------
 * NAME: TMR_IsTimerReady
 * DESCRIPTION: Check if a specified timer is ready
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: TRUE if the timer (specified by the timerID) is ready,
 *         FALSE otherwise
 *---------------------------------------------------------------------------*/
extern bool_t TMR_IsTimerReady
(
    tmrTimerID_t timerID
);

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_IsLpTimerReady
 * DESCRIPTION: Check if a specified LPTMR-based timer is ready
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: TRUE if the timer (specified by the timerID) is ready,
 *         FALSE otherwise
 *---------------------------------------------------------------------------*/
extern bool_t TMR_IsLpTimerReady
(
    tmrTimerID_t timerID
);
#endif

/*---------------------------------------------------------------------------
 * NAME: TMR_GetRemainingTime
 * DESCRIPTION: Returns the remaining time until timeout, for the specified
 *              timer
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: remaining time in milliseconds until timer timeouts.
 *---------------------------------------------------------------------------*/
extern uint32_t TMR_GetRemainingTime
(
    tmrTimerID_t tmrID
);
   
#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_GetLpRemainingTime
 * DESCRIPTION: Returns the remaining time until timeout, for the specified
 *              timer
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: remaining time in milliseconds until specified timer timeout
 *---------------------------------------------------------------------------*/
extern uint32_t TMR_GetLpRemainingTime
(
    tmrTimerID_t tmrID
);
#endif

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
extern void TMR_StartTimer
(
    tmrTimerID_t timerID,                       
    tmrTimerType_t timerType,                   
    tmrTimeInMilliseconds_t timeInMilliseconds, 
    void (*pfTimerCallBack)(tmrTimerID_t)       
);

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
extern void TMR_StartLowPowerTimer
(
    tmrTimerID_t timerId,
    tmrTimerType_t timerType,
    uint32_t timeIn,
    void (*pfTmrCallBack)(tmrTimerID_t)
);

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
extern void TMR_StartMinuteTimer
(
    tmrTimerID_t timerId, 
    tmrTimeInMinutes_t timeInMinutes, 
    void (*pfTmrCallBack)(tmrTimerID_t)
);

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
extern void TMR_StartLpMinuteTimer
(
    tmrTimerID_t timerId, 
    tmrTimeInMinutes_t timeInMinutes, 
    void (*pfTmrCallBack)(tmrTimerID_t)
);
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
extern void TMR_StartSecondTimer
(
    tmrTimerID_t timerId, 
    tmrTimeInSeconds_t timeInSeconds, 
    void (*pfTmrCallBack)(tmrTimerID_t)
);

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
extern void TMR_StartLpSecondTimer
(
    tmrTimerID_t timerId, 
    tmrTimeInSeconds_t timeInSeconds, 
    void (*pfTmrCallBack)(tmrTimerID_t)
);
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
extern void TMR_StartIntervalTimer
(
    tmrTimerID_t timerID,
    tmrTimeInMilliseconds_t timeInMilliseconds,
    void (*pfTimerCallBack)(tmrTimerID_t)
);

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
extern void TMR_StartLpIntervalTimer
(
    tmrTimerID_t timerID,
    tmrTimeInMilliseconds_t timeInMilliseconds,
    void (*pfTimerCallBack)(tmrTimerID_t)
);
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
extern void TMR_StartSingleShotTimer
(
    tmrTimerID_t timerID,
    tmrTimeInMilliseconds_t timeInMilliseconds,
    void (*pfTimerCallBack)(tmrTimerID_t)
);

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
extern void TMR_StartLpSingleShotTimer
(
    tmrTimerID_t timerID,
    tmrTimeInMilliseconds_t timeInMilliseconds,
    void (*pfTimerCallBack)(tmrTimerID_t)
);
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
extern void TMR_StopTimer
(
    tmrTimerID_t timerID
);

#if gTMR_EnableHWLowPowerTimers_d
/*---------------------------------------------------------------------------
 * NAME: TMR_StopLpTimer
 * DESCRIPTION: Stop a LPTMR-based timer
 * PARAMETERS:  IN: timerID - the ID of the timer
 * RETURN: None
 *---------------------------------------------------------------------------*/
extern void TMR_StopLpTimer
(
    tmrTimerID_t timerID
);
#endif /* gTMR_EnableHWLowPowerTimers_d */

/*---------------------------------------------------------------------------
 * NAME: TMR_Task
 * DESCRIPTION: Timer task. 
 *              Called by the kernel when the timer ISR posts a timer event.
 * PARAMETERS:  IN: events - timer events mask
 * RETURN: None
 * NOTES: none
 *---------------------------------------------------------------------------*/
extern void TMR_Task
(
    event_t events
);

/*---------------------------------------------------------------------------
 * NAME: TMR_EnableTimer
 * DESCRIPTION: Enable the specified timer
 * PARAMETERS:  IN: tmrID - the timer ID
 * RETURN: None
 * NOTES: none
 *---------------------------------------------------------------------------*/
extern void TMR_EnableTimer
(
    tmrTimerID_t tmrID
);

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
);
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
extern uint16_t TMR_NotCountedTicksBeforeSleep
(
    void
);

/*---------------------------------------------------------------------------
 * NAME: TMR_SyncLpmTimers
 * DESCRIPTION: This function is called by the Low Power module
 * each time the MCU wakes up.
 * PARAMETERS:  sleep duration in milliseconds
 * RETURN: none
 * NOTES: none
 *---------------------------------------------------------------------------*/                             
extern void TMR_SyncLpmTimers
(
    uint32_t sleepDurationTmrTicks
);

/*---------------------------------------------------------------------------
 * NAME: TmrTicksFromMilliseconds
 * DESCRIPTION: Convert milliseconds to ticks
 * PARAMETERS:  IN: milliseconds
 * RETURN: hwTmrType - ticks number
 * NOTES: none
 *---------------------------------------------------------------------------*/
extern tmrTimerTicks32_t TmrTicksFromMilliseconds
(
    tmrTimeInMilliseconds_t milliseconds
);

/*---------------------------------------------------------------------------
 * NAME: TMR_AllocateMinuteTimer
 * DESCRIPTION: Reserve a minute timer
 * PARAMETERS: -
 * RETURN: gTmrInvalidTimerID_c if there are no timers available
 * NOTES: none
 *---------------------------------------------------------------------------*/
#define TMR_AllocateMinuteTimer() TMR_AllocateTimer()


/*---------------------------------------------------------------------------
 * NAME: TMR_AllocateSecondTimer
 * DESCRIPTION: Reserve a second timer
 * PARAMETERS: -
 * RETURN: gTmrInvalidTimerID_c if there are no timers available
 * NOTES: none
 *---------------------------------------------------------------------------*/
#define TMR_AllocateSecondTimer() TMR_AllocateTimer() 

/*---------------------------------------------------------------------------
 * NAME: TMR_FreeMinuteTimer
 * DESCRIPTION: Free a minute timer. Safe to call even if the timer is running
 * PARAMETERS: timer ID
 * RETURN: -
 * NOTES: none
 *---------------------------------------------------------------------------*/
#define TMR_FreeMinuteTimer(timerID) TMR_FreeTimer(timerID)

/*---------------------------------------------------------------------------
 * NAME: TMR_FreeSecondTimer
 * DESCRIPTION: Free a second timer. Safe to call even if the timer is running
 * PARAMETERS: timer ID
 * RETURN: -
 * NOTES: none
 *---------------------------------------------------------------------------*/
#define TMR_FreeSecondTimer(timerID) TMR_FreeTimer(timerID)

/*---------------------------------------------------------------------------
 * NAME: TMR_StopMinuteTimer
 * DESCRIPTION: Stop a timer started by TMR_StartMinuteTimer()
 * PARAMETERS: timer ID
 * RETURN: TRUE if there are no active non-low power timers, FALSE otherwise
 *         Used by power management
 * NOTES: none
 *---------------------------------------------------------------------------*/
#define TMR_StopMinuteTimer(timerID)  TMR_StopTimer(timerID)  

/*  */
/*---------------------------------------------------------------------------
 * NAME: TMR_StopSecondTimer
 * DESCRIPTION: stop a timer started by TMR_StartSecondTimer()
 * PARAMETERS: timer ID
 * RETURN: TRUE if there are no active non-low power timers, FALSE otherwise
 *         Used by power management
 * NOTES: none
 *---------------------------------------------------------------------------*/
#define TMR_StopSecondTimer(timerID)  TMR_StopTimer(timerID)

/*
 * LPTMR
 */
#if gTMR_EnableHWLowPowerTimers_d
#define TMR_AllocateLpMinuteTimer() TMR_AllocateLpTimer()
#define TMR_AllocateLpSecondTimer() TMR_AllocateLpTimer() 
#define TMR_FreeLpMinuteTimer(timerID) TMR_FreeLpTimer(timerID)
#define TMR_FreeLpSecondTimer(timerID) TMR_FreeLpTimer(timerID)
#define TMR_StopLpMinuteTimer(timerID)  TMR_StopLpTimer(timerID)  
#define TMR_StopLpSecondTimer(timerID)  TMR_StopLpTimer(timerID)
#endif


#else /* stub functions */

#define TMR_Init()
#define TMR_NotifyClkChanged(clkKhz)
#define TMR_AllocateTimer()	gTmrInvalidTimerID_c
#define TMR_AreAllTimersOff() TRUE
#define TMR_FreeTimer(timerID)
#define TMR_InterruptHandler VECT_DefaultISR
#define TMR_IsTimerActive(timerID)	FALSE
#define TMR_StartTimer(timerID,timerType,timeInMilliseconds, pfTimerCallBack)
#define TMR_StartLowPowerTimer(timerId,timerType,timeIn,pfTmrCallBack)
#if gTMR_EnableMinutesSecondsTimers_d
#define TMR_StartMinuteTimer(timerId,timeInMinutes,pfTmrCallBack)
#endif
#if gTMR_EnableMinutesSecondsTimers_d
#define TMR_StartSecondTimer(timerId,timeInSeconds,pfTmrCallBack)
#endif
#define TMR_StartIntervalTimer(timerID,timeInMilliseconds,pfTimerCallBack)
#define TMR_StartSingleShotTimer(timerID,timeInMilliseconds,pfTimerCallBack)
#define TMR_StopTimer(timerID)
#define TMR_Task(events)
#define TMR_EnableTimer(tmrID)
#define TMR_NotCountedTicksBeforeSleep()	0
#define TMR_SyncLpmTimers(sleepDurationTmrTicks)
#define TmrTicksFromMilliseconds(milliseconds)		0
#define TMR_AllocateMinuteTimer() TMR_AllocateTimer()
#define TMR_AllocateSecondTimer() TMR_AllocateTimer() 
#define TMR_FreeMinuteTimer(timerID) TMR_FreeTimer(timerID)
#define TMR_FreeSecondTimer(timerID) TMR_FreeTimer(timerID)
#define TMR_StopMinuteTimer(timerID)  TMR_StopTimer(timerID)  
#define TMR_StopSecondTimer(timerID)  TMR_StopTimer(timerID)
#define TMR_GetRemainingTime(timerID)    0

#if gTMR_EnableHWLowPowerTimers_d
#define TMR_AllocateLpTimer()	gTmrInvalidTimerID_c
#define TMR_AreAllLpTimersOff() TRUE
#define TMR_FreeLpTimer(timerID)
#define TMR_InterruptHandler VECT_DefaultISR
#define PWRLib_LPTMR_Isr VECT_DefaultISR
#define TMR_IsLpTimerActive(timerID)	FALSE
#if gTMR_EnableMinutesSecondsTimers_d
#define TMR_StartMinuteTimer(timerId,timeInMinutes,pfTmrCallBack)
#define TMR_StartLpMinuteTimer(timerId,timeInMinutes,pfTmrCallBack)
#endif
#if gTMR_EnableMinutesSecondsTimers_d
#define TMR_StartSecondTimer(timerId,timeInSeconds,pfTmrCallBack)
#define TMR_StartLpSecondTimer(timerId,timeInSeconds,pfTmrCallBack)
#endif
#define TMR_StartLpIntervalTimer(timerID,timeInMilliseconds,pfTimerCallBack)
#define TMR_StartLpSingleShotTimer(timerID,timeInMilliseconds,pfTimerCallBack)
#define TMR_StopLpTimer(timerID)
#define TMR_EnableLpTimer(tmrID)
#define TMR_AllocateLpMinuteTimer() TMR_AllocateLpTimer()
#define TMR_AllocateLpSecondTimer() TMR_AllocateLpTimer() 
#define TMR_FreeLpMinuteTimer(timerID) TMR_FreeLpTimer(timerID)
#define TMR_FreeLpSecondTimer(timerID) TMR_FreeLpTimer(timerID)
#define TMR_StopLpMinuteTimer(timerID)  TMR_StopLpTimer(timerID)  
#define TMR_StopLpSecondTimer(timerID)  TMR_StopLpTimer(timerID)
#define TMR_GetLpRemainingTime(timerID)    0
#endif

#endif /* gTMR_Enabled_d */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __TMR_INTERFACE_H__ */

/*****************************************************************************
 *                               <<< EOF >>>                                 *
 *****************************************************************************/
