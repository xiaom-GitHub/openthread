/*****************************************************************************
* BeeApp.c
*
* This application (SeMeteringDevice) demonstrates a Smart Energy application.
*
* Copyright (c) 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* USER INTERFACE
* --------------
*
* Like most BeeStack sample applications, this application uses the the common
* ASL user interface. The ASL interface has two "modes" to allow the keys, LEDs
* and (optional on NCB) the LCD display to be used for multiple purposes.
* Configuration mode includes commands such as starting and leaving the network,
* whereas application mode includes application specific functions and display.
*
* Each key (aka switch) can be pressed for a short duration (short press) or
* long duration of about 2+ seconds (long press). Examples include SW1 or LSW3.
*
* Application specific code can be found here. Code common among applications
* can be found in ASL_UserInterface.c
*
* Config Mode:
* SW1  - Form/join network (form if ZC, join if ZR or ZED) with previous configuration
* SW2  - Toggle Permit Join (ZC/ZR only)
* SW3  - 
* SW4  - Choose channel (default is 25). Only functional when NOT on network.
* LSW1 - Toggle display/keyboard mode (Config and Application)
* LSW2 - Leave network
* LSW3 - 
* LSW4 - Form/join network (form if ZC, join if ZR or ZED) with new configuration
*
* Application Mode:
* SW1  -
* SW2  -
* SW3  - Toggle Identify mode on/off (will stay on for 20 seconds)
* SW4  - 
* LSW1 - Toggle display/keyboard mode (Config and Application)
* LSW2 -
* LSW3 - 
* LSW4 - 
*
*****************************************************************************/
#include "BeeStack_Globals.h"
#include "BeeStackConfiguration.h"
#include "BeeStackParameters.h"
#include "AppZdoInterface.h"
#include "TS_Interface.h"
#include "TMR_Interface.h"
#include "AppAfInterface.h"
#include "FunctionLib.h"
#include "PublicConst.h"
#include "keyboard.h"
#include "Display.h"
#include "EndPointConfig.h"
#include "BeeApp.h"
#include "ZDOStateMachineHandler.h"
#include "ZdoApsInterface.h"
#include "BeeAppInit.h"
#include "NVM_Interface.h"
#include "ZtcInterface.h"
#include "HaProfile.h"
#include "ZclOptions.h"
#include "ZdpManager.h"
#include "ASL_ZdpInterface.h"
#include "ASL_UserInterface.h"
#include "ASL_ZCLInterface.h"
#include "Led.h"
#include "zclse.h"
#include "ZclSEDRLC.h"
#include "ZclSEPrice.h"
#include "ZclSEMDUPairing.h"
#include "ZclSEMessaging.h"
#include "SEprofile.h"
#include "Display.h"
#include "ZclSETunneling.h"
#include "ZclSEMetering.h"
#include "ZclSEPrepayment.h"
#include "ZDONwkManager.h"
#include "ZclFoundation.h"
#include "ApsMgmtInterface.h"

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/
/* Size of a time attribute record */
#define mSizeOfTimeAttrRecord_c (sizeof(zclAttrId_t) + sizeof(zclStatus_t) + sizeof (zclAttrType_t)+ sizeof(uint32_t))

/* The wait time for a Service Discovery procedure*/
#define mServiceDiscoveryWaitTime 2
/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/

void BeeAppTask(event_t events);
void BeeAppDataIndication(void);
void BeeAppDataConfirm(void);
#if gInterPanCommunicationEnabled_c
void BeeAppInterPanDataConfirm(void);
void BeeAppInterPanDataIndication(void);
#endif 

/* Common SE application behaviour*/
void BeeAppKeyEstabSuccess(void);
void BeeAppAddDeviceToESIList(ASL_SendingNwkData_t sendingNwkData);
void BeeAppSEDeviceRunningCallBack(tmrTimerID_t tmrid);
void BeeAppEstablishLinkKeyWithESI(zbIeeeAddr_t aIeeeAddr, zbNwkAddr_t aNwkAddr);
void BeeAppServiceDiscoveryTimeoutCallback(tmrTimerID_t tmrid);
void BeeAppServiceDiscWaitCallback(tmrTimerID_t tmrid);

/* Specific SE application behaviour*/
static void BeeAppDisplayMessage(void);
void BeeAppRequestPriceInfoFromESI(uint8_t currentTier, uint8_t currentThreshold);
void BeeAppSendMsgConfReq(void);
void BeeAppGetLastMsgReq(void);
void BeeAppHandlePrice(void *pData);
void BeeAppHandleBlockPeriod(void *pData);
void BeeAppGetCurrentPrice(void);
void BeeAppClearCurrentMsg(void);
void BeeAppMessageTimerCallback(tmrTimerID_t timer);
void BeeAppRequestPhyEnvFromESI(void);
void BeeAppsUpdateAsynchronousAttributes(zclAttrId_t attrId, zbClusterId_t clusterId);
void BeeAppsAsynchronousDeviceCallBack(tmrTimerID_t timerID);

/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/



tmrTimerID_t gBeeAppsAsynchronousDeviceToSettleTimerID;
tmrTimerID_t AppTimer;
tmrTimerID_t AppServiceDiscoveryTimerID;
const zbNwkAddr_t gESIAddr = {0x00,0x00};

tmrTimerID_t mMessageTimer;
uint8_t mCPPEvtIndex = 0xFF;


uint8_t currentMsgReceived[gMaxRcvdMsgToDisplay] = "NoMsg";

#if gLCDSupported_d || gASL_PrintToTestClient_d
/* Keep track of next field to be printed */
uint8_t nextField = 0;
#endif
/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/
zbEndPoint_t gTCEndPoint = gZdoInvalidEndPoint_c;
extern uint8_t gZclSENoOfFailedKeepAlive;
seESIDescriptor_t gZclSeESIList[gMaxNoOfESISupported_c];
bool_t gZclSE_KeyEstablished = FALSE;
extern bool_t gSuitableParentFound;
zbEndPoint_t appEndPoint;

gZclAlarmTable_t gAlarmsTable[MaxAlarmsPermitted]; //used for alarm cluster
extern bool_t gAcceptTunnel;
extern bool_t gAcceptFastPoll;
extern void ZCL_ReportingTimeout(tmrTimerID_t iTimerId);
publishPriceEntry_t *pCurrentPrice = NULL; //No Price available
#ifdef gASL_ZclPrice_Optionals_d
publishBlockPeriodEntry_t curentBlockPeriod= {0xff}; //No block period available
#endif
#if gASL_ZclPrice_Optionals_d || gASL_ZclSE_12_Features_d 
publishConversionFactorEntry_t curentConversionFactor={0xFF}; //No conversion factor available
publishCalorificValueEntry_t curentCaloricValue={0xFF};//No caloric value available
#endif

zclMsg_MsgTableEntry_t  *pLastMsgReceived = NULL;

