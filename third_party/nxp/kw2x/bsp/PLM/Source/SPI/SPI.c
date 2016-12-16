/**************************************************************************
* Filename: SPI.c
*
* Description: SPI (DSPI) implementation file for ARM CORTEX-M4 processor
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
#include "SPI.h"
#include "TS_Interface.h"
#include "NVIC.h"

/******************************************************************************
 *******************************************************************************
 * Private macros
 *******************************************************************************
 ******************************************************************************/

/******************************************************************************
 *******************************************************************************
 * Private type definitions
 *******************************************************************************
 ******************************************************************************/

/******************************************************************************
 *******************************************************************************
 * Public memory definitions
 *******************************************************************************
 ******************************************************************************/

#if (gSPI_Enabled_d == TRUE)

/*
 * Name: gSpiConfig
 * Description: Global variable that keeps the current SPI module configuration
 * Valid ranges/values: see definition of spiConfig_t
 */
spiConfig_t gSpiConfig;

/*
 * Name: gSpiTaskId
 * Description: SPI Task ID
 * Valid ranges/values: see definition of tsTaskID_t
 */
tsTaskID_t  gSpiTaskId;

#endif


/******************************************************************************
 *******************************************************************************
 * Private memory definitions
 *******************************************************************************
 ******************************************************************************/

#if (gSPI_Enabled_d == TRUE)

/*
 * Name: maSpiTxBufRefTable
 * Description: SPI slave transmission circular buffers reference table
 * Valid ranges/values: see definition of SpiTxBufRef_t
 */
static SpiTxBufRef_t          maSpiTxBufRefTable[gSPI_SlaveTransmitBuffersNo_c];

/*
 * Name: mSpiTxBufRefLeadingIndex
 * Description: leading (write) index in the SPI slave TX buffers reference table
 * Valid ranges/values: range of index_t
 */
static index_t                mSpiTxBufRefLeadingIndex;

/*
 * Name: mSpiTxBufRefTrailingIndex
 * Description: trailing (read) index in the SPI slave TX buffers reference table
 * Valid ranges/values: range of index_t
 */
static index_t                mSpiTxBufRefTrailingIndex;

/*
 * Name: mSpiTxBufRefCurIndex
 * Description: current index for the TX buffer specified by 'mSpiTxBufRefTrailingIndex'
 * Valid ranges/values: range of index_t
 */
static index_t                mSpiTxBufRefCurIndex;

/*
 * Name: mSpiRxQueue
 * Description: Slave RX circular queue
 * Valid ranges/values: see definition of SPI_CQueue_t
 */
static SPI_CQueue_t mSpiRxQueue;

/*
 * Name: mSpiTxCallbackQueue
 * Description: Slave TX callback queue
 * Valid ranges/values: see definition of SpiTxCallbackQueue_t
 */
static SpiTxCallbackQueue_t mSpiTxCallbackQueue;

/*
 * Name: pfSpiMasterTxCallBack
 * Description: local variable used to store the SPI master TX callback provided
 *              by the application
 * Valid ranges/values: -
 */
static void (*pfSpiMasterTxCallBack)(bool_t status);

/*
 * Name: pfSpiMasterRxCallBack
 * Description: local variable used to store the SPI master RX callback provided
 *              by the application
 * Valid ranges/values: -
 */
static void (*pfSpiMasterRxCallBack)(bool_t status);

/*
 * Name: pfSpiSlaveRxCallBack
 * Description: local variable used to store the SPI slave RX callback
 *              by the application
 * Valid ranges/values: -
 */
static void                   (*pfSpiSlaveRxCallBack)(void);

/*
 * Name: mSpiMasterOp
 * Description: local variable that stores the SPI MASTER operation parameters,
 *              such as direction (RX/TX), data buffer and buffer size
 * Valid ranges/values: see definition of spiMasterOp_t
 */
static spiMasterOp_t          mSpiMasterOp;

/*
 * Name: mSpiSlaveOpDir
 * Description: local variable that stores the SPI SLAVE operation 
 *              direction (RX/TX)
 * Valid ranges/values: see definition of spiOpType_t
 */
static spiOpType_t      mSpiSlaveOpDir;

/*
 * Name: mSpiBytesToTransfer
 * Description: local variable that stores the number of bytes that the SPI driver will
 *              attempt to transfer (send or receive) over the SPI bus. This variable is
 *              checked against the value of SPIx_TCR register for every SPI master
 *              operations.
 * Valid ranges/values:
 */
static uint16_t             mSpiBytesToTransfer;

/*
 * Name: mSpiPeripheralCS
 * Description: local variable that stores the SPI Peripheral Chip Select configuration
 * Valid ranges/values: depends on the SPI module in use (SPI0, SPI1 or SPI2).
 *                      check the reference manual for further details.
 */
static uint8_t              mSpiPeripheralCS;

#endif

/******************************************************************************
 *******************************************************************************
 * Private function prototypes
 *******************************************************************************
 ******************************************************************************/

#if(gSPI_Enabled_d == TRUE)

/******************************************************************************
 * Name: SPI_HwInit
 * Description: initialize the DSPI hardware module
 * Parameters: -
 * Return: -
 ******************************************************************************/
static void SPI_HwInit
(
    void
);

/******************************************************************************
 * Name: SPI_SaveConfig
 * Description: save the current (last) configuration
 * Parameters: [IN] pConfig - pointer to SPI configuration (see spiConfig_t)
 * Return: TRUE if the configuration is successfully saved, FALSE otherwise
 ******************************************************************************/
static bool_t SPI_SaveConfig
(
    spiConfig_t const* pConfig
);


/******************************************************************************
 * Name: SPI_SetBaudrate
 * Description: set the SPI baudrate (MASTER MODE ONLY)
 * Parameters: [IN] baud - the desired baudrate (see spiBaudrate_t)
 * Return: TRUE if the baudrate is successfully set, FALSE otherwise
 ******************************************************************************/
static bool_t SPI_SetBaudrate
(
    spiBaudrate_t baud
);


/******************************************************************************
 * Name: SPI_Task
 * Description: handles and manages the SPI events
 * Parameters: [IN] events - SPI events
 * Return: -
 ******************************************************************************/
static void SPI_Task
(
    event_t events
);


/******************************************************************************
 * Name: SPI_SlaveSendNextByte
 * Description: when SPI is running in slave TX mode, this function is called
 *              to send the next byte from current slave TX buffer. The slave
 *              TX buffers reference table, callback table and queue indexes
 *              are updated accordingly.
 *              Also, it handles the pin that signals to the master that the
 *              slave have data to be send.
 * Parameters: -
 * Return: -
 ******************************************************************************/
static void SPI_SlaveSendNextByte
(
    void
);


/******************************************************************************
 * Name: SPI_ClearRxFIFO
 * Description: flushes the SPI RX FIFO
 * Parameters: -
 * Return: -
 ******************************************************************************/
static void SPI_ClearRxFIFO
(
    void
);

/******************************************************************************
 * Name: SPI_ClearFIFOs
 * Description: flushes both SPI RX and TX FIFOs
 * Parameters: -
 * Return: -
 ******************************************************************************/
static void SPI_ClearFIFOs
(
    void
);


/******************************************************************************
 * Name: InitQueue 
 * Description: Initialize the queue passed as parameter
 * Params: [IN] pQueue - pointer to queue
 * Return: TRUE if the pointer is valid, FALSE otherwise
 ******************************************************************************/
static bool_t SPI_InitRxQueue
(
    SPI_CQueue_t *pQueue
);

/******************************************************************************
 * Name: SPI_PushDataToRxQueue
 * Description: Add a new element to the queue
 * Params: [IN] pQueue - pointer to queue
 *         [IN] data - data to be added  
 * Return: TRUE if the push operation succeeded, FALSE otherwise
 ******************************************************************************/
static bool_t SPI_PushDataToRxQueue
(
    SPI_CQueue_t *pQueue, 
    unsigned char data
);

/******************************************************************************
 * Name: SPI_PopDataFromRxQueue
 * Description: Retrieves the head element from the queue
 * Params: [IN] pQueue - pointer to queue
 *         [OUT] pData - pointer to the location where data shall be placed
 * Return: TRUE if the pop operation succeeded, FALSE otherwise
 ******************************************************************************/
static bool_t SPI_PopDataFromRxQueue
(
    SPI_CQueue_t *pQueue, 
    unsigned char *pData
);

/******************************************************************************
 * Name: SPI_AddCallbackToQueue
 * Description: Add a new callback to the queue
 * Params: [IN] callback - callback to be added  
 * Return: TRUE if the operation succeeded, FALSE otherwise
 ******************************************************************************/
