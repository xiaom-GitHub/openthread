/**************************************************************************
* Filename: SPI_Interface.h
*
* Description: SPI (DSPI) export interface file for ARM CORTEX-M4 processor
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

#ifndef SPI_INTERFACE_H_
#define SPI_INTERFACE_H_

#include "AppToPlatformConfig.h"
#include "TS_Interface.h"

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/
/*
 * Name: gSPI_Enabled_d
 * Description: enable/disable code generation for the SPI module
 */
#ifndef gSPI_Enabled_d
#define gSPI_Enabled_d                      FALSE
#endif

/*
 * Name: gSPI_SlaveTransmitBuffersNo_c
 * Description: Number of entries in the circular transmit buffer
 */
#ifndef gSPI_SlaveTransmitBuffersNo_c
#define gSPI_SlaveTransmitBuffersNo_c       3
#endif

/*
 * Name: gSPI_SlaveReceiveBufferSize_c
 * Description: Size of the driver'sRx circular buffer
 */
#ifndef gSPI_SlaveReceiveBufferSize_c
#define gSPI_SlaveReceiveBufferSize_c       128
#endif

/*
 * Name: SPI_0, SPI_1, SPI_2
 * Description: Name-to-Value mapping for SPI hardware devices available
 */

#if (gSPI_HW_MODULES_AVAILABLE_c > 0)
#define SPI_0   0
#endif
#if (gSPI_HW_MODULES_AVAILABLE_c > 1)
#define SPI_1   1
#endif
#if (gSPI_HW_MODULES_AVAILABLE_c > 2)
#define SPI_2   2
#endif
/*
 * Name: gSPI_ModuleUsed_d
 * Description: SPI hardware device used by this driver
 */
#ifndef gSPI_ModuleUsed_d
#define gSPI_ModuleUsed_d   SPI_0
#endif

#if (defined(MCU_MK20D5))
#if ( (MCU_MK20D5 == 1) && (gSPI_ModuleUsed_d != SPI_0) )
#warning "The selected SPI module is not available for K20D50M target"
#undef gSPI_ModuleUsed_d
#define gSPI_ModuleUsed_d   SPI_0 /* DO NOT EDIT */
#warning "The SPI module was automatically switched to default (SPI_0)"
#endif
#endif

/*
 * Name: gSPI_IRQ_NUM_c
 * Description: SPI Interrupt Request number (DO NOT EDIT)
 */
#if (gSPI_ModuleUsed_d == SPI_0)
#define gSPI_IRQ_NUM_c                      gSPI_0_IRQ_NUM_c
#elif (gSPI_ModuleUsed_d == SPI_1)
#define gSPI_IRQ_NUM_c                      gSPI_1_IRQ_NUM_c
#elif (gSPI_ModuleUsed_d == SPI_2)
#define gSPI_IRQ_NUM_c                      gSPI_2_IRQ_NUM_c
#else
#error "Invalid SPI module in use"
#endif

/*
 * Name: gSPI_InterruptPriority_c
 * Description: SPI interrupt priority
 */
#define gSPI_InterruptPriority_c            3

/*
 * Name: gSPI_MasterMode_c, gSPI_SlaveMode_c
 * Description: SPI Mode of operation (master or slave) internal coding
 */
/*  */
#define gSPI_MasterMode_c                   0x1
#define gSPI_SlaveMode_c                    0x0

/*
 * Name: gSPI_ActiveLowPolarity_c, gSPI_ActiveHighPolarity_c
 * Description: SPI clock polarity internal coding
 */

#define gSPI_ActiveLowPolarity_c            0x1
#define gSPI_ActiveHighPolarity_c           0x0

/*
 * Name: gSPI_OddEdgeShifting_c, gSPI_EvenEdgeShifting_c
 * Description: SPI clock phase internal coding
 */
#define gSPI_OddEdgeShifting_c              0x1
#define gSPI_EvenEdgeShifting_c             0x0

/*
 * Name: gSPI_MSBFirst_c, gSPI_LSBFirst_c
 * Description: SPI MSB/LSB first internal coding
 */
#define gSPI_MSBFirst_c                     0x0
#define gSPI_LSBFirst_c                     0x1

/*
 * Name: gSPI_DefaultMode_c
 * Description: Default SPI operation mode
 */