/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
* BeeAppInit
*
* Initialize the application.
****************************************************************************/
void BeeAppInit
  (
  void
  )
{
  uint8_t i;

  /* register the application endpoint(s), so we receive callbacks */
  for(i=0; i<gNum_EndPoints_c; ++i) {
    (void)AF_RegisterEndPoint(endPointList[i].pEndpointDesc);
  }

  /* where to send switch commands from */
  appEndPoint = endPointList[0].pEndpointDesc->pSimpleDesc->endPoint;

  /* start with all LEDs off */
  ASL_InitUserInterface("SeMeter");

  /*Init SE clusters*/
  SE_Init();
  
#if gASL_ZclPrepayment_d  
  ZCL_PrepaymentServerInit();
#endif  
  ZCL_PriceClientInit();
  
#if gInterPanCommunicationEnabled_c
  ZCL_RegisterInterPanClient((pfnInterPanIndication_t)ZCL_InterPanClusterClient);
#endif
 
  /* Initialize SE Tunneling data */
#if gASL_ZclSETunneling_d  
  ZCL_SETunnel_InitData();
#endif  

  ZCL_SmplMet_MeterInit();
  
  ZCL_MsgInit();
  ZCL_LdCtrlClientInit();
  
  /*indicate that meter is running*/
  ASL_AppSetLed(LED4, gLedFlashing_c);
  /* Allocate asynchronous timer */
  gBeeAppsAsynchronousDeviceToSettleTimerID = TMR_AllocateTimer();
  AppTimer = TMR_AllocateTimer();
  AppServiceDiscoveryTimerID = TMR_AllocateTimer();
  
  /* Message startup and duration timer */
  mMessageTimer = TMR_AllocateTimer();

#if !gCoordinatorCapability_d && gZclSeEnableAutoJoining_d
  SE_SetState(gZclSE_AutoJoiningState_c);
  TS_SendEvent(gSETaskID, gZclEvtSEPeriodicScan_c);
#endif  
}

/*****************************************************************************
* BeeAppAppTask
*
* The application task.
*****************************************************************************/
void BeeAppTask
  (
  event_t events    /*IN: events for the application task */
  )
{
  /* received one or more data confirms */
  if(events & gAppEvtDataConfirm_c)
    BeeAppDataConfirm();

  /* received one or more data indications */
  if(events & gAppEvtDataIndication_c)
    BeeAppDataIndication();
  
#if gInterPanCommunicationEnabled_c
    /* received one or more data confirms */
  if(events & gInterPanAppEvtDataConfirm_c)
    BeeAppInterPanDataConfirm();

  /* received one or more data indications */
  if(events & gInterPanAppEvtDataIndication_c)
    BeeAppInterPanDataIndication();
#endif 

  /* ZCL specific */
/*  
  if(events & gAppEvtAddGroup_c)
    ASL_ZclAddGroupHandler();

  if(events & gAppEvtStoreScene_c)
    ASL_ZclStoreSceneHandler();
*/
    if(events & gAppEvtSyncReq_c)
      ASL_Nlme_Sync_req(FALSE);
}

/*****************************************************************************
* BeeAppHandleKeys
*
* The default keyboard handling uses a model system: a network configuration-mode
* and an application run-mode. Combined with the concepts of short and
* long-press, this gives the application a total of 16 keys on a 4 button system
* (4 buttons * 2 modes * short and long).
*
* Config-mode covers joining and leaving a network, binding and other
* non-application specific keys, and are common across all Freescale applications.
*
* Run-mode covers application specific keys.
*****************************************************************************/
void BeeAppHandleKeys
  (
  key_event_t events  /*IN: Events from keyboard modul */
  )
{
(void) events;

  /* Application-mode keys */
  if( gmUserInterfaceMode == gApplicationMode_c ) {
    switch (events){
      case gKBD_EventSW2_c:
        {
          /* Toggle the status attribute bit: Check Meter */
          zbClusterId_t clusterId={gaZclClusterSmplMet_c}; 
          uint8_t meterStatusAttr;
          (void)ZCL_GetAttribute(appEndPoint, clusterId, gZclAttrMetMSStatus_c, gZclServerAttr_c, &meterStatusAttr, NULL);
          if((meterStatusAttr & gZclMSStatusCheckMeter_c) == gZclMSStatusCheckMeter_c)
          {
            meterStatusAttr &=  ~gZclMSStatusCheckMeter_c;
            ASL_AppSetLed(LED2, gLedOff_c);
            //Led2Off();
          }
          else
          {
            meterStatusAttr |=  gZclMSStatusCheckMeter_c;
            ASL_AppSetLed(LED2, gLedOn_c);  
            //Led2On();
          }
          (void) ZCL_SetAttribute(appEndPoint,clusterId, gZclAttrMetMSStatus_c, gZclServerAttr_c, &meterStatusAttr); 
        }
        break;
      case gKBD_EventSW4_c:
        {
          /* toggle on and off energy consumption */ 
          if(TMR_IsTimerActive(gGetProfileTestTimerID))
          {
            ASL_AppSetLed(LED4, gLedStopFlashing_c);  
            TMR_StopTimer(gGetProfileTestTimerID);
          }
          else
          {
              ASL_AppSetLed(LED4, gLedFlashing_c);  
              //Led4Flashing();          
              #if(gProfIntrvPeriod_c < 5)
              TMR_StartMinuteTimer(gGetProfileTestTimerID, gTimerValue_c, GetProfileTestTimerCallBack);
            #else
              TMR_StartSecondTimer(gGetProfileTestTimerID, gTimerValue_c, GetProfileTestTimerCallBack);
            #endif
          }
        }
        break;
      default:
        /* no application specific keys, let ASL handle all of them */
        ASL_HandleKeys(events);
        break;
    }
  }
  else {
    /* If you are not on Appication Mode, then call ASL_HandleKeys, to be eable to use the Configure Mode */
    ASL_HandleKeys(events);
  }
}

/*****************************************************************************
* BeeAppUpdateDevice
*
* Contains application specific
*****************************************************************************/

