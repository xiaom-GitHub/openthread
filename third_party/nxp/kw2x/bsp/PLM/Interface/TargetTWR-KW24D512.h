/**************************************************************************
* Filename: TargetTWR-KW24D512.h
*
* Description: Header file for TWR-KW24D512 target board
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

#ifndef TARGETTWRKW24D512_H_
#define TARGETTWRKW24D512_H_

#include "MK21D5.h"

#ifdef __cplusplus
    extern "C" {
#endif

////////////////////////////////////////
///////     Global Definitions    //////
////////////////////////////////////////

/*
 *  Name: gSystemClk24MHz_c
 *  Description: 24 MHz system clock
 */
#ifndef gSystemClk24MHz_c
#define gSystemClk24MHz_c                        24
#endif

/*
 *  Name: gSystemClk48MHz_c
 *  Description: 48 MHz system clock
 */
#ifndef gSystemClk48MHz_c
#define gSystemClk48MHz_c                        48
#endif

/*
 *  Name: gSystemClk96MHz_c
 *  Description: 96 MHz system clock
 */
#ifndef gSystemClk96MHz_c
#define gSystemClk96MHz_c                        96
#endif

/*
 * Name: gSystemClock_d
 * Description: Peripheral Bus Clock (MHz units)
 */
#ifndef gSystemClock_c
#define gSystemClock_c                           gSystemClk48MHz_c
#endif

/*
 * Name: gMhzToKHzConvert_c
 * Description: Specifies how many KHz are in 1 MHz
 */
#ifndef gMhzToKHzConvert_c
#define gMhzToKHzConvert_c                       (1000)
#endif


////////////////////////////////////////
/////////////// I/O PORT ///////////////
////////////////////////////////////////

/*
 * Name: PORTx
 * Description: ports definition
 */
#ifndef PORTA
#define PORTA    0
#endif
#ifndef PORTB
#define PORTB    1
#endif
#ifndef PORTC
#define PORTC    2
#endif
#ifndef PORTD
#define PORTD    3
#endif
#ifndef PORTE
#define PORTE    4
#endif

/* 
 * ---------------------------------------------------------------------------------
 *                               !!! IMPORTANT NOTE !!!
 * ---------------------------------------------------------------------------------
 * For non-user defined targets, the I/O ports are configured by the PLM components
 * using the GPIO driver. Nevertheless, dummy macro definitions must exists for a
 * proper compilation of PlatformInit.c module
 */
/////////////
/// PORTA ///
/////////////


#define gPORTA_ClockGatingRegister_c    SIM_SCGC5
#define gPORTA_ClockGatingBit_c         9
// GPIOA port setting

///////////////////////////////////////////////////////////////////
// WARNING: PORTA pin [0..3] are used by the JTAG/SWD debugger   //
//          PORTA pin 4 is used by RESET (NMI) signal            //
//          DO NOT ALTER THESE BITS IN THE ABOVE GPIOA REGISTERS //
///////////////////////////////////////////////////////////////////            
#define gGPIOA_PDOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOA_PDDR_Value_c     0x00000000 /* Configurable by Platform Editor */

// PORTA pin control register setting

#define gPORTA_PCR0_Value_c     0x00000742 /* Configurable by Platform Editor, if JTAG/SWD not in use */
#define gPORTA_PCR1_Value_c     0x00000743 /* Configurable by Platform Editor, if JTAG/SWD not in use */
#define gPORTA_PCR2_Value_c     0x00000743 /* Configurable by Platform Editor, if JTAG/SWD not in use */
#define gPORTA_PCR3_Value_c     0x00000743 /* Configurable by Platform Editor, if JTAG/SWD not in use */
#define gPORTA_PCR4_Value_c     0x00000743 /* Configurable by Platform Editor, if RESET/NMI not in use */
#define gPORTA_PCR5_Value_c     0x00000043 /* Configurable by Platform Editor */
#define gPORTA_PCR6_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR7_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR8_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR9_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR10_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR11_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR12_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR13_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR14_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR15_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR16_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR17_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR18_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR19_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR20_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR21_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR22_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR23_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR24_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR25_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR26_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR27_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR28_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR29_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR30_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTA_PCR31_Value_c    0x00000000 /* Configurable by Platform Editor */
      

