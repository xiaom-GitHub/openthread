/*! @file 	  EzCommissioning.c
 *
 * @brief	  This source file describes specific functionality implemented
 *			  for the EZ-Mode Commissioning method.
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

#include "EmbeddedTypes.h"
#include "NV_Data.h"
#include "NVM_Interface.h"
#include "zigbee.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "BeeStackParameters.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "ZdoApsInterface.h"
#include "ApsMgmtInterface.h"
#include "BeeAppInit.h"
#include "BeeCommon.h"
#include "ASL_UserInterface.h"
#include "ZCL.h"
#include "ZdpManager.h"
#include "EndPointConfig.h"
#include "ASL_ZdpInterface.h"
#include "EzCommissioning.h"



#if gASL_EnableEZCommissioning_d
/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define EZCommissioning_GetState()      ( gEZCommissioningState )
#define EZCommissioning_SetState(state) ( gEZCommissioningState = (state) )

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
#if gASL_EnableEZCommissioning_Initiator_d
static void EZComissioning_SendIdentifyQueryReq(uint16_t time, bool_t senderInIdentify);
static void EZCommissioning_SendBind(void);
#if gSimple_Desc_req_d
static void EZComissioning_SendSimpleDescReq(void);
#endif
#endif
static void EZCommissioning_StateMachine(event_t events);
#if gEndDevCapability_d || gComboDeviceCapability_d
static void EZCommissioning_UpdatePollRate(bool_t fastPollRate);
#endif
#if gASL_EnableEZCommissioning_Initiator_d
static bool_t EZComissioning_DevBindMatchClusters(void);
#if gASL_EzCommissioning_EnableGroupBindCapability_d
static void EZCommissioning_PerformGroupBind(void);
#endif
#endif
void EZCommissioning_AutoCloseTimerCallback(tmrTimerID_t timerId);
void EZCommissioning_AutoClose(uint32_t timeout);

/*!
 * @fn 		void EZCommissioning_Reset(void)
 *
 * @brief	Perform Software Reset
 *
 */
extern void PWRLib_Reset(void);

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/* EZ Commissioning - Operating modes */
enum
{
  EZCommissioning_InitialState_c,
  EZCommissioning_NetworkSteering_c,
  EZCommissioning_Identify_c,
  EZCommissioning_Binding_c,
  EZCommissioning_GroupBinding_c,
  EZCommissioning_IdleState_c
};

enum
{
  EZCommissioning_BindTableNotAvailable_c,
  EZCommissioning_BindTableAvailable_c
};

#if gASL_EnableEZCommissioning_Initiator_d 
/* internal structure for EZcommissioning-device-bind */
typedef struct zdoEZComDBTable_tag
{
  uint8_t             aState;
  bool_t              waitForIeeeAddrRsp;
  zbNwkAddr_t         aBindingTarget;
  zbIeeeAddr_t        aSrcIeeeAddress;
  zbEndPoint_t        srcEndPoint;
  index_t             iMatchIndex;      /* does the other side match? Record the index in the EDB table that matches */
  zbClusterId_t       aMatchCluster[EZCommissioning_MaxNoOfBoundClusterPerDevice_d];  /* what input cluster matched? */
} zdoEZComDBTable_t;
#endif

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
extern uint16_t gZclIdentifyTime;
extern uint8_t  gZclIdentifyCommissioningState;

#if gASL_EnableEZCommissioning_Initiator_d  
extern zbClusterId_t gaZdpMatchedOutCluster[];
extern index_t giMatchIndex;

zdoEZComDBTable_t gaEZComDBTable;
#endif

tsTaskID_t gEzCmsTaskId = gTsInvalidTaskID_c;
tmrTimerID_t gEZCommissioningAutocloseTimerId = gTmrInvalidTimerID_c;
char gGroupName[] = {0x09, 0x54, 0x65, 0x73, 0x74, 0x47, 0x72, 0x6f, 0x75, 0x70};

bool_t  gEZCommissioningOneShoot;
bool_t  gEZCommissioningUseGroup = FALSE;
bool_t  gEZCommissioning_ResetDevice = FALSE;
event_t gEZCommissioning_LastEvent =  0x00;        

uint8_t gEZCommissioningTempData[gEZCommisioning_TempDataLength_d];	 
#if gComboDeviceCapability_d
uint8_t gEZCommissioningNetworkDiscoveryAttempts;
#endif
uint8_t gEZCommissioningState = EZCommissioning_IdleState_c;
uint8_t gEZCommissioningPrimaryDeviceType = EZCommissioning_PrimaryDeviceType_d;


/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

void EZCommissioning_Init(void);
void EZComissioning_Start(uint8_t startMode);

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private Debug stuff
*************************************************************************************
************************************************************************************/