void BeeAppUpdateDevice
  (
  zbEndPoint_t endPoint,    /* IN: endpoint update happend on */
  zclUIEvent_t event,       /* IN: state to update */
  zclAttrId_t attrId,       /* IN: attribute id */
  zbClusterId_t clusterId,  /* IN: cluster id */
  void *pData /* IN: OTA indication */ 
  )
{
  
  
  /* Avoid warning if attrId parameter is not referenced. */
  #if !gZclEnableReporting_c
  (void) attrId;
  #endif
  
  
    
  switch(event)
  {
  case gZDOToAppMgmtZCRunning_c:
  case gZDOToAppMgmtZRRunning_c:
  case gZDOToAppMgmtZEDRunning_c:
    gSuitableParentFound = FALSE;
    SE_SetState(gZclSE_InitState_c);
    TMR_StartTimer(AppTimer, gTmrLowPowerSecondTimer_c, TmrSeconds(1), 
                BeeAppSEDeviceRunningCallBack);
	break;  
   case gZclUI_KeyEstabSuccesful_c:
      BeeAppKeyEstabSuccess();
      ZCL_SaveNvmZclData();
      break;        	
  case gBindingSuccess_c:
  case gBindingFailure_c:
    TS_SendEvent(gSETaskID, gZclEvtServiceBindFinished_c);
    break;
  case gMatchFailure_c:
     if (SE_GetState() == gZclSE_ServiceDiscoveryState_c)
       TMR_StartTimer(AppTimer, gTmrLowPowerSecondTimer_c, TmrSeconds(mServiceDiscoveryWaitTime), 
                BeeAppServiceDiscWaitCallback);
     break;
  case gMatchDescriptorSuccess_c:
    BeeAppAddDeviceToESIList(gSendingNwkData);
    
    /* We just discovered the ep where the MDU Pairing server resides*/
    if (SE_GetState() == gZclSE_ServiceDiscoveryForMDUPairingState_c)
      gFTCEndPoint = gSendingNwkData.endPoint;
      
    if (SE_GetState() == gZclSE_ServiceDiscoveryState_c)
      TMR_StartTimer(AppTimer, gTmrLowPowerSecondTimer_c, TmrSeconds(mServiceDiscoveryWaitTime), 
                BeeAppServiceDiscWaitCallback);
    else
      TS_SendEvent(gSETaskID, gZclEvtServiceDiscoveryFinished_c);
      if(InterpretMatchDescriptor(gSendingNwkData.NwkAddrOfIntrest, gSendingNwkData.endPoint)==FALSE)
          {

          }
      break;
    
    case gIeeeAddrSuccess_c:
      {
        zbExtendedDevResp_t *pRsp = (zbExtendedDevResp_t *) pData;
        zbApsDeviceKeyPairSet_t  *pSecurityMaterial = NULL;
#ifndef gHostApp_d
      {
        zbApsDeviceKeyPairSet_t whereToCpyEntry;
        pSecurityMaterial = APS_GetSecurityMaterilaEntry(pRsp->aIeeeAddrRemoteDev, &whereToCpyEntry);
      }
#endif
      if (!pSecurityMaterial)
        BeeAppEstablishLinkKeyWithESI(pRsp->aIeeeAddrRemoteDev, pRsp->aNwkAddrRemoteDev);
    }
    break; 
    
    case gNwkAddrSuccess_c:
    {
      if (SE_GetState() == gZclSE_MDUPairingState_c)
        TS_SendEvent(gSETaskID, gZclEvtQueryNextPairing_c);
    }
    break;      

    case gNodeDescSuccess_c:
    {
      AddUpdateDeviceDescriptor((zbNodeDescriptorResponse_t *) pData);
    }
    break;     
    
    case gRejoinFailed_c:
    {
      if (gSuitableParentFound)
        TS_SendEvent(gSETaskID, gZclEvtRejoinFailure_c);
    }
    break;
	
    case gZclUI_SEServiceDiscoveryCompleted_c:
      TMR_StartTimer(AppServiceDiscoveryTimerID, gTmrLowPowerMinuteTimer_c, 
                  TmrMinutes(gSE_ServiceDiscoveryInterval_c), 
                  BeeAppServiceDiscoveryTimeoutCallback);
    break;
	
  case gZclUI_SEReadyState:
#if gZclMirroring_d && gZclMirroringAutoCfg_d      
    BeeAppRequestPhyEnvFromESI();
#endif  
    break;

    case gZclUI_MsgDisplayMessageReceived_c:
      {	
        ZCLTime_t currentTime = ZCL_GetUTCTime();
        zclMsg_MsgTableEntry_t *pEntry = (zclMsg_MsgTableEntry_t *)pData;
        Duration_t msgDurationInMinutes = pEntry->DurationInMinutes;
        
        pLastMsgReceived = pEntry;
        
        if (!FLib_MemCmp(&currentTime, &pEntry->StartTime, sizeof(ZCLTime_t)))
        {
          uint16_t TimeToDisplay;
          ZCLTime_t EndTime;
          
          EndTime = OTA2Native32(pEntry->StartTime) + (uint32_t)msgDurationInMinutes * 60;
          
          if (OTA2Native32(pEntry->StartTime) > currentTime)
          {
            /* The message will be displayed on timeout */
            TimeToDisplay = (uint16_t) (OTA2Native32(pEntry->StartTime) - currentTime);
            pEntry->EntryStatus = gMessagePending_c;
            TMR_StartSecondTimer(mMessageTimer, TimeToDisplay, BeeAppMessageTimerCallback);
          }
          else if (currentTime > OTA2Native32(pEntry->StartTime) && (currentTime < EndTime || msgDurationInMinutes == 0xFFFF))
          {
            /* Display immediately */            
            BeeAppDisplayMessage();
            pEntry->EntryStatus = gMessageDisplayed_c;
            
            if (msgDurationInMinutes < 0xffff)
            {
              msgDurationInMinutes = (uint16_t) (EndTime - currentTime);
              /* The message will be deleted on timeout */
              TMR_StartSecondTimer(mMessageTimer, msgDurationInMinutes, BeeAppMessageTimerCallback);
            }
          }
          /* else */
          /* The message is outdated, discard it */
        }
        else
        {
          /* If start time is 0x00000000, display the message immediately */
          BeeAppDisplayMessage();          
          pEntry->EntryStatus = gMessageDisplayed_c;
          
          if (msgDurationInMinutes < 0xffff)
          {
            /* The message will be deleted on timeout */            
            TMR_StartMinuteTimer(mMessageTimer, msgDurationInMinutes, BeeAppMessageTimerCallback);
          }
        }  
      }
      break;
    case gZclUI_MsgCancelMessageReceived_c:
      {
        {  
          zclCmdMsg_CancelMsgReq_t *pReq = (zclCmdMsg_CancelMsgReq_t *) pData; 
          
          if(FLib_MemCmp(pLastMsgReceived->MsgID, pReq->MsgID, sizeof(MsgId_t)))
          {                                              
            if ((pLastMsgReceived->MsgCtrl & gZclSEMsgCtrl_MsgConfRequired_c) == gZclSEMsgCtrl_MsgConfRequired_c)
            {
              /* Confirmation required */
              /* Delete the message on SW1 press */
#if gLCDSupported_d          
              ASL_PrintEvent("Msg cancel cnf");          
#endif            
              ASL_SetLed(LED2, gLedFlashing_c);
              pLastMsgReceived->EntryStatus = gMessageCanceled_c;
            }
            else
            {
              /* No confirmation required */
              /* Delete the message immediately */
              if (pLastMsgReceived->EntryStatus != gNoMessage_c)
              {            
                TMR_StopTimer(mMessageTimer);
                pLastMsgReceived->EntryStatus = gNoMessage_c;              
                BeeAppClearCurrentMsg();              
                ASL_SetLed(LED2, gLedOff_c);
              }
            }
          }
        }
      }
      break;
    case gZclUI_CPPEvt_c:
      {
        mCPPEvtIndex = *((uint8_t *)pData);
        ASL_SetLed(LED4, gLedFlashing_c);
      }
      break;
    case gZclUI_PriceEvt_c:
      BeeAppHandlePrice(pData);
      break;
#ifdef gASL_ZclPrice_Optionals_d
    case  gZclUI_BlockPeriodEvt_c:
      BeeAppHandleBlockPeriod(pData);
      break;
#endif
    case gZclUI_MsgUpdateTimeInDisplay_c:
      {
#if gLCDSupported_d
#if gTargetMC1322xNCB
        ASL_PrintUTCTime();
#elif gTargetQE128EVB_d
        if (gASL_FieldTime_c == nextField) 
        {
          ASL_PrintUTCTime();
        }  
#endif  
#endif
      }
      break;  
 
    case gZclUI_SendReadAttributeRequest_c:
    {
        zclFrame_t *pFrame;      
        afDeviceDef_t *pDevice;        
        afAddrInfo_t addrInfo;
        uint8_t attributesCount;         
        uint8_t payloadLen, maxAsduLen;
        afToApsdeMessage_t *pMsgRsp;
        afClusterDef_t *pCluster;    
		zbApsdeDataIndication_t *pIndication = (zbApsdeDataIndication_t*) pData;
		
		if (!pIndication)
		  break;                   
          
        if(TwoBytesToUint16(clusterId) != gZclClusterSmplMet_c)
          break;
                
        pDevice = AF_GetEndPointDevice(pIndication->dstEndPoint);        
        if (NULL == pDevice)
          break;
         
        pCluster = ZCL_FindCluster(pDevice, pIndication->aClusterId);
        if (NULL == pCluster)
          break;
        
        pMsgRsp = AF_MsgAlloc();              
        if (NULL == pMsgRsp)
          break;
        
        /* prepare the response with the correct transaction id */
        pFrame = (zclFrame_t *)(((zbApsdeDataIndication_t*)pData)->pAsdu);
        AF_PrepareForReply(&addrInfo, pIndication);
        
          /* Get the maximum ASDU length for the response */
        maxAsduLen = AF_MaxPayloadLen(&addrInfo);
                  
        {        
#if gFragmentationCapability_d      

          zbRxFragmentedHdr_t *pRxFrag = (zbRxFragmentedHdr_t*)pIndication->fragmentHdr.pNextDataBlock;
          uint16_t iAsduLen;
         
           /* Count the attributes */
          attributesCount = pIndication->asduLength - sizeof(zclFrame_t);
          
          while (pRxFrag != NULL)
          {
            attributesCount += pRxFrag->iDataSize;
            pRxFrag = pRxFrag->pNextDataBlock;
          }
          
          attributesCount = attributesCount / sizeof(zclAttrId_t);
   
         /* Use buffer allocated by ZCL_CreateFrame */
         MSG_Free(pMsgRsp);
         pMsgRsp = ZCL_CreateFrame( &addrInfo, 
                              gZclCmdReadAttrRsp_c,
                              gZclFrameControl_FrameTypeGeneral | 
                              ((pFrame->frameControl & gZclFrameControl_DirectionRsp)?0:gZclFrameControl_DirectionRsp ) 
                              | gZclFrameControl_DisableDefaultRsp,          /* IN: frame control field */
                              &pFrame->transactionId, 
                              (uint8_t*)&payloadLen,
                              NULL);
         if(!pMsgRsp)
            break;
         
         iAsduLen = ZCL_BuildReadAttrResponseWithFragmentation(
                                              pDevice,                  /* IN: ptr to device, for instance data */
                                              pCluster,                 /* IN: cluster to read from */
                                              attributesCount,          /* IN: read attr count */
                                              pIndication,              /* IN: request input frame */
                                              AF_Payload(pMsgRsp));              /* OUT: response output frame */

          /* Check if fragmentation is required*/
          if(iAsduLen > AF_MaxPayloadLen(&addrInfo))
            addrInfo.txOptions |= gApsTxOptionFragmentationRequested_c;
          else
            payloadLen = (uint8_t)iAsduLen;
#else   
        /* build response frame */
        zclFrame_t *pFrameRsp; 
        
        pFrameRsp = (zclFrame_t *)(((uint8_t *)pMsgRsp) + gAsduOffset_c);
        pFrameRsp->frameControl = gZclFrameControl_FrameTypeGeneral | gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp;
        pFrameRsp->transactionId = pFrame->transactionId;
        pFrameRsp->command = gZclCmdReadAttrRsp_c;
        
        /* place code here for updating device */
        
        /* Demo code set the attribute with dummy value, the value should be read by etc. uart before calling ZCL_SetAttribute */ 
        //(void)ZCL_GetAttribute(appEndPoint, ClusterId, gZclAttrSmplMetRISCurrSummDlvrd_c, &Payload[0], NULL);
        
        /* number of attributes in the list */  
        attributesCount = (uint8_t)((uint8_t)(pIndication->asduLength - (uint8_t)sizeof(zclFrame_t)) / (uint8_t)sizeof(zclAttrId_t));

        /* Build response and send it */
        payloadLen = ZCL_BuildReadAttrResponse( pDevice,                  /* IN: ptr to device, for instance data */
                                                pCluster,                 /* IN: cluster to read from */
                                                attributesCount,          /* read attr count */
                                                pFrame,                   /* IN: request input frame */
                                                maxAsduLen,                /* IN: maximum ASDU length */ 
                                                pFrameRsp);               /* OUT: response output frame */
#endif
        }        
        (void)maxAsduLen;
        (void)ZCL_DataRequestNoCopy(&addrInfo, payloadLen, pMsgRsp);                                                    
      }
      break;
#if gZclEnableReporting_c
  case gZclUI_SendReportingAttributeRequest_c:
    {
      BeeAppsUpdateAsynchronousAttributes(attrId, clusterId);
      /* Give asynchronous attributes a little time to update before calling SendReport */
      TMR_StartTimer(gBeeAppsAsynchronousDeviceToSettleTimerID, gTmrSingleShotTimer_c, gBeeAppsAsynchronousDeviceToSettle_c, BeeAppsAsynchronousDeviceCallBack);
    }
    break;
#endif  
  case gZclUI_SETunnel_RequestTunnelReqReceived_c:
    {
      gAcceptTunnel = TRUE;
    }
    
  case gZclUI_ZCLResponse_c:
    {
      zclCmd_t command;
      uint8_t *pPayload;
     zbApsdeDataIndication_t *pIndication = (zbApsdeDataIndication_t *) pData;
      
      if (!pIndication)
        break;
      pPayload = ZCL_GetPayload(pIndication);
      command = ZCL_GetCommand(pIndication);
      
      if(command == gZclCmdReadAttrRsp_c)
      {
        zclCmdReadAttrRsp_t *pReadAttRspPayload;
        pReadAttRspPayload=(zclCmdReadAttrRsp_t*)pPayload;
        /* Update the Time attributes from first Read Attribure response */
        if (pReadAttRspPayload->aRsp[0].attrId == gZclAttrTime_c )
        {                        
          /* Reset the Keep Alive Counter*/
          gZclSENoOfFailedKeepAlive = 0;
            
          if(pReadAttRspPayload->aRsp->status == gZbSuccess_c)
          {
            gZclTimeServerAttrs.Time = FourBytesToUint32(pReadAttRspPayload->aRsp->aData);
            gZclTimeServerAttrs.TimeZone = FourBytesToUint32(((uint8_t*)pReadAttRspPayload->aRsp->aData) + mSizeOfTimeAttrRecord_c );   //next attr          
            gZclTimeServerAttrs.DstStart = FourBytesToUint32(((uint8_t*)pReadAttRspPayload->aRsp->aData) + 2*mSizeOfTimeAttrRecord_c); //next attr
            gZclTimeServerAttrs.DstEnd = FourBytesToUint32(((uint8_t*)pReadAttRspPayload->aRsp->aData) + 3*mSizeOfTimeAttrRecord_c);
            gZclTimeServerAttrs.DstShift =FourBytesToUint32(((uint8_t*)pReadAttRspPayload->aRsp->aData) + 4*mSizeOfTimeAttrRecord_c);            
            }
        }
	else if (pReadAttRspPayload->aRsp[0].attrId == gZclAttrTimeStatus_c)
        {
          if(pReadAttRspPayload->aRsp->status == gZbSuccess_c)
          {
             ZCL_CheckTimeServer(pIndication, pReadAttRspPayload->aRsp->aData[0]);
          }
          TS_SendEvent(gSETaskID, gZclEvtTimeServerQueried_c);
        }
#if gZclMirroring_d && gZclMirroringAutoCfg_d        
        else if (pReadAttRspPayload->aRsp[0].attrId == gZclAttrBasic_PhysicalEnvironmentId_c )
        {
          if(pReadAttRspPayload->aRsp->status == gZbSuccess_c)
          {
            zclMet_RequestMirrorReq_t req;
            afAddrInfo_t addrInfo;
            
            addrInfo.dstAddrMode = 0x02;
            Copy2Bytes(&addrInfo.dstAddr, &gESIAddr);
            addrInfo.dstEndPoint = pIndication->srcEndPoint;
            Set2Bytes(addrInfo.aClusterId, gZclClusterSmplMet_c) ;
            addrInfo.srcEndPoint = pIndication->dstEndPoint;
            addrInfo.txOptions = 0x01;
            addrInfo.radiusCounter = afDefaultRadius_c;
            FLib_MemCpy(&req.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
            req.zclTransactionId = gZclTransactionId++; 
            
            ZclMet_RequestMirrorReq(&req);
          }
        }
#endif        
        /* Other read atribute responses ....  */
        //if (pReadAttRspPayload->aRsp[0].attrId == otherAttrib)
        
      }
      
      /* Other responses ....  */
      //if(command == otherResp)
      
    }
    break;
    case gZclUI_FastPollReqRcv_c:
    {
#if !gASL_ZclSmplMet_AcceptFastPollModeReq_d
      (void)ZclSmplMet_AcceptFastPollModeReq(TRUE);
#endif
    }
    break;
    case gZclUI_AlarmGenerate_c:
    {
#if (gASL_ZclMet_Optionals_d)  
      zbClusterId_t aClusterId[2];
      uint8_t *pAlarmCode;
      uint8_t  min = 0, i = 0; 
      uint16_t count = 0;
      uint32_t time = 0;
      pAlarmCode = (uint8_t *)pData;
      Set2Bytes(aClusterId[0], gZclClusterALarms_c);
      Set2Bytes(aClusterId[1], gZclClusterSmplMet_c);
      time = Native2OTA32(ZCL_GetUTCTime());
      (void)ZCL_GetAttribute(appEndPoint, aClusterId[0], gZclAttrAlarms_AlarmCount_c, gZclServerAttr_c, &count, NULL);
      /*updates in the Alarm Table*/
      if(count < MaxAlarmsPermitted)
      {
        for(i=0; i < count; i++)
        /*test if code alarm exist for specified cluster*/
          if((gAlarmsTable[i].AlarmCode == pAlarmCode[0])&&(IsEqual2BytesInt(aClusterId[1], gAlarmsTable[i].ClusterID) == TRUE)) 
          {
              count--;
              break; 
          }
         count++; 
      }
      else{
        for(i=0; i < (count-1); i++)
          min = (gAlarmsTable[i].TimeStamp < gAlarmsTable[i+1].TimeStamp)? i:i+1;
        i = min;
      }
      gAlarmsTable[i].AlarmCode = pAlarmCode[0];
      Copy2Bytes(&gAlarmsTable[i].ClusterID, aClusterId[1]);
      gAlarmsTable[i].TimeStamp = time;  
      (void)ZCL_SetAttribute(appEndPoint, aClusterId[0], gZclAttrAlarms_AlarmCount_c, gZclServerAttr_c, &count);
#endif      
    }
    break;
  }
    ASL_UpdateDevice(endPoint,event);
  
}

/*****************************************************************************
  BeeAppDataIndication

  Process incoming ZigBee over-the-air messages.
*****************************************************************************/
void BeeAppDataIndication
  (
  void
  )
{
  apsdeToAfMessage_t *pMsg;
  zbApsdeDataIndication_t *pIndication;
  zbStatus_t status = gZclMfgSpecific_c;

  while(MSG_Pending(&gAppDataIndicationQueue))
  {
    /* Get a message from a queue */
    pMsg = MSG_DeQueue( &gAppDataIndicationQueue );

    /* ask ZCL to handle the frame */
    pIndication = &(pMsg->msgData.dataIndication);
    status = ZCL_InterpretFrame(pIndication);

    /* not handled by ZCL interface, handle cluster here... */
    if(status == gZclMfgSpecific_c)
    {
      /* insert manufacturer specific code here... */
	  ZCL_SendDefaultMfgResponse(pIndication);
    }

    /* Free memory allocated by data indication */
    AF_FreeDataIndicationMsg(pMsg);
  }
}

/*****************************************************************************
  BeeAppDataConfirm

  Process incoming ZigBee over-the-air data confirms.
*****************************************************************************/
void BeeAppDataConfirm
  (
  void
  )
{
  apsdeToAfMessage_t *pMsg;
  zbApsdeDataConfirm_t *pConfirm;

  while(MSG_Pending(&gAppDataConfirmQueue))
  {
    /* Get a message from a queue */
    pMsg = MSG_DeQueue( &gAppDataConfirmQueue );
    pConfirm = &(pMsg->msgData.dataConfirm);

    ZclApsAckConfirm(pConfirm->dstAddr.aNwkAddr, pConfirm->status);
    /* Action taken when confirmation is received. */
    if( pConfirm->status != gZbSuccess_c )
    {
      /* The data wasn't delivered -- Handle error code here */
      TS_SendEvent(gSETaskID, gZclEvtKeepAliveFailure_c);
    }
    /* Free memory allocated in Call Back function */
    MSG_Free(pMsg);
  }
}

#if gInterPanCommunicationEnabled_c

/*****************************************************************************
  BeeAppInterPanDataIndication

  Process InterPan incoming ZigBee over-the-air messages.
*****************************************************************************/
void BeeAppInterPanDataIndication(void)
{
  InterPanMessage_t *pMsg;
  zbInterPanDataIndication_t *pIndication;
  zbStatus_t status = gZclMfgSpecific_c;

  while(MSG_Pending(&gInterPanAppDataIndicationQueue))
  {
    /* Get a message from a queue */
    pMsg = MSG_DeQueue( &gInterPanAppDataIndicationQueue );

    /* ask ZCL to handle the frame */
    pIndication = &(pMsg->msgData.InterPandataIndication );
    status = ZCL_InterpretInterPanFrame(pIndication);

    /* not handled by ZCL interface, handle cluster here... */
    if(status == gZclMfgSpecific_c)
    {
      /* insert manufacturer specific code here... */
    }

    /* Free memory allocated by data indication */
    MSG_Free(pMsg);
  }
}

/*****************************************************************************
  BeeAppDataConfirm

  Process InterPan incoming ZigBee over-the-air data confirms.
*****************************************************************************/
void BeeAppInterPanDataConfirm
(
void
)
{
  InterPanMessage_t *pMsg;
  zbInterPanDataConfirm_t *pConfirm;
  
  while(MSG_Pending(&gInterPanAppDataConfirmQueue))
  {
    /* Get a message from a queue */
    pMsg = MSG_DeQueue( &gInterPanAppDataConfirmQueue );
    pConfirm = &(pMsg->msgData.InterPandataConf);
    
    /* Action taken when confirmation is received. */
    if( pConfirm->status != gZbSuccess_c )
    {
      /* The data wasn't delivered -- Handle error code here */
    }
    
    /* Free memory allocated in Call Back function */
    MSG_Free(pMsg);
  }
}
#endif 


/*****************************************************************************
  BeeAppServiceDiscoveryTimeoutCallback()

  Announces that a new Service Discovery must be issued
*****************************************************************************/
void BeeAppServiceDiscoveryTimeoutCallback(tmrTimerID_t tmrid)
{
  (void) tmrid;
  TS_SendEvent(gSETaskID, gZclEvtESIDiscoveryTimeOut_c);
}

/*****************************************************************************
  BeeAppKeyEstabSuccess

  Sets the flag for Key Established and sends the ZCL in Service Discovery State
*****************************************************************************/
void BeeAppKeyEstabSuccess(void)
{    
  if (!gZclSE_KeyEstablished)
  {
    gZclSE_KeyEstablished = TRUE;
  }
}

/*****************************************************************************
  BeeAppServiceDiscWaitCallback

  Sends an event to the SE state machine that the device has finished the 
  service discovery procedure
*****************************************************************************/
void BeeAppServiceDiscWaitCallback(tmrTimerID_t tmrid)
{
  (void) tmrid;
  TS_SendEvent(gSETaskID, gZclEvtServiceDiscoveryFinished_c);
}

/*****************************************************************************
  BeeAppSEDeviceRunningCallBack

  Sends an event to the SE state machine that the device can enter the running 
  mode
*****************************************************************************/
void BeeAppSEDeviceRunningCallBack(tmrTimerID_t tmrid)
{
  (void) tmrid;
  TS_SendEvent(gSETaskID, gZclEvtSEDeviceRunning_c);
}

void BeeAppEstablishLinkKeyWithESI(zbIeeeAddr_t aIeeeAddr, zbNwkAddr_t aNwkAddr)
{
  zbApsmeRequestKeyReq_t  apsmeRequestKey;
  zbAddressMap_t addrMap;
   
  /* Fill destination address */
  (void)AddrMap_SearchTableEntry(NULL, (zbNwkAddr_t*)gTrustCenterAddr, &addrMap);
  Copy8Bytes(apsmeRequestKey.aDestAddress, addrMap.aIeeeAddr);
  
  /* Fill partner address */
  Copy8Bytes(apsmeRequestKey.aPartnerAddress, aIeeeAddr);
  
  apsmeRequestKey.keyType = gApplicationMasterKey_c;
  
  APP_ZDP_RequestKeyRequest(NULL, (uint8_t*)gTrustCenterAddr, &apsmeRequestKey);
}

void BeeAppAddDeviceToESIList(ASL_SendingNwkData_t sendingNwkData)
{
  index_t i;
  zbApsDeviceKeyPairSet_t  *pSecurityMaterial = NULL;
  zbAddressMap_t addressMapEntry;
  
  for (i = 0; i < gMaxNoOfESISupported_c; i++)
  {
    if ( (gZclSeESIList[i].EntryStatus == gEntryNotUsed_c) ||
        IsEqual2Bytes(gZclSeESIList[i].NwkAddr, sendingNwkData.NwkAddrOfIntrest))
      break;
  }
  
  /* We register the TC when we discover the KE cluster. Remember the endpoint */
  if (i == 0)
    gTCEndPoint = sendingNwkData.endPoint;
  
  if (i < gMaxNoOfESISupported_c)
  {
    Copy2Bytes(&gZclSeESIList[i].NwkAddr, &sendingNwkData.NwkAddrOfIntrest);
    gZclSeESIList[i].EndPoint = sendingNwkData.endPoint;
    gZclSeESIList[i].EntryStatus = gEntryUsed_c;
    
    if (!Cmp2BytesToZero(&sendingNwkData.NwkAddrOfIntrest))
    {
      /* Establish application link key with non TC ESIs. */
      
      (void)AddrMap_SearchTableEntry(NULL, &sendingNwkData.NwkAddrOfIntrest, &addressMapEntry);
#ifndef gHostApp_d
#if gStandardSecurity_d || gHighSecurity_d      
      {
        zbApsDeviceKeyPairSet_t whereToCpyEntry;
        pSecurityMaterial = SSP_ApsGetSecurityMaterilaEntry(addressMapEntry.aIeeeAddr, &whereToCpyEntry);
      }
#endif        
#endif
      if (!pSecurityMaterial)
      {
        uint8_t *pIeeeAddr, aExtAddr[8];        
        
        pIeeeAddr = APS_GetIeeeAddress(gZclSeESIList[i].NwkAddr, aExtAddr);
        
        if (pIeeeAddr)
        {
          BeeAppEstablishLinkKeyWithESI(pIeeeAddr, gZclSeESIList[i].NwkAddr);
        }
        else
        {
          /* Find the device's IEEE address if we don't have it*/
          (void)ASL_IEEE_addr_req(NULL, sendingNwkData.NwkAddrOfIntrest, sendingNwkData.NwkAddrOfIntrest, 0x00, 0x00);
        }          
      }
    }    
  }
}

/*****************************************************************************
* BeeAppMessageTimerCallback
*
* Message timer callback.
*****************************************************************************/

void BeeAppMessageTimerCallback(tmrTimerID_t timer)
{  

  (void) timer;
  switch (pLastMsgReceived->EntryStatus)
  {
  case gMessagePending_c: 
    /* Display message */
    BeeAppDisplayMessage();
    if (pLastMsgReceived->DurationInMinutes < 0xffff)
    {
      /* Start duration timer */
      TMR_StartMinuteTimer(mMessageTimer, pLastMsgReceived->DurationInMinutes, BeeAppMessageTimerCallback);
    }
    pLastMsgReceived->EntryStatus = gMessageDisplayed_c;
    break;
    
  case gMessageDisplayed_c:
    /* Delete message */
    BeeAppClearCurrentMsg();    
    ASL_SetLed(LED2, gLedOff_c);
    pLastMsgReceived->EntryStatus = gNoMessage_c;
    break;
    
  }

}
/*****************************************************************************/
void BeeAppSendMsgConfReq()
{
  uint32_t otaTime = Native2OTA32(ZCL_GetUTCTime());
  /*check if it is a InterPan msg confirmation */
  if(pLastMsgReceived->MsgCtrl & 0x02)
  { 
#if gInterPanCommunicationEnabled_c    
    /*Send the msg confirmation*/
    zclInterPanMsgConfReq_t confInterPanReq;
    FLib_MemCpy(&confInterPanReq.addrInfo, &(pLastMsgReceived->addrInfo), sizeof(InterPanAddrInfo_t));
    confInterPanReq.zclTransactionId = gZclTransactionId++;
    FLib_MemCpy(&confInterPanReq.cmdFrame.MsgID[0], &pLastMsgReceived->MsgID[0] , sizeof(MsgId_t));
    confInterPanReq.cmdFrame.ConfTime = otaTime;  
    
    (void)ZclMsg_InterPanMsgConf(&confInterPanReq);
#endif    
  }
  
  else 
  {
    /* send the msg confirmation for SE device */
    zclMsgConfReq_t confReq;
    FLib_MemCpy(&(confReq.addrInfo), &(pLastMsgReceived->addrInfo), sizeof(afAddrInfo_t));
    confReq.zclTransactionId = gZclTransactionId++;
    FLib_MemCpy(&confReq.cmdFrame.MsgID[0], &pLastMsgReceived->MsgID[0] , sizeof(MsgId_t));
    confReq.cmdFrame.ConfTime = otaTime;  
    (void)ZclMsg_MsgConf(&confReq);
  }
  /* msg was confirm */
  ASL_AppSetLed(LED2, gLedStopFlashing_c);
}

/*****************************************************************************/
void BeeAppGetLastMsgReq(void)
{
  /* Is it a InterPan msg source address InterPan ??*/
  if (pLastMsgReceived->MsgCtrl & 0x02)
  {
#if gInterPanCommunicationEnabled_c    
    /* Send the InterPan Get Last Msg Req */
    zclInterPanGetLastMsgReq_t interPanGetLastMsg;
    FLib_MemCpy(&interPanGetLastMsg, pLastMsgReceived, sizeof(InterPanAddrInfo_t));
    interPanGetLastMsg.zclTransactionId = gZclTransactionId++;
    (void)ZclMsg_InterPanGetLastMsgReq(&interPanGetLastMsg);
#endif    
  } 
  else
  {   
    zclGetLastMsgReq_t getLastMsg;
    FLib_MemCpy(&getLastMsg, pLastMsgReceived, sizeof(afAddrInfo_t));
    getLastMsg.zclTransactionId = gZclTransactionId++;
    /* Send request for last message */ 
    (void)ZclMsg_GetLastMsgReq(&getLastMsg);
  } 
}          

void BeeAppSendCPPEvtRsp(uint8_t status)
{
  afAddrInfo_t addrInfo; 
  zclPrice_CPPEventRsp_t *pCPPEventRsp;
  
  pCPPEventRsp = AF_MsgAlloc();
  
  if (!pCPPEventRsp)
    return;
  
  addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
  Set2Bytes(&addrInfo.aClusterId, gZclClusterPrice_c);
  Copy2Bytes(&addrInfo.dstAddr, &gZclSeESIList[0].NwkAddr);
  addrInfo.txOptions = afTxOptionsDefault_c;
  addrInfo.srcEndPoint = appEndPoint;
  addrInfo.dstEndPoint = gZclSeESIList[0].EndPoint;
  addrInfo.radiusCounter = afDefaultRadius_c;
   
  FLib_MemCpy(&pCPPEventRsp->addrInfo, &addrInfo, sizeof(afAddrInfo_t));
  pCPPEventRsp->zclTransactionId = gZclTransactionId++;
  pCPPEventRsp->cmdFrame.IssuerEvtID = gaClientCPPEventTable[mCPPEvtIndex].CPPEvent.IssuerEvtID;
  pCPPEventRsp->cmdFrame.CPPAuth = status;

  if (zclPrice_CPPEventRsp(pCPPEventRsp) == gZbSuccess_c)
  {
    /* CPP has been addressed */
    mCPPEvtIndex = gInvalidTableIndex_c;
    ASL_SetLed(LED4, gLedOff_c);
  }
  

  
  MSG_Free(pCPPEventRsp);
}

static void BeeAppDisplayMessage()
{ 
  
#if gLCDSupported_d      
  char *pMsg;
#endif  
 
  /* Ask the user to confirm the message if requested */
  if((pLastMsgReceived->MsgCtrl & gZclSEMsgCtrl_MsgConfRequired_c) == gZclSEMsgCtrl_MsgConfRequired_c)
  {    
    ASL_SetLed(LED2, gLedFlashing_c);
  /* Display the message */  
#if gLCDSupported_d    
    pMsg = "Msg rcvd confirm";
  }
  else
    pMsg = "Msg rcvd        ";
  
  ASL_PrintEvent(pMsg);
  
  #if gTargetQE128EVB_d || gTargetMC1323xRCM_d
  nextField = gASL_FieldMessage_c;
  ASL_PrintField(nextField, &pLastMsgReceived->cmdFrame.Msg.aStr[0]);
  #elif gTargetMC1322xNCB
  ASL_PrintMessage((uint8_t*)&pLastMsgReceived->cmdFrame.Msg.aStr[0]);
  #endif
  
#else
  }
#endif  
#if gASL_PrintToTestClient_d
  ASL_PrintToTestClient(pLastMsgReceived->msgData);
#endif   
 
       
}
/*****************************************************************************
  BeeAppGetCurrentPrice()

 Sent the Get Current Price Command
*****************************************************************************/
void BeeAppGetCurrentPrice(void)
{
  zclPrice_GetCurrPriceReq_t req;
  
  req.addrInfo.dstAddrMode = 0x02; /*short address*/
  Copy2Bytes(req.addrInfo.dstAddr.aNwkAddr, gTrustCenterAddr);
  req.addrInfo.dstEndPoint = gSendingNwkData.endPoint;
  Set2Bytes(req.addrInfo.aClusterId, gZclClusterPrice_c);
  req.addrInfo.srcEndPoint = appEndPoint;
  req.addrInfo.txOptions = afTxOptionsDefault_c;
  req.addrInfo.radiusCounter = afDefaultRadius_c;
  req.zclTransactionId = gZclTransactionId++;
  // receive price updates from ESP
  req.cmdFrame.CmdOptions = gGetCurrPrice_RequestorRxOnWhenIdle_c;
  
  (void)zclPrice_GetCurrPriceReq (&req);
}