#define gSPI_DefaultMode_c                  gSPI_SlaveMode_c

/*
 * Name: gSPI_DefaultBaudRate_c
 * Description: Default baud rate for SPI master mode
 */
#define gSPI_DefaultBaudRate_c              gSPI_BaudRate_100000_c

/*
 * Name: gSPI_DefaultClockPol_c
 * Description: Default SPI clock polarity
 */
#define gSPI_DefaultClockPol_c              gSPI_ActiveHighPolarity_c

/*
 * Name: gSPI_DefaultClockPhase_c
 * Description: Default SPI clock phase
 */
#define gSPI_DefaultClockPhase_c            gSPI_EvenEdgeShifting_c

/*
 * Name: gSPI_FRAME_SZ_DEFAULT_c
 * Description: Default SPI frame size (bits unit)
 *              Effective number of bits is gSPI_FRAME_SZ_DEFAULT_c + 1
 */
#define gSPI_FRAME_SZ_DEFAULT_c             7

/*
 * Name: gSPI_DefaultBitwiseShifting_c
 * Description: Default SPI transfer endianness (LSbit / MSbit first)
 */
#define gSPI_DefaultBitwiseShifting_c        gSPI_MSBFirst_c

/*
 * Name: gSPI_PCS_SCK_DELAY_PRESCALER_DEFAULT_c
 * Description: Default PCS to SCK delay prescaler value
 */
#define gSPI_PCS_SCK_DELAY_PRESCALER_DEFAULT_c          0x1

/*
 * Name: gSPI_AFTER_SCK_DELAY_PRESCALER_DEFAULT_c
 * Description: Default After SCK delay prescaler value
 */
#define gSPI_AFTER_SCK_DELAY_PRESCALER_DEFAULT_c        0x1

/*
 * Name: gSPI_DELAY_AFTER_TRANSFER_PRESCALER_DEFAULT_c
 * Description: Default Delay After Transfer prescaler value
 */
#define gSPI_DELAY_AFTER_TRANSFER_PRESCALER_DEFAULT_c   0x1

/*
 * Name: gSPI_PCS_SCK_DELAY_SCALER_DEFAULT_c
 * Description: Default PCS to SCK delay scaler value
 */
#define gSPI_PCS_SCK_DELAY_SCALER_DEFAULT_c             0x2

/*
 * Name: gSPI_AFTER_SCK_DELAY_SCALER_DEFAULT
 * Description: Default After SCK delay scaler value
 */
#define gSPI_AFTER_SCK_DELAY_SCALER_DEFAULT             0x1

/*
 * Name: gSPI_DELAY_AFTER_TRANSFER_SCALER_DEFAULT_c
 * Description: Default Delay After Transfer scaler value
 */
#define gSPI_DELAY_AFTER_TRANSFER_SCALER_DEFAULT_c      0x2

/*
 * Name: gSPI_PCS_Max_c
 * Description: Specifies the maximum number of Peripheral Chip Select signals available
 *              Refer to "Chip Configuration" chapter from reference manual for details.
 */
#if (gSPI_ModuleUsed_d == SPI_0)
#define gSPI_PCS_Max_c                                  gSPI_0_PCS_CNT_c
#elif (gSPI_ModuleUsed_d == SPI_1)
#define gSPI_PCS_Max_c                                  gSPI_1_PCS_CNT_c
#else
#define gSPI_PCS_Max_c                                  gSPI_2_PCS_CNT_c
#endif

/*
 * Name: gSPI_PCS_DEFAULT_c
 * Description: Default Peripheral Chip Select pin (valid values: 0 .. gSPI_PCS_Max_c-1)
 */
#define gSPI_PCS_DEFAULT_c                              0

/*
 * Name: gSPI_PCS_STROBE_ENABLE_DEFAULT_c
 * Description: Default setting for PCS Strobe Enable
 */
#define gSPI_PCS_STROBE_ENABLE_DEFAULT_c                FALSE

/*
 * Name: gSPI_PCS_INACTIVE_STATE_DEFAULT_c
 * Description: Default setting for PCS inactive state (0-inactive low, 1-inactive high)
 */
#define gSPI_PCS_INACTIVE_STATE_DEFAULT_c               0x00

/*
 * Name: gSPI_PCS_INACTIVE_STATE_MASK_c
 * Description: PCS inactive state mask
 */