/*!
 * @fn 		void EZCommissioning_Init(void) 
 *
 * @brief	Init EZ-Mode Commissioning Procedure
 *
 */
void EZCommissioning_Init(void)
{
  if(gEzCmsTaskId == gTsInvalidTaskID_c)
  {
    gEzCmsTaskId = TS_CreateTask(gTsFirstApplicationTaskPriority_c +2, EZCommissioning_StateMachine);
  }
  
  EZCommissioning_SetState(EZCommissioning_InitialState_c);
#if gASL_EnableEZCommissioning_Initiator_d  
  gaEZComDBTable.aState = EZCommissioning_BindTableAvailable_c;
#endif  
  gEZCommissioningAutocloseTimerId = TMR_AllocateTimer();
  gEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1] = 0xFF;
}

/*!
 * @fn 		void EZComissioning_Start(uint8_t startMode)
 *
 * @brief	Start EZ-Mode Commissioning Procedure
 *
 */
void EZComissioning_Start(uint8_t startMode)
{
  if(startMode & gEzCommissioning_FactoryFresh_c)
  {
	appState = 0;
	BeeApp_FactoryFresh();
    ZDO_StopEx(gZdoStopMode_ResetTables_c);
    gEZCommissioning_ResetDevice = TRUE;
    return;
  }
  if(startMode & gEzCommissioning_NetworkSteering_c)
  {
    gEZCommissioningOneShoot = FALSE;
    TS_SendEvent(gEzCmsTaskId, gStart_c);	
    if(!(gZclIdentifyCommissioningState & gZclCommissioningState_NetworkState_d))
    	BeeAppUpdateDevice(appEndPoint, gStartNetwork_c, 0, 0, NULL);
  }
  if(startMode & gEzCommissioning_FindingAndBinding_c)
  {
	if(startMode & gEzCommissioning_NetworkSteering_c)
	{
	  gEZCommissioningOneShoot = TRUE;
	}
	/* No Network Steering, No Factory Fresh. Check if the device is on the network before moving to Finding and Binding */
	else if(!(startMode & gEzCommissioning_FactoryFresh_c) && (gZclIdentifyCommissioningState & gZclCommissioningState_NetworkState_d))
	{
	  EZCommissioning_SetState(EZCommissioning_Identify_c);
	  TS_SendEvent(gEzCmsTaskId, gStartIdentify_c);
	}
	/* Finding and Binding with Network Fresh but without Network Steering doesn't do anything */
  }
}

/*!
 * @fn 		void EZCommissioning_StateMachine(event_t events)
 *
 * @brief	Process EZ-Mode Commissioning States
 *
 */
