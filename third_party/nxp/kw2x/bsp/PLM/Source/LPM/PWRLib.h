/************************************************************************************
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

#ifndef __PWR_LIB_H__
#define __PWR_LIB_H__

/*****************************************************************************
 *                               INCLUDED HEADERS                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the headers that this module needs to include.    *
 * Note that it is not a good practice to include header files into header   *
 * files, so use this section only if there is no other better solution.     *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

#include "PWR_Interface.h"
 
#ifdef __cplusplus
    extern "C" {
#endif
   
/*****************************************************************************
 *                             PUBLIC MACROS                                *
 *---------------------------------------------------------------------------*
 * Add to this section all the access macros, registers mappings, bit access *
 * macros, masks, flags etc ...
 *---------------------------------------------------------------------------*
 *****************************************************************************/


/*****************************************************************************
 *                             PRIVATE MACROS                                *
 *---------------------------------------------------------------------------*
 * Add to this section all the access macros, registers mappings, bit access *
 * macros, masks, flags etc ...
 *---------------------------------------------------------------------------*
 *****************************************************************************/

#if ((MCU_MK60D10 == 1) || (MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
#define cRAM2_POWERED_ON              SMC_VLLSCTRL_RAM2PO_MASK
#define cRAM2_POWERED_OFF             0
#endif

/* Definitions for LPTMR timer setup */

// LPTMR prescaler clocking options

#define cLPTMR_Source_Int_MCGIRCLK    0
#define cLPTMR_Source_Int_LPO_1KHz    1
#define cLPTMR_Source_Ext_ERCLK32K    2
#define cLPTMR_Source_Ext_OSCERCLK    3

// LPTMR period for LPO 1KHz source clock
#define cLPTMR_PRS_00001ms            ((0<<3)|(1<<2))
#define cLPTMR_PRS_00002ms            (0<<3)
#define cLPTMR_PRS_00004ms            (1<<3)
#define cLPTMR_PRS_00008ms            (2<<3)
#define cLPTMR_PRS_00016ms            (3<<3)
#define cLPTMR_PRS_00032ms            (4<<3)
#define cLPTMR_PRS_00064ms            (5<<3)
#define cLPTMR_PRS_00128ms            (6<<3)
#define cLPTMR_PRS_00256ms            (7<<3)
#define cLPTMR_PRS_00512ms            (8<<3)
#define cLPTMR_PRS_01024ms            (9<<3)
#define cLPTMR_PRS_02048ms            (10<<3)
#define cLPTMR_PRS_04096ms            (11<<3)
#define cLPTMR_PRS_08192ms            (12<<3)
#define cLPTMR_PRS_16384ms            (13<<3)
#define cLPTMR_PRS_32768ms            (14<<3)
#define cLPTMR_PRS_65536ms            (15<<3)

// LPTMR period for LPO 32.768KHz source clock
#define cLPTMR_PRS_125_div_by_4096ms  ((0<<3)|(1<<2))
#define cLPTMR_PRS_125_div_by_2048ms  (0<<3)
#define cLPTMR_PRS_125_div_by_1024ms  (1<<3)
#define cLPTMR_PRS_125_div_by_512ms   (2<<3)
#define cLPTMR_PRS_125_div_by_256ms   (3<<3)
#define cLPTMR_PRS_125_div_by_128ms   (4<<3)
#define cLPTMR_PRS_125_div_by_64ms    (5<<3)
#define cLPTMR_PRS_125_div_by_32ms    (6<<3)
#define cLPTMR_PRS_125_div_by_16ms    (7<<3)
#define cLPTMR_PRS_125_div_by_8ms     (8<<3)
#define cLPTMR_PRS_125_div_by_4ms     (9<<3)
#define cLPTMR_PRS_125_div_by_2ms     (10<<3)
#define cLPTMR_PRS_0125ms             (11<<3)
#define cLPTMR_PRS_0250ms             (12<<3)
#define cLPTMR_PRS_0500ms             (13<<3)
#define cLPTMR_PRS_1000ms             (14<<3)
#define cLPTMR_PRS_2000ms             (15<<3)

#if (MCU_MK60N512VMD100 == 1)
#define RTC_IER                       RTC_CCR
#define RTC_IER_TSIE_MASK             0x10u
#define RTC_IER_TSIE_SHIFT            4
#define RTC_IER_TAIE_MASK             0x4u
#define RTC_IER_TAIE_SHIFT            2
#define RTC_IER_TOIE_MASK             0x2u
#define RTC_IER_TOIE_SHIFT            1
#define RTC_IER_TIIE_MASK             0x1u
#define RTC_IER_TIIE_SHIFT            0
#endif