/////////////
/// PORTB ///
///////////// 
#define gPORTB_ClockGatingRegister_c    SIM_SCGC5
#define gPORTB_ClockGatingBit_c         10
// GPIOB port settings 

#define gGPIOB_PDOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOB_PDDR_Value_c     0x00000000 /* Configurable by Platform Editor */

// PORTB pin control register settings

#define gPORTB_PCR0_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR1_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR2_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR3_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR4_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR5_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR6_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR7_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR8_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR9_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR10_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR11_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR12_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR13_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR14_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR15_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR16_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR17_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR18_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR19_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR20_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR21_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR22_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR23_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR24_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR25_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR26_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR27_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR28_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR29_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR30_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTB_PCR31_Value_c    0x00000000 /* Configurable by Platform Editor */

/////////////
/// PORTC ///
/////////////
#define gPORTC_ClockGatingRegister_c    SIM_SCGC5
#define gPORTC_ClockGatingBit_c         11
// GPIOC port settings

#define gGPIOC_PDOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOC_PDDR_Value_c     0x00000000 /* Configurable by Platform Editor */

// PORTC pin control register settings

#define gPORTC_PCR0_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR1_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR2_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR3_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR4_Value_c     0x00000103 /* Configurable by Platform Editor */
#define gPORTC_PCR5_Value_c     0x00000103 /* Configurable by Platform Editor */
#define gPORTC_PCR6_Value_c     0x00000103 /* Configurable by Platform Editor */
#define gPORTC_PCR7_Value_c     0x00000103 /* Configurable by Platform Editor */
#define gPORTC_PCR8_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR9_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR10_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR11_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR12_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR13_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR14_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR15_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR16_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR17_Value_c    0x00000000 /* Configurable by Platform Editor */ 
#define gPORTC_PCR18_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR19_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR20_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR21_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR22_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR23_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR24_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR25_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR26_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR27_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR28_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR29_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR30_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTC_PCR31_Value_c    0x00000000 /* Configurable by Platform Editor */

/////////////
/// PORTD ///
/////////////
#define gPORTD_ClockGatingRegister_c    SIM_SCGC5
#define gPORTD_ClockGatingBit_c         12
// GPIOD port settings

#define gGPIOD_PDOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOD_PDDR_Value_c     0x00000000 /* Configurable by Platform Editor */

// PORTD pin control register settings

#define gPORTD_PCR0_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR1_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR2_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR3_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR4_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR5_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR6_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR7_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR8_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR9_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR10_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR11_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR12_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR13_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR14_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR15_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR16_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR17_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR18_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR19_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR20_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR21_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR22_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR23_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR24_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR25_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR26_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR27_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR28_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR29_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR30_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTD_PCR31_Value_c    0x00000000 /* Configurable by Platform Editor */

/////////////
/// PORTE ///
/////////////
#define gPORTE_ClockGatingRegister_c    SIM_SCGC5
#define gPORTE_ClockGatingBit_c         13
// GPIOE port settings 

#define gGPIOE_PDOR_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gGPIOE_PDDR_Value_c     0x00000000 /* Configurable by Platform Editor */

// PORTE pin control register settings

#define gPORTE_PCR0_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR1_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR2_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR3_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR4_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR5_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR6_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR7_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR8_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR9_Value_c     0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR10_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR11_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR12_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR13_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR14_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR15_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR16_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR17_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR18_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR19_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR20_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR21_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR22_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR23_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR24_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR25_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR26_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR27_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR28_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR29_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR30_Value_c    0x00000000 /* Configurable by Platform Editor */
#define gPORTE_PCR31_Value_c    0x00000000 /* Configurable by Platform Editor */
      
