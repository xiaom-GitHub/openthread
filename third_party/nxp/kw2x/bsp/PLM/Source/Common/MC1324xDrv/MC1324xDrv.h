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

#ifndef __MC1324x_DRV_H__
#define __MC1324x_DRV_H__

/*****************************************************************************
 *                               INCLUDED HEADERS                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the headers that this module needs to include.    *
 * Note that it is not a good practice to include header files into header   *
 * files, so use this section only if there is no other better solution.     *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

#include "PortConfig.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
 *                             PRIVATE MACROS                                *
 *---------------------------------------------------------------------------*
 * Add to this section all the access macros, registers mappings, bit access *
 * macros, masks, flags etc ...
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/* Transceiver HW port/pin mapping */

/****************************************************************************/
/* Transceiver SPI interface configuration */
/****************************************************************************/

/*                                       .... ...------------------------------------ Reserved.  0000000 */
/*                                       |||| |||.----------------------------------- ISF.       0 - Configured interrupt has not been detected. */
/*                                       |||| |||| ....------------------------------ Reserved.  0000 */
/*                                       |||| |||| |||| ....------------------------- IRQC.      0000 - Interrupt/DMA Request disabled. */
/*                                       |||| |||| |||| |||| .----------------------- LK.        0 - Pin Control Register bits [15:0] are not locked. */
/*                                       |||| |||| |||| |||| |... .------------------ Reserved.  0000 */
/*                                       |||| |||| |||| |||| |||| |...--------------- MUX.       010 - 010 Alternative 2 (SPI2_PCS1). */
/*                                       |||| |||| |||| |||| |||| |||| .------------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |.------------ DSE.       1 - High drive strength is configured on the corresponding pin, if pin is configured as a digital output.*/
/*                                       |||| |||| |||| |||| |||| |||| ||.----------- ODE.       0 - Open Drain output is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||.---------- PFE.       0 - Passive Input Filter is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| .-------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |||| |.------- SRE.       0 - Fast slew rate is configured on the corresponding pin, if pin is configured as a digital output. */
/*                                       |||| |||| |||| |||| |||| |||| |||| ||.------ PE.        0 - Internal pull-up or pull-down resistor is not enabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||.----- PS.        0 - Internal pull-down resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||| */
/*                                     0b0000 0000 0000 0000 0000 0010 0100 0000 */
#define cSPI_SSEL_PCR_Config_c         (0x00000240UL)

/*                                       .... ...------------------------------------ Reserved.  0000000 */
/*                                       |||| |||.----------------------------------- ISF.       0 - Configured interrupt has not been detected. */
/*                                       |||| |||| ....------------------------------ Reserved.  0000 */
/*                                       |||| |||| |||| ....------------------------- IRQC.      0000 - Interrupt/DMA Request disabled. */
/*                                       |||| |||| |||| |||| .----------------------- LK.        0 - Pin Control Register bits [15:0] are not locked. */
/*                                       |||| |||| |||| |||| |... .------------------ Reserved.  0000 */
/*                                       |||| |||| |||| |||| |||| |...--------------- MUX.       010 - 010 Alternative 2 (SPI2_SCK). */
/*                                       |||| |||| |||| |||| |||| |||| .------------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |.------------ DSE.       1 - High drive strength is configured on the corresponding pin, if pin is configured as a digital output.*/
/*                                       |||| |||| |||| |||| |||| |||| ||.----------- ODE.       0 - Open Drain output is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||.---------- PFE.       0 - Passive Input Filter is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| .-------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |||| |.------- SRE.       0 - Fast slew rate is configured on the corresponding pin, if pin is configured as a digital output. */
/*                                       |||| |||| |||| |||| |||| |||| |||| ||.------ PE.        0 - Internal pull-up or pull-down resistor is not enabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||.----- PS.        0 - Internal pull-down resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||| */
/*                                     0b0000 0000 0000 0000 0000 0010 0100 0000 */
#define cSPI_SCLK_PCR_Config_c         (0x00000240UL)