void EZCommissioning_StateMachine(event_t events)
{
  gEZCommissioning_LastEvent = events;
  switch (EZCommissioning_GetState())
  {
  	/* Beginning of the Network Steering procedure as specified in the Home Automation 1.2 specification revision: 053520r29 */
    case EZCommissioning_InitialState_c:
    {
      if(events & gStart_c)
      {
        if(ZDO_GetState() == gZdoInitialState_c)
        {
#if gComboDeviceCapability_d
          gEZCommissioningNetworkDiscoveryAttempts = EZCommissioning_NetworkDiscoveryAttempts_c;
#endif
          EZCommissioning_SetState(EZCommissioning_NetworkSteering_c);
#if gCoordinatorCapability_d
          /* Device is not in a network. Form a network. */
          TS_SendEvent(gEzCmsTaskId, gFormNetwork_c);
#else
          TS_SendEvent(gEzCmsTaskId, gJoinNetwork_c);
#endif
        }
        else
        {
          /* Device is in a network. Broadcast permit join */
#if gMgmt_Permit_Joining_req_d
#if gComboDeviceCapability_d
    	  if(NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
	  {
    	     zbNwkAddr_t address;
    		
             Copy2Bytes(address, gaBroadcastZCnZR);
    	     /* Send Mgmt_Permit_Joining_Req */
             ASL_Mgmt_Permit_Joining_req(&gZclTransactionId, address, EZOpenNetworkTime_c*60, TRUE);
            
             if(gEZCommissioningOneShoot)
             {
                EZCommissioning_SetState(EZCommissioning_Identify_c);
                TS_SendEvent(gEzCmsTaskId, gStartIdentify_c);
                gEZCommissioningOneShoot = FALSE;
             }
   	  }
#endif
        }
      }
      if(events & gUpdatePollRate_c)
      {
#if gEndDevCapability_d || gComboDeviceCapability_d
    	  EZCommissioning_UpdatePollRate(FALSE);
#endif      	  
      }
      break;
    }
    case EZCommissioning_NetworkSteering_c:
    {
      if(events & gFormNetwork_c)
      {
        ZDO_Start(gZdoStartMode_Zc_c | gZdoStartMode_RamSet_c | gZdoStartMode_Associate_c);
      }
      if(events & gJoinNetwork_c)
      {
        ZDO_Start(gEZCommissioningPrimaryDeviceType | gZdoStartMode_RamSet_c);
      }
#if gComboDeviceCapability_d
      if(events & gJoiningfailed_c)
      {
        if(NlmeGetRequest(gDevType_c) != gCoordinator_c)
        {
          gEZCommissioningNetworkDiscoveryAttempts--;
          
          if(!gEZCommissioningNetworkDiscoveryAttempts)
          {
            ZDO_StopEx(gZdoStopMode_Stop_c);
            TS_SendEvent(gEzCmsTaskId, gFormNetwork_c);
          }
        }
      }
#endif
      if(events & gDeviceInNetwork_c)
      {
        if(!(gZclIdentifyCommissioningState&gZclCommissioningState_NetworkState_d))
        {
        	gZclIdentifyCommissioningState |= gZclCommissioningState_NetworkState_d;
        	ZCL_SaveNvmZclData();
        }
#if (gMgmt_Permit_Joining_req_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))
#if gComboDeviceCapability_d
    	if(NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
    	{
      	  /* Send Mgmt_Permit_Joining_Req. Routers send to 0xFFFC, Coordinator sends to itself. */
    	  zbNwkAddr_t aDestAddr = {0x00, 0x00};
#if (gRouterCapability_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
    	  if(NlmeGetRequest(gDevType_c) == gRouter_c)
#endif
    	  {
    		Copy2Bytes(aDestAddr, gaBroadcastZCnZR);
    	  }
#endif
          ASL_Mgmt_Permit_Joining_req(&gZclTransactionId, aDestAddr, EZOpenNetworkTime_c*60, TRUE);
    	}
#endif
    	if(gEZCommissioningOneShoot)
    	{
    	  gEZCommissioningOneShoot = FALSE;
    	  /* Move to Finding and Binding */
    	  EZCommissioning_SetState(EZCommissioning_Identify_c);
    	  TS_SendEvent(gEzCmsTaskId, gStartIdentify_c);
    	}
    	else
    	{
    	  EZCommissioning_SetState(EZCommissioning_InitialState_c);
    	}
      }
      break;
    }
    /* End of the Network Steering procedure as specified in the Home Automation 1.2 specification revision: 053520r29 */
    /* Beginning of the Finding and Binding procedure as specified in the Home Automation 1.2 specification revision: 053520r29 */
    case EZCommissioning_Identify_c:
    {
      if(events & gStartIdentify_c)
      {
        BeeAppUpdateDevice(0, gZclUI_EZCommissioning_FindingAndBinding_c, 0, 0, NULL);	  
        ZCL_SetIdentifyMode(appEndPoint, Native2OTA16(EZIdWindowTime_c*60)); 
        TS_SendEvent(gEzCmsTaskId, gUpdatePollRate_c); /*start FastPoll Rate -  EndDevice*/
      }
      if(events & gSendIdentifyReq_c)
      {
#if gASL_EnableEZCommissioning_Initiator_d
        EZComissioning_SendIdentifyQueryReq(gZclIdentifyTime, TRUE);
#endif
      }
      if(events & gReceivedIdentifyRsp_c)
      {
#if gASL_EzCommissioning_EnableGroupBindCapability_d
        if(gEZCommissioningUseGroup)
        {
          EZCommissioning_SetState(EZCommissioning_GroupBinding_c);
          #if gEndDevCapability_d || gComboDeviceCapability_d
              TS_SendEvent(gEzCmsTaskId, gUpdatePollRate_c); 
          #else      
              TS_SendEvent(gEzCmsTaskId, gPerformGroupBind_c);
          #endif  
        }
        else
#endif
        {
          EZCommissioning_SetState(EZCommissioning_Binding_c);
          TS_SendEvent(gEzCmsTaskId, gSendSimpleDescReq_c);
        }
      }
      if(events & gIdentifyEnd_c)
      {
    	EZCommissioning_SetState(EZCommissioning_InitialState_c);		
    	TS_SendEvent(gEzCmsTaskId, gUpdatePollRate_c); /*stop FastPoll Rate -  EndDevice*/		
      }
      if(events & gUpdatePollRate_c)
      {      
#if gEndDevCapability_d || gComboDeviceCapability_d
    	  EZCommissioning_UpdatePollRate(TRUE);
#endif
      }
      break;
    }
#if gASL_EzCommissioning_EnableGroupBindCapability_d
    case EZCommissioning_GroupBinding_c:
    {
      if(events & gPerformGroupBind_c)
      {
        EZCommissioning_PerformGroupBind();
      }
#if gEndDevCapability_d || gComboDeviceCapability_d      
      if(events & gUpdatePollRate_c)
      {
    	EZCommissioning_UpdatePollRate(FALSE);  
    	TS_SendEvent(gEzCmsTaskId, gPerformGroupBind_c);
      }
#endif       
      if(events & gUpdateDevice_c)
      {   	
        BeeAppUpdateDevice(0, gZclUI_EZCommissioning_Succesfull_c, 0, 0, NULL);  
        gEZCommissioning_LastEvent = 0x00;
      }      
      break;
    }
#endif
    case EZCommissioning_Binding_c:
    {
#if gASL_EnableEZCommissioning_Initiator_d   	
      if(events & gSendSimpleDescReq_c)
      {
#if gSimple_Desc_req_d
        EZComissioning_SendSimpleDescReq();
#endif
      }
      if(events & gReceivedSimpleDescRsp_c)
      {
        if(EZComissioning_DevBindMatchClusters())
        {

          gaEZComDBTable.waitForIeeeAddrRsp = FALSE;
          if(APS_GetIeeeAddress(gaEZComDBTable.aBindingTarget, gaEZComDBTable.aSrcIeeeAddress))
            TS_SendEvent(gEzCmsTaskId, gPerformBind_c);
          else
          {
            gaEZComDBTable.waitForIeeeAddrRsp = TRUE;
            (void)ASL_IEEE_addr_req(NULL, gaEZComDBTable.aBindingTarget, gaEZComDBTable.aBindingTarget, 0x00, 0x00);
          }
        }
        else
        {
          /* Ez Commissioning failed */
          EZCommissioning_SetState(EZCommissioning_InitialState_c);
          TS_SendEvent(gEzCmsTaskId, gUpdatePollRate_c); /*stop FastPoll Rate -  EndDevice*/
        }
      }

      if(events & gReceivedIEEEAddress_c)
      {
        TS_SendEvent(gEzCmsTaskId, gPerformBind_c);
      }
      
      if(events & gPerformBind_c)
      {
        EZCommissioning_SendBind();
      }
#if gEndDevCapability_d || gComboDeviceCapability_d      
      if(events & gUpdatePollRate_c)
      {
    	EZCommissioning_UpdatePollRate(FALSE);  
    	TS_SendEvent(gEzCmsTaskId, gUpdateDevice_c);
      }
#endif      
      if(events & gUpdateDevice_c)
      {     	
        BeeAppUpdateDevice(0, gZclUI_EZCommissioning_Succesfull_c, 0, 0, NULL);  
        gEZCommissioning_LastEvent = 0x00;
      }      
#endif      
      break;
    }
    case EZCommissioning_IdleState_c:
    default:    	
      break;
  }
}
#if gASL_EnableEZCommissioning_Initiator_d
/*!
 * @fn 		static void EZComissioning_SendIdentifyQueryReq(uint16_t time, bool_t senderInIdentify)
 *
 * @brief	Send over the air an IdentifyQueryReq (Broadcast). 
 *
 */
static void EZComissioning_SendIdentifyQueryReq(uint16_t time, bool_t senderInIdentify)
{
   zclIdentifyQueryReq_t *pReq;
   afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0xFF, 0xFF}, 0x08, {gaZclClusterIdentify_c}, 0x00, gApsTxOptionNone_c, 1};
   uint8_t len = 0;
   
   pReq = AF_MsgAlloc();
           
   if(!pReq)
      return;
           
   /* get address information */
   addrInfo.radiusCounter = afDefaultRadius_c;
#if gNum_EndPoints_c != 0    
   addrInfo.srcEndPoint = endPointList[0].pEndpointDesc->pSimpleDesc->endPoint;
   addrInfo.dstEndPoint = 0xff;
#endif  
   FLib_MemCpy(&pReq->addrInfo, &addrInfo, sizeof(addrInfo));
   /*send Identify Query Request - Broadcast */
   (void)zclIdentify_IdentifyQueryReq(pReq, len);
          
    MSG_Free(pReq);
}

