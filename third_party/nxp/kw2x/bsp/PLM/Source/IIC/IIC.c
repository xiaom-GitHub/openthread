/**************************************************************************
* Filename: IIC.c
*
* Description: IIC implementation file for ARM CORTEX-M4 processor
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
#include "IIC.h"
#include "NVIC.h"

#if gMacStandAlone_d
#include "Mac_Globals.h"
#endif

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
*******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
*******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private memory definitions
*******************************************************************************
*******************************************************************************/

#if gIIC_Enabled_d

/*
 * Name: maIIcTxBufRefTable
 * Description: TX buffer reference table
 * Valid ranges/values: see definition of IIC_TxBufRef_t
 */
static IIC_TxBufRef_t    maIIcTxBufRefTable[gIIC_SlaveTransmitBuffersNo_c];


/*
 * Name: maIIcTxBufLenTable
 * Description: table for TX buffer lengths
 * Valid ranges/values: - 
 */
static index_t    		maIIcTxBufLenTable[gIIC_SlaveTransmitBuffersNo_c];


/*
 * Name: mIIcTxCurIndex
 * Description: index in the TX buffer that is currently transmitted
 * Valid ranges/values: 0..255
 */
static index_t        	mIIcTxCurIndex;


/*
 * Name: 
 * Description: TX buffer reference table leading index.
 *              The leading index is the next position to store 
 *              a buffer reference.
 * Valid ranges/values: 0..255
 */
static index_t        	mIIcTxBufRefLeadingIndex;    /* Post-increment. */


/*
 * Name: mIIcTxBufRefTrailingIndex
 * Description: TX buffer reference table trailing index
 *              The trailing index is the buffer currently being transmitted.
 * Valid ranges/values: 0..255
 */
static index_t        	mIIcTxBufRefTrailingIndex;   /* Post-increment. */


/*
 * Name: maIIcTxCallbackTable
 * Description: TX callback table
 * Valid ranges/values: see definition of IIC_TxBufRef_t
 */
static IIC_TxBufRef_t  	maIIcTxCallbackTable[gIIC_SlaveTransmitBuffersNo_c];


/*
 * Name: maIIcTxCallbackLeadingIndex
 * Description: leading index in TX callback table
 * Valid ranges/values: 0..255
 */
static index_t        	maIIcTxCallbackLeadingIndex = 0;


/*
 * Name: maIIcTxCallbackTrailingIndex
 * Description: trailing index in TX callback table
 * Valid ranges/values: 0..255
 */
static index_t        	maIIcTxCallbackTrailingIndex = 0; 

/*
 * Name: mIIcRxBuf
 * Description: I2C RX circular buffer
 * Valid ranges/values: -
 */
static IIC_CQueue_t		mIIcRxBuf;

/*
 * Name: pfIIcSlaveRxCallBack
 * Description: Local variable to keep the I2C SLAVE RX callback provided by the application 
 * Valid ranges/values: -
 */
static void           	(*pfIIcSlaveRxCallBack)(void);

/*
 * Name: pfIIcMasterRxCallBack
 * Description: Local variable to keep the I2C MASTER RX callback provided by the application
 * Valid ranges/values: 
 */
static void           	(*pfIIcMasterRxCallBack)(bool_t status);

/*
 * Name: pfIIcMasterTxCallBack
 * Description: Local variable to keep the I2C MASTER TX callback provided by the application
 * Valid ranges/values: 
 */
static void           	(*pfIIcMasterTxCallBack)(bool_t status);

/*
 * Name: mIICMasterOp
 * Description: data structure that keeps information about I2C master transfer type, data buffer
 *              pointers and data length.
 * Valid ranges/values: see definition of iicMasterOp_t
 */
static IIC_MasterOp_t   	mIICMasterOp ;

#endif

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
*******************************************************************************/

#if gIIC_Enabled_d

/*
 * Name: gIIcTaskId
 * Description: ID for IIC task 
 * Valid ranges: see definition of tsTaskID_t
 */
tsTaskID_t    gIIcTaskId;

#endif

/******************************************************************************
*******************************************************************************
* Private function prototypes
*******************************************************************************
*******************************************************************************/

#if gIIC_Enabled_d

/******************************************************************************
 * Name: IIC_TaskInit
 * Description: creates the I2C module task
 * Parameters: -
 * Return: -
 ******************************************************************************/
static void IIC_TaskInit
(
		void
);


/******************************************************************************
 * Name: IIC_SendNextByte
 * Description: Send next byte of information from the Tx buffer
 * Parameters: -
 * Return: -
 ******************************************************************************/
static void IIC_SendNextByte
(
		void
);