/*                                       .... ...------------------------------------ Reserved.  0000000 */
/*                                       |||| |||.----------------------------------- ISF.       0 - Configured interrupt has not been detected. */
/*                                       |||| |||| ....------------------------------ Reserved.  0000 */
/*                                       |||| |||| |||| ....------------------------- IRQC.      0000 - Interrupt/DMA Request disabled. */
/*                                       |||| |||| |||| |||| .----------------------- LK.        0 - Pin Control Register bits [15:0] are not locked. */
/*                                       |||| |||| |||| |||| |... .------------------ Reserved.  0000 */
/*                                       |||| |||| |||| |||| |||| |...--------------- MUX.       010 - 010 Alternative 2 (SPI2_SOUT). */
/*                                       |||| |||| |||| |||| |||| |||| .------------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |.------------ DSE.       1 - High drive strength is configured on the corresponding pin, if pin is configured as a digital output.*/
/*                                       |||| |||| |||| |||| |||| |||| ||.----------- ODE.       0 - Open Drain output is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||.---------- PFE.       0 - Passive Input Filter is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| .-------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |||| |.------- SRE.       0 - Fast slew rate is configured on the corresponding pin, if pin is configured as a digital output. */
/*                                       |||| |||| |||| |||| |||| |||| |||| ||.------ PE.        0 - Internal pull-up or pull-down resistor is not enabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||.----- PS.        0 - Internal pull-down resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||| */
/*                                     0b0000 0000 0000 0000 0000 0010 0100 0000 */
#define cSPI_MOSI_PCR_Config_c         (0x00000240UL)

/*                                       .... ...------------------------------------ Reserved.  0000000 */
/*                                       |||| |||.----------------------------------- ISF.       0 - Configured interrupt has not been detected. */
/*                                       |||| |||| ....------------------------------ Reserved.  0000 */
/*                                       |||| |||| |||| ....------------------------- IRQC.      0000 - Interrupt/DMA Request disabled. */
/*                                       |||| |||| |||| |||| .----------------------- LK.        0 - Pin Control Register bits [15:0] are not locked. */
/*                                       |||| |||| |||| |||| |... .------------------ Reserved.  0000 */
/*                                       |||| |||| |||| |||| |||| |...--------------- MUX.       010 - 010 Alternative 2 (SPI2_SIN). */
/*                                       |||| |||| |||| |||| |||| |||| .------------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |.------------ DSE.       1 - High drive strength is configured on the corresponding pin, if pin is configured as a digital output.*/
/*                                       |||| |||| |||| |||| |||| |||| ||.----------- ODE.       0 - Open Drain output is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||.---------- PFE.       0 - Passive Input Filter is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| .-------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |||| |.------- SRE.       0 - Fast slew rate is configured on the corresponding pin, if pin is configured as a digital output. */
/*                                       |||| |||| |||| |||| |||| |||| |||| ||.------ PE.        0 - Internal pull-up or pull-down resistor is not enabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||.----- PS.        0 - Internal pull-down resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||| */
/*                                     0b0000 0000 0000 0000 0000 0010 0100 0000 */
#define cSPI_MISO_PCR_Config_c         (0x00000240UL)

#define StartSPITransfer()             MC1324x_SPI_MCR &= (uint32_t)(~(SPI_MCR_HALT_MASK))
#define StopSPITransferAndFlushFifo()  MC1324x_SPI_MCR |= (uint32_t)((SPI_MCR_HALT_MASK | SPI_MCR_CLR_RXF_MASK | SPI_MCR_CLR_TXF_MASK))
#define StopSPITransfer()              MC1324x_SPI_MCR |= (uint32_t)((SPI_MCR_HALT_MASK))
#define FlushSPIFifo()                 MC1324x_SPI_MCR |= (uint32_t)((SPI_MCR_CLR_RXF_MASK | SPI_MCR_CLR_TXF_MASK))

/* SPI PUSHR register masks */

#define cSPI_PUSHR_CONT_enable_c       (0x80000000UL)
#define cSPI_PUSHR_CTAS0_select_c      (0x00000000UL)
#define cSPI_PUSHR_CTAS1_select_c      (0x10000000UL)
#define cSPI_PUSHR_EOQ_assert_c        (0x08000000UL)

#define cSPI_PUSHR_CTAS_read_select_c  cSPI_PUSHR_CTAS0_select_c
#define cSPI_PUSHR_CTAS_write_select_c cSPI_PUSHR_CTAS0_select_c

/* SPI Status Register masks */

