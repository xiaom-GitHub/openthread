/******************************************************************************
* Filename: Keyboard.c
*
* Description: Keyboard implementation file for ARM CORTEX-M4 processor.
*              Uses TSI driver to read touch pad presses.
*              The keyboard handling logic can understand one or more keys 
*              pressed simultaneous.                               
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
#include "Keyboard.h"
#include "TS_Interface.h"
#include "NVIC.h"
#include "GPIO_Interface.h"
#include "FunctionLib.h"

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/*
* Hardware settings
*/
#if gKeyBoardSupported_d

    #define mSWITCH1_PORT_TARGET_c          SWITCH1_PORT
    #define mSWITCH2_PORT_TARGET_c          SWITCH2_PORT
    #define mSWITCH3_PORT_TARGET_c          SWITCH3_PORT
    #define mSWITCH4_PORT_TARGET_c          SWITCH4_PORT

    #ifdef mSWITCH1_MASK
      #define mSWITCH1_TARGET_c             mSWITCH1_MASK
    #else
      #define mSWITCH1_TARGET_c             0
    #endif
    #ifdef mSWITCH2_MASK
      #define mSWITCH2_TARGET_c             mSWITCH2_MASK
    #else
      #define mSWITCH2_TARGET_c             0
    #endif
    #ifdef mSWITCH3_MASK
      #define mSWITCH3_TARGET_c             mSWITCH3_MASK
    #else
      #define mSWITCH3_TARGET_c             0
    #endif
    #ifdef mSWITCH4_MASK
      #define mSWITCH4_TARGET_c             mSWITCH4_MASK
    #else
      #define mSWITCH4_TARGET_c             0
    #endif

    /* Define PCB switch mapping */
    #define mSWITCH1_PORT_c                 mSWITCH1_PORT_TARGET_c
    #define mSWITCH2_PORT_c                 mSWITCH2_PORT_TARGET_c
    #define mSWITCH3_PORT_c                 mSWITCH3_PORT_TARGET_c
    #define mSWITCH4_PORT_c                 mSWITCH4_PORT_TARGET_c

    /* gSWITCHx_MASK_c is used for detecting which key is pressed */
    #define gSWITCH1_MASK_c                 mSWITCH1_TARGET_c
    #define gSWITCH2_MASK_c                 mSWITCH2_TARGET_c
    #define gSWITCH3_MASK_c                 mSWITCH3_TARGET_c
    #define gSWITCH4_MASK_c                 mSWITCH4_TARGET_c
#else
    #define SwitchPortGet                   0
    #define gSWITCH1_MASK_c                 0
    #define gSWITCH2_MASK_c                 0
    #define gSWITCH3_MASK_c                 0
    #define gSWITCH4_MASK_c                 0
#endif /* gKeyBoardSupported_d */

#if gKeyBoardSupported_d
	#ifdef gSWITCH1_MASK_c
	#define gSWITCH1_IRQ_NUM_c                  SWITCH1_IRQ_NUM
	#define gSWITCH1_PORT_CLK_GATING_REG_c      SWITCH1_PORT_CLK_GATING_REG
	#define gSWITCH1_PORT_CLK_GATING_MASK_c     SWITCH1_PORT_CLK_GATING_MASK
	#define gSWITCH1_PDDR_c                     SWITCH1_PDDR
	#define gSWITCH1_PCR_REG_c                  SWITCH1_PCR_REG
	#endif
	#ifdef gSWITCH2_MASK_c
	#define gSWITCH2_IRQ_NUM_c                  SWITCH2_IRQ_NUM
	#define gSWITCH2_PORT_CLK_GATING_REG_c      SWITCH2_PORT_CLK_GATING_REG
	#define gSWITCH2_PORT_CLK_GATING_MASK_c     SWITCH2_PORT_CLK_GATING_MASK
	#define gSWITCH2_PDDR_c                     SWITCH2_PDDR
	#define gSWITCH2_PCR_REG_c                  SWITCH2_PCR_REG
	#endif
	#ifdef gSWITCH3_MASK_c
	#define gSWITCH3_IRQ_NUM_c                  SWITCH3_IRQ_NUM
	#define gSWITCH3_PORT_CLK_GATING_REG_c      SWITCH3_PORT_CLK_GATING_REG
	#define gSWITCH3_PORT_CLK_GATING_MASK_c     SWITCH3_PORT_CLK_GATING_MASK
	#define gSWITCH3_PDDR_c                     SWITCH3_PDDR
	#define gSWITCH3_PCR_REG_c                  SWITCH3_PCR_REG
	#endif
	#ifdef gSWITCH4_MASK_c
	#define gSWITCH4_IRQ_NUM_c                  SWITCH4_IRQ_NUM
	#define gSWITCH4_PORT_CLK_GATING_REG_c      SWITCH4_PORT_CLK_GATING_REG
	#define gSWITCH4_PORT_CLK_GATING_MASK_c     SWITCH4_PORT_CLK_GATING_MASK
	#define gSWITCH4_PDDR_c                     SWITCH4_PDDR
	#define gSWITCH4_PCR_REG_c                  SWITCH4_PCR_REG
	#endif
	#define gSWITCH_IRQ_PRIO_c   9
	#define gSWITCH_IRQ_ON_FALLING_EDGE         0x0A
#endif /* gKeyBoardSupported_d */

#if gTsiSupported_d
#if ((MCU_MK60D10 == 1) || (MCU_MK60N512VMD100 == 1))
    /* TSI clock gating register */
    #define gTSI_ClockGating_Reg_c          SIM_SCGC5
    #define gTSI_ClockGating_Bit_c          SIM_SCGC5_TSI_SHIFT    
    /* TSI IRQ number */
    #define gTSI_IRQ_NUM_c                  83    
#elif (MCU_MK20D5 == 1)
  /* TSI clock gating register */
    #define gTSI_ClockGating_Reg_c          SIM_SCGC5
    #define gTSI_ClockGating_Bit_c          SIM_SCGC5_TSI_SHIFT
    /* TSI IRQ number */
    #define gTSI_IRQ_NUM_c                  37
#endif

#if gTSI_ElectrodesCnt_c > 0
#define gTSI_E1_ClockGating_Reg_c           TSI_E1_CLK_GATING_REG
#define gTSI_E1_ClockGating_Bit_c           TSI_E1_CLK_GATING_BIT
#endif
#if gTSI_ElectrodesCnt_c > 1
#define gTSI_E2_ClockGating_Reg_c           TSI_E2_CLK_GATING_REG
#define gTSI_E2_ClockGating_Bit_c           TSI_E2_CLK_GATING_BIT
#endif
#if gTSI_ElectrodesCnt_c > 2
#define gTSI_E3_ClockGating_Reg_c           TSI_E3_CLK_GATING_REG
#define gTSI_E3_ClockGating_Bit_c           TSI_E3_CLK_GATING_BIT
#endif
#if gTSI_ElectrodesCnt_c > 3
#define gTSI_E4_ClockGating_Reg_c           TSI_E4_CLK_GATING_REG
#define gTSI_E4_ClockGating_Bit_c           TSI_E4_CLK_GATING_BIT
#endif
#if gTSI_ElectrodesCnt_c > 0
#define gTSI_E1_PinCtrl_Reg_c               TSI_E1_PCR_REG
#endif
#if gTSI_ElectrodesCnt_c > 1
#define gTSI_E2_PinCtrl_Reg_c               TSI_E2_PCR_REG
#endif
#if gTSI_ElectrodesCnt_c > 2
#define gTSI_E3_PinCtrl_Reg_c               TSI_E3_PCR_REG
#endif
#if gTSI_ElectrodesCnt_c > 3
#define gTSI_E4_PinCtrl_Reg_c               TSI_E4_PCR_REG
#endif
#endif /* gTsiSupported_d */

#if gKeyBoardSupported_d
/*
 * Name: mNoKey_c
 * Description: no key macro definition
 */
#define mNoKey_c 0xff
#endif /* gKeyBoardSupported_d */

#if gTsiSupported_d
/*
 * Electrodes internal mappings 
 */
#define gTSI_ELECTRODE0_c  0
#define gTSI_ELECTRODE1_c  1
#define gTSI_ELECTRODE2_c  2
#define gTSI_ELECTRODE3_c  3

/*
 * Electrodes touch thresholds
 */
#define gTSI_ELECTRODE0_TOUCH_c  0x250
#define gTSI_ELECTRODE1_TOUCH_c  0x250
#define gTSI_ELECTRODE2_TOUCH_c  0x250
#define gTSI_ELECTRODE3_TOUCH_c  0x250