/******************************************************************************
 * Name: I2cInitHW
 * Description: Initialize the I2C hardware module and associated registers
 * Parameters: -
 * Return: -
 ******************************************************************************/
static void I2cInitHW
(
		void
);

/******************************************************************************
 * Name: IIC_InitQueue
 * Description: Initialize the IIC RX queue
 * Parameters: [IN] pQueue - pointer to queue
 * Return: TRUE if the pointer is valid, FALSE otherwise
 ******************************************************************************/
static bool_t IIC_InitQueue
(
		IIC_CQueue_t *pQueue
);

/******************************************************************************
 * Name: IIC_PushData
 * Description: Add a new element to the RX queue
 * Parameters: [IN] pQueue - pointer to queue
 *         [IN] data - data to be added  
 * Return: TRUE if the push operation succeeded, FALSE otherwise
 ******************************************************************************/
static bool_t IIC_PushData
(
		IIC_CQueue_t *pQueue, 
		unsigned char data
);

/******************************************************************************
 * Name: IIC_PopData
 * Description: Retrieves the head element from the RX queue
 * Parameters: [IN] pQueue - pointer to queue
 *         [OUT] pData - pointer to the location where data shall be placed
 * Return: TRUE if the pop operation succeeded, FALSE otherwise
 ******************************************************************************/
static bool_t IIC_PopData
(
		IIC_CQueue_t *pQueue, 
		unsigned char *pData
);

#endif

/******************************************************************************
*******************************************************************************
* Private functions 
*******************************************************************************
*******************************************************************************/

#if gIIC_Enabled_d

/******************************************************************************
 * Name: IIC_TaskInit
 * Description: creates the I2C module task
 * Parameters: -
 * Return: -
 ******************************************************************************/
static void IIC_TaskInit
(
		void
)
{
	gIIcTaskId = TS_CreateTask(gTsI2CTaskPriority_c, IIC_Task);
}



/******************************************************************************
 * Name: IIC_SendNextByte
 * Description: Send next byte of information from the Tx buffer
 * Parameters: -
 * Return: -
 ******************************************************************************/
static void IIC_SendNextByte
(
		void
)
{
	void (*pfCallBack)(unsigned char const *pBuf);

#if gIIC_Slave_TxDataAvailableSignal_Enable_c  
	if(IIC_IsTxDataAvailable())
#else
		if(maIIcTxBufLenTable[mIIcTxBufRefTrailingIndex])  
#endif  
		{
			/* Write a byte. */
			I2C0_D = maIIcTxBufRefTable[mIIcTxBufRefTrailingIndex].pBuf[mIIcTxCurIndex];

			/* Finished with this buffer? */
			if (++mIIcTxCurIndex >= maIIcTxBufLenTable[mIIcTxBufRefTrailingIndex]) 
			{

				/* Mark this one as done, and call the callback. */
				maIIcTxBufLenTable[mIIcTxBufRefTrailingIndex] = 0;
				pfCallBack = maIIcTxBufRefTable[mIIcTxBufRefTrailingIndex].pfCallBack;
				if (pfCallBack)
				{
					/* Signal the IIC task that we got a callback to be executed */
					TS_SendEvent(gIIcTaskId, gIIC_Event_SlaveTx_c);
					/* Add callback information to the callback table */
					maIIcTxCallbackTable[maIIcTxCallbackLeadingIndex].pfCallBack = pfCallBack;
					maIIcTxCallbackTable[maIIcTxCallbackLeadingIndex].pBuf = maIIcTxBufRefTable[mIIcTxBufRefTrailingIndex].pBuf;
					/* Increment and wrap around the leading index */
					if (++maIIcTxCallbackLeadingIndex >= NumberOfElements(maIIcTxCallbackTable)) {
						maIIcTxCallbackLeadingIndex = 0;
					}

				}
				/* Increment to the next buffer. */
				mIIcTxCurIndex = 0;
				if (++mIIcTxBufRefTrailingIndex >= NumberOfElements(maIIcTxBufRefTable)) 
				{
					mIIcTxBufRefTrailingIndex = 0;
				}

#if gIIC_Slave_TxDataAvailableSignal_Enable_c        
				/* If there is no more data to send, turn off the transmit interrupt. */
				if (!maIIcTxBufLenTable[mIIcTxBufRefTrailingIndex]) 
				{
					/* Signal to host that there are no more data to receive */
					IIC_TxDataAvailable(FALSE);
				}
#endif      
			}
		}
		else
		{
			/* Write a byte. */
			I2C0_D = 0;
		}	
}

/******************************************************************************
 * Name: I2cInitHW
 * Description: Initialize the I2C hardware module and the associated 
 *              registers
 * Parameters: -
 * Return: -
 ******************************************************************************/
