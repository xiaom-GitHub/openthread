/******************************************************************************
* Filename: vectors.h
*
* Description: Vector table header file for ARM CORTEX-M4 processor
* 
* NOTE: This vector table is a superset table, so interrupt sources might be 
*       listed that are not available on the specific CORTEX-M4 device.
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

#ifndef __VECTORS_H__
#define __VECTORS_H__

#include "EmbeddedTypes.h"

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
 *******************************************************************************
 * Public type definitions
 *******************************************************************************
 ******************************************************************************/
typedef void (*pfVectorEntry_t)(void);

/******************************************************************************
 ******************************************************************************
 * Public Declarations | Module External Variables
 ******************************************************************************
 ******************************************************************************/

#if defined(__IAR_SYSTEMS_ICC__)
    /*
     * Name: __BOOT_STACK_ADDRESS
     * Description: Stack pointer initialization address
     */	
    extern unsigned long __BOOT_STACK_ADDRESS[];

		/*
		 * Name: __iar_program_start
		 * Description: Program entry point
		 */	
	extern void __iar_program_start(void);	
	
    #pragma location = ".intvec"
    /*
     * Name: __vector_table
     * Description: Interrupt Vector Table
     */    
	extern const pfVectorEntry_t  __vector_table[];	 
#elif defined(__GNUC__)
	#ifdef __cplusplus
	extern "C" {
	#endif
		/*
		 * Name: __SP_INIT
		 * Description: Stack pointer initialization address
		 */
		extern uint32_t __SP_INIT[];
		
		/*
		 * Name: __thumb_startup
		 * Description: Program entry point
		 */
		extern void __thumb_startup( void );
	#ifdef __cplusplus
	}
	#endif
    /*
     * Name: __vector_table
     * Description: Interrupt Vector Table
     */    
	extern const pfVectorEntry_t  __vector_table[];
#endif



/******************************************************************************
 ******************************************************************************
 * Public functions prototypes
 ******************************************************************************
 *****************************************************************************/

/******************************************************************************
 * Name: VECT_NMIInterrupt
 * Description: NMI interrupt handler 
 * Parameter(s): -
 * Return: -
 ******************************************************************************/ 
extern void VECT_NMIInterrupt
(
        void
);

/******************************************************************************
 * Name: VECT_DefaultISR
 * Description: default CPU interrupt handler 
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
extern void VECT_DefaultISR
(
        void
); 

#ifdef __cplusplus
}
#endif

#endif /*__VECTORS_H*/