/*
 * Electrodes overrun thresholds
 */
#if (defined(MCU_MK60N512VMD100) || defined(MCU_MK20D5))
  #define gTSI_ELECTRODE0_OVRRUN_c  0x800
  #define gTSI_ELECTRODE1_OVRRUN_c  0x800
  #define gTSI_ELECTRODE2_OVRRUN_c  0x800
  #define gTSI_ELECTRODE3_OVRRUN_c  0x800
#elif defined(MCU_MK60D10)
  #define gTSI_ELECTRODE_OVRRUN_c   0x2000
#endif
#endif /* gTsiSupported_d */

#if gTsiSupported_d

/*
 * Name: gTSI_ELECTRODE_ENABLE_REG
 * Description: TSI Pin Enable Register mapping macro
 */
#define gTSI_ELECTRODE_ENABLE_REG           TSI0_PEN

/*
 * Name: gTSI_GENCS_REG
 * Description: TSI General Control and Status Register mapping macro
 */
#define gTSI_GENCS_REG                      TSI0_GENCS

/*
 * Name: gTSI_SCAN_CTRL_REG
 * Description: TSI Scan Control Register mapping macro
 */
#define gTSI_SCAN_CTRL_REG                  TSI0_SCANC

/*
 * Name: gTSI_STATUS_REG
 * Description: TSI Status Register mapping macro
 */
#if (MCU_MK60N512VMD100 == 1)
#define gTSI_STATUS_REG                     TSI0_STATUS
#endif

/*
 * Name: gTSI_SW_TRIGGER_START_MASK_c
 * Description: TSI Software Trigger Start mask
 */
#define gTSI_SW_TRIGGER_START_MASK_c        TSI_GENCS_SWTS_MASK

/*
 * Name: gTSI_EOS_FLAG_MASK_c
 * Description: TSI End Of Scan mask
 */
#define gTSI_EOS_FLAG_MASK_c                TSI_GENCS_EOSF_MASK

/*
 * Name: gTSI_ENABLE_MASK_c
 * Description: TSI Enable mask
 */
#define gTSI_ENABLE_MASK_c                  TSI_GENCS_TSIEN_MASK

/*
 * Name: gTSI_OUT_OF_RANGE_MASK_c
 * Description: TSI Out Of Range mask
 */
#define gTSI_OUT_OF_RANGE_MASK_c            TSI_GENCS_OUTRGF_MASK

/*
 * Name: gTSI_SCAN_TRIGGER_MODE_MASK_c
 * Description: TSI Scan Trigger Mode mask
 */
#define gTSI_SCAN_TRIGGER_MODE_MASK_c       TSI_GENCS_STM_MASK

/*
 * Name: gTSI_INTERRUPT_EN_MASK_c
 * Description: TSI Interrupt Enable mask
 */
#define gTSI_INTERRUPT_EN_MASK_c            TSI_GENCS_TSIIE_MASK

/*
 * Name: TSI_GENCS_ESOR_MASK
 * Description: TSI End-of-Scan /  Out-Of-Range interrupt select mask
 */
#define gTSI_EOS_OR_MASK_c                  TSI_GENCS_ESOR_MASK

/*
 * Name: TSI_StartScaning
 * Description: macro used to start a TSI scan
 */
#define TSI_StartScaning()                  gTSI_GENCS_REG |= gTSI_SCAN_TRIGGER_MODE_MASK_c

/*
 * Name: TSI_EnableEOSInterrupt
 * Description: macro used to enable End-Of-Scan (EOS) interrupt
 */
#define TSI_EnableEOSInterrupt()            gTSI_GENCS_REG |= (gTSI_INTERRUPT_EN_MASK_c | gTSI_EOS_OR_MASK_c)

/*
 * Name: TSI_EnableModule
 * Description: macro used to enable the TSI module
 */
#define TSI_EnableModule()                  gTSI_GENCS_REG |= gTSI_ENABLE_MASK_c

/*
 * Name: TSI_DisableModule
 * Description: macro used to disable the TSI module
 */
#define TSI_DisableModule()                 gTSI_GENCS_REG &= ~gTSI_ENABLE_MASK_c

/*
 * Name: gTSI_KeyTouched_c
 * Description: convenience mapping macro for key touched
 */
#define gTSI_KeyTouched_c                   TRUE

/*
 * Name: gTSI_KeyUntouched_c
 * Description: convenience mapping macro for key release or untouched
 */
#define gTSI_KeyUntouched_c                 FALSE

#endif /* gTsiSupported_d */

/* Configuration check */

#if (gTsiSupported_d == TRUE) && (gTSI_ElectrodesCnt_c > 4)
#error "Cannot support more than 4 electrodes"
#endif

#if (gKeyBoardSupported_d == TRUE) && (gKBD_KeysCount_c > 4)
#error "Cannot support more than 4 switches"
#endif

#if (gTsiSupported_d == TRUE) && (gTSI_ElectrodesCnt_c == 0)
#warning "TSI module is enabled but the electrodes count is ZERO"
#endif

#if (gKeyBoardSupported_d == TRUE) && (gKBD_KeysCount_c == 0)
#warning "KEYBOARD module is enabled but the pushbuttons count is ZERO"
#endif

#if (gKeyBoardSupported_d == TRUE) && (gTMR_Enabled_d == FALSE)
#warning "Keyboard scan cannot operate without the TIMER platform component"
#endif


/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/
#if gKeyBoardSupported_d || gTsiSupported_d
/* 
 * Name: KeyState_t
 * Description: enumerated data type for key states
 */
#if (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c)

typedef enum tag_KeyState{
  mStateKeyIdle,        /* coming in for first time */
  mStateKeyDetected,    /* got a key, waiting to see if it's a long key */
  mStateKeyWaitRelease  /* got the long key, waiting for the release to go back to idle */
}KeyState_t;

#elif (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c)

typedef enum tag_KeyState {
  mStateKeyDebounce,
  mStateKeyHoldDetection,
  mStateKeyHoldGen,
} KeyState_t;

#endif /* gKeyEventNotificationMode_d */
#endif /* gKeyBoardSupported_d || gTsiSupported_d */

#if gKeyBoardSupported_d
/*
 * Name:
 * Description: switches scan result definition 
 */
typedef uint32_t    switchScan_t;
#endif /* gKeyBoardSupported_d */


/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

#if gTsiSupported_d
/******************************************************************************
 * Name: TSI_DelayMs
 * Description: time delay using LPTMR module (blocking method)
 * Parameter(s): [IN] count_val - delay value expressed in milliseconds
 * Return: -
 ******************************************************************************/
static void TSI_DelayMs
(
        uint16_t count_val
);

/******************************************************************************
 * Name: TSI_SelfCalibration
 * Description: Simple auto calibration version, only sums a prefixed amount 
 *              to the current baseline
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
static void TSI_SelfCalibration
(
        void
);

/******************************************************************************
 * Name: TsiElectrodesScan
 * Description: process the key presses/releases using a simple state machine
 * Parameter(s): [IN] key - the key to be processed
 *               [IN] state - TRUE if key is pressed / FALSE if key is released
 *                            or not touched (pressed)  
 * Return: -
 ******************************************************************************/
static void TsiElectrodesScan
(
        uint8_t     key,
        bool_t      state
);

/******************************************************************************
 * Name: TSI_Task
 * Description: TSI and KBD task
 * Parameter(s): [IN] events - events to be processed 
 * Return: -
 ******************************************************************************/
static void TSI_Task
(
        event_t events
);
#endif /* gTsiSupported_d */

#if gKeyBoardSupported_d
/******************************************************************************
 * Name: KbGpioInit
 * Description: Initialize the GPIOs used by the keyboard (switches)
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
static void KbGpioInit
(
    void
);

#if gTMR_Enabled_d

/******************************************************************************
 * Name: KBD_KeySwitchPortGet
 * Description: Gets the switch port
 * Parameter(s): -
 * Return: switch port value (pressed / not pressed keys)
 ******************************************************************************/
static switchScan_t KBD_KeySwitchPortGet
(
        void
);

/******************************************************************************
 * Name: KBD_KeyCheck
 * Description: Called to check if a key is still pressed
 * Parameter(s): [IN] previousPressed - previously pressed key
 * Return: TRUE if the key passed as argument is still pressed, FALSE otherwise
 ******************************************************************************/