static void I2cInitHW
(
		void
)
{   
	/* enable clock gating for I2C port in use */
	gI2C_PORT_SIM_SCG_c |= gI2C_PORT_SIM_SGC_BIT_c;

	/* setup Pin Control Register (PCR) for SCL pin */
	gI2C_SCL_PCR_c |= PORT_PCR_MUX(gI2C_SCL_ALT_c);

	/* setup Pin Control Register (PCR) for SDA pin */
	gI2C_SDA_PCR_c |= PORT_PCR_MUX(gI2C_SDA_ALT_c);    

	/* enable clock gating for I2C module */
	gI2C_SIM_SCGC_c |= gI2C_SIM_SCGC_BIT_c;

	/* clear address register */
	I2Cx_A1 &= ~I2C_A1_AD_MASK;    
	/* enable I2C module */
	I2Cx_C1 |= I2C_C1_IICEN_MASK;
	/* default: slave mode */
	I2Cx_C1 &= ~I2C_C1_MST_MASK;
	/* default: receiver */
	I2Cx_C1 &= ~I2C_C1_TX_MASK;
	/* clear status register */
	I2Cx_S = 0x00;
	/* clear data register */
	I2C0_D = 0x00;
	/* default: general call disabled */    
	I2Cx_C2 &= ~I2C_C2_GCAEN_MASK;
	/* default: 7-bit address scheme */
	I2Cx_C2 &= ~I2C_C2_ADEXT_MASK;
	/* set default address */
	IIC_SetSlaveAddress(gIIC_DefaultSlaveAddress_c);
	/* default: normal drive mode */
	I2Cx_C2 &= ~I2C_C2_HDRS_MASK;
	/* default: slave baud rate follows the master ones */
	I2Cx_C2 &= ~I2C_C2_SBRC_MASK;
	/* default: range address disabled */
	I2Cx_C2 &= ~I2C_C2_RMEN_MASK;
	/* default: glitch filter bypass */
	I2Cx_FLT &= ~I2C_FLT_FLT_MASK;    
	/* setup the default I2C baud rate */
	I2Cx_F = gIIC_DefaultBaudRate_c;
	/* enable I2C interrupt requests */
	I2Cx_C1 |= I2C_C1_IICIE_MASK;

	/* Enable I2C interrupt within the Nested Vector Interrupt Controller (NVIC)  */
	NVIC_EnableIRQ(gI2C_IRQ_NUM_c);
	/* Set I2C IRQ priority */
	NVIC_SetPriority(gI2C_IRQ_NUM_c, gI2C_InterruptPriority_c);

#if  gIIC_Slave_TxDataAvailableSignal_Enable_c
	/* Configure as output the GPIO that will be used to signal to the host that
	 * the blackBox I2C slave device has data to be transmitted 
	 */

	/* Enable clock gating for the selected port */
	gIIC_TxDataAvailablePortClkGatingReg_c |= gIIC_TxDataAvailablePortClkGatingBit_c;

	/* Setup the Pin Control Register (PCR) */
	gIIC_TxDataAvailablePinCtrlReg_c = (uint32_t)((gIIC_TxDataAvailablePinCtrlReg_c & (uint32_t)~gIIC_TxDataAvailablePinCtrlMask_c) | 
			(uint32_t)gIIC_TxDataAvailablePinCtrlValue_c);

	/* Setup the Port Data Direction Register (PDDR) */
	gIIC_TxDataAvailablePortDDirReg_c |= (1<<gIIC_TxDataAvailablePin_c);

	/* Signal to the host that there are no data available to be read */    
	gIIC_TxDataAvailablePortDataReg_c |= (1<<gIIC_TxDataAvailablePin_c); 

#endif
}

/******************************************************************************
 * Name: IIC_InitQueue
 * Description: Initialize the IIC RX queue
 * Parameters: [IN] pQueue - pointer to queue
 * Return: TRUE if the pointer is valid, FALSE otherwise
 ******************************************************************************/
static bool_t IIC_InitQueue
(
		IIC_CQueue_t *pQueue
)
{
	if(NULL == pQueue)
	{
		return FALSE;
	}

	pQueue->Head = 0;
	pQueue->Tail = 0;
	pQueue->EntriesCount = 0;

	return TRUE;
}

/******************************************************************************
 * Name: IIC_PushData
 * Description: Add a new element to the RX queue
 * Parameters: [IN] pQueue - pointer to queue
 *         [IN] data - data to be added  
 * Return: TRUE if the push operation succeeded, FALSE otherwise
 ******************************************************************************/