#define gSPI_PCS_INACTIVE_STATE_MASK_c                  0x3F

/*
 * Name: gSPI_CONTINUOUS_CLK_EN_DEFAULT_c
 * Description: Default setting for Continuous Clock Enable
 */
#define gSPI_CONTINUOUS_CLK_EN_DEFAULT_c                FALSE

/*
 * Name: gSPI_RXFIFO_OVFL_EN_DEFAULT_c
 * Description: Default setting for RX FIFO Overflow Enable
 */
#define gSPI_RXFIFO_OVFL_EN_DEFAULT_c                   TRUE

/*
 * Name: gSPI_CONTINUOUS_PCS_EN_DEFAULT_c
 * Description: Default setting for Continuous Peripheral Chip Select Enable
 */
#define gSPI_CONTINUOUS_PCS_EN_DEFAULT_c                TRUE

/*
 * Name: gSPI_DOZE_EN_DEFAULT_c
 * Description: Default setting for Doze Enable
 */
#define gSPI_DOZE_EN_DEFAULT_c                          FALSE

/*
 * Name: gSPI_CTAR_RESET_VALUE
 * Description: Reset value of CTARn register
 */
#define gSPI_CTAR_RESET_VALUE                           0x00000000UL

/*
 * Name: see below
 * Description: PBR and BR registers value for various baudrates and system clock
 */
#if (gSystemClock_c == gSystemClk48MHz_c)
/* 100 KHz */
#define gSPI_PBR_100K_Value_c       0x3u
#define gSPI_BR_100K_Value_c        0x6u
/* 200 KHz */
#define gSPI_PBR_200K_Value_c       0x3u
#define gSPI_BR_200K_Value_c        0x5u
/* 400 KHz */
#define gSPI_PBR_400K_Value_c       0x0u
#define gSPI_BR_400K_Value_c        0x6u
/* 800 KHz */
#define gSPI_PBR_800K_Value_c       0x0u
#define gSPI_BR_800K_Value_c        0x5u
/* 1MHz */
#define gSPI_PBR_1M_Value_c         0x1u
#define gSPI_BR_1M_Value_c          0x4u
/* 2MHz */
#define gSPI_PBR_2M_Value_c         0x1u
#define gSPI_BR_2M_Value_c          0x3u
/* 4 MHz */
#define gSPI_PBR_4M_Value_c         0x1u
#define gSPI_BR_4M_Value_c          0x1u
/* 8 MHz */
#define gSPI_PBR_8M_Value_c         0x1u
#define gSPI_BR_8M_Value_c          0x0u
#else
#warning "System clock must be set to 48 MHz"
#endif

/*
 * Name: PORTx
 * Description: mapping macros
 */
#ifndef PORTA
#define PORTA     0
#endif
#ifndef PORTB
#define PORTB     1
#endif
#ifndef PORTC
#define PORTC     2
#endif
#ifndef PORTD
#define PORTD     3
#endif
#ifndef PORTE
#define PORTE     4
#endif

/*
 * Name: gSPI_Slave_TxDataAvailableSignal_Enable_d
 * Description: Slave transmitter can signal to the master if there's data available
 */
#ifndef gSPI_Slave_TxDataAvailableSignal_Enable_d
#define gSPI_Slave_TxDataAvailableSignal_Enable_d       TRUE
#endif


#if (gSPI_Slave_TxDataAvailableSignal_Enable_d)

#ifndef gSPI_DataAvailablePin_c
#define gSPI_DataAvailablePin_c                         1
#endif

/* Port Data register pin mask */
#ifndef gSPI_DataAvailablePinMask_c
#define gSPI_DataAvailablePinMask_c                     (1<<gSPI_DataAvailablePin_c)
#endif

#ifndef gSPI_DataAvailablePortDataReg_c
#define gSPI_DataAvailablePortDataReg_c                 PORTD /* default value */
#endif