/*****************************************************************************
  BeeAppHandlePrice()

 Print Information about received prices...
 Save the current price and get the new price if the current price expires.
*****************************************************************************/
void BeeAppHandlePrice(void *pData)
{
  publishPriceEntry_t *pPriceData;
  
  pPriceData = (publishPriceEntry_t *)pData; 
  /* Print Price Information */
  switch(pPriceData->EntryStatus)
  {
  case gPriceReceivedStatus_c:
    break;
  case gPriceStartedStatus_c:
    {
      uint8_t currentPriceTier, currentBlockThreshold;
      pCurrentPrice = pPriceData;
      
      /* Check for new TOU period inside a Block period*/
      if ((curentBlockPeriod.EntryStatus == gBlockPeriodStartedStatus_c) ||
          (curentBlockPeriod.EntryStatus == gBlockPeriodUpdateStatus_c))
      {
        currentPriceTier = pCurrentPrice->Price.Price.PriceTrailingDigitAndPriceTier & 0x0F;
        currentBlockThreshold = curentBlockPeriod.blockPeriod.NrOfPriceTiersNrOfBlockThresholds & 0x0F;
        
        /* Get the price information from the server */
        BeeAppRequestPriceInfoFromESI(currentPriceTier, currentBlockThreshold);
      }
    }
    
    break;
  case gPriceUpdateStatus_c:
    break;
  case gPriceCompletedStatus_c:
    pCurrentPrice = NULL;//current price expired (not available)
    break;		
  default:
    break;
  }
#if gLCDSupported_d || gASL_PrintToTestClient_d
  /* Print the status */
  ASL_PrintPriceEvtStatus(pPriceData->EntryStatus);
  /* Here, start to print with the first field */  
  if (pCurrentPrice->EntryStatus != 0xff)  
    nextField = gASL_FieldLabel_c;
  else
    nextField = gASL_FieldTime_c;
    ASL_PrintField(nextField, &pPriceData->Price);    
#endif /* #if gLCDSupported_d */

}