#define mSETUP_PORT_A
      
#define mSETUP_PORT_B
      
#define  mSETUP_PORT_C    gPORTC_ClockGatingRegister_c |= (1<<gPORTC_ClockGatingBit_c);\
                          GPIOC_PDOR = gGPIOC_PDOR_Value_c;\
                          GPIOC_PDDR = gGPIOC_PDDR_Value_c;\
                          PORTC_PCR0 = gPORTC_PCR0_Value_c;\
                          PORTC_PCR1 = gPORTC_PCR1_Value_c;\
                          PORTC_PCR2 = gPORTC_PCR2_Value_c;\
                          PORTC_PCR3 = gPORTC_PCR3_Value_c;\
                          PORTC_PCR4 = gPORTC_PCR4_Value_c;\
                          PORTC_PCR5 = gPORTC_PCR5_Value_c;\
                          PORTC_PCR6 = gPORTC_PCR6_Value_c;\
                          PORTC_PCR7 = gPORTC_PCR7_Value_c;\
                          PORTC_PCR8 = gPORTC_PCR8_Value_c;\
                          PORTC_PCR9 = gPORTC_PCR9_Value_c;\
                          PORTC_PCR10 = gPORTC_PCR10_Value_c;\
                          PORTC_PCR11 = gPORTC_PCR11_Value_c;\
                          PORTC_PCR12 = gPORTC_PCR12_Value_c;\
                          PORTC_PCR13 = gPORTC_PCR13_Value_c;\
                          PORTC_PCR14 = gPORTC_PCR14_Value_c;\
                          PORTC_PCR15 = gPORTC_PCR15_Value_c;\
                          PORTC_PCR16 = gPORTC_PCR16_Value_c;\
                          PORTC_PCR17 = gPORTC_PCR17_Value_c;\
                          PORTC_PCR18 = gPORTC_PCR18_Value_c;\
                          PORTC_PCR19 = gPORTC_PCR19_Value_c;\
                          PORTC_PCR20 = gPORTC_PCR20_Value_c;\
                          PORTC_PCR21 = gPORTC_PCR21_Value_c;\
                          PORTC_PCR22 = gPORTC_PCR22_Value_c;\
                          PORTC_PCR23 = gPORTC_PCR23_Value_c;\
                          PORTC_PCR24 = gPORTC_PCR24_Value_c;\
                          PORTC_PCR25 = gPORTC_PCR25_Value_c;\
                          PORTC_PCR26 = gPORTC_PCR26_Value_c;\
                          PORTC_PCR27 = gPORTC_PCR27_Value_c;\
                          PORTC_PCR28 = gPORTC_PCR28_Value_c;\
                          PORTC_PCR29 = gPORTC_PCR29_Value_c;\
                          PORTC_PCR30 = gPORTC_PCR30_Value_c;\
                          PORTC_PCR31 = gPORTC_PCR31_Value_c;      
      
#define mSETUP_PORT_D
      
#define mSETUP_PORT_E

////////////////////////////////////////
///////////////// UART /////////////////
////////////////////////////////////////

/*
 * UART hardware modules numeric mapping macros
 */
#define gUART_HW_MOD_0_c                         0
#define gUART_HW_MOD_1_c                         1
#define gUART_HW_MOD_2_c                         2
#define gUART_HW_MOD_3_c                         3

#define gUART_HW_MOD_MAX_c                       gUART_HW_MOD_3_c                          

/*
 * Name: gUart1_c
 * Description: hardware UART module associated to the first UART peripheral
 */
#ifndef gUart1_c
#define gUart1_c                                 gUART_HW_MOD_1_c
#endif

/*
 * Name: gUart2_c
 * Description: hardware UART module associated to the second UART peripheral
 */
#ifndef gUart2_c
#define gUart2_c                                 gUART_HW_MOD_0_c
#endif

#if ((gUart1_c > gUART_HW_MOD_MAX_c) || (gUart2_c > gUART_HW_MOD_MAX_c))
#error "The selected UART hardware module is not available"
#endif