#if ((gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c) ||  \
    (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c))
static bool_t KBD_KeyCheck
(
    switchScan_t previousPressed
);
#endif /* #if ((gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c) ||    \
    (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c)) */

/******************************************************************************
 * Name: KeyScan
 * Description: scan the keyboard switches and detects key press/hold/release 
 *              or short/long press
 * Parameter(s): [IN]timerId - key scan timer ID
 * Return: -
 ******************************************************************************/
static void KeyScan
(
    uint8_t timerId
);
#endif /* #if gTMR_Enabled_d */
#endif /* gKeyBoardSupported_d */

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

#if gTsiSupported_d || gKeyBoardSupported_d

#if gTsiSupported_d
/*
 * Name: mElectrodeTouch
 * Description: Electrodes touch thresholds values
 */
static uint16_t  mElectrodeTouch[gTSI_ElectrodesCnt_c] = {0};

/*
 * Name: mElectrodeBaseline
 * Description: Electrodes baselines values
 */
static uint16_t  mElectrodeBaseline[gTSI_ElectrodesCnt_c] = {0};

/*
 * Name: mDebounceCounter
 * Description: electrodes debounce counters values
 */
static uint32_t  mDebounceCounter[gTSI_ElectrodesCnt_c] = {gTSI_DBOUNCE_COUNTS_c};

/*
 * Name: mTsiTaskId
 * Description: TSI task ID
 */
static tsTaskID_t   mTsiTaskId;
#endif /* gTsiSupported_d */

#if (gKeyEventNotificationMode_d == gKbdEventPressOnly_c)
/*
 * Name: mKeyPressed
 * Description: Keys pressed mask; each bit represents a key / electrode; 
 *             (i.e. bit0 -> KEY0, bit1 -> KEY1, etc)  
 */
static volatile uint16_t mKeyPressed;
#endif /* gKeyEventNotificationMode_d == gKbdEventPressOnly_c */

#if (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c)
/*
 * Name: mLongTouchCount
 * Description: array of counters used for key long press detection
 */
#if gTsiSupported_d
static volatile uint16_t mLongTouchCount[gTSI_ElectrodesCnt_c];
#endif /* gTsiSupported_d */
#endif /* gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c */

#if (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c)
#if gTsiSupported_d
/*
 * Name: mTsiHoldDectionKeyCount
 * Description: array of counters used for key hold detection
 */
static uint16_t       mTsiHoldDectionKeyCount[gTSI_ElectrodesCnt_c];

/*
 * Name: mTsiHoldGenKeyCount
 * Description: array of counters used for key hold generation
 */
static uint16_t       mTsiHoldGenKeyCount[gTSI_ElectrodesCnt_c];
#endif /* gTsiSupported_d */

#if gKeyBoardSupported_d
#if gTMR_Enabled_d
/*
 * Name: mKbdHoldDectionKeyCount
 * Description: counter used for key hold detection
 */
static uint16_t       mKbdHoldDectionKeyCount;

/*
 * Name: mKbdHoldGenKeyCount
 * Description: counter used for key hold generation
 */
static uint16_t       mKbdHoldGenKeyCount;
#endif /* #if gTMR_Enabled_d */
#endif /* gKeyBoardSupported_d */
#endif /* gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c */

#if ( (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c) || (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c) ) 
#if gTsiSupported_d
/*
 * Name: mTsiElectrodeState
 * Description: keyboard keys state (see definition of KeyState_t)
 */
static KeyState_t mTsiElectrodeState[gTSI_ElectrodesCnt_c];
#endif /* gTsiSupported_d */

#if gKeyBoardSupported_d
#if gTMR_Enabled_d
/*
 * Name: mSwitch_SCAN
 * Description: switch scan result
 */
static uint32_t mSwitch_SCAN;

#if (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c)
/*
 * Name: mKeyState
 * Description: key scan state variable used in the scanning state machine
 */
static uint8_t mKeyState = mStateKeyIdle;

/*
 * Name: mLongKeyCount
 * Description: self explanatory
 */
static uint8_t mLongKeyCount;
#elif (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c)
/*
 * Name: mKeyState
 * Description: key scan state variable used in the scanning state machine
 */
static uint8_t mKeyState = mStateKeyDebounce;

#endif /* gKeyEventNotificationMode_d */
#endif /* #if gTMR_Enabled_d */
#endif /* gKeyBoardSupported_d */
#endif /* (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c) || (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c) */

/*
 * Name: pressedKey
 * Description: pressed key number
 */
#if gKeyBoardSupported_d
#if gTMR_Enabled_d
static uint8_t pressedKey;
#endif
#endif

/*
 * Name: mKeyScanTimerID
 * Description: timer ID used for key scanning
 */
tmrTimerID_t mKeyScanTimerID = gTmrInvalidTimerID_c;

/*
 * Name: mpfKeyFunction
 * Description: pointer to the application callback function
 */
static KBDFunction_t mpfKeyFunction = NULL;

#endif /* gTsiSupported_d || gKeyBoardSupported_d */

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

#if gTsiSupported_d
/******************************************************************************
 * Name: TSI_DelayMs
 * Description: time delay using LPTMR module (blocking method)
 * Parameter(s): [IN] count_val - delay value expressed in milliseconds
 * Return: -
 ******************************************************************************/
static void TSI_DelayMs
(
        uint16_t count_val
)
{
    /* Turn on clock to LPTMR module */
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;
    /* Set compare value */
    LPTMR0_CMR = count_val;
    /* Use 1Khz LPO clock and bypass prescaler */
    LPTMR0_PSR = LPTMR_PSR_PCS(1)|LPTMR_PSR_PBYP_MASK;
    /* Start counting */
    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;

    /* Wait for counter to reach compare value */
    while (!(LPTMR0_CSR & LPTMR_CSR_TCF_MASK)) {}

    /* Clear Timer Compare Flag */
    LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;
    
    return;
}

/******************************************************************************
 * Name: TSI_Task
 * Description: TSI task, simply calls the application callback
 * Parameter(s): [IN] events - events to be processed 
 * Return: -
 ******************************************************************************/
static void TSI_Task
(
        event_t events
)
{       
    if(NULL != mpfKeyFunction)
    {
        mpfKeyFunction(events);
    }
}

/******************************************************************************
 * Name: TSI_SelfCalibration
 * Description: Simple auto calibration version, only sums a prefixed amount 
 *              to the current baseline
 * Parameter(s): 
 * Return: 
 ******************************************************************************/