#if (gSPI_DataAvailablePortDataReg_c == PORTA)
    /* Port Data Input register mapping */
    #ifndef gSPI_DataAvailablePortDataINReg_c
    #define gSPI_DataAvailablePortDataINReg_c           GPIOA_PDIR
    #endif
    /* Port Data Output register mapping */
    #ifndef gSPI_DataAvailablePortDataOUTReg_c
    #define gSPI_DataAvailablePortDataOUTReg_c          GPIOA_PDOR
    #endif
    /* Port Data Direction register mapping */
    #ifndef gSPI_DataAvailablePortDDirReg_c
    #define gSPI_DataAvailablePortDDirReg_c             GPIOA_PDDR
    #endif
    /* Pin Control register mapping */
    #ifndef gSPI_DataAvailablePinCtrlReg_c
    #define gSPI_DataAvailablePinCtrlReg_c              CONCAT(PORTA_PCR,gSPI_DataAvailablePin_c)
    #endif
    /* SPI data available clock gating register */
    #ifndef gSPI_DataAvailablePortClkGatingReg_c
    #define gSPI_DataAvailablePortClkGatingReg_c        SIM_SCGC5
    #endif
    /* SPI data available clock gating mask */
    #ifndef gSPI_DataAvailablePortClkGatingBit_c
    #define gSPI_DataAvailablePortClkGatingBit_c        (1<<9)
    #endif
#elif (gSPI_DataAvailablePortDataReg_c == PORTB)
    /* Port Data Input register mapping */
    #ifndef gSPI_DataAvailablePortDataINReg_c
    #define gSPI_DataAvailablePortDataINReg_c           GPIOB_PDIR
    #endif
    /* Port Data Output register mapping */
    #ifndef gSPI_DataAvailablePortDataOUTReg_c
    #define gSPI_DataAvailablePortDataOUTReg_c          GPIOB_PDOR
    #endif
    /* Port Data Direction register mapping */
    #ifndef gSPI_DataAvailablePortDDirReg_c
    #define gSPI_DataAvailablePortDDirReg_c             GPIOB_PDDR
    #endif
    /* Pin Control register mapping */
    #ifndef gSPI_DataAvailablePinCtrlReg_c
    #define gSPI_DataAvailablePinCtrlReg_c              CONCAT(PORTB_PCR,gSPI_DataAvailablePin_c)
    #endif
    /* SPI data available clock gating register */
    #ifndef gSPI_DataAvailablePortClkGatingReg_c
    #define gSPI_DataAvailablePortClkGatingReg_c        SIM_SCGC5
    #endif
    /* SPI data available clock gating mask */
    #ifndef gSPI_DataAvailablePortClkGatingBit_c
    #define gSPI_DataAvailablePortClkGatingBit_c        (1<<10)
    #endif
#elif (gSPI_DataAvailablePortDataReg_c == PORTC)
    /* Port Data Input register mapping */
    #ifndef gSPI_DataAvailablePortDataINReg_c
    #define gSPI_DataAvailablePortDataINReg_c           GPIOC_PDIR
    #endif
    /* Port Data Output register mapping */
    #ifndef gSPI_DataAvailablePortDataOUTReg_c
    #define gSPI_DataAvailablePortDataOUTReg_c          GPIOC_PDOR
    #endif
    /* Port Data Direction register mapping */
    #ifndef gSPI_DataAvailablePortDDirReg_c
    #define gSPI_DataAvailablePortDDirReg_c             GPIOC_PDDR
    #endif
    /* Pin Control register mapping */
    #ifndef gSPI_DataAvailablePinCtrlReg_c
    #define gSPI_DataAvailablePinCtrlReg_c              CONCAT(PORTC_PCR,gSPI_DataAvailablePin_c)
    #endif
    /* SPI data available clock gating register */
    #ifndef gSPI_DataAvailablePortClkGatingReg_c
    #define gSPI_DataAvailablePortClkGatingReg_c        SIM_SCGC5
    #endif
    /* SPI data available clock gating mask */
    #ifndef gSPI_DataAvailablePortClkGatingBit_c
    #define gSPI_DataAvailablePortClkGatingBit_c        (1<<11)
    #endif
