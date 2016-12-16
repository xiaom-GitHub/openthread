/******************************************************************************
* ZclOTA.c
*
* This module contains code that handles ZigBee Cluster Library commands and
* clusters for OTA (Over The Air upgrading cluster).
*
* Copyright (c) 2011, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* Documents used in this specification:
* [R1] - docs-05-3474-20-0csg-zigbee-specification
* [R2] - docs-07-5123-04-0afg-zigbee-cluster-library-specification.pdf
*******************************************************************************/

#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "ZclOptions.h"
#if gZclEnableOTAServer_d
#if defined(PROCESSOR_HCS08)||defined(PROCESSOR_MC13233C)||defined(PROCESSOR_MC13234C)
#include "BootloaderFlashUtils.h"
#endif
#endif
#if gZclEnableOTAServer_d || gZclEnableOTAClient_d

#include "OtaSupport.h"
#include "ZclOta.h"
#include "ApsMgmtInterface.h"
#include "ASL_UserInterface.h"
#include "ASL_ZdpInterface.h"
#include "EndPointConfig.h"
#include "ZTCInterface.h"

#ifdef PROCESSOR_KINETIS
#include "Eeprom.h"
#endif

#if (gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d || gZclOtaServerImgIntegrityCodeValidation_d)
#ifdef PROCESSOR_KINETIS
#include "mmcau_interface.h"
#else
#include "ASM_interface.h"
#endif /* PROCESSOR_KINETIS */
#endif /* gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d || gZclOtaServerImgIntegrityCodeValidation_d*/

#if gZclEnableOTAClientECCLibrary_d
#include "eccapi.h"
#endif

#ifndef PROCESSOR_KINETIS
#include "PwrLib.h"
#else
#include "PWR_Interface.h"
#endif

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

#define gHaZtcOpCodeGroup_c                 0x70 /* opcode group used for HA commands/events */
#define gOTAImageProgressReport_c           0xDA /* for sending to external app the image progress report */
#define mZtcOtaSupportImageChunkReq_c       0x2F /* for sending a chunk request to host app */
#define mZtcOtaSupportQueryImageReq_c       0xC2 /* for sending a query nex image request */

#define HEADER_LEN_OFFSET                   6
#define SESSION_BUFFER_SIZE                 128
#define BLOCK_PROCESSING_CALLBACK_DELAY     50  /* ms */
#define AES128_KeyExpansion_d 44

#if gZclEnableOTAServer_d
#if defined(PROCESSOR_HCS08)||defined(PROCESSOR_MC13233C)||defined(PROCESSOR_MC13234C)
#define gExtFlash_TotalSize_c               gFlashParams_MaxImageLength_c
#else
#ifdef PROCESSOR_KINETIS
#define gExtFlash_TotalSize_c  gEepromParams_TotalSize_c
#else
#define gExtFlash_TotalSize_c               0x20000  
#endif
#endif
#endif

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

#if gZclEnableOTAServer_d
zbStatus_t ZCL_OTAClusterServer_NextImageRequest_Handler(zbApsdeDataIndication_t *pIndication);
static bool_t OTAServer_CheckAbortBlockRspList(zbNwkAddr_t   deviceID);
static void OTAClusterProcessAES_MMOHash(uint8_t *pImageBlock, uint8_t blockLength);

#if gZclOtaExternalMemorySupported_d
static void OTAServerBlockRequestTmrCallback(uint8_t tmr);
static void OTAServerQueryImageRequestTmrCallback(uint8_t tmr);
static uint8_t ReadAndTestOtaHeader(uint16_t manufacturerCode, uint16_t imageType, uint32_t fileVersion, bool_t testFileVersion);
static uint8_t OTAServerSearchForImageRequest(uint16_t manufacturerCode, uint16_t imageType, uint32_t fileVersion, bool_t testFileVersion);
#endif

static void ZCL_OTAClusterServer_AddToTemporaryQueryReqList(zbNwkAddr_t   aNwkAddr, zbEndPoint_t  endPoint);
static zbEndPoint_t ZCL_OTAClusterServer_GetEndpointFromTemporaryQueryList(zbNwkAddr_t   aNwkAddr);
#endif

#if gZclEnableOTAClient_d
void OTAClusterClientAbortSession(void);
zbStatus_t OTAClusterClientStartSession(uint32_t fileLength, uint32_t fileVersion);
zbStatus_t OTAClusterClientRunImageProcessStateMachine();
zbStatus_t OTAClusterClientProcessBlock(uint8_t *pImageBlock, uint8_t blockLength);
#if gEndDevCapability_d || gComboDeviceCapability_d 
static void OTAClient_UpdatePollRateCallBack(uint8_t tmrId);
#endif
static void OTAClusterClientProcessBlockTimerCallback(uint8_t tmr);
static void OTAClientRetransmitLastBlockTmrCallback(uint8_t tmr);
static void OTAClusterCPUResetCallback(uint8_t tmr);
static void OTAClusterDelayedUpgradeCallback(uint8_t tmr);
static void OTAClusterClientNextImageReqTimerCallback(uint8_t tmr);
static void OTAClusterClientMatchDescReqTimerCallback(uint8_t tmr);
static zbStatus_t ZCL_OTAClusterClient_RetransmitBlockRequest(zbIeeeAddr_t  ieeeAddr, zbEndPoint_t  endPoint);
static zbStatus_t ZCL_OTAClusterClient_QueryNextImageRequest(zbIeeeAddr_t  ieeeAddr,  zbEndPoint_t  endPoint);
static zbStatus_t ZCL_OTABlockRequestCmd(afAddrInfo_t addrInfo, zclOTABlockRequest_t  zclOTABlockRequest);
static zbStatus_t ZCL_OTAImageRequestCmd(afAddrInfo_t addrInfo, zclOTANextImageRequest_t zclOTANextImageRequest);
#endif

#if gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d || gZclOtaServerImgIntegrityCodeValidation_d
#ifdef PROCESSOR_KINETIS
void encryptAES(uint8_t* key,  uint8_t* input_data, uint8_t* output_data, uint8_t data_length, uint8_t* init_vector);
#endif /* PROCESSOR_KINETIS */
#endif /* gZclEnableOTAClientECCLibrary_d */



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

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/******************************
  OTA Cluster Data
  See ZCL Specification Section 6.3
*******************************/
zclOTAAttrsRAM_t gOTAAttrs = 
{
  {0xFF, 0xFF, 0xFF, 0xFF,0xFF, 0xFF, 0xFF, 0xFF},   /* server IEEE address */
  gOTAUpgradeStatusNormal_c                          /* status  */
#if gZclClusterOptionals_d
  ,0,                                                /* file offset */
  OTA_FILE_VERSION_CURRENT,                          /* current version */
  OTA_FILE_VERSION_CURRENT                           /* downloaded version */
#if gZigbeeProIncluded_d
  ,gZclStackVer_ZigBeePro_c,                         /* stack version */ 
  gZclStackVer_ZigBeePro_c,                          /* downloaded stack version */
#else
  ,gZclStackVer_ZigBee2007_c,                        /* stack version */
  gZclStackVer_ZigBee2007_c,                         /* downloaded stack version */
#endif /* gZigbeeProIncluded_d */
  OTA_MANUFACTURER_CODE_FSL,                         /* manufacturer Code  */
  OTA_IMAGE_TYPE_CURRENT,                            /* image type */   
  0x0000                                             /* minimum activation delay = 0 = no delay */
#endif /*gZclClusterOptionals_d*/
};

const zclAttrDef_t gaZclOTAClusterAttrDef[] = 
{
  { gZclAttrIdOTA_UpgradeServerId_c,              gZclDataTypeIeeeAddr_c,  gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c,sizeof(IEEEaddress_t), &gOTAAttrs.UpgradeServerId},//(void *)MbrSizeof(zclOTAAttrsRAM_t, UpgradeServerId)},
  { gZclAttrIdOTA_ImageUpgradeStatusId_c,           gZclDataTypeEnum8_c, gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c, sizeof(uint8_t), &gOTAAttrs.ImageUpgradeStatus},//(void *)MbrSizeof(zclOTAAttrsRAM_t, ImageUpgradeStatus)},
#if gZclClusterOptionals_d
  { gZclAttrIdOTA_FileOffsetId_c,                   gZclDataTypeUint32_c, gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c,sizeof(uint32_t), &gOTAAttrs.FileOffset},
  { gZclAttrIdOTA_CurrentFileVersionId_c,           gZclDataTypeUint32_c, gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c,sizeof(uint32_t), &gOTAAttrs.CurrentFileVersion},
  { gZclAttrIdOTA_CurrentZigBeeStackVersionId_c,    gZclDataTypeUint16_c, gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c,sizeof(uint16_t), &gOTAAttrs.CurrentZigBeeStackVersion},
  { gZclAttrIdOTA_DownloadedFileVersionId_c,        gZclDataTypeUint32_c, gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c,sizeof(uint32_t), &gOTAAttrs.DownloadedFileVersion},
  { gZclAttrIdOTA_DownloadedZigBeeStackVersionId_c, gZclDataTypeUint16_c, gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c,sizeof(uint16_t), &gOTAAttrs.DownloadedZigBeeStackVersion},
  { gZclAttrIdOTA_ManufacturerId_c, gZclDataTypeUint16_c, gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c,sizeof(uint16_t), &gOTAAttrs.ManufacturerId},
  { gZclAttrIdOTA_ImageTypeId_c, gZclDataTypeUint16_c, gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c,sizeof(uint16_t), &gOTAAttrs.ImageType},
  { gZclAttrIdOTA_MinimumBlockRequestDelayId_c, gZclDataTypeUint16_c, gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c,sizeof(uint16_t), &gOTAAttrs.MinimumBlockReqDelay},
#endif
};

const zclAttrSet_t gaZclOTAClusterAttrSet[] = 
{
  {gZclAttrOTASet_c, (void *)&gaZclOTAClusterAttrDef, NumberOfElements(gaZclOTAClusterAttrDef)}
};

const zclAttrSetList_t gZclOTAClusterAttrSetList = 
{
  NumberOfElements(gaZclOTAClusterAttrSet),
  gaZclOTAClusterAttrSet
};

const zclCmd_t gaZclOTAClusterCmdReceivedDef[]=
{
  /* commands received */ 
  gZclCmdOTA_QueryNextImageRequest_c,
  gZclCmdOTA_ImageBlockRequest_c,
  gZclCmdOTA_UpgradeEndRequest_c
};

const zclCmd_t gaZclOTAClusterCmdGeneratedDef[]=
{
  /* commands generated */
  gZclCmdOTA_ImageNotify_c,
  gZclCmdOTA_QueryNextImageResponse_c,
  gZclCmdOTA_ImageBlockResponse_c,
  gZclCmdOTA_UpgradeEndResponse_c
};

const zclCommandsDefList_t gZclOTAClusterCommandsDefList =
{
   NumberOfElements(gaZclOTAClusterCmdReceivedDef), gaZclOTAClusterCmdReceivedDef,
   NumberOfElements(gaZclOTAClusterCmdGeneratedDef), gaZclOTAClusterCmdGeneratedDef
};

/*  Size optimization. The values represent  should represent the size of the Image Notify Command
 based on the payload type.*/
const uint8_t imgNotifySizeArray[gPayloadMax_c] = {2, 4, 6, 10};
uint8_t gZclTransactionOtaId = 0x00;

#if gZclEnableOTAClient_d
/* client default information */
static uint8_t gOTAClient_RequestedTsqId = 0;
static uint8_t mMultipleUpgradeImage = 0;

const uint8_t OtaUpgrade_OutputClusterList[2] = 
{
    gaZclClusterOTA_c,  /* Ota Upgrade Cluster  */ 
};

bool_t mSendOtaMatchDescriptorRequest = FALSE;
bool_t mSendOTAIEEEAddrRequest = FALSE;

zbSimpleDescriptor_t otaCluster_simpleDescriptor = 
{
    8, /* Endpoint number */
    0x0,0x0, /* Application profile ID */
    0x0, 0x0, /* Application device ID */
    0, /* Application version ID */
    1, /* Number of input clusters */
    (uint8_t *)OtaUpgrade_OutputClusterList, /* Input cluster list */
    0, /* Number of output clusters */
    NULL, /* Output cluster list */
};  

zclOTAClientParams_t  clientParams = 
{
    OTA_MANUFACTURER_CODE_FSL,
    OTA_IMAGE_TYPE_CURRENT,
    OTA_FILE_VERSION_CURRENT,
    OTA_FILE_VERSION_DL_DEFAULT,
    OTA_HW_VERSION_CURRENT,
    OTA_CLIENT_MAX_DATA_SIZE
};


zclOTAClientSession_t clientSession = 
{ 
    FALSE,                      /* sessionStarted */
    0x00000000,                 /* fileLength   */
    0x00000000,                 /* currentOffset  */
    0x00000000,                 /* downloadingFileVersion */
    0x00,                       /* bytesNeedForNextState */
    NULL,                       /* pStateBuffer */
    0x00,                       /* stateBufferIndex */
    OTAClient_InitState_c,      /* state */
    0,                          /* steps */    
    0,                          /* serverEndpoint */
    0                           /* blockRequestDelay */   
};
zclOTABlockCallbackState_t* gpBlockCallbackState = NULL;
tmrTimerID_t gBlockCallbackTimer = gTmrInvalidTimerID_c;
tmrTimerID_t gRetransmitLastBlockTimer = gTmrInvalidTimerID_c; 
tmrTimerID_t gNextImageReqTimer = gTmrInvalidTimerID_c;
zbEndPoint_t gClientOTAEndpoint = 0x00;
uint8_t gRetransmitCounter = 0x00;

#if  (gZclEnableOTAProgressReport_d)
uint8_t gOtaProgressReportStatus = otaStartProgress_c;  //have 4 states: 0 = startProgress, 1 = 33%, 2 = 66%, 3 = 100%
#endif

#if (gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d)
static uint8_t  mMsgDigest[AES_MMO_SIZE];
#endif  

#endif

#if gZclEnableOTAServer_d
/* server default information */
#define gMaxNumberOfOTAImage_c   2   /* max number of the ota images supported*/

static ztcServerUtility_t mServerUtility = 
{
#if gZclOtaExternalMemorySupported_d		
    gUseExternalMemoryForOtaUpdate_c, /* operation mode  */
    gOtaServerLoadingImageComplete_c, /* imageLoadingState */
    0,                                /* currentAddress */   
    OTA_FILE_VERSION_SUPPORTED_UPGRADE | OTA_FILE_VERSION_SUPPORTED_DOWNGRADE, /* localFileVersionPolicies */
#else
    gDoNotUseExternalMemoryForOtaUpdate_c, /* operation mode */
#endif                                                
    FALSE,                                 /* otaServerIsActive*/   
    FALSE                            /* blockResponseInProgress */ 
};

#if gZclOtaExternalMemorySupported_d     
static ztcOtaServerImageListParam_t listOfImage[gMaxNumberOfOTAImage_c];
static uint8_t mIndexOfCurrentImage = 0;
#endif
static zclZtcOTABlockResponse_t  *pZtcBlockResponse;  
static zclOTAServerInitParams_t mOtaServerInitParams = 
{
    OTA_CLIENT_MAX_RANDOM_JITTER,  /* querryJitter */
    0,                             /* currentTime */
    40,                            /* upgradeRequestTime */
    0,                             /* upgradeTime */
    0                              /* blockRequestDelay: miliseconds*/    
};
tmrTimerID_t gRequestBlockTimer = gTmrInvalidTimerID_c; 

#define zclOTAServer_MaxTemporaryReqList_d 4
zclQueryReqList_t mTemporaryQueryReqList[zclOTAServer_MaxTemporaryReqList_d];
zclAbortBlockRspList_t  mTemporaryAbortBlockRspList[zclOTAServer_MaxTemporaryReqList_d] = 
{
  TRUE, 0x00, 0x00,
  TRUE, 0x00, 0x00,
  TRUE, 0x00, 0x00,
  TRUE, 0x00, 0x00
};

zbEndPoint_t gServerOTAEndpoint = 0x00;
#if gZclOtaServerImgIntegrityCodeValidation_d
static uint8_t  mMsgDigestServer[AES_MMO_SIZE];
#endif

#endif
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/


/*****************************************************************************
******************************************************************************
*
* ZCL OTA SERVER FUNCTIONS
*
******************************************************************************
*****************************************************************************/