static void TSI_SelfCalibration(void)
{
    /* start a scan sequence */
    gTSI_GENCS_REG |= gTSI_SW_TRIGGER_START_MASK_c;

    /* wait until scan is complete */
    while(!(gTSI_GENCS_REG & gTSI_EOS_FLAG_MASK_c)){};
    
    /* wait the calibration time (using LP timer) */
    TSI_DelayMs(gTSICalibrationTime);

    /* calibrate TSI module */
#if defined(MCU_MK60N512VMD100) || defined(MCU_MK20D5)
#if gTSI_ElectrodesCnt_c > 0
    mElectrodeBaseline[gTSI_ELECTRODE0_c] = gTSI_ELECTRODE0_COUNT_REG;
    gTSI_ELECTRODE0_OVERRUN = (uint32_t)((mElectrodeBaseline[gTSI_ELECTRODE0_c] + gTSI_ELECTRODE0_OVRRUN_c));
    mElectrodeTouch[gTSI_ELECTRODE0_c] = mElectrodeBaseline[gTSI_ELECTRODE0_c] + gTSI_ELECTRODE0_TOUCH_c;
#endif
#if gTSI_ElectrodesCnt_c > 1    
    mElectrodeBaseline[gTSI_ELECTRODE1_c] = gTSI_ELECTRODE1_COUNT_REG;
    gTSI_ELECTRODE1_OVERRUN = (uint32_t)((mElectrodeBaseline[gTSI_ELECTRODE1_c] + gTSI_ELECTRODE1_OVRRUN_c));
    mElectrodeTouch[gTSI_ELECTRODE1_c] = mElectrodeBaseline[gTSI_ELECTRODE1_c] + gTSI_ELECTRODE1_TOUCH_c;
#endif    
#if gTSI_ElectrodesCnt_c > 2
    mElectrodeBaseline[gTSI_ELECTRODE2_c] = gTSI_ELECTRODE2_COUNT_REG;
    gTSI_ELECTRODE2_OVERRUN = (uint32_t)((mElectrodeBaseline[gTSI_ELECTRODE2_c] + gTSI_ELECTRODE2_OVRRUN_c));
    mElectrodeTouch[gTSI_ELECTRODE2_c] = mElectrodeBaseline[gTSI_ELECTRODE2_c] + gTSI_ELECTRODE2_TOUCH_c;
#endif
#if gTSI_ElectrodesCnt_c > 3
    mElectrodeBaseline[gTSI_ELECTRODE3_c] = gTSI_ELECTRODE3_COUNT_REG;
    gTSI_ELECTRODE3_OVERRUN = (uint32_t)((mElectrodeBaseline[gTSI_ELECTRODE3_c] + gTSI_ELECTRODE3_OVRRUN_c));
    mElectrodeTouch[gTSI_ELECTRODE3_c] = mElectrodeBaseline[gTSI_ELECTRODE3_c] + gTSI_ELECTRODE3_TOUCH_c;
#endif
#elif defined(MCU_MK60D10)
#if gTSI_ElectrodesCnt_c > 0    
    mElectrodeBaseline[gTSI_ELECTRODE0_c] = gTSI_ELECTRODE0_COUNT_REG;
    mElectrodeTouch[gTSI_ELECTRODE0_c] = mElectrodeBaseline[gTSI_ELECTRODE0_c] + gTSI_ELECTRODE0_TOUCH_c;
    gTSI_ELECTRODE0_OVERRUN = (uint32_t)((mElectrodeBaseline[gTSI_ELECTRODE0_c] + gTSI_ELECTRODE_OVRRUN_c));
#endif
#if gTSI_ElectrodesCnt_c > 1    
    mElectrodeBaseline[gTSI_ELECTRODE1_c] = gTSI_ELECTRODE1_COUNT_REG;
    mElectrodeTouch[gTSI_ELECTRODE1_c] = mElectrodeBaseline[gTSI_ELECTRODE1_c] + gTSI_ELECTRODE1_TOUCH_c;
#endif
#if gTSI_ElectrodesCnt_c > 2    
    mElectrodeBaseline[gTSI_ELECTRODE2_c] = gTSI_ELECTRODE2_COUNT_REG;
    mElectrodeTouch[gTSI_ELECTRODE2_c] = mElectrodeBaseline[gTSI_ELECTRODE2_c] + gTSI_ELECTRODE2_TOUCH_c;
#endif
#if gTSI_ElectrodesCnt_c > 3
    mElectrodeBaseline[gTSI_ELECTRODE3_c] = gTSI_ELECTRODE3_COUNT_REG;
    mElectrodeTouch[gTSI_ELECTRODE3_c] = mElectrodeBaseline[gTSI_ELECTRODE3_c] + gTSI_ELECTRODE3_TOUCH_c;
#endif

#endif

    /* Disable TSI module */
    TSI_DisableModule();

    TSI_StartScaning();
    TSI_EnableEOSInterrupt();
    TSI_EnableModule();
}

/******************************************************************************
 * Name: TsiElectrodesScan
 * Description: process the key presses/releases using a simple state machine
 * Parameter(s): [IN] key - the key to be processed
 *               [IN] state - TRUE if key is pressed / FALSE if key is released  
 * Return: -
 ******************************************************************************/
static void TsiElectrodesScan
(
        uint8_t     key,
        bool_t      state
)
{
    
#if (gKeyEventNotificationMode_d == gKbdEventPressOnly_c)
    if(state) /* key touched */
    {
        /* decrement debounce counter */
        mDebounceCounter[key]--; 
        
        if(!mDebounceCounter[key]) /* counter expires => key is pressed */
        {
            /* mark the corresponding bit */
            mKeyPressed |= (1<<key);  
        }
    }
    else  /* key not touched or released */
    {
        /* reload debounce counter with default value */
        mDebounceCounter[key] = gTSI_DBOUNCE_COUNTS_c;
    }
    
#elif (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c)
    
    if(state) /* key touched */
    {
        switch(mTsiElectrodeState[key])       
        {        
        case mStateKeyIdle:
            
            /* decrement debounce counter */
            mDebounceCounter[key]--;
                        
            if(!mDebounceCounter[key]) /* counter expires => key is pressed */
            {               
                /* reset long key detection counter */
                mLongTouchCount[key] = 0;
                /* change key state */
                mTsiElectrodeState[key] = mStateKeyDetected;    
            }
            break;

        case mStateKeyDetected:
            /* increment long press detection counter */
            mLongTouchCount[key]++;
            
            if(mLongTouchCount[key] >= gTsiLongKeyIterations_c) /* counter expires => key long pressed detected */
            {
                /* inform TSI task */
                TS_SendEvent(mTsiTaskId, gKBD_EventLongTSI1_c + key);    
                /* change key state */
                mTsiElectrodeState[key] = mStateKeyWaitRelease;                 
            }
            break;

        case mStateKeyWaitRelease:          
            break;

        default:
            break;
        }
    }
    else
    {
        if(mTsiElectrodeState[key] == mStateKeyDetected)
        {
            /* inform TSI task */
            TS_SendEvent(mTsiTaskId, gKBD_EventTSI1_c + key);    
        }
        /* change key state to Idle state */
        mTsiElectrodeState[key] = mStateKeyIdle;
        /* reload debounce counter with default value */
        mDebounceCounter[key] = gTSI_DBOUNCE_COUNTS_c;
    }
            
#elif (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c)
    if(state)
    {
        switch(mTsiElectrodeState[key])
        {       
        case mStateKeyDebounce:
            /* decrement debounce counter */
            mDebounceCounter[key]--;

            if(!mDebounceCounter[key]) /* counter expires => key is pressed */
            {       
                /* inform the TSI task */
                TS_SendEvent(mTsiTaskId, gKBD_EventPressTSI1_c + key);
                /* reset key hold detection counter */
                mTsiHoldDectionKeyCount[key] = 0; 
                /* change key state */
                mTsiElectrodeState[key] = mStateKeyHoldDetection;   
            }
            break;

        case mStateKeyHoldDetection:
            /* increment key hold counter */
            mTsiHoldDectionKeyCount[key]++;
            
            if (mTsiHoldDectionKeyCount[key] >= gTsiFirstHoldDetectIterations_c) /* counter expires => key is hold */
            {
                /* inform TSI task */
                TS_SendEvent(mTsiTaskId, gKBD_EventHoldTSI1_c + key);
                /* reset key hold generation counter */
                mTsiHoldGenKeyCount[key] = 0;
                /* change key state */
                mTsiElectrodeState[key] = mStateKeyHoldGen;
            }           
            break;

        case mStateKeyHoldGen:
            /* increment key hold generation count */
            mTsiHoldGenKeyCount[key]++;
            
            if(mTsiHoldGenKeyCount[key] >= gTsiHoldDetectIterations_c) /* counter expires => key is still hold */ 
            {
                /* reset key hold generation counter, to generate repetitive hold events */
                mTsiHoldGenKeyCount[key] = 0;
                /* inform TSI task */
                TS_SendEvent(mTsiTaskId, gKBD_EventHoldTSI1_c + key);
            }
            break;

        default:
            break;
        }   
    }
    else 
    {
        if( (mTsiElectrodeState[key] == mStateKeyHoldDetection) || (mTsiElectrodeState[key] == mStateKeyHoldGen ) )
        {
            /* inform TSI task */
            TS_SendEvent(mTsiTaskId, gKBD_EventReleaseTSI1_c + key);     
        }
        /* change key state */
        mTsiElectrodeState[key] = mStateKeyDebounce;
        /* reload debounce counter with default value */
        mDebounceCounter[key] = gTSI_DBOUNCE_COUNTS_c;
    }
#endif
}
#endif /* gTsiSupported_d */

#if gKeyBoardSupported_d
/******************************************************************************
 * Name: KbGpioInit
 * Description: Initialize the GPIOs used by the keyboard (switches)
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
static void KbGpioInit
(
    void
)
{
#if (gTarget_UserDefined_d == 0)  // no user defined target
#if gSWITCH1_MASK_c  
  GpioPinConfigAttr_t Sw1PinConfig;
#endif  
#if gSWITCH2_MASK_c  
  GpioPinConfigAttr_t Sw2PinConfig;
#endif
#if gSWITCH3_MASK_c  
  GpioPinConfigAttr_t Sw3PinConfig;
#endif
#if gSWITCH4_MASK_c  
  GpioPinConfigAttr_t Sw4PinConfig;
#endif  
#endif /* #if (gTarget_UserDefined_d == 0) */  
  