static bool_t SPI_AddCallbackToQueue
(    
    SpiTxCallbackTable_t callback
);

/******************************************************************************
 * Name: SPI_GetCallbackfromQueue
 * Description: Retrieves the head element from the queue
 * Params: [OUT] callback - pointer to the location where callback shall be 
                            placed
 * Return: TRUE if the operation succeeded, FALSE otherwise
 ******************************************************************************/
static bool_t SPI_GetCallbackfromQueue
(    
    SpiTxCallbackTable_t *callback
);

/******************************************************************************
 * Name: InitSlaveTXBuffers
 * Description: Initialize the slave TX reference buffers table, the
 *              callbacks table and the indexes. 
 * Params: -
 * Return: TRUE if initialization succeeded, FALSE otherwise
 ******************************************************************************/
static void InitSlaveTXBuffers
(
    void
);

/******************************************************************************
 * Name: ExecutePendingCallbacks
 * Description: Executes slave TX pending callbacks
 * Params: -
 * Return: -
 ******************************************************************************/
static void ExecutePendingCallbacks
(
    void
);

#endif

/******************************************************************************
 *******************************************************************************
 * Private functions 
 *******************************************************************************
 ******************************************************************************/

#if(gSPI_Enabled_d == TRUE)

/******************************************************************************
 * Name: SPI_HwInit
 * Description: initialize the DSPI hardware module
 * Parameters: -
 * Return: -
 ******************************************************************************/
static void SPI_HwInit
(
    void
)
{
#if (gSPI_ModuleUsed_d == SPI_0)
  /* enable DSPI0 clock gating */
  gSPI_0_SIM_SCGC_REG_c |= gSPI_0_SIM_SCGC_MASK_c;
#elif (gSPI_ModuleUsed_d == SPI_1)
  /* enable SPI1 clock gating */
  gSPI_1_SIM_SCGC_REG_c |= gSPI_1_SIM_SCGC_MASK_c;
#else
  /* enable SPI2 clock gating */
  gSPI_2_SIM_SCGC_REG_c |= gSPI_2_SIM_SCGC_MASK_c;
#endif

  /* enable clock gating for SPI port in use */
  gSPI_PORT_SIM_SCG_c |= gSPI_PORT_SIM_SGC_BIT_c;

  /* enable DSPI clocks */
  SPIx_MCR &= ~SPI_MCR_MDIS_MASK;

  /* pin MUX settings */
  gSPI_PCS_PCR_c &= ~PORT_PCR_MUX_MASK;
  gSPI_PCS_PCR_c |= PORT_PCR_MUX(gSPI_PCS_ALT_FUNC_c);      /* SPIx_PCS */
  gSPI_SCK_PCR_c &= ~PORT_PCR_MUX_MASK;
  gSPI_SCK_PCR_c |= PORT_PCR_MUX(gSPI_SCK_ALT_FUNC_c);      /* SPIx_SCK */
  gSPI_SOUT_PCR_c &= ~PORT_PCR_MUX_MASK;
  gSPI_SOUT_PCR_c |= PORT_PCR_MUX(gSPI_SOUT_ALT_FUNC_c);    /* SPIx_SOUT */
  gSPI_SIN_PCR_c &= ~PORT_PCR_MUX_MASK;
  gSPI_SIN_PCR_c |= PORT_PCR_MUX(gSPI_SIN_ALT_FUNC_c);      /* SPIx_SIN */

#if  gSPI_Slave_TxDataAvailableSignal_Enable_d

  /* Enable clock gating for the selected port */
  gSPI_DataAvailablePortClkGatingReg_c |= gSPI_DataAvailablePortClkGatingBit_c;

  /* Setup the Pin Control Register (PCR) */
  gSPI_DataAvailablePinCtrlReg_c = PORT_PCR_MUX(1);

  /* Setup the Port Data Direction Register (PDDR) - (default input) */
  gSPI_DataAvailablePortDDirReg_c &= ~gSPI_DataAvailablePinMask_c;

#endif
}


/******************************************************************************
 * Name: SPI_SaveConfig
 * Description: save the current (last) configuration
 * Parameters: [IN] pConfig - pointer to SPI configuration (see spiConfig_t)
 * Return: TRUE if the configuration is successfully saved, FALSE otherwise
 ******************************************************************************/
static bool_t SPI_SaveConfig
(
    spiConfig_t const* pConfig
)
{
  if(NULL == pConfig)
    return FALSE;

  gSpiConfig.devMode = pConfig->devMode;
  gSpiConfig.continuousClkEn = pConfig->continuousClkEn;
  gSpiConfig.periphCSStrobeEn = pConfig->periphCSStrobeEn;
  gSpiConfig.RxFifoOvflOverwriteEn = pConfig->RxFifoOvflOverwriteEn;
  gSpiConfig.continuousPCSEn = pConfig->continuousPCSEn;
  gSpiConfig.PCSInactiveState.value = pConfig->PCSInactiveState.value;
  gSpiConfig.dozeEn = pConfig->dozeEn;
  gSpiConfig.frameSize = pConfig->frameSize;
  gSpiConfig.clockPol = pConfig->clockPol;
  gSpiConfig.clockPhase = pConfig->clockPhase;
  gSpiConfig.lsbFirst = pConfig->lsbFirst;
  gSpiConfig.pcsToSckDelayPrescaler = pConfig->pcsToSckDelayPrescaler;
  gSpiConfig.afterSckDelayPrescaler = pConfig->afterSckDelayPrescaler;
  gSpiConfig.delayAfterTransferPrescaler = pConfig->delayAfterTransferPrescaler;
  gSpiConfig.pcsToSckDelayScaler = pConfig->pcsToSckDelayScaler;
  gSpiConfig.afterSckSDelayScaler = pConfig->afterSckSDelayScaler;
  gSpiConfig.delayAfterTransferScaler = pConfig->delayAfterTransferScaler;
  gSpiConfig.baudRate = pConfig->baudRate;

  return TRUE;
}

/******************************************************************************
 * Name: SPI_SetBaudrate
 * Description: set the SPI baudrate (MASTER MODE ONLY)
 * Parameters: [IN] baud - the desired baudrate (see spiBaudrate_t)
 * Return: TRUE if the baudrate is successfully set, FALSE otherwise
 ******************************************************************************/
static bool_t SPI_SetBaudrate
(
    spiBaudrate_t baud
)
{
  bool_t retValue = TRUE;

#if (gSystemClock_c == gSystemClk48MHz_c)

  if(SPIx_SR & SPI_SR_TXRXS_MASK) /* SPI is in running mode */
    return FALSE;

  if(!(SPIx_MCR & SPI_MCR_MSTR_MASK)) /* not in MASTER MODE */
    return FALSE;

  switch(baud)
  {
  case gSPI_BaudRate_100000_c:
    SPIx_CTAR0 |= ((gSPI_PBR_100K_Value_c<<SPI_CTAR_PBR_SHIFT) & SPI_CTAR_PBR_MASK);
    SPIx_CTAR0 |= ((gSPI_BR_100K_Value_c<<SPI_CTAR_BR_SHIFT) & SPI_CTAR_BR_MASK);
    break;

  case gSPI_BaudRate_200000_c:
    SPIx_CTAR0 |= ((gSPI_PBR_200K_Value_c<<SPI_CTAR_PBR_SHIFT) & SPI_CTAR_PBR_MASK);
    SPIx_CTAR0 |= ((gSPI_BR_200K_Value_c<<SPI_CTAR_BR_SHIFT) & SPI_CTAR_BR_MASK);
    break;

  case gSPI_BaudRate_400000_c:
    SPIx_CTAR0 |= ((gSPI_PBR_400K_Value_c<<SPI_CTAR_PBR_SHIFT) & SPI_CTAR_PBR_MASK);
    SPIx_CTAR0 |= ((gSPI_BR_400K_Value_c<<SPI_CTAR_BR_SHIFT) & SPI_CTAR_BR_MASK);
    break;

  case gSPI_BaudRate_800000_c:
    SPIx_CTAR0 |= ((gSPI_PBR_800K_Value_c<<SPI_CTAR_PBR_SHIFT) & SPI_CTAR_PBR_MASK);
    SPIx_CTAR0 |= ((gSPI_BR_800K_Value_c<<SPI_CTAR_BR_SHIFT) & SPI_CTAR_BR_MASK);
    break;

  case gSPI_BaudRate_1000000_c:
    SPIx_CTAR0 |= ((gSPI_PBR_1M_Value_c<<SPI_CTAR_PBR_SHIFT) & SPI_CTAR_PBR_MASK);
    SPIx_CTAR0 |= ((gSPI_BR_1M_Value_c<<SPI_CTAR_BR_SHIFT) & SPI_CTAR_BR_MASK);
    break;

  case gSPI_BaudRate_2000000_c:
    SPIx_CTAR0 |= ((gSPI_PBR_2M_Value_c<<SPI_CTAR_PBR_SHIFT) & SPI_CTAR_PBR_MASK);
    SPIx_CTAR0 |= ((gSPI_BR_2M_Value_c<<SPI_CTAR_BR_SHIFT) & SPI_CTAR_BR_MASK);
    break;

  case gSPI_BaudRate_4000000_c:
    SPIx_CTAR0 |= ((gSPI_PBR_4M_Value_c<<SPI_CTAR_PBR_SHIFT) & SPI_CTAR_PBR_MASK);
    SPIx_CTAR0 |= ((gSPI_BR_4M_Value_c<<SPI_CTAR_BR_SHIFT) & SPI_CTAR_BR_MASK);
    break;

  case gSPI_BaudRate_8000000_c:
    SPIx_CTAR0 |= ((gSPI_PBR_8M_Value_c<<SPI_CTAR_PBR_SHIFT) & SPI_CTAR_PBR_MASK);
    SPIx_CTAR0 |= ((gSPI_BR_8M_Value_c<<SPI_CTAR_BR_SHIFT) & SPI_CTAR_BR_MASK);
    break;

  default:
    retValue = FALSE;
    break;
  };

  return retValue;
#else /* gSystemClock_c != gSystemClk48MHz_c */
  return FALSE;
#endif
}


