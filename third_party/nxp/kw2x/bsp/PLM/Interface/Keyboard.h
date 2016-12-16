/**************************************************************************
* Filename: Keyboard.h
*
* Description: Keyboard interface file for ARM CORTEX-M4 processor
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
#ifndef _KEYBOARD_INTERFACE_H_
#define _KEYBOARD_INTERFACE_H_

#include "AppToPlatformConfig.h"
#include "PortConfig.h"
#include "TMR_Interface.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*
 * Name: gKeyBoardSupported_d
 * Description: Enables/disables the switches based keyboard
 */
#ifndef gKeyBoardSupported_d
#define gKeyBoardSupported_d                TRUE
#endif

/*
 * Name: gTsiSupported_d
 * Definition: Enables/disables the Touch Sense Interface (TSI)
 */
#ifndef gTsiSupported_d
#define gTsiSupported_d                     FALSE
#endif

/*
 * Name: gKbdEventPressOnly
 * Description: mapping for keyboard standard operation mode (press only)
 */
#define gKbdEventPressOnly_c                1

/*
 * Name: gKbdEventShortLongPressMode_c
 * Description: mapping for keyboard short/long detection operation mode
 */
#define gKbdEventShortLongPressMode_c       2

/*
 * Name: gKbdEventPressHoldReleaseMode_c
 * Description: mapping for keyboard press/hold/release operation mode
 */
#define gKbdEventPressHoldReleaseMode_c     3

/*
 * Name: gKeyEventNotificationMode_d
 * Description: configure the operation mode and implicitly
 *              the event notification mode
 */
#ifndef gKeyEventNotificationMode_d
#define gKeyEventNotificationMode_d         gKbdEventShortLongPressMode_c
#endif

#if gKeyEventNotificationMode_d > gKbdEventPressHoldReleaseMode_c
#error "Keyboard notification method not supported"
#endif

#if gTsiSupported_d
/*
 * Name: gTSI_DBOUNCE_COUNTS_c
 * Description: Number of scans needed for a touch to remain high to be considered valid
 */
#define gTSI_DBOUNCE_COUNTS_c               0x00000010

/*
 * Name: gTSICalibrationTime
 * Description: TSI calibration time (milliseconds)
 */
#define gTSICalibrationTime                 50

/*
 * Name: gTSI_IRQ_Priority_c
 * Description: TSI Interrupt Request priority
 */
#define gTSI_IRQ_Priority_c                 4

#endif /* gTsiSupported_d */


#if (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c)
#if gTsiSupported_d
/*
 * Name: gTsiLongKeyIterations_c
 * Description: The iterations required for key long press detection
 * Notes:       A key is considered to be long pressed if the key is held down
 *              for at least gLongKeyIterations_c iterations.
 *              The value has to be chosen depending on TSI module configuration.
 *              Please refer to TSI chapter within the processor reference manual
 *              for more details.
 *              Due to the fact that the key state machine is implemented within
 *              the TSI interrupt service routine, all the timings are related
 *              to TSI IRQ frequency. For the default TSI configuration, the IRQ
 *              frequency is ~540 Hz. Therefore, the TSI IRQ idle period is
 *              ~1.85 milliseconds. If an electrode is touched, the TSI IRQ period
 *              is around 2.3 milliseconds. This last value shall be used in
 *              further calculations, as time base unit.
 */
#ifndef gTsiLongKeyIterations_c
#define gTsiLongKeyIterations_c             434 /* 434 * 2.3 milliseconds ~= 1 second */
#endif
#endif /* gTsiSupported_d */

#if gKeyBoardSupported_d
/*
 * Name: gKbdLongKeyIterations_c
 * Description: The iterations required for key long press detection
 *              The detection threshold is gKbdLongKeyIterations_c x gKeyScanInterval_c milliseconds
 */
#ifndef gKbdLongKeyIterations_c
#define gKbdLongKeyIterations_c             20
#endif
#endif /* gKeyBoardSupported_d */

