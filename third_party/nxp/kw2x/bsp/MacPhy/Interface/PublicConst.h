/************************************************************************************
* This header file is provided as part of the interface to the Freescale 802.15.4
* MAC and PHY layer.
*
* The file defines constant values given by the 802.15.4 specification.
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

#ifndef _PUB_CONST_H_
#define _PUB_CONST_H_

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

  // MAC enumerations (D18, table 64, p. 109 + table 68 p. 125)
#define gSuccess_c                 0x00
#define gPanAtCapacity_c           0x01
#define gPanAccessDenied_c         0x02
#define gCounterError_c            0xDB
#define gImproperKeyType_c         0xDC
#define gImproperSecurityLevel_c   0xDD
#define gUnsupportedLegacy_c       0xDE
#define gUnsupportedSecurity_c     0xDF
#define gBeaconLoss_c              0xE0
#define gChannelAccessFailure_c    0xE1
#define gDenied_c                  0xE2
#define gDisableTrxFailure_c       0xE3
#ifndef gMAC2006_d
#define gFailedSecurityCheck_c     0xE4
#else
#define gSecurityError_c           0xE4
#endif //gMAC2006_d
#define gFrameTooLong_c            0xE5
#define gInvalidGts_c              0xE6
#define gInvalidHandle_c           0xE7
#define gInvalidParameter_c        0xE8
#define gNoAck_c                   0xE9
#define gNoBeacon_c                0xEA
#define gNoData_c                  0xEB
#define gNoShortAddress_c          0xEC
#define gOutOfCap_c                0xED
#define gPanIdConflict_c           0xEE
#define gRealignment_c             0xEF
#define gTransactionExpired_c      0xF0
#define gTransactionOverflow_c     0xF1
#define gTxActive_c                0xF2
#define gUnavailableKey_c          0xF3
#define gUnsupportedAttribute_c    0xF4
#ifdef gMAC2006_d
  #define gInvalidAddress_c          0xF5
  #define gOnTimeTooLong_c           0xF6
  #define gPastTime_c                0xF7
  #define gTrackingOff_c             0xF8
  #define gInvalidIndex_c            0xF9
  #define gLimitReached_c            0xFA
  #define gReadOnly_c                0xFB
  #define gScanInProgress_c          0xFC
  #define gSuperframeOverlap_c       0xFD
#endif //gMAC2006_d  


/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

#endif /* _PUB_CONST_H_ */
