/**************************************************************************
* Filename: Timer.h
*
* Description: TIMER header file for AMR CORTEX-M4 processor
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

 #ifndef __TIMER_H__
 #define __TIMER_H__
 
#include "TMR_Interface.h"
#include "PortConfig.h"
#include "EmbeddedTypes.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/

/***************************************************************** 
 *                   MCU SPECIFIC DEFINITIONS                    *
 *****************************************************************/
#if ((MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
	/* FTMx IRQ number */
	#define gTMR_FTM0_IRQ_c                   42
	#define gTMR_FTM1_IRQ_c                   43
	#define gTMR_FTM2_IRQ_c                   44
	/* FTMx clock gating register */
	#define gTMR_FTM_SIM_SCGC_REG_c           SIM_SCGC6
	/* FTMx clock gating register bit */ 
	#define gTMR_FTM_0_SIM_SCGC_BIT_c         24       
	#define gTMR_FTM_1_SIM_SCGC_BIT_c         25
	#define gTMR_FTM_2_SIM_SCGC_BIT_c         26
#elif ((MCU_MK60D10 == 1) || (MCU_MK60N512VMD100 == 1))
	/* FTMx IRQ number */
	#define gTMR_FTM0_IRQ_c                   62
	#define gTMR_FTM1_IRQ_c                   63
	#define gTMR_FTM2_IRQ_c                   64
	/* FTMx SIM SCGC associated register */
	#if (gTMR_FTMx == 2)
	#define gTMR_FTM_SIM_SCGC_REG_c           SIM_SCGC3
	#else
	#define gTMR_FTM_SIM_SCGC_REG_c           SIM_SCGC6
	#endif
	/* FTM SIM SCGC bit position */
	#define gTMR_FTM_0_SIM_SCGC_BIT_c         24       
	#define gTMR_FTM_1_SIM_SCGC_BIT_c         25
	#define gTMR_FTM_2_SIM_SCGC_BIT_c         24
#elif (MCU_MK20D5 == 1)
	/* FTMx IRQ number */
	#define gTMR_FTM0_IRQ_c                   25
	#define gTMR_FTM1_IRQ_c                   26
	/* FTMx SIM SCGC associated register */
	#define gTMR_FTM_SIM_SCGC_REG_c           SIM_SCGC6
	/* FTM SIM SCGC bit position */
	#define gTMR_FTM_0_SIM_SCGC_BIT_c         24       
	#define gTMR_FTM_1_SIM_SCGC_BIT_c         25
#endif

#if gTMR_EnableHWLowPowerTimers_d
    #define gTMR_LPTMR_IRQ_c    (INT_LPTimer - 16)
#endif

/*
 * NAME: NumberOfElements()
 * DESCRIPTION: calculates the number of elements in a given array
 */
#define NumberOfElements(array)   ((sizeof(array) / (sizeof(array[0]))))

/*
 * NAME: mTmrStatusFree_c
 * DESCRIPTION: self explanatory
 */
#define mTmrStatusFree_c    0x00

/*
 * NAME: mTmrStatusActive_c
 * DESCRIPTION: self explanatory
 */
#define mTmrStatusActive_c    0x20

/*
 * NAME: mTmrStatusReady_c
 * DESCRIPTION: self explanatory
 */
#define mTmrStatusReady_c    0x40

/*
 * NAME: mTmrStatusInactive_c
 * DESCRIPTION: self explanatory
 */
#define mTmrStatusInactive_c    0x80

/*
 * NAME: mTimerStatusMask_c
 * DESCRIPTION: timer status mask
 */
#define mTimerStatusMask_c      ( mTmrStatusActive_c \
| mTmrStatusReady_c \
| mTmrStatusInactive_c)

/*
 * NAME: TMR_IsTimerAllocated()
 * DESCRIPTION: checks if a specified timer is allocated
 */
#define TMR_IsTimerAllocated(timerID)   (maTmrTimerStatusTable[(timerID)])

#if gTMR_EnableHWLowPowerTimers_d
/*
 * NAME: TMR_IsLpTimerAllocated()
 * DESCRIPTION: checks if a specified timer is allocated
 */
#define TMR_IsLpTimerAllocated(timerID)   (maLpTmrTimerStatusTable[(timerID)])
#endif

/*
 * NAME: TMR_MarkTimerFree()
 * DESCRIPTION: marks the specified timer as free
 */