#if gZclEnableOTAServer_d
/*****************************************************************************
* ZCL_OTAClusterServer Function
*
* Incoming OTA cluster frame processing loop for cluster server side
*****************************************************************************/
zbStatus_t ZCL_OTAClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
    zclFrame_t *pFrame;
    zclOTABlockRequest_t          blockRequest;
    zclOTAUpgradeEndRequest_t     upgradeEndRequest;
    ztcZclOTAUpgdareEndResponse_t upgradeEndResponse;
    zbStatus_t status = gZclSuccess_c;    
     /* Parameter not used, avoid compiler warning */
    (void)pDevice;
    
    /* ZCL frame */
    pFrame = (zclFrame_t*)pIndication->pAsdu;
    gZclTransactionOtaId = pIndication->pAsdu[1];
    
    if((gServerOTAEndpoint == 0x00) || (gServerOTAEndpoint==0xFF))
      gServerOTAEndpoint = ZCL_GetEndPointForSpecificCluster(pIndication->aClusterId, TRUE);
    /* Handle the commands */
    switch(pFrame->command)
    {
      case gZclCmdOTA_QueryNextImageRequest_c:  
        status = ZCL_OTAClusterServer_NextImageRequest_Handler(pIndication);   
         break;
  
      case gZclCmdOTA_ImageBlockRequest_c:
        if(mServerUtility.blockResponseInProgress == FALSE){
          mServerUtility.blockResponseInProgress = TRUE;
          FLib_MemCpy(&blockRequest, (pFrame+1), sizeof(zclOTABlockRequest_t));
    	  pZtcBlockResponse = MSG_Alloc(sizeof(zclZtcOTABlockResponse_t)+ blockRequest.maxDataSize); 
    	  if(!pZtcBlockResponse)
    	  {
            mServerUtility.blockResponseInProgress = FALSE; 
            return gZclNoMem_c;	
    	  }
    	  Copy2Bytes(&pZtcBlockResponse->aNwkAddr, &pIndication->aSrcAddr);
    	  pZtcBlockResponse->endPoint = pIndication->srcEndPoint;
          if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
          {      
#if gZclOtaExternalMemorySupported_d           
          if(mServerUtility.imageLoadingState == gOtaServerLoadingImageStart_c)
            pZtcBlockResponse->zclOTABlockResponse.status = gZclOTAAbort_c;
          else
            pZtcBlockResponse->zclOTABlockResponse.status = (mServerUtility.imageLoadingState == gOtaServerLoadingImageProcess_c)?gZclOTAWaitForData_c:gZclOTASuccess_c;   
 #endif
          }
          else
            pZtcBlockResponse->zclOTABlockResponse.status = gZclOTASuccess_c;
          pZtcBlockResponse->zclOTABlockResponse.msgData.success.fileOffset = blockRequest.fileOffset;  		
    	  pZtcBlockResponse->zclOTABlockResponse.msgData.success.dataSize = blockRequest.maxDataSize;
          pZtcBlockResponse->zclOTABlockResponse.msgData.success.manufacturerCode = blockRequest.manufacturerCode;
          pZtcBlockResponse->zclOTABlockResponse.msgData.success.imageType = blockRequest.imageType;
          pZtcBlockResponse->zclOTABlockResponse.msgData.success.fileVersion = blockRequest.fileVersion;
          if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
          {
#if gZclOtaExternalMemorySupported_d             
    	    status = ZCL_OTABlockResponse(pZtcBlockResponse);
            mServerUtility.blockResponseInProgress = FALSE;
            MSG_Free(pZtcBlockResponse);
            pZtcBlockResponse = NULL;
#endif  /* gZclOtaExternalMemorySupported_d  */          
          }
          else
          {         
            if(mServerUtility.otaServerIsActive == TRUE)
            {
              ztcOtaServerChunkRequest_t chunkRequest;
              Copy2Bytes(&chunkRequest.deviceID, &pIndication->aSrcAddr);
              chunkRequest.currentOffset = blockRequest.fileOffset; 
              /* Host App control the remaining size of image */
              chunkRequest.blockSize = blockRequest.maxDataSize; 
#ifndef gHostApp_d  
              ZTCQueue_QueueToTestClient((const uint8_t*)&chunkRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportImageChunkReq_c, sizeof(ztcOtaServerChunkRequest_t));
#else
              ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&chunkRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportImageChunkReq_c, sizeof(ztcOtaServerChunkRequest_t));
#endif        
              status = gZclSuccess_c; 
            }
            else
            {
              pZtcBlockResponse->zclOTABlockResponse.status = gZclOTAAbort_c;
              status = ZCL_OTABlockResponse(pZtcBlockResponse);
              mServerUtility.blockResponseInProgress = FALSE;
              MSG_Free(pZtcBlockResponse);
              pZtcBlockResponse = NULL;
            }          
          }  
        }
        else
        {
          zclZtcOTABlockResponse_t  *pZtcBlockResponseWaitForData;
          if((pZtcBlockResponse->aNwkAddr[0] == pIndication->aSrcAddr[0])&&
             (pZtcBlockResponse->aNwkAddr[1] == pIndication->aSrcAddr[1]))
               return gZclSuccess_c;
          pZtcBlockResponseWaitForData = MSG_Alloc(sizeof(zclZtcOTABlockResponse_t)); 
    	  if(!pZtcBlockResponseWaitForData)
    	  {
    		 mServerUtility.blockResponseInProgress = FALSE; 
    		 return gZclNoMem_c;	
    	  }
          Copy2Bytes(&pZtcBlockResponseWaitForData->aNwkAddr, &pIndication->aSrcAddr);
    	  pZtcBlockResponseWaitForData->endPoint = pIndication->srcEndPoint;
          pZtcBlockResponseWaitForData->zclOTABlockResponse.status = gZclOTAWaitForData_c;
          status = ZCL_OTABlockResponse(pZtcBlockResponseWaitForData);
          MSG_Free(pZtcBlockResponseWaitForData);
        }
        break;
    case gZclCmdOTA_UpgradeEndRequest_c:
        FLib_MemCpy(&upgradeEndRequest, (pFrame+1), sizeof(zclOTAUpgradeEndRequest_t));
        if(upgradeEndRequest.status == gZclOTASuccess_c)
        {          
          Copy2Bytes(&upgradeEndResponse.aNwkAddr, &pIndication->aSrcAddr);
          upgradeEndResponse.endPoint = pIndication->srcEndPoint;
          upgradeEndResponse.zclOTAUpgradeEndResponse.manufacturerCode = upgradeEndRequest.manufacturerCode;
          upgradeEndResponse.zclOTAUpgradeEndResponse.imageType = upgradeEndRequest.imageType;
          upgradeEndResponse.zclOTAUpgradeEndResponse.fileVersion = upgradeEndRequest.fileVersion;
          upgradeEndResponse.zclOTAUpgradeEndResponse.currentTime = Native2OTA32(mOtaServerInitParams.currentTime);
          upgradeEndResponse.zclOTAUpgradeEndResponse.upgradeTime = Native2OTA32(mOtaServerInitParams.upgradeTime);                          
          status = ZCL_OTAUpgradeEndResponse(&upgradeEndResponse);          
        }
        else{
        	status = gZclSuccessDefaultRsp_c;
        }
        if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
        {   
#if (gZclEnableOTAProgressReportToExternalApp_d == TRUE)    
            zclZtcImageOtaProgressReport_t ztcOtaProgressReportInf;
            /*send a report message with status TRUE or FALSE: imageLength field 
            *(TRUE - image successfully transmitted to client , FALSE - otherwise)*/
            ztcOtaProgressReportInf.currentOffset = 0xFFFFFFFF;
            if(status == gZclSuccess_c)
              ztcOtaProgressReportInf.imageLength = Native2OTA32(TRUE);
            else
              ztcOtaProgressReportInf.imageLength = Native2OTA32(FALSE);
            Copy2Bytes(&ztcOtaProgressReportInf.deviceAddr,&pIndication->aSrcAddr); 
#ifndef gHostApp_d  
            ZTCQueue_QueueToTestClient((const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#else
            ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#endif 
#endif    
        }
        break;
        /* command not supported on this cluster */
    default:
        return gZclUnsupportedClusterCommand_c;
    }
    return status;
}

/******************************************************************************
* Request to send the image notify command
*
* Usually called by the ZTC/host
******************************************************************************/

zbStatus_t ZCL_OTAImageNotify(zclZtcOTAImageNotify_t* pZtcImageNotifyParams)
{
  afToApsdeMessage_t *pMsg;
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionNone_c, 1};
  zclFrameControl_t frameControl;
  uint8_t len;  
  
  /* Create the destination address */
  Copy2Bytes(&addrInfo.dstAddr,&pZtcImageNotifyParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcImageNotifyParams->endPoint;
  addrInfo.srcEndPoint = ZCL_GetEndPointForSpecificCluster(addrInfo.aClusterId, TRUE);
 
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  /* Initialize the lenght of the command based on the payload type.*/
  if(pZtcImageNotifyParams->zclOTAImageNotify.payloadType < gPayloadMax_c)
  {
    len = imgNotifySizeArray[pZtcImageNotifyParams->zclOTAImageNotify.payloadType];
  }
  else
  {
    return gZclFailure_c;
  }
  if(!IsValidNwkUnicastAddr(addrInfo.dstAddr.aNwkAddr))
  {
	
    frameControl = gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp;
  }
  else
  {
	addrInfo.txOptions |=  gApsTxOptionSecEnabled_c;
    frameControl = gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp;
  }

  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_ImageNotify_c,
                          frameControl, 
                          &gZclTransactionId, 
                          &len,
                          &pZtcImageNotifyParams->zclOTAImageNotify);
  gZclTransactionId++;
  if(!pMsg)
    return gZclNoMem_c;
 return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  
}


/******************************************************************************
* ZCL_OTAClusterServer_NextImageRequest_Handler
*
* Sends back a next image response 
******************************************************************************/  

zbStatus_t ZCL_OTAClusterServer_NextImageRequest_Handler
(
  zbApsdeDataIndication_t *pIndication  
)
{
   zclFrame_t *pFrame;
   zbStatus_t status = gZclSuccess_c;  
   zclOTANextImageRequest_t   ztcNextImageReq;
   /*ZCL frame*/
   pFrame = (zclFrame_t*)pIndication->pAsdu;
   FLib_MemCpy(&ztcNextImageReq, (pFrame+1), sizeof(zclOTANextImageRequest_t));
   ztcNextImageReq.manufacturerCode = OTA2Native16(ztcNextImageReq.manufacturerCode);
   ztcNextImageReq.imageType = OTA2Native16(ztcNextImageReq.imageType);
   ztcNextImageReq.fileVersion = OTA2Native32(ztcNextImageReq.fileVersion);
   if(mServerUtility.operationMode == gDoNotUseExternalMemoryForOtaUpdate_c)
   {   
     if(mServerUtility.otaServerIsActive)
     {
        ztcOtaServerQueryImageRequest_t queryRequest;

        Copy2Bytes(&queryRequest.deviceID, &pIndication->aSrcAddr);
        queryRequest.manufacturerCode = Native2OTA16(ztcNextImageReq.manufacturerCode);
        queryRequest.imageType = Native2OTA16(ztcNextImageReq.imageType);
        queryRequest.fileVersion = Native2OTA32(ztcNextImageReq.fileVersion);
        /* wait the host application to confirm (see OTAServerQueryImageRsp) */
#ifndef gHostApp_d  
        ZTCQueue_QueueToTestClient((const uint8_t*)&queryRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportQueryImageReq_c, sizeof(ztcOtaServerQueryImageRequest_t));
#else
        ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&queryRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportQueryImageReq_c, sizeof(ztcOtaServerQueryImageRequest_t));
#endif 
        ZCL_OTAClusterServer_AddToTemporaryQueryReqList(pIndication->aSrcAddr, pIndication->srcEndPoint);
     }
      else  
      {
          zclZtcOTANextImageResponse_t  ztcNextImageRsp;
          ztcNextImageRsp.zclOTANextImageResponse.status = gZclOTANoImageAvailable_c;
          Copy2Bytes(&ztcNextImageRsp.aNwkAddr,&pIndication->aSrcAddr);
          ztcNextImageRsp.endPoint = pIndication->srcEndPoint;
          ztcNextImageRsp.zclOTANextImageResponse.manufacturerCode = Native2OTA16(ztcNextImageReq.manufacturerCode);
          ztcNextImageRsp.zclOTANextImageResponse.imageType = Native2OTA16(ztcNextImageReq.imageType);
          status = ZCL_OTANextImageResponse(&ztcNextImageRsp);
      }    
   }
   else
   {
      zclZtcOTANextImageResponse_t  ztcNextImageRsp;
#if gZclOtaExternalMemorySupported_d       
      uint8_t index = OTAServerSearchForImageRequest(ztcNextImageReq.manufacturerCode,ztcNextImageReq.imageType, ztcNextImageReq.fileVersion, FALSE); 
      if(index == gMaxNumberOfOTAImage_c)
         ztcNextImageRsp.zclOTANextImageResponse.status = gZclOTANoImageAvailable_c;
      else
      {
        ztcNextImageRsp.zclOTANextImageResponse.status = gZclSuccess_c;
        ztcNextImageRsp.zclOTANextImageResponse.fileVersion = Native2OTA32(listOfImage[index].fileVersion);
        ztcNextImageRsp.zclOTANextImageResponse.imageSize = Native2OTA32(listOfImage[index].totalImageSize);  
      }      
      ztcNextImageRsp.zclOTANextImageResponse.manufacturerCode = Native2OTA16(listOfImage[index].manufacturerCode);
      ztcNextImageRsp.zclOTANextImageResponse.imageType = Native2OTA16(listOfImage[index].imageType);
#else
      ztcNextImageRsp.zclOTANextImageResponse.status = gZclOTANoImageAvailable_c;      
#endif   
      Copy2Bytes(&ztcNextImageRsp.aNwkAddr,&pIndication->aSrcAddr);
      ztcNextImageRsp.endPoint = pIndication->srcEndPoint;    
      status = ZCL_OTANextImageResponse(&ztcNextImageRsp);     
   }
   return status;
}

/******************************************************************************
* Request to send the image notify command
*
* Incoming OTA cluster frame processing loop for cluster client side
******************************************************************************/
zbStatus_t ZCL_OTANextImageResponse(zclZtcOTANextImageResponse_t* pZtcNextImageResponseParams)
{
  afToApsdeMessage_t *pMsg;
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif  
  uint8_t len = 0;  
  
  switch(pZtcNextImageResponseParams->zclOTANextImageResponse.status)
    {
  	  case gZclOTASuccess_c:
  	    len = sizeof(zclOTANextImageResponse_t);
  	    break;
  	  case gZclOTANoImageAvailable_c:
  	  case gZclOTANotAuthorized_c:	  
  	    len = sizeof(zclOTAStatus_t);
  	    break;
  	  default:
  	    /* Invalid status */
  	    return gZclMalformedCommand_c;
    }
  
  /* Create the destination address */
  Copy2Bytes(&addrInfo.dstAddr,&pZtcNextImageResponseParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcNextImageResponseParams->endPoint;
  
  addrInfo.srcEndPoint = gServerOTAEndpoint;

  addrInfo.radiusCounter = afDefaultRadius_c;
  
  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_QueryNextImageResponse_c,
                          gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp, 
                          &(gZclTransactionOtaId), 
                          &len,
                          &pZtcNextImageResponseParams->zclOTANextImageResponse);
  if(!pMsg)
    return gZclNoMem_c;
  return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  
}