/*****************************************************************************
  BeeAppHandleBlockPeriod()

 Save the current Block Period and get the new price if the current Block Period expires.
*****************************************************************************/

void BeeAppHandleBlockPeriod(void *pData)
{
  publishBlockPeriodEntry_t *pBlockPeriodData;
  
  pBlockPeriodData = (publishBlockPeriodEntry_t *)pData; 
  switch(pBlockPeriodData->EntryStatus)
  {
  case gBlockPeriodReceivedStatus_c:
    break;
  case gBlockPeriodStartedStatus_c:
    {
      zbClusterId_t aClusterId={gaZclClusterPrice_c}; 
      
      uint8_t currentPriceTier, currentBlockThreshold;
      FLib_MemCpy(&curentBlockPeriod, pData, sizeof(publishBlockPeriodEntry_t));
      currentPriceTier = pCurrentPrice->Price.Price.PriceTrailingDigitAndPriceTier & 0x0F;
      currentBlockThreshold = curentBlockPeriod.blockPeriod.NrOfPriceTiersNrOfBlockThresholds & 0x0F;
      
      /* Get the price information from the server */
      BeeAppRequestPriceInfoFromESI(currentPriceTier, currentBlockThreshold);
	  
	  /* Set StartofBlockPeriod attribute*/
      (void)ZCL_SetAttribute(appEndPoint, aClusterId, gZclAttrPrice_StartofBlockPeriod_c, gZclServerAttr_c, &curentBlockPeriod.EffectiveStartTime);
      /* Set BlockPeriodDuration attribute*/
      (void)ZCL_SetAttribute(appEndPoint, aClusterId, gZclAttrPrice_BlockPeriodDuration_c, gZclServerAttr_c, &curentBlockPeriod.blockPeriod.BlockPeriodDuration);

    }
    break;
  case gBlockPeriodUpdateStatus_c:
    break;
  case gBlockPeriodCompletedStatus_c:
    curentBlockPeriod.EntryStatus = 0xff;//current price expired (not available)
    break;		
  default:
    break;
  }
}