/* Definitions for RTC timer setup */
#define cRTC_OSC_STARTUP_TIME         1000

/*****************************************************************************
 *                        PRIVATE TYPE DEFINITIONS                           *
 *---------------------------------------------------------------------------*
 * Add to this section all the data types definitions: stuctures, unions,    *
 * enumerations, typedefs ...                                                *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/* 
 * Name: PWRLib_LLWU_WakeupPin_t
 * Description: The possible LLWU wakeup pins
 */
typedef enum
{
  gPWRLib_LLWU_WakeupPin_PTE1_c = 0,
  gPWRLib_LLWU_WakeupPin_PTE2_c,
  gPWRLib_LLWU_WakeupPin_PTE4_c,
  gPWRLib_LLWU_WakeupPin_PTA4_c,
  gPWRLib_LLWU_WakeupPin_PTA13_c,
  gPWRLib_LLWU_WakeupPin_PTB0_c,
  gPWRLib_LLWU_WakeupPin_PTC1_c,
  gPWRLib_LLWU_WakeupPin_PTC3_c,
  gPWRLib_LLWU_WakeupPin_PTC4_c,
  gPWRLib_LLWU_WakeupPin_PTC5_c,
  gPWRLib_LLWU_WakeupPin_PTC6_c,
  gPWRLib_LLWU_WakeupPin_PTC11_c,
  gPWRLib_LLWU_WakeupPin_PTD0_c,
  gPWRLib_LLWU_WakeupPin_PTD2_c,
  gPWRLib_LLWU_WakeupPin_PTD4_c,
  gPWRLib_LLWU_WakeupPin_PTD6_c,
  gPWRLib_LLWU_InvalidWakeupPin_c
} PWRLib_LLWU_WakeupPin_t;

/* 
 * Name: PWRLib_LLWU_WakeupPinConfig_t
 * Description: The possible LLWU wakeup pin configuration value
 */
typedef enum
{
  gPWRLib_LLWU_WakeupPin_Disable = 0,
  gPWRLib_LLWU_WakeupPin_RisingEdge_c,
  gPWRLib_LLWU_WakeupPin_FallingEdge_c,
  gPWRLib_LLWU_WakeupPin_AnyEdge_c,
  gPWRLib_LLWU_WakeupPin_InvalidConfig_c,
} PWRLib_LLWU_WakeupPinConfig_t;

/* 
 * Name: PWRLib_LLWU_WakeupModule_t
 * Description: The possible LLWU wakeup modules
 */
#if ((MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
typedef enum
{
  gPWRLib_LLWU_WakeupModule_LPTMR_c = 0,
  gPWRLib_LLWU_WakeupModule_CMP0_c,
  gPWRLib_LLWU_WakeupModule_CMP1_c,
  gPWRLib_LLWU_WakeupModule_CMP2_c,
  gPWRLib_LLWU_WakeupModule_RTC_Alarm_c = 5,
  gPWRLib_LLWU_WakeupModule_RTC_Second_c = 7,
  gPWRLib_LLWU_InvalidWakeupModule_c
} PWRLib_LLWU_WakeupModule_t;
#elif ((MCU_MK60D10 == 1) || (MCU_MK60N512VMD100 == 1))
typedef enum
{
  gPWRLib_LLWU_WakeupModule_LPTMR_c = 0,
  gPWRLib_LLWU_WakeupModule_CMP0_c,
  gPWRLib_LLWU_WakeupModule_CMP1_c,
  gPWRLib_LLWU_WakeupModule_CMP2_c,
  gPWRLib_LLWU_WakeupModule_TSI_c,
  gPWRLib_LLWU_WakeupModule_RTC_Alarm_c,
  gPWRLib_LLWU_WakeupModule_Reserved_c,
  gPWRLib_LLWU_WakeupModule_ErrorDetect_c,
  gPWRLib_LLWU_InvalidWakeupModule_c
} PWRLib_LLWU_WakeupModule_t;
#endif


/*****************************************************************************
 *                               PUBLIC VARIABLES(External)                  *
 *---------------------------------------------------------------------------*
 * Add to this section all the variables and constants that have global      *
 * (project) scope.                                                          *
 * These variables / constants can be accessed outside this module.          *
 * These variables / constants shall be preceded by the 'extern' keyword in  *
 * the interface header.                                                     *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/* Zigbee stack status */
extern PWRLib_StackPS_t PWRLib_StackPS;

/* For LVD function */
#if ((cPWR_LVD_Enable == 1) || (cPWR_LVD_Enable == 2))
  #if (cPWR_LVD_Enable == 2)
    extern uint16_t                PWRLib_LVD_CollectCounter;
  #endif  // #if (cPWR_LVD_Enable == 2)
  extern PWRLib_LVD_VoltageLevel_t PWRLib_LVD_SavedLevel;