/*!
 * @fn 		static void EZComissioning_SendIdentifyQueryReq(uint16_t time, bool_t senderInIdentify)
 *
 * @brief	Send over the air an Simple Descriptor Request to a specific EZ-mode Target device. 
 *
 */
#if gSimple_Desc_req_d
static void EZComissioning_SendSimpleDescReq(void)
{  
  ASL_Simple_Desc_req(NULL, ((zbApsdeDataIndication_t *)gEZCommissioningTempData)->aSrcAddr, ((zbApsdeDataIndication_t *)gEZCommissioningTempData)->srcEndPoint);
}
#endif
#endif
/*!
 * @fn 		static bool_t EZComissioning_DevBindMatchClusters(void)
 *
 * @brief	Checks if the Identify Query Rsp entry matches clusters with the original entry. Return TRUE if is does, FALSE if not
 *
 */
#if gASL_EnableEZCommissioning_Initiator_d
static bool_t EZComissioning_DevBindMatchClusters(void)
{
  bool_t  fFoundMatch = FALSE;
  uint8_t matchIndex = 0;
  zbClusterId_t *tempApp, *tempRcv;
  uint8_t noInClusters = 0, noOutClusters = 0, offset = 0;
  zbSimpleDescriptorResponse_t* pSimpleDescRsp = (zbSimpleDescriptorResponse_t*)gEZCommissioningTempData;

  if(gaEZComDBTable.aState == EZCommissioning_BindTableNotAvailable_c)
    return fFoundMatch;
  
#if gNum_EndPoints_c != 0   
  /*
    check if the output device clusters  match to the input clusters of the second
    device. 
  */
  tempApp = (void *)gEZModeClusterList.pAppOutClusterList; 
  noInClusters = pSimpleDescRsp->simpleDescriptor.inClusterList.cNumClusters;
  tempRcv = (void *)((uint8_t*)(&pSimpleDescRsp->simpleDescriptor.inClusterList) + 1);
  offset = noInClusters;
  giMatchIndex = 0;
  if (Zdp_MatchClusters(gEZModeClusterList.appNumOutClusters, 
                        noInClusters,
                        tempApp,
                        tempRcv,
                        TRUE))
  {
    matchIndex = (giMatchIndex > EZCommissioning_MaxNoOfBoundClusterPerDevice_d)? EZCommissioning_MaxNoOfBoundClusterPerDevice_d: giMatchIndex;
    FLib_MemCpy(&gaEZComDBTable.aMatchCluster, &gaZdpMatchedOutCluster,(sizeof(zbClusterId_t) * matchIndex));
    gaEZComDBTable.iMatchIndex = matchIndex;
    fFoundMatch = TRUE;
  }  
  /*
    check if the input device clusters  match to the output clusters of the second
    device. 
  */
  tempApp = (void *)gEZModeClusterList.pAppInClusterList;
  noOutClusters = pSimpleDescRsp->simpleDescriptor.inClusterList.aClusterList[offset][0];
  tempRcv = (void *)((uint8_t*)(&pSimpleDescRsp->simpleDescriptor.inClusterList.aClusterList[offset][1]));
  giMatchIndex = 0;
  if (Zdp_MatchClusters(gEZModeClusterList.appNumInClusters, 
                        noOutClusters,
                        tempApp,
                        tempRcv,
                        TRUE))
  {
	if(giMatchIndex + gaEZComDBTable.iMatchIndex > EZCommissioning_MaxNoOfBoundClusterPerDevice_d)  
		matchIndex = EZCommissioning_MaxNoOfBoundClusterPerDevice_d - gaEZComDBTable.iMatchIndex;
	else
		matchIndex = giMatchIndex;
    FLib_MemCpy(&gaEZComDBTable.aMatchCluster[gaEZComDBTable.iMatchIndex], &gaZdpMatchedOutCluster,(sizeof(zbClusterId_t) * matchIndex));
    gaEZComDBTable.iMatchIndex += matchIndex;
    fFoundMatch = TRUE;
  } 
  
#endif /* gNum_EndPoints_c != 0  */
  
  if(fFoundMatch == TRUE)
  {
    Copy2Bytes(gaEZComDBTable.aBindingTarget, pSimpleDescRsp->aNwkAddrOfInterest);
    //APS_GetIeeeAddress(pSimpleDescRsp->aNwkAddrOfInterest, gaEZComDBTable.aSrcIeeeAddress);
    gaEZComDBTable.srcEndPoint = pSimpleDescRsp->simpleDescriptor.endPoint;
    gaEZComDBTable.aState = EZCommissioning_BindTableNotAvailable_c;
  }
  return fFoundMatch;  
}