static bool_t IIC_PushData
(
		IIC_CQueue_t *pQueue, 
		unsigned char data
)
{   
	if(NULL == pQueue)
	{
		return FALSE;
	}

	if((pQueue->Tail == pQueue->Head) && (pQueue->EntriesCount > 0))
	{
		/* increment the head (read index) */
		pQueue->Head = (pQueue->Head + 1) & ((unsigned char) (gIIC_SlaveReceiveBufferSize_c - 1));
	}

	/* Add the item to queue */
	pQueue->QData[pQueue->Tail] = data;

	/* Reset the tail when it reach gIIC_SlaveReceiveBufferSize_c */
	pQueue->Tail = (pQueue->Tail + 1) % ((unsigned char) (gIIC_SlaveReceiveBufferSize_c));

	/* Increment the entries count */
	if(pQueue->EntriesCount < (unsigned char) (gIIC_SlaveReceiveBufferSize_c)) pQueue->EntriesCount++;

	return TRUE;
}

/******************************************************************************
 * Name: IIC_PopData
 * Description: Retrieves the head element from the RX queue
 * Parameters: [IN] pQueue - pointer to queue
 *         [OUT] pData - pointer to the location where data shall be placed
 * Return: TRUE if the pop operation succeeded, FALSE otherwise
 ******************************************************************************/
static bool_t IIC_PopData
(
		IIC_CQueue_t *pQueue, 
		unsigned char *pData
)
{    
	if( (NULL == pQueue) || (pQueue->EntriesCount <= 0) || ((NULL == pData)) )
	{
		return FALSE;
	}

	*pData = pQueue->QData[pQueue->Head];

	/* Reset the head when it reach gIIC_SlaveReceiveBufferSize_c */
	pQueue->Head = (pQueue->Head + 1) % ((unsigned char) (gIIC_SlaveReceiveBufferSize_c ));

	/* Decrement the entries count */
	pQueue->EntriesCount--;

	return TRUE;
}

#endif

/******************************************************************************
 ******************************************************************************
 * Public functions
 ******************************************************************************
 ******************************************************************************/

#if gIIC_Enabled_d

/******************************************************************************
 * Name: IIC_ModuleInit
 * Description: Initialize the I2C module. 
 *              Must be called before any further access
 * Parameters: -
 * Return: -
 ******************************************************************************/
void IIC_ModuleInit
(
		void
)
{
	/* Initialize the I2C RX software buffer */
	IIC_InitQueue(&mIIcRxBuf);

	pfIIcSlaveRxCallBack = NULL;
	pfIIcMasterTxCallBack = pfIIcMasterRxCallBack = NULL;

	/* Initialize the I2C hardware */
	I2cInitHW();

	/* Create I2C module main task */
	IIC_TaskInit();
}

/******************************************************************************
 * Name: IIC_ModuleUninit
 * Description: Used to disable the I2C module. 
 * Parameters: -
 * Return: -
 ******************************************************************************/
void IIC_ModuleUninit
(
		void
)
{
	/* disable I2C interrupts */
	I2Cx_C1 &= ~I2C_C1_IICIE_MASK;

	/* disable I2C module */
	I2Cx_C1 &= ~I2C_C1_IICEN_MASK;    

	/* disable I2C clock gating */
	gI2C_PORT_SIM_SCG_c &= ~gI2C_PORT_SIM_SGC_BIT_c;

#if  gIIC_Slave_TxDataAvailableSignal_Enable_c

	/* Signal to the host that there are no data available to be read */    
	gIIC_TxDataAvailablePortDataReg_c |= (1<<gIIC_TxDataAvailablePin_c);

	/* disable clock gating for the selected port */
	gIIC_TxDataAvailablePortClkGatingReg_c &= ~gIIC_TxDataAvailablePortClkGatingBit_c;

#endif
}

/******************************************************************************
 * Name: IIC_SetBaudRate
 * Description: set the baudrate for the I2C module. 
 * Parameters: -
 * Return: -
 ******************************************************************************/
bool_t IIC_SetBaudRate
(
		uint8_t baudRate
)
{
	if ( (I2Cx_S & I2C_S_BUSY_MASK) || 
			(I2Cx_C1 & I2C_C1_MST_MASK))
		return FALSE;

	I2Cx_F = baudRate;
	return TRUE;
}

/******************************************************************************
 * Name: IIC_SetSlaveAddress
 * Description: Set the IIC module slave address 
 * Parameters: [IN] slaveAddress - the slave address
 * Return: -
 ******************************************************************************/