#elif (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c)
#if gTsiSupported_d
/*
 * Name: gFirstHoldDetectIterations_c
 * Description: The iterations required for key hold detection
 */
#ifndef gTsiFirstHoldDetectIterations_c
#define gTsiFirstHoldDetectIterations_c     652 /* 1.5 seconds */
#endif

/*
 * Name: gHoldDetectIterations_c
 * Description: The iterations required for key hold detection (repetitive generation of event)
 *              May be the same value as  gFirstHoldDetectIterations_c
 */
#ifndef gTsiHoldDetectIterations_c
#define gTsiHoldDetectIterations_c          652 /* 1.5 seconds */
#endif
#endif /* gTsiSupported_d */

#if gKeyBoardSupported_d
/*
 * Name: gKbdFirstHoldDetectIterations_c
 * Description: The iterations required for key hold detection
 */
#ifndef gKbdFirstHoldDetectIterations_c
#define gKbdFirstHoldDetectIterations_c     20 /* 1 second, if gKeyScanInterval_c = 50ms */
#endif

/*
 * Name: gKbdHoldDetectIterations_c
 * Description: The iterations required for key hold detection (repetitive generation of event)
 *              May be the same value as  gKbdFirstHoldDetectIterations_c
 */
#ifndef gKbdHoldDetectIterations_c
#define gKbdHoldDetectIterations_c          6 /* 1 second, if gKeyScanInterval_c = 50ms */
#endif
#endif /* gKeyBoardSupported_d */
#endif /*gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c */

#if gKeyBoardSupported_d
/*
 * Name: gKeyScanInterval_c
 * Description: Constant for a key press. A short key will be returned after this
 * number of millisecond if pressed make sure this constant is long enough for debounce time
 */
#ifndef gKeyScanInterval_c
#define gKeyScanInterval_c                  50 /* default is 50 milliseconds */
#endif

/*
 * Name: gKeyBitOffset
 * Description: keyboard events bit position offset. When TSI is also enabled, the event bits
 *              in the event mask is as follows:
 *              -------------------------------------------------
 *              [Bit7][Bit6][Bit5][Bit4][Bit3][Bit2][Bit1][Bit0]
 *              -------------------------------------------------
 *              [SW_3][SW_2][SW_1][SW_0][EL_3][EL_2][EL_1][EL_0]
 *              -------------------------------------------------
 *              where, SW stands for SWITCH and EL stands for
 *              ELECTRODE
 *
 */
#if gTsiSupported_d && (gKeyEventNotificationMode_d == gKbdEventPressOnly_c)
#define gKeyBitOffset                       4
#endif /* gTsiSupported_d */
#endif /* gKeyBoardSupported_d */

/******************************************************************************
 ******************************************************************************
 * Public type definitions
 ******************************************************************************
 ******************************************************************************/
/*
 * Name: KBDFunction_t
 * Description: callback function type definition
 */
typedef void (*KBDFunction_t) ( uint8_t events );

/*
 * Name: key_event_t
 * Description: Each key delivered to the callback function is of this type (see the following enumerations)
 */
typedef uint8_t key_event_t;

/*
 * Description: which key code is given to the callback function
 */
enum
{
    gKBD_EventPB1_c = 1,         /* Pushbutton 1 */
    gKBD_EventPB2_c,             /* Pushbutton 2 */
    gKBD_EventPB3_c,             /* Pushbutton 3 */
    gKBD_EventPB4_c,             /* Pushbutton 4 */
    gKBD_EventTSI1_c,            /* TSI Electrode 1 */
    gKBD_EventTSI2_c,            /* TSI Electrode 2 */
    gKBD_EventTSI3_c,            /* TSI Electrode 3 */
    gKBD_EventTSI4_c,            /* TSI Electrode 4 */
    gKBD_EventLongPB1_c,         /* Pushbutton 1 */
    gKBD_EventLongPB2_c,         /* Pushbutton 2 */
    gKBD_EventLongPB3_c,         /* Pushbutton 3 */
    gKBD_EventLongPB4_c,         /* Pushbutton 4 */
    gKBD_EventLongTSI1_c,        /* TSI Electrode 1 */
    gKBD_EventLongTSI2_c,        /* TSI Electrode 2 */
    gKBD_EventLongTSI3_c,        /* TSI Electrode 3 */
    gKBD_EventLongTSI4_c         /* TSI Electrode 4 */
};