/******************************************************************************
 * Name: SPI_Task
 * Description: handles and manages the SPI events
 * Parameters: [IN] events - SPI events
 * Return: -
 ******************************************************************************/
static void SPI_Task
(
    event_t events
)
{
  /* Master TX success event */
  if(events & gSPI_Event_MasterTxSuccess_c)
    {
      /* Run the master TX callback with TRUE as status argument then invalidate it */
      (*pfSpiMasterTxCallBack)(TRUE);
      pfSpiMasterTxCallBack = NULL;
    }

  /* Master TX fail event */
  if(events & gSPI_Event_MasterTxFail_c)
    {
      /* Run the master TX callback with FALSE as status argument then invalidate it */
      (*pfSpiMasterTxCallBack)(FALSE);
      pfSpiMasterTxCallBack = NULL;
    }

  /* Master RX success event */
  if(events & gSPI_Event_MasterRxSuccess_c)
    {
      /* Run the master RX callback with TRUE as status argument then invalidate it */
      (*pfSpiMasterRxCallBack)(TRUE);
      pfSpiMasterRxCallBack = NULL;
    }

  /* Master RX fail event */
  if(events & gSPI_Event_MasterRxFail_c)
    {
      /* Run the master RX callback with FALSE as status argument then invalidate it */
      (*pfSpiMasterRxCallBack)(FALSE);
      pfSpiMasterRxCallBack = NULL;
    }

  /* Slave TX event */
  if(events & gSPI_Event_SlaveTx_c)
    {
      ExecutePendingCallbacks();
    }

  /* Slave RX event */
  if(events & gSPI_Event_SlaveRx_c)
    {
      pfSpiSlaveRxCallBack();
    }
}

/******************************************************************************
 * Name: SPI_SlaveSendNextByte
 * Description: when SPI is running in slave TX mode, this function is called
 *              to send the next byte from current slave TX buffer. The slave
 *              TX buffers reference table, callback table and queue indexes
 *              are updated accordingly.
 *              Also, it handles the pin that signals to the master that the
 *              slave have data to be send.
 * Parameters: -
 * Return: -
 ******************************************************************************/
static void SPI_SlaveSendNextByte(void)
{      
  
  SpiTxCallbackTable_t callbackTbl;
  
  if(maSpiTxBufRefTable[mSpiTxBufRefTrailingIndex].mTxBufLen)
    {
      /* Fill the FIFO  */                
      SPIx_PUSHR_SLAVE = maSpiTxBufRefTable[mSpiTxBufRefTrailingIndex].pTxBuf[mSpiTxBufRefCurIndex++];

      if (SPIx_SR & SPI_SR_TFUF_MASK)
        {
          /* If characters have been put into the TX FIFO, clear the TX FiFo Underflow Flag */
          SPIx_SR |= SPI_SR_TFUF_MASK;
        }

      /* Finished with this buffer? */
      if(mSpiTxBufRefCurIndex >= maSpiTxBufRefTable[mSpiTxBufRefTrailingIndex].mTxBufLen)
        {
          /* Mark this one as done and call the callback */
          maSpiTxBufRefTable[mSpiTxBufRefTrailingIndex].mTxBufLen = 0;      
                    
          if(maSpiTxBufRefTable[mSpiTxBufRefTrailingIndex].pfTxBufCallBack)
            {       
              callbackTbl.pfTxCallBack = maSpiTxBufRefTable[mSpiTxBufRefTrailingIndex].pfTxBufCallBack;
              callbackTbl.pTxBuf = maSpiTxBufRefTable[mSpiTxBufRefTrailingIndex].pTxBuf;
          
              /* add callback to queue */
              if(SPI_AddCallbackToQueue(callbackTbl))
              {
                /* Signal the SPI Task that we got a callback to be executed */
                TS_SendEvent(gSpiTaskId, gSPI_Event_SlaveTx_c);
              }
              else
              {
                /* not a normal situation, but it must be handled, even in interrupt context */
                ExecutePendingCallbacks();
                SPI_AddCallbackToQueue(callbackTbl);
              }
            }

          /* Reset the current index */
          mSpiTxBufRefCurIndex = 0;

          /* Increment and wrap around the trailing index */
          if(++mSpiTxBufRefTrailingIndex >= gSPI_SlaveTransmitBuffersNo_c)
            {
              mSpiTxBufRefTrailingIndex = 0;
            }
        }
    }
}


/******************************************************************************
 * Name: SPI_ClearRxFIFO
 * Description: flushes the SPI RX FIFO
 * Parameters: -
 * Return: -
 ******************************************************************************/
static void SPI_ClearRxFIFO
(
    void
)
{
  SPIx_MCR |= SPI_MCR_CLR_RXF_MASK;
}

/******************************************************************************
 * Name: SPI_ClearFIFOs
 * Description: flushes both SPI RX and TX FIFOs
 * Parameters: -
 * Return: -
 ******************************************************************************/
static void SPI_ClearFIFOs
(
    void
)
{
  SPIx_MCR |= (SPI_MCR_CLR_RXF_MASK | SPI_MCR_CLR_TXF_MASK);
}

/******************************************************************************
 * Name: SPI_InitRxQueue 
 * Description: Initialize the queue passed as parameter
 * Params: [IN] pQueue - pointer to queue
 * Return: TRUE if the pointer is valid, FALSE otherwise
 ******************************************************************************/
