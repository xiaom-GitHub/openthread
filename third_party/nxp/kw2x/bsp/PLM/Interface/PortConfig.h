/**************************************************************************
* Filename: PortConfig.h
*
* Description: Mapping of the IO ports and pins
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

#ifndef _PORT_CONFIG_H_
#define _PORT_CONFIG_H_

#ifndef __RAM2FLASH_APP
#include "AppToPlatformConfig.h"
#endif

#if gTarget_UserDefined_d == 0

#if (gTargetTWR_K60N512_d == 1) // TWR-K60N512
  #define MCU_MK60N512VMD100 1
  #include "TargetTWR-K60N512.h"
  #define gRNG_HwSupport_d    1
  #define gMMCAU_Support_d    1

#elif (gTargetTWR_K60D100M_d == 1) // TWR-K60D100
  #define MCU_MK60D10 1
  #include "TargetTWR-K60D100M.h"
  #define gRNG_HwSupport_d    1
  #define gMMCAU_Support_d    1   

#elif (gTargetTWR_K20D50M_d == 1) // TWR-K20D50M
  #define MCU_MK20D5 1
  #include "TargetTWR-K20D50M.h"  

#elif (gTargetTWR_K21D50M_d == 1)
  #define MCU_MK21DN512 1
  #include "TargetTWR-K21D50M.h"
  #define gRNG_HwSupport_d    1
  #define gMMCAU_Support_d    1 

#elif (gTargetTWR_KW21D512_d == 1) // TWR-KW21D512
  #define MCU_MK21DN512 1 
  #include "TargetTWR-KW21D512.h"
  #define gRNG_HwSupport_d    1
  #define gMMCAU_Support_d    1

#elif (gTargetTWR_KW21D256_d == 1) // TWR-KW21D256
  #define MCU_MK21DX256 1
  #include "TargetTWR-KW21D256.h"
  #define gRNG_HwSupport_d    1
  #define gMMCAU_Support_d    1   

#elif (gTargetTWR_KW22D512_d == 1) // TWR-KW22D512
  #define MCU_MK21DN512 1
  #include "TargetTWR-KW22D512.h"
  #define gRNG_HwSupport_d    1
  #define gMMCAU_Support_d    1

#elif (gTargetTWR_KW24D512_d == 1) // TWR-KW24D512
  #define MCU_MK21DN512 1
  #include "TargetTWR-KW24D512.h"
  #define gRNG_HwSupport_d    1
  #define gMMCAU_Support_d    1   

#elif (gTargetKW24D512_USB_d == 1) // KW24D512-USB
  #define MCU_MK21DN512 1
  #include "TargetKW24D512-USB.h"
  #define gRNG_HwSupport_d    1
  #define gMMCAU_Support_d    1   

#else
  //#warning "No target defined!"
#endif

#else /* gTarget_UserDefined_d */

/*===========================================================================*/
/*                 USER DEFINED TARGET (CUSTOM BOARD SUPPORT)                */
/*===========================================================================*/

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// HEADERS /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if defined (MCU_MK60D10)
  #include "MK60D10.h"
  #define gRNG_HwSupport_d    1
  #define gMMCAU_Support_d    1

#elif defined (MCU_MK20D5)
  #include "MK20D5.h"

#elif defined (MCU_MK60N512VMD100)
  #include "MK60N512VMD100.h"
  #define gRNG_HwSupport_d    1
  #define gMMCAU_Support_d    1

#elif defined (MCU_MK21DN512)
  #include "MK21D5.h"
  #define gRNG_HwSupport_d     1
  #define gMMCAU_Support_d     1

#elif defined (MCU_MK21DX256)
  #include "MK21D5.h"
  #define gRNG_HwSupport_d     1
  #define gMMCAU_Support_d     1

#else
  #warning No valid MCU selected for the user defined target platform. Please select a supported MCU.
#endif

/*
 * Name: PORTx
 * Description: ports definition
 */
#define PORTA    0
#define PORTB    1
#define PORTC    2
#define PORTD    3
#define PORTE    4

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////// GPIO ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
//// PLATFORM EDITOR START SECTION /////
////////////////////////////////////////

/////////////
/// PORTA ///
/////////////

// GPIOA port setting

///////////////////////////////////////////////////////////////////
// WARNING: PORTA pin [0..3] are used by the JTAG/SWD debugger   //
//          PORTA pin 4 is used by RESET (NMI) signal            //
//          DO NOT ALTER THESE BITS IN THE ABOVE GPIOA REGISTERS //
///////////////////////////////////////////////////////////////////

