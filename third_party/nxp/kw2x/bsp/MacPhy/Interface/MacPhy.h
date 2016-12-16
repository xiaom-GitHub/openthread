/************************************************************************************
* Header file for interface between MAC and PHY layer.
* The functionality declared in this file all resides in the PHY layer of ZigBee
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

#ifndef _MACPHY_H_
#define _MACPHY_H_
/************************************************************************************
*************************************************************************************
* Includes
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "MacPhyFLib.h"
#include "MsgSystem.h"

#include "Phy.h"

#ifdef __cplusplus
    extern "C" {
#endif

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

  // Get the offset from the beginning of a structure to the member variable
#define GetRelAddr(strct, member) ((uint8_t)((uint32_t)&(((strct *)(void *)0)->member))) 
#define GetRelAddr_16(strct, member) ((uint16_t)((uint32_t)&(((strct *)(void *)0)->member))) 

/************************************************************************************
*************************************************************************************
* New types and defines! Everybody must change to these!
*************************************************************************************
************************************************************************************/

// Allign all structs to 1 byte
typedef PACKED_STRUCT txPacket_tag 
{
  uint8_t frameLength;
  uint8_t txInfo;
  PACKED_STRUCT
      {
      uint8_t csmaCaNb;
      uint8_t csmaCaBe;
      uint8_t txCount;
      } csmaAndTx;
      uint16_t expireTime;
      uint8_t msduHandle;
      uint8_t txData[gMaxRxTxDataLength_c];
#ifdef gMAC2006_d
      uint8_t securityLevel;
      uint8_t keyIdMode;
      uint8_t keySource[8];
      uint8_t keyIndex;
#endif //gMAC2006_d
#if gDualPanEnabled_d
      uint8_t dualPanInfo;
#endif
} txPacket_t;

#ifdef I_AM_A_SNIFFER
 typedef PACKED_STRUCT rxPacketSniffer_tag 
{
    uint8_t frameLength;
    uint8_t linkQuality;
    uint8_t headerLength;
    uint8_t timeStampAbelLSB1;
    uint8_t timeStampAbelLSB0;
    uint8_t timeStampMCU;
    uint8_t rxData[gMaxRxTxDataLength_c];
    zbClock24_t timeStamp;  
  }  rxPacket_t;
//  typedef struct rxPacketSniffer_tag rxPacket_t;
#else // I_AM_A_SNIFFER
typedef PACKED_STRUCT rxPacket_tag 
{
    uint8_t frameLength;
    uint8_t linkQuality;
    uint8_t headerLength;
    uint8_t rxData[gMaxRxTxDataLength_c];
    zbClock24_t timeStamp;
#ifdef gMAC2006_d
    uint8_t securityLevel;
    uint8_t keyIdMode;
    uint8_t keySource[8];
    uint8_t keyIndex;
#endif //gMAC2006_d
#if gDualPanEnabled_d
    uint8_t dualPanInfo;
#endif
} rxPacket_t;  
#endif // I_AM_A_SNIFFER


  // For accessing fixed location fields in (MPDU/SPDU) rx/txData in rx/txPacket_t:
#define gFrameControlLsbPos_c   (GetRelAddr(hdrGenericHeaderType_t, frameControlLsb)) // Position in rx/txData in rx/txPacket_t
#define gFrameControlMsbPos_c   (GetRelAddr(hdrGenericHeaderType_t, frameControlMsb)) // Position in rx/txData in rx/txPacket_t
#define gSeqNumberPos_c         (GetRelAddr(hdrGenericHeaderType_t, seqNr)) // Position in rx/txData in rx/txPacket_t
#define gAddrFieldsStartPos_c   (GetRelAddr(hdrGenericHeaderType_t, addrFieldsStart)) // Position in rx/txData in rx/txPacket_t

  // For accessing txInfo info:
#define gTxInfoCommandFrameFlag_c               ((uint8_t)(1 << 0))
#define gTxInfoDataReqFlag_c                    ((uint8_t)(1 << 1))
#define gTxInfoIndirectFlag_c                   ((uint8_t)(1 << 2))
#define gTxInfoConfirmFlag_c                    ((uint8_t)(1 << 3)) // If set then a data indication will be sent after the frame has completed.
#define gTxInfoGtsFlag_c                        ((uint8_t)(1 << 4))
#define gTxInfoHiPriDataFlag_c                  ((uint8_t)(1 << 5)) // If set then this marks a high priority data frame that should be placed first in the direct queue
#define gTxInfoTreemodeRouteFlag_c              ((uint8_t)(1 << 6)) 
#define gTxInfoFramePendingModifyAllowFlag_c    ((uint8_t)(1 << 7))

  // Difference between Frame length (in rx/txPacket_t) and total packet legth over
  // the air is Preamble sequence, Start of frame delimiter and the Frame length
  // fields
#define gFrameLengthToTotalPhyLengthDiff_c (6)

