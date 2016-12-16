/******************************************************************************
* Filename: vectors.c
*
* Description: Interrupt vector table for ARM CORTEX-M4 processor
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

#include "PortConfig.h"
#include "vectors.h"
#include "mcg.h"

/******************************************************************************
 ******************************************************************************
 * User Interfaces
 ******************************************************************************
 ******************************************************************************/

#include "TMR_Interface.h"
#include "Keyboard.h"
#include "IIC_Interface.h"
#include "SPI_Interface.h"
#include "UART_Interface.h"
#include "ApplicationConf.h"
#include "AppToPlatformConfig.h"
#include "USB_Interface.h"
#include "PWR_Configuration.h"

#if defined(__IAR_SYSTEMS_ICC__)
#include "intrinsics.h"
#endif

#if gMAC_PHY_INCLUDED_c
extern void PHY_InterruptHandler( void );
#endif // gMAC_PHY_INCLUDED_c

#if !defined(gTMR2_Enabled)
#define gTMR2_Enabled FALSE
#endif

#if (gTMR2_Enabled == TRUE)
extern void  TMR2_InterruptHandler( void );
#endif


/******************************************************************************
 ******************************************************************************
 * Public functions
 ******************************************************************************
 *****************************************************************************/

/******************************************************************************
 * Name: VECT_NMIInterrupt
 * Description: NMI interrupt handler
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
void VECT_NMIInterrupt
(
    void
)
{
  /* EMPTY IMPLEMENTATION */
}

/******************************************************************************
 * Name: VECT_DefaultISR
 * Description: default CPU interrupt handler
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
void VECT_DefaultISR
(
    void
)
{
#ifdef NDEBUG
  asm("loop: BL loop");
#else
  asm ("BKPT 255");
#endif
}

/******************************************************************************
 * Name: VECT_HWfaultISR
 * Description: hardware fault interrupt handler
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
void VECT_HWfaultISR
(
    void
)
{
#ifdef NDEBUG
  asm("loop: BL loop");
#else
  asm ("BKPT 255");
#endif
}




#if defined(__IAR_SYSTEMS_ICC__)
	#pragma location = ".intvec"
	const pfVectorEntry_t  __vector_table[] =
	{
	  (pfVectorEntry_t)__BOOT_STACK_ADDRESS,  /*  0x0000_0000  0    ARM core Initial Stack Pointer  */
	  (pfVectorEntry_t)__iar_program_start,  /*  0x0000_0004  1    ARM core Initial Program Counter  */