/*
 * UART1 RX, TX, RTS and CTS pins configuration
 */

#define gUART1_PORT_c                            PORTE
#define gUART1_RX_PIN_c                          1
#define gUART1_TX_PIN_c                          0
#define gUART1_RTS_PIN_c                         3
#define gUART1_CTS_PIN_c                         2
#define gUART1_PORT_MUX_c                        3           /* Port MUX (alternate function) */

/*
 * UART2 RX, TX, RTS and CTS pins configuration
 */   
#define gUART2_PORT_c                            PORTD
#define gUART2_RX_PIN_c                          6
#define gUART2_TX_PIN_c                          7
#define gUART2_RTS_PIN_c                         4
#define gUART2_CTS_PIN_c                         5
#define gUART2_PORT_MUX_c                        3           /* Port MUX (alternate function) */

/* 
 * UART_1 Software Flow Control pins mapping
 */

#define gUART1_SW_FLOW_CONTROL_RTS_PORT_c        PORTC
#define gUART1_SW_FLOW_CONTROL_RTS_PIN_c         13
#define gUART1_SW_FLOW_CONTROL_CTS_PORT_c        PORTC
#define gUART1_SW_FLOW_CONTROL_CTS_PIN_c         12
#define gUART_1_CTS_IRQ_NUM_c                    0x59          /* UART1 CTS line - port interrupt number */
#ifndef gUART_1_CTS_IRQ_PRIO_c
#define gUART_1_CTS_IRQ_PRIO_c                   0x07          /* UART1 CTS line - port interrupt priority */
#endif

/* 
 * UART_2 Software Flow Control pins mapping
 */    

#define gUART2_SW_FLOW_CONTROL_RTS_PORT_c        PORTC
#define gUART2_SW_FLOW_CONTROL_RTS_PIN_c         19
#define gUART2_SW_FLOW_CONTROL_CTS_PORT_c        PORTC
#define gUART2_SW_FLOW_CONTROL_CTS_PIN_c         18      
#define gUART_2_CTS_IRQ_NUM_c                    0x5A          /* UART2 CTS line - port interrupt number */   
#ifndef gUART_2_CTS_IRQ_PRIO_c
#define gUART_2_CTS_IRQ_PRIO_c                   0x07          /* UART2 CTS line - port interrupt priority */
#endif

////////////////////////////////////////
///////////////// LED //////////////////
////////////////////////////////////////

/**************************************
 * COMMON
 **************************************/

/* LEDs count on target board */
#ifndef gLEDsOnTargetBoardCnt_c
#define gLEDsOnTargetBoardCnt_c                  4
#endif   

/**************************************
 * LED_1
 **************************************/

#define gLED1_PORT_c    PORTD
#define gLED1_PIN_c     4

/* LED_1 Port Data Set register mapping */
#define gLED_Led1PortDataSetReg_c                GPIOD_PSOR
/* LED_1 Port Data Clear register mapping */
#define gLED_Led1PortDataClearReg_c              GPIOD_PCOR
/* LED_1 Port Data Toggle register mapping */
#define gLED_Led1PortDataToggleReg_c             GPIOD_PTOR
/* LED_1 pin definition */
#define gLED_Led1Pin_c                           (1<<gLED1_PIN_c)

/**************************************
 * LED_2
 **************************************/

#define gLED2_PORT_c    PORTD
#define gLED2_PIN_c     5   

/* LED_2 Port Data Set register mapping */
#define gLED_Led2PortDataSetReg_c                GPIOD_PSOR
/* LED_2 Port Data Clear register mapping */
#define gLED_Led2PortDataClearReg_c              GPIOD_PCOR
/* LED_2 Port Data Toggle register mapping */
#define gLED_Led2PortDataToggleReg_c             GPIOD_PTOR
/* LED_2 pin definition */
#define gLED_Led2Pin_c                           (1<<gLED2_PIN_c)