#elif (gSPI_DataAvailablePortDataReg_c == PORTD)
    /* Port Data Input register mapping */
    #ifndef gSPI_DataAvailablePortDataINReg_c
    #define gSPI_DataAvailablePortDataINReg_c           GPIOD_PDIR
    #endif
    /* Port Data Output register mapping */
    #ifndef gSPI_DataAvailablePortDataOUTReg_c
    #define gSPI_DataAvailablePortDataOUTReg_c          GPIOD_PDOR
    #endif
    /* Port Data Direction register mapping */
    #ifndef gSPI_DataAvailablePortDDirReg_c
    #define gSPI_DataAvailablePortDDirReg_c             GPIOD_PDDR
    #endif
    /* Pin Control register mapping */
    #ifndef gSPI_DataAvailablePinCtrlReg_c
    #define gSPI_DataAvailablePinCtrlReg_c              CONCAT(PORTD_PCR,gSPI_DataAvailablePin_c)
    #endif
    /* SPI data available clock gating register */
    #ifndef gSPI_DataAvailablePortClkGatingReg_c
    #define gSPI_DataAvailablePortClkGatingReg_c        SIM_SCGC5
    #endif
    /* SPI data available clock gating mask */
    #ifndef gSPI_DataAvailablePortClkGatingBit_c
    #define gSPI_DataAvailablePortClkGatingBit_c        (1<<12)
    #endif
#elif (gSPI_DataAvailablePortDataReg_c == PORTE)
    /* Port Data Input register mapping */
    #ifndef gSPI_DataAvailablePortDataINReg_c
    #define gSPI_DataAvailablePortDataINReg_c           GPIOE_PDIR
    #endif
    /* Port Data Output register mapping */
    #ifndef gSPI_DataAvailablePortDataOUTReg_c
    #define gSPI_DataAvailablePortDataOUTReg_c          GPIOE_PDOR
    #endif
    /* Port Data Direction register mapping */
    #ifndef gSPI_DataAvailablePortDDirReg_c
    #define gSPI_DataAvailablePortDDirReg_c             GPIOE_PDDR
    #endif
    /* Pin Control register mapping */
    #ifndef gSPI_DataAvailablePinCtrlReg_c
    #define gSPI_DataAvailablePinCtrlReg_c              CONCAT(PORTE_PCR,gSPI_DataAvailablePin_c)
    #endif
    /* SPI data available clock gating register */
    #ifndef gSPI_DataAvailablePortClkGatingReg_c
    #define gSPI_DataAvailablePortClkGatingReg_c        SIM_SCGC5
    #endif
    /* SPI data available clock gating mask */
    #ifndef gSPI_DataAvailablePortClkGatingBit_c
    #define gSPI_DataAvailablePortClkGatingBit_c        (1<<13)
    #endif
#else
#error "Invalid port"    
#endif

/*
 * Name: SPI_SignalTxDataAvailable
 * Description: macro used to assert/clear the TX data available pin (SPI slave mode)
 */
#define SPI_SignalTxDataAvailable(bTrue) (bTrue) ? (gSPI_DataAvailablePortDataOUTReg_c |= gSPI_DataAvailablePinMask_c)\
    : (gSPI_DataAvailablePortDataOUTReg_c &= ~gSPI_DataAvailablePinMask_c)

/*
 * Name: SPI_IsTxDataAvailable
 * Description: macro that checks the SPI TX data available pin (slave side)
 */
#define SPI_IsTxDataAvailable()           ((gSPI_DataAvailablePortDataOUTReg_c & gSPI_DataAvailablePinMask_c))

/*
 * Name: SPI_IsRxDataAvailable
 * Description: macro that checks the SPI TX data available pin (master side)
 */
#define SPI_IsRxDataAvailable()           ((gSPI_DataAvailablePortDataINReg_c & gSPI_DataAvailablePinMask_c))

/*
 * Name: SPI_ConfigTxDataAvailablePin
 * Description: configure the SPI TX data available pin (slave side, set GPIO as output)
 */
#define SPI_ConfigTxDataAvailablePin() gSPI_DataAvailablePortDDirReg_c |= gSPI_DataAvailablePinMask_c; \
    gSPI_DataAvailablePortDataOUTReg_c &= ~gSPI_DataAvailablePinMask_c;

/*
 * Name: SPI_ConfigRxDataAvailablePin
 * Description: configure the SPI TX data available pin (slave side, set GPIO as input)
 */
#define SPI_ConfigRxDataAvailablePin() gSPI_DataAvailablePortDDirReg_c &= ~gSPI_DataAvailablePinMask_c;

#endif /* gSPI_Slave_TxDataAvailableSignal_Enable_d */

/*
 * Name: see below
 * Description: SPI registers mapping
 * Note: DO NOT EDIT
 */