#define cSPI_SR_RFDF_MASK_c             (0x00020000UL)
#define cSPI_SR_RFOF_MASK_c             (0x00080000UL)
#define cSPI_SR_TFFF_MASK_c             (0x02000000UL)
#define cSPI_SR_TFUF_MASK_c             (0x08000000UL)
#define cSPI_SR_EOQF_MASK_c             (0x10000000UL)
#define cSPI_SR_TXRXS_MASK_c            (0x40000000UL)
#define cSPI_SR_TCF_MASK_c              (0x80000000UL)

#define SPI_DummyRead()                 {char dummy=MC1324x_SPI_POPR;}

#define WaitSPI_TransferCompleteFlag()  while( (MC1324x_SPI_SR & SPI_SR_TCF_MASK) != SPI_SR_TCF_MASK )
#define ClearSPI_TransferCompleteFlag() MC1324x_SPI_SR |= (uint32_t) (SPI_SR_TCF_MASK)

#define SPI_TxFIFO_IsNotFull()         ((MC1324x_SPI_SR & SPI_SR_TFFF_MASK) == SPI_SR_TFFF_MASK )
#define WaitSPI_TxFIFO_NotFullFlag()    while( (MC1324x_SPI_SR & SPI_SR_TFFF_MASK) != SPI_SR_TFFF_MASK )
#define ClearSPI_TxFIFO_NotFullFlag()   MC1324x_SPI_SR |= (uint32_t) (SPI_SR_TFFF_MASK)

#define SPI_RxFIFO_IsNotEmpty()         ( (MC1324x_SPI_SR & SPI_SR_RFDF_MASK) == SPI_SR_RFDF_MASK )
#define WaitSPI_RxFIFO_NotEmptyFlag()   while( (MC1324x_SPI_SR & SPI_SR_RFDF_MASK) != SPI_SR_RFDF_MASK )
#define ClearSPI_RxFIFO_NotEmptyFlag()  MC1324x_SPI_SR |= (uint32_t) (SPI_SR_RFDF_MASK)

#define WaitSPI_EndOfQueueFlag()        while( (MC1324x_SPI_SR & SPI_SR_EOQF_MASK) != SPI_SR_EOQF_MASK )
#define ClearSPI_EndOfQueuelag()        MC1324x_SPI_SR |= (uint32_t) (SPI_SR_EOQF_MASK)

/*________________________________________*/
/* SCK  = (fsys)*(1+DBR)*(1/BR)*(1/PBR)   */
/* TDT  = (1/fsys)*PDT*DT                 */
/* TASC = (1/fsys)*PASC*ASC               */
/* TCSC = (1/fsys)*PCSSCK*CSSCK           */
/* fsys = 48 MHz                          */
/*________________________________________*/
/* READ ACCESS (max 9 MHz)                */
/* SCK  = (48)*(1+0)*(1/2)*(1/3) = 8 MHz  */
/* DBR  = 0                               */
/* BR   = 0000(2)                         */
/* PBR  = 01(3)                           */
/*________________________________________*/
/* WRITE ACCESS (max 16 MHz)              */
/* SCK  = (48)*(1+0)*(1/2)*(1/2) = 12 MHz */
/* DBR  = 0                               */
/* BR   = 0000(2)                         */
/* PBR  = 00(2)                           */
/*________________________________________*/
/* TASC min. 31.25 ns                     */
/* TASC = (1/48)*1*2 = 41.6ns             */
/* PASC = 00(1)                           */
/* ASC  = 0000(2)                         */
/*________________________________________*/
/* TCSC min. 31.25 ns                     */
/* TCSC = (1/48)*1*2 = 41.6ns             */
/* PCSSCK = 00(1)                         */
/* CSSCK  = 0000(2)                       */
/*________________________________________*/
/* TDT min. 62.5 ns                       */
/* TDT = (1/48)*1*4 = 83.3ns              */
/* PDT = 00(1)                            */
/* DT  = 0001(4)                          */
/*________________________________________*/