/*!
 * @fn 		static void EZCommissioning_SendBind(void)
 *
 * @brief	Create internal one-to-one bindings for matched clusters (only on the Initiator device)
 *
 */
static void EZCommissioning_SendBind(void)
{
  appToZdpMessage_t *pMsg;
  zbBindUnbindRequest_t bindRequest;
  zbClusterId_t clusterId;

  /* allocate the message */
  pMsg = MSG_AllocType(appToZdpMessage_t);
  if(!pMsg)
  {
    /* Try again later */
    TS_SendEvent(gEzCmsTaskId, gPerformBind_c);
    return;
  }
  
  if(gaEZComDBTable.iMatchIndex > 0)
  {
    gaEZComDBTable.iMatchIndex--;
    if(gaEZComDBTable.iMatchIndex)
    {
      TS_SendEvent(gEzCmsTaskId, gPerformBind_c);
    }
    else
    {
      /* Done */
      gaEZComDBTable.aState = EZCommissioning_BindTableAvailable_c;
      EZCommissioning_AutoClose(EZCommissioning_AutoCloseTimeout_c);
#if gEndDevCapability_d || gComboDeviceCapability_d
      TS_SendEvent(gEzCmsTaskId, gUpdatePollRate_c); 
#else      
      TS_SendEvent(gEzCmsTaskId, gUpdateDevice_c); 
#endif      
    }
  }
  else
  {
    /* Done */
    EZCommissioning_AutoClose(EZCommissioning_AutoCloseTimeout_c);
#if gEndDevCapability_d || gComboDeviceCapability_d
      TS_SendEvent(gEzCmsTaskId, gUpdatePollRate_c); 
#else      
      TS_SendEvent(gEzCmsTaskId, gUpdateDevice_c); 
#endif 
  }
  
  Copy2Bytes(clusterId, gaEZComDBTable.aMatchCluster[gaEZComDBTable.iMatchIndex]);

  /* fill in the message */
  pMsg->msgType = gBind_req_c;
  Copy2Bytes(pMsg->aDestAddr, NlmeGetRequest(gNwkShortAddress_c));
  /* fill in the request */
  Copy8Bytes(bindRequest.aSrcAddress, NlmeGetRequest(gNwkIeeeAddress_c));
#if gNum_EndPoints_c != 0    
  bindRequest.srcEndPoint = endPointList[0].pEndpointDesc->pSimpleDesc->endPoint;
#else
  bindRequest.srcEndPoint = 0x08;
#endif
  bindRequest.addressMode = gZbAddrMode64Bit_c;
  Copy8Bytes(bindRequest.destData.extendedMode.aDstAddress, gaEZComDBTable.aSrcIeeeAddress);
  bindRequest.destData.extendedMode.dstEndPoint = gaEZComDBTable.srcEndPoint; 
  Copy2Bytes(bindRequest.aClusterId, clusterId);

  /* copy the request */
  FLib_MemCpy(&pMsg->msgData.bindReq, &bindRequest, sizeof(bindRequest));

  /* call ZDP to send the message out */
  (void)APP_ZDP_SapHandler( pMsg );
}