#define gMacDataAndCommandThreshold_c 128
/*ClockManager defines*/

#define aCCATime_b (13) //[bytes]

#define aMinUnslottedRxSize_b (11+4) //[bytes]
#define aAckUnslottedSize_b  (11) //[bytes]
#define gEndtimeGuard_c (2) // [bytes]
#define gEarlyRxStart_c (0) // [symbols] @RNI: special GTS fix
#define aAckSlottedRxSlack aUnitBackoffPeriod_b // 10 [bytes], preferably 19 sym, but rounding is ok


  // Events to be used in the status field of the action_t.
  // Valid only for the SeqActionCompleteInd and SeqActionFailInd primitives.
  // NOT used in the SeqActionStartInd!!
enum {
  gSeqMemAcUndefined_c = gMacDataAndCommandThreshold_c, // Dummy: Only used for error handling. 
                              // Enum must start with 128, because data frames uses the Id field
                              // as the length field (frame lengths are always less than 128). 
                              // MLME to MEM id's are located in the range from 128 to 255.
                              // Values from 0 to 127 denotes a MAC frame of that size.
                              // This is taken advantage of in the MEM input handler.
  gSeqMemAcSuccess_c,
  gSeqMemAcRxTimeout_c,       // Rx ended with timeout
  gSeqMemAcRxPollNoTxData_c,
  gSeqMemAcTxPollNoRxData_c,
  gSeqMemAcTxChannelBusy_c,// CCA response (for TX too)
  gSeqMemAcTxAckMissing_c, // Tx data completed without acknowledge
  gSeqMemStatusLastEntry_c // THIS MUST BE THE LAST ENTRY IN THE ENUMERATION
};

  // Must NOT overlap other MEM event opcodes (gMlmeMemScanEdReq_c > gSeqMemStatusLastEntry_c)
enum {
  gMlmeMemScanEdReq_c = 180,  // The four Scan-related opcodes MUST follow each other
  gMlmeMemScanActiveReq_c,    // in the following order: energy, active, passive, orphan.
  gMlmeMemScanPassiveReq_c,
  gMlmeMemScanOrphanReq_c,
  gMlmeMemScanAbortReq_c,
  gMlmeMemStartReq_c,
  gMlmeMemTimerReq_c,
  gMlmeMemRxOnWhenIdleChanged_c,
  gMlmeMemSyncReq_c,
  gMlmeMemRxEnableReq_c,
  gMemCmdIdLastEntry // MUST BE THE LAST ENTRY
};
  // This is internal MEM events. Add-on to the memSeqStatus_t and gMemEventId_t 
  // structures.
  // Must NOT overlap other MEM event opcodes (gMemIntNoEvent_c > gMemCmdIdLastEntry)
enum {
  gMemIntNoEvent_c = 200, // A dummy event used for event buffering.
  gSeqMemActionFail_c, 
  gSeqMemActionFinish_c,
  gSeqMemDataInd_c,
  gMcpsDirectDataOrPollReq_c,
  gMemLwrAbortAll,
  gMemLwrTrigDataStateMachine_c,
  gMemCtrlDataStateMachineDone_c,
  gMemLwrTrigBeaconStateMachine_c,
  gMemLwrBeaconStateMachineDone_c,
  gMemCtrlAbortAllDone_c,
  gMemCtrlEdScanDone_c,
  gMemCtrlScanDone_c,
  gMemCtrlTimeout_c,
  gMemCtrlRxEnableDone_c,
  gMemIntLastEntry_c
};
typedef uint8_t memEventId_t;

  // Note: This enum contains all possible "timer related" status values. The timer
  // is currently only used during association. More status values can be added as
  // needed (currently only a single value is used although two are defined!)
enum {
  gTimerSuccess_c = 0,
  gTimerFailure_c
};


  // A header type with the generic information
typedef PACKED_STRUCT hdrGenericHeaderType_tag 
{
  uint8_t frameControlLsb;
  uint8_t frameControlMsb;
  uint8_t seqNr;
  uint8_t addrFieldsStart;
} hdrGenericHeaderType_t;


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// New types and defines block end...
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

#define CHANNELS_SUPPORTED_31_24 (0x07)
#define CHANNELS_SUPPORTED_23_16 (0xFF)
#define CHANNELS_SUPPORTED_15_8  (0xF8)
#define CHANNELS_SUPPORTED_7_0   (0x00)


/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/


/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/


/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

#define gRxDataIndexOffset_c (GetRelAddr(rxPacket_t, rxData))

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

extern uint8_t gCurrentPALevel;
extern uint8_t *gpaPowerLevelLimits;

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
// Mac-Phy Interface functions

bool_t  IsCurrentActionExtRx(void);
bool_t  IsCurrentActionAutoRx(void);

void    InitializeMac(bool_t resetPib);

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
}
#endif

/***********************************************************************************/
#endif /* _MACPHY_H_  */