/******************************************************************************
* ZCL_OTABlockResponse
*
* Send a data block to the client
******************************************************************************/
zbStatus_t ZCL_OTABlockResponse(zclZtcOTABlockResponse_t *pZtcBlockResponseParams)
{
 uint8_t len = 0; 
 afToApsdeMessage_t *pMsg;
 
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif   
  
  if(OTAServer_CheckAbortBlockRspList(pZtcBlockResponseParams->aNwkAddr))
    pZtcBlockResponseParams->zclOTABlockResponse.status = gZclOTAAbort_c;

  if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
  {
#if(gZclOtaExternalMemorySupported_d)
    uint32_t addr;
    uint32_t offsetImage = 0;

    uint16_t manufacturerCode = OTA2Native16(pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.manufacturerCode);
    uint16_t imageType = OTA2Native16(pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.imageType);
    uint32_t fileVersion = OTA2Native32(pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.fileVersion);
    
    if(pZtcBlockResponseParams->zclOTABlockResponse.status == gZclOTASuccess_c)
    {
      uint8_t index = OTAServerSearchForImageRequest(manufacturerCode,imageType,fileVersion, TRUE); 
      if(index == gMaxNumberOfOTAImage_c)
    	 return gZclOTANoImageAvailable_c; 
      else
      {
        offsetImage = OTA2Native32(pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.fileOffset);
        len = pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.dataSize;
      
        if(listOfImage[index].totalImageSize - offsetImage < len)
			len =listOfImage[index].totalImageSize - offsetImage;
        addr = offsetImage + listOfImage[index].addressOfCurrentImage;
        if (OTA_ReadExternalMemory(&pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.data[0], len, addr) != gOtaSucess_c )
	 	     pZtcBlockResponseParams->zclOTABlockResponse.status = gZclOTAAbort_c; 
        pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.dataSize = len;	

#if (gZclEnableOTAProgressReportToExternalApp_d == TRUE)    
        {
          zclZtcImageOtaProgressReport_t ztcOtaProgressReportInf;
          if((offsetImage == 0)||((offsetImage+len)%(len*100) == 0)||(offsetImage+len == listOfImage[index].totalImageSize))
          {
            ztcOtaProgressReportInf.currentOffset = Native2OTA32(offsetImage+len);
            ztcOtaProgressReportInf.imageLength =  Native2OTA32(listOfImage[index].totalImageSize);
            Copy2Bytes(&ztcOtaProgressReportInf.deviceAddr,&pZtcBlockResponseParams->aNwkAddr); 
#ifndef gHostApp_d  
            ZTCQueue_QueueToTestClient((const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#else
            ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#endif 
          }
        }
#endif  /* gZclEnableOTAProgressReportToExternApp_d  */    
      }
    }
#endif /* gZclOtaExternalMemorySupported_d */
  }
  /* The lenght of the frame is status dependant */
  switch(pZtcBlockResponseParams->zclOTABlockResponse.status)
  {
	  case gZclOTASuccess_c:
	    len = sizeof(zclOTABlockResponse_t) + pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.dataSize - 1;
	    break;
	  case gZclOTAAbort_c:
	    len = sizeof(zclOTAStatus_t);
	    break;
	  case gZclOTAWaitForData_c:
            {
              pZtcBlockResponseParams->zclOTABlockResponse.msgData.wait.currentTime = Native2OTA32(mOtaServerInitParams.currentTime); 
              pZtcBlockResponseParams->zclOTABlockResponse.msgData.wait.requestTime = Native2OTA32(mOtaServerInitParams.upgradeRequestTime);     /* seconds */
	      pZtcBlockResponseParams->zclOTABlockResponse.msgData.wait.blockRequestDelay = Native2OTA16(mOtaServerInitParams.blockRequestDelay); /* Miliseconds*/
              len = sizeof(zclOTABlockResponseStatusWait_t) + 1;
            }
	    break;
	  default:
	    /* Invalid status */
	    return gZclMalformedCommand_c;
  }
  /* Create the destination address */
  Copy2Bytes(&addrInfo.dstAddr,&pZtcBlockResponseParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcBlockResponseParams->endPoint;
  addrInfo.srcEndPoint = gServerOTAEndpoint;
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  pMsg = ZCL_CreateFrame( &addrInfo, 
	                      gZclCmdOTA_ImageBlockResponse_c,
	                      gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp, 
	                      &(gZclTransactionOtaId), 
	                      &len,
	                      &pZtcBlockResponseParams->zclOTABlockResponse);
  if(!pMsg)
	   return gZclNoMem_c;
  return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);    
}

/******************************************************************************
* Request to send upgrade end response command
*
* Send the response to client's upgrade end request
******************************************************************************/
zbStatus_t ZCL_OTAUpgradeEndResponse(ztcZclOTAUpgdareEndResponse_t* pZtcOTAUpgdareEndResponse)
{
  afToApsdeMessage_t *pMsg;
  uint8_t len = sizeof(zclOTAUpgradeEndResponse_t);  
  
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif  
  
  /* Create the destination address */
  Copy2Bytes(&addrInfo.dstAddr,&pZtcOTAUpgdareEndResponse->aNwkAddr);
  addrInfo.dstEndPoint = pZtcOTAUpgdareEndResponse->endPoint;
  addrInfo.srcEndPoint = gServerOTAEndpoint;
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_UpgradeEndResponse_c,
                          gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp, 
                          &(gZclTransactionOtaId), 
                          &len,
                          &pZtcOTAUpgdareEndResponse->zclOTAUpgradeEndResponse);
  if(!pMsg)
    return gZclNoMem_c;
  return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  
}

/******************************************************************************
* OTA Server - serial protocol functions
******************************************************************************/

/******************************************************************************
* OTAServerStartImageProcess
*
* start an OTA Server process
******************************************************************************/
zbStatus_t OTAServerStartImageProcess(uint8_t operationMode)
{
   zbClusterId_t clusterId = {gaZclClusterOTA_c};
   
   mServerUtility.operationMode = operationMode;
   gServerOTAEndpoint = ZCL_GetEndPointForSpecificCluster(clusterId, TRUE);
  
   if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
   {
#if (gZclOtaExternalMemorySupported_d)  
      if(mServerUtility.imageLoadingState == gOtaServerLoadingImageProcess_c)
       return gOtaInvalidParam_c;
      mServerUtility.currentAddress = 0x00;
      mIndexOfCurrentImage = 0;
      /* Init external memory */
      OTA_InitExternalMemory();
      
#if !(gTargetTWR_KW24D512_d || gTargetTWR_KW22D512_d || gTargetTWR_KW21D256_d || gTargetTWR_KW21D512_d)  
      /* Erase external memory */
      if(gOtaSucess_c != OTA_EraseExternalMemory())
        return gOtaExternalFlashError_c; 
#endif      
      /*send a query request */
      gRequestBlockTimer = TMR_AllocateTimer(); 
      if(gRequestBlockTimer == gTmrInvalidTimerID_c)
        return gZclNoMem_c;
#if gZclOtaServerImgIntegrityCodeValidation_d      
      FLib_MemSet(&mMsgDigestServer[0], 0x00, AES_MMO_SIZE);
#endif   
      
      TMR_StartTimer(gRequestBlockTimer, gTmrSingleShotTimer_c,
                         BLOCK_PROCESSING_CALLBACK_DELAY, OTAServerQueryImageRequestTmrCallback);
      return gOtaSucess_c;      
#else      
      return gOtaInvalidParam_c; 
#endif      
   }   
   else
   {
      if(mServerUtility.operationMode == gOtaTestingProcess)
      {
	/* switch the operation mode */
	 mServerUtility.operationMode = (mServerUtility.operationMode==gUseExternalMemoryForOtaUpdate_c)?gDoNotUseExternalMemoryForOtaUpdate_c:gUseExternalMemoryForOtaUpdate_c;			 
      }
      if(mServerUtility.operationMode == gDoNotUseExternalMemoryForOtaUpdate_c)
      {
	 uint8_t i;
	 for(i=0; i< zclOTAServer_MaxTemporaryReqList_d; i++)
	 {
            mTemporaryQueryReqList[i].endPoint = 0xFF;
            Set2Bytes(mTemporaryQueryReqList[i].aNwkAddr, 0xFFFF);
	 }   
         mServerUtility.otaServerIsActive = TRUE;
      }
   }
   return gOtaSucess_c;
}


/**********************************************************************
* OTAServerQueryImageRequestTmrCallback
*
* this callback is used only if the server has no information about the image
***********************************************************************/
static void OTAServerQueryImageRequestTmrCallback(uint8_t tmr) 
{
    ztcOtaServerQueryImageRequest_t queryRequest;
    (void)tmr;
    queryRequest.deviceID[0] = 0xFF;
    queryRequest.deviceID[1] = 0xFF;
    queryRequest.manufacturerCode = 0xFFFF;
    queryRequest.imageType = 0xFFFF;
    queryRequest.fileVersion = 0xFFFFFFFF;
    TMR_FreeTimer(gRequestBlockTimer);
#ifndef gHostApp_d  
    ZTCQueue_QueueToTestClient((const uint8_t*)&queryRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportQueryImageReq_c, sizeof(ztcOtaServerQueryImageRequest_t));
#else
    ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&queryRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportQueryImageReq_c, sizeof(ztcOtaServerQueryImageRequest_t));
#endif 
}

/******************************************************************************
* OTAServerQueryImageRsp
*
* receive Image information from an external application
******************************************************************************/
zbStatus_t OTAServerQueryImageRsp(uint8_t *queryImageRsp)
{
   ztcOtaServerQueryImageRsp_t queryResponse;
   
   FLib_MemCpy(&queryResponse, &queryImageRsp[0], sizeof(ztcOtaServerQueryImageRsp_t));  
   if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
   {
#if (gZclOtaExternalMemorySupported_d)   
      if(queryResponse.imageStatus != gOtaSucess_c) 
        return gZclFailure_c;
      if(mIndexOfCurrentImage >= gMaxNumberOfOTAImage_c) 
        return gZclFailure_c;
      queryResponse.totalImageSize = OTA2Native32(queryResponse.totalImageSize);
      /* test the posibility to store the image */
      if ((queryResponse.totalImageSize == 0) || 
            (queryResponse.totalImageSize + mServerUtility.currentAddress > gExtFlash_TotalSize_c)) 
        return gZclFailure_c;
      /* store server information */
      listOfImage[mIndexOfCurrentImage].imageType = OTA2Native16(queryResponse.imageType);
      listOfImage[mIndexOfCurrentImage].totalImageSize = queryResponse.totalImageSize;
      listOfImage[mIndexOfCurrentImage].manufacturerCode = OTA2Native16(queryResponse.manufacturerCode);
      listOfImage[mIndexOfCurrentImage].fileVersion = OTA2Native32(queryResponse.fileVersion); 
      listOfImage[mIndexOfCurrentImage].addressOfCurrentImage =  mServerUtility.currentAddress;
      /*send a first block request */
      mServerUtility.imageLoadingState = gOtaServerLoadingImageStart_c;
      gRequestBlockTimer = TMR_AllocateTimer(); 
      if(gRequestBlockTimer == gTmrInvalidTimerID_c)
        return gZclNoMem_c;
      TMR_StartTimer(gRequestBlockTimer, gTmrSingleShotTimer_c,
                         BLOCK_PROCESSING_CALLBACK_DELAY, OTAServerBlockRequestTmrCallback);
#endif  
   }
   else  /* gNotUseExternalMemoryForOtaUpdate_c */
   {
     /* send a query next image response over the air */
     zclZtcOTANextImageResponse_t  ztcNextImageRsp;
     Copy2Bytes(&ztcNextImageRsp.aNwkAddr, &queryResponse.deviceID);
     ztcNextImageRsp.endPoint = ZCL_OTAClusterServer_GetEndpointFromTemporaryQueryList(queryResponse.deviceID);
     ztcNextImageRsp.zclOTANextImageResponse.status = queryResponse.imageStatus;
     ztcNextImageRsp.zclOTANextImageResponse.manufacturerCode = queryResponse.manufacturerCode;
     ztcNextImageRsp.zclOTANextImageResponse.imageType = queryResponse.imageType;
     ztcNextImageRsp.zclOTANextImageResponse.fileVersion = queryResponse.fileVersion;
     ztcNextImageRsp.zclOTANextImageResponse.imageSize = queryResponse.totalImageSize;   
     return ZCL_OTANextImageResponse(&ztcNextImageRsp);
   }  
    return gOtaSucess_c;
}

/****************************************************************************
 * newImageReceived
 *   
 * called for sending Image Notify command over the air,
 *      when a new image is available
 ****************************************************************************/
zbStatus_t newImageReceived(ztcOtaServerImageNotify_t imgNotify)
{
    zclZtcOTAImageNotify_t imageNotify;
    
    /* send a broadcast or unicast image notify */
    Copy2Bytes(&imageNotify.aNwkAddr, &imgNotify.deviceID);
    imageNotify.endPoint = 0xFF;
 
    /* see 6.10.3- ImageNotify Command - Ota cluster specification (r17)*/
    if(IsValidNwkUnicastAddr(imageNotify.aNwkAddr) == FALSE)
      imageNotify.zclOTAImageNotify.payloadType = gJitter_c;
    else
      imageNotify.zclOTAImageNotify.payloadType = gJitterMfgImageFile_c;
    imageNotify.zclOTAImageNotify.queryJitter = OTA_CLIENT_MAX_RANDOM_JITTER;
    /*inform clients:  new image(s) are available*/  
    imageNotify.zclOTAImageNotify.manufacturerCode = imgNotify.manufacturerCode;
    imageNotify.zclOTAImageNotify.imageType = imgNotify.imageType;   
    imageNotify.zclOTAImageNotify.fileVersion = imgNotify.fileVersion; 
    return ZCL_OTAImageNotify(&imageNotify);      
}

/******************************************************************************
* OTAServerImageNotify
*
*  - if operationMode = gUseExternalMemoryForOtaUpdate_c  -  test the image parameters, If no match start 
*                                                           download it;
*                     = gNotUseExternalMemoryForOtaUpdate_c - send the ImageNotify command over the air;                  
******************************************************************************/
zbStatus_t OTAServerImageNotify(uint8_t *imgNotify)
{
  ztcOtaServerImageNotify_t imageNotify;  
  FLib_MemCpy(&imageNotify, &imgNotify[0], sizeof(ztcOtaServerImageNotify_t));
  imageNotify.manufacturerCode = OTA2Native16(imageNotify.manufacturerCode);
  imageNotify.imageType = OTA2Native16(imageNotify.imageType);
  imageNotify.fileVersion = OTA2Native32(imageNotify.fileVersion);
  if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
  {
#if (gZclOtaExternalMemorySupported_d)      
    uint8_t index = OTAServerSearchForImageRequest(imageNotify.manufacturerCode, imageNotify.imageType, imageNotify.fileVersion, TRUE);
    if(index == gMaxNumberOfOTAImage_c)
    {
      /* start downloading process */
      gRequestBlockTimer = TMR_AllocateTimer();
      if(gRequestBlockTimer == gTmrInvalidTimerID_c)
        return gZclNoMem_c;
      TMR_StartTimer(gRequestBlockTimer, gTmrSingleShotTimer_c,
                         BLOCK_PROCESSING_CALLBACK_DELAY, OTAServerQueryImageRequestTmrCallback);
      return gZclSuccess_c;
    }
    else
      return gZclFailure_c;
#endif    
  }
  return newImageReceived(imageNotify);
}

#if (gZclOtaExternalMemorySupported_d) 
/******************************************************************************
* OTAServerSearchForImageRequest  
*
* return index of required image in the image list,  otherwise return gMaxNumberOfOTAImage
******************************************************************************/
static uint8_t OTAServerSearchForImageRequest(uint16_t manufacturerCode, uint16_t imageType, uint32_t fileVersion, bool_t testFileVersion)
{
  uint8_t i;
  
   if( mServerUtility.imageLoadingState != gOtaServerLoadingImageComplete_c)
     return gMaxNumberOfOTAImage_c;
  
   /*test if the image exist in the list*/
   for(i=0;i<gMaxNumberOfOTAImage_c;i++)
     if(((imageType == listOfImage[i].imageType)||(listOfImage[i].imageType == OTA_IMAGE_TYPE_MATCH_ALL))&&
    	 ((manufacturerCode == listOfImage[i].manufacturerCode)||(listOfImage[i].manufacturerCode == OTA_MANUFACTURER_CODE_MATCH_ALL)))
     {
    	 if(testFileVersion == TRUE)
    	 {
            if((fileVersion == listOfImage[i].fileVersion)||(listOfImage[i].fileVersion==OTA_FILE_VERSION_MATCH_ALL))
    		return i;
    	 }
    	 else
         {
            if(((mServerUtility.localFileVersionPolicies & OTA_FILE_VERSION_SUPPORTED_UPGRADE)&& (fileVersion < listOfImage[i].fileVersion))||
                ((mServerUtility.localFileVersionPolicies & OTA_FILE_VERSION_SUPPORTED_REINSTALL)&& (fileVersion == listOfImage[i].fileVersion))||
                 ((mServerUtility.localFileVersionPolicies & OTA_FILE_VERSION_SUPPORTED_DOWNGRADE)&& (fileVersion > listOfImage[i].fileVersion)))
                  return i;
         }
     }
      
   return ReadAndTestOtaHeader(manufacturerCode, imageType, fileVersion, testFileVersion); 
}

/******************************************************************************
* ReadAndTestOtaHeader  
*
* read the ota header and compare part of it with image request
******************************************************************************/
static uint8_t ReadAndTestOtaHeader(uint16_t manufacturerCode, uint16_t imageType, uint32_t fileVersion, bool_t testFileVersion) 
{
    uint8_t buffer[sizeof(zclOTAFileHeader_t)]; 
    uint8_t *pData;
    zclOTAFileHeader_t otaHeader; //to store the ota header information
    uint32_t currentAddress = 0x00; 
    uint8_t index = 0; //index in the image list
    /* Initialize External Memory */
    OTA_InitExternalMemory();
    pData = &buffer[0];
    while((currentAddress < gExtFlash_TotalSize_c)&&(index < gMaxNumberOfOTAImage_c))
    {
        // Read Header
        (void)OTA_ReadExternalMemory(pData, sizeof(zclOTAFileHeader_t), currentAddress);
        FLib_MemCpy(&otaHeader, &pData[0], sizeof(zclOTAFileHeader_t));
        otaHeader.fileIdentifier = OTA2Native32( otaHeader.fileIdentifier);
        if(otaHeader.fileIdentifier != OTA_FILE_IDENTIFIER)
          return gMaxNumberOfOTAImage_c;
        listOfImage[index].addressOfCurrentImage  = currentAddress;
        listOfImage[index].manufacturerCode = OTA2Native16(otaHeader.manufacturerCode);
        listOfImage[index].imageType = OTA2Native16(otaHeader.imageType);
        listOfImage[index].fileVersion = OTA2Native32(otaHeader.fileVersion);
        listOfImage[index].totalImageSize = OTA2Native32(otaHeader.totalImageSize);
       
        if(((imageType == listOfImage[index].imageType)||(listOfImage[index].imageType == OTA_IMAGE_TYPE_MATCH_ALL))&&
    	 ((manufacturerCode == listOfImage[index].manufacturerCode)||(listOfImage[index].manufacturerCode == OTA_MANUFACTURER_CODE_MATCH_ALL)))

        {
          	 if(testFileVersion == TRUE)
          	 {
                    if((fileVersion == listOfImage[index].fileVersion)||(listOfImage[index].fileVersion==OTA_FILE_VERSION_MATCH_ALL))
                    {
                      mServerUtility.imageLoadingState = gOtaServerLoadingImageComplete_c;
                      return index;
                    }
                    else
                    {
                      currentAddress+= listOfImage[index].totalImageSize;
                      index++;
                    }
           	 }
           	 else
           	 {
                    if(((mServerUtility.localFileVersionPolicies & OTA_FILE_VERSION_SUPPORTED_UPGRADE)&& (fileVersion < listOfImage[index].fileVersion))||
                        ((mServerUtility.localFileVersionPolicies & OTA_FILE_VERSION_SUPPORTED_REINSTALL)&& (fileVersion == listOfImage[index].fileVersion))||
                          ((mServerUtility.localFileVersionPolicies & OTA_FILE_VERSION_SUPPORTED_DOWNGRADE)&& (fileVersion > listOfImage[index].fileVersion)))
                    {
                      mServerUtility.imageLoadingState = gOtaServerLoadingImageComplete_c;
                      return index;
                    }
                    else
                    {
                      currentAddress+= listOfImage[index].totalImageSize;
                      index++;
                    }
           	 }
        }
        else
        {
        	currentAddress+= listOfImage[index].totalImageSize;
        	index++;
        }
    }
   mServerUtility.imageLoadingState = gOtaServerLoadingImageComplete_c;
   return gMaxNumberOfOTAImage_c;
}


/**********************************************************************
* OTAServerBlockRequestTmrCallback
*
* request an Image block(if the device contain external memory)
***********************************************************************/
static void OTAServerBlockRequestTmrCallback(uint8_t tmr) 
{
  ztcOtaServerChunkRequest_t chunkRequest;
    (void)tmr;  
    Copy2Bytes(chunkRequest.deviceID, NlmeGetRequest(gNwkShortAddress_c));
    if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
    {	
      chunkRequest.currentOffset = (mIndexOfCurrentImage ==0)?Native2OTA32(mServerUtility.currentAddress):Native2OTA32(mServerUtility.currentAddress - listOfImage[mIndexOfCurrentImage-1].totalImageSize);
      /* block size = 200 or rest of image data*/
      chunkRequest.blockSize = (listOfImage[mIndexOfCurrentImage].totalImageSize -
                     mServerUtility.currentAddress + listOfImage[mIndexOfCurrentImage].addressOfCurrentImage > 200)?
                     200:(uint8_t)(listOfImage[mIndexOfCurrentImage].totalImageSize - mServerUtility.currentAddress +
                     listOfImage[mIndexOfCurrentImage].addressOfCurrentImage);  
#ifndef gHostApp_d  
    ZTCQueue_QueueToTestClient((const uint8_t*)&chunkRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportImageChunkReq_c, sizeof(ztcOtaServerChunkRequest_t));
#else
    ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&chunkRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportImageChunkReq_c, sizeof(ztcOtaServerChunkRequest_t));
#endif 
    }
}

