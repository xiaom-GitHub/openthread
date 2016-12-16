/**************************************************************************
* Filename: IIC.h
*
* Description: IIC header file for ARM CORTEX-M4 processor
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

#ifndef IIC_H_
#define IIC_H_

#include "IIC_Interface.h"
#include "PortConfig.h"

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/***************************************************************** 
 *                    MCU SPECIFIC DEFINITIONS                   *
 *****************************************************************/
#if ((MCU_MK60D10 == 1) || (MCU_MK60N512VMD100 == 1) || (MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
    #if (gI2C_USED_d == I2C_0)
	#define gI2C_IRQ_NUM_c                           24                 /* I2C0 IRQ number */
	#define gI2C_SIM_SCGC_c                          SIM_SCGC4			/* I2C0 system clock gating control register */
	#define gI2C_SIM_SCGC_BIT_c                      (1<<6)     		/* I2C0 system clock gating control bit position */
	#else
	#define gI2C_IRQ_NUM_c                           25                 /* I2C1 IRQ number */
	#define gI2C_SIM_SCGC_c                          SIM_SCGC4			/* I2C1 system clock gating control register */
	#define gI2C_SIM_SCGC_BIT_c                      (1<<7)      		/* I2C1 system clock gating control bit position */
	#endif
#elif (MCU_MK20D5 == 1)
	#if (gI2C_USED_d == I2C_0)
	#define gI2C_IRQ_NUM_c                           11                 /* I2C0 IRQ number */
	#define gI2C_SIM_SCGC_c                          SIM_SCGC4			/* I2C0 system clock gating control register */
	#define gI2C_SIM_SCGC_BIT_c                      (1<<6)     		/* I2C0 system clock gating control bit position */
	#else
	#error "I2C_1 is not available for the selected target MCU"
	#endif   
#endif

/*
 * Name: see section below
 * Description: I2C registers mapping
 */
#if (gI2C_USED_d == I2C_0)
#define I2Cx_A1     I2C0_A1
#define I2Cx_F      I2C0_F
#define I2Cx_C1     I2C0_C1
#define I2Cx_S      I2C0_S
#define I2Cx_D      I2C0_D
#define I2Cx_C2     I2C0_C2
#define I2Cx_FLT    I2C0_FLT
#define I2Cx_RA     I2C0_RA
#define I2Cx_SMB    I2C0_SMB
#define I2Cx_A2     I2C0_A2
#define I2Cx_SLTH   I2C0_SLTH
#define I2Cx_SLTL   I2C0_SLTL
#else 
#define I2Cx_A1     I2C1_A1
#define I2Cx_F      I2C1_F
#define I2Cx_C1     I2C1_C1
#define I2Cx_S      I2C1_S
#define I2Cx_D      I2C1_D
#define I2Cx_C2     I2C1_C2
#define I2Cx_FLT    I2C1_FLT
#define I2Cx_RA     I2C1_RA
#define I2Cx_SMB    I2C1_SMB
#define I2Cx_A2     I2C1_A2
#define I2Cx_SLTH   I2C1_SLTH
#define I2Cx_SLTL   I2C1_SLTL
#endif


/*
 * Name: see section below
 * Description: I2C 'easy access' macros
 */
#define I2cBusBusy()              		(I2Cx_S & I2C_S_BUSY_MASK)
#define I2cSendStart()           		(I2Cx_C1 |= I2C_C1_MST_MASK)
#define I2cSendStop()             		(I2Cx_C1 &= ~I2C_C1_MST_MASK)
#define I2cSendRepStart()         		(I2Cx_C1 |= I2C_C1_RSTA_MASK)
#define I2cArbitrationLost()      		(I2Cx_S & I2C_S_ARBL_MASK)
#define I2cALClear()              		(I2Cx_S &= ~I2C_S_ARBL_MASK)
#define I2cMIFClear()             		(I2Cx_S &= ~I2C_S_IICIF_MASK)
#define I2cWaitForMCF()           		({while(!(I2Cx_S & I2C_S_TCF_MASK)){}})
#define I2cWaitForMIF()           		({while(!(I2Cx_S & I2C_S_IICIF_MASK)){}})
#define I2cSendAck(ackStatus)     		(ackStatus == gI2cAckResponse_c) ? (I2Cx_C1 &= ~I2C_C1_TXAK_MASK) : (I2Cx_C1 |= I2C_C1_TXAK_MASK)
#define I2cReceivedAck()          		(!(I2Cx_S & I2C_S_RXAK_MASK))
#define I2cSendByte(byteData)     		I2Cx_D = (byteData)
#define I2cReceiveByte(byteData)  		*(byteData) = I2Cx_D

/*
 * Name: IIC_IsTxDataAvailable
 * Description: Verify if there is data to be received from the Slave
 */
#if gIIC_Slave_TxDataAvailableSignal_Enable_c
#define IIC_IsTxDataAvailable()  ((gIIC_TxDataAvailablePortDataReg_c & (1<<gIIC_TxDataAvailablePin_c)) == 0)
#endif

/*
 * Name: see section below
 * Description: Events for IIC task
 */
#define gIIC_Event_SlaveRx_c          	(1<<2)
#define gIIC_Event_SlaveTx_c          	(1<<3)
#define gIIC_Event_MasterRxFail_c     	(1<<4)
#define gIIC_Event_MasterRxSuccess_c  	(1<<5)
#define gIIC_Event_MasterTxFail_c     	(1<<6)
#define gIIC_Event_MasterTxSuccess_c  	(1<<7)

/*
 * Name: NumberOfElements
 * Description: Macro that returns the elements count within a specified array 
 */
#ifndef NumberOfElements
#define NumberOfElements(array)     	((sizeof(array) / (sizeof(array[0]))))
#endif


/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/*
 * Name: IIcTxBufRef_t
 * Description: TX buffer reference data type
 */
typedef struct IcTxBufRef_tag 
{
	unsigned char const *pBuf;
	void (*pfCallBack)(unsigned char const *pBuf);
} IIC_TxBufRef_t;

/*
 * Name: IIC_OpType_t
 * Description: data type for I2C operations (RX/TX)
 */
typedef enum
{
	mIIC_OpType_Tx_c,
	mIIC_OpType_Rx_c
} IIC_OpType_t;

/*
 * Name: IIC_MasterOp_t
 * Description: data type for I2C master operations (RX/TX)and afferent data
 */
typedef struct iicMasterOp_tag 
{
	IIC_OpType_t iicOpType;
	uint8_t *pBuf;
	uint8_t bufLen;
} IIC_MasterOp_t;

/*
 * Name: IIC_CQueue_t
 * Description: Circular queue data type definition
 */
typedef struct tag_IICQueue
{
	uint8_t		QData[gIIC_SlaveReceiveBufferSize_c];  /* queue data */
	index_t		Head;	/* read index */
	index_t		Tail;	/* write index */
	uint16_t	EntriesCount; /* entries count */
} IIC_CQueue_t;

#ifdef __cplusplus
}
#endif

#endif /* IIC_H_ */
