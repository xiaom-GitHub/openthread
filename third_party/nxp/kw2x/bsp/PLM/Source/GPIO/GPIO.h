/**************************************************************************
* Filename: GPIO.h
*
* Description: GPIO header file for AMR CORTEX-M4 processor
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

#ifndef _GPIO_H_
#define _GPIO_H_

#include "PortConfig.h"
#include "MK21D5.h"
#include "GPIO_Interface.h"

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
 *******************************************************************************
 * Private macros
 *******************************************************************************
 ******************************************************************************/

/*
 * Name: gGPIOClockGatingReg_c
 * Description: GPIO clock gating register
 */
#define gGPIOClockGatingReg_c         SIM_SCGC5 // TODO: check for all MCUs

/*
 * Name: gGPIOClockGatingReg_c
 * Description: GPIO PORTA clock gating register (used as reference index)
 */
#define gGPIOPortAClockGatingBit_c    9 // TODO: check for all MCUs

/*
 * Name: see below
 * Description: GPIO pin control register masks
 */
#define gGPIO_IRQC_Mask_c	0x0F
#define gGPIO_LK_Mask_c     0x01
#define gGPIO_MUX_Mask_c    0x07
#define gGPIO_DSE_Mask_c    0x01
#define gGPIO_ODE_Mask_c    0x01
#define gGPIO_PFE_Mask_c    0x01
#define gGPIO_SRE_Mask_c    0x01
#define gGPIO_PE_Mask_c     0x01
#define gGPIO_PS_Mask_c     0x01


/******************************************************************************
 *******************************************************************************
 * Private type definitions
 *******************************************************************************
 ******************************************************************************/
/*
 * Name: GpioPinCtrlRegMapping_t
 * Description: GPIO pin control register mapping
 */
typedef union {
	uint32_t regValue;
	struct {		
		unsigned int PS : 1;		
		unsigned int PE : 1;
		unsigned int SRE : 1;
		unsigned int reserved_field5 : 1;
		unsigned int PFE : 1;
		unsigned int ODE : 1;
		unsigned int DSE : 1;
		unsigned int reserved_field4 : 1;
		unsigned int MUX : 3;
		unsigned int reserved_field3 : 4;
		unsigned int LK : 1;
		unsigned int IRQC : 4;
		unsigned int reserved_field2 : 4; 
		unsigned int ISF : 1;
		unsigned int reserved_field1 : 7;
	} bits;
}  GpioPinCtrlRegMapping_t;

#ifdef __cplusplus
}
#endif

#endif /* _GPIO_H_ */