#if gSWITCH1_MASK_c
#if (gTarget_UserDefined_d == 0)
    /* enable clock gating for the switch port */    
    GPIO_EnablePortClockGating((GpioPortID_t)SW1_Port_c);
    /* configure the pin as input */
    GPIO_SetPinDir((GpioPortID_t)SW1_Port_c, (GpioPin_t)SW1_Pin_c, gGpioDirIn_c);
    /* configure the pin as digital I/O, enable pin interrupt on falling edge, pull-up enable */    
    FLib_MemSet(&Sw1PinConfig, 0, sizeof(Sw1PinConfig));
    Sw1PinConfig.muxCtrl = gGpioAlternate1_c;
    Sw1PinConfig.intCtrl = gGPIOIrqOnFallingEdge_c;
    Sw1PinConfig.pullEnable = TRUE;
    Sw1PinConfig.pullSelect = gGPIOInternalPullup_c;
    GPIO_SetPinPortAttr((GpioPortID_t)SW1_Port_c, (GpioPin_t)SW1_Pin_c, &Sw1PinConfig);   
#endif    
    /* enable port interrupt enable */
    NVIC_EnableIRQ(gSWITCH1_IRQ_NUM_c);
    /* set interrupt priority */
    NVIC_SetPriority(gSWITCH1_IRQ_NUM_c, gSWITCH_IRQ_PRIO_c);
#endif
    
#if gSWITCH2_MASK_c
#if (gTarget_UserDefined_d == 0)    
    /* enable clock gating for the switch port */    
    GPIO_EnablePortClockGating((GpioPortID_t)SW2_Port_c);
    /* configure the pin as input */    
    GPIO_SetPinDir((GpioPortID_t)SW2_Port_c, (GpioPin_t)SW2_Pin_c, gGpioDirIn_c);
    /* configure the pin as digital I/O, enable pin interrupt on falling edge, pull-up enable */
    FLib_MemSet(&Sw2PinConfig, 0, sizeof(Sw2PinConfig));
    Sw2PinConfig.muxCtrl = gGpioAlternate1_c;
    Sw2PinConfig.intCtrl = gGPIOIrqOnFallingEdge_c;
    Sw2PinConfig.pullEnable = TRUE;
    Sw2PinConfig.pullSelect = gGPIOInternalPullup_c;
    GPIO_SetPinPortAttr((GpioPortID_t)SW2_Port_c, (GpioPin_t)SW2_Pin_c, &Sw2PinConfig);    
#endif    
    /* enable port interrupt enable */
    NVIC_EnableIRQ(gSWITCH2_IRQ_NUM_c);
    /* set interrupt priority */
    NVIC_SetPriority(gSWITCH2_IRQ_NUM_c, gSWITCH_IRQ_PRIO_c);
#endif
    
#if gSWITCH3_MASK_c
#if (gTarget_UserDefined_d == 0)    
    /* enable clock gating for the switch port */    
    GPIO_EnablePortClockGating((GpioPortID_t)SW3_Port_c);
    /* configure the pin as input */    
    GPIO_SetPinDir((GpioPortID_t)SW3_Port_c, (GpioPin_t)SW3_Pin_c, gGpioDirIn_c);
    /* configure the pin as digital I/O, enable pin interrupt on falling edge, pull-up enable */
    FLib_MemSet(&Sw3PinConfig, 0, sizeof(Sw3PinConfig));
    Sw3PinConfig.muxCtrl = gGpioAlternate1_c;
    Sw3PinConfig.intCtrl = gGPIOIrqOnFallingEdge_c;
    Sw3PinConfig.pullEnable = TRUE;
    Sw3PinConfig.pullSelect = gGPIOInternalPullup_c;
    GPIO_SetPinPortAttr((GpioPortID_t)SW3_Port_c, (GpioPin_t)SW3_Pin_c, &Sw3PinConfig);    
#endif    
    /* enable port interrupt enable */
    NVIC_EnableIRQ(gSWITCH3_IRQ_NUM_c);
    /* set interrupt priority */
    NVIC_SetPriority(gSWITCH3_IRQ_NUM_c, gSWITCH_IRQ_PRIO_c);
#endif
    
#if gSWITCH4_MASK_c
#if (gTarget_UserDefined_d == 0)
    /* enable clock gating for the switch port */    
    GPIO_EnablePortClockGating((GpioPortID_t)SW4_Port_c);
    /* configure the pin as input */    
    GPIO_SetPinDir((GpioPortID_t)SW4_Port_c, (GpioPin_t)SW4_Pin_c, gGpioDirIn_c);
    /* configure the pin as digital I/O, enable pin interrupt on falling edge, pull-up enable */
    FLib_MemSet(&Sw4PinConfig, 0, sizeof(Sw4PinConfig));
    Sw4PinConfig.muxCtrl = gGpioAlternate1_c;
    Sw4PinConfig.intCtrl = gGPIOIrqOnFallingEdge_c;
    Sw4PinConfig.pullEnable = TRUE;
    Sw4PinConfig.pullSelect = gGPIOInternalPullup_c;
    GPIO_SetPinPortAttr((GpioPortID_t)SW4_Port_c, (GpioPin_t)SW4_Pin_c, &Sw4PinConfig);    
#endif    
    /* enable port interrupt enable */
    NVIC_EnableIRQ(gSWITCH4_IRQ_NUM_c);
    /* set interrupt priority */
    NVIC_SetPriority(gSWITCH4_IRQ_NUM_c, gSWITCH_IRQ_PRIO_c);
#endif  
}

#if gTMR_Enabled_d

/******************************************************************************
 * Name: KBD_KeySwitchPortGet
 * Description: Gets the switch port
 * Parameter(s): -
 * Return: switch port value (pressed / not pressed keys)
 ******************************************************************************/
static switchScan_t KBD_KeySwitchPortGet
(
        void
)
{
    uint32_t portScan;
    pressedKey = mNoKey_c;    
    
    /* IAR EW fix @ Warning[Pa082]: undefined behavior: the order of volatile accesses is undefined in this statement */
#if gSWITCH1_MASK_c
    portScan = ((mSWITCH1_PORT_c & gSWITCH1_MASK_c) ^ gSWITCH1_MASK_c);
#endif
#if gSWITCH2_MASK_c
    portScan |= ((mSWITCH2_PORT_c & gSWITCH2_MASK_c) ^ gSWITCH2_MASK_c);
#endif
#if gSWITCH3_MASK_c
    portScan |= ((mSWITCH3_PORT_c & gSWITCH3_MASK_c) ^ gSWITCH3_MASK_c);
#endif
#if gSWITCH4_MASK_c
    portScan |= ((mSWITCH4_PORT_c & gSWITCH4_MASK_c) ^ gSWITCH4_MASK_c);
#endif

#if gSWITCH1_MASK_c 
    if(portScan & gSWITCH1_MASK_c)
        pressedKey = 0;
#if (gSWITCH2_MASK_c | gSWITCH3_MASK_c | gSWITCH4_MASK_c)
    else
#endif
#endif 

#if gSWITCH2_MASK_c
        if(portScan & gSWITCH2_MASK_c)
            pressedKey = 1;
#if(gSWITCH3_MASK_c | gSWITCH4_MASK_c)
        else
#endif 
#endif 

#if gSWITCH3_MASK_c
            if(portScan & gSWITCH3_MASK_c)
                pressedKey = 2;
#if gSWITCH4_MASK_c
            else
#endif
#endif 

#if gSWITCH4_MASK_c 
                if(portScan & gSWITCH4_MASK_c)
                    pressedKey = 3;
#endif
    
    return portScan;
}

/******************************************************************************
 * Name: KBD_KeyCheck
 * Description: Called to check if a key is still pressed
 * Parameter(s): [IN] previousPressed - previously pressed key
 * Return: TRUE if the key passed as argument is still pressed, FALSE otherwise
 ******************************************************************************/
#if ((gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c) ||  \
    (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c))