#if (gSPI_ModuleUsed_d == SPI_0)
#define SPIx_MCR            SPI0_MCR
#define SPIx_TCR            SPI0_TCR
#define SPIx_CTAR0          SPI0_CTAR0
#define SPIx_CTAR0_SLAVE    SPI0_CTAR0_SLAVE
#define SPIx_CTAR1          SPI0_CTAR1
#define SPIx_SR             SPI0_SR
#define SPIx_RSER           SPI0_RSER
#define SPIx_PUSHR          SPI0_PUSHR
#define SPIx_PUSHR_SLAVE    SPI0_PUSHR_SLAVE
#define SPIx_POPR           SPI0_POPR
#define SPIx_TXFR0          SPI0_TXFR0
#define SPIx_TXFR1          SPI0_TXFR1
#define SPIx_TXFR2          SPI0_TXFR2
#define SPIx_TXFR3          SPI0_TXFR3
#define SPIx_RXFR0          SPI0_RXFR0
#define SPIx_RXFR1          SPI0_RXFR1
#define SPIx_RXFR2          SPI0_RXFR2
#define SPIx_RXFR3          SPI0_RXFR3
#elif (gSPI_ModuleUsed_d == SPI_1)
/* SPI1 */
#define SPIx_MCR            SPI1_MCR
#define SPIx_TCR            SPI1_TCR
#define SPIx_CTAR0          SPI1_CTAR0
#define SPIx_CTAR0_SLAVE    SPI1_CTAR0_SLAVE
#define SPIx_CTAR1          SPI1_CTAR1
#define SPIx_SR             SPI1_SR
#define SPIx_RSER           SPI1_RSER
#define SPIx_PUSHR          SPI1_PUSHR
#define SPIx_PUSHR_SLAVE    SPI1_PUSHR_SLAVE
#define SPIx_POPR           SPI1_POPR
#define SPIx_TXFR0          SPI1_TXFR0
#define SPIx_TXFR1          SPI1_TXFR1
#define SPIx_TXFR2          SPI1_TXFR2
#define SPIx_TXFR3          SPI1_TXFR3
#define SPIx_RXFR0          SPI1_RXFR0
#define SPIx_RXFR1          SPI1_RXFR1
#define SPIx_RXFR2          SPI1_RXFR2
#define SPIx_RXFR3          SPI1_RXFR3
#elif (gSPI_ModuleUsed_d == SPI_2)
/* SPI2 */
#define SPIx_MCR            SPI2_MCR
#define SPIx_TCR            SPI2_TCR
#define SPIx_CTAR0          SPI2_CTAR0
#define SPIx_CTAR0_SLAVE    SPI2_CTAR0_SLAVE
#define SPIx_CTAR1          SPI2_CTAR1
#define SPIx_SR             SPI2_SR
#define SPIx_RSER           SPI2_RSER
#define SPIx_PUSHR          SPI2_PUSHR
#define SPIx_PUSHR_SLAVE    SPI2_PUSHR_SLAVE
#define SPIx_POPR           SPI2_POPR
#define SPIx_TXFR0          SPI2_TXFR0
#define SPIx_TXFR1          SPI2_TXFR1
#define SPIx_TXFR2          SPI2_TXFR2
#define SPIx_TXFR3          SPI2_TXFR3
#define SPIx_RXFR0          SPI2_RXFR0
#define SPIx_RXFR1          SPI2_RXFR1
#define SPIx_RXFR2          SPI2_RXFR2
#define SPIx_RXFR3          SPI2_RXFR3
#else
#error "No valid SPI module defined"
#endif

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/*
 * Name: spiBaudrate_t
 * Description: SPI valid baudrates enumeration
 */
typedef enum spiBaudrate_tag
{
    gSPI_BaudRate_100000_c,
    gSPI_BaudRate_200000_c,
    gSPI_BaudRate_400000_c,
    gSPI_BaudRate_800000_c,
    gSPI_BaudRate_1000000_c,
    gSPI_BaudRate_2000000_c,
    gSPI_BaudRate_4000000_c,
    gSPI_BaudRate_8000000_c
} spiBaudrate_t;

/*
 * Name: spiPCSInactiveState_t
 * Description: SPI Peripheral Chip Select (PCS) inactive state data type.
 *              Used to configure the HW PCS.
 */