/*                                       .------------------------------------------- DBR.       0 - 0 The baud rate is computed normally with a 50/50 duty cycle. */
/*                                       |... .-------------------------------------- FMSZ.      0111 - The number of bits transferred per frame is equal to the FMSZ field value plus 1. */
/*                                       |||| |.------------------------------------- CPOL.      0 - The inactive state value of SCK is low. */
/*                                       |||| ||.------------------------------------ CPHA.      0 - Data is captured on the leading edge of SCK and changed on the following edge. */
/*                                       |||| |||.----------------------------------- LSBFE.     0 - Data is transferred MSB first. */
/*                                       |||| |||| ..-------------------------------- PCSSCK.    00 - PCS to SCK Prescaler value is 1. */
/*                                       |||| |||| ||..------------------------------ PASC.      00 - Delay after Transfer Prescaler value is 1. */
/*                                       |||| |||| |||| ..----------------------------PDT.       00 - Delay after Transfer Prescaler value is 1.*/
/*                                       |||| |||| |||| ||..------------------------- PBR.       00 - Baud Rate Prescaler value is 2. */
/*                                       |||| |||| |||| |||| ....-------------------- CSSCK.     0000 - PCS to SCK Delay Scaler is 2.*/
/*                                       |||| |||| |||| |||| |||| ....--------------- ASC.       0000 - After SCK Delay Scaler is 1 */
/*                                       |||| |||| |||| |||| |||| |||| ....---------- DT.        0001 - Delay After Transfer Scaler is 4. */
/*                                       |||| |||| |||| |||| |||| |||| |||| ....----- BR.        0000 - Baud Rate Scaler is 2 */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||| */
/*                                     0b0011 1000 0000 0000 0000 0000 0001 0000 */

#define cSPI_CTAR0_Config_c            (0xB8010010UL) //16MHz

/*                                       .------------------------------------------- DBR.       0 - 0 The baud rate is computed normally with a 50/50 duty cycle. */
/*                                       |... .-------------------------------------- FMSZ.      0111 - The number of bits transferred per frame is equal to the FMSZ field value plus 1. */
/*                                       |||| |.------------------------------------- CPOL.      0 - The inactive state value of SCK is low. */
/*                                       |||| ||.------------------------------------ CPHA.      0 - Data is captured on the leading edge of SCK and changed on the following edge. */
/*                                       |||| |||.----------------------------------- LSBFE.     0 - Data is transferred MSB first. */
/*                                       |||| |||| ..-------------------------------- PCSSCK.    00 - PCS to SCK Prescaler value is 1. */
/*                                       |||| |||| ||..------------------------------ PASC.      00 - Delay after Transfer Prescaler value is 1. */
/*                                       |||| |||| |||| ..----------------------------PDT.       00 - Delay after Transfer Prescaler value is 1.*/
/*                                       |||| |||| |||| ||..------------------------- PBR.       01 - Baud Rate Prescaler value is 3. */
/*                                       |||| |||| |||| |||| ....-------------------- CSSCK.     0000 - PCS to SCK Delay Scaler is 2.*/
/*                                       |||| |||| |||| |||| |||| ....--------------- ASC.       0000 - After SCK Delay Scaler is 1 */
/*                                       |||| |||| |||| |||| |||| |||| ....---------- DT.        0001 - Delay After Transfer Scaler is 4. */
/*                                       |||| |||| |||| |||| |||| |||| |||| ....----- BR.        0000 - Baud Rate Scaler is 2 */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||| */
/*                                     0b0011 1000 0000 0001 0000 0000 0001 0000 */

#define cSPI_CTAR1_Config_c            (0x38010010UL) //8Mhz

/*                                       .------------------------------------------- TCF_RE.    0 - 0 TCF interrupt requests are disabled. */
/*                                       |..----------------------------------------- Reserved.  00 */
/*                                       |||.---------------------------------------- EOQF_RE.   0 - EOQF interrupt requests are disabled. */
/*                                       |||| .-------------------------------------- TFUF_RE.   0 - TFUF interrupt requests are disabled. */
/*                                       |||| |.------------------------------------  Reserved.  0 */
/*                                       |||| ||.------------------------------------ TFFF_RE.   0 - TFFF interrupts or DMA requests are disabled. */
/*                                       |||| |||.----------------------------------- TFFF_DIRS. 0 - TFFF flag generates interrupt requests. */
/*                                       |||| |||| ....------------------------------ Reserved.  0000 */
/*                                       |||| |||| |||| .---------------------------- RFOF_RE.   0 - RFOF interrupt requests are disabled. */
/*                                       |||| |||| |||| |.--------------------------- Reserved.  0 */
/*                                       |||| |||| |||| ||.-------------------------- RFDF_RE.   0 - RFDF interrupt or DMA requests are disabled. */
/*                                       |||| |||| |||| |||.------------------------- RFDF_DIRS. 0 - Interrupt request. */
/*                                       |||| |||| |||| |||| .... .... .... ....----- Reserved.  0000000000000000 */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||| */
/*                                     0b0000 0000 0000 0000 0000 0000 0000 0000 */
#define cSPI_RSER_Config_c             (0x00000000UL)

