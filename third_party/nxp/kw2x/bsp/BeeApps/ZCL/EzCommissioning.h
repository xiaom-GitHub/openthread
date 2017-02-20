/*! @file 	EzCommissioning.h
 *
 * @brief	Types, definitions and prototypes for the ZigBee EZ-Mode Comissioning implementition.
 *
 * @copyright Copyright(c) 2013, Freescale, Inc. All rights reserved.
 *
 * @license	Redistribution and use in source and binary forms, with or without modification,
 *			are permitted provided that the following conditions are met:
 *
 *			o Redistributions of source code must retain the above copyright notice, this list
 *			of conditions and the following disclaimer.
 *
 *			o Redistributions in binary form must reproduce the above copyright notice, this
 *			list of conditions and the following disclaimer in the documentation and/or
 *			other materials provided with the distribution.
 *
 *			o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *			contributors may be used to endorse or promote products derived from this
 *			software without specific prior written permission.
 *
 *			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *			ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *			WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *			DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 *			ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *			(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *			LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *			ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *			(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *			SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**********************************************************************
***********************************************************************
* Public Macros
***********************************************************************
***********************************************************************/

/**********************************************************************
***********************************************************************
* Public Prototypes
***********************************************************************
***********************************************************************/

/**********************************************************************
***********************************************************************
* Public type definitions
***********************************************************************
***********************************************************************/
#ifndef _ZCL_EZCOMMISSIONING_H
#define _ZCL_EZCOMMISSIONING_H

#if gASL_EnableEZCommissioning_d

#define	gEzCommissioning_FactoryFresh_c			(1<<0)
#define	gEzCommissioning_NetworkSteering_c		(1<<1)
#define	gEzCommissioning_FindingAndBinding_c	        (1<<2)

/* Event definition for the Ez Commissioning state machine. */
#define         gStart_c                        (1<<0)
#define         gFormNetwork_c                  (1<<1)
#define         gJoinNetwork_c                  (1<<2)
#define         gUpdatePollRate_c               (1<<3)
#define         gDeviceInNetwork_c             	(1<<4)
#define         gStartIdentify_c                (1<<5)
#define         gSendIdentifyReq_c              (1<<6)
#define         gReceivedIdentifyRsp_c          (1<<7)
#define         gIdentifyEnd_c                  (1<<8)
#define         gSendSimpleDescReq_c            (1<<9)
#define         gReceivedSimpleDescRsp_c        (1<<10)
#define         gPerformBind_c                  (1<<11)
#define         gPerformGroupBind_c             (1<<12)
#define         gJoiningfailed_c                (1<<13)
#define         gReceivedIEEEAddress_c          (1<<14)
#define 	gUpdateDevice_c			(1<<15)	


/*EZ mode values */
#define EZScanFormFallbackTime_c   15 /* <seconds> : how long a device scans */
#define EZScanJoinTime_c            3 /* <minutes> : time to scan for a network */
#define EZOpenNetworkTime_c         3 /* <minutes> : EZMode Opener sets PermitJoining to this constant */
#define EZIdQueryTime_c             5 /* <seconds> : Interval at which Identify Queries are broadcasted by an Initiator device */
#define EZIdWindowTime_c            3 /* <minutes> : Time window for an EZ-Mode endpoint to stay in EZ Mode Identify state */
#define EZIdSessionTime            10 /* <minutes> : Timeout to cancel bindings if EZ Mode Session not closed. Set through OpenSession cmd */   


#define EZCommissioning_GroupAddr_c             0x34,0x12
#define EZCommissioning_AutoCloseTimeout_c      7000

#define gEZCommisioning_TempDataLength_d	100         /* maximum temp buffer length used for EZ mode commissioning procedure */



#if gASL_EnableEZCommissioning_Initiator_d
/*  
  The maximum no of clusters used in the EZ mode commissioning Binding step (per device).
  This value should be set according with the gMaximumApsBindingTableEntries_c and gMaximumApsBindingTableClusters_c 
*/
#define EZCommissioning_MaxNoOfBoundClusterPerDevice_d  2*gMaximumApsBindingTableClusters_c  /* allocate maximum 2 entries from binding Table per device */
#endif

/*
   Primary device type for Combo devices. In case joining fails the device
   type will be set to Zc and a new network will be formed
*/
#define EZCommissioning_PrimaryDeviceType_d   gZdoStartMode_Zr_c

/* Number of attempts to look for a network */
#define EZCommissioning_NetworkDiscoveryAttempts_c    0x03
/**********************************************************************
***********************************************************************
* Public Memory Declarations
***********************************************************************
***********************************************************************/

typedef PACKED_STRUCT EZ_ModeClusterList_tag
{
  /*Number of Input Cluster ID Supported by the application */
  zbCounter_t   appNumInClusters;
  /*Place Holder for the list of Input Cluster ID*/
  uint8_t      *pAppInClusterList;
  /*Number of Output Cluster ID Supported by the application*/
  zbCounter_t   appNumOutClusters;
  /*Place Holder for the list of Output Cluster ID*/
  uint8_t      *pAppOutClusterList;
}EZ_ModeClusterList_t;

extern EZ_ModeClusterList_t gEZModeClusterList;
extern tsTaskID_t gEzCmsTaskId;

extern uint8_t gEZCommissioningTempData[];
extern uint8_t gEZCommissioningPrimaryDeviceType;

extern bool_t  gEZCommissioningUseGroup;
extern event_t gEZCommissioning_LastEvent;

/**********************************************************************
***********************************************************************
* Public prototypes
***********************************************************************
**********************************************************************/
/*!
 * @fn 		void EZCommissioning_Init(void) 
 *
 * @brief	Init EZ-Mode Commissioning Procedure
 *
 */
extern void EZCommissioning_Init(void);
/*!
 * @fn 		void EZComissioning_Start(uint8_t startMode)
 *
 * @brief	Start EZ-Mode Commissioning Procedure
 *
 */
 
extern void EZComissioning_Start(uint8_t startMode);
/*!
 * @fn 		void EZCommissioning_Reset(void)
 *
 * @brief	Perform EZ-Mode Commissioning Reset
 *
 */
 
extern void EZCommissioning_Reset(void);

/*!
 * @fn 		void EZCommissioning_VerifyIEEEaddrRsp(zbIeeeAddr_t  aIeeeAddr)
 *
 * @brief	Check and temporarily store the target IEEEaddress(used into a multi-hop solution)
 *
 */
#if gASL_EnableEZCommissioning_Initiator_d 
void EZCommissioning_VerifyIEEEaddrRsp(zbIeeeAddr_t  aIeeeAddr);
#endif
#endif /* gASL_EnableEZCommissioning_d*/

/**********************************************************/
#endif /*_ZCL_EZCOMMISSIONING_H*/