#elif defined(__GNUC__)
	const pfVectorEntry_t __vector_table[] __attribute__ ((section(".vectortable"))) =
	{
	  (pfVectorEntry_t)__SP_INIT,                     /* 0x00  0x00000000   -   ivINT_Initial_Stack_Pointer   */
	  (pfVectorEntry_t)__thumb_startup,         /* 0x01  0x00000004   -   ivINT_Initial_Program_Counter  */
#endif
/*********************************** MK21DN512 / MK21DX256 ***********************************************/
#if defined (MCU_MK21DN512) || defined (MCU_MK21DX256)
  (pfVectorEntry_t)&VECT_NMIInterrupt,  /*  0x0000_0008  2    ARM core Non-maskable Interrupt (NMI)  */
  (pfVectorEntry_t)&VECT_HWfaultISR,  /*  0x0000_000C  3    ARM core Hard Fault  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0010  4    ARM core MemManage Fault  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0014  5    ARM core Bus Fault  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0018  6    ARM core Usage Fault  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_001C  7      */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0020  8      */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0024  9      */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0028  10      */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_002C  11    ARM core Supervisor call (SVCall)  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0030  12    ARM core Debug Monitor  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0034  13      */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0038  14    ARM core Pendable request for system service  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_003C  15    ARM core System tick timer (SysTick)  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0040  16  0  DMA DMA channel 0 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0044  17  1  DMA DMA channel 1 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0048  18  2  DMA DMA channel 2 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_004C  19  3  DMA DMA channel 3 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0050  20  4  DMA DMA channel 4 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0054  21  5  DMA DMA channel 5 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0058  22  6  DMA DMA channel 6 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_005C  23  7  DMA DMA channel 7 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0060  24  8  DMA DMA channel 8 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0064  25  9  DMA DMA channel 9 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0068  26  10  DMA DMA channel 10 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_006C  27  11  DMA DMA channel 11 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0070  28  12  DMA DMA channel 12 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0074  29  13  DMA DMA channel 13 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0078  30  14  DMA DMA channel 14 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_007C  31  15  DMA DMA channel 15 transfer complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0080  32  16  DMA DMA error interrupt channels 0-15  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0084  33  17  MCM   */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0088  34  18  Flash memory Command complete  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_008C  35  19  Flash memory Read collision  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0090  36  20  Mode Controller Low-voltage detect  */
#if (cPWR_UsePowerDownMode == 1)
  (pfVectorEntry_t)PWRLib_LLWU_Isr,  /*  0x0000_0094  37  21  LLWU Low Leakage Wakeup  */
#else
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0094  37  21  LLWU Low Leakage Wakeup  */
#endif
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0098  38  22  WDOG   */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_009C  39  23  RNG Randon Number Generator  */
#if (gIIC_Enabled_d == TRUE)
  (pfVectorEntry_t)IIC_Isr,          /*  0x0000_00A0  40  24  I2C0   */
#else
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00A0  40  24  I2C0   */
#endif
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00A4  41  25  I2C1   */
#if (gSPI_Enabled_d == TRUE)
  (pfVectorEntry_t)SPI_Isr,           /*  0x0000_00A8  42  26  SPI0 Single interrupt vector for all sources  */
#else
  (pfVectorEntry_t)VECT_DefaultISR,   /*  0x0000_00A8  42  26  SPI0 Single interrupt vector for all sources  */
#endif
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00AC  43  27  SPI1 Single interrupt vector for all sources  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00B0  44  28  I2S0 Transmit  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00B4  45  29  I2S0 Receive  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00B8  46  30  UART0 Single interrupt vector for UART LON  */
#if (gUart1_Enabled_d == TRUE) && (gUart1_c == 0)
  (pfVectorEntry_t)Uart1Isr,        /*  0x0000_00BC  47  31  UART0 Single interrupt vector for UART status  */
  (pfVectorEntry_t)Uart1ErrorIsr,   /*  0x0000_00C0  48  32  UART0 Single interrupt vector for UART error  */
#elif (gUart2_Enabled_d == TRUE) && (gUart2_c == 0)
  (pfVectorEntry_t)Uart2Isr,        /*  0x0000_00BC  47  31  UART0 Single interrupt vector for UART status  */
  (pfVectorEntry_t)Uart2ErrorIsr,   /*  0x0000_00C0  48  32  UART0 Single interrupt vector for UART error  */
#else
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00BC  48  31  UART0 Single interrupt vector for UART status  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00C0  48  32  UART0 Single interrupt vector for UART error  */
#endif
#if (gUart1_Enabled_d == TRUE) && (gUart1_c == 1)
  (pfVectorEntry_t)Uart1Isr,        /*  0x0000_00C4  49  33  UART1 Single interrupt vector for UART status  */
  (pfVectorEntry_t)Uart1ErrorIsr,   /*  0x0000_00C8  50  34  UART1 Single interrupt vector for UART error  */
#elif (gUart2_Enabled_d == TRUE) && (gUart2_c == 1)
  (pfVectorEntry_t)Uart2Isr,        /*  0x0000_00C4  49  33  UART1 Single interrupt vector for UART status  */
  (pfVectorEntry_t)Uart2ErrorIsr,   /*  0x0000_00C8  50  34  UART1 Single interrupt vector for UART error  */
#else
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00C4  49  33  UART1 Single interrupt vector for UART status  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00C8  50  34  UART1 Single interrupt vector for UART error  */
#endif
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00CC  51  35  UART2 Single interrupt vector for UART status  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00D0  52  36  UART2 Single interrupt vector for UART error  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00D4  53  37  UART3 Single interrupt vector for UART status  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00D8  54  38  UART3 Single interrupt vector for UART error  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00DC  55  39  ADC0   */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00E0  56  40  CMP0   */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00E4  57  41  CMP1   */
#if (gTMR_Enabled_d == TRUE)
  (pfVectorEntry_t)TMR_InterruptHandler, /*  0x0000_00E8  58  42  FTM0 Single interrupt vector for all sources  */
#else
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00E8  58  42  FTM0 Single interrupt vector for all sources  */
#endif
#if (gTMR2_Enabled == TRUE)
  (pfVectorEntry_t)TMR2_InterruptHandler,  /*  0x0000_00EC  59  43  FTM1 Single interrupt vector for all sources  */
#else
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00EC  59  43  FTM1 Single interrupt vector for all sources  */
#endif
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00F0  60  44  FTM2 Single interrupt vector for all sources  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00F4  61  45  CMT   */
#if (cPWR_UsePowerDownMode == 1)
  (pfVectorEntry_t)PWRLib_RTC_Isr, /*  0x0000_00F8  62  46  RTC Alarm interrupt  */
#else
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00F8  62  46  RTC Alarm interrupt  */
#endif
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_00FC  63  47  RTC Seconds interrupt  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0100  64  48  PIT Channel 0  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0104  65  49  PIT Channel 1  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0108  66  50  PIT Channel 2  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_010C  67  51  PIT Channel 3  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0110  68  52  PDB   */
#if gUsbIncluded_d
  (pfVectorEntry_t)USB_ISR,
#else
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0114  69  53  USB OTG   */
#endif
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0118  70  54  USB Charger  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_011C  71  55     */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0120  72  56  DAC0   */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0124  73  57  MCG   */
#if (cPWR_UsePowerDownMode == 1)
  (pfVectorEntry_t)PWRLib_LPTMR_Isr,  /*  0x0000_0128  74  58  Low Power Timer   */
#else
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0128  74  58  Low Power Timer   */
#endif
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_012C  75  59  Port control module Pin detect (Port A)  */
#if gMAC_PHY_INCLUDED_c
  (pfVectorEntry_t)PHY_InterruptHandler, /*  0x0000_0130  76  60  Port control module Pin detect (Port B)  */
#else
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0130  76  60  Port control module Pin detect (Port B)  */
#endif
#if (gKeyBoardSupported_d == TRUE)
  (pfVectorEntry_t)Switch_Press_ISR, /*  0x0000_0134  77  61  Port control module Pin detect (Port C)  */
#else
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0134  77  61  Port control module Pin detect (Port C)  */
#endif
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0138  78  62  Port control module Pin detect (Port D)  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_013C  79  63  Port control module Pin detect (Port E)  */
  (pfVectorEntry_t)VECT_DefaultISR,  /*  0x0000_0140  80  64  Software Software interrupt4  */

/*********************************** MK20D5 ***********************************/
#elif defined(MCU_MK20D5)
	  (pfVectorEntry_t)&VECT_NMIInterrupt,        /* 0x02  0x00000008   -2  ivINT_NMI            */
	  (pfVectorEntry_t)&VECT_HWfaultISR,          /* 0x03  0x0000000C   -1  ivINT_Hard_Fault         */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x04  0x00000010   -   ivINT_Reserved4        */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x05  0x00000014   -   ivINT_Bus_Fault        */
	  (pfVectorEntry_t)VECT_DefaultISR,           /* 0x06  0x00000018   -   ivINT_Usage_Fault        */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x07  0x0000001C   -   ivINT_Reserved7        */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x08  0x00000020   -   ivINT_Reserved8        */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x09  0x00000024   -   ivINT_Reserved9        */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x0A  0x00000028   -   ivINT_Reserved10         */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x0B  0x0000002C   -   ivINT_SVCall           */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x0C  0x00000030   -   ivINT_DebugMonitor       */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x0D  0x00000034   -   ivINT_Reserved13         */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x0E  0x00000038   -   ivINT_PendableSrvReq       */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x0F  0x0000003C   -   ivINT_SysTick          */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x10  0x00000040   -   ivINT_DMA0           */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x11  0x00000044   -   ivINT_DMA1           */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x12  0x00000048   -   ivINT_DMA2           */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x13  0x0000004C   -   ivINT_DMA3           */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x14  0x00000050   -   ivINT_DMA_Error        */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x15  0x00000054   -   ivINT_MCM            */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x16  0x00000058   -   ivINT_FTFL           */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x17  0x0000005C   -   ivINT_Read_Collision       */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x18  0x00000060   -   ivINT_LVD_LVW          */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x19  0x00000064   -   ivINT_LLW            */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x1A  0x00000068   -   ivINT_Watchdog         */
	#if (gIIC_Enabled_d == TRUE)
	  (pfVectorEntry_t)IIC_Isr,           /* 0x1B  0x0000006C   -   ivINT_I2C0           */
	#else
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x1B  0x0000006C   -   ivINT_I2C0           */
	#endif
	#if (gSPI_Enabled_d == TRUE)
	  (pfVectorEntry_t)SPI_Isr,           /* 0x1C  0x00000070   -   ivINT_SPI0           */
	#else
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x1C  0x00000070   -   ivINT_SPI0           */
	#endif
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x1D  0x00000074   -   ivINT_I2S0           */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x1E  0x00000078   -   ivINT_I2S1           */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x1F  0x0000007C   -   ivINT_UART0_LON        */
	#if (gUart1_Enabled_d == TRUE) && (gUart1_c == 0)
	  (pfVectorEntry_t)Uart1Isr,           /* 0x20  0x00000080   -   ivINT_UART0_RX_TX        */
	  (pfVectorEntry_t)Uart1ErrorIsr,         /* 0x21  0x00000084   -   ivINT_UART0_ERR        */
	#elif (gUart2_Enabled_d == TRUE) && (gUart2_c == 0)
	  (pfVectorEntry_t)Uart2Isr,           /* 0x20  0x00000080   -   ivINT_UART0_RX_TX        */
	  (pfVectorEntry_t)Uart2ErrorIsr,         /* 0x21  0x00000084   -   ivINT_UART0_ERR        */
	#else
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x20  0x00000080   -   ivINT_UART0_RX_TX        */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x21  0x00000084   -   ivINT_UART0_ERR        */
	#endif
	#if (gUart1_Enabled_d == TRUE) && (gUart1_c == 1)
	  (pfVectorEntry_t)Uart1Isr,           /* 0x22  0x00000088   -   ivINT_UART1_RX_TX        */
	  (pfVectorEntry_t)Uart1ErrorIsr,         /* 0x23  0x0000008C   -   ivINT_UART1_ERR        */
	#elif (gUart2_Enabled_d == TRUE) && (gUart2_c == 1)
	  (pfVectorEntry_t)Uart2Isr,           /* 0x22  0x00000088   -   ivINT_UART1_RX_TX        */
	  (pfVectorEntry_t)Uart2ErrorIsr,         /* 0x23  0x0000008C   -   ivINT_UART1_ERR        */
	#else
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x22  0x00000088   -   ivINT_UART1_RX_TX        */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x23  0x0000008C   -   ivINT_UART1_ERR        */
	#endif
	#if (gUart1_Enabled_d == TRUE) && (gUart1_c == 2)
	  (pfVectorEntry_t)Uart1Isr,           /* 0x24  0x00000090   -   ivINT_UART2_RX_TX        */
	  (pfVectorEntry_t)Uart1ErrorIsr,         /* 0x25  0x00000094   -   ivINT_UART2_ERR        */
	#elif (gUart2_Enabled_d == TRUE) && (gUart2_c == 2)
	  (pfVectorEntry_t)Uart2Isr,           /* 0x24  0x00000090   -   ivINT_UART2_RX_TX        */
	  (pfVectorEntry_t)Uart2ErrorIsr,         /* 0x25  0x00000094   -   ivINT_UART2_ERR        */
	#else
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x24  0x00000090   -   ivINT_UART2_RX_TX        */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x25  0x00000094   -   ivINT_UART2_ERR        */
	#endif
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x26  0x00000098   -   ivINT_ADC0           */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x27  0x0000009C   -   ivINT_HSCMP0           */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x28  0x000000A0   -   ivINT_HSCMP1           */
	#if (gTMR_Enabled_d == TRUE)
	  (pfVectorEntry_t)TMR_InterruptHandler,       /* 0x29  0x000000A4   -   ivINT_FTM0           */
	#else
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x29  0x000000A4   -   ivINT_FTM0           */
	#endif
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x2A  0x000000A8   -   ivINT_FTM1           */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x2B  0x000000AC   -   ivINT_CMT            */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x2C  0x000000B0   -   ivINT_RTC            */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x2D  0x000000B4   -   ivINT_RTC_SecontInt      */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x2E  0x000000B8   -   ivINT_PIT0           */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x2F  0x000000BC   -   ivINT_PIT1           */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x30  0x000000C0   -   ivINT_PIT2           */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x31  0x000000C4   -   ivINT_PIT3           */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x32  0x000000C8   -   ivINT_PDB0           */
#if gUsbIncluded_d
      (pfVectorEntry_t)USB_ISR,
#else
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x33  0x000000CC   -   ivINT_USB0           */
#endif
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x34  0x000000D0   -   ivINT_USBDCD           */
	#if (gTsiSupported_d == TRUE)
	  (pfVectorEntry_t)TSI_ISR,             /* 0x35  0x000000D4   -   ivINT_TSI            */
	#else
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x35  0x000000D4   -   ivINT_TSI            */
	#endif
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x36  0x000000D8   -   ivINT_MCG            */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x37  0x000000DC   -   ivINT_LPTimer          */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x38  0x000000E0   -   ivINT_PORTA          */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x39  0x000000E4   -   ivINT_PORTB          */
	#if (gKeyBoardSupported_d == TRUE)
	  (pfVectorEntry_t)Switch_Press_ISR,         /* 0x3A  0x000000E8   -   ivINT_PORTC           */
	#else
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x3A  0x000000E8   -   ivINT_PORTC           */
	#endif
	#if gMAC_PHY_INCLUDED_c
	  (pfVectorEntry_t)PHY_InterruptHandler,       /* 0x3B  0x000000EC   -   ivINT_PORTD          */
	#else
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x3B  0x000000EC   -   ivINT_PORTD          */
	#endif
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x3C  0x000000F0   -   ivINT_PORTE          */
	  (pfVectorEntry_t)VECT_DefaultISR,         /* 0x3D  0x000000F4   -   ivINT_Reserved61         */

	/*********************************** MK60N512VMD100 / MK60D10 ***********************************/
#elif (defined(MCU_MK60N512VMD100) || defined(MCU_MK60D10))
    (pfVectorEntry_t)&VECT_NMIInterrupt,        /* 0x02  0x00000008   -2   ivINT_NMI                       */
    (pfVectorEntry_t)&VECT_HWfaultISR,          /* 0x03  0x0000000C   -1   ivINT_Hard_Fault                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x04  0x00000010   -   ivINT_Reserved4                  */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x05  0x00000014   -   ivINT_Bus_Fault                  */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x06  0x00000018   -   ivINT_Usage_Fault                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x07  0x0000001C   -   ivINT_Reserved7                  */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x08  0x00000020   -   ivINT_Reserved8                  */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x09  0x00000024   -   ivINT_Reserved9                  */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x0A  0x00000028   -   ivINT_Reserved10                 */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x0B  0x0000002C   -   ivINT_SVCall                     */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x0C  0x00000030   -   ivINT_DebugMonitor               */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x0D  0x00000034   -   ivINT_Reserved13                 */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x0E  0x00000038   -   ivINT_PendableSrvReq             */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x0F  0x0000003C   -   ivINT_SysTick                    */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x10  0x00000040   -   ivINT_DMA0                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x11  0x00000044   -   ivINT_DMA1                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x12  0x00000048   -   ivINT_DMA2                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x13  0x0000004C   -   ivINT_DMA3                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x14  0x00000050   -   ivINT_DMA4                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x15  0x00000054   -   ivINT_DMA5                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x16  0x00000058   -   ivINT_DMA6                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x17  0x0000005C   -   ivINT_DMA7                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x18  0x00000060   -   ivINT_DMA8                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x19  0x00000064   -   ivINT_DMA9                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x1A  0x00000068   -   ivINT_DMA10                      */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x1B  0x0000006C   -   ivINT_DMA11                      */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x1C  0x00000070   -   ivINT_DMA12                      */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x1D  0x00000074   -   ivINT_DMA13                      */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x1E  0x00000078   -   ivINT_DMA14                      */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x1F  0x0000007C   -   ivINT_DMA15                      */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x20  0x00000080   -   ivINT_DMA_Error                  */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x21  0x00000084   -   ivINT_MCM                        */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x22  0x00000088   -   ivINT_FTFL                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x23  0x0000008C   -   ivINT_Read_Collision             */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x24  0x00000090   -   ivINT_LVD_LVW                    */
#if (cPWR_UsePowerDownMode == 1)
    (pfVectorEntry_t)&PWRLib_LLWU_Isr,          /* 0x25  0x00000094   -   ivINT_LLW                        */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x25  0x00000094   -   ivINT_LLW                        */
#endif
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x26  0x00000098   -   ivINT_Watchdog                   */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x27  0x0000009C   -   ivINT_RNG                        */
#if (gIIC_Enabled_d == TRUE)
    (pfVectorEntry_t)&IIC_Isr,                /* 0x28  0x000000A0   -   ivINT_I2C0                       */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x28  0x000000A0   -   ivINT_I2C0                       */
#endif
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x29  0x000000A4   -   ivINT_I2C1                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x2A  0x000000A8   -   ivINT_SPI0                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x2B  0x000000AC   -   ivINT_SPI1                       */
#if (gSPI_Enabled_d == TRUE)
    (pfVectorEntry_t)&SPI_Isr,                /* 0x2C  0x000000B0   -   ivINT_SPI2                       */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x2C  0x000000B0   -   ivINT_SPI2                       */
#endif
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x2D  0x000000B4   -   ivINT_CAN0_ORed_Message_buffer   */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x2E  0x000000B8   -   ivINT_CAN0_Bus_Off               */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x2F  0x000000BC   -   ivINT_CAN0_Error                 */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x30  0x000000C0   -   ivINT_CAN0_Tx_Warning            */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x31  0x000000C4   -   ivINT_CAN0_Rx_Warning            */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x32  0x000000C8   -   ivINT_CAN0_Wake_Up               */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x33  0x000000CC   -   ivINT_Reserved51                 */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x34  0x000000D0   -   ivINT_Reserved52                 */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x35  0x000000D4   -   ivINT_CAN1_ORed_Message_buffer   */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x36  0x000000D8   -   ivINT_CAN1_Bus_Off               */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x37  0x000000DC   -   ivINT_CAN1_Error                 */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x38  0x000000E0   -   ivINT_CAN1_Tx_Warning            */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x39  0x000000E4   -   ivINT_CAN1_Rx_Warning            */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x3A  0x000000E8   -   ivINT_CAN1_Wake_Up               */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x3B  0x000000EC   -   ivINT_Reserved59                 */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x3C  0x000000F0   -   ivINT_Reserved60                 */
#if (gUart1_Enabled_d == TRUE) && (gUart1_c == 0)
    (pfVectorEntry_t)&Uart1Isr,                /* 0x3D  0x000000F4   -   ivINT_UART0_RX_TX                */
    (pfVectorEntry_t)&Uart1ErrorIsr,            /* 0x3E  0x000000F8   -   ivINT_UART0_ERR                  */
#elif (gUart2_Enabled_d == TRUE) && (gUart2_c == 0)
    (pfVectorEntry_t)&Uart2Isr,                 /* 0x3D  0x000000F4   -   ivINT_UART0_RX_TX                */
    (pfVectorEntry_t)&Uart2ErrorIsr,            /* 0x3E  0x000000F8   -   ivINT_UART0_ERR                  */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x3D  0x000000F4   -   ivINT_UART0_RX_TX                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x3E  0x000000F8   -   ivINT_UART0_ERR                  */
#endif
#if (gUart1_Enabled_d == TRUE) && (gUart1_c == 1)
    (pfVectorEntry_t)&Uart1Isr,                 /* 0x3F  0x000000FC   -   ivINT_UART1_RX_TX                */
    (pfVectorEntry_t)&Uart1ErrorIsr,            /* 0x40  0x00000100   -   ivINT_UART1_ERR                  */
#elif (gUart2_Enabled_d == TRUE) && (gUart2_c == 1)
    (pfVectorEntry_t)&Uart2Isr,                 /* 0x3F  0x000000FC   -   ivINT_UART1_RX_TX                */
    (pfVectorEntry_t)&Uart2ErrorIsr,            /* 0x40  0x00000100   -   ivINT_UART1_ERR                  */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x3F  0x000000FC   -   ivINT_UART1_RX_TX                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x40  0x00000100   -   ivINT_UART1_ERR                  */
#endif
#if (gUart1_Enabled_d == TRUE) && (gUart1_c == 2)
    (pfVectorEntry_t)&Uart1Isr,                 /* 0x41  0x00000104   -   ivINT_UART2_RX_TX                */
    (pfVectorEntry_t)&Uart1ErrorIsr,            /* 0x42  0x00000108   -   ivINT_UART2_ERR                  */
#elif (gUart2_Enabled_d == TRUE) && (gUart2_c == 2)
    (pfVectorEntry_t)&Uart2Isr,                 /* 0x41  0x00000104   -   ivINT_UART2_RX_TX                */
    (pfVectorEntry_t)&Uart2ErrorIsr,            /* 0x42  0x00000108   -   ivINT_UART2_ERR                  */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x41  0x00000104   -   ivINT_UART2_RX_TX                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x42  0x00000108   -   ivINT_UART2_ERR                  */
#endif
#if (gUart1_Enabled_d == TRUE) && (gUart1_c == 3)
    (pfVectorEntry_t)&Uart1Isr     ,            /* 0x43  0x0000010C   -   ivINT_UART3_RX_TX                */
    (pfVectorEntry_t)&Uart1ErrorIsr,            /* 0x44  0x00000110   -   ivINT_UART3_ERR                  */
#elif (gUart2_Enabled_d == TRUE) && (gUart2_c == 3)
    (pfVectorEntry_t)&Uart2Isr     ,            /* 0x43  0x0000010C   -   ivINT_UART3_RX_TX                */
    (pfVectorEntry_t)&Uart2ErrorIsr,            /* 0x44  0x00000110   -   ivINT_UART3_ERR                  */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x43  0x0000010C   -   ivINT_UART3_RX_TX                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x44  0x00000110   -   ivINT_UART3_ERR                  */
#endif
#if (gUart1_Enabled_d == TRUE) && (gUart1_c == 4)
    (pfVectorEntry_t)&Uart1Isr,                 /* 0x45  0x00000114   -   ivINT_UART4_RX_TX                */
    (pfVectorEntry_t)&Uart1ErrorIsr,            /* 0x46  0x00000118   -   ivINT_UART4_ERR                  */
#elif (gUart2_Enabled_d == TRUE) && (gUart2_c == 4)
    (pfVectorEntry_t)&Uart2Isr,                 /* 0x45  0x00000114   -   ivINT_UART4_RX_TX                */
    (pfVectorEntry_t)&Uart2ErrorIsr,            /* 0x46  0x00000118   -   ivINT_UART4_ERR                  */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x45  0x00000114   -   ivINT_UART4_RX_TX                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x46  0x00000118   -   ivINT_UART4_ERR                  */
#endif
#if (gUart1_Enabled_d == TRUE) && (gUart1_c == 5)
    (pfVectorEntry_t)&Uart1Isr,                 /* 0x47  0x0000011C   -   ivINT_UART5_RX_TX                */
    (pfVectorEntry_t)&Uart1ErrorIsr,            /* 0x48  0x00000120   -   ivINT_UART5_ERR                  */
#elif (gUart2_Enabled_d == TRUE) && (gUart2_c == 5)
    (pfVectorEntry_t)&Uart2Isr,                 /* 0x47  0x0000011C   -   ivINT_UART5_RX_TX                */
    (pfVectorEntry_t)&Uart2ErrorIsr,            /* 0x48  0x00000120   -   ivINT_UART5_ERR                  */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x47  0x0000011C   -   ivINT_UART5_RX_TX                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x48  0x00000120   -   ivINT_UART5_ERR                  */
#endif
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x49  0x00000124   -   ivINT_ADC0                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x4A  0x00000128   -   ivINT_ADC1                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x4B  0x0000012C   -   ivINT_CMP0                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x4C  0x00000130   -   ivINT_CMP1                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x4D  0x00000134   -   ivINT_CMP2                       */
#if (gTMR_Enabled_d == TRUE)
    (pfVectorEntry_t)&TMR_InterruptHandler,     /* 0x4E  0x00000138   7   ivINT_FTM0                       */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x4E  0x00000138   7   ivINT_FTM0                       */
#endif
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x4F  0x0000013C   -   ivINT_FTM1                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x50  0x00000140   -   ivINT_FTM2                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x51  0x00000144   -   ivINT_CMT                        */
#if (cPWR_UsePowerDownMode == 1)
    (pfVectorEntry_t)&PWRLib_RTC_Isr,           /* 0x52  0x00000148   -   ivINT_RTC Alarm                  */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x52  0x00000148   -   ivINT_RTC Alarm                  */
#endif
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x53  0x0000014C   -   ivINT_Reserved83                 */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x54  0x00000150   -   ivINT_PIT0                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x55  0x00000154   -   ivINT_PIT1                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x56  0x00000158   -   ivINT_PIT2                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x57  0x0000015C   -   ivINT_PIT3                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x58  0x00000160   -   ivINT_PDB0                       */
#if gUsbIncluded_d
    (pfVectorEntry_t)USB_ISR,
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x59  0x00000164   -   ivINT_USB0                       */
#endif
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x5A  0x00000168   -   ivINT_USBDCD                     */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x5B  0x0000016C   -   ivINT_ENET_1588_Timer            */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x5C  0x00000170   -   ivINT_ENET_Transmit              */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x5D  0x00000174   -   ivINT_ENET_Receive               */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x5E  0x00000178   -   ivINT_ENET_Error                 */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x5F  0x0000017C   -   ivINT_I2S0                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x60  0x00000180   -   ivINT_SDHC                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x61  0x00000184   -   ivINT_DAC0                       */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x62  0x00000188   -   ivINT_DAC1                       */
#if (gTsiSupported_d == TRUE)
    (pfVectorEntry_t)&TSI_ISR,                  /* 0x63  0x0000018C   -   ivINT_TSI0                       */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x63  0x0000018C   -   ivINT_TSI0                       */
#endif
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x64  0x00000190   -   ivINT_MCG                        */
#if (cPWR_UsePowerDownMode == 1)
    (pfVectorEntry_t)&PWRLib_LPTMR_Isr,         /* 0x65  0x00000194   -   ivINT_LPTimer                    */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x65  0x00000194   -   ivINT_LPTimer                    */
#endif
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x66  0x00000198   -   ivINT_Reserved102                */
#if (gKeyBoardSupported_d == TRUE)
    (pfVectorEntry_t)&Switch_Press_ISR,         /* 0x67  0x0000019C   -   ivINT_PORTA                      */
#else
     (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x67  0x0000019C   -   ivINT_PORTA                      */
#endif
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x68  0x000001A0   -   ivINT_PORTB                      */
#if gUart1_Enabled_d && gUart1_Hw_Sw_FlowControl_d
    (pfVectorEntry_t)&UartFlowControlISR,       /* 0x69  0x000001A4   -   ivINT_PORTC                      */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x69  0x000001A4   -   ivINT_PORTC                      */
#endif
#if gMAC_PHY_INCLUDED_c
    (pfVectorEntry_t)&PHY_InterruptHandler,     /* 0x6A  0x000001A8   -   ivINT_PORTD                      */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x6A  0x000001A8   -   ivINT_PORTD                      */
#endif
#if (gKeyBoardSupported_d == TRUE)
    (pfVectorEntry_t)&Switch_Press_ISR,         /* 0x6B  0x000001AC   -   ivINT_PORTE                      */
#else
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x6B  0x000001AC   -   ivINT_PORTE                      */
#endif
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x6C  0x000001B0   -   ivINT_Reserved108                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x6D  0x000001B4   -   ivINT_Reserved109                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x6E  0x000001B8   -   ivINT_Reserved110                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x6F  0x000001BC   -   ivINT_Reserved111                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x70  0x000001C0   -   ivINT_Reserved112                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x71  0x000001C4   -   ivINT_Reserved113                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x72  0x000001C8   -   ivINT_Reserved114                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x73  0x000001CC   -   ivINT_Reserved115                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x74  0x000001D0   -   ivINT_Reserved116                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x75  0x000001D4   -   ivINT_Reserved117                */
    (pfVectorEntry_t)&VECT_DefaultISR,          /* 0x76  0x000001D8   -   ivINT_Reserved118                */
    (pfVectorEntry_t)&VECT_DefaultISR           /* 0x77  0x000001DC   -   ivINT_Reserved119                */
#endif			//	(defined(MCU_MK60N512VMD100) || defined(MCU_MK60D10))
};