#if gASL_EzCommissioning_EnableGroupBindCapability_d
/*!
 * @fn 		static void EZCommissioning_PerformGroupBind(void)
 *
 * @brief	Perform internal group binding(only on the Initiator device)
 *
 */
static void EZCommissioning_PerformGroupBind(void)
{
  appToZdpMessage_t *pMsg;
  zbBindUnbindRequest_t bindRequest;
  zbGroupId_t aDestGroupAddr = {EZCommissioning_GroupAddr_c};
  
  if(gEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1] == 0xFF)
  {
#if gNum_EndPoints_c != 0
    gEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1] = endPointList[0].pEndpointDesc->pSimpleDesc->appNumOutClusters;
#else
    gEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1] = 0;
#endif
  }
  if(gEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1] == 0)
  {
    /* Done */
    gEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1] = 0xFF;
    EZCommissioning_SetState(EZCommissioning_InitialState_c);
    TS_SendEvent(gEzCmsTaskId, gUpdatePollRate_c); /*stop FastPoll Rate -  EndDevice*/
    return;
  }
  else
  {
    gEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1]--;
  }

  /* allocate the message */
  pMsg = AF_MsgAlloc(); //MSG_AllocType(appToZdpMessage_t);
  if(!pMsg)
  {
    /* Try again later */
	gEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1]++;
    TS_SendEvent(gEzCmsTaskId, gPerformGroupBind_c);
    return;
  }

  /* fill in the message */
  pMsg->msgType = gBind_req_c;
  Copy2Bytes(pMsg->aDestAddr, NlmeGetRequest(gNwkShortAddress_c));
  /* fill in the request */
  Copy8Bytes(bindRequest.aSrcAddress, NlmeGetRequest(gNwkIeeeAddress_c));
#if gNum_EndPoints_c != 0    
  bindRequest.srcEndPoint = endPointList[0].pEndpointDesc->pSimpleDesc->endPoint;
#else
  bindRequest.srcEndPoint = 0x08;
