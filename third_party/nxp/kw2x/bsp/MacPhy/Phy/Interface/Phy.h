/************************************************************************************
* Header file for interface of the PHY layer.
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
************************************************************************************/

#ifndef __PHY_H__
#define __PHY_H__

/*****************************************************************************
 *                               INCLUDED HEADERS                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the headers that this module needs to include.    *
 * Note that it is not a good practice to include header files into header   *
 * files, so use this section only if there is no other better solution.     *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*****************************************************************************
 *                             PUBLIC MACROS, DEFINITIONS                    *
 *---------------------------------------------------------------------------*
 * Add to this section all the access macros, registers mappings, bit access *
 * macros, masks, flags etc ...
 *---------------------------------------------------------------------------*
 *****************************************************************************/
 
#ifdef __cplusplus
    extern "C" {
#endif
     
#if !defined(gUsePBTransferThereshold_d)
#define gUsePBTransferThereshold_d     (0)
#endif

#ifndef gPhyReadPBinISR_d
#define gPhyReadPBinISR_d              (1)
#endif

#define gUseStandaloneCCABeforeTx_d    (1)
#define MC1324x_Irq_Priority           (0xau)
#define gPhyMaxDataLength_c            (127)

// PHY states
enum {
  gIdle_c,
  gRX_c,
  gTX_c,
  gCCA_c,
  gTR_c,
  gCCCA_c
};

// PHY channel state
enum {
  gChannelIdle_c,
  gChannelBusy_c
};

// PHY requests exit states
enum {
  gPhySuccess_c,
  gPhyBusy_c,
  gPhyInvalidParam_c
};

// PANCORDNTR bit in PP
enum {
  gMacRole_DeviceOrCoord_c,
  gMacRole_PanCoord_c
};

// Cca types
enum {
  gCcaED_c,            // energy detect - CCA bit not active, not to be used for T and CCCA sequences
  gCcaCCA_MODE1_c,     // energy detect - CCA bit ACTIVE
  gCcaCCA_MODE2_c,     // 802.15.4 compliant signal detect - CCA bit ACTIVE
  gCcaCCA_MODE3_c,     //
  gInvalidCcaType_c    // illegal type
};

enum {
  gNormalCca_c,
  gContinuousCca_c
};

//MC1324X DTS modes
enum {
  gDtsNormal_c,
  gDtsTxOne_c,
  gDtsTxZero_c,
  gDtsTx2Mhz_c,
  gDtsTx200Khz_c,
  gDtsTx1MbpsPRBS9_c,
  gDtsTxExternalSrc_c,
  gDtsTxRandomSeq_c
};

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// PhyPdDataRequest and PhyPlmeCcaEdRequest parameter bit map:
//  |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
//  slottedEn   x       x    ackReq   ccaEn  cont.En     ccaType
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// slottedEn == 1 -> slotted
// slottedEn == 0 -> unslotted
//   ackReq  == 1 -> TxRxAck
//   ackReq  == 0 -> Tx
//   ccaEn   == 1 -> CcaTx or CcaTxRxAck depending on AckReq
//   ccaEn   == 0 -> no CCA before Tx  or ED scan
//   cont.En == 1 -> Continuous CCA
//   cont.En == 0 -> normal CCA
//   ccaType == 3 -> do not use !
//   ccaType == 2 ->  CCA mode 2
//   ccaType == 1 ->  CCA mode 1
//   ccaType == 0 ->  ED

#define gSlottedEnPos_c    7
#define gAckReqPos_c       4
#define gCcaEnPos_c        3
#define gContinuousEnPos_c 2
#define gCcaTypePos_c      0

#define gSlottedEnMask_c    (1 << gSlottedEnPos_c)
#define gAckReqMask_c       (1 << gAckReqPos_c)
#define gCcaEnMask_c        (1 << gCcaEnPos_c)
#define gContinuousEnMask_c (1 << gContinuousEnPos_c)
#define gCcaTypeMask_c      (3 << gCcaTypePos_c)

// argument definitions for PhyPlmeCcaRequest()

#define gCcaReq_Continuous_Mode1_c             ((gContinuousEnMask_c)| (gCcaEnMask_c) | (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gCcaReq_Continuous_Mode2_c             ((gContinuousEnMask_c)| (gCcaEnMask_c) | (gCcaCCA_MODE2_c << gCcaTypePos_c) )
#define gCcaReq_Slotted_Mode1_c                ( (gSlottedEnMask_c)  | (gCcaEnMask_c) | (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gCcaReq_Slotted_Mode2_c                ( (gSlottedEnMask_c)  | (gCcaEnMask_c) | (gCcaCCA_MODE2_c << gCcaTypePos_c) )
#define gCcaReq_Unslotted_Mode1_c              (                       (gCcaEnMask_c) | (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gCcaReq_Unslotted_Mode2_c              (                       (gCcaEnMask_c) | (gCcaCCA_MODE2_c << gCcaTypePos_c) )
#define gCcaReq_Ed_c                           (                       (gCcaEnMask_c) | (gCcaED_c        << gCcaTypePos_c) )
#define gCcaReq_Mode1_c                        (                       (gCcaEnMask_c) | (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gCcaReq_Mode2_c                        (                       (gCcaEnMask_c) | (gCcaCCA_MODE2_c << gCcaTypePos_c) )
#define gCcaReq_Continuous_Default_c           ((gContinuousEnMask_c)| (gCcaEnMask_c) | (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gCcaReq_Slotted_Default_c              ( (gSlottedEnMask_c)  | (gCcaEnMask_c) | (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gCcaReq_Default_c                      (                       (gCcaEnMask_c) | (gCcaCCA_MODE1_c << gCcaTypePos_c) )

// argument definitions for Tx settings when calling PhyPdDataRequest()
#define gDataReq_NoAck_NoCca_Slotted_c         ( (gSlottedEnMask_c) |                                    (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gDataReq_NoAck_NoCca_Unslotted_c       (                                                         (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gDataReq_NoAck_Cca_Slotted_c           ( (gSlottedEnMask_c) |                   (gCcaEnMask_c) | (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gDataReq_NoAck_Cca_Unslotted_c         (                                        (gCcaEnMask_c) | (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gDataReq_Ack_NoCca_Slotted_c           ( (gSlottedEnMask_c) | (gAckReqMask_c) |                  (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gDataReq_Ack_NoCca_Unslotted_c         (                      (gAckReqMask_c) |                  (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gDataReq_Ack_Cca_Slotted_c             ( (gSlottedEnMask_c) | (gAckReqMask_c) | (gCcaEnMask_c) | (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gDataReq_Ack_Cca_Unslotted_c           (                      (gAckReqMask_c) | (gCcaEnMask_c) | (gCcaCCA_MODE1_c << gCcaTypePos_c) )

#define gDataReq_NoAck_NoCca_Slotted_Mode1_c   ( (gSlottedEnMask_c) |                                    (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gDataReq_NoAck_NoCca_Unslotted_Mode1_c (                                                         (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gDataReq_NoAck_Cca_Slotted_Mode1_c     ( (gSlottedEnMask_c) |                   (gCcaEnMask_c) | (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gDataReq_NoAck_Cca_Unslotted_Mode1_c   (                                        (gCcaEnMask_c) | (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gDataReq_Ack_NoCca_Slotted_Mode1_c     ( (gSlottedEnMask_c) | (gAckReqMask_c) |                  (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gDataReq_Ack_NoCca_Unslotted_Mode1_c   (                      (gAckReqMask_c) |                  (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gDataReq_Ack_Cca_Slotted_Mode1_c       ( (gSlottedEnMask_c) | (gAckReqMask_c) | (gCcaEnMask_c) | (gCcaCCA_MODE1_c << gCcaTypePos_c) )
#define gDataReq_Ack_Cca_Unslotted_Mode1_c     (                      (gAckReqMask_c) | (gCcaEnMask_c) | (gCcaCCA_MODE1_c << gCcaTypePos_c) )

#define gDataReq_NoAck_NoCca_Slotted_Mode2_c   ( (gSlottedEnMask_c) |                                    (gCcaCCA_MODE2_c << gCcaTypePos_c) )
#define gDataReq_NoAck_NoCca_Unslotted_Mode2_c (                                                         (gCcaCCA_MODE2_c << gCcaTypePos_c) )
#define gDataReq_NoAck_Cca_Slotted_Mode2_c     ( (gSlottedEnMask_c) |                   (gCcaEnMask_c) | (gCcaCCA_MODE2_c << gCcaTypePos_c) )
#define gDataReq_NoAck_Cca_Unslotted_Mode2_c   (                                        (gCcaEnMask_c) | (gCcaCCA_MODE2_c << gCcaTypePos_c) )
#define gDataReq_Ack_NoCca_Slotted_Mode2_c     ( (gSlottedEnMask_c) | (gAckReqMask_c) |                  (gCcaCCA_MODE2_c << gCcaTypePos_c) )
#define gDataReq_Ack_NoCca_Unslotted_Mode2_c   (                      (gAckReqMask_c) |                  (gCcaCCA_MODE2_c << gCcaTypePos_c) )
#define gDataReq_Ack_Cca_Slotted_Mode2_c       ( (gSlottedEnMask_c) | (gAckReqMask_c) | (gCcaEnMask_c) | (gCcaCCA_MODE2_c << gCcaTypePos_c) )
#define gDataReq_Ack_Cca_Unslotted_Mode2_c     (                      (gAckReqMask_c) | (gCcaEnMask_c) | (gCcaCCA_MODE2_c << gCcaTypePos_c) )


#define gPHY_IRQ_SEQ_Flag_c  0x00000001
#define gPHY_IRQ_RX_Flag_c   0x00000002
#define gPHY_IRQ_TX_Flag_c   0x00000004
#define gPHY_IRQ_FF_Flag_c   0x00000008



/*****************************************************************************
*                             Public type definitions                        *
*****************************************************************************/

typedef struct phyPacket_tag {
  uint8_t frameLength;
  uint8_t data[gPhyMaxDataLength_c];
} phyPacket_t;

typedef struct phyRxParams_tag {
  zbClock24_t timeStamp;
  uint8_t     linkQuality;
} phyRxParams_t;

typedef struct phyTxParams_tag {
  bool_t  useStandaloneCcaBeforeTx;
  uint8_t numOfCca;
  uint8_t phyTxMode;
} phyTxParams_t;


/*****************************************************************************
*                             Public macros                                  *
*****************************************************************************/

#define PhyGetSeqState()                     PhyPpGetState()
#define PhyPlmeForceTrxOffRequest()          PhyAbort()

#define PhyPpSetPanId(arg)                   PhyPpSetPanIdPAN0(arg)
#define PhyPpSetLongAddr(arg)                PhyPpSetLongAddrPAN0(arg)
#define PhyPpSetShortAddr(arg)               PhyPpSetShortAddrPAN0(arg)
#define PhyPpSetMacRole(arg)                 PhyPpSetMacRolePAN0(arg)

#define PhyPlmeGetCurrentChannelRequest()    PhyPlmeGetCurrentChannelRequestPAN0()
#define PhyPlmeSetCurrentChannelRequest(arg) PhyPlmeSetCurrentChannelRequestPAN0(arg)

#define PhyPlmeEdRequest()                   PhyPlmeCcaEdRequest(gCcaED_c << gCcaTypePos_c)
#define PhyPlmeCcaRequest(arg)               PhyPlmeCcaEdRequest(arg)


/*****************************************************************************
*                             Public prototypes                              *
*****************************************************************************/

// PHY Packet Processor

/*---------------------------------------------------------------------------
 * Name: PhyPpSetPromiscuous
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetPromiscuous
(
  bool_t mode
);

/*---------------------------------------------------------------------------
* Name: PhySetActivePromState()
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
void PhySetActivePromiscuous
(
bool_t state
);

/*---------------------------------------------------------------------------
* Name: PhyGetActivePromiscuous()
* Description: -
* Parameters: -
* Return: - TRUE/FALSE
*---------------------------------------------------------------------------*/
bool_t PhyGetActivePromiscuous
(
void
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetPanIdPAN0
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetPanIdPAN0
(
  uint8_t *pPanId
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetPanIdPAN1
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetPanIdPAN1
(
  uint8_t *pPanId
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetShortAddrPAN0
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetShortAddrPAN0
(
  uint8_t *pShortAddr
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetShortAddrPAN1
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetShortAddrPAN1
(
  uint8_t *pShortAddr
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetLongAddrPAN0
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetLongAddrPAN0
(
  uint8_t *pLongAddr
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetLongAddrPAN1
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetLongAddrPAN1
(
  uint8_t *pLongAddr
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetMacRolePAN0
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetMacRolePAN0
(
  bool_t macRole
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetMacRolePAN1
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetMacRolePAN1
(
  bool_t macRole
);

/*---------------------------------------------------------------------------
 * Name: PhyPpIsTxAckDataPending
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PhyPpIsTxAckDataPending
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyPpIsRxAckDataPending
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PhyPpIsRxAckDataPending
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyPpIsPollIndication
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PhyPpIsPollIndication
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyPp_AddToIndirect
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPp_AddToIndirect
(
  uint8_t index,
  uint8_t *pPanId,
  uint8_t *pAddr,
  uint8_t AddrMode
);

/*---------------------------------------------------------------------------
 * Name: PhyPp_RemoveFromIndirect
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPp_RemoveFromIndirect
(
  uint8_t index
);

/*---------------------------------------------------------------------------
 * Name: PhyPpReadLatestIndex
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPpReadLatestIndex
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyPpGetState
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPpGetState
(
  void
);


/*---------------------------------------------------------------------------
 * Name: PhyAbort
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyAbort
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyInit
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyInit
(
  void
);

// PHY PLME & DATA primitives

/*---------------------------------------------------------------------------
 * Name: PhyPdDataRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPdDataRequest
(
  phyPacket_t *pTxPacket,
  uint8_t phyTxMode,
  phyRxParams_t *pRxParams
);

/*---------------------------------------------------------------------------
 * Name: PhyPlmeRxRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeRxRequest
(
  phyPacket_t *pRxData,
  uint8_t phyRxMode,
  phyRxParams_t *pRxParams
);

/*---------------------------------------------------------------------------
 * Name: PhyPlmeCcaEdRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeCcaEdRequest
(
  uint8_t ccaParam
);

/*---------------------------------------------------------------------------
 * Name: PhyPlmeSetCurrentChannelRequestPAN0
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeSetCurrentChannelRequestPAN0
(
  uint8_t channel
);

/*---------------------------------------------------------------------------
 * Name: PhyPlmeSetCurrentChannelRequestPAN1
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeSetCurrentChannelRequestPAN1
(
  uint8_t channel
);

/*---------------------------------------------------------------------------
 * Name: PhyPlmeGetCurrentChannelRequestPAN0
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeGetCurrentChannelRequestPAN0
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyPlmeGetCurrentChannelRequestPAN1
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeGetCurrentChannelRequestPAN1
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyPlmeSetPwrLevelRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeSetPwrLevelRequest
(
  uint8_t pwrStep
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetCcaThreshold
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetCcaThreshold
(
  uint8_t ccaThreshold
);

// PHY Time

/*---------------------------------------------------------------------------
 * Name: PhyTimeSetEventTrigger
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeSetEventTrigger
(
  uint16_t startTime
);

/*---------------------------------------------------------------------------
 * Name: PhyTimeSetEventTimeout
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeSetEventTimeout
(
  zbClock24_t *pEndTime
);

/*---------------------------------------------------------------------------
 * Name: PhyTimeGetEventTimeout
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint32_t PhyTimeGetEventTimeout( void );

/*---------------------------------------------------------------------------
 * Name: PhyTimeReadClock
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeReadClock
(
  zbClock24_t *pRetClk
);

/*---------------------------------------------------------------------------
 * Name: PhyTimeDisableEventTimeout
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeDisableEventTimeout
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyTimeSetWakeUpTime
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeSetWakeUpTime
(
  zbClock24_t *pWakeUpTime
);

/*---------------------------------------------------------------------------
 * Name: PhyTimeIsWakeUpTimeExpired
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PhyTimeIsWakeUpTimeExpired
(
  void
);

// PHY ISR

/*---------------------------------------------------------------------------
 * Name: PhyPassTxParams()
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPassTxParams
(
  phyTxParams_t * pTxParam
);

/*---------------------------------------------------------------------------
 * Name: PhyPassRxParams()
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPassRxParams
(
  phyRxParams_t * pRxParam
);

/*---------------------------------------------------------------------------
 * Name: PhyPassRxDataPtr()
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPassRxDataPtr
(
  phyPacket_t * pRxData
);

/*---------------------------------------------------------------------------
 * Name: PhyIsrTimeoutCleanup
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyIsrTimeoutCleanup
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyIsrSeqCleanup
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyIsrSeqCleanup
(
  void
);

// PHY 2 MAC
/*****************************************************************************
* PhyPlmeSyncLossIndication function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPlmeSyncLossIndication
(
  void
);


/*****************************************************************************
* PhyTimeRxTimeoutIndication function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyTimeRxTimeoutIndication
(
  void
);

/*****************************************************************************
* PhyTimeStartEventIndication function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyTimeStartEventIndication
(
  void
);

/*****************************************************************************
* PhyPlmeCcaConfirm function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPlmeCcaConfirm
(
  bool_t channelInUse
);

/*****************************************************************************
* PhyPlmeEdConfirm function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPlmeEdConfirm
(
  uint8_t energyLevel
);

/*****************************************************************************
* PhyPdDataConfirm function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPdDataConfirm
(
  void
);

/*****************************************************************************
* PhyPdDataIndication function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPdDataIndication
(
void
);

/*****************************************************************************
* PhyPlmeFilterFailRx function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPlmeFilterFailRx
(
  void
);

/*****************************************************************************
* PhyPlmeRxSfdDetect function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPlmeRxSfdDetect
(
  uint8_t frameLen
);

/*****************************************************************************
* PhyPlmePreprocessData function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPlmePreprocessData
(
  uint8_t* pData
);

/*****************************************************************************
* PhyGetLastRxLqiValue function
*
* Interface assumptions:
*
* Return Value:
* The LQI value for the last received packet
*****************************************************************************/
uint8_t PhyGetLastRxLqiValue
(
void
);

// PHY TESTING

/*****************************************************************************
* PhySetDtsMode function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhySetDtsMode
(
  uint8_t mode
);

/*****************************************************************************
* PhyEnableBER function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyEnableBER
(
  void
);

/*****************************************************************************
* PhyDisableBER function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyDisableBER
(
  void
);

/*****************************************************************************
* PhyGetRandomNo function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyGetRandomNo
(
  uint32_t *pRandomNo
);

// DUAL PAN

/*****************************************************************************
* PhyPpSetDualPanAuto function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPpSetDualPanAuto
(
  bool_t mode
);

/*****************************************************************************
* PhyPpSetDualPanDwell function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPpSetDualPanDwell
(
  uint8_t
);

/*****************************************************************************
* PhyPpGetDualPanRemain function
*
* Interface assumptions:
*
* Return Value:
* The remaining time until a channel switch will occure
*****************************************************************************/
uint8_t PhyPpGetDualPanRemain
(
  void
);

/*****************************************************************************
* PhyPpSetDualPanSamLvl function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPpSetDualPanSamLvl
(
  uint8_t
);

/*****************************************************************************
* PhyPpGetDualPanSamLvl function
*
* Interface assumptions:
*
* Return Value:
* The level at which the HW queue is split for the two PANs
*****************************************************************************/
uint8_t PhyPpGetDualPanSamLvl
(
  void
);

/*****************************************************************************
* PhyPpSetDualPanSamLvl function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPpSetDualPanActiveNwk
(
  uint8_t
);

/*****************************************************************************
* PhyPpGetDualPanActiveNwk function
*
* Interface assumptions:
*
* Return Value:
* The current NWK on which the PHY is operating
*****************************************************************************/
uint8_t PhyPpGetDualPanActiveNwk
(
  void
);

/*****************************************************************************
* PhyPpGetPanOfRxPacket function
*
* Interface assumptions:
*
* Return Value:
* The PAN on which the packet was received (can be receiced on both PANs)
*****************************************************************************/
uint8_t PhyPpGetPanOfRxPacket
(
  void
);

/*****************************************************************************
* PhyPlmeSetFADStateRequest function
*
* Interface assumptions: state
*
* Return Value: gPhySuccess
*
* Description: Enable the FAD function (FAD_EN bit)
*****************************************************************************/
uint8_t PhyPlmeSetFADStateRequest(bool_t state);

/*****************************************************************************
* PhyPlmeSetFADThresholdRequest function
*
* Interface assumptions: FADThreshold
*
* Return Value: gPhySuccess
*
* Description: Correlator threshold at which the FAD will select the antenna
*****************************************************************************/
uint8_t PhyPlmeSetFADThresholdRequest(uint8_t FADThreshold);

/*****************************************************************************
* PhyPlmeSetANTXStateRequest function
*
* Interface assumptions: state
*
* Return Value: gPhySuccess
*
* Description: ANTX_IN - FAD Antenna start when FAD_EN = 1 or antenna selected 
*              when FAD_EN=0
*****************************************************************************/
uint8_t PhyPlmeSetANTXStateRequest(bool_t state);

/*****************************************************************************
* PhyPlmeGetANTXStateRequest function
*
* Interface assumptions: none
*
* Return Value: Chosen antenna by the FAD (FAD_EN = 1) of copy of ANTX_IN
*
* Description: Antenna selected in FAD of non-FAD mode
*****************************************************************************/
uint8_t PhyPlmeGetANTXStateRequest(void);

/*****************************************************************************
* PhyPlmeSetLQIModeRequest function
*
* Interface assumptions: none
*
* Return Value: gPhySuccess
*
* Description: Choose LQI Mode: 1 - LQI Based on RSSI, 
*                               0 - LQI Based on Correlation Peaks
*****************************************************************************/
uint8_t PhyPlmeSetLQIModeRequest(uint8_t lqiMode);

/*****************************************************************************
* PhyPlmeGetRSSILevelRequest function
*
* Interface assumptions: none
*
* Return Value: RSSI level
*
* Description: Returns the RSSI level value, refreshed every 125us
*****************************************************************************/
uint8_t PhyPlmeGetRSSILevelRequest(void);

#ifdef __cplusplus
}
#endif

#endif /* __PHY_H__ */