/****************************************************************************/
/* Transceiver GPIO pins configuration */
/****************************************************************************/



/*                                       .... ...------------------------------------ Reserved.  0000000 */
/*                                       |||| |||.----------------------------------- ISF.       0 - Configured interrupt has not been detected. */
/*                                       |||| |||| ....------------------------------ Reserved.  0000 */
/*                                       |||| |||| |||| ....------------------------- IRQC.      0000 - Interrupt/DMA Request disabled. */
/*                                       |||| |||| |||| |||| .----------------------- LK.        0 - Pin Control Register bits [15:0] are not locked. */
/*                                       |||| |||| |||| |||| |... .------------------ Reserved.  0000 */
/*                                       |||| |||| |||| |||| |||| |...--------------- MUX.       001 - Alternative 1 (GPIO). */
/*                                       |||| |||| |||| |||| |||| |||| .------------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |.------------ DSE.       1 - High drive strength is configured on the corresponding pin, if pin is configured as a digital output.*/
/*                                       |||| |||| |||| |||| |||| |||| ||.----------- ODE.       0 - Open Drain output is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||.---------- PFE.       0 - Passive Input Filter is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| .-------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |||| |.------- SRE.       0 - Fast slew rate is configured on the corresponding pin, if pin is configured as a digital output. */
/*                                       |||| |||| |||| |||| |||| |||| |||| ||.------ PE.        0 - Internal pull-up or pull-down resistor is not enabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||.----- PS.        0 - Internal pull-down resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||| */
/*                                     0b0000 0000 0000 0000 0000 0001 0100 0000 */
#define cGPIO3_PCR_Config_c            (0x00000140UL)

/*                                       .... ...------------------------------------ Reserved.  0000000 */
/*                                       |||| |||.----------------------------------- ISF.       0 - Configured interrupt has not been detected. */
/*                                       |||| |||| ....------------------------------ Reserved.  0000 */
/*                                       |||| |||| |||| ....------------------------- IRQC.      0000 - Interrupt/DMA Request disabled. */
/*                                       |||| |||| |||| |||| .----------------------- LK.        0 - Pin Control Register bits [15:0] are not locked. */
/*                                       |||| |||| |||| |||| |... .------------------ Reserved.  0000 */
/*                                       |||| |||| |||| |||| |||| |...--------------- MUX.       001 - Alternative 1 (GPIO). */
/*                                       |||| |||| |||| |||| |||| |||| .------------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |.------------ DSE.       1 - High drive strength is configured on the corresponding pin, if pin is configured as a digital output.*/
/*                                       |||| |||| |||| |||| |||| |||| ||.----------- ODE.       0 - Open Drain output is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||.---------- PFE.       0 - Passive Input Filter is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| .-------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |||| |.------- SRE.       0 - Fast slew rate is configured on the corresponding pin, if pin is configured as a digital output. */
/*                                       |||| |||| |||| |||| |||| |||| |||| ||.------ PE.        0 - Internal pull-up or pull-down resistor is not enabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||.----- PS.        0 - Internal pull-down resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||| */
/*                                     0b0000 0000 0000 0000 0000 0001 0100 0000 */
#define cGPIO4_PCR_Config_c            (0x00000140UL)

/*                                       .... ...------------------------------------ Reserved.  0000000 */
/*                                       |||| |||.----------------------------------- ISF.       0 - Configured interrupt has not been detected. */
/*                                       |||| |||| ....------------------------------ Reserved.  0000 */
/*                                       |||| |||| |||| ....------------------------- IRQC.      0000 - Interrupt/DMA Request disabled. */
/*                                       |||| |||| |||| |||| .----------------------- LK.        0 - Pin Control Register bits [15:0] are not locked. */
/*                                       |||| |||| |||| |||| |... .------------------ Reserved.  0000 */
/*                                       |||| |||| |||| |||| |||| |...--------------- MUX.       001 - Alternative 1 (GPIO). */
/*                                       |||| |||| |||| |||| |||| |||| .------------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |.------------ DSE.       1 - High drive strength is configured on the corresponding pin, if pin is configured as a digital output.*/
/*                                       |||| |||| |||| |||| |||| |||| ||.----------- ODE.       0 - Open Drain output is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||.---------- PFE.       0 - Passive Input Filter is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| .-------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |||| |.------- SRE.       0 - Fast slew rate is configured on the corresponding pin, if pin is configured as a digital output. */
/*                                       |||| |||| |||| |||| |||| |||| |||| ||.------ PE.        0 - Internal pull-up or pull-down resistor is not enabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||.----- PS.        0 - Internal pull-down resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||| */
/*                                     0b0000 0000 0000 0000 0000 0001 0100 0000 */
#define cGPIO5_PCR_Config_c            (0x00000140UL)