static bool_t KBD_KeyCheck
(
    switchScan_t previousPressed
)
{
    bool_t pressed = FALSE;

    uint32_t portScan;
    uint8_t key = mNoKey_c;
    
    /* IAR EW fix @ Warning[Pa082]: undefined behavior: the order of volatile accesses is undefined in this statement */
#if gSWITCH1_MASK_c
    portScan = ((mSWITCH1_PORT_c & gSWITCH1_MASK_c) ^ gSWITCH1_MASK_c);
#endif
#if gSWITCH2_MASK_c
    portScan |= ((mSWITCH2_PORT_c & gSWITCH2_MASK_c) ^ gSWITCH2_MASK_c);
#endif
#if gSWITCH3_MASK_c
    portScan |= ((mSWITCH3_PORT_c & gSWITCH3_MASK_c) ^ gSWITCH3_MASK_c);
#endif
#if gSWITCH4_MASK_c
    portScan |= ((mSWITCH4_PORT_c & gSWITCH4_MASK_c) ^ gSWITCH4_MASK_c);
#endif

    (void)previousPressed; /* avoid compiler warnings */
    
#if gSWITCH1_MASK_c
    if(portScan & gSWITCH1_MASK_c)
        key = 0;
#if(gSWITCH2_MASK_c | gSWITCH3_MASK_c | gSWITCH4_MASK_c) 
    else 
#endif
#endif 

#if gSWITCH2_MASK_c
        if(portScan & gSWITCH2_MASK_c)
            key = 1;
#if(gSWITCH3_MASK_c | gSWITCH4_MASK_c)
        else
#endif
#endif 

#if gSWITCH3_MASK_c
            if(portScan & gSWITCH3_MASK_c)
                key = 2;
#if gSWITCH4_MASK_c
            else
#endif
#endif  

#if gSWITCH4_MASK_c
                if(portScan & gSWITCH4_MASK_c)
                    key = 3;
#endif 
    /* Check if the switch is still pressed */
    if(pressedKey == key)
    {
        pressed = TRUE;
    }

    return pressed;
}
#endif /*#if ((gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c) || \
    (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c))*/


/******************************************************************************
 * Name: KeyScan
 * Description: scan the keyboard switches and detects key press/hold/release 
 *              or short/long press
 * Parameter(s): [IN]timerId - key scan timer ID
 * Return: -
 ******************************************************************************/
#if (gKeyEventNotificationMode_d == gKbdEventPressOnly_c)
static void KeyScan
(
    uint8_t timerId
)
{   
    if(KBD_KeySwitchPortGet())
    {
        TMR_StopTimer(timerId);
#if gTsiSupported_d
        mpfKeyFunction(1<<(pressedKey + gKeyBitOffset)); /* bits 0..3 are for TSI, bits 4..7 for keyboard */
#else
        mpfKeyFunction(1<<pressedKey); /* bits 0..3 are for keyboard */
#endif
    }   
}

#elif (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c)
static void KeyScan
(
        uint8_t timerId
)
{
    uint8_t keyBase;
    uint32_t portScan;

    switch(mKeyState) 
    {

    /* got a fresh key */
    case mStateKeyIdle:      
        mSwitch_SCAN = KBD_KeySwitchPortGet();
        if(mSwitch_SCAN != 0)
        {        
            mKeyState = mStateKeyDetected;
            mLongKeyCount = 0;
        }
        else
        {
            TMR_StopTimer(timerId); 
        }
        break;

        /* a key was detected. Has it been released or still being pressed? */
    case mStateKeyDetected:
        keyBase = 0;  /* assume no key */

        /* Check to see if the key is still pressed. Ignore other pressed keys */
        if( KBD_KeyCheck(mSwitch_SCAN) ) 
        {
            mLongKeyCount++;

            if(mLongKeyCount >= gKbdLongKeyIterations_c) 
            {
                keyBase = gKBD_EventLongPB1_c;
            }
        }
        else 
        {     
            /* short key press */
            keyBase = gKBD_EventPB1_c;
        }

        if(keyBase) 
        {       
            /* if a key was pressed, send it */
            if(pressedKey != mNoKey_c)
            {
                mpfKeyFunction(keyBase + pressedKey);
            }

            /* whether we sent a key or not, wait to go back to keyboard  */
            mKeyState = mStateKeyWaitRelease;
        }
        break;

        /* got the long key, waiting for the release to go back to idle */
    case mStateKeyWaitRelease:      
        /* wait for the release before going back to idle */  
        
        /* IAR EW fix @ Warning[Pa082]: undefined behavior: the order of volatile accesses is undefined in this statement */
#if gSWITCH1_MASK_c
        portScan = ((mSWITCH1_PORT_c & gSWITCH1_MASK_c) ^ gSWITCH1_MASK_c);
#endif
#if gSWITCH2_MASK_c
        portScan |= ((mSWITCH2_PORT_c & gSWITCH2_MASK_c) ^ gSWITCH2_MASK_c);
#endif
#if gSWITCH3_MASK_c
        portScan |= ((mSWITCH3_PORT_c & gSWITCH3_MASK_c) ^ gSWITCH3_MASK_c);
#endif
#if gSWITCH4_MASK_c
        portScan |= ((mSWITCH4_PORT_c & gSWITCH4_MASK_c) ^ gSWITCH4_MASK_c);
#endif
        
        if((mSwitch_SCAN == 0) || (portScan == 0)) 
        {
            mKeyState = mStateKeyIdle;
            TMR_StopTimer(timerId);                
        }
        break;
    }
}

#elif (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c)
static void KeyScan
(
        uint8_t timerId
)
{
    uint32_t portScan;
  
    switch(mKeyState) 
    {    
    case mStateKeyDebounce:
        mSwitch_SCAN = KBD_KeySwitchPortGet();
        if(mSwitch_SCAN != 0) 
        {
            mKeyState =  mStateKeyHoldDetection;
            mKbdHoldDectionKeyCount = 0; 
            //Generate press event indication     
            if(pressedKey != mNoKey_c) 
            {
                mpfKeyFunction(gKBD_EventPressPB1_c + pressedKey);           
            }
        } 
        else 
        {
            TMR_StopTimer(timerId);
        }      
        break;
    case mStateKeyHoldDetection:
        if( KBD_KeyCheck(mSwitch_SCAN) ) 
        {
            mKbdHoldDectionKeyCount++;        
            if (mKbdHoldDectionKeyCount >= gKbdFirstHoldDetectIterations_c) 
            {
                //first hold event detected - generate hold event
                if(pressedKey != mNoKey_c) 
                {
                    mpfKeyFunction(gKBD_EventHoldPB1_c + pressedKey);                       
                }                    
                mKbdHoldGenKeyCount = 0;
                mKeyState = mStateKeyHoldGen;
            }
        } 
        else 
        {    
            /* IAR EW fix @ Warning[Pa082]: undefined behavior: the order of volatile accesses is undefined in this statement */
            portScan = ((mSWITCH1_PORT_c & gSWITCH1_MASK_c) ^ gSWITCH1_MASK_c);
            portScan |= ((mSWITCH2_PORT_c & gSWITCH2_MASK_c) ^ gSWITCH2_MASK_c);
            portScan |= ((mSWITCH3_PORT_c & gSWITCH3_MASK_c) ^ gSWITCH3_MASK_c);
            portScan |= ((mSWITCH4_PORT_c & gSWITCH4_MASK_c) ^ gSWITCH4_MASK_c);    
            if((mSwitch_SCAN == 0) || (portScan == 0)) 
            {
                if(pressedKey != mNoKey_c) 
                {
                    mpfKeyFunction(gKBD_EventReleasePB1_c + pressedKey);                       
                    mKeyState = mStateKeyDebounce;
                    TMR_StopTimer(timerId);            
                }                                       
            }                
        }
        break;
    case mStateKeyHoldGen:
        if( KBD_KeyCheck(mSwitch_SCAN) ) 
        {
            mKbdHoldGenKeyCount++;
            if(mKbdHoldGenKeyCount >= gKbdHoldDetectIterations_c) 
            {
                mKbdHoldGenKeyCount = 0;
                if(pressedKey != mNoKey_c) 
                {

                    mpfKeyFunction(gKBD_EventHoldPB1_c + pressedKey);

                }           
            }
        } 
        else 
        {   /* IAR EW fix @ Warning[Pa082]: undefined behavior: the order of volatile accesses is undefined in this statement */
            portScan = ((mSWITCH1_PORT_c & gSWITCH1_MASK_c) ^ gSWITCH1_MASK_c);
            portScan |= ((mSWITCH2_PORT_c & gSWITCH2_MASK_c) ^ gSWITCH2_MASK_c);
            portScan |= ((mSWITCH3_PORT_c & gSWITCH3_MASK_c) ^ gSWITCH3_MASK_c);
            portScan |= ((mSWITCH4_PORT_c & gSWITCH4_MASK_c) ^ gSWITCH4_MASK_c);
            if((mSwitch_SCAN == 0) || (portScan == 0)) 
            {
                if(pressedKey != mNoKey_c) 
                {

                    mpfKeyFunction(gKBD_EventReleasePB1_c + pressedKey);

                    mKeyState = mStateKeyDebounce;
                    TMR_StopTimer(timerId);            
                }                                       
            }                     
        }
        break;
    default:
        break;
    }
}
#endif /* gKeyEventNotificationMode_d */
#endif /* gTMR_Enabled_d */
#endif /* gKeyBoardSupported_d */

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************/