#define TMR_MarkTimerFree(timerID)       maTmrTimerStatusTable[(timerID)] = 0

#if gTMR_EnableHWLowPowerTimers_d
/*
 * NAME: TMR_MarkLpTimerFree()
 * DESCRIPTION: marks the specified LPTMR-based timer as free
 */
#define TMR_MarkLpTimerFree(timerID)       maLpTmrTimerStatusTable[(timerID)] = 0
#endif

/*
 * NAME: IsLowPowerTimer()
 * DESCRIPTION: Detect if the timer is a low-power timer
 */
#define IsLowPowerTimer(type)           ((type) & gTmrLowPowerTimer_c)

/*
 * NAME: mTimerType_c
 * DESCRIPTION: timer types
 */
#define mTimerType_c            ( gTmrSingleShotTimer_c \
	| gTmrSetSecondTimer_c \
	| gTmrSetMinuteTimer_c \
	| gTmrIntervalTimer_c \
	| gTmrLowPowerTimer_c )

/*
 * NAME: gTMR_Event_c
 * DESCRIPTION: TMR_Task() event flag needed for FTM event signalling
 */
#define gTMR_Event_c    ( 1 << 0 )

/*
 * NAME: gLPTMR_Event_c
 * DESCRIPTION: TMR_Task() event flag needed for LPTMR event signalling
 */
#define gLPTMR_Event_c  ( 1 << 1)

/*
 * NAME: gTMR_FTMIrqNo
 * DESCRIPTION: FTMx interrupt number
 * VALID VALUES: refer to Kinetis manual
 */

#if(gTMR_FTMx == 0)
#define gTMR_FTMIrqNo                   gTMR_FTM0_IRQ_c
#elif (gTMR_FTMx == 1)
#define gTMR_FTMIrqNo                   gTMR_FTM1_IRQ_c
#else
#define gTMR_FTMIrqNo                   gTMR_FTM2_IRQ_c
#endif

#if(gTMR_FTMx == 0)
#define gTMR_FTM_SIM_SCGC_BIT_c          gTMR_FTM_0_SIM_SCGC_BIT_c
#elif (gTMR_FTMx == 1)
#define gTMR_FTM_SIM_SCGC_BIT_c          gTMR_FTM_1_SIM_SCGC_BIT_c
#else
#define gTMR_FTM_SIM_SCGC_BIT_c          gTMR_FTM_2_SIM_SCGC_BIT_c
#endif

/*
 * NAME: gTMR_FTMInterruptPriority
 * DESCRIPTION: FTMx interrupt priority
 * VALID VALUES: 0..15
 */
#ifndef gTMR_FTMInterruptPriority
#define gTMR_FTMInterruptPriority       3
#endif

/*
 * NAME: gTMR_LPTMRInterruptPriority
 * DESCRIPTION: LPTMR interrupt priority
 * VALID VALUES: 0..15
 */
#ifndef gTMR_LPTMRInterruptPriority
#define gTMR_LPTMRInterruptPriority       3
#endif

/*
 * NAME: gTmrFTM_max
 * DESCRIPTION: Maximum number of FTM hardware modules 
 * 			   available on target platform	  
 * VALID VALUES: refer to Kinetis manual
 */

#ifndef gTmrFTM_max
#define gTmrFTM_max                     2 
#endif

/*
 * NAME: gTmrFTM_CN_max
 * DESCRIPTION: Maximum number of FTM channels
 * 			   available on target platform	  
 * VALID VALUES: refer to Kinetis manual
 */

#ifndef gTmrFTM_CN_max
#define gTmrFTM_CN_max                  7
#endif

/* Some checks ... */
#if ( gTMR_FTMx > gTmrFTM_max )
#error "FTM timer out of range [0-2]"
#endif

#if ( gTMR_FTM_CNx > gTmrFTM_CN_max )
#error "FTM channel out of range [0-7]"
#endif

/*
 * NAME: see below
 * DESCRIPTION: FTM registers definitions  
 * VALID VALUES: refer to Kinetis manual
 */