/*****************************************************************************
  BeeAppRequestPriceInfoFromESI()

  Sends a read attribute command to ESI in order to read the price information.
*****************************************************************************/
void BeeAppRequestPriceInfoFromESI(uint8_t currentTier, uint8_t currentThreshold)
{
  zclReadAttrReq_t pReq;
  uint8_t idx;

  /* Direct Address for the ESI */
  pReq.addrInfo.dstAddrMode = 2; 
  Copy2Bytes(pReq.addrInfo.dstAddr.aNwkAddr, gTrustCenterAddr);
  pReq.addrInfo.dstEndPoint = gSendingNwkData.endPoint;
  Set2Bytes(pReq.addrInfo.aClusterId, gZclClusterPrice_c);

  pReq.addrInfo.srcEndPoint = appEndPoint;
  /*Set security options to aps security if link keys are enabled*/
  pReq.addrInfo.txOptions = afTxOptionsDefault_c;
  pReq.addrInfo.radiusCounter = afDefaultRadius_c;
  
  /* Request Time, TimeZone and DST Start time attributes */
  pReq.count = currentThreshold;
  
  for (idx = 0; idx < currentThreshold; idx++)
  {
#if (gBigEndian_c)  
    pReq.cmdFrame.aAttr[idx] = gZclAttrPrice_BlockPriceInfoSet_c + (((16 * currentTier) + idx) << 8);
#else
   pReq.cmdFrame.aAttr[idx] = (gZclAttrPrice_BlockPriceInfoSet_c << 8) + (16 * currentTier) + idx;
#endif  
  }
  (void)ZCL_ReadAttrReq(&pReq);
}
/*****************************************************************************
  BeeAppClearCurrentMsg()

  Clear the last received msg.
*****************************************************************************/
void BeeAppClearCurrentMsg(void)
{  
#if gTargetQE128EVB_d || gTargetMC1323xRCM_d
  FLib_MemCpy(&currentMsgReceived, "NoMsg           ", 16);

#if gLCDSupported_d  
  nextField = gASL_FieldMessage_c;
  ASL_PrintField(nextField, &currentMsgReceived);
#endif  
#elif gTargetMC1322xNCB
#if gLCDSupported_d
  FLib_MemSet(currentMsgReceived,  gMaxRcvdMsgToDisplay, ' ');
  ASL_PrintMessage(&currentMsgReceived[0]);
#endif  
#endif
  
#if gASL_PrintToTestClient_d
  FLib_MemSet(pLastMsgReceived->msgData, gMAX_LCD_CHARS_c, ' ');
  FLib_MemCpy(pLastMsgReceived->msgData, &currentMsgReceived, 5);
  pLastMsgReceived->length = 5;
  ASL_PrintToTestClient(pLastMsgReceived->msgData);
#endif           
}

