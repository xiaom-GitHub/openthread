/**************************************************************************
* Filename: IIC_Interface.h
*
* Description: IIC export interface file for ARM CORTEX-M4 processor
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

#include "EmbeddedTypes.h"
#include "AppToPlatformConfig.h"
#include "TS_Interface.h"

#ifndef IIC_INTERFACE_H_
#define IIC_INTERFACE_H_

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
 ******************************************************************************
 * Public macros
 ******************************************************************************
 ******************************************************************************/

/* 
 * Name: gIIC_Enabled_d
 * Description: enable or disable the IIC module 
 */
#ifndef gIIC_Enabled_d
#define gIIC_Enabled_d                 FALSE
#endif

/*
 * Name: I2C_0
 * Description: mapping convention for I2C hardware module #0
 */
#define I2C_0                          0

/*
 * Name: I2C_1
 * Description: mapping convention for I2C hardware module #1
 */
#define I2C_1                          1

/*
 * Name: gI2C_USED_d
 * Description: The I2C hardware module used
 */
#ifndef gI2C_USED_d
#define gI2C_USED_d                    I2C_0
#endif

/*
 * Name: PORTx
 * Description: mapping macros
 */
#ifndef PORTA
#define PORTA                0
#endif
#ifndef PORTB
#define PORTB                1
#endif
#ifndef PORTC
#define PORTC                2
#endif
#ifndef PORTD
#define PORTD                3
#endif
#ifndef PORTE
#define PORTE                4
#endif

/*
 * Name: gIIC_Slave_TxDataAvailableSignal_Enable_c
 * Description: If TRUE, slave transmitter can signal to the master if there are data available 
 */
#ifndef gIIC_Slave_TxDataAvailableSignal_Enable_c
#define gIIC_Slave_TxDataAvailableSignal_Enable_c    TRUE
#endif

#if (gIIC_Slave_TxDataAvailableSignal_Enable_c)

/* Setup the Port Data register pin mask */
#ifndef gIIC_TxDataAvailablePin_c                           
#define gIIC_TxDataAvailablePin_c                    1
#endif

/* Setup the Pin Control register mask */
#ifndef gIIC_TxDataAvailablePinCtrlMask_c
#define gIIC_TxDataAvailablePinCtrlMask_c            (0x01000100UL) // (ISF=0, MUX=1)        
#endif

/* Setup the Pin Control register value */
#ifndef gIIC_TxDataAvailablePinCtrlValue_c
#define gIIC_TxDataAvailablePinCtrlValue_c           (0x0100UL)      
#endif

#ifndef gIIC_DataAvailablePortDataReg_c
#define gIIC_DataAvailablePortDataReg_c              PORTD /* default value */
#endif

#if (gIIC_DataAvailablePortDataReg_c == PORTA)
	/* Setup the Port Data register */
	#ifndef gIIC_TxDataAvailablePortDataReg_c
	#define gIIC_TxDataAvailablePortDataReg_c        GPIOA_PDOR
        #endif
	/* Setup the Port Data Direction register */
	#ifndef gIIC_TxDataAvailablePortDDirReg_c
	#define gIIC_TxDataAvailablePortDDirReg_c        GPIOA_PDDR
	#endif
	/* Setup the Pin Control register */
	#ifndef gIIC_TxDataAvailablePinCtrlReg_c
	#define gIIC_TxDataAvailablePinCtrlReg_c         CONCAT(PORTA_PCR,gIIC_TxDataAvailablePin_c)
	#endif
	/* Setup the System Clock Gating Control register */
	#ifndef gIIC_TxDataAvailablePortClkGatingReg_c
	#define gIIC_TxDataAvailablePortClkGatingReg_c   SIM_SCGC5
	#endif
	/* Setup the System Clock Gating Control register bit */
	#ifndef gIIC_TxDataAvailablePortClkGatingBit_c
	#define gIIC_TxDataAvailablePortClkGatingBit_c   (1<<9)
	#endif
