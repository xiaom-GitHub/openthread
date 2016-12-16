/**************************************************************************
* Filename: crt0.h
*
* Description: Startup routines header file for ARM CORTEX-M4 processor
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
#ifndef __CRT0_H__
#define __CRT0_H__

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
 ******************************************************************************
 * Public macros
 ******************************************************************************
 ******************************************************************************/

/*
* Name: STACK_FILL
* Description: Debug macro that enables the stack fill at startup.  
*/
#ifndef STACK_FILL
#define STACK_FILL 0            /* init stack */
#endif

#define STACK_INIT_VALUE  0x55  /* stack init value */

/*
 * Name: gCRT0_DEBUG_TRACE_CLK_d
 * Description: Debug macro that enables the trace clock and FlexBus clock  
 */
#ifndef gCRT0_DEBUG_TRACE_CLK_d
#define gCRT0_DEBUG_TRACE_CLK_d     0
#endif

#if (defined(MCU_MK60N512VMD100) || defined(MCU_MK60D10))
    
/*
 * Name: gCRT0_MK60N512_WDOG_UnlockAddr_c
 * Description: Address of Watchdog Unlock Register (16 bits) 
 */
#define gCRT0_WDOG_UnlockAddr_c    0x4005200E

/*
 * Name: gCRT0_MK60N512_WDOG_STCTRLH_Addr_c
 * Description: Address of Watchdog Status and Control Register High (16 bits) 
 */
#define gCRT0_WDOG_STCTRLH_Addr_c   0x40052000

/*
 * Name: gCRT0_MK60N512_WDOG_UnlockSeq1_c, gCRT0_MK60N512_WDOG_UnlockSeq2_c
 * Description: Unlocking Watchdog sequence words
 */
#define gCRT0_WDOG_UnlockSeq1_c   0xC520
#define gCRT0_WDOG_UnlockSeq2_c   0xD928

/*
 * Name: gCRT0_MK60N512_WDOG_Disabled_Ctrl_c
 * Description: Word to be written in in STCTRLH after unlocking sequence in order to disable the Watchdog 
 */
#define gCRT0_WDOG_Disabled_Ctrl_c  0xD2
    
#endif /* MCU_MK60N512VMD100 */
  
#if (defined (MCU_MK20D5) || defined (MCU_MK21DN512) || defined (MCU_MK21DX256))
    
/*
 * Name: gCRT0_MK60N512_WDOG_UnlockAddr_c
 * Description: Address of Watchdog Unlock Register (16 bits) 
 */
#define gCRT0_WDOG_UnlockAddr_c    0x4005200E

/*
 * Name: gCRT0_MK60N512_WDOG_STCTRLH_Addr_c
 * Description: Address of Watchdog Status and Control Register High (16 bits) 
 */
#define gCRT0_WDOG_STCTRLH_Addr_c   0x40052000

/*
 * Name: gCRT0_MK60N512_WDOG_UnlockSeq1_c, gCRT0_MK60N512_WDOG_UnlockSeq2_c
 * Description: Unlocking Watchdog sequence words
 */
#define gCRT0_WDOG_UnlockSeq1_c   0xC520
#define gCRT0_WDOG_UnlockSeq2_c   0xD928

/*
 * Name: gCRT0_MK60N512_WDOG_Disabled_Ctrl_c
 * Description: Word to be written in in STCTRLH after unlocking sequence in order to disable the Watchdog 
 */
#define gCRT0_WDOG_Disabled_Ctrl_c  0xD2
    
#endif /* MCU_MK20D5 || MCU_MK21DN512 || MCU_MK21DX256*/
    
/******************************************************************************
 *******************************************************************************
 * Public prototypes
 *******************************************************************************
 ******************************************************************************/

#if (defined(__GNUC__))   
/******************************************************************************
 * Name: __init_hardware
 * Description: performs the hardware initialization of system 
 * Parameters: -
 * Return: -
 ******************************************************************************/
extern void __init_hardware
(
        void
);

#endif // defined(__GNUC__)

#ifdef __cplusplus
}
#endif

#endif /* __CRT0_H__ */