#define MC1324xDrv_GPIO3_PortConfig()                         \
{                                                                 \
  MC1324x_GPIO_PORT_SIM_SCG |= cGPIO3_PORT_SIM_SCG_Config_c; \
  MC1324x_GPIO3_PCR |= cGPIO3_PCR_Config_c;                   \
}

#define MC1324xDrv_GPIO4_PortConfig()                         \
{                                                                 \
  MC1324x_GPIO_PORT_SIM_SCG |= cGPIO4_PORT_SIM_SCG_Config_c; \
  MC1324x_GPIO4_PCR |= cGPIO4_PCR_Config_c;                   \
}

/****************************************************************************/
/* Transceiver RESET pin configuration */
/****************************************************************************/

/* RESET Pin Control Registers */

/*                                       .... ...------------------------------------ Reserved.  0000000 */
/*                                       |||| |||.----------------------------------- ISF.       0 - Configured interrupt has not been detected. */
/*                                       |||| |||| ....------------------------------ Reserved.  0000 */
/*                                       |||| |||| |||| ....------------------------- IRQC.      0000 - Interrupt/DMA Request disabled. */
/*                                       |||| |||| |||| |||| .----------------------- LK.        0 - Pin Control Register bits [15:0] are not locked. */
/*                                       |||| |||| |||| |||| |... .------------------ Reserved.  0000 */
/*                                       |||| |||| |||| |||| |||| |...--------------- MUX.       001 - Alternative 1 (GPIO). */
/*                                       |||| |||| |||| |||| |||| |||| .------------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |.------------ DSE.       1 - High drive strength is configured on the corresponding pin, if pin is configured as a digital output.*/
/*                                       |||| |||| |||| |||| |||| |||| ||.----------- ODE.       0 - Open Drain output is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||.---------- PFE.       0 - Passive Input Filter is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| .-------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |||| |.------- SRE.       0 - Fast slew rate is configured on the corresponding pin, if pin is configured as a digital output. */
/*                                       |||| |||| |||| |||| |||| |||| |||| ||.------ PE.        0 - Internal pull-up or pull-down resistor is not enabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||.----- PS.        0 - Internal pull-down resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||| */
/*                                     0b0000 0000 0000 0000 0000 0001 0100 0000 */
#define cRST_PCR_Config_c              (0x00000140UL)

/****************************************************************************/
/* Transceiver IRQ pin configuration */
/****************************************************************************/

/* Interrupt Pin Control Registers */

/*                                       .... ...------------------------------------ Reserved.  0000000 */
/*                                       |||| |||.----------------------------------- ISF.       0 - Configured interrupt has not been detected. */
/*                                       |||| |||| ....------------------------------ Reserved.  0000 */
/*                                       |||| |||| |||| ....------------------------- IRQC.      0000 - Interrupt/DMA Request disabled. */
/*                                       |||| |||| |||| |||| .----------------------- LK.        0 - Pin Control Register bits [15:0] are not locked. */
/*                                       |||| |||| |||| |||| |... .------------------ Reserved.  0000 */
/*                                       |||| |||| |||| |||| |||| |...--------------- MUX.       001 - Alternative 1 (GPIO). */
/*                                       |||| |||| |||| |||| |||| |||| .------------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |.------------ DSE.       0 - High drive strength is configured on the corresponding pin, if pin is configured as a digital output.*/
/*                                       |||| |||| |||| |||| |||| |||| ||.----------- ODE.       0 - Open Drain output is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||.---------- PFE.       0 - Passive Input Filter is disabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| .-------- Reserved.  0 */
/*                                       |||| |||| |||| |||| |||| |||| |||| |.------- SRE.       0 - Fast slew rate is configured on the corresponding pin, if pin is configured as a digital output. */
/*                                       |||| |||| |||| |||| |||| |||| |||| ||.------ PE.        0 - Internal pull-up or pull-down resistor is not enabled on the corresponding pin. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||.----- PS.        0 - Internal pull-down resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set. */
/*                                       |||| |||| |||| |||| |||| |||| |||| |||| */
/*                                     0b0000 0000 0000 0000 0000 0001 0000 0000 */
#define cIRQ_PCR_Config_c              (0x00000100UL)