#endif   
  bindRequest.addressMode = gZbAddrModeGroup_c;
  Copy2Bytes(bindRequest.destData.groupMode.aDstaddress, aDestGroupAddr); 
  Copy2Bytes(bindRequest.aClusterId, &endPointList[0].pEndpointDesc->pSimpleDesc->pAppOutClusterList[2*gEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1]]);

  /* copy the request */
  FLib_MemCpy(&pMsg->msgData.bindReq, &bindRequest, sizeof(bindRequest));

  /* call ZDP to send the message out */
  (void)APP_ZDP_SapHandler( pMsg );
  
  if(gEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1] == 0)
  {
    /* Send AddGrioupIfIdentify request */
    zclGroupAddGroupIfIdentifyingReq_t* pAddGroupIfIdentifyReq;
    zbApsdeDataIndication_t* pSourceInfo;
    pSourceInfo = (zbApsdeDataIndication_t*)gEZCommissioningTempData;
    
    pAddGroupIfIdentifyReq = MSG_Alloc(sizeof(zclGroupAddGroupIfIdentifyingReq_t) + 9);//9 represents the TestGroup string length
    /* Fill destination information */
    pAddGroupIfIdentifyReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
    Copy2Bytes(pAddGroupIfIdentifyReq->addrInfo.dstAddr.aNwkAddr, pSourceInfo->aSrcAddr);
    pAddGroupIfIdentifyReq->addrInfo.dstEndPoint = pSourceInfo->srcEndPoint;
    Set2Bytes(pAddGroupIfIdentifyReq->addrInfo.aClusterId, gZclClusterGroups_c);
    pAddGroupIfIdentifyReq->addrInfo.srcEndPoint = endPointList[0].pEndpointDesc->pSimpleDesc->endPoint;
    pAddGroupIfIdentifyReq->addrInfo.txOptions = 0;
    pAddGroupIfIdentifyReq->addrInfo.radiusCounter = afDefaultRadius_c;
    
    Copy2Bytes(pAddGroupIfIdentifyReq->cmdFrame.aGroupId, aDestGroupAddr); 
    FLib_MemCpy(pAddGroupIfIdentifyReq->cmdFrame.szGroupName, gGroupName, 10);
    
    (void)ASL_ZclGroupAddGroupIfIdentifyReq(pAddGroupIfIdentifyReq);
    /* Done */
    gEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1] = 0xFF;
    EZCommissioning_AutoClose(EZCommissioning_AutoCloseTimeout_c);    
    TS_SendEvent(gEzCmsTaskId, gUpdateDevice_c); 
  }
  else
  {
    TS_SendEvent(gEzCmsTaskId, gPerformGroupBind_c);
  }
}
#endif /* gASL_EzCommissioning_EnableGroupBindCapability_d */
#endif /* gASL_EnableEZCommissioning_Initiator_d */

/*!
 * @fn 		void EZCommissioning_AutoClose(uint32_t timeout)
 *
 * @brief	Called in case of succes to close an EZ-Mode Commissioning session
 *
 */
void EZCommissioning_AutoClose(uint32_t timeout)
{
  
  if(!timeout)
  {
    if(!(gZclIdentifyCommissioningState&gZclCommissioningState_OperationalState_d))
    {
    	gZclIdentifyCommissioningState |= gZclCommissioningState_OperationalState_d;
    	ZCL_SaveNvmZclData();
    }	  
	
#if gASL_EnableEZCommissioning_Initiator_d
    {
#if gASL_ZclIdentifyReq_d
      zclIdentifyReq_t identifyRequest;
#endif
#if gASL_ZclCmdUpdateCommissioningStateReq_d
      zclUpdateCommissioningStateReq_t updateCommissioningStateRequest;
#endif
      afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0x08, {gaZclClusterIdentify_c}, 0x00, gApsTxOptionNone_c, 1};
      if(gEZCommissioningUseGroup)
      {
        zbApsdeDataIndication_t* pSourceInfo;
        pSourceInfo = (zbApsdeDataIndication_t*)gEZCommissioningTempData;        
        Copy2Bytes(addrInfo.dstAddr.aNwkAddr, pSourceInfo->aSrcAddr);
        addrInfo.dstEndPoint = pSourceInfo->srcEndPoint;
      }
      else
      {
        Copy2Bytes(addrInfo.dstAddr.aNwkAddr, gaEZComDBTable.aBindingTarget);
        addrInfo.dstEndPoint = gaEZComDBTable.srcEndPoint;
      }
      addrInfo.radiusCounter = afDefaultRadius_c;
      addrInfo.srcEndPoint = ZCL_GetEndPointForSpecificCluster(addrInfo.aClusterId, TRUE);;
#if gASL_ZclIdentifyReq_d
	  /* Set the Identify Time on the Target to 0 */
      FLib_MemCpy(&identifyRequest.addrInfo, &addrInfo, sizeof(addrInfo));
      identifyRequest.cmdFrame.iTimeIdentify = 0;
      (void)ASL_ZclIdentifyReq(&identifyRequest);
#endif

#if gASL_ZclCmdUpdateCommissioningStateReq_d
      /* Notify the Target that it has been commissioned */
      FLib_MemCpy(&updateCommissioningStateRequest.addrInfo, &addrInfo, sizeof(addrInfo));
      updateCommissioningStateRequest.cmdFrame.action = 0x01;
      updateCommissioningStateRequest.cmdFrame.commissioningStateMask = (gZclCommissioningState_NetworkState_d | gZclCommissioningState_OperationalState_d);
      (void)ASL_ZclUpdateCommissioningStateReq(&updateCommissioningStateRequest);
#endif
    }
#endif    
    EZCommissioning_SetState(EZCommissioning_InitialState_c);
    ZCL_SetIdentifyMode(appEndPoint, 0);   
  }
  else
  {
    TMR_StartSingleShotTimer(gEZCommissioningAutocloseTimerId, timeout, EZCommissioning_AutoCloseTimerCallback);
  }
}