#if(gTMR_FTMx == 0)
    #define gFTMxSC_c       		FTM0_SC                 /* FTM0 Status and Control register. */
    #define gFTMxCNT        		FTM0_CNT                /* FTM0 Counter register */
    #define gFTMxMOD_c     			FTM0_MOD                /* FTM0 Modulo register */
    #define gFTMxMODE_c			    FTM0_MODE               /* FTM0 Features Mode Selection register */
    #define gFTMxCNTIN_c			FTM0_CNTIN              /* FTM0 Counter Initial Value register */ 

    /*
     * NAME: see below
     * DESCRIPTION: FTM channel registers definition
     * VALID VALUES: refer to Kinetis manual
     */

    #if (gTMR_FTM_CNx == 0)
        #define gFTMxCnSC_c     	FTM0_C0SC               /* Channel 0 Status and Control register */
        #define gFTMxCnV_c      	FTM0_C0V                /* Channel 0 Value register */
    #elif (gTMR_FTM_CNx == 1)
        #define gFTMxCnSC_c     	FTM0_C1SC               /* Channel 1 Status and Control register */
        #define gFTMxCnV_c      	FTM0_C1V                /* Channel 1 Value register */
    #elif (gTMR_FTM_CNx == 2)
        #define gFTMxCnSC_c     	FTM0_C2SC               /* Channel 2 Status and Control register */
        #define gFTMxCnV_c      	FTM0_C2V                /* Channel 2 Value register */
    #elif (gTMR_FTM_CNx == 3)
        #define gFTMxCnSC_c     	FTM0_C3SC               /* Channel 3 Status and Control register */
        #define gFTMxCnV_c      	FTM0_C3V                /* Channel 3 Value register */
    #elif (gTMR_FTM_CNx == 4)
        #define gFTMxCnSC_c     	FTM0_C4SC               /* Channel 4 Status and Control register */
        #define gFTMxCnV_c      	FTM0_C4V                /* Channel 4 Value register */
    #elif (gTMR_FTM_CNx == 5)
        #define gFTMxCnSC_c     	FTM0_C5SC               /* Channel 5 Status and Control register */
        #define gFTMxCnV_c      	FTM0_C5V                /* Channel 5 Value register */
    #elif (gTMR_FTM_CNx == 6)
        #define gFTMxCnSC_c     	FTM0_C6SC               /* Channel 6 Status and Control register */
        #define gFTMxCnV_c      	FTM0_C6V                /* Channel 6 Value register */
    #else
        #define gFTMxCnSC_c     	FTM0_C7SC               /* Channel 7 Status and Control register */
        #define gFTMxCnV_c      	FTM0_C7V                /* Channel 7 Value register */
    #endif
    
#elif (gTMR_FTMx == 1)
    #define gFTMxSC_c       		FTM1_SC                 /* FTM1 Status and Control register. */
    #define gFTMxCNT        		FTM1_CNT                /* FTM1 Counter register */
    #define gFTMxMOD_c     			FTM1_MOD                /* FTM1 Modulo register */
    #define gFTMxMODE_c			    FTM1_MODE               /* FTM1 Features Mode Selection register */
    #define gFTMxCNTIN_c			FTM1_CNTIN              /* FTM1 Counter Initial Value register */

    /*
     * NAME: see below
     * DESCRIPTION: FTM channel registers definition
     * VALID VALUES: refer to Kinetis manual
     */
     
    #if (gTMR_FTM_CNx == 0)
        #define gFTMxCnSC_c     	FTM1_C0SC               /* Channel 0 Status and Control register */
        #define gFTMxCnV_c      	FTM1_C0V                /* Channel 0 Value register */
    #else
        #define gFTMxCnSC_c     	FTM1_C1SC               /* Channel 1 Status and Control register */
        #define gFTMxCnV_c      	FTM1_C1V                /* Channel 1 Value register */
    #endif

#else 
    #define gFTMxSC_c       		FTM2_SC                 /* FTM2 Status and Control register. */
    #define gFTMxCNT        		FTM2_CNT                /* FTM2 Counter register */
    #define gFTMxMOD_c     			FTM2_MOD                /* FTM2 Modulo register */
    #define gFTMxMODE_c			    FTM2_MODE               /* FTM2 Features Mode Selection register */
    #define gFTMxCNTIN_c			FTM2_CNTIN              /* FTM2 Counter Initial Value register */
    
    /*
     * NAME: see below
     * DESCRIPTION: FTM channel registers definition
     * VALID VALUES: refer to Kinetis manual
     */
    #if (gTMR_FTM_CNx == 0)
        #define gFTMxCnSC_c     	FTM2_C0SC         		/* Channel 0 Status and Control register */
        #define gFTMxCnV_c      	FTM2_C0V          		/* Channel 0 Value register */
    #else
        #define gFTMxCnSC_c     	FTM2_C1SC         		/* Channel 1 Status and Control register */
        #define gFTMxCnV_c      	FTM2_C1V          		/* Channel 1 Value register */
    #endif
    