/**************************************
 * LED_3
 **************************************/

#define gLED3_PORT_c    PORTD
#define gLED3_PIN_c     6

/* LED_3 Port Data Set register mapping */
#define gLED_Led3PortDataSetReg_c                GPIOD_PSOR
/* LED_3 Port Data Clear register mapping */
#define gLED_Led3PortDataClearReg_c              GPIOD_PCOR
/* LED_3 Port Data Toggle register mapping */
#define gLED_Led3PortDataToggleReg_c             GPIOD_PTOR
/* LED_3 pin definition */
#define gLED_Led3Pin_c                           (1<<gLED3_PIN_c)

/**************************************
 * LED_4
 **************************************/

#define gLED4_PORT_c    PORTD
#define gLED4_PIN_c     7

/* LED_4 Port Data Set register mapping */
#define gLED_Led4PortDataSetReg_c                GPIOD_PSOR
/* LED_4 Port Data Clear register mapping */
#define gLED_Led4PortDataClearReg_c              GPIOD_PCOR
/* LED_4 Port Data Toggle register mapping */
#define gLED_Led4PortDataToggleReg_c             GPIOD_PTOR
/* LED_4 pin definition*/
#define gLED_Led4Pin_c                           (1<<gLED4_PIN_c)

////////////////////////////////////////
/////////////// KEYBOARD ///////////////
////////////////////////////////////////

/* Keys (swithes) available on board */
#define gKBD_KeysCount_c                         4

#define SW1_Port_c                               PORTC         /* Switch 1 port */
#define SW1_Pin_c                                7             /* Switch 1 port pin */
#define SW2_Port_c                               PORTC         /* Switch 2 port */
#define SW2_Pin_c                                6             /* Switch 2 port pin */
#define SW3_Port_c                               PORTC         /* Switch 3 port */
#define SW3_Pin_c                                5             /* Switch 3 port pin */
#define SW4_Port_c                               PORTC         /* Switch 4 port */
#define SW4_Pin_c                                4             /* Switch 4 port pin */

#define SWITCH1_PORT                             GPIOC_PDIR    /* Switch 1 port data input register */
#define SWITCH2_PORT                             GPIOC_PDIR    /* Switch 2 port data input register */
#define SWITCH3_PORT                             GPIOC_PDIR    /* Switch 3 port data input register */
#define SWITCH4_PORT                             GPIOC_PDIR    /* Switch 4 port data input register */

#define mSWITCH1_MASK                            (1<<SW1_Pin_c)  /* Switch 1 mask */
#define mSWITCH2_MASK                            (1<<SW2_Pin_c)  /* Switch 2 mask */
#define mSWITCH3_MASK                            (1<<SW3_Pin_c)  /* Switch 3 mask */
#define mSWITCH4_MASK                            (1<<SW4_Pin_c)  /* Switch 4 mask */

#define SWITCH1_PCR_REG                          PORTC_PCR7    /* Switch 1 pin control register */
#define SWITCH2_PCR_REG                          PORTC_PCR6    /* Switch 2 pin control register */
#define SWITCH3_PCR_REG                          PORTC_PCR5    /* Switch 3 pin control register */
#define SWITCH4_PCR_REG                          PORTC_PCR4    /* Switch 4 pin control register */

#define SWITCH1_IRQ_NUM                          61            /* Switch 1 port IRQ number */
#define SWITCH2_IRQ_NUM                          61            /* Switch 2 port IRQ number */ 
#define SWITCH3_IRQ_NUM                          61            /* Switch 3 port IRQ number */  
#define SWITCH4_IRQ_NUM                          61            /* Switch 4 port IRQ number */

////////////////////////////////////////
////////////////// SPI /////////////////
////////////////////////////////////////