bool_t IIC_SetSlaveAddress
(
		uint8_t slaveAddress
)
{	
	/* Check if the I2C address is valid */
	if((slaveAddress > 0x7f) ||
			(((slaveAddress & 0x78) == 0) && ((slaveAddress & 0x07) != 0)) ||
			((slaveAddress & 0x78) == 0x78))
	{
		return FALSE;
	}
	{	    
		I2Cx_A1 = (I2Cx_A1 & ~I2C_A1_AD_MASK) | ((slaveAddress<<1) & I2C_A1_AD_MASK);

		return TRUE;
	}
}

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
void IIC_BusRecovery
(
		void
)
{
	register uint8_t data;    

	/* disable the I2C module before recovering the bus */
	I2Cx_C1 &=  ~(I2C_C1_MST_MASK | I2C_C1_IICEN_MASK);

	/* re-enable module and generate START condition */
	I2Cx_C1 |=  (I2C_C1_MST_MASK | I2C_C1_IICEN_MASK);

	if (I2Cx_S & I2C_S_ARBL_MASK) /* arbitration was lost? */
	{
		/* clear flag by writing a one to ARBL bit */
		I2Cx_S |= I2C_S_ARBL_MASK;
	}
	/* read dummy data */
	data = I2C0_D;

	/* wait for transfer complete */
	while ((I2Cx_S & I2C_S_TCF_MASK)) {}  
	while ((I2Cx_S & I2C_S_TCF_MASK) == 0) {}

	if (I2Cx_S & I2C_S_ARBL_MASK) 
	{
		/* clear flag by writing a one to ARBL bit */
		I2Cx_S |= I2C_S_ARBL_MASK; 
	}
	/* clear interrupt flag */
	I2Cx_S &= ~I2C_S_IICIF_MASK;

	/* generate STOP condition */
	I2Cx_C1 &= ~I2C_C1_MST_MASK;

	/* to avoid compiler warnings */
	data = data;   
}

/******************************************************************************
 * Name: IIC_SetSlaveRxCallBack
 * Description: Sets a pointer to the function to be called whenever a byte 
 *              is received. If the pointer is set to NULL clear the RX buffer.
 * Parameters: [IN] pfCallBack - callback function
 * Return: 
 ******************************************************************************/
void IIC_SetSlaveRxCallBack
(
		void (*pfCallBack)(void)
)
{
	pfIIcSlaveRxCallBack = pfCallBack;
}

/******************************************************************************
 * Name: IIC_Transmit_Slave
 * Description: Begin transmitting (as SLAVE) of data from *pBuffer
 * Parameters: [IN] pBuf - pointer to data to be transmitted
 *             [IN] bufLen - the length of the buffer
 *             [IN] pfCallBack - pointer to the callback function  
 * Return: Returns FALSE if there are no more slots in the buffer reference 
 *         table
 ******************************************************************************/
bool_t IIC_Transmit_Slave
(
		uint8_t const *pBuf, 
		index_t bufLen,
		void (*pfCallBack)(uint8_t const *pBuf)
) 
{
	/* Handle empty buffers. */
	if (!bufLen) 
	{
		if(pfCallBack)
		{
			(*pfCallBack)(pBuf);  
		}
		return TRUE;
	}

	/* Room for one more? */
	if (maIIcTxBufLenTable[mIIcTxBufRefLeadingIndex]) 
	{
		return FALSE;
	}

	maIIcTxBufRefTable[mIIcTxBufRefLeadingIndex].pBuf = pBuf;
	maIIcTxBufRefTable[mIIcTxBufRefLeadingIndex].pfCallBack = pfCallBack;
	/* INFO: This has to be last, in case the Tx ISR finishes with the previous */
	/* buffer while this function is executing. */
	maIIcTxBufLenTable[mIIcTxBufRefLeadingIndex] = bufLen;

	if (++mIIcTxBufRefLeadingIndex >= NumberOfElements(maIIcTxBufRefTable)) 
	{
		mIIcTxBufRefLeadingIndex = 0;
	}

#if gIIC_Slave_TxDataAvailableSignal_Enable_c 
	/* Signal to host that there are data to receive */
	IIC_TxDataAvailable(TRUE);
#endif
	
	return TRUE;
} 

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
bool_t IIC_Transmit_Master
(
		uint8_t const *pBuf,
		index_t bufLen, 
		uint8_t destAddress, 
		void (*pfCallBack)(bool_t status)
) 
{
	/* Handle empty buffers. */
	if (!bufLen) 
	{
		if(pfCallBack)
		{
			(*pfCallBack)(TRUE);  
		}
		return TRUE;
	}

	destAddress <<= 1;

	if(destAddress == (I2Cx_A1 & I2C_A1_AD_MASK))
	{
		return FALSE;
	}

	/* INFO: pfIIcMasterTxCallBack is reset by IIC task after the callback is called
	         if pfIIcMasterTxCallBack is != 0 it means that the previous callback didn't run yet
	 */

	if(pfIIcMasterTxCallBack) 
	{
		return FALSE;
	}

	if(I2Cx_S & I2C_S_BUSY_MASK)
	{
		return FALSE; 
	}

	mIICMasterOp.iicOpType = mIIC_OpType_Tx_c;
	mIICMasterOp.pBuf = (uint8_t*)pBuf;
	mIICMasterOp.bufLen = bufLen;
	pfIIcMasterTxCallBack = pfCallBack;

	/* generate start condition */
	I2Cx_C1 |= (I2C_C1_MST_MASK | I2C_C1_TX_MASK);

	/* address the slave for writing */
	I2Cx_D = destAddress;
	
	return TRUE;
} 