/*!
 * @fn 		void EZCommissioning_AutoCloseTimerCallback(tmrTimerID_t timerId)
 *
 * @brief	EZ-Mode Commissioning Auto Close Timer Callback
 *
 */
void EZCommissioning_AutoCloseTimerCallback(tmrTimerID_t timerId)
{
   (void)timerId;
   EZCommissioning_AutoClose(0);
}

/*!
 * @fn 		void EZCommissioning_VerifyIEEEaddrRsp(zbIeeeAddr_t  aIeeeAddr)
 *
 * @brief	Check and temporarily store the target IEEEaddress(used into a multi-hop solution)
 *
 */
#if gASL_EnableEZCommissioning_Initiator_d 
void EZCommissioning_VerifyIEEEaddrRsp(zbIeeeAddr_t  aIeeeAddr)
{
  if(gaEZComDBTable.waitForIeeeAddrRsp)
  {
   FLib_MemCpy(gaEZComDBTable.aSrcIeeeAddress, aIeeeAddr, 8);
   TS_SendEvent(gEzCmsTaskId, gReceivedIEEEAddress_c);
  }
}
#endif

/*!
 * @fn 		void EZCommissioning_Reset(void)
 *
 * @brief	Perform EZ-Mode Commissioning Reset
 *
 */
void EZCommissioning_Reset(void)
{
#if gComboDeviceCapability_d
  if(!gEZCommissioningNetworkDiscoveryAttempts && EZCommissioning_GetState() == EZCommissioning_NetworkSteering_c)
  {
    /* The device failed to join a network. Do not reset the device, it will start as a coordinator */
    return;
  }
#endif
  
  gZclIdentifyCommissioningState = 0;
  
  if(gEZCommissioning_ResetDevice)
  {
    /* Save the new commissioning state in NVM */
    NvSyncSave(&gZclIdentifyCommissioningState, TRUE,TRUE);

    
    /* Reset the device */
	#ifndef __IAR_SYSTEMS_ICC__
	  // S08 platform reset
	  PWRLib_Reset();               
	#else
	#ifdef PROCESSOR_KINETIS
	  PWRLib_Reset(); 
	#else  
	  CRM_SoftReset();
	#endif
	#endif  
	
  }
  
  EZCommissioning_SetState(EZCommissioning_InitialState_c);
  TMR_StopTimer(gEZCommissioningAutocloseTimerId);
  gZclIdentifyCommissioningState = 0;
  gEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1] = 0xFF;
  ASL_ChangeUserInterfaceModeTo(gConfigureMode_c);
  ASL_DisplayChangeToCurrentMode(gmUserInterfaceMode);
}

/*!
 * @fn 		static void EZCommissioning_UpdatePollRate(bool_t fastPollRate)
 *
 * @brief	Update poll rate for an EndDevice.
 *
 */
 #if gEndDevCapability_d || gComboDeviceCapability_d
static void EZCommissioning_UpdatePollRate(bool_t fastPollRate)
{
    static uint16_t orgPollRate = gNwkIndirectPollRate_c-1;
	uint16_t pollRate = 0x00;
	
	if (!IsLocalDeviceReceiverOnWhenIdle())
	{
		/*	update poll rate during the Finding And Binding process: go to FastPollMode */ 	
		if((fastPollRate)&&(NlmeGetRequest(gNwkIndirectPollRate_c)!=orgPollRate))
		{
		    /* ZED: remember original polling rate */
			orgPollRate = NlmeGetRequest(gNwkIndirectPollRate_c);
			/* set the new poll Rate */
#if gZclEnablePollControlCluster_d	
			pollRate = (uint16_t)(OTA2Native16(gPollControlAttrs.shortPollInterval)*1000/4);
		
#else
			pollRate = 250;
#endif
		}
		else
		{
			if(NlmeGetRequest(gNwkIndirectPollRate_c) < orgPollRate)
			{
				pollRate = orgPollRate;		
				orgPollRate--;
			}
		}
		if(pollRate)
		{
			(void)ZDO_NLME_ChangePollRate(pollRate);
#ifdef PROCESSOR_KINETIS
            /* Save the new pollRate in NVM */
            NvSaveOnIdle(&gSAS_Ram, TRUE);
#else		    
            ZCL_SaveNvmZclData();
#endif 
		}
	}   
}
#endif  /* gEndDevCapability_d || gComboDeviceCapability_d */
#endif /* gASL_EnableEZCommissioning_d */