#elif (gIIC_DataAvailablePortDataReg_c == PORTB)
	/* Setup the Port Data register */
	#ifndef gIIC_TxDataAvailablePortDataReg_c
	#define gIIC_TxDataAvailablePortDataReg_c        GPIOB_PDOR 
        #endif
	/* Setup the Port Data Direction register */
	#ifndef gIIC_TxDataAvailablePortDDirReg_c
	#define gIIC_TxDataAvailablePortDDirReg_c        GPIOB_PDDR
	#endif
	/* Setup the Pin Control register */
	#ifndef gIIC_TxDataAvailablePinCtrlReg_c
	#define gIIC_TxDataAvailablePinCtrlReg_c         CONCAT(PORTB_PCR,gIIC_TxDataAvailablePin_c)
	#endif
	/* Setup the System Clock Gating Control register */
	#ifndef gIIC_TxDataAvailablePortClkGatingReg_c
	#define gIIC_TxDataAvailablePortClkGatingReg_c   SIM_SCGC5
	#endif
	/* Setup the System Clock Gating Control register bit */
	#ifndef gIIC_TxDataAvailablePortClkGatingBit_c
	#define gIIC_TxDataAvailablePortClkGatingBit_c   (1<<10)
	#endif
#elif (gIIC_DataAvailablePortDataReg_c == PORTC)
	/* Setup the Port Data register */
	#ifndef gIIC_TxDataAvailablePortDataReg_c
	#define gIIC_TxDataAvailablePortDataReg_c        GPIOC_PDOR 
        #endif
	/* Setup the Port Data Direction register */
	#ifndef gIIC_TxDataAvailablePortDDirReg_c
	#define gIIC_TxDataAvailablePortDDirReg_c        GPIOC_PDDR
	#endif
	/* Setup the Pin Control register */
	#ifndef gIIC_TxDataAvailablePinCtrlReg_c
	#define gIIC_TxDataAvailablePinCtrlReg_c         CONCAT(PORTC_PCR,gIIC_TxDataAvailablePin_c)
	#endif
	/* Setup the System Clock Gating Control register */
	#ifndef gIIC_TxDataAvailablePortClkGatingReg_c
	#define gIIC_TxDataAvailablePortClkGatingReg_c   SIM_SCGC5
	#endif
	/* Setup the System Clock Gating Control register bit */
	#ifndef gIIC_TxDataAvailablePortClkGatingBit_c
	#define gIIC_TxDataAvailablePortClkGatingBit_c   (1<<11)
	#endif
#elif (gIIC_DataAvailablePortDataReg_c == PORTD)
	/* Setup the Port Data register */
	#ifndef gIIC_TxDataAvailablePortDataReg_c
	#define gIIC_TxDataAvailablePortDataReg_c        GPIOD_PDOR  	
        #endif
	/* Setup the Port Data Direction register */
	#ifndef gIIC_TxDataAvailablePortDDirReg_c
	#define gIIC_TxDataAvailablePortDDirReg_c        GPIOD_PDDR
	#endif
	/* Setup the Pin Control register */
	#ifndef gIIC_TxDataAvailablePinCtrlReg_c
	#define gIIC_TxDataAvailablePinCtrlReg_c         CONCAT(PORTD_PCR,gIIC_TxDataAvailablePin_c)
	#endif
	/* Setup the System Clock Gating Control register */
	#ifndef gIIC_TxDataAvailablePortClkGatingReg_c
	#define gIIC_TxDataAvailablePortClkGatingReg_c   SIM_SCGC5
	#endif
	/* Setup the System Clock Gating Control register bit */
	#ifndef gIIC_TxDataAvailablePortClkGatingBit_c
	#define gIIC_TxDataAvailablePortClkGatingBit_c   (1<<12)
	#endif