#define gGPIOA_PDOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOA_PSOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOA_PCOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOA_PTOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOA_PDIR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOA_PDDR_Value_c     0x00000000 /* Configurable by Platform Editor */

// PORTA pin control register setting

#define gPORTA_PCR0_Value_c     0x00000742 /* Configurable by Platform Editor, if JTAG/SWD not in use */
#define gPORTA_PCR1_Value_c     0x00000743 /* Configurable by Platform Editor, if JTAG/SWD not in use */
#define gPORTA_PCR2_Value_c     0x00000743 /* Configurable by Platform Editor, if JTAG/SWD not in use */
#define gPORTA_PCR3_Value_c     0x00000743 /* Configurable by Platform Editor, if JTAG/SWD not in use */
#define gPORTA_PCR4_Value_c     0x00000743 /* Configurable by Platform Editor, if RESET/NMI not in use */
#define gPORTA_PCR5_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR6_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR7_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR8_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR9_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR10_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR11_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR12_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR13_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR14_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR15_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR16_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR17_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR18_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR19_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR20_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR21_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR22_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR23_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR24_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR25_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR26_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR27_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR28_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR29_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR30_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTA_PCR31_Value_c    0x00000100 /* Configurable by Platform Editor */

/////////////
/// PORTB ///
///////////// 

// GPIOB port settings 

#define gGPIOB_PDOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOB_PSOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOB_PCOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOB_PTOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOB_PDIR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOB_PDDR_Value_c     0x00000000 /* Configurable by Platform Editor */

// PORTB pin control register settings

#define gPORTB_PCR0_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR1_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR2_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR3_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR4_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR5_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR6_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR7_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR8_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR9_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR10_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR11_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR12_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR13_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR14_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR15_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR16_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR17_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR18_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR19_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR20_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR21_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR22_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR23_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR24_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR25_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR26_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR27_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR28_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR29_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR30_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTB_PCR31_Value_c    0x00000100 /* Configurable by Platform Editor */

/////////////
/// PORTC ///
/////////////

// GPIOC port settings

#define gGPIOC_PDOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOC_PSOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOC_PCOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOC_PTOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOC_PDIR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOC_PDDR_Value_c     0x00000000 /* Configurable by Platform Editor */

// PORTC pin control register settings

#define gPORTC_PCR0_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR1_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR2_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR3_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR4_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR5_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR6_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR7_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR8_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR9_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR10_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR11_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR12_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR13_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR14_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR15_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR16_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR17_Value_c    0x00000100 /* Configurable by Platform Editor */ 
#define gPORTC_PCR18_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR19_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR20_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR21_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR22_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR23_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR24_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR25_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR26_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR27_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR28_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR29_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR30_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTC_PCR31_Value_c    0x00000100 /* Configurable by Platform Editor */

/////////////
/// PORTD ///
/////////////

// GPIOD port settings

#define gGPIOD_PDOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOD_PSOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOD_PCOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOD_PTOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOD_PDIR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOD_PDDR_Value_c     0x00000000 /* Configurable by Platform Editor */

// PORTD pin control register settings

#define gPORTD_PCR0_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR1_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR2_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR3_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR4_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR5_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR6_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR7_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR8_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR9_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR10_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR11_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR12_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR13_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR14_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR15_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR16_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR17_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR18_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR19_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR20_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR21_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR22_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR23_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR24_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR25_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR26_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR27_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR28_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR29_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR30_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTD_PCR31_Value_c    0x00000100 /* Configurable by Platform Editor */

/////////////
/// PORTE ///
/////////////

// GPIOE port settings 

#define gGPIOE_PDOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOE_PSOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOE_PCOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOE_PTOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOE_PDIR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOE_PDDR_Value_c     0x00000000 /* Configurable by Platform Editor */

// PORTE pin control register settings

#define gPORTE_PCR0_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR1_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR2_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR3_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR4_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR5_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR6_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR7_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR8_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR9_Value_c     0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR10_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR11_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR12_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR13_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR14_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR15_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR16_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR17_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR18_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR19_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR20_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR21_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR22_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR23_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR24_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR25_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR26_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR27_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR28_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR29_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR30_Value_c    0x00000100 /* Configurable by Platform Editor */
#define gPORTE_PCR31_Value_c    0x00000100 /* Configurable by Platform Editor */

//////////////////////////////////////
//// PLATFORM EDITOR END SECTION /////
//////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////// UART ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////   