/*
 * Description: which key code is given to the callback function
 */
enum
{
    gKBD_EventPressPB1_c = 1,
    gKBD_EventPressPB2_c,
    gKBD_EventPressPB3_c,
    gKBD_EventPressPB4_c,
    gKBD_EventPressTSI1_c,
    gKBD_EventPressTSI2_c,
    gKBD_EventPressTSI3_c,
    gKBD_EventPressTSI4_c,
    gKBD_EventHoldPB1_c,
    gKBD_EventHoldPB2_c,
    gKBD_EventHoldPB3_c,
    gKBD_EventHoldPB4_c,
    gKBD_EventHoldTSI1_c,
    gKBD_EventHoldTSI2_c,
    gKBD_EventHoldTSI3_c,
    gKBD_EventHoldTSI4_c,
    gKBD_EventReleasePB1_c,
    gKBD_EventReleasePB2_c,
    gKBD_EventReleasePB3_c,
    gKBD_EventReleasePB4_c,
    gKBD_EventReleaseTSI1_c,
    gKBD_EventReleaseTSI2_c,
    gKBD_EventReleaseTSI3_c,
    gKBD_EventReleaseTSI4_c
};

/*
 * Mapping macros needed by applications
 */
#if (gKeyBoardSupported_d || gTsiSupported_d)
  #if (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c)

    #if gTsiSupported_d /* TSI supported */
      #define gKBD_EventSW1_c                     gKBD_EventTSI1_c
      #define gKBD_EventLongSW1_c                 gKBD_EventLongTSI1_c
      #define gKBD_EventSW2_c                     gKBD_EventTSI2_c
      #define gKBD_EventLongSW2_c                 gKBD_EventLongTSI2_c
      #define gKBD_EventSW3_c                     gKBD_EventTSI3_c
      #define gKBD_EventLongSW3_c                 gKBD_EventLongTSI3_c
      #define gKBD_EventSW4_c                     gKBD_EventTSI4_c
      #define gKBD_EventLongSW4_c                 gKBD_EventLongTSI4_c
    #endif /* TSI supported */

    #if gKeyBoardSupported_d /* KBD supported */
    #if gTsiSupported_d /* TSI supported also */
      #define gKBD_EventSW5_c                     gKBD_EventPB1_c
      #define gKBD_EventLongSW5_c                 gKBD_EventLongPB1_c
      #define gKBD_EventSW6_c                     gKBD_EventPB2_c
      #define gKBD_EventLongSW6_c                 gKBD_EventLongPB2_c
      #define gKBD_EventSW7_c                     gKBD_EventPB3_c
      #define gKBD_EventLongSW7_c                 gKBD_EventLongPB3_c
      #define gKBD_EventSW8_c                     gKBD_EventPB4_c
      #define gKBD_EventLongSW8_c                 gKBD_EventLongPB4_c
    #else /* TSI not supported, only KBD */
      #define gKBD_EventSW1_c                     gKBD_EventPB1_c
      #define gKBD_EventLongSW1_c                 gKBD_EventLongPB1_c
      #define gKBD_EventSW2_c                     gKBD_EventPB2_c
      #define gKBD_EventLongSW2_c                 gKBD_EventLongPB2_c
      #define gKBD_EventSW3_c                     gKBD_EventPB3_c
      #define gKBD_EventLongSW3_c                 gKBD_EventLongPB3_c
      #define gKBD_EventSW4_c                     gKBD_EventPB4_c
      #define gKBD_EventLongSW4_c                 gKBD_EventLongPB4_c
    #endif /* gTsiSupported_d */
    #endif /* gKeyBoardSupported_d */

  #elif (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c)

    #if gTsiSupported_d /* TSI supported */
      #define gKBD_EventPressSW1_c                gKBD_EventPressTSI1_c
      #define gKBD_EventHoldSW1_c                 gKBD_EventHoldTSI1_c
      #define gKBD_EventReleaseSW1_c              gKBD_EventReleaseTSI1_c
      #define gKBD_EventPressSW2_c                gKBD_EventPressTSI2_c
      #define gKBD_EventHoldSW2_c                 gKBD_EventHoldTSI2_c
      #define gKBD_EventReleaseSW2_c              gKBD_EventReleaseTSI2_c
      #define gKBD_EventPressSW3_c                gKBD_EventPressTSI3_c
      #define gKBD_EventHoldSW3_c                 gKBD_EventHoldTSI3_c
      #define gKBD_EventReleaseSW3_c              gKBD_EventReleaseTSI3_c
      #define gKBD_EventPressSW4_c                gKBD_EventPressTSI4_c
      #define gKBD_EventHoldSW4_c                 gKBD_EventHoldTSI4_c
      #define gKBD_EventReleaseSW4_c              gKBD_EventReleaseTSI4_c
    #endif /* gTsiSupported_d */

    #if gKeyBoardSupported_d /* KBD supported */
    #if gTsiSupported_d /* TSI supported also */
      #define gKBD_EventPressSW5_c                gKBD_EventPressPB1_c
      #define gKBD_EventHoldSW5_c                 gKBD_EventHoldPB1_c
      #define gKBD_EventReleaseSW5_c              gKBD_EventReleasePB1_c
      #define gKBD_EventPressSW6_c                gKBD_EventPressPB2_c
      #define gKBD_EventHoldSW6_c                 gKBD_EventHoldPB2_c
      #define gKBD_EventReleaseSW6_c              gKBD_EventReleasePB2_c
      #define gKBD_EventPressSW7_c                gKBD_EventPressPB3_c
      #define gKBD_EventHoldSW7_c                 gKBD_EventHoldPB3_c
      #define gKBD_EventReleaseSW7_c              gKBD_EventReleasePB3_c
      #define gKBD_EventPressSW8_c                gKBD_EventPressPB4_c
      #define gKBD_EventHoldSW8_c                 gKBD_EventHoldPB4_c
      #define gKBD_EventReleaseSW8_c              gKBD_EventReleasePB4_c
    #else /* TSI not supported, only KBD */
      #define gKBD_EventPressSW1_c                gKBD_EventPressPB1_c
      #define gKBD_EventHoldSW1_c                 gKBD_EventHoldPB1_c
      #define gKBD_EventReleaseSW1_c              gKBD_EventReleasePB1_c
      #define gKBD_EventPressSW2_c                gKBD_EventPressPB2_c
      #define gKBD_EventHoldSW2_c                 gKBD_EventHoldPB2_c
      #define gKBD_EventReleaseSW2_c              gKBD_EventReleasePB2_c
      #define gKBD_EventPressSW3_c                gKBD_EventPressPB3_c
      #define gKBD_EventHoldSW3_c                 gKBD_EventHoldPB3_c
      #define gKBD_EventReleaseSW3_c              gKBD_EventReleasePB3_c
      #define gKBD_EventPressSW4_c                gKBD_EventPressPB4_c
      #define gKBD_EventHoldSW4_c                 gKBD_EventHoldPB4_c
      #define gKBD_EventReleaseSW4_c              gKBD_EventReleasePB4_c
    #endif /* gTsiSupported_d */
    #endif /* gKeyBoardSupported_d */

  #endif /* gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c */