#elif (gIIC_DataAvailablePortDataReg_c == PORTE)
	/* Setup the Port Data register */
	#ifndef gIIC_TxDataAvailablePortDataReg_c
	#define gIIC_TxDataAvailablePortDataReg_c        GPIOE_PDOR 
        #endif
	/* Setup the Port Data Direction register */
	#ifndef gIIC_TxDataAvailablePortDDirReg_c
	#define gIIC_TxDataAvailablePortDDirReg_c        GPIOE_PDDR
	#endif
	/* Setup the Pin Control register */
	#ifndef gIIC_TxDataAvailablePinCtrlReg_c
	#define gIIC_TxDataAvailablePinCtrlReg_c         CONCAT(PORTE_PCR,gIIC_TxDataAvailablePin_c)
	#endif
	/* Setup the System Clock Gating Control register */
	#ifndef gIIC_TxDataAvailablePortClkGatingReg_c
	#define gIIC_TxDataAvailablePortClkGatingReg_c   SIM_SCGC5
	#endif
	/* Setup the System Clock Gating Control register bit */
	#ifndef gIIC_TxDataAvailablePortClkGatingBit_c
	#define gIIC_TxDataAvailablePortClkGatingBit_c   (1<<13)
	#endif
#else
#error "Invalid port"	
#endif

#endif /* gIIC_Slave_TxDataAvailableSignal_Enable_c */

/*
 * Name: gIIC_FrequencyDivider_X_c
 * Description: I2C Frequency Divider register value
 *              For a proper setup of this register, consult the manual.
 * Note: use one of these defines when passing the parameter to IIC_SetBaudRate() function.                              
 */
#if(gSystemClock_c == gSystemClk48MHz_c)
#define gIIC_FrequencyDivider_50000_c                (uint8_t)((0<<6)| 0x33) /* 53571.4 */
#define gIIC_FrequencyDivider_100000_c               (uint8_t)((0<<6)| 0x27) /* 100000.0 */
#define gIIC_FrequencyDivider_200000_c               (uint8_t)((0<<6)| 0x22) /* 214285.7 */
#define gIIC_FrequencyDivider_400000_c               (uint8_t)((0<<6)| 0x1A) /* 428571.4 */
#else
#warning "System clock must be set to 48 MHz"
#endif

#define  gIIC_BaudRate_50000_c			             gIIC_FrequencyDivider_50000_c
#define  gIIC_BaudRate_100000_c   		             gIIC_FrequencyDivider_100000_c
#define  gIIC_BaudRate_200000_c   		             gIIC_FrequencyDivider_200000_c
#define  gIIC_BaudRate_400000_c   		             gIIC_FrequencyDivider_400000_c

/*
 * Name: gIIC_DefaultBaudRate_c
 * Description: Default baud rate 
 */
#ifndef gIIC_DefaultBaudRate_c
#define gIIC_DefaultBaudRate_c  		             gIIC_BaudRate_100000_c
#endif

/*
 * Name: gIIC_DefaultSlaveAddress_c
 * Description: The I2C slave address 
 */
#ifndef gIIC_DefaultSlaveAddress_c
#define gIIC_DefaultSlaveAddress_c   			     0x76
#endif

#if((gIIC_DefaultSlaveAddress_c > 0x7f) || \
		(((gIIC_DefaultSlaveAddress_c & 0x78) == 0) && ((gIIC_DefaultSlaveAddress_c & 0x07) != 0)) || \
		((gIIC_DefaultSlaveAddress_c & 0x78) == 0x78))
#error Illegal Slave Address!!!
#endif

/*
 * Name: gI2C_InterruptPriority_c
 * Description: I2C IRQ priority
 */
#define gI2C_InterruptPriority_c  				     10

/*
 * Name: gIIC_SlaveTransmitBuffersNo_c
 * Description: Number of entries in the transmit-buffers-in-waiting list 
 */
#ifndef gIIC_SlaveTransmitBuffersNo_c
#define gIIC_SlaveTransmitBuffersNo_c      		     3
#endif

/*
 * Name: gIIC_SlaveReceiveBufferSize_c
 * Description: Size of the driver's RX circular buffer. This buffers is used to
 *              hold received bytes until the application can retrieve them via the
 *              IIcX_GetBytesFromRxBuffer() functions, and are not otherwise accessible
 *              from outside the driver. The size does not need to be a power of two.
 */