#endif

/*
 * NAME: gTmrSourceClkKHz_c
 * DESCRIPTION: Source timer clock in KHz
 */
#define gTmrSourceClkKHz_c      48000

/*
 * NAME: gFTM_CNT_OVERFLOW_VALUE_c
 * DESCRIPTION: FTM counter overflow value
 */   
#define gFTM_CNT_OVERFLOW_VALUE_c     0xFFFF
   
/*
 * NAME: gLPTMR_OVERFLOW_VALUE_c
 * DESCRIPTION: LPTMR counter overflow value
 */   
#define gLPTMR_OVERFLOW_VALUE_c   0xFFFF

/*
 * NAME: gFTMxSC_PrescaleX_c
 * DESCRIPTION: Prescaler values
 * VALID VALUES: refer to Kinetis manual
 */
#define gFTMxSC_Prescale1_c     0x00    					/* For prescale Value of 1   */
#define gFTMxSC_Prescale2_c     0x01    					/* For prescale Value of 2   */
#define gFTMxSC_Prescale4_c     0x02    					/* For prescale Value of 4   */
#define gFTMxSC_Prescale8_c     0x03    					/* For prescale Value of 8   */
#define gFTMxSC_Prescale16_c    0x04    					/* For prescale Value of 16  */
#define gFTMxSC_Prescale32_c    0x05    					/* For prescale Value of 32  */
#define gFTMxSC_Prescale64_c    0x06    					/* For prescale Value of 64  */
#define gFTMxSC_Prescale128_c   0x07    					/* For prescale Value of 128 */

/*
The resolution of the timer is between 
4ms(max. time a task to be executed) - 0xffff * (1tick in ms).  
 */

/*
 * NAME: gFTMxSC_PrescaleCount_c
 * DESCRIPTION: Prescaler value used by TMR module
 *              Changing the prescaler the resolution will increase or decrease
 * VALID VALUES: refer to Kinetis manual
 */ 
#define gFTMxSC_PrescaleCount_c     gFTMxSC_Prescale128_c

/*
 * NAME: see below
 * DESCRIPTION: Bits in the FTMx timer status and control registers (FTMxSC)
 * VALID VALUES: refer to Kinetis manual
 */
#define gFTMxSC_TOF_c           0x80    					/* Timer overflow flag. (RO) */
#define gFTMxSC_TOIE_c          0x40    					/* Timer overflow interrupt enable. (RW) */
#define gFTMxSC_CPWMS_c         0x20    					/* Center-aliged PWM select. (RW) */
#define gFTMxSC_CLKSB_c         0x10    					/* Clock source, high bit. (RW) */
#define gFTMxSC_CLKSA_c         0x08    					/* Clock source, low bit. (RW) */
#define gFTMxSC_PS2_c           0x04    					/* Prescale divisor select, high bit. (RW) */
#define gFTMxSC_PS1_c           0x02    					/* Prescale divisor select, middle bit. (RW) */
#define gFTMxSC_PS0_c           0x01    					/* Prescale divisor select, low bit. (RW) */

/*
 * NAME: see below
 * DESCRIPTION: Bits in the FTMx channel N status and control registers (FTMxCnSC)
 * VALID VALUES: refer to Kinetis manual
 */
#define gFTMxCnSC_CHF_c	        0x80    					/* Channel Overflow flag. */
#define gFTMxCnSC_CHIE_c        0x40    					/* Channel Interrupt enable. */        
#define gFTMxCnSC_MSB_c         0x20    					/* Channel Mode select bit B. */
#define gFTMxCnSC_MSA_c         0x10    					/* Channel Mode select bit A. */
#define gFTMxCnSC_ELSB_c        0x08    					/* Channel Edge/level select high bit. */
#define gFTMxCnSC_ELSA_c        0x04    					/* Channel Edge/level select low bit. */
#define gFTMxCnSC_DMA_c         0x01    					/* DMA transfer enable bit */

/*
 * NAME: gFTMxSC_ClockSource_c
 * DESCRIPTION: Clock source for TMR module
 * VALID VALUES: refer to Kinetis manual
 */
#define gFTMxSC_ClockSource_c	gFTMxSC_CLKSA_c 			/* Select the system clock. */