#if (gTsiSupported_d || gKeyBoardSupported_d) 

/******************************************************************************
 * Name: KBD_Init
 * Description: Initializes the TSI hardware module and keyboard module internal
 *              variables 
 * Parameter(s): [IN] pfCallBackAdr - pointer to application callback function
 * Return: -
 * Notes: It the TIMER platform component is enabled, TMR_Init() function MUST
 *        be called before KBD_Init() function
 ******************************************************************************/
void KBD_Init
(
        KBDFunction_t pfCallBackAdr
)
{
#if (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c || (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c))
#if gTsiSupported_d
    uint8_t loopCnt = 0;
#endif
#endif

    /* if no valid pointer provided, return */
    if(NULL == pfCallBackAdr) 
        return;

#if gTsiSupported_d            
    /* create the TSI task */
    mTsiTaskId = TS_CreateTask(gTsiTaskPriority_c, TSI_Task);

    /* return if unable to create the TSI task */
    if(mTsiTaskId == gTsInvalidTaskID_c)
        return;        
#endif

    /* store the pointer to callback function provided by the application */
    mpfKeyFunction = pfCallBackAdr;

    /* initialize counters and keys state variables */

#if gTsiSupported_d

#if (gKeyEventNotificationMode_d == gKbdEventPressOnly_c)   
    mKeyPressed = 0;
#elif (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c)
    for(loopCnt = 0; loopCnt < gTSI_ElectrodesCnt_c; loopCnt++)
    {
        mLongTouchCount[loopCnt] = 0;
        mTsiElectrodeState[loopCnt] = mStateKeyIdle;    
    }
#elif (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c)
    for(loopCnt = 0; loopCnt < gTSI_ElectrodesCnt_c; loopCnt++)
    {
        mTsiElectrodeState[loopCnt] = mStateKeyDebounce;
    }
#endif

    /* Turn on clock to TSI module */
    gTSI_ClockGating_Reg_c |= (1 << gTSI_ClockGating_Bit_c);

    /* Turn on clock gating on TSI electrodes (ports) */
#if gTSI_ElectrodesCnt_c > 0
    gTSI_E1_ClockGating_Reg_c |= gTSI_E1_ClockGating_Bit_c;
#endif
#if gTSI_ElectrodesCnt_c > 1
    gTSI_E2_ClockGating_Reg_c |= gTSI_E2_ClockGating_Bit_c;
#endif
#if gTSI_ElectrodesCnt_c > 2
    gTSI_E3_ClockGating_Reg_c |= gTSI_E3_ClockGating_Bit_c;
#endif
#if gTSI_ElectrodesCnt_c > 3
    gTSI_E4_ClockGating_Reg_c |= gTSI_E4_ClockGating_Bit_c;
#endif

#if gTSI_ElectrodesCnt_c > 0
    gTSI_E1_PinCtrl_Reg_c = PORT_PCR_MUX(0);
#endif
#if gTSI_ElectrodesCnt_c > 1
    gTSI_E2_PinCtrl_Reg_c = PORT_PCR_MUX(0);
#endif
#if gTSI_ElectrodesCnt_c > 2
    gTSI_E3_PinCtrl_Reg_c = PORT_PCR_MUX(0);
#endif
#if gTSI_ElectrodesCnt_c > 3
    gTSI_E4_PinCtrl_Reg_c = PORT_PCR_MUX(0);
#endif

    /* setup the TSI registers */
    gTSI_GENCS_REG |= ((TSI_GENCS_NSCN(10))|(TSI_GENCS_PS(3)));

#ifdef MCU_MK60N512VMD100
    gTSI_SCAN_CTRL_REG |= ((TSI_SCANC_EXTCHRG(3))|(TSI_SCANC_REFCHRG(31))|(TSI_SCANC_DELVOL(7))|(TSI_SCANC_SMOD(0))|(TSI_SCANC_AMPSC(0)));
#elif defined(MCU_MK20D5) || defined(MCU_MK60D10)
    gTSI_SCAN_CTRL_REG = TSI_SCANC_EXTCHRG(1);
    gTSI_SCAN_CTRL_REG |= TSI_SCANC_REFCHRG(15);
    gTSI_SCAN_CTRL_REG |= TSI_SCANC_SMOD(0);
    gTSI_SCAN_CTRL_REG |= TSI_SCANC_AMPSC(0);
#endif
    
    /* enable electrodes in use */
#if gTSI_ElectrodesCnt_c > 0
    gTSI_ELECTRODE_ENABLE_REG = gTSI_ELECTRODE0_EN_MASK_c;
#endif
#if gTSI_ElectrodesCnt_c > 1
    gTSI_ELECTRODE_ENABLE_REG |= gTSI_ELECTRODE1_EN_MASK_c;
#endif
#if gTSI_ElectrodesCnt_c > 2
    gTSI_ELECTRODE_ENABLE_REG |= gTSI_ELECTRODE2_EN_MASK_c;
#endif
#if gTSI_ElectrodesCnt_c > 3
    gTSI_ELECTRODE_ENABLE_REG |= gTSI_ELECTRODE3_EN_MASK_c;
#endif  
    /* enable electrode in use for low power mode*/
    gTSI_ELECTRODE_ENABLE_REG |= TSI_PEN_LPSP(9);

    /* setup the TSI control register for low power mode */
    gTSI_GENCS_REG = ((gTSI_GENCS_REG & ~(TSI_GENCS_LPCLKS_MASK)) |
            TSI_GENCS_LPSCNITV(0) |
            TSI_GENCS_STPE_MASK);

    /* Enable TSI */
    gTSI_GENCS_REG |= (gTSI_ENABLE_MASK_c);  

    /* Enable TSI interrupt within the Nested Vector Interrupt Controller (NVIC)  */
    NVIC_EnableIRQ(gTSI_IRQ_NUM_c);

    /* Set TSI IRQ priority */
    NVIC_SetPriority(gTSI_IRQ_NUM_c, gTSI_IRQ_Priority_c);  

    /* start TSI operation */
    TSI_SelfCalibration();
#endif /* #if gTsiSupported_d */

#if gKeyBoardSupported_d        
#if gTMR_Enabled_d
    /* timer is used to determine short or long key press */
    mKeyScanTimerID = TMR_AllocateTimer();
#endif /* #if gTMR_Enabled_d */    
    /* initialize all the GPIO pins for keyboard */
    KbGpioInit();    
#endif /* gKeyBoardSupported_d */                
}

#if gTsiSupported_d
/******************************************************************************
 * Name: TSI_ModuleEnableLowPowerWakeup
 * Description: prepare TSI for low power operation
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
void TSI_ModuleEnableLowPowerWakeup
(
  void
)
{
  /* disable TSI module */
  TSI_DisableModule(); 
  /* enable out of range interrupt */
  TSI0_GENCS &= ~(TSI_GENCS_ESOR_MASK);
  /* enable TSI module */
  TSI_EnableModule();
}

/******************************************************************************
 * Name: TSI_ModuleDisableLowPowerWakeup
 * Description: prepare TSI for normal operation
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
void TSI_ModuleDisableLowPowerWakeup
(
  void
)
{
  /* disable TSI module */
  TSI_DisableModule();
  /* enable out of range interrupt */
  TSI0_GENCS |= (TSI_GENCS_ESOR_MASK);
  /* enable TSI module */
  TSI_EnableModule();
}

/******************************************************************************
 * Name: TSI_IsWakeUpSource
 * Description: 
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
bool_t TSI_IsWakeUpSource
(
  void
)
{
  bool_t res = FALSE;
  uint32_t irqMaskRegister;
  irqMaskRegister = IntDisableAll();
  if( ( gTSI_GENCS_REG & gTSI_OUT_OF_RANGE_MASK_c ) == gTSI_OUT_OF_RANGE_MASK_c )
  {
    /* clear OUT OF RANGE flag */
    gTSI_GENCS_REG |= gTSI_OUT_OF_RANGE_MASK_c;
    res = TRUE;
  }
  IntRestoreAll(irqMaskRegister);
  return res;
}