#ifndef gIIC_SlaveReceiveBufferSize_c
#define gIIC_SlaveReceiveBufferSize_c			     128
#endif

/******************************************************************************
 *******************************************************************************
 * Public type definitions
 *******************************************************************************
 ******************************************************************************/

/* none */

/******************************************************************************
 *******************************************************************************
 * Public memory declarations
 *******************************************************************************
 ******************************************************************************/

#if gIIC_Enabled_d

/*
 * Name: gIIcTaskId
 * Description: ID for IIC task 
 * Valid ranges: see definition of tsTaskID_t
 */
extern tsTaskID_t    gIIcTaskId;

/******************************************************************************
 *******************************************************************************
 * Public function prototypes
 *******************************************************************************
 ******************************************************************************/

/******************************************************************************
 * Name: IIC_ModuleInit
 * Description: Initialize the I2C module. 
 *              Must be called before any further access
 * Parameters: -
 * Return: -
 ******************************************************************************/
extern void IIC_ModuleInit
(
		void
);

/******************************************************************************
 * Name: IIC_ModuleUninit
 * Description: Used to disable the I2C module. 
 * Parameters: -
 * Return: -
 ******************************************************************************/
extern void IIC_ModuleUninit
(
		void
);

/******************************************************************************
 * Name: IIC_SetBaudRate
 * Description: set the baudrate for the I2C module. 
 * Parameters: -
 * Return: -
 ******************************************************************************/
extern bool_t IIC_SetBaudRate
(
		uint8_t baudRate
);

/******************************************************************************
 * Name: IIC_SetSlaveAddress
 * Description: Set the IIC module slave address 
 * Parameters: [IN] slaveAddress - the slave address
 * Return: -
 ******************************************************************************/
extern bool_t IIC_SetSlaveAddress
(
		uint8_t slaveAddress
);

/******************************************************************************
 * Name: IIC_BusRecovery
 * Description: The function is called to recover the I2C bus.
 *              Force the I2C module to become the I2C bus master and drive 
 *              the SCL signal (even though SDA may already be driven, 
 *              which indicates that the bus is busy)
 * Parameters: -
 * Return: gI2cErrModuleIsDis_c - if I2C module is disabled
 *         gI2cErrNoError_c - if no error occurred
 ******************************************************************************/
extern void IIC_BusRecovery
(
		void
);

/******************************************************************************
 * Name: IIC_SetSlaveRxCallBack
 * Description: Sets a pointer to the function to be called whenever a byte 
 *              is received. If the pointer is set to NULL clear the RX buffer.
 * Parameters: [IN] pfCallBack - callback function
 * Return: 
 ******************************************************************************/
extern void IIC_SetSlaveRxCallBack
(
		void (*pfCallBack)(void)
);

/******************************************************************************
 * Name: IIC_Transmit_Slave
 * Description: Begin transmitting (as SLAVE) of data from *pBuffer
 * Parameters: [IN] pBuf - pointer to data to be transmitted
 *             [IN] bufLen - the length of the buffer
 *             [IN] pfCallBack - pointer to the callback function  
 * Return: Returns FALSE if there are no more slots in the buffer reference 
 *         table
 ******************************************************************************/
extern bool_t IIC_Transmit_Slave
(
		uint8_t const *pBuf, 
		index_t bufLen,
		void (*pfCallBack)(uint8_t const *pBuf)
);

/******************************************************************************
 * Name: IIC_Transmit_Master
 * Description: Begin transmitting (as MASTER) of data from *pBuffer
 * Parameters: [IN] pBuf - pointer to data to be transmitted
 *             [IN] bufLen - the length of the buffer
 *             [IN] destAddress - the slave destination address
 *             [IN] pfCallBack - pointer to the callback function
 * Return: Returns FALSE if there are no more slots in the buffer reference 
 *         table
 ******************************************************************************/
