/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements IRQ driver for kw2x MCU.
 */

#include <stdint.h>

#pragma GCC diagnostic ignored "-Wpedantic"
#pragma location=".isr_vector"

#define __WEAK                     __attribute__((weak))
#define __USED                     __attribute__((used))
#define __BOOT_VECTOR              __attribute__ ((section(".isr_vector")))
#define __BOOT_STACK               &__stack_start__
#define __BOOT_STARTUP             __gcc_program_start

// Interrupt Vector Table Function Pointers
typedef void (*irq_handler_t)(void);
extern unsigned int __stack_start__[];
extern void __gcc_program_start(void);

// Declaration of default interrupt service routine
inline void halt_isr(void)           { __asm("BKPT 255"); while (1); }
static void default_isr(void)        { while (1); }

/* External handlers defined in libopenthread-kw2x.a. */
extern void PHY_InterruptHandler();
extern void LPTMR_IrqHandler();
extern void UART1_IrqHandler();
extern void UART1_ERR_IrqHandler();
extern void USB_ISR();

/* The kernel interrupts - in their CMSIS form.
 * These must be implemented locally, or compiler will
 * give conflicting definition for __vector_table.
 */
__WEAK void NMI_Handler()            { }
__WEAK void HardFault_Handler()      { halt_isr(); }
__WEAK void SVC_Handler()            { halt_isr(); }
__WEAK void PendSV_Handler()         { halt_isr(); }
__WEAK void SysTick_Handler()        { halt_isr(); }

__WEAK void MemManage_Handler()      { halt_isr(); }
__WEAK void DebugMon_Handler()       { halt_isr(); }
__WEAK void BusFault_Handler()       { halt_isr(); }

/* Default handlers for other KW2X peripheral interrupts. */
__WEAK void I2C0_IrqHandler()        { default_isr(); }
__WEAK void I2C1_IrqHandler()        { default_isr(); }
__WEAK void SPI0_IrqHandler()        { default_isr(); }
__WEAK void SPI1_IrqHandler()        { default_isr(); }

__WEAK void UART0_LON_IrqHandler()   { default_isr(); }
__WEAK void UART0_IrqHandler()       { default_isr(); }
__WEAK void UART0_ERR_IrqHandler()   { default_isr(); }
__WEAK void UART1_IrqHandler()       { default_isr(); }
__WEAK void UART1_ERR_IrqHandler()   { default_isr(); }
__WEAK void UART2_IrqHandler()       { default_isr(); }
__WEAK void UART2_ERR_IrqHandler()   { default_isr(); }
__WEAK void UART3_IrqHandler()       { default_isr(); }
__WEAK void UART3_ERR_IrqHandler()   { default_isr(); }

__WEAK void FlexTimer0_IrqHandler()  { default_isr(); }
__WEAK void FlexTimer1_IrqHandler()  { default_isr(); }
__WEAK void FlexTimer2_IrqHandler()  { default_isr(); }

__WEAK void PIT0_IrqHandler()        { default_isr(); }
__WEAK void PIT1_IrqHandler()        { default_isr(); }
__WEAK void PIT2_IrqHandler()        { default_isr(); }
__WEAK void PIT3_IrqHandler()        { default_isr(); }

__WEAK void PORTA_IrqHandler()       { default_isr(); }
__WEAK void PORTB_IrqHandler()       { default_isr(); }
__WEAK void PORTC_IrqHandler()       { default_isr(); }
__WEAK void PORTD_IrqHandler()       { default_isr(); }
__WEAK void PORTE_IrqHandler()       { default_isr(); }

__WEAK void SWI_IrqHandler()         { default_isr(); }

__WEAK void USB_ISR()                { default_isr(); }
__WEAK void LPTMR_IrqHandler()       { default_isr(); }
__WEAK void PHY_InterruptHandler()   { default_isr(); }