typedef union spiPCSInactiveState_tag
{
    uint8_t value;
    struct
    {
        unsigned int PCS0 : 1;
        unsigned int PCS1 : 1;
        unsigned int PCS2 : 1;
        unsigned int PCS3 : 1;
        unsigned int PCS4 : 1;
        unsigned int PCS5 : 1;
        unsigned int reserved_bits : 2;
    } bits;
} spiPCSInactiveState_t;

/*
 * Name: spiConfig_t
 * Description: Structure keeping the current SPI module configuration
 */
typedef struct spiConfig_tag
{
    uint8_t devMode;
    bool_t  continuousClkEn;
    bool_t  periphCSStrobeEn;
    bool_t  RxFifoOvflOverwriteEn;
    bool_t  continuousPCSEn;
    spiPCSInactiveState_t PCSInactiveState;
    bool_t  dozeEn;
    uint8_t frameSize;
    uint8_t clockPol;
    uint8_t clockPhase;
    uint8_t lsbFirst;
    uint8_t pcsToSckDelayPrescaler;
    uint8_t afterSckDelayPrescaler;
    uint8_t delayAfterTransferPrescaler;
    uint8_t pcsToSckDelayScaler;
    uint8_t afterSckSDelayScaler;
    uint8_t delayAfterTransferScaler;
    spiBaudrate_t baudRate;
} spiConfig_t;

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

#if (gSPI_Enabled_d == TRUE)

/*
 * Name: gSpiConfig
 * Description: Global variable keeping the current SPI module configuration
 * Valid ranges/values: see definition of spiConfig_t
 */
extern spiConfig_t gSpiConfig;

/*
 * Name: gSpiTaskId
 * Description: Id for SPI Task
 * Valid ranges/values: see definition of tsTaskID_t
 */
extern tsTaskID_t  gSpiTaskId;

#endif

/******************************************************************************
*******************************************************************************
* Public function prototypes
*******************************************************************************
******************************************************************************/

#if (gSPI_Enabled_d == TRUE)

/******************************************************************************
 * Name: SPI_Init
 * Description: Initialize the SPI module.
 *              Must be called before any further access.
 *              Set a default SPI module configuration.
 * Parameters: -
 * Return: TRUE if the initialization succeeded / FALSE otherwise
 ******************************************************************************/
extern bool_t SPI_Init
(
        void
);

/******************************************************************************
 * Name: SPI_Uninit
 * Description: Stop and disable the SPI module.
 * Parameters: -
 * Return: -
 ******************************************************************************/
extern void SPI_Uninit
(
        void
);

/******************************************************************************
 * Name: SPI_SetConfig
 * Description: Apply the SPI configuration pointed by 'pSpiConfig'.
 * Parameters: [IN ]pSpiConfig - pointer to an SPI configuration
 * Return: TRUE if configuration is successfully applied / FALSE otherwise
 ******************************************************************************/
extern bool_t SPI_SetConfig
(
        const spiConfig_t* pSpiConfig
);

/******************************************************************************
 * Name: SPI_GetConfig
 * Description: Retrieve the current SPI configuration and store it to a
 *              location pointed by 'pSpiConfig'.
 * Parameters: [OUT] pSpiConfig - pointer to SPI configuration location
 * Return: TRUE if configuration is successfully retrieved / FALSE otherwise
 ******************************************************************************/
extern bool_t SPI_GetConfig
(
        spiConfig_t* pSpiConfig
);

/******************************************************************************
 * Name: SPI_MasterTransmit
 * Description: SPI MASTER TX function
 * Parameters: [IN] pBuf - pointer to data to be send
 *             [IN] bufLen - buffer length
 *             [IN] pfCallback - SPI master TX callback function
 * Return: TRUE / FALSE
 ******************************************************************************/
extern bool_t SPI_MasterTransmit
(
        uint8_t *pBuf,
        index_t bufLen,
        void (*pfCallBack)(bool_t status)
);

/******************************************************************************
 * Name: SPI_MasterReceive
 * Description: SPI MASTER RX function
 * Parameters: [IN] pBuf - pointer to the location where received data shall
 *                         be stored
 *             [IN] bufLen - how many bytes to receive
 *             [IN] pfCallback - SPI master RX callback function
 * Return: TRUE / FALSE
 ******************************************************************************/
extern bool_t SPI_MasterReceive
(
        uint8_t *pBuf,
        index_t bufLen,
        void (*pfCallBack)(bool_t status)
);