/*****************************************************************************
  BeeAppsAsynchronousDeviceCallBack

  Calling SendReport in Zclmain.c.
*****************************************************************************/
void BeeAppsAsynchronousDeviceCallBack(tmrTimerID_t timerID)
{
  (void)timerID;  /* prevent compiler warning */
  
  TS_SendEvent(gZclTaskId, gZclEventReportTimeout_c);
}

#if gZclEnableReporting_c
/*****************************************************************************
  BeeAppsUpdateAsynchronousAttributes

  Updates asynchronous attributes.
*****************************************************************************/
void BeeAppsUpdateAsynchronousAttributes(zclAttrId_t attrId, zbClusterId_t clusterId)
{
  /* Check if cluster is for simple metering device */
  if(TwoBytesToUint16(clusterId) == gZclClusterSmplMet_c)
  {
    switch (attrId)
    {
      case gZclAttrMetRISCurrSummDlvrd_c: 
        {
          /* Demo code update the attribute with a random value, the value should be read by etc. uart before calling ZCL_SetAttribute */ 
          UpdatePowerConsumption();
        }
        break;
      /* insert code for other asynchronous device attribute. 
      case gOtherAsynchronousDevicesId_c:
        break;*/
      default:
      break;    
    }
  }
  
}
#endif


/*****************************************************************************
  BeeAppRequestPhyEnvFromESI()

  Sends a read attribute command to ESI in order to read the time.
*****************************************************************************/
void BeeAppRequestPhyEnvFromESI(void)
{
  zclReadAttrReq_t pReq;

  /* Direct Address for the ESI*/
  pReq.addrInfo.dstAddrMode = 2; 
  Copy2Bytes(pReq.addrInfo.dstAddr.aNwkAddr, gESIAddr);
  pReq.addrInfo.dstEndPoint = gSendingNwkData.endPoint;
  
  Set2Bytes(pReq.addrInfo.aClusterId, gZclClusterBasic_c);

  pReq.addrInfo.srcEndPoint = appEndPoint;
  /*Set security options to aps security if link keys are enabled*/
  pReq.addrInfo.txOptions = gApsTxOptionUseNwkKey_c;
  pReq.addrInfo.radiusCounter = afDefaultRadius_c;
  /* Request Time, TimeZone and DST Start time attributes */
  pReq.count = 1;
  pReq.cmdFrame.aAttr[0] = gZclAttrBasic_PhysicalEnvironmentId_c;  
  (void)ZCL_ReadAttrReq(&pReq);
}

/*****************************************************************************
  BeeAppRequestCommodityTypeFromESI()

  Sends a read attribute command to ESI in order to read the commodity type.
*****************************************************************************/
void BeeAppRequestCommodityTypeFromESI(void)
{
  zclReadAttrReq_t pReq;

  /* Direct Address for the ESI*/
  pReq.addrInfo.dstAddrMode = 2; 
  Copy2Bytes(pReq.addrInfo.dstAddr.aNwkAddr, gESIAddr);
  pReq.addrInfo.dstEndPoint = gSendingNwkData.endPoint;
  
  Set2Bytes(pReq.addrInfo.aClusterId, gZclClusterBasic_c);

  pReq.addrInfo.srcEndPoint = appEndPoint;
  pReq.addrInfo.txOptions = gApsTxOptionSecEnabled_c;
  pReq.addrInfo.radiusCounter = afDefaultRadius_c;
  pReq.count = 1;
  pReq.cmdFrame.aAttr[0] = gZclAttrIdPrice_CommodityType_c;  
  (void)ZCL_ReadAttrReq(&pReq);
}
/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************/