#else /* neither KBD nor TSI are enabled */

  #if (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c)
    #define gKBD_EventSW1_c                     gKBD_EventPB1_c
    #define gKBD_EventLongSW1_c                 gKBD_EventLongPB1_c
    #define gKBD_EventSW2_c                     gKBD_EventPB2_c
    #define gKBD_EventLongSW2_c                 gKBD_EventLongPB2_c
    #define gKBD_EventSW3_c                     gKBD_EventPB3_c
    #define gKBD_EventLongSW3_c                 gKBD_EventLongPB3_c
    #define gKBD_EventSW4_c                     gKBD_EventPB4_c
    #define gKBD_EventLongSW4_c                 gKBD_EventLongPB4_c
  #elif (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c)
    #define gKBD_EventPressSW1_c                gKBD_EventPressPB1_c
    #define gKBD_EventHoldSW1_c                 gKBD_EventHoldPB1_c
    #define gKBD_EventReleaseSW1_c              gKBD_EventReleasePB1_c
    #define gKBD_EventPressSW2_c                gKBD_EventPressPB2_c
    #define gKBD_EventHoldSW2_c                 gKBD_EventHoldPB2_c
    #define gKBD_EventReleaseSW2_c              gKBD_EventReleasePB2_c
    #define gKBD_EventPressSW3_c                gKBD_EventPressPB3_c
    #define gKBD_EventHoldSW3_c                 gKBD_EventHoldPB3_c
    #define gKBD_EventReleaseSW3_c              gKBD_EventReleasePB3_c
    #define gKBD_EventPressSW4_c                gKBD_EventPressPB4_c
    #define gKBD_EventHoldSW4_c                 gKBD_EventHoldPB4_c
    #define gKBD_EventReleaseSW4_c              gKBD_EventReleasePB4_c
  #endif