static bool_t SPI_InitRxQueue
(
    SPI_CQueue_t *pQueue
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
 * Name: SPI_PushDataToRxQueue
 * Description: Add a new element to the queue
 * Params: [IN] pQueue - pointer to queue
 *         [IN] data - data to be added  
 * Return: TRUE if the push operation succeeded, FALSE otherwise
 ******************************************************************************/
static bool_t SPI_PushDataToRxQueue
(
    SPI_CQueue_t *pQueue, 
    unsigned char data
)
{   
  if( (NULL == pQueue) || (NULL == pQueue->pData) )
    {
      return FALSE;
    }

  if( (pQueue->Tail == pQueue->Head) && (pQueue->EntriesCount > 0) )
    {
      /* increment the head (read index) */
      pQueue->Head = (pQueue->Head + 1) & ((unsigned char) (gSPI_SlaveReceiveBufferSize_c - 1));
    }

  /* Add the item to queue */
  pQueue->pData[pQueue->Tail] = data;

  /* Reset the tail when it reach gSPI_SlaveReceiveBufferSize_c */
  pQueue->Tail = (pQueue->Tail + 1) % ((unsigned char) (gSPI_SlaveReceiveBufferSize_c));

  /* Increment the entries count */
  if(pQueue->EntriesCount < (unsigned char) (gSPI_SlaveReceiveBufferSize_c)) pQueue->EntriesCount++;

  return TRUE;
}

/******************************************************************************
 * Name: SPI_PopDataFromRxQueue
 * Description: Retrieves the head element from the queue
 * Params: [IN] pQueue - pointer to queue
 *         [OUT] pData - pointer to the location where data shall be placed
 * Return: TRUE if the pop operation succeeded, FALSE otherwise
 ******************************************************************************/
static bool_t SPI_PopDataFromRxQueue
(
    SPI_CQueue_t *pQueue, 
    unsigned char *pData
)
{    
  if( ( NULL == pQueue )  || (pQueue->EntriesCount <= 0) )
    {
      return FALSE;
    }

  if( NULL == pData )
    {
      return FALSE;
    }

  *pData = pQueue->pData[pQueue->Head];

  /* Reset the head when it reach gSPI_SlaveReceiveBufferSize_c */
  pQueue->Head = (pQueue->Head + 1) % ((unsigned char) (gSPI_SlaveReceiveBufferSize_c ));

  /* Decrement the entries count */
  pQueue->EntriesCount--;

  return TRUE;
}

/******************************************************************************
 * Name: SPI_AddCallbackToQueue
 * Description: Add a new callback to the queue
 * Params: [IN] callback - callback to be added  
 * Return: TRUE if the operation succeeded, FALSE otherwise
 ******************************************************************************/
static bool_t SPI_AddCallbackToQueue
(    
    SpiTxCallbackTable_t callback
)
{
  if((mSpiTxCallbackQueue.Tail == mSpiTxCallbackQueue.Head) && (mSpiTxCallbackQueue.EntriesCount > 0))
    {
      return FALSE;
    }

  /* Add the item to queue */
  mSpiTxCallbackQueue.pData[mSpiTxCallbackQueue.Tail] = callback;

  /* Reset the tail when it reach gSPI_SlaveTransmitBuffersNo_c */
  mSpiTxCallbackQueue.Tail = (mSpiTxCallbackQueue.Tail + 1) % ((unsigned char) (gSPI_SlaveTransmitBuffersNo_c));

  /* Increment the entries count */
  if(mSpiTxCallbackQueue.EntriesCount < (unsigned char) (gSPI_SlaveTransmitBuffersNo_c)) 
    mSpiTxCallbackQueue.EntriesCount++;

  return TRUE; 
}

/******************************************************************************
 * Name: SPI_GetCallbackfromQueue
 * Description: Retrieves the head element from the queue
 * Params: [OUT] callback - pointer to the location where callback shall be 
                            placed
 * Return: TRUE if the operation succeeded, FALSE otherwise
 ******************************************************************************/
static bool_t SPI_GetCallbackfromQueue
(    
    SpiTxCallbackTable_t *callback
)
{
  if(mSpiTxCallbackQueue.EntriesCount <= 0)
    {
      return FALSE;
    }

  if(NULL == callback)
    {
      return FALSE;
    }

  *callback = mSpiTxCallbackQueue.pData[mSpiTxCallbackQueue.Head];

  /* Reset the head when it reach gSPI_SlaveTransmitBuffersNo_c */
  mSpiTxCallbackQueue.Head = (mSpiTxCallbackQueue.Head + 1) % ((unsigned char) (gSPI_SlaveTransmitBuffersNo_c ));

  /* Decrement the entries count */
  mSpiTxCallbackQueue.EntriesCount--;

  return TRUE;
}

/******************************************************************************
 * Name: InitSlaveTXBuffers
 * Description: Initialize the slave TX reference buffers table, the
 *              callbacks table and indexes. 
 * Params: -
 * Return: TRUE if initialization succeeded, FALSE otherwise
 ******************************************************************************/
static void InitSlaveTXBuffers
(
    void
)
{
  index_t loopCnt;

  for(loopCnt = 0; loopCnt < (index_t)gSPI_SlaveTransmitBuffersNo_c; loopCnt++)
    {
      maSpiTxBufRefTable[loopCnt].pTxBuf = NULL;
      maSpiTxBufRefTable[loopCnt].mTxBufLen = 0;
      maSpiTxBufRefTable[loopCnt].pfTxBufCallBack = NULL;
    }

  mSpiTxBufRefLeadingIndex = 0;
  mSpiTxBufRefTrailingIndex = 0;
  mSpiTxBufRefCurIndex = 0;  
  
  mSpiTxCallbackQueue.Head = mSpiTxCallbackQueue.Tail = mSpiTxCallbackQueue.EntriesCount = 0;
}

/******************************************************************************
 * Name: ExecutePendingCallbacks
 * Description: Executes slave TX pending callbacks
 * Params: -
 * Return: -
 ******************************************************************************/
static void ExecutePendingCallbacks
(
    void
)
{
  SpiTxCallbackTable_t callbackTbl;
  
  while(SPI_GetCallbackfromQueue(&callbackTbl))
  {
    callbackTbl.pfTxCallBack(callbackTbl.pTxBuf);
  }  
}

#endif

/******************************************************************************
 ******************************************************************************
 * Public functions
 ******************************************************************************
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
bool_t SPI_Init
(
    void
)
{
  /* Initialize the SPI hardware */
  SPI_HwInit();

  /* Enable SPI interrupt within the Nested Vector Interrupt Controller (NVIC)  */
  NVIC_EnableIRQ(gSPI_IRQ_NUM_c);

  /* Set SPI IRQ priority */
  NVIC_SetPriority(gSPI_IRQ_NUM_c, gSPI_InterruptPriority_c);

  /* Create the SPI Task */
  gSpiTaskId = TS_CreateTask(gTsSpiTaskPriority_c, SPI_Task);

  if(gTsInvalidTaskID_c != gSpiTaskId)
    {
      /* Setup the default configuration */
      gSpiConfig.devMode = gSPI_DefaultMode_c;
      gSpiConfig.continuousClkEn = gSPI_CONTINUOUS_CLK_EN_DEFAULT_c;
      gSpiConfig.periphCSStrobeEn = gSPI_PCS_STROBE_ENABLE_DEFAULT_c;
      gSpiConfig.RxFifoOvflOverwriteEn = gSPI_RXFIFO_OVFL_EN_DEFAULT_c;
      gSpiConfig.continuousPCSEn = gSPI_CONTINUOUS_PCS_EN_DEFAULT_c;
      gSpiConfig.PCSInactiveState.value = gSPI_PCS_INACTIVE_STATE_DEFAULT_c & gSPI_PCS_INACTIVE_STATE_MASK_c;
      gSpiConfig.dozeEn = gSPI_DOZE_EN_DEFAULT_c;
      gSpiConfig.frameSize = gSPI_FRAME_SZ_DEFAULT_c;
      gSpiConfig.clockPol = gSPI_DefaultClockPol_c;
      gSpiConfig.clockPhase = gSPI_DefaultClockPhase_c;
      gSpiConfig.lsbFirst = gSPI_DefaultBitwiseShifting_c;
      gSpiConfig.pcsToSckDelayPrescaler = gSPI_PCS_SCK_DELAY_PRESCALER_DEFAULT_c;
      gSpiConfig.afterSckDelayPrescaler = gSPI_AFTER_SCK_DELAY_PRESCALER_DEFAULT_c;
      gSpiConfig.delayAfterTransferPrescaler = gSPI_DELAY_AFTER_TRANSFER_PRESCALER_DEFAULT_c;
      gSpiConfig.pcsToSckDelayScaler = gSPI_PCS_SCK_DELAY_SCALER_DEFAULT_c;
      gSpiConfig.afterSckSDelayScaler = gSPI_AFTER_SCK_DELAY_SCALER_DEFAULT;
      gSpiConfig.delayAfterTransferScaler = gSPI_DELAY_AFTER_TRANSFER_SCALER_DEFAULT_c;
      gSpiConfig.baudRate = gSPI_DefaultBaudRate_c;

      /* Apply default configuration */
      if(!SPI_SetConfig(&gSpiConfig))
        {
          return FALSE;
        }

      /* RFDF and TFFF flags generates interrupts */
      SPIx_RSER &= ~(SPI_RSER_RFDF_DIRS_MASK | SPI_RSER_TFFF_DIRS_MASK);

      /* Flush RX and TX FIFOs */
      SPI_ClearFIFOs();

      /* Initialize all SPI callback pointers to NULL */
      pfSpiSlaveRxCallBack  = NULL;
      pfSpiMasterTxCallBack = NULL;
      pfSpiMasterRxCallBack = NULL;

      /* Initialize slave TX buffers */
      InitSlaveTXBuffers();

      /* Initialize slave RX queue */
      if(!SPI_InitRxQueue(&mSpiRxQueue))
        {
          return FALSE;
        }

      /* set default SPI slave mode to RX */
      mSpiSlaveOpDir = mSpi_OpType_Rx_c;

      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

/******************************************************************************
 * Name: SPI_Uninit
 * Description: Stop and disable the SPI module.
 * Parameters: -
 * Return: -
 ******************************************************************************/
void SPI_Uninit
(
    void
)
{

  /* Destroy the SPI task */
  if (gTsInvalidTaskID_c != gSpiTaskId) /* valid ID */
    {
      TS_DestroyTask(gSpiTaskId);
    }

  /* stop DSPI transfer */
  SPIx_MCR |= SPI_MCR_HALT_MASK;

#if (gSPI_ModuleUsed_d == SPI_0)
  /* disable DSPI0 clock gating */
  gSPI_0_SIM_SCGC_REG_c &= ~gSPI_0_SIM_SCGC_MASK_c;
#elif (gSPI_ModuleUsed_d == SPI_1)
  /* disable SPI1 clock gating */
  gSPI_1_SIM_SCGC_REG_c &= ~gSPI_1_SIM_SCGC_MASK_c;
#else
  /* disable SPI2 clock gating */
  gSPI_2_SIM_SCGC_REG_c &= ~gSPI_2_SIM_SCGC_MASK_c;
#endif

  /* disable SPIx interrupt request */
  NVIC_DisableIRQ(gSPI_IRQ_NUM_c);

#if  gSPI_Slave_TxDataAvailableSignal_Enable_d
  /* Disable clock gating for the selected port */
  gSPI_DataAvailablePortClkGatingReg_c &= ~gSPI_DataAvailablePortClkGatingBit_c;
#endif
}

/******************************************************************************
 * Name: SPI_SetConfig
 * Description: Apply the SPI configuration pointed by 'pSpiConfig'.
 * Parameters: [IN ]pSpiConfig - pointer to an SPI configuration
 * Return: TRUE if configuration is successfully applied / FALSE otherwise
 ******************************************************************************/
bool_t SPI_SetConfig
(
    const spiConfig_t* pSpiConfig
)
{
  uint32_t RSER_RegVal;

  /* check against NULL pointer */
  if(NULL == pSpiConfig)
    return FALSE;

  /* store RSER register value */
  RSER_RegVal = SPIx_RSER;

  /* halt SPI */
  SPIx_MCR |= SPI_MCR_HALT_MASK;

  /* disable all interrupts */
  SPIx_RSER &= ~(SPI_RSER_TCF_RE_MASK | SPI_RSER_EOQF_RE_MASK | SPI_RSER_TFUF_RE_MASK | 
      SPI_RSER_TFFF_RE_MASK | SPI_RSER_RFOF_RE_MASK | SPI_RSER_RFDF_RE_MASK);

  /* clear FIFOs */
  SPI_ClearFIFOs();

  if(SPIx_SR & SPI_SR_TXRXS_MASK) /* still running ? */
    return FALSE;           

  /* apply configuration */        
  SPIx_MCR |= ((pSpiConfig->devMode << SPI_MCR_MSTR_SHIFT) & SPI_MCR_MSTR_MASK);
  SPIx_MCR |= ((pSpiConfig->RxFifoOvflOverwriteEn << SPI_MCR_ROOE_SHIFT) & SPI_MCR_ROOE_MASK);
  SPIx_MCR |= ((pSpiConfig->dozeEn << SPI_MCR_DOZE_SHIFT) & SPI_MCR_DOZE_MASK);
  SPIx_MCR |= ((pSpiConfig->PCSInactiveState.value << SPI_MCR_PCSIS_SHIFT) & SPI_MCR_PCSIS_MASK);

  if (pSpiConfig->devMode == gSPI_MasterMode_c) /* MASTER MODE */
    {           
      SPIx_MCR |= ((pSpiConfig->continuousClkEn << SPI_MCR_CONT_SCKE_SHIFT) & SPI_MCR_CONT_SCKE_MASK);
#if ((MCU_MK60N512VMD100 == 1) || (MCU_MK20D5 == 1) || (MCU_MK60D10 == 1))  
      SPIx_MCR |= ((pSpiConfig->periphCSStrobeEn << SPI_MCR_PCSSE_SHIFT) & SPI_MCR_PCSSE_MASK);           
#endif

      SPIx_CTAR0 = gSPI_CTAR_RESET_VALUE;
      SPIx_CTAR0 |= ((pSpiConfig->frameSize << SPI_CTAR_FMSZ_SHIFT) & SPI_CTAR_FMSZ_MASK);
      SPIx_CTAR0 |= ((pSpiConfig->clockPol << SPI_CTAR_CPOL_SHIFT) & SPI_CTAR_CPOL_MASK);
      SPIx_CTAR0 |= ((pSpiConfig->clockPhase << SPI_CTAR_CPHA_SHIFT) & SPI_CTAR_CPHA_MASK);
      SPIx_CTAR0 |= ((pSpiConfig->lsbFirst << SPI_CTAR_LSBFE_SHIFT) & SPI_CTAR_LSBFE_MASK);
      SPIx_CTAR0 |= ((pSpiConfig->pcsToSckDelayPrescaler << SPI_CTAR_PCSSCK_SHIFT) & SPI_CTAR_PCSSCK_MASK);
      SPIx_CTAR0 |= ((pSpiConfig->afterSckDelayPrescaler << SPI_CTAR_PASC_SHIFT) & SPI_CTAR_PASC_MASK);
      SPIx_CTAR0 |= ((pSpiConfig->delayAfterTransferPrescaler << SPI_CTAR_PDT_SHIFT) & SPI_CTAR_PDT_MASK);
      SPIx_CTAR0 |= ((pSpiConfig->pcsToSckDelayScaler << SPI_CTAR_CSSCK_SHIFT) & SPI_CTAR_CSSCK_MASK);
      SPIx_CTAR0 |= ((pSpiConfig->afterSckSDelayScaler << SPI_CTAR_ASC_SHIFT) & SPI_CTAR_ASC_MASK);
      SPIx_CTAR0 |= ((pSpiConfig->delayAfterTransferScaler << SPI_CTAR_DT_SHIFT) & SPI_CTAR_DT_MASK);

      SPI_SetBaudrate(pSpiConfig->baudRate);
    }
  else /* SLAVE MODE */  
    {     
      SPIx_CTAR0_SLAVE = gSPI_CTAR_RESET_VALUE;
      SPIx_CTAR0_SLAVE |= ((pSpiConfig->frameSize << SPI_CTAR_FMSZ_SHIFT) & SPI_CTAR_FMSZ_MASK);
      SPIx_CTAR0_SLAVE |= ((pSpiConfig->clockPol << SPI_CTAR_CPOL_SHIFT) & SPI_CTAR_CPOL_MASK);
      SPIx_CTAR0_SLAVE |= ((pSpiConfig->clockPhase << SPI_CTAR_CPHA_SHIFT) & SPI_CTAR_CPHA_MASK);
    }

#if gSPI_Slave_TxDataAvailableSignal_Enable_d
  if(pSpiConfig->devMode == gSPI_MasterMode_c)
    {
      /* Configure as input the GPIO used to be signaled by the slave
           device when it has data to be transmitted.
       */
      SPI_ConfigRxDataAvailablePin();
    }
  else if (pSpiConfig->devMode == gSPI_SlaveMode_c)
    {
      /* Configure as output the GPIO that will be used to signal the master
           device that the slave device has data to transmit.
       */
      SPI_ConfigTxDataAvailablePin();
    }
#endif

  /* save the current configuration */
  SPI_SaveConfig(pSpiConfig);

  /* restore RSER register value */
  SPIx_RSER = RSER_RegVal;

  if (pSpiConfig->devMode == gSPI_SlaveMode_c)
    {
      /* RFDF flag generates interrupt */
      SPIx_RSER &= ~SPI_RSER_RFDF_DIRS_MASK;
      /* Enable the Receive FIFO Drain interrupt */
      SPIx_RSER |= SPI_RSER_RFDF_RE_MASK;

      /* Clear the HALT bit */
      SPIx_MCR &= !SPI_MCR_HALT_MASK;   
    }

  return TRUE;
}

/******************************************************************************
 * Name: SPI_GetConfig
 * Description: Retrieve the current SPI configuration and store it to a
 *              location pointed by 'pSpiConfig'.
 * Parameters: [OUT] pSpiConfig - pointer to SPI configuration location
 * Return: TRUE if configuration is successfully retrieved / FALSE otherwise
 ******************************************************************************/
bool_t SPI_GetConfig
(
    spiConfig_t* pSpiConfig
)
{
  /* check against NULL pointer */
  if(NULL == pSpiConfig)
    {
      return FALSE;
    }

  pSpiConfig->devMode = gSpiConfig.devMode;
  pSpiConfig->continuousClkEn = gSpiConfig.continuousClkEn;
  pSpiConfig->periphCSStrobeEn = gSpiConfig.periphCSStrobeEn;
  pSpiConfig->RxFifoOvflOverwriteEn = gSpiConfig.RxFifoOvflOverwriteEn;
  pSpiConfig->continuousPCSEn = gSpiConfig.continuousPCSEn;
  pSpiConfig->PCSInactiveState.value = gSpiConfig.PCSInactiveState.value;
  pSpiConfig->dozeEn = gSpiConfig.dozeEn;
  pSpiConfig->frameSize = gSpiConfig.frameSize;
  pSpiConfig->clockPol = gSpiConfig.clockPol;
  pSpiConfig->clockPhase = gSpiConfig.clockPhase;
  pSpiConfig->lsbFirst = gSpiConfig.lsbFirst;
  pSpiConfig->pcsToSckDelayPrescaler = gSpiConfig.pcsToSckDelayPrescaler;
  pSpiConfig->afterSckDelayPrescaler = gSpiConfig.afterSckDelayPrescaler;
  pSpiConfig->delayAfterTransferPrescaler = gSpiConfig.delayAfterTransferPrescaler;
  pSpiConfig->pcsToSckDelayScaler = gSpiConfig.pcsToSckDelayScaler;
  pSpiConfig->afterSckSDelayScaler = gSpiConfig.afterSckSDelayScaler;
  pSpiConfig->delayAfterTransferScaler = gSpiConfig.delayAfterTransferScaler;
  pSpiConfig->baudRate = gSpiConfig.baudRate;

  return TRUE;
}

/******************************************************************************
 * Name: SPI_MasterTransmit
 * Description: SPI MASTER TX function
 * Parameters: [IN] pBuf - pointer to data to be send
 *             [IN] bufLen - buffer length
 *             [IN] pfCallback - SPI master TX callback function
 * Return: TRUE / FALSE
 ******************************************************************************/
bool_t SPI_MasterTransmit
(
    uint8_t *pBuf,
    index_t bufLen,
    void (*pfCallBack)(bool_t status)
)
{
  bool_t retStatus = TRUE;  

  /* check against NULL pointer */
  if (NULL == pBuf)
    return FALSE;

  if (gSpiConfig.devMode == gSPI_MasterMode_c) 
    {
      /* Handle empty buffers. */
      if (!bufLen) 
        {
          /* Call the callback function (with the status FALSE), if the pointer to it is valid */
          if (pfCallBack) 
            {
              (*pfCallBack)(FALSE);
            }
        } 
      else 
        {
          /* 'pfSpiMasterTxCallBack' is reset by SPI Task after the callback is called */
          /* If 'pfSpiMasterTxCallBack' is != 0 it means that the previous callback didn't run yet */
          if (pfSpiMasterTxCallBack) 
            {
              retStatus = FALSE;
            } 
          else 
            {
              /* Update the variable storing the master operation and also the callback */
              mSpiMasterOp.spiOpType = mSpi_OpType_Tx_c;
              mSpiMasterOp.pBuf = pBuf;
              mSpiMasterOp.bufLen = bufLen;
              pfSpiMasterTxCallBack = pfCallBack;

              /* Save the buffer length (this value is used in ISR to validate the transfer) */
              mSpiBytesToTransfer = bufLen;

              /* Clear TFFF, if set */
              if (SPIx_SR & SPI_SR_TFFF_MASK) 
                {
                  SPIx_SR |= SPI_SR_TFFF_MASK;
                }

              if (mSpiMasterOp.bufLen < gSPI_TxFifoSize_c) 
                {
                  /* Disable TX FIFO */
                  SPIx_MCR |= SPI_MCR_DIS_TXF_MASK;
                } 
              else 
                {
                  /* Enable TX FIFO */
                  SPIx_MCR &= ~SPI_MCR_DIS_TXF_MASK;
                  /* Clear FIFOs */
                  SPI_ClearFIFOs();
                }

              /* Enable Transmit FIFO Fill Request interrupt */
              SPIx_RSER |= SPI_RSER_TFFF_RE_MASK;
              SPIx_RSER &= ~SPI_RSER_TFFF_DIRS_MASK;

              /* Disable Receive FIFO Drain Request interrupt */
              SPIx_RSER &= ~SPI_RSER_RFDF_RE_MASK;

              /* Enable DSPI Finished interrupt request */
              SPIx_RSER |= SPI_RSER_EOQF_RE_MASK;
            }
        }
    } 
  else 
    {
      retStatus = FALSE;
    }

  return retStatus;
}

/******************************************************************************
 * Name: SPI_MasterReceive
 * Description: SPI MASTER RX function
 * Parameters: [IN] pBuf - pointer to the location where received data shall
 *                         be stored
 *             [IN] bufLen - how many bytes to receive
 *             [IN] pfCallback - SPI master RX callback function
 * Return: TRUE / FALSE
 ******************************************************************************/
bool_t SPI_MasterReceive
(
    uint8_t *pBuf,
    index_t bufLen,
    void (*pfCallBack)(bool_t status)
)
{
  bool_t retStatus = TRUE;
  uint32_t contPCSMask = 0;

  /* check against NULL pointer */
  if (NULL == pBuf)
    return FALSE;

  if (gSpiConfig.devMode == gSPI_MasterMode_c) 
    {
      /* Handle empty buffers. */
      if (!bufLen) 
        {
          /* Call the callback function (with the status FALSE), if it is a valid pointer to it */
          if (pfCallBack) 
            {
              (*pfCallBack)(FALSE);
            }
        } 
      else 
        {
          /* 'pfSpiMasterRxCallBack' is reset by Spi Task after the callback is called */
          /* If 'pfSpiMasterRxCallBack' is != 0 it means that the previous callback didn't run yet */
          if (pfSpiMasterRxCallBack) 
            {
              retStatus = FALSE;
            } 
          else 
            {
              /* Update the variable storing the master operation and also the callback */
              mSpiMasterOp.spiOpType = mSpi_OpType_Rx_c;
              mSpiMasterOp.pBuf = pBuf;
              mSpiMasterOp.bufLen = bufLen;
              pfSpiMasterRxCallBack = pfCallBack;

              /* Save the buffer length (this value is used in ISR to validate the transfer) */
              mSpiBytesToTransfer = bufLen;

              /* Stop SPI transfer */
              SPIx_MCR |= SPI_MCR_HALT_MASK;

              /* Clear RFOF flag, if set */
              if (SPIx_SR & SPI_SR_RFOF_MASK) 
                {
                  SPIx_SR |= SPI_SR_RFOF_MASK;
                }

              /* Clear RFDF flag, if set */
              if (SPIx_SR & SPI_SR_RFDF_MASK) 
                {
                  SPIx_SR |= SPI_SR_RFDF_MASK;
                }

              /* Clear TFFF if set */
              if (SPIx_SR & SPI_SR_TFFF_MASK) 
                {
                  SPIx_SR |= SPI_SR_TFFF_MASK;
                }

              /* Clear FIFOs */
              SPI_ClearFIFOs();

              /* Disable TX FIFO */
              SPIx_MCR |= SPI_MCR_DIS_TXF_MASK;

              /* Set continuous PCS enable mask, if option is set by configuration */
              if(gSpiConfig.continuousPCSEn)
                {
                  contPCSMask = SPI_PUSHR_CONT_MASK;
                }

              /* Fill the TX buffer with dummy data, clear TCR register */
              SPIx_PUSHR = contPCSMask + SPI_PUSHR_CTCNT_MASK + SPI_PUSHR_PCS(mSpiPeripheralCS) + gSPI_MasterRecvDummyTxData;

              /* TFFF and RFDF generates interrupt requests */
              SPIx_RSER &= ~(SPI_RSER_TFFF_DIRS_MASK | SPI_RSER_RFDF_DIRS_MASK);        

              /* Enable the Transmit FIFO Fill Request interrupt and Receive FIFO Drain interrupt */
              SPIx_RSER |= (SPI_RSER_TFFF_RE_MASK | SPI_RSER_RFDF_RE_MASK);

              /* Start transfer */
              SPIx_MCR &= ~SPI_MCR_HALT_MASK;
            }
        }
    } 
  else 
    {
      retStatus = FALSE;
    }

  return retStatus;
}

/******************************************************************************
 * Name: SPI_SlaveTransmit
 * Description: SPI SLAVE RX function
 * Parameters: [IN] pBuf - pointer to the location where data to be sent
 *                         are stored
 *             [IN] bufLen - how many bytes to transmit
 *             [IN] pfCallback - SPI slave TX callback function
 * Return: TRUE / FALSE
 ******************************************************************************/
bool_t SPI_SlaveTransmit
(
    uint8_t *pBuf,
    index_t bufLen,
    void (*pfCallBack)(uint8_t *pBuf)
)
{
  bool_t retStatus = TRUE;
  
  /* execute pending callback, if any */
  if(mSpiTxCallbackQueue.EntriesCount > 0)
  {
    ExecutePendingCallbacks();
  }
  
  /* check against NULL pointer */
  if(NULL == pBuf)
    return FALSE;

  if(gSpiConfig.devMode == gSPI_SlaveMode_c)
    {
      /* Handle empty buffers. */
      if(!bufLen)
        {
          /* Call the callback if the pointer to it, is valid */
          if(pfCallBack)
            {
              (*pfCallBack)(pBuf);
            }
        }
      else
        {                        
          /* Room for one more? */
          if(maSpiTxBufRefTable[mSpiTxBufRefLeadingIndex].mTxBufLen)
            {
              retStatus = FALSE;
            }
          else
            {
              /* Update the table ref. with pointer of the Tx buffer and the callback */
              maSpiTxBufRefTable[mSpiTxBufRefLeadingIndex].pTxBuf           = pBuf;
              maSpiTxBufRefTable[mSpiTxBufRefLeadingIndex].pfTxBufCallBack  = pfCallBack;
              /* Update the table with bufer lengths with the new element */
              maSpiTxBufRefTable[mSpiTxBufRefLeadingIndex].mTxBufLen        = bufLen;
                          
              /* Increment the leading index and reset if exceed the number of TX
           buffers in slave mode */
              if(++mSpiTxBufRefLeadingIndex >= gSPI_SlaveTransmitBuffersNo_c)
                {
                  mSpiTxBufRefLeadingIndex = 0;
                }              
              
              /* Disable TX FIFO */
              SPIx_MCR |= SPI_MCR_DIS_TXF_MASK;

              /* Slave operation type = transmit */
              mSpiSlaveOpDir = mSpi_OpType_Tx_c;

              /* Disable TFFF interrupt */
              SPIx_RSER &= ~SPI_RSER_TFFF_RE_MASK;

              /* Fill the TX FIFO / PUSHR register */
              SPI_SlaveSendNextByte();

              /* Inform the master that the slave is willing to transmit */
#if gSPI_Slave_TxDataAvailableSignal_Enable_d                                
              SPI_SignalTxDataAvailable(TRUE);

              /* Enable RFDF interrupt */
              SPIx_RSER |= SPI_RSER_RFDF_RE_MASK;
#endif
            }
        }
    }
  else
    {
      retStatus = FALSE;
    }
  
  return retStatus;
}

/******************************************************************************
 * Name: SPI_SetSlaveRxCallBack
 * Description: Set SPI slave RX callback
 * Parameters: [IN] pfCallBack - pointer to callback function
 * Return: -
 ******************************************************************************/
void SPI_SetSlaveRxCallBack
(
    void (*pfCallBack)(void)
)
{
  pfSpiSlaveRxCallBack = pfCallBack;
}

/******************************************************************************
 * Name: SPI_IsSlaveTxActive
 * Description: check if the SPI slave transmission is active
 * Parameters: -
 * Return: TRUE if SPI slave transmission is in progress / FALSE otherwise
 ******************************************************************************/
bool_t SPI_IsSlaveTxActive
(
    void
)
{
  return (SPIx_SR & SPI_SR_TXRXS_MASK);
}

/******************************************************************************
 * Name: SPI_GetByteFromBuffer
 * Description: Retrieve a byte from the driver's RX circular buffer and store
 *              it at *pDest.
 * Parameters: [OUT] pDest - pointer to a location where the retrieved byte
 *                           shall be placed.
 * Return: TRUE if a byte was retrieved / FALSE if the RX buffer is
 *         empty.
 ******************************************************************************/
bool_t SPI_GetByteFromBuffer
(
    uint8_t *pDst
)
{   
  return SPI_PopDataFromRxQueue(&mSpiRxQueue, pDst);
}

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
bool_t SPI_ConfigPCS
(
    uint8_t CSNum,
    bool_t strobeEn,
    bool_t inactiveHigh
)
{
  if( (gSPI_PCS_Max_c <= CSNum) || (SPIx_SR & SPI_SR_TXRXS_MASK) )
    {
      return FALSE;
    }

  /* store the peripheral chip select (PCS) to be used */
  mSpiPeripheralCS = ( 1<<CSNum );

  /* set PCS strobe enable */
#if ((MCU_MK60N512VMD100 == 1) || (MCU_MK20D5 == 1) || (MCU_MK60D10 == 1))          
  SPIx_MCR |= ((strobeEn << SPI_MCR_PCSSE_SHIFT) & SPI_MCR_PCSSE_MASK);
#endif

  /* set PCS inactive state */
  SPIx_MCR |= ((inactiveHigh << (SPI_MCR_PCSIS_SHIFT + CSNum)) & SPI_MCR_PCSIS_MASK);

  /* update the changes made above to global SPI configuration variable */
  gSpiConfig.periphCSStrobeEn = strobeEn;
  gSpiConfig.PCSInactiveState.value |= (inactiveHigh << CSNum);

  return TRUE;
}

/******************************************************************************
 * Name: SPI_GetSlaveRecvBytesCount
 * Description: Gets the number of bytes stored in the RX queue (slave mode)
 * Parameters: -
 * Return: RX queue entries count 
 ******************************************************************************/
uint16_t SPI_GetSlaveRecvBytesCount
(
    void
)
{
  return mSpiRxQueue.EntriesCount;
}

/******************************************************************************
 * Name: SPI_SetContinuousPCSEn
 * Description: Enable/Disable Continuous Peripheral Chip Select signal
 * Parameters: [IN] enable - if TRUE, the PCS signal is kept asserted between
 *                  transfers; if FALSE, the PCS signal returns to its inactive
 *                  state between transfers.
 * Return: -
 ******************************************************************************/
void SPI_SetContinuousPCSEn
(
    bool_t enable    
)
{
  gSpiConfig.continuousPCSEn = enable;
}

/******************************************************************************
 * Name: SPI_Isr
 * Description: SPI Interrupt Service Routine
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
void SPI_Isr
(
    void
)
{
  static bool_t recvNextByte = TRUE;

  uint32_t SPI_SRVal;
  uint32_t contPCSEnMask = 0;
  uint8_t SpiRecvData;

  /* store the SPI status register */
  SPI_SRVal = SPIx_SR;

  /* Set continuous PCS enable mask, if option is set by configuration */
  if(gSpiConfig.continuousPCSEn)
    {
      contPCSEnMask = SPI_PUSHR_CONT_MASK;
    } 

  if( (SPIx_RSER & SPI_RSER_TFFF_RE_MASK) && (SPI_SRVal & SPI_SR_TFFF_MASK) ) /* TX FIFO FILL REQUEST */
    {
      /* clear the flag */
      SPIx_SR |= SPI_SR_TFFF_MASK;

      if(SPIx_MCR & SPI_MCR_MSTR_MASK) /* MASTER */
        {
          if(mSpiMasterOp.spiOpType == mSpi_OpType_Tx_c) /* TX */
            {
              if(mSpiMasterOp.bufLen)
                {
                  if(((SPI_SRVal & SPI_SR_TXCTR_MASK) >> SPI_SR_TXCTR_SHIFT) < gSPI_TxFifoSize_c)
                    {
                      if(1 == mSpiBytesToTransfer)
                        {
                          /* the first and the only byte to transfer */
                          SPIx_PUSHR = (contPCSEnMask | SPI_PUSHR_CTCNT_MASK | SPI_PUSHR_EOQ_MASK) + SPI_PUSHR_PCS(mSpiPeripheralCS) + *mSpiMasterOp.pBuf++;
                        }
                      else
                        {
                          if(mSpiBytesToTransfer == mSpiMasterOp.bufLen)
                            {
                              /* first byte of the transfer, assert the slave, reset the transfer counter */
                              SPIx_PUSHR = (contPCSEnMask | SPI_PUSHR_CTCNT_MASK) + SPI_PUSHR_PCS(mSpiPeripheralCS) + *mSpiMasterOp.pBuf++;
                            }
                          else if(1 == mSpiMasterOp.bufLen)
                            {
                              /* last byte of the transfer, set the EOQ bit in the control word */
                              SPIx_PUSHR = (contPCSEnMask | SPI_PUSHR_EOQ_MASK) + SPI_PUSHR_PCS(mSpiPeripheralCS) + *mSpiMasterOp.pBuf;               
                            }
                          else
                            {   /* bytes in between */
                              SPIx_PUSHR = contPCSEnMask + SPI_PUSHR_PCS(mSpiPeripheralCS) + *mSpiMasterOp.pBuf++;         
                            }
                        }

                      /* Decrement the buffer length */
                      mSpiMasterOp.bufLen--;                        

                      if(SPIx_MCR & SPI_MCR_HALT_MASK) /* transfer halted */
                        {
                          /* Start transfer */
                          SPIx_MCR &= ~SPI_MCR_HALT_MASK;
                        }
                    }
                }
              else
                {                                 
                  /* release the slave (negate PCS signal) */
                  SPIx_PUSHR &= ~(contPCSEnMask + SPI_PUSHR_PCS(mSpiPeripheralCS));

                  /* disable the TFFF interrupt */
                  SPIx_RSER &= ~SPI_RSER_TFFF_RE_MASK;

                  /* disable RFDF interrupt */
                  SPIx_RSER &= ~SPI_RSER_RFDF_RE_MASK;
                }
            }
          else    /* MASTER RX */
            {                       
              if(mSpiMasterOp.bufLen) /* there are bytes to be received */
                {   
                  if(recvNextByte) /* previous reception completed */
                    {
                      /* clear the flag */
                      recvNextByte = FALSE;

                      /* trigger the next reception */
                      SPIx_PUSHR = contPCSEnMask + SPI_PUSHR_PCS(mSpiPeripheralCS) + gSPI_MasterRecvDummyTxData;                              
                    }

                  /* disable TFFF until received data is picked up from the RX FIFO */
                  SPIx_RSER &= ~SPI_RSER_TFFF_RE_MASK;

                  if(SPIx_MCR & SPI_MCR_HALT_MASK) /* transfer is halted */
                    {
                      /* Start transfer */
                      SPIx_MCR &= ~SPI_MCR_HALT_MASK;
                    }    
                }
              else /* all bytes received */
                {                                               
                  if(pfSpiMasterRxCallBack)
                    {
                      /* Send an event to the SPI Task with the master RX operation status */
                      if(mSpiBytesToTransfer == (uint16_t)((SPIx_TCR & SPI_TCR_SPI_TCNT_MASK) >> SPI_TCR_SPI_TCNT_SHIFT))
                        {
                          TS_SendEvent(gSpiTaskId, gSPI_Event_MasterRxSuccess_c);
                        }
                      else
                        {
                          TS_SendEvent(gSpiTaskId, gSPI_Event_MasterRxFail_c);
                        }
                    }

                  /* release the slave (negate PCS signal) */
                  SPIx_PUSHR &= ~(contPCSEnMask + SPI_PUSHR_PCS(mSpiPeripheralCS));

                  /* disable the TFFF interrupt */
                  SPIx_RSER &= ~SPI_RSER_TFFF_RE_MASK;

                  /* disable RFDF interrupt */
                  SPIx_RSER &= ~SPI_RSER_RFDF_RE_MASK;
                }
            }
        }
      else /* SLAVE TX */
        {
          /* no action */
        }
    }

  if( (SPIx_RSER &SPI_RSER_EOQF_RE_MASK) && (SPI_SRVal & SPI_SR_EOQF_MASK) ) /* END OF QUEUE  */ 
    {      
      /* halt transfer */
      SPIx_MCR |= SPI_MCR_HALT_MASK;      

      /* clear the EOQ flag */
      SPIx_SR |= SPI_SR_EOQF_MASK;

      /* disable EOQF interrupt */
      SPIx_RSER &= ~SPI_RSER_EOQF_RE_MASK;

      /* disable TX FIFO FILL request */
      SPIx_RSER &= ~SPI_RSER_TFFF_RE_MASK;

      /* disable RX FIFO DRAIN request */                 
      SPIx_RSER &= ~SPI_RSER_RFDF_RE_MASK;

      /* flush FIFOs */
      SPI_ClearFIFOs();

      if( SPIx_MCR & SPI_MCR_MSTR_MASK ) /* MASTER */
        {
          if(mSpiMasterOp.spiOpType == mSpi_OpType_Tx_c) /* TX */
            {     
              /* inform the task scheduler, if valid callback is configured */
              if(pfSpiMasterTxCallBack)
                {
                  if(mSpiBytesToTransfer == (uint16_t)((SPIx_TCR & SPI_TCR_SPI_TCNT_MASK) >> SPI_TCR_SPI_TCNT_SHIFT))
                    {
                      TS_SendEvent(gSpiTaskId, gSPI_Event_MasterTxSuccess_c);
                    }
                  else
                    {
                      TS_SendEvent(gSpiTaskId, gSPI_Event_MasterTxFail_c);
                    }
                }
            }
        }
    }    

  if( (SPIx_RSER & SPI_RSER_RFDF_RE_MASK) && (SPI_SRVal & SPI_SR_RFDF_MASK)) /* RX FIFO DRAIN REQUEST */
    {               
      /* clear the RFDF flag */
      SPIx_SR |= SPI_SR_RFDF_MASK;

      if(SPIx_MCR & SPI_MCR_MSTR_MASK) /* MASTER */
        {
          if(mSpiMasterOp.spiOpType == mSpi_OpType_Rx_c) /* RX */
            {
              if(mSpiMasterOp.bufLen)
                {                 
                  /* get the data */
                  *mSpiMasterOp.pBuf = (uint8_t)SPIx_POPR;  

                  /* move to the next data */
                  mSpiMasterOp.pBuf++;

                  /* decrement the buffer length */
                  mSpiMasterOp.bufLen--;

                  /* enable TFFF interrupt */
                  SPIx_RSER |= SPI_RSER_TFFF_RE_MASK;

                  /* inform that the next receive operation can be triggered */
                  recvNextByte = TRUE;
                }
            }     
        }
      else /* SLAVE */
        {
          /* get the data from POPR register */
          SpiRecvData = (uint8_t)SPIx_POPR;

          if(mSpiSlaveOpDir == mSpi_OpType_Tx_c) /* TX */             
            {
              /* discard the received byte */
              (void)SpiRecvData;

              if((mSpiTxBufRefTrailingIndex == mSpiTxBufRefLeadingIndex) && (mSpiTxBufRefCurIndex == 0))
                {
                  /* no more data to send  */
#if gSPI_Slave_TxDataAvailableSignal_Enable_d                              
                  SPI_SignalTxDataAvailable(FALSE);
#endif /* gSPI_Slave_TxDataAvailableSignal_Enable_d */
                  /* execute pending slave TX callbacks, if any */
                  TS_SendEvent(gSpiTaskId, gSPI_Event_SlaveTx_c);
                  /* switch to slave RX mode */
                  mSpiSlaveOpDir = mSpi_OpType_Rx_c;
                }
              else
                {
                  SPI_SlaveSendNextByte();
                }
            }
          else /* RX */
            {
              /* Put the received byte in the RX queue */
              SPI_PushDataToRxQueue(&mSpiRxQueue, SpiRecvData);      

              /* Let the application know that one or more bytes has been received */
              TS_SendEvent(gSpiTaskId, gSPI_Event_SlaveRx_c);
            }
        }
    }

  if(SPI_SRVal & SPI_SR_TCF_MASK) /* TRANSFER COMPLETE */
    {
      /* clear the flag */
      SPIx_SR |= SPI_SR_TCF_MASK;
    }

  if(SPI_SRVal & SPI_SR_TFUF_MASK) /* TX FIFO UNDERFLOW */
    {
      /* this flag is set when TX FIFO is empty, 
       * the SPI is in slave mode and an external 
       * SPI master initiates a transfer 
       */

      /* clear the flag */
      SPIx_SR |= SPI_SR_TFUF_MASK;

#if gSPI_Slave_TxDataAvailableSignal_Enable_d
      if (!(SPIx_SR & SPI_SR_TXCTR_MASK))
        {
          /* If there is no data in the TX FiFo
      clear the TX data available line */
          SPI_SignalTxDataAvailable(FALSE);
        }
#endif
    }

  if(SPI_SRVal & SPI_SR_RFOF_MASK) /* RX FIFO OVERFLOW FLAG */
    {
      /*
       * Indicates an overflow condition in the RX FIFO. 
       * The bit is set when the RX FIFO and shift register are full and a transfer is initiated.
       * In MASTER TX mode, the Receive FIFO Drain request is disabled because the driver doesn't
       * need to be interrupted on every dummy byte received from slave. Therefore, from time to time
       * the RX FIFO overflows and is handled here.
       * 
       */

      /* clear the flag */
      SPIx_SR |= SPI_SR_RFOF_MASK;

      /* clear the FIFO */
      SPI_ClearRxFIFO();
    }
}

#endif /* #if (gSPI_Enabled_d == TRUE) */