/*
 * NAME: gFTMxSC_Stop_c
 * DESCRIPTION: FTMx_SC register value for disabling (stopping) the timer
 * VALID VALUES: refer to Kinetis manual
 */
#define gFTMxSC_Stop_c          0x00

/*
 * NAME: FTMReadCnVRegister()
 * DESCRIPTION: Macro used to read the compare registers 
 */
#define FTMReadCnVRegister(variable)	(variable) = gFTMxCnV_c;

/*
 * NAME: FTMReadCNTRegister()
 * DESCRIPTION: Macro used to read the free running counter
 */
#define FTMReadCNTRegister(variable)   	(variable) = gFTMxCNT;

/*
 * NAME: FTMReadCNTRegister()
 * DESCRIPTION: Enable the hardware timer
 */
#define FTMStartTimerHardware()		(gFTMxSC_c |= (gFTMxSC_ClockSource_c | gFTMxSC_PrescaleCount_c))

/*
 * NAME: FTMReadCNTRegister()
 * DESCRIPTION: Disable the hardware timer
 */
#define FTMStopTimerHardware()		(gFTMxSC_c = gFTMxSC_Stop_c)

/*
 * LPTMR
 */
#if gTMR_EnableHWLowPowerTimers_d
/*
 * NAME: LPTMRReadCompareRegister()
 * DESCRIPTION: Macro used to read the compare register 
 */
#define LPTMRReadCompareRegister(variable)	(variable) = LPTMR0_CMR;

/*
 * NAME: LPTMRReadCounterRegister()
 * DESCRIPTION: Macro used to read the free running counter
 */
#define LPTMRReadCounterRegister(variable)   	{LPTMR0_CNR = LPTMR0_CNR; (variable) = LPTMR0_CNR;} /* first dummy write to latch register */

/*
 * NAME: FTMReadCNTRegister()
 * DESCRIPTION: Enable the hardware timer
 */
#define LPTMRStartTimerHardware()    LPTMR0_CSR=(LPTMR_CSR_TCF_MASK | LPTMR_CSR_TIE_MASK | LPTMR_CSR_TFC_MASK);\
                                     LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;

/*
 * NAME: FTMReadCNTRegister()
 * DESCRIPTION: Disable the hardware timer
 */
#define LPTMRStopTimerHardware()     LPTMR0_CSR &= ~(LPTMR_CSR_TEN_MASK);
#endif
   
/*****************************************************************************
******************************************************************************
* Private type definitions
******************************************************************************
*****************************************************************************/

/*
 * Type name: tmrTimerStatus_t
 * Type description: The status and type are bitfields, to save RAM.
 *                   This costs some code space, though.
 * Members: N/A
 */
typedef uint8_t tmrTimerStatus_t;

/*
 * NAME: tmrStatus_t
 * DESCRIPTION: timer status - see the status macros.
 *              If none of these flags are on, the timer is not allocated.
 *              For allocated timers, exactly one of these flags will be set.
 *              mTmrStatusActive_c - Timer has been started and has not yet expired.
 *              mTmrStatusReady_c - TMR_StartTimer() has been called for this timer, but
 *                                  the timer task has not yet actually started it. The
 *                                  timer is considered to be active.
 *              mTmrStatusInactive_c Timer is allocated, but is not active.
 */
typedef uint8_t tmrStatus_t;



/*
 * Type name: tmrTimerTableEntry_tag
 * Type description: One entry in the main timer table.
 * Members: intervalInTicks - The timer's original duration, in ticks.
 *                            Used to reset intervnal timers.
 *
 *          countDown - When a timer is started, this is set to the duration.
 *                      The timer task decrements this value. When it reaches
 *                      zero, the timer has expired.
 *          pfCallBack - Pointer to the callback function
 */
typedef struct tmrTimerTableEntry_tag {
  tmrTimerTicks32_t intervalInTicks;
  tmrTimerTicks32_t remainingTicks;
  tmrTimerTicks32_t timestamp;
  pfTmrCallBack_t pfCallBack;
} tmrTimerTableEntry_t;

#if gTMR_EnableHWLowPowerTimers_d
/*
 * NAME: tmrHwSourceType_t
 * DESCRIPTION: Hardware timer type used by the software timers
 */
typedef enum  
{
	tmrFTMbased_c = 0,
	tmrLPTMRbased_c
} tmrHwSourceType_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __TIMER_H__ */

 /*****************************************************************************
 *                               <<< EOF >>>                                  *
 ******************************************************************************/
