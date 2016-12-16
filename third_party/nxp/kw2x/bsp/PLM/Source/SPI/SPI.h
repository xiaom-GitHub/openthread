/**************************************************************************
* Filename: SPI.h
*
* Description: SPI (DSPI) header file for AMR CORTEX-M4 processor
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

#ifndef SPI_H_
#define SPI_H_

#include "PortConfig.h"
#include "SPI_Interface.h"

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/***************************************************************** 
 *                   MCU SPECIFIC DEFINITIONS                    *
 *****************************************************************/

#if ((MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
    /* Number of SPI modules available on chip */
    #define gSPI_HW_MODULES_AVAILABLE_c              1
    /* NOTE: The device has 2 SPI hardware modules, but SPI_1 is used by the radio */

    /* SPI clock gating register and mask */
    #define gSPI_0_SIM_SCGC_REG_c                    SIM_SCGC6
    #define gSPI_0_SIM_SCGC_MASK_c                   SIM_SCGC6_SPI0_MASK

    /* SPI0 IRQ number */
    #ifndef gSPI_0_IRQ_NUM_c
    #define gSPI_0_IRQ_NUM_c                         26
    #endif

    /* Peripheral Chip Select available signals */
    #ifndef gSPI_0_PCS_CNT_c
    #define gSPI_0_PCS_CNT_c                         4
    #endif
#elif ((MCU_MK60D10 == 1) || (MCU_MK60N512VMD100 == 1))
    /* Number of SPI modules available on chip */
    #define gSPI_HW_MODULES_AVAILABLE_c              3
    
    /* SPI clock gating register and mask */
    #define gSPI_0_SIM_SCGC_REG_c                    SIM_SCGC6
    #if(MCU_MK60D10 == 1)
    #define gSPI_0_SIM_SCGC_MASK_c                   SIM_SCGC6_SPI0_MASK
    #else
    #define gSPI_0_SIM_SCGC_MASK_c                   SIM_SCGC6_DSPI0_MASK
    #endif
    #define gSPI_1_SIM_SCGC_REG_c                    SIM_SCGC7
    #define gSPI_1_SIM_SCGC_MASK_c                   SIM_SCGC6_SPI1_MASK
    #define gSPI_2_SIM_SCGC_REG_c                    SIM_SCGC3
    #define gSPI_2_SIM_SCGC_MASK_c                   SIM_SCGC3_SPI2_MASK
    
    /* SPIx IRQ number */
    #ifndef gSPI_0_IRQ_NUM_c
    #define gSPI_0_IRQ_NUM_c                         26
    #endif
    #ifndef gSPI_1_IRQ_NUM_c
    #define gSPI_1_IRQ_NUM_c                         27
    #endif
    #ifndef gSPI_2_IRQ_NUM_c
    #define gSPI_2_IRQ_NUM_c                         28
    #endif

    /* SPIx Peripheral Chip Select available signals */
    #ifndef gSPI_0_PCS_CNT_c
    #define gSPI_0_PCS_CNT_c                         6
    #endif
    #ifndef gSPI_1_PCS_CNT_c
    #define gSPI_1_PCS_CNT_c                         4
    #endif
    #ifndef gSPI_2_PCS_CNT_c
    #define gSPI_2_PCS_CNT_c                         2
    #endif
#elif (MCU_MK20D5 == 1)
    /* Number of SPI modules available on chip */
    #define gSPI_HW_MODULES_AVAILABLE_c              3
    /* SPI clock gating register and mask */
    #define gSPI_0_SIM_SCGC_REG_c                    SIM_SCGC6
    #define gSPI_0_SIM_SCGC_MASK_c                   SIM_SCGC6_SPI0_MASK    
    /* SPIx IRQ number */
    #ifndef gSPI_0_IRQ_NUM_c
    #define gSPI_0_IRQ_NUM_c                         12
    #endif
    /* SPI0 Peripheral Chip Select available signals */
    #ifndef gSPI_0_PCS_CNT_c
    #define gSPI_0_PCS_CNT_c                         5
    #endif    
#endif

/* 
 * Events for SPI task
 */
#define gSPI_Event_MasterTxSuccess_c        (1<<1)
#define gSPI_Event_MasterTxFail_c           (1<<2)
#define gSPI_Event_MasterRxSuccess_c        (1<<3)
#define gSPI_Event_MasterRxFail_c           (1<<4)
#define gSPI_Event_SlaveTx_c                (1<<5)
#define gSPI_Event_SlaveRx_c                (1<<6)

/*
 * Name: gSPI_TxFifoSize_c
 * Description: TX FIFO size (as specified in Reference Manual - Chip 
 *              Configuration chapter) 
 */
#define gSPI_TxFifoSize_c                   4

/*
 * Name: gSPI_RxFifoSize_c
 * Description: RX FIFO size (as specified in Reference Manual - Chip 
 *              Configuration chapter) 
 */
#define gSPI_RxFifoSize_c                   4

/*
 * Name: gSPI_MasterRecvDummyTxData
 * Description: Value used in Master RX mode. This mode assumes the 
 *              transmission of a dummy byte to slave and the reception
 *              of a valid byte from slave. Can be set to any value
 *              except the value of the handshake byte.
 */
#define gSPI_MasterRecvDummyTxData          0x0000


/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/*
 * Name: SPI_CQueue_t
 * Description: SPI circular queue data type definition
 */
typedef struct tag_CQueue
{
    uint8_t         pData[gSPI_SlaveReceiveBufferSize_c];   /* queue data */
    index_t         Head;                                   /* read index */
    index_t         Tail;                                   /* write index */
    unsigned int    EntriesCount;                           /* self explanatory */
} SPI_CQueue_t;

/*
 * Name: SpiTxCallbackTable_t
 * Description: SPI TX callback table type definition
 */
typedef struct SpiTxCallbackTable_tag
{
    void          (*pfTxCallBack)(uint8_t *pTxBuf);
    uint8_t       *pTxBuf;
} SpiTxCallbackTable_t;

/*
 * Name: SpiTxCallbackQueue_t
 * Description: SPI TX callback queue type definition
 */
typedef struct SpiTxCallbackQueue_tag
{
  SpiTxCallbackTable_t pData[gSPI_SlaveTransmitBuffersNo_c]; /* queue data */
  index_t              Head;                                          /* read index */
  index_t              Tail;                                          /* write index */
  unsigned int         EntriesCount;                                  /* entries count */
} SpiTxCallbackQueue_t;

/*
 * Name: SpiTxBufRef_t
 * Description: SPI (slave) TX buffer reference table type definition
 */
typedef struct SpiTxBufRef_tag
{
    uint8_t       *pTxBuf;
    uint8_t       mTxBufLen;
    void          (*pfTxBufCallBack)(uint8_t *pTxBuf);
} SpiTxBufRef_t;

/*
 * Name: spiOpType_t
 * Description: Data enumeration for SPI operations (transmit/receive)
 */
typedef enum
{
    mSpi_OpType_Tx_c,
    mSpi_OpType_Rx_c
} spiOpType_t;

/*
 * Name: spiMasterOp_t
 * Description: SPI master data and operation type definition
 */
typedef struct spiMasterOp_tag
{
    spiOpType_t   spiOpType;
    uint8_t       *pBuf;
    uint8_t       bufLen;  
} spiMasterOp_t;

#ifdef __cplusplus
}
#endif

#endif /* SPI_H_ */