/******************************************************************************
 * Name: TSI_ISR
 * Description: TSI interrupt subroutine
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
void TSI_ISR
(
        void
)
{
    /* local counter variable */
    uint16_t u16Counter;
    
    /* clear OUT OF RANGE flag */
    gTSI_GENCS_REG |= gTSI_OUT_OF_RANGE_MASK_c;
    /* clear END OF SCAN flag */
    gTSI_GENCS_REG |= gTSI_EOS_FLAG_MASK_c;
    
#if gTSI_ElectrodesCnt_c > 0            
    /* Process electrode 0 */
    u16Counter = gTSI_ELECTRODE0_COUNT_REG;
    if(u16Counter > mElectrodeTouch[gTSI_ELECTRODE0_c]) /* electrode touched */
    {     
        TsiElectrodesScan(gTSI_ELECTRODE0_c, gTSI_KeyTouched_c);        
    }
    else
    {     
        TsiElectrodesScan(gTSI_ELECTRODE0_c, gTSI_KeyUntouched_c);
    }
#endif
#if gTSI_ElectrodesCnt_c > 1
    /* Process electrode 1 */
    u16Counter = gTSI_ELECTRODE1_COUNT_REG;
    if(u16Counter > mElectrodeTouch[gTSI_ELECTRODE1_c])
    {
        TsiElectrodesScan(gTSI_ELECTRODE1_c, gTSI_KeyTouched_c);        
    }
    else
    {
        TsiElectrodesScan(gTSI_ELECTRODE1_c, gTSI_KeyUntouched_c);
    }
#endif
#if gTSI_ElectrodesCnt_c > 2            
    /* Process electrode 2 */
    u16Counter = gTSI_ELECTRODE2_COUNT_REG;
    if(u16Counter > mElectrodeTouch[gTSI_ELECTRODE2_c])
    {
        TsiElectrodesScan(gTSI_ELECTRODE2_c, gTSI_KeyTouched_c);        
    }
    else
    {
        TsiElectrodesScan(gTSI_ELECTRODE2_c, gTSI_KeyUntouched_c);
    }
#endif
#if gTSI_ElectrodesCnt_c > 3        
    /* Process electrode 3 */
    u16Counter = gTSI_ELECTRODE3_COUNT_REG;
    if(u16Counter > mElectrodeTouch[gTSI_ELECTRODE3_c])
    {
        TsiElectrodesScan(gTSI_ELECTRODE3_c, gTSI_KeyTouched_c);    
    }
    else
    {
        TsiElectrodesScan(gTSI_ELECTRODE3_c, gTSI_KeyUntouched_c);
    }
#endif        
    
#if (gKeyEventNotificationMode_d == gKbdEventPressOnly_c)   
    
      if(mKeyPressed)
      {
          TS_SendEvent(mTsiTaskId, mKeyPressed);
      }
      
#if gTSI_ElectrodesCnt_c > 0      
      if(mKeyPressed & ((1<<gTSI_ELECTRODE0_c))) /* detected a valid touch  */ 
      {    
          mKeyPressed &= ~((1<<gTSI_ELECTRODE0_c)); /* Clear valid touch */
      }
#endif
#if gTSI_ElectrodesCnt_c > 1      
      if(mKeyPressed & ((1<<gTSI_ELECTRODE1_c)))
      {     
          mKeyPressed &= ~((1<<gTSI_ELECTRODE1_c));
      }
#endif
#if gTSI_ElectrodesCnt_c > 2      
      if(mKeyPressed & ((1<<gTSI_ELECTRODE2_c)))
      {     
          mKeyPressed &= ~((1<<gTSI_ELECTRODE2_c));
      }
#endif
#if gTSI_ElectrodesCnt_c > 3      
      if(mKeyPressed & ((1<<gTSI_ELECTRODE3_c)))
      {     
          mKeyPressed &= ~((1<<gTSI_ELECTRODE3_c));
      }
#endif
      
#endif /* gKeyEventNotificationMode_d == gKbdEventPressOnly_c */
        
#ifdef MCU_MK60N512VMD100
      /* clear touch sensing error flags */
      gTSI_STATUS_REG = 0xFFFFFFFF;
#endif /* MCU_MK60N512VMD100 */
      
}
#endif /* gTsiSupported_d */

/******************************************************************************
 * Name: KBD_IsWakeUpSource
 * Description: 
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
#if gKeyBoardSupported_d
bool_t KBD_IsWakeUpSource
(
  void
)
{
    
    bool_t kbi_irq = FALSE;
    
#if gSWITCH1_MASK_c     
    if(gSWITCH1_PCR_REG_c & PORT_PCR_ISF_MASK)
    {
        /* clear the interrupt flag */
        //gSWITCH1_PCR_REG_c |= PORT_PCR_ISF_MASK;
        /* set the local variable to mark that the interrupt is caused by one of the keyboard switches */
        kbi_irq = TRUE;
    }
#endif
    
#if gSWITCH2_MASK_c     
    if(gSWITCH2_PCR_REG_c & PORT_PCR_ISF_MASK)
    {
        /* clear the interrupt flag */
        //gSWITCH2_PCR_REG_c |= PORT_PCR_ISF_MASK;
        /* set the local variable to mark that the interrupt is caused by one of the keyboard switches */
        kbi_irq = TRUE;
    }
#endif
    
#if gSWITCH3_MASK_c     
    if(gSWITCH3_PCR_REG_c & PORT_PCR_ISF_MASK)
    {
        /* clear the interrupt flag */
        //gSWITCH3_PCR_REG_c |= PORT_PCR_ISF_MASK;
        /* set the local variable to mark that the interrupt is caused by one of the keyboard switches */
        kbi_irq = TRUE;
    }
#endif
    
#if gSWITCH4_MASK_c     
    if(gSWITCH4_PCR_REG_c & PORT_PCR_ISF_MASK)
    {
        /* clear the interrupt flag */
        //gSWITCH4_PCR_REG_c |= PORT_PCR_ISF_MASK;
        /* set the local variable to mark that the interrupt is caused by one of the keyboard switches */
        kbi_irq = TRUE;
    }
#endif
    return kbi_irq;
}
#endif /* gKeyBoardSupported_d */


/******************************************************************************
 * Name: Switch_Press_ISR
 * Description: Keyboard (switches) interrupt handler
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
#if gKeyBoardSupported_d
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
void Switch_Press_ISR
(
        void
)
{
    
    bool_t kbi_irq = FALSE;
    
#if gSWITCH1_MASK_c     
    if(gSWITCH1_PCR_REG_c & PORT_PCR_ISF_MASK)
    {
        /* clear the interrupt flag */
        gSWITCH1_PCR_REG_c |= PORT_PCR_ISF_MASK;
        /* set the local variable to mark that the interrupt is caused by one of the keyboard switches */
        kbi_irq = TRUE;
    }
#endif
    
#if gSWITCH2_MASK_c     
    if(gSWITCH2_PCR_REG_c & PORT_PCR_ISF_MASK)
    {
        /* clear the interrupt flag */
        gSWITCH2_PCR_REG_c |= PORT_PCR_ISF_MASK;
        /* set the local variable to mark that the interrupt is caused by one of the keyboard switches */
        kbi_irq = TRUE;
    }
#endif
    
#if gSWITCH3_MASK_c     
    if(gSWITCH3_PCR_REG_c & PORT_PCR_ISF_MASK)
    {
        /* clear the interrupt flag */
        gSWITCH3_PCR_REG_c |= PORT_PCR_ISF_MASK;
        /* set the local variable to mark that the interrupt is caused by one of the keyboard switches */
        kbi_irq = TRUE;
    }
#endif
    
#if gSWITCH4_MASK_c     
    if(gSWITCH4_PCR_REG_c & PORT_PCR_ISF_MASK)
    {
        /* clear the interrupt flag */
        gSWITCH4_PCR_REG_c |= PORT_PCR_ISF_MASK;
        /* set the local variable to mark that the interrupt is caused by one of the keyboard switches */
        kbi_irq = TRUE;
    }
#endif  
    
    if(kbi_irq)
    {
#if gTMR_Enabled_d
        TMR_StartIntervalTimer(mKeyScanTimerID, gKeyScanInterval_c, KeyScan);       
#endif
    }
}
#endif /* gKeyBoardSupported_d */
#endif /* #if gTsiSupported_d || gKeyBoardSupported_d */