/******************************************************************************
 * Name: IIC_Receive_Master
 * Description: receive data (as MASTER) from the specified SLAVE
 * Parameters: [IN] pBuf - pointer to destination buffer
 *             [IN] bufLen - the length of the buffer
 *             [IN] destAddress - the slave destination address
 *             [IN] pfCallBack - pointer to the callback function  
 * Return: 
 ******************************************************************************/
bool_t IIC_Receive_Master
(
		uint8_t *pBuf, 
		index_t bufLen, 
		uint8_t destAddress, 
		void (*pfCallBack)(bool_t status)
) 
{
	/* Handle empty buffers. */
	if (!bufLen) 
	{
		if(pfCallBack)
		{
			(*pfCallBack)(TRUE);  
		}
		return TRUE;
	}

	destAddress <<= 1;
	if(destAddress == (I2Cx_A1 & I2C_A1_AD_MASK))
	{
		return FALSE;
	}
	/* INFO: pfIIcMasterTxCallBack is reset by IIC task after the callback is called
	         if pfIIcMasterTxCallBack is != 0 it means that the previous callback didn't run yet 
	 */

	if(pfIIcMasterRxCallBack) 
	{
		return FALSE;
	}

	if(I2Cx_S & I2C_S_BUSY_MASK)
	{
		return FALSE; 
	}

	mIICMasterOp.iicOpType = mIIC_OpType_Rx_c;
	mIICMasterOp.pBuf = pBuf;
	mIICMasterOp.bufLen = bufLen;
	pfIIcMasterRxCallBack = pfCallBack;

	/* generate start condition */
	I2Cx_C1 |= (I2C_C1_MST_MASK | I2C_C1_TX_MASK);

	/* address the slave for reading */
	I2C0_D = destAddress  | 0x1;

	return TRUE;
}

/******************************************************************************
 * Name: IIC_IsSlaveTxActive
 * Description: checks if there are still data to be transmitted to the master
 * Parameters: -
 * Return: Returns TRUE if there are still data to be transmitted to the master
 *         Returns FALSE if nothing left to transmit
 ******************************************************************************/
bool_t IIC_IsSlaveTxActive
(
		void
) 
{
	return ((maIIcTxBufLenTable[mIIcTxBufRefTrailingIndex] != 0) || (I2Cx_S & I2C_S_BUSY_MASK));  
}

/******************************************************************************
 * Name: IIC_TxDataAvailable
 * Description: Depending on the passed parameter value, the master will be   
 *              signaled that it needs to read data from the slave
 * Parameters: [IN]bIsAvailable - if TRUE, the master is signaled that new
 *                                data is available and shall be read 
 * Return: -
 ******************************************************************************/
void IIC_TxDataAvailable
(
		bool_t bIsAvailable
)
{
#if gIIC_Slave_TxDataAvailableSignal_Enable_c
	if(bIsAvailable) 
	{
		gIIC_TxDataAvailablePortDataReg_c &= ~(1<<gIIC_TxDataAvailablePin_c); 
	}
	else
	{
		gIIC_TxDataAvailablePortDataReg_c |= (1<<gIIC_TxDataAvailablePin_c); 
	}
#else
	(void)bIsAvailable;
#endif    
}


/******************************************************************************
 * Name: IIC_GetByteFromRxBuffer
 * Description: Retrieves a byte from the driver's RX buffer and store it 
 *              at *pDst.
 * Parameters: [IN]pDst - pointer to a memory location where the read byte  
 *                        shall be stored
 * Return: TRUE if a byte was retrieved; FALSE if the RX buffer is empty.
 ******************************************************************************/