////////////////////////////////////////
//// PLATFORM EDITOR START SECTION /////
//////////////////////////////////////// 

#ifndef gUart1_c
#define gUart1_c             gUART_HW_MOD_1_c  // Hardware UART module associated to the first logical UART module
#endif

#ifndef gUart2_c
#define gUart2_c             gUART_HW_MOD_0_c  // Hardware UART module associated to the second logical UART module
#endif

// UART_1 configuration
#define gUART1_PORT_c        PORTE   /* Configurable by Platform Editor */
#define gUART1_RX_PIN_c      1       /* Configurable by Platform Editor */
#define gUART1_TX_PIN_c      0       /* Configurable by Platform Editor */
#define gUART1_RTS_PIN_c     3       /* Configurable by Platform Editor */
#define gUART1_CTS_PIN_c     2       /* Configurable by Platform Editor */
#define gUART1_PORT_MUX_c    3       /* Configurable by Platform Editor */

// UART2 configuration
#define gUART2_PORT_c        PORTD   /* Configurable by Platform Editor */
#define gUART2_RX_PIN_c      6       /* Configurable by Platform Editor */
#define gUART2_TX_PIN_c      7       /* Configurable by Platform Editor */
#define gUART2_RTS_PIN_c     4       /* Configurable by Platform Editor */
#define gUART2_CTS_PIN_c     5       /* Configurable by Platform Editor */
#define gUART2_PORT_MUX_c    3       /* Configurable by Platform Editor */

//////////////////////////////////////
//// PLATFORM EDITOR END SECTION /////
//////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
////////////////////////////////// LED ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
//// PLATFORM EDITOR START SECTION /////
////////////////////////////////////////

/* LEDs count on target board */
#ifndef gLEDsOnTargetBoardCnt_c
#define gLEDsOnTargetBoardCnt_c    4 /* Configurable by Platform Editor */
#endif

#if gLEDsOnTargetBoardCnt_c > 0
#define gLED1_Port_c        PORTD    /* Configurable by Platform Editor */
#define gLED1_Pin_c         4        /* Configurable by Platform Editor */
#define gLED1_Value_c       0        /* Configurable by Platform Editor */
#endif
#if gLEDsOnTargetBoardCnt_c > 1
#define gLED2_Port_c        PORTD    /* Configurable by Platform Editor */                          
#define gLED2_Pin_c         5        /* Configurable by Platform Editor */
#define gLED2_Value_c       0        /* Configurable by Platform Editor */
#endif
#if gLEDsOnTargetBoardCnt_c > 2
#define gLED3_Port_c        PORTD    /* Configurable by Platform Editor */
#define gLED3_Pin_c         6        /* Configurable by Platform Editor */
#define gLED3_Value_c       0        /* Configurable by Platform Editor */
#endif
#if gLEDsOnTargetBoardCnt_c > 3
#define gLED4_Port_c        PORTD    /* Configurable by Platform Editor */                          
#define gLED4_Pin_c         7        /* Configurable by Platform Editor */
#define gLED4_Value_c       0        /* Configurable by Platform Editor */
#endif

//////////////////////////////////////
//// PLATFORM EDITOR END SECTION /////
//////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////// KEYBOARD /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
//// PLATFORM EDITOR START SECTION /////
////////////////////////////////////////

#define gKBD_KeysCount_c    4       // Number of switches implemented on target board

#if (gKBD_KeysCount_c > 0)
#define gSW1Port_c          PORTC   // SW1 port [PORTA..PORTE]
#define gSW1Pin_c           7       // SW1 pin number [0..31]
#endif

#if (gKBD_KeysCount_c > 1)
#define gSW2Port_c          PORTC   // SW2 port [PORTA..PORTE]
#define gSW2Pin_c           6       // SW2 pin number [0..31]
#endif

#if (gKBD_KeysCount_c > 2)
#define gSW3Port_c          PORTC   // SW3 port [PORTA..PORTE]
#define gSW3Pin_c           5       // SW3 pin number [0..31]
#endif

#if (gKBD_KeysCount_c > 3)
#define gSW4Port_c          PORTC   // SW4 port [PORTA..PORTE]
#define gSW4Pin_c           4       // SW4 pin number [0..31]
#endif

//////////////////////////////////////
//// PLATFORM EDITOR END SECTION /////
//////////////////////////////////////

/* DO NOT REMOVE THE FOLLOWING FILE INCLUSION */

#include "TargetUserDefined.h"

#endif /* gTarget_UserDefined */
#endif /* _PORT_CONFIG_H_ */