#endif

/****************************************************************************
 *  ZtcOtaServerBlockReceive
 *
 *  called every time a new image block is received.
 ****************************************************************************/
zbStatus_t ZtcOtaServerBlockReceive(uint8_t *dataBlock, uint8_t length)
{
  zbStatus_t status = gOtaSucess_c;
  
  /* Validate parameters */
  if((length == 0) || (dataBlock == NULL)) 
  {
    return gOtaInvalidParam_c;
  }
  
  if(mServerUtility.operationMode == gDoNotUseExternalMemoryForOtaUpdate_c) 
  {
    /*if ExternalMemory = FALSE - complete the image block response*/
    FLib_MemCpy(&pZtcBlockResponse->zclOTABlockResponse.msgData.success.data[0], dataBlock, length);
    pZtcBlockResponse->zclOTABlockResponse.msgData.success.dataSize = length;
    status = ZCL_OTABlockResponse(pZtcBlockResponse);
    mServerUtility.blockResponseInProgress = FALSE;
    MSG_Free(pZtcBlockResponse);   
  }
  else
  { 
    /*operation mode: gUseExternalMemoryForOtaUpdate_c*/
#if (gZclOtaExternalMemorySupported_d)   
    
#if gZclOtaServerImgIntegrityCodeValidation_d      
    /* compute the Hash to check the ImageIntegrity */
    OTAClusterProcessAES_MMOHash(dataBlock, length);
#endif     
    /*Write image to external memory*/
    status = OTA_WriteExternalMemory(dataBlock, length, mServerUtility.currentAddress);
    if(status == gOtaSucess_c)
    { 
      mServerUtility.currentAddress+= length;
      /* send a request for next block */
      if(mServerUtility.currentAddress - listOfImage[mIndexOfCurrentImage].addressOfCurrentImage < listOfImage[mIndexOfCurrentImage].totalImageSize)
        TMR_StartTimer(gRequestBlockTimer, gTmrSingleShotTimer_c,
                          BLOCK_PROCESSING_CALLBACK_DELAY, OTAServerBlockRequestTmrCallback); 
      else  
      { 
        ztcOtaServerImageNotify_t imgNotify = {0xFF, 0xFF, 0, 0, 0};
#if gZclOtaServerImgIntegrityCodeValidation_d    
        zclOTaImageIntegritySubElement_t  subElemInf; 
        uint8_t buffer[sizeof(zclOTaImageIntegritySubElement_t)];
        uint8_t *pData;
        pData = &buffer[0];
        (void)OTA_ReadExternalMemory(pData, sizeof(zclOTaImageIntegritySubElement_t),  mServerUtility.currentAddress - sizeof(zclOTaImageIntegritySubElement_t));  
        FLib_MemCpy(&subElemInf, &pData[0], sizeof(zclOTaImageIntegritySubElement_t));
        TMR_FreeTimer(gRequestBlockTimer);    
   
        /*check the image integrity Code */
        if(subElemInf.id == gZclOtaImageIntegrityCodeTagId && 
           subElemInf.length == gImageIntegrityCodeLengthField)
        {
          if(!FLib_MemCmp(mMsgDigestServer, subElemInf.hash, AES_MMO_SIZE))
          {
            /* clear the image fields*/
            listOfImage[mIndexOfCurrentImage].manufacturerCode = 0x00;
            listOfImage[mIndexOfCurrentImage].imageType = 0x00;  
            listOfImage[mIndexOfCurrentImage].totalImageSize = 0x00;
            listOfImage[mIndexOfCurrentImage].fileVersion = 0x00;
            /* clear the image header from memory */
            (void)OTA_EraseBlock(listOfImage[mIndexOfCurrentImage].addressOfCurrentImage);
            return gOtaCrcError_c;
          }
        }
#endif       
        mServerUtility.imageLoadingState = gOtaServerLoadingImageComplete_c;
        status = newImageReceived(imgNotify);
        mIndexOfCurrentImage++;
 #if gZclOtaServerImgIntegrityCodeValidation_d     
         /* clear mMsgDigestServer Information */    
        FLib_MemSet(&mMsgDigestServer[0], 0x00, AES_MMO_SIZE);
#endif         
      }
    }
#endif /*  gZclOtaExternalMemorySupported_d   */
  } 
  return status;
}

/****************************************************************************
 *  ZCL_OTAClusterServer_AddToTemporaryQueryReqList
 *   - it is used for Server dongle solution to store temporary the client 
 *      information(address, endpoint)  
 ****************************************************************************/
static void ZCL_OTAClusterServer_AddToTemporaryQueryReqList(zbNwkAddr_t   aNwkAddr, zbEndPoint_t  endPoint)
{
  uint8_t i = 0;
  for(i=0; i< zclOTAServer_MaxTemporaryReqList_d; i++)
    if(mTemporaryQueryReqList[i].endPoint == 0xFF)
    {
      mTemporaryQueryReqList[i].endPoint = endPoint;
      Copy2Bytes(&mTemporaryQueryReqList[i].aNwkAddr, aNwkAddr);
      break;
    }
}
/****************************************************************************
 *  ZCL_OTAClusterServer_GetEndpointFromTemporaryQueryList
 *   - it is used for Server dongle solution to obtain the client endpoint
 ****************************************************************************/
static zbEndPoint_t ZCL_OTAClusterServer_GetEndpointFromTemporaryQueryList(zbNwkAddr_t   aNwkAddr)
{
  uint8_t i = 0;
  zbEndPoint_t endpoint = 0xFF;
  for(i=0; i< zclOTAServer_MaxTemporaryReqList_d; i++)
    if(IsEqual2Bytes(aNwkAddr, mTemporaryQueryReqList[i].aNwkAddr))
    {
      endpoint =  mTemporaryQueryReqList[i].endPoint;
      /* slot free */
      mTemporaryQueryReqList[i].endPoint = 0xFF;
      Set2Bytes(mTemporaryQueryReqList[i].aNwkAddr, 0xFFFF);
      break;
    }
  return endpoint;;
}
       
/****************************************************************************
 * OTAServer_CloseOTAProcess:
 *   - calling this function will terminate the Upgrade Process for the dongle case
 ****************************************************************************/
zbStatus_t OTAServer_CloseOTAProcess(void)
{
  uint8_t i;
  if(mServerUtility.operationMode == gDoNotUseExternalMemoryForOtaUpdate_c)
    mServerUtility.otaServerIsActive = FALSE;
  mServerUtility.blockResponseInProgress = FALSE;

  /* free slots */ 
   for(i = 0; i<zclOTAServer_MaxTemporaryReqList_d; i++)
      mTemporaryAbortBlockRspList[i].status = TRUE;
   
   return gOtaSucess_c;
}

/****************************************************************************
 * OTAServer_SetLocalFileVersionPolicies:
 *   - calling this function will set the local file version policies: 
 *     allow upgrade/reinstall/downgrade
 ****************************************************************************/

zbStatus_t OTAServer_SetLocalFileVersionPolicies(uint8_t localFileVersionPolicies)
{
#if gZclOtaExternalMemorySupported_d
  if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
    mServerUtility.localFileVersionPolicies = localFileVersionPolicies;
    
#endif  
  return gOtaSucess_c;
}


/****************************************************************************
 * OTAServer_AddToAbortBlockRspList:
 *   - calling this function will add the client in the abort BlockResponse List 
 ****************************************************************************/
zbStatus_t OTAServer_AddToAbortBlockRspList(zbNwkAddr_t   deviceID) 
{
  uint8_t i = 0;
  for(i = 0; i<zclOTAServer_MaxTemporaryReqList_d; i++)
  {
    if(mTemporaryAbortBlockRspList[i].status == TRUE)
    {
      mTemporaryAbortBlockRspList[i].status = FALSE;
      Copy2Bytes(mTemporaryAbortBlockRspList[i].aNwkAddr, deviceID);
        return gOtaSucess_c;
    }      
  }
  return gZclNoMem_c;
}

/****************************************************************************
 * ZtcOtaSupportAbortUpgradeFunc:
 *   - calling this function will check the abort BlockResponse List 
 ****************************************************************************/
static bool_t OTAServer_CheckAbortBlockRspList(zbNwkAddr_t   deviceID) 
{
  uint8_t i = 0;
  for(i = 0; i<zclOTAServer_MaxTemporaryReqList_d; i++)
  {
    if(mTemporaryAbortBlockRspList[i].status == FALSE)
    {
      if(IsEqual2Bytes(mTemporaryAbortBlockRspList[i].aNwkAddr, deviceID))  
      {
        mTemporaryAbortBlockRspList[i].status = TRUE;
        return TRUE;
      }
    }
  }
  return FALSE;
}

#if gZclOtaServerImgIntegrityCodeValidation_d
/****************************************************************************
 * OTAClusterProcessAES_MMOHash:
*  This function computes the AES MMO hash for OTA Server Application using blocks of image 
*  and stores the result in mMsgDigestServer.
 ****************************************************************************/
static void OTAClusterProcessAES_MMOHash(uint8_t *pImageBlock, uint8_t blockLength)
{
  uint8_t bytesToCopy, bytesCopied = 0;
  uint8_t sizeBlock = 2*AES_MMO_SIZE;
  static uint8_t aesMmoBlock[2*AES_MMO_SIZE];
  static uint8_t mPosition = 0;
  static bool_t lastBlockForHash = FALSE;
  
  if(listOfImage[mIndexOfCurrentImage].totalImageSize + listOfImage[mIndexOfCurrentImage].addressOfCurrentImage > (mServerUtility.currentAddress + blockLength + AES_MMO_SIZE))
  {
    while(bytesCopied < blockLength)
    {
        lastBlockForHash=FALSE;
        bytesToCopy = sizeBlock - mPosition;
        if(bytesToCopy > (blockLength - bytesCopied))
        {
            bytesToCopy = (blockLength - bytesCopied);
        }
        FLib_MemCpy(aesMmoBlock+mPosition, pImageBlock+bytesCopied, bytesToCopy);
        bytesCopied +=bytesToCopy;
        mPosition+=bytesToCopy;
        if(mPosition == sizeBlock)
        {
              OTAClusterAesMMO_hash(aesMmoBlock, sizeBlock, lastBlockForHash, listOfImage[mIndexOfCurrentImage].totalImageSize-AES_MMO_SIZE, mMsgDigestServer);
              mPosition = 0;
        }
      }
  }
  else
  {
    if(lastBlockForHash==FALSE)
    {
      uint8_t *lastAesMmoBlock;
      lastBlockForHash = TRUE;
      bytesToCopy = listOfImage[mIndexOfCurrentImage].totalImageSize + listOfImage[mIndexOfCurrentImage].addressOfCurrentImage - mServerUtility.currentAddress - AES_MMO_SIZE;
      sizeBlock = mPosition + bytesToCopy;
      lastAesMmoBlock = AF_MsgAlloc(); 
      if(!lastAesMmoBlock)
    	  return;
      FLib_MemCpy(lastAesMmoBlock, aesMmoBlock, mPosition);
      FLib_MemCpy(lastAesMmoBlock+mPosition, pImageBlock, bytesToCopy);
      OTAClusterAesMMO_hash(lastAesMmoBlock, sizeBlock, lastBlockForHash, listOfImage[mIndexOfCurrentImage].totalImageSize-AES_MMO_SIZE, mMsgDigestServer);
      mPosition = 0;
      MSG_Free(lastAesMmoBlock);
    }
  }
}
#endif /* gZclOtaServerImgIntegrityCodeValidation_d */  
#endif

/*****************************************************************************
******************************************************************************
*
* ZCL OTA CLIENT FUNCTIONS
*
******************************************************************************
*****************************************************************************/

#if gZclEnableOTAClient_d
 