bool_t IIC_GetByteFromRxBuffer
(
		uint8_t *pDst
)
{
	/* Temporary storage for I2C control register */
	register uint8_t iicControlReg;

	if( (NULL == pDst) || (mIIcRxBuf.EntriesCount == 0) )
	{
		return FALSE;
	}	

	/* Disable all interrupts */
	DisableInterrupts();

	/* Store the control register */
	iicControlReg = I2Cx_C1;

	/* Disable I2C interrupts */
	I2Cx_C1 &= ~I2C_C1_IICIE_MASK;

	/* Enable all interrupts */
	EnableInterrupts();

	/* Get the byte from queue */
	IIC_PopData(&mIIcRxBuf, pDst);

	/* Restore the I2C control register */
	I2Cx_C1 = iicControlReg;

	return TRUE;
}

/******************************************************************************
 * Name: IIC_Task
 * Description: Main task of the I2C module
 * Parameters: [IN]events - see I2C task events
 * Return: -
 ******************************************************************************/
void IIC_Task
(
		event_t events
)
{
	void (*pfCallBack)(uint8_t const *pBuf);

	if (events & gIIC_Event_SlaveRx_c)
	{
		pfIIcSlaveRxCallBack();
	}

	if (events & gIIC_Event_SlaveTx_c)
	{
		/* Tx call back event received - run through the callback table and execute any
         pending Callbacks */
		while (maIIcTxCallbackLeadingIndex != maIIcTxCallbackTrailingIndex)
		{
			pfCallBack = maIIcTxCallbackTable[maIIcTxCallbackTrailingIndex].pfCallBack;
			/* Call callback with buffer info as parameter */
			(*pfCallBack)(maIIcTxCallbackTable[maIIcTxCallbackTrailingIndex].pBuf);
			/* Increment and wrap around trailing index */
			if (++maIIcTxCallbackTrailingIndex >= NumberOfElements(maIIcTxCallbackTable))
			{
				maIIcTxCallbackTrailingIndex = 0;
			}
		}
	}

	if (events & gIIC_Event_MasterRxFail_c)
	{
		(*pfIIcMasterRxCallBack)(FALSE); 
		pfIIcMasterRxCallBack = NULL;
	}
	if (events & gIIC_Event_MasterRxSuccess_c)
	{
		(*pfIIcMasterRxCallBack)(TRUE); 
		pfIIcMasterRxCallBack = NULL;
	}    
	if (events & gIIC_Event_MasterTxFail_c)  
	{
		(*pfIIcMasterTxCallBack)(FALSE);
		pfIIcMasterTxCallBack = NULL;
	}
	if (events & gIIC_Event_MasterTxSuccess_c)  
	{
		(*pfIIcMasterTxCallBack)(TRUE);
		pfIIcMasterTxCallBack = NULL;
	}      
} 

/******************************************************************************
 * Name: IIC_GetRxBytesCount
 * Description: Gets the number of bytes received and stored in the RX queue
 * Parameters: -
 * Return: RX queue entries count 
 ******************************************************************************/
uint16_t IIC_GetRxBytesCount
(
		void
)
{
	return mIIcRxBuf.EntriesCount;
}

/******************************************************************************
 * Name: I2c_Isr
 * Description: I2C interrupt service routine
 * Parameters: -
 * Return: -
 ******************************************************************************/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