/*****************************************************************************
 *                            PUBLIC FUNCTIONS                               *
 *---------------------------------------------------------------------------*
 * Add to this section all the global functions prototype preceded (as a     *
 * good practice) by the keyword 'extern'                                    *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_SPIInit
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
extern void MC1324xDrv_SPIInit
(
  void
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_SPI_DMA_Init
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_SPI_DMA_Init
(
  void
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_Start_PB_DMA_SPI_Write
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_Start_PB_DMA_SPI_Write
(
  uint8_t * srcAddress,
  uint8_t numOfBytes
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_Start_PB_DMA_SPI_Read
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_Start_PB_DMA_SPI_Read
(
  uint8_t * dstAddress,
  uint8_t numOfBytes
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_DirectAccessSPIWrite
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_DirectAccessSPIWrite
(
 uint8_t address,
 uint8_t value
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_DirectAccessSPIMultiByteWrite
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_DirectAccessSPIMultiByteWrite
(
 uint8_t startAddress,
 uint8_t * byteArray,
 uint8_t numOfBytes
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_PB_SPIBurstWrite
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_PB_SPIBurstWrite
(
 uint8_t * byteArray,
 uint8_t numOfBytes
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_DirectAccessSPIRead
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t MC1324xDrv_DirectAccessSPIRead
(
 uint8_t address
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_DirectAccessSPIMultyByteRead
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/

void MC1324xDrv_DirectAccessSPIMultiByteRead
(
 uint8_t startAddress,
 uint8_t * byteArray,
 uint8_t numOfBytes
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_PB_SPIBurstRead
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_PB_SPIBurstRead
(
 uint8_t * byteArray,
 uint8_t numOfBytes
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_IndirectAccessSPIWrite
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_IndirectAccessSPIWrite
(
 uint8_t address,
 uint8_t value
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_IndirectAccessSPIMultiByteWrite
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_IndirectAccessSPIMultiByteWrite
(
 uint8_t startAddress,
 uint8_t * byteArray,
 uint8_t numOfBytes
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_IndirectAccessSPIRead
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t MC1324xDrv_IndirectAccessSPIRead
(
 uint8_t address
);
/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_IndirectAccessSPIMultiByteRead
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_IndirectAccessSPIMultiByteRead
(
 uint8_t startAddress,
 uint8_t * byteArray,
 uint8_t numOfBytes
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_IRQ_PortConfig
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_IRQ_PortConfig
(
  void
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_IsIrqPending
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t  MC1324xDrv_IsIrqPending
(
  void
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_IRQ_Disable
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_IRQ_Disable
(
  void
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_IRQ_Enable
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_IRQ_Enable
(
  void
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_IRQ_IsEnabled
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t MC1324xDrv_IRQ_IsEnabled
(
  void
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_IRQ_Clear
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_IRQ_Clear
(
  void
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_RST_PortConfig
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_RST_B_PortConfig
(
  void
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_RST_Assert
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_RST_B_Assert
(
  void
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_RST_Deassert
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_RST_B_Deassert
(
  void
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_SoftRST_Assert
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_SoftRST_Assert
(
  void
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_SoftRST_Deassert
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_SoftRST_Deassert
(
  void
);


/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_RESET
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_RESET
(
  void
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_Soft_RESET
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_Soft_RESET
(
  void
);

/*---------------------------------------------------------------------------
 * Name: MC1324xDrv_Set_CLK_OUT_Freq
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void MC1324xDrv_Set_CLK_OUT_Freq
(
  uint8_t freqDiv
);

#define ProtectFromMC1324xInterrupt()   MC1324xDrv_IRQ_Disable()
#define UnprotectFromMC1324xInterrupt() MC1324xDrv_IRQ_Enable()

#ifdef __cplusplus
}
#endif

#endif /* __MC1324x_DRV_H__ */