#define gSPI_PCS_PCR_c                           PORTC_PCR4   /* SPI Peripheral Chip Select (PCS) Pin Control Register */
#define gSPI_PCS_ALT_FUNC_c                      2             /* pin alternate function */
#define gSPI_SCK_PCR_c                           PORTC_PCR5   /* SPI Serial Clock (SCK) Pin Control Register */
#define gSPI_SCK_ALT_FUNC_c                      2             /* pin alternate function */
#define gSPI_SOUT_PCR_c                          PORTC_PCR6   /* SPI Serial Output (SOUT, MOSI) Pin Control Register */
#define gSPI_SOUT_ALT_FUNC_c                     2             /* pin alternate function */
#define gSPI_SIN_PCR_c                           PORTC_PCR7   /* SPI Serial Input(SIN, MISO) Pin Control Register */
#define gSPI_SIN_ALT_FUNC_c                      2             /* pin alternate function */

#define gSPI_PORT_SIM_SCG_c                      SIM_SCGC6     /* SPI port clock gating register */
#define gSPI_PORT_SIM_SGC_BIT_c                  (1<<12)       /* SPI port clock gating bit mask */

////////////////////////////////////////
///////////////// IIC //////////////////
////////////////////////////////////////

#define gI2C_SCL_PCR_c                           PORTD_PCR2    /* SCL Pin Control Register */
#define gI2C_SCL_ALT_c                           4             /* pin alternate function */
#define gI2C_SDA_PCR_c                           PORTD_PCR3    /* SDA Pin Control Register */
#define gI2C_SDA_ALT_c                           4             /* pin alternate function */

#define gI2C_PORT_SIM_SCG_c                      SIM_SCGC4     /* I2C port clock gating register */
#define gI2C_PORT_SIM_SGC_BIT_c                  (1<<6)        /* I2C port clock gating bit mask */

////////////////////////////////////////
///////////////// RADIO ////////////////
////////////////////////////////////////

/* Transceiver HW port/pin mapping */

/****************************************************************************/
/* Transceiver SPI interface */
/****************************************************************************/

/* SPI Pin Control Registers */
#define MC1324x_SPI_SSEL_PCR                     PORTB_PCR10   /* Port B Pin Control Register 10 */
#define MC1324x_SPI_SCLK_PCR                     PORTB_PCR11   /* Port B Pin Control Register 11 */
#define MC1324x_SPI_MOSI_PCR                     PORTB_PCR16   /* Port B Pin Control Register 16 */
#define MC1324x_SPI_MISO_PCR                     PORTB_PCR17   /* Port B Pin Control Register 17 */

/* SPI PORT  Clock Gating Control Register */
#define MC1324x_SPI_PORT_SIM_SCG                 SIM_SCGC5

#define cSPI_PORT_SIM_SCG_Config_c               (1<<10)       /* PORT B Clock Gate Control enable */

/* SPI Module Clock Gating Control Register */
#define MC1324x_SPI_SIM_SCG                      SIM_SCGC6     /* System Clock Gating Control Register 6 */
#define cSPI_SIM_SCG_Config_c                    (1<<13)       /* SPI1 Clock Gate Control enable */