/******************************************************************************
 * Name: SPI_SlaveTransmit
 * Description: SPI SLAVE RX function
 * Parameters: [IN] pBuf - pointer to the location where data to be sent
 *                         are stored
 *             [IN] bufLen - how many bytes to transmit
 *             [IN] pfCallback - SPI slave TX callback function
 * Return: TRUE / FALSE
 ******************************************************************************/
extern bool_t SPI_SlaveTransmit
(
        uint8_t *pBuf,
        index_t bufLen,
        void (*pfCallBack)(uint8_t *pBuf)
);

/******************************************************************************
 * Name: SPI_SetSlaveRxCallBack
 * Description: Set SPI slave RX callback
 * Parameters: [IN] pfCallBack - pointer to callback function
 * Return: -
 ******************************************************************************/
extern void SPI_SetSlaveRxCallBack
(
        void (*pfCallBack)(void)
);

/******************************************************************************
 * Name: SPI_IsSlaveTxActive
 * Description: check if the SPI slave transmission is active
 * Parameters: -
 * Return: TRUE if SPI slave transmission is in progress / FALSE otherwise
 ******************************************************************************/
extern bool_t SPI_IsSlaveTxActive
(
        void
);

/******************************************************************************
 * Name: SPI_GetByteFromBuffer
 * Description: Retrieve a byte from the driver's RX circular buffer and store
 *              it at *pDest.
 * Parameters: [OUT] pDest - pointer to a location where the retrieved byte
 *                           shall be placed.
 * Return: TRUE if a byte was retrieved / FALSE if the RX buffer is
 *         empty.
 ******************************************************************************/
extern bool_t SPI_GetByteFromBuffer
(
        uint8_t *pDst
);

/******************************************************************************
 * Name: SPI_ConfigPCS
 * Description: Configure the Peripheral Chip Select (PCS) pins
 * Parameters: [IN] CSNum - PCS number (check manual for details)
 *             [IN] strobeEn - strobe enabled / disabled for selected PCS pin
 *             [IN] inactiveHigh - the inactive state is high for selected PCS
 *                                 pin
 * Return: FALSE if invalid PCS is selected or if the SPI module is in running
 *         state / TRUE otherwise
 ******************************************************************************/
extern bool_t SPI_ConfigPCS
(
        uint8_t CSNum,
        bool_t strobeEn,
        bool_t inactiveHigh
);

/******************************************************************************
 * Name: SPI_GetSlaveRecvBytesCount
 * Description: Gets the number of bytes stored in the RX queue (slave mode)
 * Parameters: -
 * Return: RX queue entries count
 ******************************************************************************/
extern uint16_t SPI_GetSlaveRecvBytesCount
(
        void
);

/******************************************************************************
 * Name: SPI_SetContinuousPCSEn
 * Description: Gets the number of bytes stored in the RX queue (slave mode)
 * Parameters: -
 * Return: RX queue entries count 
 ******************************************************************************/
extern void SPI_SetContinuousPCSEn
(
    bool_t enable    
);

/******************************************************************************
 * Name: SPI_Isr
 * Description: SPI Interrupt Service Routine
 * Parameters: -
 * Return: -
 ******************************************************************************/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
extern void SPI_Isr
(
        void
);

#else
/* Stub functions */
#define SPI_Init()                                      FALSE
#define SPI_Uninit()
#define SPI_SetConfig(pSpiConfig)                       FALSE
#define SPI_GetConfig(pSpiConfig)                       FALSE
#define SPI_MasterTransmit(pBuf,bufLen,pfCallBack)      FALSE
#define SPI_MasterReceive(pBuf,bufLen,pfCallBack)       FALSE
#define SPI_SlaveTransmit(pBuf,bufLen,pfCallBack)       FALSE
#define SPI_SetSlaveRxCallBack(pfCallBack)
#define SPI_IsSlaveTxActive()                           FALSE
#define SPI_GetByteFromBuffer(pDst)                     FALSE
#define SPI_ConfigPCS(CSNum,strobeEn,inactiveHigh)      FALSE
#define SPI_GetSlaveRecvBytesCount()                    0
#define SPI_Isr                                         VECT_DefaultISR
#endif /* gSPI_Enabled_d */

#ifdef __cplusplus
}
#endif

#endif /* SPI_INTERFACE_H_ */