/******************************************************************************
* ZCL_OTAClusterClient_EndUpgradeRequest
*
* Sends upgrade end request to server
******************************************************************************/  
zbStatus_t ZCL_OTAClusterClient_EndUpgradeRequest
(
  zbNwkAddr_t   aNwkAddr,
  zbEndPoint_t  endPoint,
  zclOTAStatus_t status
) 
{
  zclZtcOTAUpgradeEndRequest_t upgradeEndRequest;
  
  Copy2Bytes(&upgradeEndRequest.aNwkAddr, aNwkAddr);
  upgradeEndRequest.endPoint = endPoint;
  upgradeEndRequest.zclOTAUpgradeEndRequest.status = status;
  upgradeEndRequest.zclOTAUpgradeEndRequest.manufacturerCode = Native2OTA16(clientParams.manufacturerCode);
  upgradeEndRequest.zclOTAUpgradeEndRequest.imageType = Native2OTA16(clientParams.imageType);            
  upgradeEndRequest.zclOTAUpgradeEndRequest.fileVersion = Native2OTA32(clientSession.downloadingFileVersion);
  
#if  (gZclEnableOTAProgressReport_d == TRUE) 
  if(status == gZclOTASuccess_c)
  {
      gOtaProgressReportStatus = otaProgress100_c;
      BeeAppUpdateDevice(0x08, gOTAProgressReportEvent_c, 0, 0, NULL);
  }
#endif 

#if (gZclEnableOTAProgressReportToExternalApp_d == TRUE)  
  {
    /*send a report message with status TRUE or FALSE in the imageLength field 
    *(TRUE - image successfully transmitted to client , FALSE - otherwise)*/
    zclZtcImageOtaProgressReport_t ztcOtaProgressReportInf;
    if(status == gZclOTASuccess_c)
      ztcOtaProgressReportInf.imageLength = Native2OTA32(TRUE);
    else
      ztcOtaProgressReportInf.imageLength = Native2OTA32(FALSE);
    ztcOtaProgressReportInf.currentOffset = 0xFFFFFFFF;
    Copy2Bytes(ztcOtaProgressReportInf.deviceAddr,NlmeGetRequest(gNwkShortAddress_c));   
#ifndef gHostApp_d  
    ZTCQueue_QueueToTestClient((const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#else
    ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#endif 
  }
#endif   /* gZclEnableOTAProgressReportToExternApp_d */

  return ZCL_OTAUpgradeEndRequest(&upgradeEndRequest);
}

/******************************************************************************
* ZCL_OTAClusterClient_EndUpgradeAbortRequest
*
* Aborts current client session and sends end request with Abort or Invalid Image
*(if security failed) status to server
******************************************************************************/  
zbStatus_t ZCL_OTAClusterClient_EndUpgradeAbortRequest
(
  zbNwkAddr_t   aNwkAddr,
  zbEndPoint_t  endPoint, 
  zclOTAStatus_t status
) 
{
  uint8_t result = ZCL_OTAClusterClient_EndUpgradeRequest(aNwkAddr, endPoint, status);
  if (result != gZclSuccess_c)
    return gZclOTAAbort_c;
  
  OTAClusterClientAbortSession();
  return gZclSuccess_c;
}


/******************************************************************************
* ZCL_OTAClusterClient_NextImageRequest
*
* Sends back a next image request (may be as a result of Image Notify)
******************************************************************************/  

zbStatus_t ZCL_OTAClusterClient_NextImageRequest 
(
  zbNwkAddr_t   aNwkAddr,
  zbEndPoint_t  endPoint    
)
{
  zclZtcOTANextImageRequest_t nextImageRequest;     
  
  /* Send back the image request. */
  Copy2Bytes(&nextImageRequest.aNwkAddr, aNwkAddr);
  nextImageRequest.endPoint = endPoint;
  nextImageRequest.zclOTANextImageRequest.fieldControl = gZclOTANextImageRequest_HwVersionPresent;
  nextImageRequest.zclOTANextImageRequest.manufacturerCode = Native2OTA16(clientParams.manufacturerCode);
  nextImageRequest.zclOTANextImageRequest.imageType = Native2OTA16(clientParams.imageType);
  nextImageRequest.zclOTANextImageRequest.fileVersion = Native2OTA32(clientParams.currentFileVersion);
  nextImageRequest.zclOTANextImageRequest.hardwareVersion = Native2OTA16(clientParams.hardwareVersion);
  return ZCL_OTAImageRequest(&nextImageRequest);  
}


/******************************************************************************
* ZCL_OTAClusterClient_ImageNotify_Handler
*
* Handles the receipt of an ImageNotify Command on the client
******************************************************************************/  
zbStatus_t ZCL_OTAClusterClient_ImageNotify_Handler 
(
  zbApsdeDataIndication_t *pIndication
)
{
  zclOTAImageNotify_t *pImageNotify;
  uint8_t frameLen;
  uint8_t clientJitter;
  zclStatus_t result;
  bool_t  dropPacket = FALSE;
  
  bool_t isUnicast = (pIndication->fWasBroadcast == TRUE)?FALSE:TRUE;;
 
  /* update transaction Sequence number */
  if(pIndication->pAsdu[1] == gZclTransactionId)
	  gZclTransactionId++;
  pImageNotify =  (zclOTAImageNotify_t *)(pIndication->pAsdu + sizeof(zclFrame_t));
  frameLen = pIndication->asduLength - sizeof(zclFrame_t); 

  /* invalid payload type or invalid data length for specified payload type data */
  if (frameLen < sizeof(pImageNotify->payloadType) ||
      pImageNotify->payloadType >= gPayloadMax_c ||
      frameLen != imgNotifySizeArray[pImageNotify->payloadType])
    return  gZclMalformedCommand_c;
    
  /* unicast */
  if (isUnicast) 
  {
      /* send back query next image */
      result = ZCL_OTAClusterClient_NextImageRequest(pIndication->aSrcAddr, pIndication->srcEndPoint);
      if (result != gZclSuccess_c)
        result = gZclOTAAbort_c;
        return result;
  }

  /*  broadcast/multicast */
  
  /* validate jitter */ 
  if (pImageNotify->queryJitter > OTA_CLIENT_MAX_RANDOM_JITTER)
    dropPacket = TRUE;
  else
    /* compute random jitter */
    clientJitter = GetRandomRange(0, OTA_CLIENT_MAX_RANDOM_JITTER);
  
  /* validate manufacturer */
  if (pImageNotify->payloadType > gJitter_c) 
  {
    pImageNotify->manufacturerCode = OTA2Native16(pImageNotify->manufacturerCode);
    if (pImageNotify->manufacturerCode != clientParams.manufacturerCode &&
        pImageNotify->manufacturerCode != OTA_MANUFACTURER_CODE_MATCH_ALL)
        dropPacket = TRUE;
  }
  
  /* validate imageType */
  if (pImageNotify->payloadType > gJitterMfg_c) 
  {
    pImageNotify->imageType = OTA2Native16(pImageNotify->imageType);
    if (pImageNotify->imageType != clientParams.imageType &&
        pImageNotify->imageType != OTA_IMAGE_TYPE_MATCH_ALL)
        dropPacket = TRUE;
  }
  
  /* validate fileVersion */
  if (pImageNotify->payloadType > gJitterMfgImage_c) 
  {
    pImageNotify->fileVersion = OTA2Native32(pImageNotify->fileVersion);
    if (pImageNotify->fileVersion == clientParams.currentFileVersion)
        dropPacket = TRUE;
  }
  
  
  if (!dropPacket &&  clientJitter <= pImageNotify->queryJitter)
    /* send jitter message */
    {
      result = ZCL_OTAClusterClient_NextImageRequest(pIndication->aSrcAddr, pIndication->srcEndPoint);   
      if (result != gZclSuccess_c)
        result = gZclOTAAbort_c;
        return result;      
    }
  else
    /* no further processing */
    return gZclSuccess_c; 
  
}


/******************************************************************************
* ZCL_OTAClusterClient_NextBlockRequest
*
* Sends back a next image request (may be as a result of Image Notify)
******************************************************************************/  

zbStatus_t ZCL_OTAClusterClient_NextBlockRequest 
(
  zbNwkAddr_t   aNwkAddr,
  zbEndPoint_t  endPoint    
)
{ 
  zclZtcOTABlockRequest_t     blockRequest;

  Copy2Bytes(&blockRequest.aNwkAddr,aNwkAddr);
  blockRequest.endPoint = endPoint;
  blockRequest.zclOTABlockRequest.fieldControl = 0x00;
  if(gZclOTABlockRequest_BlockRequestDelayPresent_c == 0x02)
  {
	    
    blockRequest.zclOTABlockRequest.fieldControl |= gZclOTABlockRequest_BlockRequestDelayPresent_c;
    blockRequest.zclOTABlockRequest.blockRequestDelay = Native2OTA16(clientSession.blockRequestDelay);
  }

  blockRequest.zclOTABlockRequest.manufacturerCode = Native2OTA16(clientParams.manufacturerCode);
  blockRequest.zclOTABlockRequest.imageType = Native2OTA16(clientParams.imageType);
  blockRequest.zclOTABlockRequest.fileVersion = Native2OTA32(clientSession.downloadingFileVersion);
  blockRequest.zclOTABlockRequest.fileOffset = Native2OTA32(clientSession.currentOffset);
  blockRequest.zclOTABlockRequest.maxDataSize = clientParams.maxDataSize;
  return ZCL_OTABlockRequest(&blockRequest);
}

/******************************************************************************
* ZCL_OTAClusterClient_NextImageRequest
*
* Sends back a next image request (may be as a result of Image Notify)
******************************************************************************/  

zbStatus_t ZCL_OTAClusterClient_NextImageResponse_Handler
(
  zbApsdeDataIndication_t *pIndication
)
{
  zclOTANextImageResponse_t   *pNextImageResponse;
  uint8_t result;

  pNextImageResponse = (zclOTANextImageResponse_t *)(pIndication->pAsdu + sizeof(zclFrame_t));
  
  /* status is successful */
  if(pNextImageResponse->status == gZclOTASuccess_c)
  {
    /* validate frame length */
    if (pIndication->asduLength - sizeof(zclFrame_t) != sizeof(zclOTANextImageResponse_t))
      return gZclMalformedCommand_c;
    
    pNextImageResponse->manufacturerCode = OTA2Native16(pNextImageResponse->manufacturerCode);
    pNextImageResponse->imageType = OTA2Native16(pNextImageResponse->imageType);
    pNextImageResponse->fileVersion = OTA2Native32(pNextImageResponse->fileVersion);
    pNextImageResponse->imageSize = OTA2Native32(pNextImageResponse->imageSize);    
    
    /* validate frame params */
    if ((pNextImageResponse->manufacturerCode != clientParams.manufacturerCode &&
        pNextImageResponse->manufacturerCode != OTA_MANUFACTURER_CODE_MATCH_ALL) ||
        (pNextImageResponse->imageType != clientParams.imageType &&
        pNextImageResponse->imageType != OTA_IMAGE_TYPE_MATCH_ALL ))   
      return gZclMalformedCommand_c;
    
    /* begin client session */
    result = OTAClusterClientStartSession(pNextImageResponse->imageSize, pNextImageResponse->fileVersion);
    if(result != gZbSuccess_c) return gZclOTAAbort_c;
    /* stop timer that send query image request periodically */
    TMR_FreeTimer(gNextImageReqTimer); 
    
    gRetransmitLastBlockTimer = TMR_AllocateTimer(); 
    if(gRetransmitLastBlockTimer == gTmrInvalidTimerID_c)
       return gZclNoMem_c;	
    
    /* send the first block request */  
    result = ZCL_OTAClusterClient_NextBlockRequest(pIndication->aSrcAddr, pIndication->srcEndPoint);
    if(result != gZclSuccess_c) {
      return ZCL_OTAClusterClient_EndUpgradeAbortRequest(pIndication->aSrcAddr, pIndication->srcEndPoint, gZclOTAAbort_c);
    }
    return  gZclSuccess_c;
  }
  else
  {
	if(pNextImageResponse->status == gZclOTANoImageAvailable_c)  
	{
                TMR_FreeTimer(gNextImageReqTimer); 
		gNextImageReqTimer = TMR_AllocateTimer(); 
		if(gNextImageReqTimer == gTmrInvalidTimerID_c)
			return gZclNoMem_c;		
		TMR_StartMinuteTimer(gNextImageReqTimer, gMinforNextImageRequest_c, OTAClusterClientNextImageReqTimerCallback);
	}
	else
	{ 
          zbClusterId_t clusterId = {gaZclClusterOTA_c};          
          zclInitiateOtaProcess_t  initServerDiscovery = {0, 0};
          initServerDiscovery.isServer = FALSE;
          gClientOTAEndpoint = ZCL_GetEndPointForSpecificCluster(clusterId, FALSE);
          initServerDiscovery.clientInit.endPoint = gClientOTAEndpoint;
          (void)OTA_InitiateOtaClusterProcess(&initServerDiscovery);	
	}
    return gZclSuccess_c;
  } 
}

/******************************************************************************
* ZCL_OTAClusterClient_BlockResponse_Handler
*
* Handles the block response indication
******************************************************************************/  

zbStatus_t ZCL_OTAClusterClient_BlockResponse_Handler
(
  zbApsdeDataIndication_t *pIndication
) 
{
  zclOTABlockResponse_t *pBlockResponse;

  uint8_t result;
  
  /* Handle succes status received. Verify the received parameters */
  pBlockResponse = (zclOTABlockResponse_t *)(pIndication->pAsdu + sizeof(zclFrame_t));

  if(pBlockResponse->status == gZclOTASuccess_c)
  {
      /* validate frame length */
    if (pIndication->asduLength - sizeof(zclFrame_t) != 
        sizeof(zclOTAStatus_t) + MbrOfs(zclOTABlockResponseStatusSuccess_t, data) + 
        pBlockResponse->msgData.success.dataSize)
      return gZclMalformedCommand_c;
      
    /* command fields */
    pBlockResponse->msgData.success.manufacturerCode = OTA2Native16(pBlockResponse->msgData.success.manufacturerCode);
    pBlockResponse->msgData.success.imageType = OTA2Native16(pBlockResponse->msgData.success.imageType);
    pBlockResponse->msgData.success.fileVersion = OTA2Native32(pBlockResponse->msgData.success.fileVersion);
    pBlockResponse->msgData.success.fileOffset = OTA2Native32(pBlockResponse->msgData.success.fileOffset);
          
    /* Error cases. Send back malformed command indication - no abort of the current upgrade executed. */
    if((pBlockResponse->msgData.success.manufacturerCode != clientParams.manufacturerCode)||
       (pBlockResponse->msgData.success.imageType != clientParams.imageType)||
       (pBlockResponse->msgData.success.fileVersion != clientSession.downloadingFileVersion) ||
       (pBlockResponse->msgData.success.dataSize > clientParams.maxDataSize ) ||
       (pBlockResponse->msgData.success.dataSize + clientSession.currentOffset > clientSession.fileLength)) 
    {  
      return gZclMalformedCommand_c;
    }
    /* handle out of sync packets by repeating the request (spec does not handle this as error) */
    else if ( pBlockResponse->msgData.success.fileOffset != clientSession.currentOffset ) 
    {
      /* send the first block request */   
      result = ZCL_OTAClusterClient_NextBlockRequest(pIndication->aSrcAddr, pIndication->srcEndPoint);
      if(result != gZclSuccess_c) 
      {
        return ZCL_OTAClusterClient_EndUpgradeAbortRequest(pIndication->aSrcAddr, pIndication->srcEndPoint, gZclOTAAbort_c);
      }
      return  gZclSuccess_c;
    }
    else 
    {
      /* Handle the received data chunk - push it to the image storage if we have a session started */
      if(clientSession.sessionStarted == TRUE)
      {
        if(gpBlockCallbackState)
        {
          MSG_Free(gpBlockCallbackState);
          gpBlockCallbackState = NULL;
        }
        /* do it on a timer to handle processing aps and 
          writing to external memory sync issues */
        if(gBlockCallbackTimer == gTmrInvalidTimerID_c)
        	gBlockCallbackTimer = TMR_AllocateTimer();
        
        if(gpBlockCallbackState == NULL)
        	gpBlockCallbackState = AF_MsgAlloc();

        if (gpBlockCallbackState && (gBlockCallbackTimer != gTmrInvalidTimerID_c)) 
        {  
           uint16_t blockDelay = (clientSession.blockRequestDelay > 50)? clientSession.blockRequestDelay:BLOCK_PROCESSING_CALLBACK_DELAY;
          
           Copy2Bytes(&gpBlockCallbackState->dstAddr, &pIndication->aSrcAddr);
           gpBlockCallbackState->dstEndpoint = pIndication->srcEndPoint;
           gpBlockCallbackState->blockSize = pBlockResponse->msgData.success.dataSize;
           FLib_MemCpy(gpBlockCallbackState->blockData,
                       pBlockResponse->msgData.success.data, 
                       pBlockResponse->msgData.success.dataSize);
           
           
           TMR_StartTimer(gBlockCallbackTimer, gTmrSingleShotTimer_c,
                          blockDelay, OTAClusterClientProcessBlockTimerCallback);
                          
           return gZclSuccess_c;
        } 
        else 
        {
          return ZCL_OTAClusterClient_EndUpgradeAbortRequest(pIndication->aSrcAddr, pIndication->srcEndPoint, gZclOTAAbort_c);
        }
      }
      else 
      {
        return ZCL_OTAClusterClient_EndUpgradeAbortRequest(pIndication->aSrcAddr, pIndication->srcEndPoint, gZclOTAAbort_c);
      }
    }
  }
    else if(pBlockResponse->status == gZclOTAWaitForData_c)
    {
    	uint32_t timeInSeconds = OTA2Native32(pBlockResponse->msgData.wait.requestTime) -
						OTA2Native32(pBlockResponse->msgData.wait.currentTime);
        if(gZclOTABlockRequest_BlockRequestDelayPresent_c == 0x02)
        {
          clientSession.blockRequestDelay = OTA2Native16(pBlockResponse->msgData.wait.blockRequestDelay);
#if gZclClusterOptionals_d          
          gOTAAttrs.MinimumBlockReqDelay =  clientSession.blockRequestDelay;
#endif          
        }
        TMR_StartSecondTimer(gRetransmitLastBlockTimer,(uint16_t)timeInSeconds, OTAClientRetransmitLastBlockTmrCallback); 
        return  gZclSuccess_c;
    }
    else if(pBlockResponse->status == gZclOTAAbort_c)
    {
        OTAClusterClientAbortSession();
        return gZclSuccess_c;
    }
    else /*unknown status */
      /* Error detected. Send back malformed command indication - no abort of the current upgrade executed. */
      return gZclMalformedCommand_c;

}
  


/******************************************************************************
* ZCL_OTAClusterClient_UpgradeEndResponse_Handler
*
* Handles the upgrade end response indication
******************************************************************************/  

zbStatus_t ZCL_OTAClusterClient_UpgradeEndResponse_Handler
(
  zbApsdeDataIndication_t *pIndication
) 

{
    zclOTAUpgradeEndResponse_t *pUpgradeEndResponse;
    pUpgradeEndResponse = (zclOTAUpgradeEndResponse_t*)(pIndication->pAsdu + sizeof(zclFrame_t));
    pUpgradeEndResponse->manufacturerCode = OTA2Native16(pUpgradeEndResponse->manufacturerCode);
    pUpgradeEndResponse->imageType = OTA2Native16(pUpgradeEndResponse->imageType);
    pUpgradeEndResponse->fileVersion = OTA2Native32(pUpgradeEndResponse->fileVersion);
    pUpgradeEndResponse->currentTime = OTA2Native32(pUpgradeEndResponse->currentTime);
    pUpgradeEndResponse->upgradeTime = OTA2Native32(pUpgradeEndResponse->upgradeTime);
      
    /* Verify the data received in the response  */
    if((pUpgradeEndResponse->manufacturerCode != clientParams.manufacturerCode)||
       (pUpgradeEndResponse->imageType != clientParams.imageType)||
       (pUpgradeEndResponse->fileVersion != clientSession.downloadingFileVersion))
    {
      return gZclMalformedCommand_c;
    }
    else
    {
      uint32_t offset = pUpgradeEndResponse->upgradeTime - pUpgradeEndResponse->currentTime;

      if(offset == 0)
      {   
        offset++;
        if(clientSession.steps >= 3)
        {
          /* allow at least 1 second before reset */	
          gBlockCallbackTimer = TMR_AllocateSecondTimer();
          /* Flash flags will be write in next instance of idle task  */
          OTA_SetNewImageFlag();
          /* OTA TODO this does not accept 32 bit second timers */
          TMR_StartSecondTimer(gBlockCallbackTimer, (uint16_t)offset, OTAClusterCPUResetCallback);      
        }
        else
        {
        	OTAClusterClientAbortSession();
        }
      }
      else
      {
        if(pUpgradeEndResponse->upgradeTime == 0xFFFFFFFF)
        {
            /* wait to upgrate after receiving the 2nd Upgrade End Response   */
            gOTAAttrs.ImageUpgradeStatus = gOTAUpgradeStatusWaitingToUpgrade_c;      
        }
        else
        {
            /* wait offset seconds after the client shall perform to apply the upgrade process; */
            gOTAAttrs.ImageUpgradeStatus = gOTAUpgradeStatusCountDown_c; 
            gBlockCallbackTimer = TMR_AllocateSecondTimer();
            /* OTATODO this does not accept 32 bit second timers */
            TMR_StartSecondTimer(gBlockCallbackTimer, (uint16_t)offset, OTAClusterDelayedUpgradeCallback);  
        }
      } 
      return gZclSuccess_c;
    }
}

/******************************************************************************
* OTAClusterDelayedUpgradeCallback
*
* Resets the node to upgrade after Upgrade End Response
******************************************************************************/
static void OTAClusterDelayedUpgradeCallback(uint8_t tmr) 
{
  (void)tmr;  
  /* Flash flags will be write in next instance of idle task */
  if(clientSession.steps >= 3)
  { 
      OTA_SetNewImageFlag();
      /* allow at least 1 second before reset */
      TMR_StartSecondTimer(gBlockCallbackTimer, 0x01, OTAClusterCPUResetCallback); 
  }
  else
  {
      OTAClusterClientAbortSession();
  }
}

/******************************************************************************
* OTAClusterResetCallback
*
* Resets the node to upgrade after Upgrade End Response
******************************************************************************/
static void OTAClusterCPUResetCallback(uint8_t tmr) 
{
  (void)tmr;
#ifndef __IAR_SYSTEMS_ICC__
  /* S08 platform reset */
  PWRLib_Reset();               
#else
#ifdef PROCESSOR_KINETIS
  PWRLib_Reset(); 
#else  
  CRM_SoftReset();
#endif
#endif  
}


/******************************************************************************
* ZCL_OTAClusterClient
*
* Incoming OTA cluster frame processing loop for cluster client side
******************************************************************************/
zbStatus_t ZCL_OTAClusterClient
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
)
{
    zclCmd_t            command;
    zclFrame_t          *pFrame;
    zbStatus_t          result = gZclSuccessDefaultRsp_c;    
    zclCmdDefaultRsp_t * pDefaultRsp;
    zbIeeeAddr_t aExtAddr;
    /* prevent compiler warning */
    (void)pDevice;
    pFrame = (void *)pIndication->pAsdu;
    
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
    	result = gZclSuccess_c;
	
    /* handle the command */
    command = pFrame->command;
    switch(command)
    {
      case gZclCmdOTA_ImageNotify_c:
      {
    	/*ignore command if an ota session started */
    	if(clientSession.sessionStarted == TRUE)
            return result;
        /*test the upgradeServerID attribute*/
        (void)APS_GetIeeeAddress(pIndication->aSrcAddr, aExtAddr);
        gClientOTAEndpoint = ZCL_GetEndPointForSpecificCluster(pIndication->aClusterId, FALSE);    
        if(!FLib_MemCmp(aExtAddr,gOTAAttrs.UpgradeServerId, 8))
        {   
          otaCluster_simpleDescriptor.endPoint = gClientOTAEndpoint;
          Copy2Bytes(&otaCluster_simpleDescriptor.aAppProfId, pIndication->aProfileId);     		  
          ASL_MatchDescriptor_req(NULL,pIndication->aSrcAddr, (zbSimpleDescriptor_t*)&otaCluster_simpleDescriptor); 
          mSendOtaMatchDescriptorRequest = TRUE; 
          return result;
        }
        result = ZCL_OTAClusterClient_ImageNotify_Handler(pIndication);  
        break;
      } 
      case gZclCmdOTA_QueryNextImageResponse_c:
      {
    	if(pFrame->transactionId == gOTAClient_RequestedTsqId) 
    		result =  ZCL_OTAClusterClient_NextImageResponse_Handler(pIndication);
    	else
    		result = gZclFailure_c;
        break;
      }
      case gZclCmdOTA_ImageBlockResponse_c:
      {
    	  
    	 if(pFrame->transactionId == gOTAClient_RequestedTsqId) 
    	 {
    	    /* process packed received */	
    	    TMR_StopTimer(gRetransmitLastBlockTimer);
    	    gRetransmitCounter = 0x00;
    	    result =  ZCL_OTAClusterClient_BlockResponse_Handler(pIndication);
    	 }
    	 
    	 if(!TMR_IsTimerActive(gRetransmitLastBlockTimer))
    	 {
            if(!TMR_IsTimerReady(gRetransmitLastBlockTimer))
            {
              /* force an request if the client don't receive the BlockResponse */
               uint16_t blockDelay = (clientSession.blockRequestDelay > 50)? clientSession.blockRequestDelay:BLOCK_PROCESSING_CALLBACK_DELAY; 
    	       TMR_StartIntervalTimer(gRetransmitLastBlockTimer, blockDelay + 4000, OTAClientRetransmitLastBlockTmrCallback); 
            }
    	 }
    	 break;
      }
      case gZclCmdOTA_UpgradeEndResponse_c:  
      {  
    	result = ZCL_OTAClusterClient_UpgradeEndResponse_Handler(pIndication);
    	break;
      }
      case gZclCmdDefaultRsp_c:
         pDefaultRsp = ZCL_GetPayload(pIndication);
         /* abort the session when receive an DefaultResponse with status 0x95 for End Request Command; */
         if(((uint8_t)(pDefaultRsp->status) == gZclOTAAbort_c)&&(pDefaultRsp->cmd==gZclCmdOTA_UpgradeEndRequest_c))
           OTAClusterClientAbortSession();
         return gZclSuccess_c;
      default:
      {
    	result = gZclUnsupportedClusterCommand_c;
        break;
      }
    }
    
    if(result == gZclSuccess_c)
    {
    	result = (pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)?gZclSuccess_c:gZclSuccessDefaultRsp_c;
    }   
    return result;
}


/******************************************************************************
* ZCL_OTASetClientParams
*
* Interface function to set client parameters
******************************************************************************/
zbStatus_t ZCL_OTASetClientParams(zclOTAClientParams_t* pClientParams)
{
  FLib_MemCpy(&clientParams, pClientParams, sizeof(zclOTAClientParams_t));
  clientParams.manufacturerCode = OTA2Native16(clientParams.manufacturerCode);
  clientParams.imageType = OTA2Native16(clientParams.imageType);
  clientParams.currentFileVersion = OTA2Native32(clientParams.currentFileVersion);
  clientParams.downloadedFileVersion = OTA2Native32(clientParams.downloadedFileVersion);
  clientParams.hardwareVersion = OTA2Native16(clientParams.hardwareVersion);
#if gZclClusterOptionals_d 
  gOTAAttrs.CurrentFileVersion = Native2OTA32(clientParams.currentFileVersion);
  gOTAAttrs.DownloadedFileVersion = Native2OTA32(clientParams.downloadedFileVersion);
#endif
  return gZclSuccess_c;
}

/******************************************************************************
* ZCL_OTAImageRequest
*
* Request to send a image request
******************************************************************************/
zbStatus_t ZCL_OTAImageRequest(zclZtcOTANextImageRequest_t* pZtcNextImageRequestParams)
{
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif  
  
  /* Create the destination address */
  Copy2Bytes(&addrInfo.dstAddr,&pZtcNextImageRequestParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcNextImageRequestParams->endPoint;
  addrInfo.srcEndPoint = gClientOTAEndpoint; 
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  return ZCL_OTAImageRequestCmd(addrInfo, pZtcNextImageRequestParams->zclOTANextImageRequest);
}

/******************************************************************************
* ZCL_OTAImageRequestCmd
*
* Request to send a image request command
******************************************************************************/
static zbStatus_t ZCL_OTAImageRequestCmd(afAddrInfo_t addrInfo, zclOTANextImageRequest_t zclOTANextImageRequest)
{
  afToApsdeMessage_t *pMsg;	
  uint8_t len = sizeof(zclOTANextImageRequest_t);  
  
  if(!(zclOTANextImageRequest.fieldControl & gZclOTANextImageRequest_HwVersionPresent)) 
    len -=sizeof(uint16_t); /* remove hardware version from packet */   

  /* store the current TSQ */
  gOTAClient_RequestedTsqId = gZclTransactionId;
  
  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_QueryNextImageRequest_c,
                          gZclFrameControl_FrameTypeSpecific, 
                          NULL, 
                          &len,
                          &zclOTANextImageRequest);
  if(!pMsg)
    return gZclNoMem_c;
  
 return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  
}

/******************************************************************************
* ZCL_OTABlockRequest
*
* Request to send an image block
******************************************************************************/
zbStatus_t ZCL_OTABlockRequest(zclZtcOTABlockRequest_t *pZtcBlockRequestParams)
{
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif  
  
  /* Create the destination address */
  Copy2Bytes(&addrInfo.dstAddr,&pZtcBlockRequestParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcBlockRequestParams->endPoint;
  addrInfo.srcEndPoint = gClientOTAEndpoint;  
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  return ZCL_OTABlockRequestCmd(addrInfo, pZtcBlockRequestParams->zclOTABlockRequest);
}

/******************************************************************************
* ZCL_OTABlockRequestCmd
*
* Request to send an image block request
******************************************************************************/
static zbStatus_t ZCL_OTABlockRequestCmd(afAddrInfo_t addrInfo, zclOTABlockRequest_t  zclOTABlockRequest)
{
   afToApsdeMessage_t *pMsg;	
   uint8_t len = sizeof(zclOTABlockRequest_t);
   
   if(!(zclOTABlockRequest.fieldControl & gZclOTABlockRequest_BlockRequestDelayPresent_c))
     len -=sizeof(uint16_t); /* remove block request delay from packet */ 
   
   /* store the current TSQ */
   gOTAClient_RequestedTsqId = gZclTransactionId;
   
   pMsg = ZCL_CreateFrame( &addrInfo, 
	                       gZclCmdOTA_ImageBlockRequest_c,
	                       gZclFrameControl_FrameTypeSpecific, 
	                       NULL, 
	                       &len,
	                       &zclOTABlockRequest);
   if(!pMsg)
	   return gZclNoMem_c;
   
   return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  	
}


/******************************************************************************
* ZCL_OTAUpgradeEndRequest
*
* Request to send an upgrade end request
******************************************************************************/
zbStatus_t ZCL_OTAUpgradeEndRequest(zclZtcOTAUpgradeEndRequest_t *pZtcUpgradeEndParams)
{
  afToApsdeMessage_t *pMsg;
  uint8_t len;
  
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif

  /* Create the destination address */
  Copy2Bytes(&addrInfo.dstAddr,&pZtcUpgradeEndParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcUpgradeEndParams->endPoint;
  addrInfo.srcEndPoint = gClientOTAEndpoint;
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  len = sizeof(zclOTAUpgradeEndRequest_t);

  /* store the current TSQ */
  gOTAClient_RequestedTsqId = gZclTransactionId;
  
  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_UpgradeEndRequest_c,
                          gZclFrameControl_FrameTypeSpecific, 
                          NULL, 
                          &len,
                          &pZtcUpgradeEndParams->zclOTAUpgradeEndRequest);
  if(!pMsg)
    return gZclNoMem_c;
 return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  
}

/******************************************************************************
* OTAClusterClientAbortSession
*
* Private function. Abort a started image download session
******************************************************************************/
void OTAClusterClientAbortSession()
{
  clientSession.sessionStarted = FALSE;
  clientSession.fileLength = 0;
  clientSession.currentOffset = 0;  
  clientSession.steps = 0;
  gOTAClient_RequestedTsqId = 0;
  
  /* clear OTA client session timer */
  if(gBlockCallbackTimer != gTmrInvalidTimerID_c)
  {
	  TMR_FreeTimer(gBlockCallbackTimer);
  	  gBlockCallbackTimer = gTmrInvalidTimerID_c;
  }
  
  /*clear OTA client session Callback */
  MSG_Free(gpBlockCallbackState);
  gpBlockCallbackState = NULL;
   
  /* restore OTA attributes */
  gOTAAttrs.ImageUpgradeStatus = gOTAUpgradeStatusNormal_c;
#if gZclClusterOptionals_d
  gOTAAttrs.CurrentFileVersion = Native2OTA32(clientParams.currentFileVersion);
  gOTAAttrs.DownloadedFileVersion = Native2OTA32(clientParams.downloadedFileVersion);
  gOTAAttrs.FileOffset = 0;
  gOTAAttrs.MinimumBlockReqDelay = 0;
#endif
  if(clientSession.pStateBuffer != NULL)
  {
    MSG_Free(clientSession.pStateBuffer);
    clientSession.pStateBuffer = NULL;
  }
  OTA_CancelImage();
  //(void)OTA_EraseExternalMemory();
  
#if gTargetTWR_KW24D512_d || gTargetTWR_KW22D512_d || gTargetTWR_KW21D256_d || gTargetTWR_KW21D512_d
#if !gUseInternalFlashForOta_c  
  /* re-Init the keyboard module: for the specified platforms the keyboard and external EEprom are on the same Bus */
  KBD_Init(BeeAppHandleKeys);
#endif /* !gUseInternalFlashForOta_c */  
#endif /* gTargetTWR_KW24D512_d || gTargetTWR_KW22D512_d || gTargetTWR_KW21D256_d || gTargetTWR_KW21D512_d */ 
  
#if gEndDevCapability_d || gComboDeviceCapability_d 
  /* stop the OTA FastPoll Mode */
  if(gRetransmitLastBlockTimer == gTmrInvalidTimerID_c)
	 gRetransmitLastBlockTimer = TMR_AllocateTimer(); 
  TMR_StartSecondTimer(gRetransmitLastBlockTimer, 1, OTAClient_UpdatePollRateCallBack);
#else
  if(gRetransmitLastBlockTimer != gTmrInvalidTimerID_c)
  {
	  TMR_FreeTimer(gRetransmitLastBlockTimer); 
	  gRetransmitLastBlockTimer = gTmrInvalidTimerID_c;
  }
#endif     
  
  /* start timer for the next QueryRequest */
  gNextImageReqTimer = TMR_AllocateTimer(); 
  if(gNextImageReqTimer == gTmrInvalidTimerID_c)
     return;
  TMR_StartMinuteTimer(gNextImageReqTimer, gMinforNextImageRequest_c, OTAClusterClientNextImageReqTimerCallback);
}

#if gEndDevCapability_d || gComboDeviceCapability_d 
/******************************************************************************
* OTAClient_UpdatePollRateCallBack
*
* stop OTA Client Fast Poll Mode  
******************************************************************************/
static void OTAClient_UpdatePollRateCallBack(uint8_t tmrId)
{  
	(void)tmrId;
	if (!IsLocalDeviceReceiverOnWhenIdle())
	{ 
#if gZclEnablePollControlCluster_d	  
		if(NlmeGetRequest(gNwkIndirectPollRate_c) < OTA2Native32(gPollControlAttrs.longPollInterval)*1000/4)  
		{
                    (void)ZDO_NLME_ChangePollRate((uint16_t)(OTA2Native32(gPollControlAttrs.longPollInterval)*1000/4));
#ifdef PROCESSOR_KINETIS
                    /* Save the new pollRate in NVM */
                    NvSaveOnIdle(&gSAS_Ram, TRUE);
#else		    
                    ZCL_SaveNvmZclData();
#endif                    
		}
#else
	    if(NlmeGetRequest(gNwkIndirectPollRate_c) < mDefaultValueOfIndirectPollRate_c)
	    {
	      (void)ZDO_NLME_ChangePollRate(mDefaultValueOfIndirectPollRate_c);
#ifdef PROCESSOR_KINETIS
                    /* Save the new pollRate in NVM */
                    NvSaveOnIdle(&gSAS_Ram, TRUE);
#else		    
                    ZCL_SaveNvmZclData();
#endif                    
	    }
#endif
   }
   TMR_FreeTimer(gRetransmitLastBlockTimer); 
   gRetransmitLastBlockTimer = gTmrInvalidTimerID_c;
}
#endif	

/******************************************************************************
* OTAClusterClientStartSession
*
* Private function. Starts an image download session.
******************************************************************************/
zbStatus_t OTAClusterClientStartSession(uint32_t fileLength, uint32_t fileVersion)
{
  /* Download file management */
  clientSession.sessionStarted = TRUE;
  clientSession.fileLength = fileLength;
  clientSession.downloadingFileVersion = fileVersion;
  clientSession.currentOffset = 0;  
  //Buffer management and state initialization for image file stream processing
  clientSession.state = OTAClient_InitState_c;
  clientSession.steps = 0;
  clientSession.bytesNeededForState = HEADER_LEN_OFFSET + sizeof(uint16_t);
  clientSession.stateBufferIndex = 0;
  if(clientSession.pStateBuffer == NULL)
  {
    clientSession.pStateBuffer = MSG_Alloc(SESSION_BUFFER_SIZE);
  }
  if(clientSession.pStateBuffer == NULL) return gZbNoMem_c;

#if (gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d)
  FLib_MemSet(&mMsgDigest[0], 0x00, AES_MMO_SIZE);
#endif  
#if (gZclEnableOTAProgressReport_d)
  gOtaProgressReportStatus = otaStartProgress_c;  //have 4 states: 0 = startProgress, 1 = 33%, 2 = 66%, 3 = 100%
#endif  
  gOTAAttrs.ImageUpgradeStatus = gOTAUpgradeStatusDownloadInProgress_c;
   
#if gEndDevCapability_d || gComboDeviceCapability_d
  if (!IsLocalDeviceReceiverOnWhenIdle())
  { 
	/*	update poll rate during the OTA process: go to FastPollMode */ 		  
#if gZclEnablePollControlCluster_d	    
	(void)ZDO_NLME_ChangePollRate((uint16_t)(OTA2Native16(gPollControlAttrs.shortPollInterval)*1000/4));
#else
	(void)ZDO_NLME_ChangePollRate(250);
#endif
  }
#endif 
  
  return gZbSuccess_c;
}

/******************************************************************************
* OTAClusterClientRunImageProcessStateMachine
*
* Private function. Process a block of received data.
******************************************************************************/
zbStatus_t OTAClusterClientRunImageProcessStateMachine()
{
  zclOTAFileHeader_t* pHeader;
  zclOTAFileSubElement_t* pSubElement;
  static uint32_t subElementLen;
  static uint8_t *pBitmap;
  uint16_t headerLen;
  
#if (!gZclEnableOTAClientECCLibrary_d)
  uint32_t crcReceived = 0;
  /* Current CRC value */
  static uint16_t mCrcCompute  = 0;
#else
  static IdentifyCert_t mCertReceived;
#endif
  
  switch(clientSession.state)
  {
  case OTAClient_InitState_c:  
    /* In the init state we only extract the header length and move to the next state.
    * The bytes received so far are not consumed */
    headerLen = *(uint16_t*)(clientSession.pStateBuffer + HEADER_LEN_OFFSET);
    clientSession.bytesNeededForState = (uint8_t)OTA2Native16(headerLen);
    clientSession.state = OTAClient_ProcessHeaderState_c;
#if (!gZclEnableOTAClientECCLibrary_d)
    mCrcCompute  = 0;
#endif
    break;
  case OTAClient_ProcessHeaderState_c:
    pHeader = (zclOTAFileHeader_t*)clientSession.pStateBuffer;
#if (!gZclEnableOTAClientECCLibrary_d)
    /* process image CRC */
    mCrcCompute = OTA_CrcCompute(clientSession.pStateBuffer, clientSession.bytesNeededForState, mCrcCompute);
#endif    
    
    /* Check the header for consistency */
    if((pHeader->headerVersion != gZclOtaHeaderVersion_c)||((OTA2Native16(pHeader->imageType)!= OTA_IMAGE_TYPE_MATCH_ALL)&&(OTA2Native16(pHeader->imageType) != clientParams.imageType))||
       ((OTA2Native16(pHeader->manufacturerCode) != clientParams.manufacturerCode)&&(OTA2Native16(pHeader->manufacturerCode) != OTA_MANUFACTURER_CODE_MATCH_ALL)))
    {
      return gZclOTAAbort_c;
    }
    
    /* check the field control for supported features - upgrade file destination and security credential version not supported */
    if((OTA2Native16(pHeader->fieldControl) & SECURITY_CREDENTIAL_VERSION_PRESENT)||(OTA2Native16(pHeader->fieldControl) & DEVICE_SPECIFIC_FILE))
      return gZclOTAAbort_c;
    
    /* If HW version is specified, verify it against our own */
    if(OTA2Native16(pHeader->fieldControl) & HARDWARE_VERSION_PRESENT)
    {
      if(!((OTA2Native16(pHeader->minHWVersion) <= clientParams.hardwareVersion)&&(clientParams.hardwareVersion <= OTA2Native16(pHeader->maxHWVersion))))
        return gZclOTAAbort_c;
    }
    
    /* If we got here it means we are ready for the upgrade image tag. 
     * All bytes from the buffer have been processed. The next state requires receiving a sub-element */
    clientSession.state = OTAClient_ProcessSubElementTagState_c;
    clientSession.bytesNeededForState =  sizeof(zclOTAFileSubElement_t);
    clientSession.stateBufferIndex = 0;
    clientSession.steps++;
  break;
  case OTAClient_ProcessSubElementTagState_c:
    {
      pSubElement = (zclOTAFileSubElement_t*)clientSession.pStateBuffer;
#if (!gZclEnableOTAClientECCLibrary_d)   
      /* process image CRC */ 
      if(pSubElement->id != gZclOtaUpgradeCRCTagId_c)    
        mCrcCompute = OTA_CrcCompute(clientSession.pStateBuffer, clientSession.bytesNeededForState, mCrcCompute);
#endif /* (!gZclEnableOTAClientECCLibrary_d) */
    
      switch(pSubElement->id)
      {      
        case gZclOtaUpgradeImageTagId_c:
          clientSession.state = OTAClient_ProcessUpgradeImageState_c;
          /* All OK, get the image length */
          subElementLen = OTA2Native32(pSubElement->length);
          /* Start the FLASH upgrade process */
          if(OTA_StartImage(subElementLen) != gOtaSucess_c)
          {
            return gZclOTAAbort_c;
          } 
          clientSession.steps++;
          clientSession.bytesNeededForState = (uint8_t)((subElementLen > SESSION_BUFFER_SIZE) ? SESSION_BUFFER_SIZE : subElementLen);
          break;
        case gZclOtaSectorBitmapTagId_c:  
          clientSession.state = OTAClient_ProcessBitmapState_c;
          clientSession.bytesNeededForState = (uint8_t)OTA2Native32(pSubElement->length);
          break;
#if (!gZclEnableOTAClientECCLibrary_d) 	          
        case gZclOtaUpgradeCRCTagId_c:
          clientSession.state = OTAClient_ProcessCRCState_c;
          clientSession.bytesNeededForState = (uint8_t)OTA2Native32(pSubElement->length);
          break;          
        case gZclOtaImageIntegrityCodeTagId:
          clientSession.state = OTAClient_ProcessImgIntegrityCodeState_c;
          clientSession.bytesNeededForState = (uint8_t)OTA2Native32(pSubElement->length);
          break;        
#else        
        case gZclOtaECDSASigningCertTagId_c: 
          clientSession.state = OTAClient_ProcessECDSASignCertState_c;
          clientSession.bytesNeededForState = (uint8_t)OTA2Native32(pSubElement->length);
          break;
        case gZclOtaECDSASignatureTagId_c: 
          clientSession.state = OTAClient_ProcessECDSASigningState_c;
          clientSession.bytesNeededForState = (uint8_t)OTA2Native32(pSubElement->length);
          break;
#endif        
        default:
          clientSession.state = OTAClient_StateMax_c;
          clientSession.bytesNeededForState = 1;   
          break;
      }     
      clientSession.stateBufferIndex = 0;
      break;
    }
  case OTAClient_ProcessUpgradeImageState_c:
#if (!gZclEnableOTAClientECCLibrary_d) 
    /* process image CRC */
    mCrcCompute = OTA_CrcCompute(clientSession.pStateBuffer, clientSession.bytesNeededForState, mCrcCompute);
#endif  
    
    /* New image chunk arrived. upgradeImageLen is updated by the OTA platform component. */
    if(OTA_PushImageChunk(clientSession.pStateBuffer, clientSession.bytesNeededForState, NULL) != gOtaSucess_c)
    {
      return gZclOTAAbort_c;
    }
    subElementLen-=clientSession.bytesNeededForState;
    /* Prepare for next chunk or next state if the image was downloaded */
    if(subElementLen != 0)
    {
      /* More chuncks to come */
      clientSession.bytesNeededForState =  (uint8_t)((subElementLen > SESSION_BUFFER_SIZE) ? SESSION_BUFFER_SIZE : subElementLen);
      clientSession.stateBufferIndex = 0;
    }
    else
    {
      /* We need to move to the next state */
      clientSession.state = OTAClient_ProcessSubElementTagState_c;
      clientSession.bytesNeededForState = sizeof(zclOTAFileSubElement_t);
      clientSession.stateBufferIndex = 0;
    }
    break;  
  case OTAClient_ProcessBitmapState_c:
    pBitmap = MSG_Alloc(clientSession.bytesNeededForState); 
    if(!pBitmap) 
       return gZclOTAAbort_c;
    FLib_MemCpy(pBitmap, clientSession.pStateBuffer, (clientSession.bytesNeededForState));   
#if (!gZclEnableOTAClientECCLibrary_d)
    /* process image CRC */
    mCrcCompute = OTA_CrcCompute(clientSession.pStateBuffer, clientSession.bytesNeededForState, mCrcCompute);
#endif    
    clientSession.state = OTAClient_ProcessSubElementTagState_c;
    clientSession.bytesNeededForState = sizeof(zclOTAFileSubElement_t);
    clientSession.stateBufferIndex = 0;
    clientSession.steps++;
    break;  
#if (!gZclEnableOTAClientECCLibrary_d)   
  case OTAClient_ProcessImgIntegrityCodeState_c:
  case OTAClient_ProcessCRCState_c:
    if(clientSession.state == OTAClient_ProcessCRCState_c) 
    {
      /* check the CRC Value */
      FLib_MemCpy(&crcReceived, clientSession.pStateBuffer, sizeof(crcReceived));
      crcReceived = OTA2Native32(crcReceived);
      if(crcReceived != mCrcCompute)
        return gZclOTAInvalidImage_c; 
    }
#if gZclOtaClientImgIntegrityCodeValidation_d    
    if(clientSession.state == OTAClient_ProcessImgIntegrityCodeState_c)
    {
      /* check the Image Integrity Code Value */
      uint8_t hashReceived[AES_MMO_SIZE];
      FLib_MemCpy(&hashReceived, clientSession.pStateBuffer, AES_MMO_SIZE);
      if(!FLib_MemCmp(mMsgDigest, hashReceived, AES_MMO_SIZE))
        return gZclOTAInvalidImage_c;
    } 
#endif     
#else
  case OTAClient_ProcessECDSASigningState_c:
    /* ECDSA signature Validation */
    if(OtaSignatureVerification((uint8_t*)&clientSession.pStateBuffer[0], mCertReceived, (uint8_t*)&clientSession.pStateBuffer[sizeof(zbIeeeAddr_t)])!= gZbSuccess_c)
       return gZclOTAInvalidImage_c;  
#endif  /* #if(gEccIncluded_d == FALSE)  */ 
    /* We need to close the written image here; commit image has different prototype on ARM7 vs S08 */
    if(clientSession.steps >= 3)
    {
#if (gBigEndian_c != TRUE)
#ifndef PROCESSOR_KINETIS     
      if(OTA_CommitImage(FALSE, *(uint32_t *)(pBitmap)) != gOtaSucess_c) return gZclOTAAbort_c;
#else
      if(OTA_CommitImage(pBitmap) != gOtaSucess_c) return gZclOTAAbort_c;
#endif      
#else
      if(OTA_CommitImage(pBitmap) != gOtaSucess_c) return gZclOTAAbort_c;
#endif    
      MSG_Free(pBitmap); 
    }
    /* Advance to an illegal state. This state machine should not be called again in this upgrade session. */
    clientSession.state = OTAClient_StateMax_c;
    clientSession.bytesNeededForState = 1; 
    clientSession.stateBufferIndex = 0;    
    break;
#if (gZclEnableOTAClientECCLibrary_d == TRUE)	  
  case OTAClient_ProcessECDSASignCertState_c:  
    /* store the certificate */
    FLib_MemCpy(&mCertReceived, clientSession.pStateBuffer, sizeof(IdentifyCert_t));
    /* Prepare for next state */
    clientSession.state = OTAClient_ProcessSubElementTagState_c;
    clientSession.bytesNeededForState = sizeof(zclOTAFileSubElement_t);
    clientSession.stateBufferIndex = 0;
    break;    
#endif /* #if(gEccIncluded_d == TRUE) */   
  case OTAClient_StateMax_c:
  default:
    return gZclOTAAbort_c;
  }  
  return gZbSuccess_c;
}

/******************************************************************************
* OTAClusterClientProcessBlock
*
* Private function. Process a block of received data.
******************************************************************************/
zbStatus_t OTAClusterClientProcessBlock(uint8_t *pImageBlock, uint8_t blockLength)
{
  uint8_t bytesToCopy;
  uint8_t bytesCopied = 0;
  zbStatus_t result = gZbSuccess_c; 
  
#if gZclEnableOTAClientECCLibrary_d 
  uint8_t signatureSize = 2*SECT163K1_SIGNATURE_ELEMENT_LENGTH;
#endif
#if  gZclOtaClientImgIntegrityCodeValidation_d
  uint8_t signatureSize = AES_MMO_SIZE;
#endif  
  
#if (gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d) 
  uint8_t sizeBlock = 2*AES_MMO_SIZE;
  static uint8_t aesMmoBlock[2*AES_MMO_SIZE];
  static uint8_t mPosition = 0;
  static bool_t lastBlockForHash = FALSE;
  
  if(clientSession.fileLength > (clientSession.currentOffset + blockLength + signatureSize))
  {
    while(bytesCopied < blockLength)
    {
        lastBlockForHash=FALSE;
        bytesToCopy = sizeBlock - mPosition;
        if(bytesToCopy > (blockLength - bytesCopied))
        {
            bytesToCopy = (blockLength - bytesCopied);
        }
        FLib_MemCpy(aesMmoBlock+mPosition, pImageBlock+bytesCopied, bytesToCopy);
        bytesCopied +=bytesToCopy;
        mPosition+=bytesToCopy;
        if(mPosition == sizeBlock)
        {
              OTAClusterAesMMO_hash(aesMmoBlock, sizeBlock, lastBlockForHash, clientSession.fileLength-signatureSize, mMsgDigest);
              mPosition = 0;
        }
      }
  }
  else
  {
    if(lastBlockForHash==FALSE)
    {
      uint8_t *lastAesMmoBlock;
      lastBlockForHash = TRUE;
      bytesToCopy = clientSession.fileLength - clientSession.currentOffset - signatureSize;
      sizeBlock = mPosition + bytesToCopy;
      lastAesMmoBlock = MSG_Alloc(sizeBlock); 
      FLib_MemCpy(lastAesMmoBlock, aesMmoBlock, mPosition);
      FLib_MemCpy(lastAesMmoBlock+mPosition, pImageBlock, bytesToCopy);
      OTAClusterAesMMO_hash(lastAesMmoBlock, sizeBlock, lastBlockForHash, clientSession.fileLength-signatureSize, mMsgDigest);
      mPosition = 0;
      MSG_Free(lastAesMmoBlock);
    }
  }
#endif  
  
  bytesCopied = 0;
  while(bytesCopied < blockLength)
  {
    bytesToCopy = clientSession.bytesNeededForState - clientSession.stateBufferIndex;
    if(bytesToCopy > (blockLength - bytesCopied))
    {
      bytesToCopy = (blockLength - bytesCopied);
    }
    FLib_MemCpy(clientSession.pStateBuffer + clientSession.stateBufferIndex, pImageBlock + bytesCopied, bytesToCopy);
    bytesCopied +=bytesToCopy;
    clientSession.stateBufferIndex+=bytesToCopy;
    if(clientSession.stateBufferIndex == clientSession.bytesNeededForState)
    {
      result = OTAClusterClientRunImageProcessStateMachine();
      if(result != gZbSuccess_c) return result;
    }
  }
 
 #if  (gZclEnableOTAProgressReport_d == TRUE) 
  {
	  static uint8_t mSendReport = 0;
	  if((clientSession.currentOffset > clientSession.fileLength/3) && (clientSession.currentOffset < 2*clientSession.fileLength/3))
		  gOtaProgressReportStatus = otaProgress33_c;
	  else
		  if((clientSession.currentOffset > 2*clientSession.fileLength/3) && (clientSession.currentOffset < clientSession.fileLength))
			  gOtaProgressReportStatus = otaProgress66_c;
	  mSendReport++;
	  if(mSendReport%2 == 0)
	  {
		  mSendReport = 0;
		  BeeAppUpdateDevice(0x08, gOTAProgressReportEvent_c, 0, 0, NULL);
	  }
  }	
#endif  
   
#if (gZclEnableOTAProgressReportToExternalApp_d == TRUE)  
  {
    zclZtcImageOtaProgressReport_t ztcOtaProgressReportInf;
    if((clientSession.currentOffset == 0)||((clientSession.currentOffset+blockLength)%(blockLength*100) == 0)||(clientSession.currentOffset+blockLength ==  clientSession.fileLength))
    {
      ztcOtaProgressReportInf.currentOffset = Native2OTA32(clientSession.currentOffset + blockLength);
      ztcOtaProgressReportInf.imageLength = Native2OTA32(clientSession.fileLength);
      Copy2Bytes(ztcOtaProgressReportInf.deviceAddr, NlmeGetRequest(gNwkShortAddress_c));   
#ifndef gHostApp_d  
        ZTCQueue_QueueToTestClient((const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#else
        ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#endif 
    }
  }
#endif   //gZclEnableOTAProgressReportToExternApp_d
  return result;
}


/******************************************************************************
* OTAClusterClientProcessBlockTimerCallback
*
* Timer callback to process block indications
******************************************************************************/
static void OTAClusterClientProcessBlockTimerCallback(uint8_t tmr) 
{
  zbStatus_t          result = gZclUnsupportedClusterCommand_c;    
  (void) tmr;
                                                               
  result = OTAClusterClientProcessBlock(gpBlockCallbackState->blockData, gpBlockCallbackState->blockSize);
  TMR_FreeTimer(gBlockCallbackTimer);
  gBlockCallbackTimer = gTmrInvalidTimerID_c;
  
  if(result != gZbSuccess_c)
  {
     (void)ZCL_OTAClusterClient_EndUpgradeAbortRequest(gpBlockCallbackState->dstAddr,  gpBlockCallbackState->dstEndpoint, result);
     return;
  }
  //Update the transfer info
  clientSession.currentOffset += gpBlockCallbackState->blockSize;
#if gZclClusterOptionals_d
  gOTAAttrs.FileOffset = Native2OTA32(clientSession.currentOffset);
#endif
    
  if(clientSession.currentOffset < clientSession.fileLength)
  {
    //More data to be received - send back a block request
    (void)ZCL_OTAClusterClient_NextBlockRequest(gpBlockCallbackState->dstAddr, gpBlockCallbackState->dstEndpoint);
  }
  else 
  {
    //Save relevant data on the new image
    clientParams.downloadedFileVersion = clientSession.downloadingFileVersion;
#if gZclClusterOptionals_d
    gOTAAttrs.DownloadedFileVersion = Native2OTA32(clientSession.downloadingFileVersion);
#endif    
    gOTAAttrs.ImageUpgradeStatus = gOTAUpgradeStatusDownloadComplete_c; 
    if(mMultipleUpgradeImage != gZclOTARequireMoreImage_c)
      /* All image data received. Issue upgrade end request. */
      (void)ZCL_OTAClusterClient_EndUpgradeRequest(gpBlockCallbackState->dstAddr, gpBlockCallbackState->dstEndpoint, gZclSuccess_c);    
    else
    {
      (void)ZCL_OTAClusterClient_EndUpgradeRequest(gpBlockCallbackState->dstAddr, gpBlockCallbackState->dstEndpoint, gZclOTARequireMoreImage_c);  
      gOTAAttrs.ImageUpgradeStatus = gOTAUpgradeStatusWaitForMore_c;
      /* initiate new client session: init client params; */
      mMultipleUpgradeImage = 0x00;
      gNextImageReqTimer = TMR_AllocateTimer(); 
      if(gNextImageReqTimer != gTmrInvalidTimerID_c)
      {
    	  /* wait 2 second and start new ota upgrade image process */
    	  TMR_StartSecondTimer(gNextImageReqTimer, 2, OTAClusterClientNextImageReqTimerCallback);
      }
    }
#if gEndDevCapability_d || gComboDeviceCapability_d 
    /* stop the OTA FastPoll Mode */
    if(gRetransmitLastBlockTimer == gTmrInvalidTimerID_c)
    	gRetransmitLastBlockTimer = TMR_AllocateTimer(); 
    TMR_StartSecondTimer(gRetransmitLastBlockTimer, 1, OTAClient_UpdatePollRateCallBack);
#else    
    TMR_FreeTimer(gRetransmitLastBlockTimer); 
    gRetransmitLastBlockTimer = gTmrInvalidTimerID_c;
#endif       
  }
 // MSG_Free(gpBlockCallbackState);             
 }

static void OTAClientRetransmitLastBlockTmrCallback(uint8_t tmr) 
{
  zbClusterId_t aClusterId={gaZclClusterOTA_c}; 
  uint8_t iEEEAddress[8];
	
   (void)tmr;
   gRetransmitCounter++;
   if(gRetransmitCounter == gMaxRetransmisionCounter)
   {
    	 OTAClusterClientAbortSession();
    	 return;
   }
    if(ZCL_GetAttribute(gClientOTAEndpoint, aClusterId, gZclAttrOTA_UpgradeServerId_c, gZclClientAttr_c, iEEEAddress, NULL)==gZbSuccess_c)  
    {
    	(void)ZCL_OTAClusterClient_RetransmitBlockRequest(iEEEAddress, clientSession.serverEndpoint); 
    }
}


/******************************************************************************
* ZCL_OTAClusterClient_RetransmitBlockRequest
*
* retransmit last block Request
******************************************************************************/  
static zbStatus_t ZCL_OTAClusterClient_RetransmitBlockRequest 
(
  zbIeeeAddr_t  ieeeAddr,
  zbEndPoint_t  endPoint    
)
{ 
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode64Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode64Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif   
  zclOTABlockRequest_t  blockRequest;

  blockRequest.fieldControl = 0x00;
  if(gZclOTABlockRequest_BlockRequestDelayPresent_c == 0x02)
  {
    blockRequest.fieldControl |= gZclOTABlockRequest_BlockRequestDelayPresent_c;
    blockRequest.blockRequestDelay = Native2OTA16(clientSession.blockRequestDelay);
  }

  blockRequest.manufacturerCode = Native2OTA16(clientParams.manufacturerCode);
  blockRequest.imageType = Native2OTA16(clientParams.imageType);
  blockRequest.fileVersion = Native2OTA32(clientSession.downloadingFileVersion);
  blockRequest.fileOffset = Native2OTA32(clientSession.currentOffset);
  blockRequest.maxDataSize = clientParams.maxDataSize;
  
  /* Create the destination address */
  Copy8Bytes(addrInfo.dstAddr.aIeeeAddr, ieeeAddr);
  addrInfo.dstEndPoint = endPoint;
  addrInfo.srcEndPoint = gClientOTAEndpoint;  
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  return ZCL_OTABlockRequestCmd(addrInfo, blockRequest);
  
}

#endif


#if (gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d || gZclOtaServerImgIntegrityCodeValidation_d)	
/******************************************************************************
* aesHashNextBlock
*
* This function computes the AES-128 of the <tt>input</tt> parameter using 
* the Key <tt>keyInit</tt>, and stores the result in <tt>output</tt>.
******************************************************************************/
void aesHashNextBlock(uint8_t* input, uint8_t *keyInit, uint8_t* output)
{
  uint8_t i=0;
#ifdef __IAR_SYSTEMS_ICC__  
#ifdef PROCESSOR_KINETIS
  uint8_t tempIn[AES_MMO_SIZE],tempKey[AES_MMO_SIZE], tempOut[AES_MMO_SIZE]; 
  uint8_t init_vector[AES_MMO_SIZE];
  
  for(i=0;i<AES_MMO_SIZE; i++)
    init_vector[i] = 0;
  FLib_MemCpy(tempIn, input, AES_MMO_SIZE);
  FLib_MemCpy(tempKey, keyInit, AES_MMO_SIZE);
  FLib_MemCpy(tempOut, output, AES_MMO_SIZE);
#else  
  
  uint32_t tempIn[AES_MMO_SIZE/4],tempKey[AES_MMO_SIZE/4], tempOut[AES_MMO_SIZE/4];
  for(i=0;i<AES_MMO_SIZE/4;i++){
    FLib_MemCpyReverseOrder(&tempIn[i], &input[i*4], AES_MMO_SIZE/4);
    FLib_MemCpyReverseOrder(&tempKey[i], &keyInit[i*4], AES_MMO_SIZE/4);
    FLib_MemCpyReverseOrder(&tempOut[i], &output[i*4], AES_MMO_SIZE/4);
  }
#endif
  
#ifdef PROCESSOR_KINETIS
  encryptAES(tempKey, tempIn, tempOut, AES_MMO_SIZE, init_vector);
  FLib_MemCpy(output, tempOut, AES_MMO_SIZE);
#else  
  (void)Asm_CbcMacEncryptDecrypt(tempKey, NULL, tempIn, tempOut);
  for(i=0;i<AES_MMO_SIZE/4;i++)
    FLib_MemCpyReverseOrder(&output[i*4], &tempOut[i], AES_MMO_SIZE/4);
#endif
#else /*! __IAR_SYSTEMS_ICC__   */
  uint8_t tempIn[AES_MMO_SIZE],tempKey[AES_MMO_SIZE], tempOut[AES_MMO_SIZE]; 
  
  FLib_MemCpy(tempIn, input, AES_MMO_SIZE);
  FLib_MemCpy(tempKey, keyInit, AES_MMO_SIZE);
  FLib_MemCpy(tempOut, output, AES_MMO_SIZE); 
  
  (void)Asm_CbcMacEncryptDecrypt(tempKey, NULL, tempIn, tempOut);
  FLib_MemCpy(output, tempOut, AES_MMO_SIZE);
#endif /*__IAR_SYSTEMS_ICC__  */
  
  
  for(i=0; i<AES_MMO_SIZE; i++)
    output[i] ^=input[i];
}



#ifdef PROCESSOR_KINETIS
/******************************************************************************
* encryptAES
*
* This function computes the AES CBC(Cipher Block Chaining) of the <tt>input_data</tt> parameter using 
* the Key <tt>key</tt> and the  <tt>init_vector</tt>, and stores the result in <tt>output_data</tt>.
******************************************************************************/
static void encryptAES(uint8_t* key,  uint8_t* input_data, uint8_t* output_data, uint8_t data_length, uint8_t* init_vector)
{	
    uint8_t i;
    uint8_t blocks;
    uint8_t rounds;
    uint8_t temp_block[AES_MMO_SIZE];
    uint8_t temp_iv[AES_MMO_SIZE];

    uint8_t key_expansion[AES128_KeyExpansion_d*4];
    
    /*validate data length*/
    if( data_length % AES_MMO_SIZE )
       return ;/*wrong length*/
       
    /*expand AES key*/
    mmcau_aes_set_key(key, AES_MMO_SIZE*8, key_expansion);
       
    /*get number of blocks*/
    blocks = data_length/AES_MMO_SIZE;
    
    /*copy init vector to temp storage*/
    FLib_MemCpy((void*)temp_iv,(void*)init_vector,AES_MMO_SIZE);
    
    do
    {
       /*copy to temp storage*/
       FLib_MemCpy((void*)temp_block,(void*)input_data,AES_MMO_SIZE);
       /*xor for CBC*/
       for (i = 0; i < AES_MMO_SIZE; i++)
           temp_block[i] ^= temp_iv[i];
            
       mmcau_aes_encrypt(temp_block, key_expansion, AES128_ROUNDS, output_data);
       FLib_MemCpy((void*)temp_iv,(void*)output_data,AES_MMO_SIZE);
       input_data += AES_MMO_SIZE;
       output_data += AES_MMO_SIZE;  
    }while(--blocks);

}
#endif

/******************************************************************************
* OTAClusterAesMMO_hash
*
* This function computes the AES MMO hash for OTA Application including also 
* the signature tag, signer IEEE for image and stores the result in <tt>output</tt>.
******************************************************************************/
void OTAClusterAesMMO_hash(uint8_t *blockToProcess, uint8_t length, bool_t lastBlock, uint32_t totalLength, uint8_t *hashValue)
{
  uint8_t temp[AES_MMO_SIZE];
  uint8_t moreDataLength = length;
  
  for (;AES_MMO_SIZE <= moreDataLength; 
        blockToProcess += AES_MMO_SIZE, moreDataLength -= AES_MMO_SIZE)
      aesHashNextBlock(blockToProcess, hashValue, hashValue);
  
  if(lastBlock == TRUE){
      FLib_MemSet(&temp[0], 0x00, AES_MMO_SIZE);
      FLib_MemCpy(&temp[0], &blockToProcess[0], moreDataLength);
      temp[moreDataLength] = 0x80;
    
      if (AES_MMO_SIZE - moreDataLength < 3) {
        aesHashNextBlock(temp, hashValue, hashValue);
        FLib_MemSet(&temp[0], 0x00, AES_MMO_SIZE);
      }
      temp[AES_MMO_SIZE - 2] = (uint8_t)(totalLength >> 5);
      temp[AES_MMO_SIZE - 1] = (uint8_t)(totalLength << 3);
      aesHashNextBlock(temp, hashValue, hashValue); 
  }
  
}
#endif /* (gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d)|| gZclOtaServerImgIntegrityCodeValidation_d*/

#ifdef gZclEnableOTAClient_d
#if gZclEnableOTAClientECCLibrary_d
extern const uint8_t CertAuthPubKey[gZclCmdKeyEstab_CompressedPubKeySize_c];
extern const uint8_t CertAuthIssuerID[8];
zbStatus_t OtaSignatureVerification(uint8_t *signerIEEE,IdentifyCert_t certificate, uint8_t* signature)
{
   uint8_t devicePublicKey_rec[gZclCmdKeyEstab_CompressedPubKeySize_c];
   uint8_t signerIEEE_reverse[8];
   zbIeeeAddr_t signersIEEEList = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
   uint8_t i=0;
   
   for(i=0;i<8;i++)
      signerIEEE_reverse[i] = signerIEEE[8-i-1];
   /*first Step - determine if the signer of the image is an authorized signer*/
   if(Cmp8Bytes(&signersIEEEList[0], &signerIEEE_reverse[0])==FALSE)
     return gZbFailed_c;
   /*Verify the signer IEEE address within the subject field of the ECDSA Certificate */
   if(Cmp8Bytes(&certificate.Subject[0], &signerIEEE_reverse[0])==FALSE)
     return gZbFailed_c;
   /*Test the issuer field of the ECDSA Certificate with a specific list*/
   if(Cmp8Bytes(&certificate.Issuer[0], (uint8_t*)&CertAuthIssuerID[0])==FALSE)
     return gZbFailed_c;
   /*reconstruct public key using certificate and specific CertAuthPubKey*/
   (void)ZSE_ECQVReconstructPublicKey((uint8_t *)&certificate,(uint8_t *)CertAuthPubKey, devicePublicKey_rec,aesMmoHash, NULL, 0);
   /*ECDSA Verification*/
   if(ZSE_ECDSAVerify(devicePublicKey_rec, mMsgDigest,  &signature[0], &signature[SECT163K1_SIGNATURE_ELEMENT_LENGTH],NULL,0)!=gZbSuccess_c)
     return gZbFailed_c;
   /*if all ok return gZbSuccess_c*/
   return gZbSuccess_c;
}
#endif /* gZclEnableOTAClientECCLibrary_d */
#endif /* gZclEnableOTAClient_d*/


zbStatus_t OTA_InitiateOtaClusterProcess 
(
    zclInitiateOtaProcess_t* initiateOtaProcess
)
{
#if (gZclEnableOTAClient_d)
#if gNum_EndPoints_c != 0     
  uint8_t i = 0;
  otaCluster_simpleDescriptor.endPoint =  initiateOtaProcess->clientInit.endPoint;     
  for(i=0; i< gNum_EndPoints_c; ++i) 
  {
      if(endPointList[i].pEndpointDesc->pSimpleDesc->endPoint == initiateOtaProcess->clientInit.endPoint) 
         Copy2Bytes(&otaCluster_simpleDescriptor.aAppProfId, endPointList[i].pEndpointDesc->pSimpleDesc->aAppProfId);     
  }  
  /*wait 1 second to send Match_Desc_req*/
  TMR_FreeTimer(gNextImageReqTimer);
  gNextImageReqTimer = TMR_AllocateTimer();	
  if(gNextImageReqTimer == gTmrInvalidTimerID_c)
     	return FALSE;
  TMR_StartSecondTimer(gNextImageReqTimer, 1, OTAClusterClientMatchDescReqTimerCallback);

#endif  //gNum_EndPoints_c
#endif  //gZclEnableOTAClient_d
#if gZclEnableOTAServer_d    
  mOtaServerInitParams.querryJitter = initiateOtaProcess->serverInit.querryJitter;
  mOtaServerInitParams.currentTime = OTA2Native32(initiateOtaProcess->serverInit.currentTime); 
  mOtaServerInitParams.upgradeRequestTime = OTA2Native32(initiateOtaProcess->serverInit.upgradeRequestTime);
  mOtaServerInitParams.upgradeTime = OTA2Native32(initiateOtaProcess->serverInit.upgradeTime);
  mOtaServerInitParams.blockRequestDelay = OTA2Native16(initiateOtaProcess->serverInit.blockRequestDelay);
#endif
 return gZbSuccess_c;
}

#if gZclEnableOTAClient_d 

bool_t InterpretOtaMatchDescriptor(zbNwkAddr_t  aDestAddress, uint8_t endPoint)
{

  if(mSendOtaMatchDescriptorRequest == TRUE)
  {
    gOTAAttrs.ImageUpgradeStatus = gOTAUpgradeStatusNormal_c;
#if gZclClusterOptionals_d
    gOTAAttrs.FileOffset = 0;
    gOTAAttrs.MinimumBlockReqDelay = 0;
    gOTAAttrs.CurrentZigBeeStackVersion = NlmeGetRequest(gNwkStackProfile_c);
    gOTAAttrs.DownloadedZigBeeStackVersion = NlmeGetRequest(gNwkStackProfile_c);
#endif
    /*Send IEEE_Addr_Req*/
    mSendOtaMatchDescriptorRequest = FALSE;
    clientSession.serverEndpoint = endPoint;
    ASL_IEEE_addr_req(NULL, aDestAddress, aDestAddress, 0x00, 0x00);
    mSendOTAIEEEAddrRequest = TRUE;
    return TRUE;
  }
  return FALSE;
}

void InterpretOtaIEEEAddrReq(zbIeeeAddr_t  aIeeeAddr)
{
  if(mSendOTAIEEEAddrRequest == TRUE)
  {
    FLib_MemCpy(gOTAAttrs.UpgradeServerId, aIeeeAddr, 8);
    mSendOTAIEEEAddrRequest = FALSE;
    gNextImageReqTimer = TMR_AllocateTimer(); 
    if(gNextImageReqTimer == gTmrInvalidTimerID_c)
    	return;	
    OTAClusterClientNextImageReqTimerCallback(gNextImageReqTimer);
  }
}


/******************************************************************************
ZCL_StartClientNextImageTransfer - used only if you discovered a valid OTA Server
*****************************************************************************/
zbStatus_t OTA_OtaStartClientNextImageTransfer 
(
    zclStartClientNextImageTransfer_t* startClientNextImageTransfer
)
{
  zbStatus_t status = gZbSuccess_c;
 
  zbClusterId_t clusterId = {gaZclClusterOTA_c};          
  
  if(clientSession.sessionStarted)
    return gZbFailed_c;
      
  gClientOTAEndpoint = ZCL_GetEndPointForSpecificCluster(clusterId, FALSE);
     
  /* required EndRequest Status*/
  mMultipleUpgradeImage = startClientNextImageTransfer->multipleUpgradeImage;
  /* is valid upgradeServerId attribute? */
  if(Cmp8BytesToFs(gOTAAttrs.UpgradeServerId))
  {
      zclInitiateOtaProcess_t initServerDiscovery = {0, 0};
      initServerDiscovery.isServer = FALSE;
      initServerDiscovery.clientInit.endPoint = gClientOTAEndpoint;
      (void)OTA_InitiateOtaClusterProcess(&initServerDiscovery);
      return gZbFailed_c;
  }
 
  /* send a Query Next Image Request */ 
  status = ZCL_OTAClusterClient_QueryNextImageRequest(gOTAAttrs.UpgradeServerId, clientSession.serverEndpoint);  
 
  return status;
}

static void OTAClusterClientNextImageReqTimerCallback(uint8_t tmr) 
{
  zbClusterId_t aClusterId={gaZclClusterOTA_c}; 
  uint8_t iEEEAddress[8];
  (void)tmr;
  
  if(!clientSession.sessionStarted)  
  {
    gClientOTAEndpoint = ZCL_GetEndPointForSpecificCluster(aClusterId, FALSE);
    if(ZCL_GetAttribute(gClientOTAEndpoint, aClusterId, gZclAttrOTA_UpgradeServerId_c, gZclClientAttr_c, iEEEAddress, NULL) == gZbSuccess_c)
    {
        (void)ZCL_OTAClusterClient_QueryNextImageRequest(iEEEAddress, clientSession.serverEndpoint);  	
    }
  }
   TMR_StartMinuteTimer(gNextImageReqTimer, gMinforNextImageRequest_c, OTAClusterClientNextImageReqTimerCallback);
}


/******************************************************************************
* ZCL_OTAClusterClient_QueryNextImageRequest
*
* Sends back a next image request (may be as a result of Image Notify)
******************************************************************************/  
static zbStatus_t ZCL_OTAClusterClient_QueryNextImageRequest 
(
  zbIeeeAddr_t  ieeeAddr,
  zbEndPoint_t  endPoint    
)
{
  zclOTANextImageRequest_t nextImageRequest;     
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode64Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode64Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif   
  uint8_t len = sizeof(zclOTANextImageRequest_t);  
  
  nextImageRequest.fieldControl = gZclOTANextImageRequest_HwVersionPresent;
  nextImageRequest.manufacturerCode = Native2OTA16(clientParams.manufacturerCode);
  nextImageRequest.imageType = Native2OTA16(clientParams.imageType);
  nextImageRequest.fileVersion = Native2OTA32(clientParams.currentFileVersion);
  nextImageRequest.hardwareVersion = Native2OTA16(clientParams.hardwareVersion);
  
  
  if(!(nextImageRequest.fieldControl & gZclOTANextImageRequest_HwVersionPresent)) 
    len -=sizeof(uint16_t); /* remove hardware version from packet */ 
  
  /* Create the destination address */
  FLib_MemCpy(addrInfo.dstAddr.aIeeeAddr, ieeeAddr, sizeof(zbIeeeAddr_t));
  addrInfo.dstEndPoint = endPoint;
  addrInfo.srcEndPoint = gClientOTAEndpoint; 
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  return ZCL_OTAImageRequestCmd(addrInfo, nextImageRequest);
}

/******************************************************************************
* OTAClusterClientMatchDescReqTimerCallback
*
* parts of Server discovery procedure
******************************************************************************/ 
static void OTAClusterClientMatchDescReqTimerCallback(uint8_t tmr) 
{
  zbNwkAddr_t  aDestAddress = {0xFD, 0xFF};
  (void)tmr;

  ASL_MatchDescriptor_req(NULL,(uint8_t *)aDestAddress, (zbSimpleDescriptor_t*)&otaCluster_simpleDescriptor); 
  mSendOtaMatchDescriptorRequest = TRUE;
  TMR_FreeTimer(gNextImageReqTimer);
}

#endif //gZclEnableOTAClient_d

#endif