/* SPI Registers */
#define MC1324x_SPI_MCR                          SPI1_MCR          /* DSPI Module Configuration Register */
#define MC1324x_SPI_TCR                          SPI1_TCR          /* DSPI Transfer Count Register */
#define MC1324x_SPI_CTAR0                        SPI1_CTAR0        /* DSPI Clock and Transfer Attributes Register in Master Mode*/
#define MC1324x_SPI_CTAR0_SLAVE                  SPI1_CTAR0_SLAVE  /* DSPI Clock and Transfer Attributes Register in Slave Mode*/
#define MC1324x_SPI_CTAR1                        SPI1_CTAR1        /* DSPI Clock and Transfer Attributes Register in Master Mode*/
#define MC1324x_SPI_SR                           SPI1_SR           /* DSPI Status Register */
#define MC1324x_SPI_RSER                         SPI1_RSER         /* DSPI DMA/Interrupt Request Select and Enable Register */
#define MC1324x_SPI_PUSHR                        SPI1_PUSHR        /* DSPI PUSH TX FIFO Register In Master Mode */
#define MC1324x_SPI_PUSHR_SLAVE                  SPI1_PUSHR_SLAVE  /* DSPI PUSH TX FIFO Register In Slave Mode */
#define MC1324x_SPI_POPR                         SPI1_POPR         /* DSPI POP RX FIFO Register */
#define MC1324x_SPI_TXFR0                        SPI1_TXFR0        /* DSPI Transmit FIFO Registers 0 */
#define MC1324x_SPI_TXFR1                        SPI1_TXFR1        /* DSPI Transmit FIFO Registers 1 */
#define MC1324x_SPI_TXFR2                        SPI1_TXFR2        /* DSPI Transmit FIFO Registers 2 */
#define MC1324x_SPI_TXFR3                        SPI1_TXFR3        /* DSPI Transmit FIFO Registers 3 */
#define MC1324x_SPI_RXFR0                        SPI1_RXFR0        /* DSPI Receive FIFO Registers 0 */
#define MC1324x_SPI_RXFR1                        SPI1_RXFR1        /* DSPI Receive FIFO Registers 1 */
#define MC1324x_SPI_RXFR2                        SPI1_RXFR2        /* DSPI Receive FIFO Registers 2 */
#define MC1324x_SPI_RXFR3                        SPI1_RXFR3        /* DSPI Receive FIFO Registers 3 */

#define cSPI_PUSHR_PCS0_select_c                 (0x00010000UL)
#define cSPI_PUSHR_PCS_select_c                  cSPI_PUSHR_PCS0_select_c

/*                                       .------------------------------------------- MSTR.      1 - DSPI is in master mode. */
/*                                       |.------------------------------------------ CONT_SCKE. 0 - Continuous SCK disabled. */
/*                                       ||..---------------------------------------- DCONF.     00 - SPI. */
/*                                       |||| .-------------------------------------- FRZ.       0 - Do not halt serial transfers in debug mode. */
/*                                       |||| |.------------------------------------- MTFE.      0 - Modified SPI transfer format disabled. */
/*                                       |||| ||.------------------------------------ PCSSE.     0 - PCS[5]/PCSS is used as the Peripheral Chip Select[5] signal. */
/*                                       |||| |||.----------------------------------- ROOE.      1 - Incoming data is shifted into the shift register. */
/*                                       |||| |||| ..-------------------------------- Reserved   00 */
/*                                       |||| |||| ||.. ....------------------------- PCSIS[5:0].000001 - The inactive state of PCS[5:2]/[2]/[0] is low/high/low. */
/*                                       |||| |||| |||| |||| .----------------------- DOZE.      0 - Doze mode has no effect on DSPI.*/
/*                                       |||| |||| |||| |||| |.---------------------- MDIS.      0 - Enable DSPI clocks.*/
/*                                       |||| |||| |||| |||| ||.--------------------- DIS_TXF.   0 - Tx FIFO is enabled. */
/*                                       |||| |||| |||| |||| |||.-------------------- DIS_RXF.   0 - Rx FIFO is enabled. */
/*                                       |||| |||| |||| |||| |||| .------------------ CLR_TXF.   1 - Clear the Tx FIFO counter. */
/*                                       |||| |||| |||| |||| |||| |.----------------- CLR_RXF.   1 - Clear the Rx FIFO counter. */
/*                                       |||| |||| |||| |||| |||| ||..--------------- SMPL_PT.   00 - 0 system clocks between SCK edge and SIN sample */
/*                                       |||| |||| |||| |||| |||| |||| .... ..------- Reserved   000000 */
/*                                       |||| |||| |||| |||| |||| |||| |||| ||.------ Reserved   0 */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||.----- HALT.      1 - Stop Transfer */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||| */
/*                                     0b1000 0001 0000 0001 0000 1100 0000 0001 */
#define cSPI_MCR_Config_c              (0x81013C01UL) //PCS0