#endif // #if ((cPWR_LVD_Enable == 1) || (cPWR_LVD_Enable == 2))

/*****************************************************************************
 *                            PUBLIC FUNCTIONS                               *
 *---------------------------------------------------------------------------*
 * Add to this section all the global functions prototype preceded (as a     *
 * good practice) by the keyword 'extern'                                    *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*---------------------------------------------------------------------------
 * Name: PWRLib_MCU_Enter_WAIT
 * Description: WAIT mode entry routine. Puts the processor into wait mode.
 *              In this mode the core clock is disabled (no code executing), but 
 *              bus clocks are enabled (peripheral modules are operational). 
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
 void PWRLib_MCU_Enter_WAIT
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_MCUEnter_STOP
 * Description: Puts the processor into normal stop mode.

                Places chip in static state. Lowest power mode that retains all 
                registers while maintaining LVD protection. NVIC is disabled;
                AWIC is used to wake up from interrupt; peripheral clocks are stopped.

                Mode of operation details:
                 - ARM core enters DeepSleep Mode
                 - ARM core is clock gated (HCLK = OFF)
                 - NVIC is disable (FCLK = OFF)
                 - WIC is used to wake up from interruptions
                 - Platform and peripheral clock are stopped
                 - MCG module can be configured to leave reference clocks running
                 - All SRAM is operating (content retained and I/O states held)

                STOP mode is exited into RUN mode using any enabled interrupt or RESET.

 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_MCU_Enter_STOP
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_MCUEnter_VLPS
 * Description: Puts the processor into VLPS (Very Low Power Stop).

                Mode of operation details:
                 - ARM core enters DeepSleep Mode
                 - ARM core is clock gated (HCLK = OFF)
                 - NVIC is disable (FCLK = OFF)
                 - WIC is used to wake up from interruptions
                 - Platform and peripheral clock are stopped
                 - MCG module can be configured to leave reference clocks running
                 - On chip voltage regulator is in a mode that supplies only enough
                   power to run the MCU in a reduced frequency
                 - All SRAM is operating (content retained and I/O states held)

                VLPS mode is exited into RUN mode using any enabled interrupt (with LPWUI =1) or RESET.
                
                The AVLP must be set to 0b1 in MC_PMPROT register in order to allow VPLS mode.

 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_MCU_Enter_VLPS
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_MCUEnter_LLS
 * Description: Puts the processor into LLS (Low Leakage Stop).

                Mode of operation details:
                 - ARM core enters Deep Sleep Mode
                 - ARM core is clock gated (HCLK = OFF)
                 - NVIC is disable (FCLK = OFF)
                 - LLWU is used to wake up from interruptions
                 - Platform and peripheral clock are stopped
                 - MCG module can be configured to leave reference clocks running
                 - On chip voltage regulator is in a mode that supplies only enough 
                   power to run the MCU in a reduced frequency
                 - All SRAM is operating (content retained and I/O states held)
                 - Most of peripherals are in state retention mode (cannot operate)

                LLS mode is exited into RUN mode using LLWU module or RESET.

                The ALLP must be set to 0b1 in MC_PMPROT register in order to allow LLS mode.

 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_MCU_Enter_LLS
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_MCUEnter_VLLS3
 * Description: Puts the processor into VLLS3 (Very Low Leakage Stop3).

                Mode of operation details:
                 - ARM core enters SleepDeep Mode
                 - ARM core is clock gated (HCLK = OFF)
                 - NVIC is disable (FCLK = OFF)
                 - LLWU should configure by user to enable the desire wake up source
                 - Platform and peripheral clock are stopped
                 - MCG module can be configured to leave reference clocks running
                 - On chip voltage regulator is in a mode that supplies only enough
                   power to run the MCU in a reduced frequency
                 - All SRAM is operating (content retained and I/O states held)
                 - Most modules are disabled

                VLLS3 mode is exited into RUN mode using LLWU module or RESET.
                All wakeup goes through Reset sequence.

                The AVLLS3 must be set to 0b1 in MC_PMPROT register in order to allow VLLS3 mode.

 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_MCU_Enter_VLLS3
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_MCUEnter_VLLS2
 * Description: Puts the processor into VLLS2 (Very Low Leakage Stop2).

                Mode of operation details:
                 - ARM core enters SleepDeep Mode
                 - ARM core is clock gated (HCLK = OFF)
                 - NVIC is disable (FCLK = OFF)
                 - LLWU should configure by user to enable the desire wake up source
                 - Platform and peripheral clock are stopped
                 - MCG module can be configured to leave reference clocks running
                 - On chip voltage regulator is in a mode that supplies only enough
                   power to run the MCU in a reduced frequency
                 - SRAM_L is powered off. A portion of SRAM_U remains powered on (content retained and I/O state held).
                 - Most modules are disabled

                VLLS2 mode is exited into RUN mode using LLWU module or RESET.
                All wakeup goes through Reset sequence.

                The AVLLS2 must be set to 0b1 in MC_PMPROT register in order to allow VLLS2 mode.

 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_MCU_Enter_VLLS2
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_MCUEnter_VLLS1
 * Description: Puts the processor into VLLS1 (Very Low Leakage Stop2).

                Mode of operation details:
                 - ARM core enters SleepDeep Mode
                 - ARM core is clock gated (HCLK = OFF)
                 - NVIC is disable (FCLK = OFF)
                 - LLWU should configure by user to enable the desire wake up source
                 - Platform and peripheral clock are stopped
                 - MCG module can be configured to leave reference clocks running
                 - On chip voltage regulator is in a mode that supplies only enough
                   power to run the MCU in a reduced frequency
                 - SRAM_L and SRAM_H is powered off.
                 - Most modules are disabled

                VLLS1 mode is exited into RUN mode using LLWU module or RESET.
                All wakeup goes through Reset sequence.

                The AVLLS1 must be set to 0b1 in MC_PMPROT register in order to allow VLLS2 mode.

 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_MCU_Enter_VLLS1
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_Radio_Enter_Doze
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_Radio_Enter_Doze
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_Radio_Enter_AutoDoze
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_Radio_Enter_AutoDoze
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_Radio_Enter_Idle
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_Radio_Enter_Idle
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_Radio_Enter_Hibernate
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_Radio_Enter_Hibernate
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_WakeupPinEnable
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LLWU_WakeupPinEnable
(
  PWRLib_LLWU_WakeupPin_t       wakeupPin,
  PWRLib_LLWU_WakeupPinConfig_t edgeDetection
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_WakeupModuleEnable
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LLWU_WakeupModuleEnable
(
  PWRLib_LLWU_WakeupModule_t wakeupModule
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_WakeupPinDisable
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LLWU_WakeupPinDisable
(
  PWRLib_LLWU_WakeupPin_t wakeupPin
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_WakeupModuleDisable
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LLWU_WakeupModuleDisable
(
  PWRLib_LLWU_WakeupModule_t wakeupModule
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_IsLPTMRWakeUpSource
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PWRLib_LLWU_IsLPTMRWakeUpSource
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_IsRTCWakeUpSource
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PWRLib_LLWU_IsRTCWakeUpSource
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_IsTSIWakeUpSource
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PWRLib_LLWU_IsTSIWakeUpSource
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_IsGPIOWakeUpSource
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PWRLib_LLWU_IsGPIOWakeUpSource
(
  uint8_t pinNumber
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_Isr
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
void PWRLib_LLWU_Isr
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_ClockStart
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LPTMR_ClockStart
(
  uint8_t  ClkMode,
  uint32_t Ticks
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_ClockCheck
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint32_t PWRLib_LPTMR_ClockCheck
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_ResetTicks
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LPTMR_ResetTicks
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_ClockStop
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LPTMR_ClockStop
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_Isr
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#if (gTMR_EnableHWLowPowerTimers_d == FALSE)
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif 
void PWRLib_LPTMR_Isr
(
  void
);
#endif

/*---------------------------------------------------------------------------
 * Name: PWRLib_RTC_Init
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_RTC_Init
(
 void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_RTC_ClockStart
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_RTC_ClockStart
(
  uint32_t Ticks
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_RTC_ClockCheck
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint32_t PWRLib_RTC_ClockCheck
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_RTC_ResetTicks
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_RTC_ResetTicks
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_RTC_ClockStop
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_RTC_ClockStop
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_RTC_Isr
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
void PWRLib_RTC_Isr
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_RTC_IsOscStarted
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PWRLib_RTC_IsOscStarted
(
 void
);

/*---------------------------------------------------------------------------
 * Name: PWRLib_LVD_CollectLevel
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
PWRLib_LVD_VoltageLevel_t PWRLib_LVD_CollectLevel
(
  void
);

/******************************************************************************
 * Name: PWRLib_GetMacStateReq
 * Description: Get status from MAC. Functions just as Asp_GetMacStateReq().
 *
 * Parameter(s): - none
 * Return: - gAspMacStateIdle_c     : MAC ready for Sleep or DeepSleep
 *           gAspMacStateBusy_c     : Don't sleep
 *           gAspMacStateNotEmpty_c : MAC allows Wait
 ******************************************************************************/
uint8_t PWRLib_GetMacStateReq
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* __PWR_LIB_H__ */