#endif /* gKeyBoardSupported_d || gTsiSupported_d */

/*****************************************************************************
 ******************************************************************************
 * Public prototypes
 ******************************************************************************
 *****************************************************************************/

#if gKeyBoardSupported_d || gTsiSupported_d

/******************************************************************************
 * Name: KBD_Init
 * Description: Initializes the TSI hardware module and keyboard module internal
 *              variables
 * Parameter(s): [IN] pfCallBackAdr - pointer to application callback function
 * Return: -
 ******************************************************************************/
extern void KBD_Init
(
        KBDFunction_t pfCallBackAdr
);

#if gTsiSupported_d
/******************************************************************************
 * Name: TSI_ModuleEnableLowPowerWakeup
 * Description: prepare TSI for low power operation
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
extern void TSI_ModuleEnableLowPowerWakeup
(
  void
);

/******************************************************************************
 * Name: TSI_ModuleDisableLowPowerWakeup
 * Description: prepare TSI for normal operation
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
extern void TSI_ModuleDisableLowPowerWakeup
(
  void
);

/******************************************************************************
 * Name: TSI_IsWakeUpSource
 * Description: prepare TSI for normal operation
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
extern bool_t TSI_IsWakeUpSource
(
  void
);

/******************************************************************************
 * Name: TSI_ISR
 * Description: TSI interrupt subroutine
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
extern void TSI_ISR
(
        void
);

#endif /* gTsiSupported_d */

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
extern void Switch_Press_ISR
(
        void
);
#endif

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
);
#endif

#endif /* gKeyBoardSupported_d || gTsiSupported_d */

#if !gKeyBoardSupported_d && !gTsiSupported_d
/* stub functions if disabled */
#define KBD_Init(pfCallBackAdr)
#define Switch_Press_ISR       VECT_DefaultISR
#define TSI_ISR                VECT_DefaultISR
#define TSI_ModuleEnableLowPowerWakeup
#define TSI_ModuleDisableLowPowerWakeup
#define TSI_IsWakeUpSource FALSE
#define KBD_IsWakeUpSource FALSE
#endif /* !gKeyBoardSupported_d && !gTsiSupported_d */

#ifdef __cplusplus
}
#endif

#endif /* _KEYBOARD_INTERFACE_H_ */