/****************************************************************************/
/* Transceiver GPIO pins mapping */
/****************************************************************************/

/* GPIO Pin Control Registers */

#define MC1324x_GPIO3_PCR                        PORTC_PCR3    /* Port C Pin Control Register 3 */
#define MC1324x_GPIO4_PCR                        PORTC_PCR1    /* Port C Pin Control Register 1 */
#define MC1324x_GPIO5_PCR                        PORTC_PCR0    /* Port C Pin Control Register 0 */

/* GPIO PORT  Clock Gating Control Register */
#define MC1324x_GPIO_PORT_SIM_SCG                SIM_SCGC5

#define cGPIO3_PORT_SIM_SCG_Config_c             (1<<11)       /* PORT C Clock Gate Control enable */

/* GPIO Registers TODO */

/****************************************************************************/
/* Transceiver RESET pin mapping*/
/****************************************************************************/

/* RESET Pin Control Registers */
#define MC1324x_RST_PCR                          PORTB_PCR19   /* Port B Pin Control Register 19 */
#define cRST_PIN_Number_c                        (1<<19)       /* PORT B pin 19  */

/* RESET PORT  Clock Gating Control Register */
#define MC1324x_RST_PORT_SIM_SCG                 SIM_SCGC5
#define cRST_PORT_SIM_SCG_Config_c               (1<<10)       /* PORT B Clock Gate Control enable */

#define MC1324x_RST_PDOR                         GPIOB_PDOR    /* Port Data Output Register */
#define MC1324x_RST_PSOR                         GPIOB_PSOR    /* Port Set Output Register */
#define MC1324x_RST_PCOR                         GPIOB_PCOR    /* Port Clear Output Register */
#define MC1324x_RST_PTOR                         GPIOB_PTOR    /* Port Toggle Output Register */
#define MC1324x_RST_PDIR                         GPIOB_PDIR    /* Port Data Input Register */
#define MC1324x_RST_PDDR                         GPIOB_PDDR    /* Port Data Direction Register */

/****************************************************************************/
/* Transceiver IRQ pin mapping */
/****************************************************************************/

/* Interrupt Pin Control Registers */
#define MC1324x_IRQ_PCR                          PORTB_PCR3    /* Port B Pin Control Register 3 */
#define cIRQ_PIN_Number_c                        (1<<3)        /* PORT B pin 3  */

/* Interrupt PORT  Clock Gating Control Register */
#define MC1324x_IRQ_PORT_SIM_SCG                 SIM_SCGC5
#define cIRQ_PORT_SIM_SCG_Config_c               (1<<10)       /* PORT B Clock Gate Control enable */

#define MC1324x_IRQ_PDOR                         GPIOB_PDOR    /* Port Data Output Register */
#define MC1324x_IRQ_PSOR                         GPIOB_PSOR    /* Port Set Output Register */
#define MC1324x_IRQ_PCOR                         GPIOB_PCOR    /* Port Clear Output Register */
#define MC1324x_IRQ_PTOR                         GPIOB_PTOR    /* Port Toggle Output Register */
#define MC1324x_IRQ_PDIR                         GPIOB_PDIR    /* Port Data Input Register */
#define MC1324x_IRQ_PDDR                         GPIOB_PDDR    /* Port Data Direction Register */

#define MC1324x_Irq_Number                       60


////////////////////////////////////////
///////////////// RTC //////////////////
////////////////////////////////////////

/*
 * RTC IRQ number
 */
#define gRTC_IRQ_Number_c                        46

////////////////////////////////////////
///////////////// LPTMR ////////////////
////////////////////////////////////////

/*
 * LPTMR IRQ number
 */
#define gLPTMR_IRQ_Number_c                      58

////////////////////////////////////////
///////////////// LLWU /////////////////
////////////////////////////////////////

/*
 * LLWU IRQ number
 */
#define gLLWU_IRQ_Number_c                       21

#ifdef __cplusplus
}
#endif

#endif /* TARGETTWRKW24D512_H_ */