void IIC_Isr
(
		void
)
{
	register uint8_t dummy;

	/* Clear the interrupt request */
	I2Cx_S |= I2C_S_IICIF_MASK; /* clear the interrupt request flag by writing a "one" to it */

	/* Check arbitration and slave addressing */

	if (I2Cx_S & (I2C_S_ARBL_MASK | I2C_S_IAAS_MASK ))
	{
		if (I2Cx_S & I2C_S_IAAS_MASK) /* Addressed as slave */
		{
			/* Check if I2C module was addressed for read or for write */
			if(I2Cx_S & I2C_S_SRW_MASK) /* slave TRANSMIT (master reading from slave) */
			{
				/* Configure I2C module for TX operation. Writing to I2Cx_C1 register also clears IAAS bit */
				I2Cx_C1 |= I2C_C1_TX_MASK; /* set I2Cx_C1[TX] */
				/* Send next byte from the current Tx buffer */
				IIC_SendNextByte();
			}
			else /* slave RECEIVE (master writing to slave) */
			{
				/* Configure I2C module for RX operation. Writing to I2Cx_C1 register also clears IAAS bit */
				I2Cx_C1 &= ~(I2C_C1_TX_MASK | I2C_C1_TXAK_MASK);
				/* dummy read of data register */
				dummy = I2C0_D; 
				(void)dummy;
			}
		}
		if(I2Cx_S & I2C_S_ARBL_MASK) /* Arbitration lost */
		{
			I2Cx_S |= I2C_S_ARBL_MASK; /* clear arbitration lost flag */

			if(mIICMasterOp.iicOpType == mIIC_OpType_Tx_c)
			{
				if(pfIIcMasterTxCallBack)
				{
					TS_SendEvent(gIIcTaskId, gIIC_Event_MasterTxFail_c); 
				}
			}
			else
			{
				if(pfIIcMasterRxCallBack)
				{
					TS_SendEvent(gIIcTaskId, gIIC_Event_MasterRxFail_c);    
				}
			}
		}
	}
	else /* Arbitration okay */
	{
		/* Check addressing */
		if (I2Cx_C1 & I2C_C1_MST_MASK) /* MASTER mode */
		{
			if (I2Cx_C1 & I2C_C1_TX_MASK) /* (MASTER) TRANSMIT mode */
			{
				if (I2Cx_S & I2C_S_RXAK_MASK) /* NACK received */
				{
					/* generate STOP condition and configure the module as receiver */
					I2Cx_C1 &= ~(I2C_C1_MST_MASK | I2C_C1_TX_MASK);                    

					if(mIICMasterOp.iicOpType == mIIC_OpType_Tx_c)	/* TX */
					{
						if(pfIIcMasterTxCallBack)
						{
							TS_SendEvent(gIIcTaskId, gIIC_Event_MasterTxFail_c); 
						}
					}
					else	/* RX */
					{
						if(pfIIcMasterRxCallBack)
						{
							TS_SendEvent(gIIcTaskId, gIIC_Event_MasterRxFail_c);    
						}
					}
				}
				else	/* ACK received */
				{
					if(mIICMasterOp.iicOpType == mIIC_OpType_Rx_c)	/* RX */                    
					{
						/* send ACK response for next bytes */
						I2Cx_C1 &= ~(I2C_C1_TX_MASK | I2C_C1_TXAK_MASK);

						if(mIICMasterOp.bufLen-- == 1)  
						{
							/* send NACK response for last byte */
							I2Cx_C1 |= I2C_C1_TXAK_MASK;                            
						}

						/* dummy read of data register */
						dummy = I2C0_D; 
						(void)dummy;  
					}
					else	/* TX */
					{
						if(mIICMasterOp.bufLen) /* more data to be transmitted */
						{
							/* write next byte to I2C data register */
							I2C0_D =  *mIICMasterOp.pBuf++ ;
							/* decrement the buffer size */
							mIICMasterOp.bufLen--;
						}
						else /* last data byte transmitted */
						{
							/* generate STOP condition; after STOP I2C module is in slave mode */
							I2Cx_C1 &= ~(I2C_C1_MST_MASK | I2C_C1_TX_MASK); 

							if(pfIIcMasterTxCallBack)
							{
								TS_SendEvent(gIIcTaskId, gIIC_Event_MasterTxSuccess_c); 
							}
						}
					}
				}
			}
			else /* (MASTER) RECEIVE mode */
			{
				if(mIICMasterOp.bufLen == 0)   /* last byte received */
				{
					/* generate STOP condition; after STOP I2C module is in slave mode */
					I2Cx_C1 &= ~I2C_C1_MST_MASK;   

					if(pfIIcMasterRxCallBack)
					{
						TS_SendEvent(gIIcTaskId, gIIC_Event_MasterRxSuccess_c); 
					}
				}
				else if(mIICMasterOp.bufLen-- == 1) /* next to last byte to receive */ 
				{
					/* send NACK response for last byte */
					I2Cx_C1 |= I2C_C1_TXAK_MASK;
				}

				*mIICMasterOp.pBuf++ = I2C0_D;
			}
		}        
		else   /* SLAVE mode*/
		{
			if (I2Cx_C1 & I2C_C1_TX_MASK) /* (SLAVE) TRANSMIT mode */
			{
				/* IIC has transmmited a byte to the master. Check if ack was received */
				if (I2Cx_S & I2C_S_RXAK_MASK) /* ACK not received */
				{
					/* No ack received. Switch back to receive mode */
					I2Cx_C1 &= ~I2C_C1_TX_MASK;
					/* dummy read of data register */
					dummy = I2C0_D; 
					(void)dummy; 
				}
				else
				{
					/* Ack received. Send next byte */
					IIC_SendNextByte();
				}
			}
			else    /* (SLAVE) RECEIVE mode */
			{
				/* Put the received byte in the buffer */
				if(pfIIcSlaveRxCallBack)
				{
					IIC_PushData(&mIIcRxBuf, I2C0_D);

					/* Let the application know a byte has been received. */
					TS_SendEvent(gIIcTaskId, gIIC_Event_SlaveRx_c);
				}
				else
				{
					/* dummy read of data register */
					dummy = I2C0_D; 
					(void)dummy;  
				}
			}  
		}
	}
}

#endif /* #if gIIC_Enabled_d */

/*****************************************************************************
 *                               <<< EOF >>>                                  *
 ******************************************************************************/