//#pragma location = ".isr_vector"
__BOOT_VECTOR __USED
const irq_handler_t __vector_table[] =
{
    // Cortex-M vector table
    (irq_handler_t)__BOOT_STACK,    //INT_Initial_Stack_Pointer    = 0,  // Initial Stack Pointer
    (irq_handler_t)__BOOT_STARTUP,  //INT_Initial_Program_Counter  = 1,  // Initial Program Counter
    NMI_Handler,                    //INT_NMI                      = 2,  // Non-maskable Interrupt (NMI)
    HardFault_Handler,              //INT_Hard_Fault               = 3,  // Hard Fault
    MemManage_Handler,              //INT_Mem_Manage_Fault         = 4,  // MemManage Fault
    BusFault_Handler,               //INT_Bus_Fault                = 5,  // Bus Fault
    default_isr,                    //INT_Usage_Fault              = 6,  // Usage Fault
    default_isr,                    //INT_Reserved7                = 7,  // Reserved interrupt 7
    default_isr,                    //INT_Reserved8                = 8,  // Reserved interrupt 8
    default_isr,                    //INT_Reserved9                = 9,  // Reserved interrupt 9
    default_isr,                    //INT_Reserved10               = 10, // Reserved interrupt 10
    SVC_Handler,                    //INT_SVCall                   = 11, // Supervisor call (SVCall)
    DebugMon_Handler,               //INT_DebugMonitor             = 12, // Debug Monitor
    default_isr,                    //INT_Reserved13               = 13, // Reserved interrupt 13
    PendSV_Handler,                 //INT_PendableSrvReq           = 14, // Pendable request for system service (PendableSrvReq)
    SysTick_Handler,                //INT_SysTick                  = 15, // SysTick Interrupt

    // NXP KW2X vector table
    default_isr,                    //INT_DMA0              = 16, // DMA Channel 0 Transfer Complete
    default_isr,                    //INT_DMA1              = 17, // DMA Channel 1 Transfer Complete
    default_isr,                    //INT_DMA2              = 18, // DMA Channel 2 Transfer Complete
    default_isr,                    //INT_DMA3              = 19, // DMA Channel 3 Transfer Complete
    default_isr,                    //INT_DMA4              = 20, // DMA Channel 4 Transfer Complete
    default_isr,                    //INT_DMA5              = 21, // DMA Channel 5 Transfer Complete
    default_isr,                    //INT_DMA6              = 22, // DMA Channel 6 Transfer Complete
    default_isr,                    //INT_DMA7              = 23, // DMA Channel 7 Transfer Complete
    default_isr,                    //INT_DMA8              = 24, // DMA Channel 8 Transfer Complete
    default_isr,                    //INT_DMA9              = 25, // DMA Channel 9 Transfer Complete
    default_isr,                    //INT_DMA10             = 26, // DMA Channel 10 Transfer Complete
    default_isr,                    //INT_DMA11             = 27, // DMA Channel 11 Transfer Complete
    default_isr,                    //INT_DMA12             = 28, // DMA Channel 12 Transfer Complete
    default_isr,                    //INT_DMA13             = 29, // DMA Channel 13 Transfer Complete
    default_isr,                    //INT_DMA14             = 30, // DMA Channel 14 Transfer Complete
    default_isr,                    //INT_DMA15             = 31, // DMA Channel 15 Transfer Complete
    default_isr,                    //INT_DMA_Error         = 32, // DMA Error Interrupt
    default_isr,                    //INT_MCM               = 33, // Normal Interrupt
    default_isr,                    //INT_FTFL              = 34, // FTFL Interrupt
    default_isr,                    //INT_Read_Collision    = 35, // Read Collision Interrupt
    default_isr,                    //INT_LVD_LVW           = 36, // Low Voltage Detect, Low Voltage Warning
    default_isr,                    //INT_LLW               = 37, // Low Leakage Wakeup
    default_isr,                    //INT_Watchdog          = 38, // WDOG Interrupt
    default_isr,                    //INT_RNG               = 39, // RNGB Interrupt
    I2C0_IrqHandler,                //INT_I2C0              = 40, // I2C0 interrupt
    I2C1_IrqHandler,                //INT_I2C1              = 41, // I2C1 interrupt
    SPI0_IrqHandler,                //INT_SPI0              = 42, // SPI0 Interrupt
    SPI1_IrqHandler,                //INT_SPI1              = 43, // SPI1 Interrupt
    default_isr,                    //INT_I2S0_Tx           = 44, // I2S0 transmit interrupt
    default_isr,                    //INT_I2S0_Rx           = 45, // I2S0 receive interrupt
    UART0_LON_IrqHandler,           //INT_UART0_LON         = 46, // UART0 LON interrupt
    UART0_IrqHandler,               //INT_UART0_RX_TX       = 47, // UART0 Receive/Transmit interrupt
    UART0_ERR_IrqHandler,           //INT_UART0_ERR         = 48, // UART0 Error interrupt
    UART1_IrqHandler,               //INT_UART1_RX_TX       = 49, // UART1 Receive/Transmit interrupt
    UART1_ERR_IrqHandler,           //INT_UART1_ERR         = 50, // UART1 Error interrupt
    UART2_IrqHandler,               //INT_UART2_RX_TX       = 51, // UART2 Receive/Transmit interrupt
    UART2_ERR_IrqHandler,           //INT_UART2_ERR         = 52, // UART2 Error interrupt
    UART3_IrqHandler,               //INT_UART3_RX_TX       = 53, // UART3 Receive/Transmit interrupt
    UART3_ERR_IrqHandler,           //INT_UART3_ERR         = 54, // UART3 Error interrupt
    default_isr,                    //INT_ADC0              = 55, // ADC0 interrupt
    default_isr,                    //INT_CMP0              = 56, // CMP0 interrupt
    default_isr,                    //INT_CMP1              = 57, // CMP1 interrupt
    FlexTimer0_IrqHandler,          //INT_FTM0              = 58, // FTM0 fault, overflow and channels interrupt
    FlexTimer1_IrqHandler,          //INT_FTM1              = 59, // FTM1 fault, overflow and channels interrupt
    FlexTimer2_IrqHandler,          //INT_FTM2              = 60, // FTM2 fault, overflow and channels interrupt
    default_isr,                    //INT_CMT               = 61, // CMT interrupt
    default_isr,                    //INT_RTC               = 62, // RTC interrupt
    default_isr,                    //INT_RTC_Seconds       = 63, // RTC seconds interrupt
    PIT0_IrqHandler,                //INT_PIT0              = 64, // PIT timer channel 0 interrupt
    PIT1_IrqHandler,                //INT_PIT1              = 65, // PIT timer channel 1 interrupt
    PIT2_IrqHandler,                //INT_PIT2              = 66, // PIT timer channel 2 interrupt
    PIT3_IrqHandler,                //INT_PIT3              = 67, // PIT timer channel 3 interrupt
    default_isr,                    //INT_PDB0              = 68, // PDB0 Interrupt
    USB_ISR,                        //INT_USB0              = 69, // USB0 interrupt
    default_isr,                    //INT_USBDCD            = 70, // USBDCD Interrupt
    default_isr,                    //INT_Tamper            = 71, // Tamper detect interrupt
    default_isr,                    //INT_DAC0              = 72, // DAC0 interrupt
    default_isr,                    //INT_MCG               = 73, // MCG Interrupt
    LPTMR_IrqHandler,               //INT_LPTimer           = 74, // LPTimer interrupt
    PORTA_IrqHandler,               //INT_PORTA             = 75, // Port A interrupt
    PHY_InterruptHandler,           //INT_PORTB             = 76, // Port B interrupt
    PORTC_IrqHandler,               //INT_PORTC             = 77, // Port C interrupt
    PORTD_IrqHandler,               //INT_PORTD             = 78, // Port D interrupt
    PORTE_IrqHandler,               //INT_PORTE             = 79, // Port E interrupt
    SWI_IrqHandler                  //INT_SWI               = 80  // Software interrupt
};

/**
 * Flash address 0x400 is special on kx parts, and defines the security
 * settings for the entire flash.  If these are wrong or corrupted, the
 * chip can be bricked.  These default settings perform the following:
 *
 *  [0xC]     FDPROT = data flash regions unprotected
 *  [0xD]     FEPROT =     EEPROM regions unprotected
 *  [0xE]     FOPT = :2 disable NMI irq,
 *                   :1 disable ExPort
 *                   :0 LPBOOT (using normal boot)
 *  [0xF]     FSEC = unsecured
 */
const uint32_t flash_magic[] __attribute((section(".magic"))) =
{
    /* Flash Configuration magic bits */
    0xFFFFFFFF,     /* Backdoor Comparison Key */
    0xFFFFFFFF,
    0xFFFFFFFF,     /* FPROT */
    0xFFFFF9FE      /* -, -, FOPT ({NMI,EZPORT}_DIS), FSEC (SEC=2) */
};