extern bool_t IIC_Transmit_Master
(
		uint8_t const *pBuf,
		index_t bufLen, 
		uint8_t destAddress, 
		void (*pfCallBack)(bool_t status)
);

/******************************************************************************
 * Name: IIC_Receive_Master
 * Description: receive data (as MASTER) from the specified SLAVE
 * Parameters: [IN] pBuf - pointer to destination buffer
 *             [IN] bufLen - the length of the buffer
 *             [IN] destAddress - the slave destination address
 *             [IN] pfCallBack - pointer to the callback function  
 * Return: 
 ******************************************************************************/
extern bool_t IIC_Receive_Master
(
		uint8_t *pBuf, 
		index_t bufLen, 
		uint8_t destAddress, 
		void (*pfCallBack)(bool_t status)
);

/******************************************************************************
 * Name: IIC_IsSlaveTxActive
 * Description: checks if there are still data to be transmitted to the master
 * Parameters: -
 * Return: Returns TRUE if there are still data to be transmitted to the master
 *         Returns FALSE if nothing left to transmit
 ******************************************************************************/
extern bool_t IIC_IsSlaveTxActive
(
		void
);

/******************************************************************************
 * Name: IIC_TxDataAvailable
 * Description: Depending on the passed parameter value, the master will be   
 *              signaled that it needs to read data from the slave
 * Parameters: [IN]bIsAvailable - if TRUE, the master is signaled that new
 *                                data is available and shall be read 
 * Return: -
 ******************************************************************************/
extern void IIC_TxDataAvailable
(
		bool_t bIsAvailable
);

/******************************************************************************
 * Name: IIC_GetByteFromRxBuffer
 * Description: Retrieves a byte from the driver's RX buffer and store it 
 *              at *pDst.
 * Parameters: [IN]pDst - pointer to a memory location where the read byte  
 *                        shall be stored
 * Return: TRUE if a byte was retrieved; FALSE if the RX buffer is empty.
 ******************************************************************************/
extern bool_t IIC_GetByteFromRxBuffer
(
		uint8_t *pDst
);

/******************************************************************************
 * Name: IIC_Task
 * Description: Main task of the I2C module
 * Parameters: [IN]events - see I2C task events
 * Return: -
 ******************************************************************************/
extern void IIC_Task
(
		event_t events
);

/******************************************************************************
 * Name: IIC_GetRxBytesCount
 * Description: Gets the number of bytes received and stored in the RX queue
 * Parameters: -
 * Return: RX queue entries count 
 ******************************************************************************/
extern uint16_t IIC_GetRxBytesCount
(
		void
);

/******************************************************************************
 * Name: I2c_Isr
 * Description: I2C interrupt service routine
 * Parameters: -
 * Return: -
 ******************************************************************************/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
extern void IIC_Isr
(
		void
);

#else
/* Stub functions */
#define IIC_ModuleInit()
#define IIC_ModuleUninit()
#define IIC_SetBaudRate(baudRate)                                       FALSE
#define IIC_SetSlaveAddress(slaveAddress)                               FALSE
#define IIC_BusRecovery()
#define IIC_SetSlaveRxCallBack(pfCallBack)
#define IIC_Transmit_Slave(pBuf,bufLen,pfCallBack)                      FALSE
#define IIC_Transmit_Master(pBuf,bufLen,destAddress,pfCallBack)         FALSE
#define IIC_Receive_Master(pBuf,bufLen,destAddress,pfCallBack)          FALSE
#define IIC_IsSlaveTxActive()                                           FALSE
#define IIC_TxDataAvailable(bIsAvailable)
#define IIC_GetByteFromRxBuffer(pDst)                                   FALSE
#define IIC_Task(events)
#define IIC_GetRxBytesCount()                                           0
#define IIC_Isr()                                                       VECT_DefaultISR

#endif /* gIIC_Enabled_d */

#ifdef __cplusplus
}
#endif

#endif /* IIC_INTERFACE_H_ */

