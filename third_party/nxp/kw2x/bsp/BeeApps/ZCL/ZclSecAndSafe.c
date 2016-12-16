/*! @file 	  ZclSecAndSafe.c
 *
 * @brief	  This source file describes specific functionality implemented
 *			  for ZCL Security and Safety domain.
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

/* 
 *  [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
 *  [R2] - docs-07-5123-04-0afg-zigbee-cluster-library-specification.pdf 
 */
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "ApsMgmtInterface.h"

#include "zcl.h"
#include "ZclSecAndSafe.h"
#include "HaProfile.h"


/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private prototypes
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

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/
#define gIASApp_ZoneTypeId_d 13
static uint16_t mZoneType[gIASApp_ZoneTypeId_d] = 
{
  gZclZoneType_StandardCIE_c, gZclZoneType_MotionSensor_c, gZclZoneType_ContactSwitch_c,
  gZclZoneType_FireSensor_c, gZclZoneType_WaterSensor_c, gZclZoneType_GasSensor_c,
  gZclZoneType_PersonalEmergencyDevice_c, gZclZoneType_VibrationMovement_c,
  gZclZoneType_RemoteControl_c, gZclZoneType_KeyFob_c, gZclZoneType_Keypad_c,
  gZclZoneType_StandardWarning_c, gZclZoneType_InvalidZone_c
};

extern tmrTimerID_t gApplicationTmr;
extern zclCmdIASWD_StartWarning_t gStartWarningPayload;
extern zclCmdIASWD_WarningInf_t gWarningInf;
uint8_t gCIEApp_CurrentArmMode = SquawkMode_SystemArmed;


/******************************
  IAS Zone Cluster 
  See ZCL Specification Section 8.2
*******************************/
/* IAS Zone Cluster Attribute Definitions */
const zclAttrDef_t gaZclIASZoneClusterAttrDef[] = {
  /*Attributes of the Zone Information attribute set */
  { gZclAttrIdZoneInformationZoneState_c,  gZclDataTypeEnum8_c,   gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),  (void *) MbrOfs(zclIASZoneAttrsRAM_t, zoneState)},
  { gZclAttrIdZoneInformationZoneType_c,   gZclDataTypeEnum16_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) MbrOfs(zclIASZoneAttrsRAM_t, zoneType)},
  { gZclAttrIdZoneInformationZoneStatus_c, gZclDataTypeBitmap16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) MbrOfs(zclIASZoneAttrsRAM_t, zoneStatus)},
  /*Attributes of the Zone Settings attribute set */
  { gZclAttrIdZoneSettingsIASCIEAddress_c, gZclDataTypeIeeeAddr_c,   gZclAttrFlagsInRAM_c, sizeof(IEEEaddress_t), (void *) MbrOfs(zclIASZoneAttrsRAM_t, IASCIEAddress)},
  { gZclAttrIdZoneSettingsZoneId_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) MbrOfs(zclIASZoneAttrsRAM_t, zoneId)}
};

const zclAttrSet_t gaZclIASZoneClusterAttrSet[] = {
  {gZclAttrSetIASZone_c, (void *)&gaZclIASZoneClusterAttrDef, NumberOfElements(gaZclIASZoneClusterAttrDef)}
};

const zclAttrSetList_t gZclIASZoneClusterAttrSetList = {
  NumberOfElements(gaZclIASZoneClusterAttrSet),
  gaZclIASZoneClusterAttrSet
};


const zclCmd_t gaZclIASZoneClusterCmdReceivedDef[]={
  // commands received   
  gZclCmdRxIASZone_ZoneEnrollResponse_c
};

const zclCmd_t gaZclIASZoneClusterCmdGeneratedDef[]={
  // commands generated 
  gZclCmdTxIASZone_ZoneStatusChange_c,
  gZclCmdTxIASZone_ZoneEnrollRequest_c
};

const zclCommandsDefList_t gZclIASZoneClusterCommandsDefList =
{
   NumberOfElements(gaZclIASZoneClusterCmdReceivedDef), gaZclIASZoneClusterCmdReceivedDef,
   NumberOfElements(gaZclIASZoneClusterCmdGeneratedDef), gaZclIASZoneClusterCmdGeneratedDef
};

/******************************
  IAS ACE Cluster 
  See ZCL Specification Section 8.3
*******************************/
/*No attributes are defined for this cluster.*/


/******************************
  IAS Warning Device Cluster 
  See ZCL Specification Section 8.4
*******************************/
/* IAS Warning Device Cluster Attribute Definitions */
const zclAttrDef_t gaZclIASWDClusterAttrDef[] = {
  { gZclAttrIASWDIdMaxDuration_c, gZclDataTypeUint16_c,  gZclAttrFlagsInRAM_c, sizeof(uint16_t), (void *) MbrOfs(zclIASWDAttrsRAM_t, maxDuration)},
  };

const zclAttrSet_t gaZclIASWDClusterAttrSet[] = {
  {gZclAttrIASWDSet_c, (void *)&gaZclIASWDClusterAttrDef, NumberOfElements(gaZclIASWDClusterAttrDef)}
};

const zclAttrSetList_t gZclIASWDClusterAttrSetList = {
  NumberOfElements(gaZclIASWDClusterAttrSet),
  gaZclIASWDClusterAttrSet
};


const zclCmd_t gaZclIASWDClusterCmdReceivedDef[]={
  // commands received   
  gZclCmdRxIASWD_StartWarning_c,
  gZclCmdRxIASWD_Squawk_c
};


const zclCommandsDefList_t gZclIASWDClusterCommandsDefList =
{
   NumberOfElements(gaZclIASWDClusterCmdReceivedDef), gaZclIASWDClusterCmdReceivedDef,
   0, NULL
};



const zclCmd_t gaZclIASACEClusterCmdReceivedDef[]={
  // commands received   
  gZclCmdRxIASACE_Arm_c,
  gZclCmdRxIASACE_Bypass_c,
  gZclCmdRxIASACE_Emergency_c,
  gZclCmdRxIASACE_Fire_c,
  gZclCmdRxIASACE_Panic_c,
  gZclCmdRxIASACE_GetZoneIDMap_c,
  gZclCmdRxIASACE_GetZoneInformation_c

};

const zclCmd_t gaZclIASACEClusterCmdGeneratedDef[]={
  // commands generated 
  gZclCmdTxIASACE_ArmRsp_c,
  gZclCmdTxIASACE_GetZoneIDMApRsp_c,
  gZclCmdTxIASACE_GetZoneInfRsp_c,
  gZclCmdTxIASACE_ZoneStatusChanged_c,
  gZclCmdTxIASACE_PanelStatusChanged_c
};

const zclCommandsDefList_t gZclIASACEClusterCommandsDefList =
{
   NumberOfElements(gaZclIASACEClusterCmdReceivedDef), gaZclIASACEClusterCmdReceivedDef,
   NumberOfElements(gaZclIASACEClusterCmdGeneratedDef), gaZclIASACEClusterCmdGeneratedDef
};

/******************************
  IAS Zone Cluster 
  See ZCL Specification Section 8.2
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_IASZoneClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS Zone Cluster Client. 
 *
 */
zbStatus_t ZCL_IASZoneClusterClient
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zbStatus_t status = gZclSuccessDefaultRsp_c;
    uint8_t event = 0, i = 0, j = 0;
    uint8_t *pIEEE, aExtAddr[8];
    
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;   
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;    
    
    /* handle the command */
    switch( pFrame->command)
    {
        case gZclCmdTxIASZone_ZoneStatusChange_c:
          {
           zclCmdIASZone_ZoneStatusChange_t changeNotif;
           FLib_MemCpy(&changeNotif ,(pFrame + 1), sizeof(zclCmdIASZone_ZoneStatusChange_t));   
           pIEEE = APS_GetIeeeAddress((uint8_t*)&pIndication->aSrcAddr, aExtAddr);
           /* zoneId field represent the index in the CIE's zone table */
           if(FLib_MemCmp(&gTableZone[changeNotif.ZoneId].ZoneAddress, pIEEE, 8) == TRUE)
           {
               /* update zone status information */
              gTableZone[changeNotif.ZoneId].ZoneStatus = changeNotif.ZoneStatus;
              event = gZclUI_ChangeNotification;
           }
           BeeAppUpdateDevice(pIndication->dstEndPoint, event, 0, 0, NULL);
           return status;
          }
        case gZclCmdTxIASZone_ZoneEnrollRequest_c:  
          {    
           zclCmdIASZone_ZoneEnrollRequest_t cmdPayload;
           zclIASZone_ZoneEnrollResponse_t *pReq; 
           FLib_MemCpy(&cmdPayload ,(pFrame + 1), sizeof(zclCmdIASZone_ZoneEnrollRequest_t));
           pReq = MSG_Alloc(sizeof(zclIASZone_ZoneEnrollResponse_t)); 
           pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
           Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, pIndication->aSrcAddr); 
           pReq->addrInfo.dstEndPoint = pIndication->srcEndPoint;
           pReq->addrInfo.srcEndPoint = pIndication->dstEndPoint;
           pReq->addrInfo.txOptions = 0;
           pReq->addrInfo.radiusCounter = afDefaultRadius_c;
           pIEEE = APS_GetIeeeAddress((uint8_t*)&pIndication->aSrcAddr, aExtAddr);
           /* verify if the IEEEAddress exist already in table */
           for(i=0; i <=gIndexTableZone; i++)
           {
             if(FLib_MemCmp(&gTableZone[i].ZoneAddress, pIEEE, 8) == TRUE)
             {
                pReq->cmdFrame.ZoneID = gTableZone[i].ZoneID;
                pReq->cmdFrame.EnrollResponseCode =  gEnrollResponseCode_Succes_c;
                event = gZclUI_EnrollSuccesfull_c; 
                break;
             }
             else
             {
                if(i == gIndexTableZone)
                {
                  pReq->cmdFrame.ZoneID = gIndexTableZone;
                  /* verify the index in the zone table */
                  if(gIndexTableZone < gIASApp_MaxSupportedZones_d)
                  {
                      /*verify the zone_type*/
                      for(j=0; j< gIASApp_ZoneTypeId_d; j++)
                      
                        if(mZoneType[j] == cmdPayload.ZoneType)
                        {
                            pReq->cmdFrame.EnrollResponseCode =  gEnrollResponseCode_Succes_c;
                            /*store the device in the gZoneTable*/
                            gTableZone[gIndexTableZone].ZoneID = gIndexTableZone;
                            gTableZone[gIndexTableZone].ZoneType = cmdPayload.ZoneType;  
                            Copy8Bytes((uint8_t *)&gTableZone[gIndexTableZone].ZoneAddress, aExtAddr);
                            gTableZone[gIndexTableZone].Endpoint = pIndication->srcEndPoint;
                            /*update index for table zone*/
                            gIndexTableZone++;
                            event = gZclUI_EnrollSuccesfull_c; 
                            break;
                        }
                        else
                        {
                            if(j == gIASApp_ZoneTypeId_d - 1)
                            {
                                pReq->cmdFrame.EnrollResponseCode =  gEnrollResponseCode_NoEnrollPermit_c;
                                event = gZclUI_EnrollFailed_c; 
                            }
                        }
                    
                  }            
                  else
                  {
                      pReq->cmdFrame.EnrollResponseCode = gEnrollResponseCode_TooManyZones_c; 
                      event = gZclUI_EnrollFailed_c; 
                  }
                  break;
              }
            }
           }
           status = IASZone_ZoneEnrollResponse(pReq);       
           BeeAppUpdateDevice(pIndication->dstEndPoint, event, 0, 0, NULL);
           MSG_Free(pReq);       
           return status;
          }
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t ZCL_IASZoneClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS Zone Cluster Server. 
 *
 */
zbStatus_t ZCL_IASZoneClusterServer
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    uint8_t event = gZclUI_NoEvent_c;
    zbClusterId_t aClusterId;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    Copy2Bytes(aClusterId, pIndication->aClusterId);
    
    switch( pFrame->command)
    {
        case gZclCmdRxIASZone_ZoneEnrollResponse_c:  
        {
          zclCmdIASZone_ZoneEnrollResponse_t cmdPayload;  
          uint8_t state;
          FLib_MemCpy(&cmdPayload ,(pFrame + 1), sizeof(zclCmdIASZone_ZoneEnrollResponse_t)); 
          if(cmdPayload.EnrollResponseCode == gZclSuccess_c)
          {
            event = gZclUI_EnrollSuccesfull_c;
            state = gZclZoneState_Enrolled_c;
            (void)ZCL_SetAttribute(pIndication->dstEndPoint, aClusterId, gZclAttrZoneSettingsZoneId_c, gZclServerAttr_c, &cmdPayload.ZoneID);
          }
          else 
          {
             event = gZclUI_EnrollFailed_c;
             state = gZclZoneState_NotEnrolled_c;    
          }
          (void)ZCL_SetAttribute(pIndication->dstEndPoint, aClusterId, gZclAttrZoneInformationZoneState_c, gZclServerAttr_c, &state);
          BeeAppUpdateDevice(0, event, 0, 0, NULL);
          return (pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)?gZclSuccess_c:gZclSuccessDefaultRsp_c;        
          
        }
        default:
          return gZclUnsupportedClusterCommand_c;
    }

}

/*!
 * @fn 		zbStatus_t IASZone_ZoneStatusChange(zclIASZone_ZoneStatusChange_t *pReq) 
 *
 * @brief	Sends over-the-air a ZoneStatusChange command from the IAS Zone Cluster Server. 
 *
 */
zbStatus_t IASZone_ZoneStatusChange
(
    zclIASZone_ZoneStatusChange_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASZone_c);	
    return ZCL_SendServerReqSeqPassed(gZclCmdTxIASZone_ZoneStatusChange_c, sizeof(zclCmdIASZone_ZoneStatusChange_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void ZCL_SendStatusChangeNotification(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, uint16_t zoneStatusChangeNotif) 
 *
 * @brief	Helper function to create and send ZoneStatusChange frame over the air
 *
 */
void ZCL_SendStatusChangeNotification(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, uint16_t zoneStatusChangeNotif) 
{
    zclIASZone_ZoneStatusChange_t *pReq;
    zbClusterId_t aClusterId={gaZclClusterIASZone_c}; 
    uint8_t zoneId;
    //uint8_t  *aDstAddr;
    uint8_t aNwkAddrCpy[2];
    uint8_t iEEEAddress[8];
    pReq = MSG_Alloc(sizeof(zclIASZone_ZoneStatusChange_t)); 
     
    if(pReq) 
    {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        (void)ZCL_GetAttribute(SrcEndpoint, aClusterId, gZclAttrZoneSettingsIASCIEAddress_c,  gZclServerAttr_c, iEEEAddress, NULL);
        (void)APS_GetNwkAddress(iEEEAddress, aNwkAddrCpy);
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, aNwkAddrCpy);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame.ZoneStatus = zoneStatusChangeNotif;
        pReq->cmdFrame.ExtendedStatus = 0x00;
        (void)ZCL_SetAttribute(SrcEndpoint, aClusterId, gZclAttrZoneInformationZoneStatus_c, gZclServerAttr_c, &zoneStatusChangeNotif);
        (void)ZCL_GetAttribute(SrcEndpoint, aClusterId, gZclAttrZoneSettingsZoneId_c,  gZclServerAttr_c, &zoneId, NULL);
        pReq->cmdFrame.ZoneId = zoneId;
        pReq->cmdFrame.Delay = (zoneStatusChangeNotif & Mask_IASZone_SupervisionReports)?12:0; /* 12 = 3 seconds - shoud check the gIntervalReportStatus*/
        (void)IASZone_ZoneStatusChange(pReq);
        MSG_Free(pReq);
    }
}

/*!
 * @fn 		zbStatus_t IASZone_ZoneEnrollRequest(zclIASZone_ZoneEnrollRequest_t *pReq) 
 *
 * @brief	Sends over-the-air a ZoneEnrollRequest command from the IAS Zone Cluster Server. 
 *
 */
zbStatus_t IASZone_ZoneEnrollRequest
(
    zclIASZone_ZoneEnrollRequest_t *pReq
)
{ 
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASZone_c); 
    return ZCL_SendServerReqSeqPassed(gZclCmdTxIASZone_ZoneEnrollRequest_c,sizeof(zclCmdIASZone_ZoneEnrollRequest_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void ZCL_SendZoneEnrollRequest(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr) 
 *
 * @brief	Helper function to create and send enrollRequest frame over the air. 
 *
 */
void ZCL_SendZoneEnrollRequest(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint,  zbNwkAddr_t DstAddr) 
{
    zclIASZone_ZoneEnrollRequest_t *pReq;
    uint16_t zoneType;
    uint8_t aExtAddr[8];
    zbClusterId_t aClusterId={gaZclClusterIASZone_c}; 
   
    pReq = MSG_Alloc(sizeof(zclIASZone_ZoneEnrollRequest_t)); 
     
    if(pReq) {
        (void)APS_GetIeeeAddress(DstAddr, aExtAddr);
        (void)ZCL_SetAttribute(SrcEndpoint, aClusterId, gZclAttrZoneSettingsIASCIEAddress_c, gZclServerAttr_c, aExtAddr);
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        (void)ZCL_GetAttribute(SrcEndpoint, aClusterId, gZclAttrZoneInformationZoneType_c , gZclServerAttr_c, &zoneType, NULL);
        pReq->cmdFrame.ZoneType = zoneType;
        pReq->cmdFrame.ManufacturerCode = 0x210;
#if (TRUE == gBigEndian_c)
        pReq->cmdFrame.ManufacturerCode = Swap2Bytes( pReq->cmdFrame.ManufacturerCode);
#endif
        (void) IASZone_ZoneEnrollRequest(pReq);
        
        MSG_Free(pReq);
    }
}


/*!
 * @fn 		zbStatus_t IASZone_ZoneEnrollResponse(zclIASZone_ZoneEnrollResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ZoneEnrollResponse command from the IAS Zone Cluster Client. 
 *
 */
zbStatus_t IASZone_ZoneEnrollResponse
(
    zclIASZone_ZoneEnrollResponse_t *pReq
)  
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASZone_c);	
    return ZCL_SendClientRspSeqPassed(gZclCmdRxIASZone_ZoneEnrollResponse_c,sizeof(zclCmdIASZone_ZoneEnrollResponse_t),(zclGenericReq_t *)pReq);
}



/******************************
  IAS ACE Cluster 
  See ZCL Specification Section 8.3
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_IASACEClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS ACE Cluster Client. 
 *
 */
zbStatus_t ZCL_IASACEClusterClient
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;
    zbStatus_t status = gZclSuccessDefaultRsp_c;    
    
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    
	if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	   status = gZclSuccess_c;    
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
       case gZclCmdTxIASACE_ArmRsp_c:
         {
          zclCmdIASACE_ArmRsp_t cmdPayload;
          FLib_MemCpy(&cmdPayload ,(pFrame + 1), sizeof(zclCmdIASACE_ArmRsp_t)); 
          if(cmdPayload.ArmNotification <= gArmNotif_AllZonesArmed_c) /* gArmNotif_AllZoneDisarm_c, gArmNotif_DayHomeZoneArmed_c, gArmNotif_NightSleepZoneArmed_c, gArmNotif_AllZonesArmed_c*/        
            return status;
          else
            return gZbFailed_c;
         }
       case gZclCmdTxIASACE_GetZoneIDMApRsp_c:      
       case gZclCmdTxIASACE_GetZoneInfRsp_c:
       case gZclCmdTxIASACE_ZoneStatusChanged_c:
       case gZclCmdTxIASACE_PanelStatusChanged_c:  	   
         return status;
       default:
          return gZclUnsupportedClusterCommand_c;
    }

}

/*!
 * @fn 		zbStatus_t ZCL_IASACEClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS ACE Cluster Server. 
 *
 */
zbStatus_t ZCL_IASACEClusterServer
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zbStatus_t status = gZclSuccessDefaultRsp_c;
    uint8_t i=0,j=0;
    
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;    
    /* handle the command */
    switch( pFrame->command)
    {
        case gZclCmdRxIASACE_Arm_c:
        {
           zclCmdIASACE_Arm_t cmdPayload;
           zclIASACE_ArmRsp_t *pReq;
           FLib_MemCpy(&cmdPayload ,(pFrame + 1), sizeof(zclCmdIASACE_Arm_t));  /*Command = ArmMode */
           pReq = MSG_Alloc(sizeof(zclIASACE_ArmRsp_t));
           if(!pReq)
            return gZclNoMem_c; 
           pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
           Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr,pIndication->aSrcAddr); 
           pReq->addrInfo.dstEndPoint = pIndication->srcEndPoint;
           pReq->addrInfo.srcEndPoint = pIndication->dstEndPoint;
           pReq->addrInfo.txOptions = 0;
           pReq->addrInfo.radiusCounter = afDefaultRadius_c;
           /*send arm command to devices (armmode = arm notification)*/                  
           pReq->cmdFrame.ArmNotification = cmdPayload.ArmMode;    
           status = IASACE_ArmRsp(pReq);  
           MSG_Free(pReq); 
           
           gCIEApp_CurrentArmMode = cmdPayload.ArmMode;
           /* search a Warning device to send  squawk command */
           if(gApplicationTmr == gTmrInvalidTimerID_c)
           {
             gApplicationTmr = TMR_AllocateTimer(); 
             if(gApplicationTmr == gTmrInvalidTimerID_c) 
               return gZclNoMem_c;
             TMR_StartTimer(gApplicationTmr, gTmrSingleShotTimer_c, 100, ZCL_SendSquawkCmdCallback);
           }
           return status;
        }
        case gZclCmdRxIASACE_GetZoneIDMap_c: 
        {
           zclIASACE_GetZoneIDMApRsp_t *pReqZoneMap;
           pReqZoneMap = MSG_Alloc(sizeof(zclIASACE_GetZoneIDMApRsp_t)); 
           pReqZoneMap->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
           Copy2Bytes(pReqZoneMap->addrInfo.dstAddr.aNwkAddr,pIndication->aSrcAddr); 
           pReqZoneMap->addrInfo.dstEndPoint = pIndication->srcEndPoint;
           pReqZoneMap->addrInfo.srcEndPoint = pIndication->dstEndPoint;
           pReqZoneMap->addrInfo.txOptions = 0;
           pReqZoneMap->addrInfo.radiusCounter = afDefaultRadius_c;
           for(i=0;i<16;i++) /* for  zone section */
           {
             pReqZoneMap->cmdFrame.ZoneIDMapSection[i] = 0x0000;
             for(j=0;j<16;j++)/* for zone bit section */
             {
               if((gIndexTableZone-1) >= (i*16+j))
                  pReqZoneMap->cmdFrame.ZoneIDMapSection[i] |=  (0x0001 << j); 
               else
                  break;  
             }
#if (TRUE == gBigEndian_c)
               pReqZoneMap->cmdFrame.ZoneIDMapSection[i] = Swap2Bytes(pReqZoneMap->cmdFrame.ZoneIDMapSection[i]);
#endif
           }
           status = IASACE_GetZoneIDMapRsp(pReqZoneMap);   
           MSG_Free(pReqZoneMap);       
           return status;      
        }
        case gZclCmdRxIASACE_GetZoneInformation_c: 
          {  
           zclIASACE_GetZoneInfRsp_t *pReqZoneInf;
           zclCmdIASACE_GetZoneInformation_t cmdPayload;
           FLib_MemCpy(&cmdPayload ,(pFrame + 1), sizeof(zclCmdIASACE_GetZoneInformation_t));  /*Command = ZoneID */
           pReqZoneInf = MSG_Alloc(sizeof(zclIASACE_GetZoneInfRsp_t)); 
           pReqZoneInf->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
           Copy2Bytes(pReqZoneInf->addrInfo.dstAddr.aNwkAddr,pIndication->aSrcAddr); 
           pReqZoneInf->addrInfo.dstEndPoint = pIndication->srcEndPoint;
           pReqZoneInf->addrInfo.srcEndPoint = pIndication->dstEndPoint;
           pReqZoneInf->addrInfo.txOptions = 0;
           pReqZoneInf->addrInfo.radiusCounter = afDefaultRadius_c;
           /* obtain zone information from table zone */   
           if(cmdPayload.ZoneID < gIndexTableZone)
           {
        	   pReqZoneInf->cmdFrame.ZoneID       =   gTableZone[cmdPayload.ZoneID].ZoneID;   
        	   pReqZoneInf->cmdFrame.ZoneType     =   gTableZone[cmdPayload.ZoneID].ZoneType; 
        	   Copy8Bytes((uint8_t *)&pReqZoneInf->cmdFrame.IEEEAddress, (uint8_t *)&gTableZone[cmdPayload.ZoneID].ZoneAddress);
           }
           else
           { 
        	   pReqZoneInf->cmdFrame.ZoneID       =   0xFF;   
        	   pReqZoneInf->cmdFrame.ZoneType     =   0xFFFF; 
        	   pReqZoneInf->cmdFrame.IEEEAddress.AddressHigh	=   0xFFFFFFFF;
        	   pReqZoneInf->cmdFrame.IEEEAddress.AddressLow	  	=   0xFFFFFFFF;     	   
           }
           status =  IASACE_GetZoneInformationRsp(pReqZoneInf);       
           MSG_Free(pReqZoneInf);   
           return status;
          }
        case gZclCmdRxIASACE_Bypass_c:
           return status;
        case gZclCmdRxIASACE_Fire_c:
        case gZclCmdRxIASACE_Emergency_c:
        case gZclCmdRxIASACE_Panic_c: 
         {
           if( pFrame->command == gZclCmdRxIASACE_Fire_c)
            gStartWarningPayload.WarningModeStrobeSirenLevel.WarningMode = WarningMode_Fire;
           else
            gStartWarningPayload.WarningModeStrobeSirenLevel.WarningMode = WarningMode_Emergency; 
           gStartWarningPayload.WarningModeStrobeSirenLevel.Strobe = Strobe_StrobeParallel;
           gStartWarningPayload.WarningModeStrobeSirenLevel.SirenLevel = SirenLevel_Medium;
           gStartWarningPayload.StrobeDutyCycle = 50;
           gStartWarningPayload.StrobeLevel = StrobeLevel_Medium;
           gStartWarningPayload.WarningDuration = 0x1F;
           gStartWarningPayload.WarningDuration = OTA2Native16(gStartWarningPayload.WarningDuration);
           /* search a Warning device to send  StartWarning command */
           if(gApplicationTmr == gTmrInvalidTimerID_c)
           {
             gApplicationTmr = TMR_AllocateTimer(); 
             if(gApplicationTmr == gTmrInvalidTimerID_c) 
               return gZclNoMem_c;
             TMR_StartTimer(gApplicationTmr, gTmrSingleShotTimer_c, 100, ZCL_SendWarningCmdCallback);
           }
           return status;
         }
        default:
           return gZclUnsupportedClusterCommand_c;
    }

}

/*!
 * @fn 		zbStatus_t IASACE_Arm(zclIASACE_Arm_t *pReq) 
 *
 * @brief	Sends over-the-air an Arm command  from the IAS ACE Cluster Client. 
 *
 */                       
zbStatus_t IASACE_Arm
(
    zclIASACE_Arm_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_Arm_c, sizeof(zclCmdIASACE_Arm_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void ZCL_SendArmCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t armMode) 
 *
 * @brief	Helper function to create and send an Arm command over the air
 *
 */
void ZCL_SendArmCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t armMode) 
{
    zclIASACE_Arm_t *pReq;
    uint8_t zoneId = 0x01;
    //zbClusterId_t aClusterId={gaZclClusterIASZone_c}; 
    uint8_t armDisarmCode[8] = {0x07, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};//{0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30}; /* "00000000" */
    
    pReq = MSG_Alloc(sizeof(zclIASACE_Arm_t)); 
  
    if(pReq) 
    {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame.ArmMode = armMode;
        //(void)ZCL_GetAttribute(SrcEndpoint, aClusterId, gZclAttrZoneSettingsZoneId_c,  gZclServerAttr_c, &zoneId, NULL);
        pReq->cmdFrame.ZoneId = zoneId;
        Copy8Bytes(pReq->cmdFrame.ArmDisarmCode,armDisarmCode);
        (void) IASACE_Arm(pReq);
        MSG_Free(pReq);
    }
}

/*!
 * @fn 		zbStatus_t IASACE_ArmRsp(zclIASACE_ArmRsp_t *pReq)
 *
 * @brief	Sends over-the-air an ArmResponse command  from the IAS ACE Cluster Server. 
 *
 */ 
zbStatus_t IASACE_ArmRsp
(
    zclIASACE_ArmRsp_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_ArmRsp_c, sizeof(zclCmdIASACE_ArmRsp_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_Bypass(zclIASACE_Bypass_t *pReq) 
 *
 * @brief	Sends over-the-air an Bypass command  from the IAS ACE Cluster Client. 
 *
 */   
zbStatus_t IASACE_Bypass
(
    zclIASACE_Bypass_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_Bypass_c,((1 + pReq->cmdFrame.NumberOfZones)),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void ZCL_SendBypassCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t NrOfZones, uint8_t *pZoneId) 
 *
 * @brief	Helper function to create and send an Bypass command over the air
 *
 */
void ZCL_SendBypassCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t NrOfZones, uint8_t *pZoneId) 
{
    zclIASACE_Bypass_t *pReq;
    uint8_t i=0;
    
    pReq = AF_MsgAlloc(); 
    
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame.NumberOfZones = NrOfZones;
        for(i=0; i < NrOfZones; i++)
          pReq->cmdFrame.pZoneId[i] = pZoneId[i];
        
        (void) IASACE_Bypass(pReq);
         MSG_Free(pReq);
    }
}

/*!
 * @fn 		void ZCL_SendEFPCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t CommandEFP)
 *
 * @brief	Helper function to create and send an Emergency/Fire/Panic command over the air. 
 *
 */  
void ZCL_SendEFPCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t CommandEFP) 
{
    zclIASACE_EFP_t *pReq;
    
    pReq = MSG_Alloc(sizeof(zclIASACE_EFP_t)); 
     
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
        pReq->zclTransactionId = gZclTransactionId++;
    
        (void)ZCL_SendClientReqSeqPassed(CommandEFP,0,(zclGenericReq_t *)pReq);
        MSG_Free(pReq);
    }
}

/*!
 * @fn 		zbStatus_t IASACE_Emergency(zclIASACE_EFP_t *pReq) 
 *
 * @brief	Sends over-the-air an Emergency command  from the IAS ACE Cluster Client. 
 *
 */ 
zbStatus_t IASACE_Emergency
(
    zclIASACE_EFP_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_Emergency_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_Fire(zclIASACE_EFP_t *pReq) 
 *
 * @brief	Sends over-the-air an Fire command  from the IAS ACE Cluster Client. 
 *
 */ 
zbStatus_t IASACE_Fire
(
    zclIASACE_EFP_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_Fire_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_Panic(zclIASACE_EFP_t *pReq) 
 *
 * @brief	Sends over-the-air an Panic command  from the IAS ACE Cluster Client. 
 *
 */ 
zbStatus_t IASACE_Panic
(
    zclIASACE_EFP_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_Panic_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_GetZoneIDMap(zclIASACE_EFP_t *pReq) 
 *
 * @brief	Sends over-the-air an GetZoneIdMap command  from the IAS ACE Cluster Client. 
 *
 */
zbStatus_t IASACE_GetZoneIDMap
(
    zclIASACE_EFP_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_GetZoneIDMap_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_GetZoneIDMapRsp(zclIASACE_GetZoneIDMApRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an GetZoneIdMapResponse command  from the IAS ACE Cluster Server. 
 *
 */
zbStatus_t IASACE_GetZoneIDMapRsp
(
    zclIASACE_GetZoneIDMApRsp_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_GetZoneIDMApRsp_c, sizeof(zclCmdIASACE_GetZoneIDMApRsp_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_GetZoneInformation(zclIASACE_GetZoneInformation_t *pReq) 
 *
 * @brief	Sends over-the-air an GetZoneInformation command  from the IAS ACE Cluster Client. 
 *
 */
zbStatus_t IASACE_GetZoneInformation
(
    zclIASACE_GetZoneInformation_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_GetZoneInformation_c, sizeof(zclCmdIASACE_GetZoneInformation_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void ZCL_SendGetZoneInformationCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t zoneId) 
 *
 * @brief	Helper function to create and send an GetZoneInformation command over the air. 
 *
 */ 
void ZCL_SendGetZoneInformationCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t zoneId) 
{
    zclIASACE_GetZoneInformation_t *pReq;
    
    pReq = MSG_Alloc(sizeof(zclIASACE_GetZoneInformation_t));   
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame.ZoneID = zoneId;
             
        (void) IASACE_GetZoneInformation(pReq);
         MSG_Free(pReq);
    }
}

/*!
 * @fn 		zbStatus_t IASACE_GetZoneInformationRsp(zclIASACE_GetZoneInfRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an GetZoneInformationResponse command  from the IAS ACE Cluster Server. 
 *
 */
zbStatus_t IASACE_GetZoneInformationRsp
(
    zclIASACE_GetZoneInfRsp_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_GetZoneInfRsp_c, sizeof(zclCmdIASZone_GetZoneInfRsp_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_ZoneStatusChanged(zclIASACE_ZoneStatusChanged_t *pReq) 
 *
 * @brief	Sends over-the-air an ZoneStatusChanged command  from the IAS ACE Cluster Server. 
 *
 */                       
zbStatus_t IASACE_ZoneStatusChanged
(
    zclIASACE_ZoneStatusChanged_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_ZoneStatusChanged_c, sizeof(zclCmdIASACE_ZoneStatusChanged_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_PanelStatusChanged(zclIASACE_PanelStatusChanged_t *pReq) 
 *
 * @brief	Sends over-the-air an PanelStatusChanged command  from the IAS ACE Cluster Server. 
 *
 */                         
zbStatus_t IASACE_PanelStatusChanged
(
    zclIASACE_PanelStatusChanged_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_PanelStatusChanged_c, sizeof(zclCmdIASACE_ZoneStatusChanged_t),(zclGenericReq_t *)pReq);
}

/******************************
  IAS Warning Device Cluster 
  See ZCL Specification Section 8.4
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_IASWDClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS WarningDevice Cluster Client. 
 *
 */
zbStatus_t ZCL_IASWDClusterClient
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
       default:
          return gZclUnsupportedClusterCommand_c;
    }

}

/*!
 * @fn 		zbStatus_t ZCL_IASWDClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS WarningDevice Cluster Server. 
 *
 */
zbStatus_t ZCL_IASWDClusterServer
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    uint8_t event;
    zbClusterId_t aClusterId;
    zbStatus_t status = gZclSuccessDefaultRsp_c;
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;
    
    Copy2Bytes(aClusterId, pIndication->aClusterId);
    
    /* handle the command */
    switch(pFrame->command)
    {
        case gZclCmdRxIASWD_StartWarning_c:
          {
            zclCmdIASWD_StartWarning_t  cmdPayload;
            uint16_t maxDurationAttr;
            (void)ZCL_GetAttribute(pIndication->dstEndPoint, aClusterId, gZclAttrIASWDMaxDuration_c, gZclServerAttr_c, &maxDurationAttr, NULL);
            FLib_MemCpy(&cmdPayload ,(pFrame + 1), sizeof(zclCmdIASWD_StartWarning_t));
            cmdPayload.WarningDuration = OTA2Native16( cmdPayload.WarningDuration);
            maxDurationAttr = OTA2Native16(maxDurationAttr);
            gWarningInf.WarningDuration = ((cmdPayload.WarningDuration > maxDurationAttr)?maxDurationAttr:cmdPayload.WarningDuration);  
            gWarningInf.StrobeDutyCycle = ((cmdPayload.StrobeDutyCycle > 10)?cmdPayload.StrobeDutyCycle:10);
            gWarningInf.WarningModeStrobeSirenLevel = cmdPayload.WarningModeStrobeSirenLevel;
            if(cmdPayload.WarningModeStrobeSirenLevel.WarningMode != 0)
            {
              if(cmdPayload.WarningModeStrobeSirenLevel.Strobe != 0)      
                event = gZclUI_StartWarningStrobe_c;
              else
              {
                event = gZclUI_StartWarning_c;
                gWarningInf.StrobeDutyCycle = 0;
              }
            }
            else
            {
              if(cmdPayload.WarningModeStrobeSirenLevel.Strobe != 0)
                event = gZclUI_StartStrobe_c;
              else
              {
                gWarningInf.WarningDuration = 0;
                event = gZclUI_NoEvent_c;
              }
            }
            BeeAppUpdateDevice(0, event, 0, 0, NULL);
            return status;  
          }       
        case gZclCmdRxIASWD_Squawk_c:
            FLib_MemCpy(&gParamSquawk ,(pFrame + 1), sizeof(zclCmdIASWD_Squawk_t));
            event = (gWarningInf.WarningDuration > 0)?gZclUI_NoEvent_c:gZclUI_Squawk_c;
            BeeAppUpdateDevice(0, event, 0, 0, NULL);
            return status;
            
        default:
          return gZclUnsupportedClusterCommand_c;
    }

}

/*!
 * @fn 		zbStatus_t IASWD_StartWarning(zclIASWD_StartWarning_t *pReq) 
 *
 * @brief	Sends over-the-air a StartWarning command from the IAS WD Cluster Client. 
 *
 */                       
zbStatus_t IASWD_StartWarning
(
   zclIASWD_StartWarning_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASWD_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASWD_StartWarning_c,sizeof(zclCmdIASWD_StartWarning_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void ZCL_SendStartWarning(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, zclCmdIASWD_StartWarning_t startWarningPayload) 
 *
 * @brief	Helper function to create and send StartWarning command over the air
 *
 */
void ZCL_SendStartWarning(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, zclCmdIASWD_StartWarning_t startWarningPayload) 
{
    zclIASWD_StartWarning_t *pReq;
    pReq = MSG_Alloc(sizeof(zclIASWD_StartWarning_t)); 
  
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame.WarningModeStrobeSirenLevel = startWarningPayload.WarningModeStrobeSirenLevel;
        pReq->cmdFrame.WarningDuration = startWarningPayload.WarningDuration;
        pReq->cmdFrame.StrobeLevel = startWarningPayload.StrobeLevel;
        pReq->cmdFrame.StrobeDutyCycle = startWarningPayload.StrobeDutyCycle;
          
        (void)IASWD_StartWarning(pReq);
        MSG_Free(pReq);
    }
}

/*!
 * @fn 		zbStatus_t IASWD_Squawk(zclIASWD_Squawk_t *pReq) 
 *
 * @brief	Sends over-the-air a Squawk command from the IAS WD Cluster Client. 
 *
 */  
zbStatus_t IASWD_Squawk
(
    zclIASWD_Squawk_t *pReq
)     
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASWD_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASWD_Squawk_c,sizeof(zclCmdIASWD_Squawk_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void ZCL_SendSquawk(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, zclCmdIASWD_Squawk_t squawk)  
 *
 * @brief	Helper function to create and send Squawk command over the air
 *
 */
void ZCL_SendSquawk(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, zclCmdIASWD_Squawk_t squawk) 
{
    zclIASWD_Squawk_t *pReq;
    pReq = MSG_Alloc(sizeof(zclIASWD_StartWarning_t)); 
  
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame = squawk; 
       (void)IASWD_Squawk(pReq);
        MSG_Free(pReq);
    }
}


/*!
 * @fn 		void ZCL_SendSquawkCmdCallback(tmrTimerID_t tmrId) 
 *
 * @brief	Callback used to send the Squawk command to all Warning devices, according with the zoneTable
 *
 */
void ZCL_SendSquawkCmdCallback(tmrTimerID_t tmrId)
{
  static uint8_t currentIndex = 0;
  uint8_t aNwkAddrCpy[2];
  uint8_t i = 0;
  uint8_t pIEEE[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00};
  uint8_t srcEndpoint;
  zbClusterId_t clusterId = {gaZclClusterIASWD_c};
  
  (void)tmrId;
  
  /* check zone table */ 
   for(i = currentIndex; i<gIndexTableZone; i++)
   {
    if(gTableZone[i].ZoneType == gZclZoneType_StandardWarning_c)
    {
      zclCmdIASWD_Squawk_t squawk;
      Copy8Bytes(pIEEE, (uint8_t *)&gTableZone[i].ZoneAddress);
      (void) APS_GetNwkAddress(pIEEE, aNwkAddrCpy);
      srcEndpoint = ZCL_GetEndPointForSpecificCluster(clusterId, FALSE); 
      if(gCIEApp_CurrentArmMode == gArmNotif_AllZoneDisarm_c)
        squawk.SquawkMode = SquawkMode_SystemDisarmed;
      else
        squawk.SquawkMode = SquawkMode_SystemArmed;
      squawk.Strobe = STROBE_PARALLEL;
      squawk.SquawkLevel = SquawkLevel_LowLevel;
      ZCL_SendSquawk( gTableZone[i].Endpoint, srcEndpoint, aNwkAddrCpy, squawk);
      currentIndex = i+1;
      if(currentIndex >= gIndexTableZone)
      {
        TMR_FreeTimer(gApplicationTmr);
        gApplicationTmr = gTmrInvalidTimerID_c;
        currentIndex = 0;
      }
      else
        TMR_StartSecondTimer(tmrId, 0x01, ZCL_SendSquawkCmdCallback);
      return;
     }
   }
   TMR_FreeTimer(gApplicationTmr);
   gApplicationTmr = gTmrInvalidTimerID_c;
   currentIndex = 0;   
}


/*!
 * @fn 		void ZCL_SendWarningCmdCallback(tmrTimerID_t tmrId) 
 *
 * @brief	Callback used to send the StartWarning command to all Warning devices, according with the zoneTable
 *
 */
void ZCL_SendWarningCmdCallback(tmrTimerID_t tmrId)
{
  static uint8_t currentIndex = 0;
  uint8_t aNwkAddrCpy[2];
  uint8_t i = 0;
  uint8_t pIEEE[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00};
  uint8_t srcEndpoint;
  zbClusterId_t clusterId = {gaZclClusterIASWD_c};

  /* check zone table */ 
   for(i = currentIndex; i< gIndexTableZone; i++)
   {
    if(gTableZone[i].ZoneType == gZclZoneType_StandardWarning_c)
    {
      Copy8Bytes(pIEEE, (uint8_t *)&gTableZone[i].ZoneAddress);
      (void) APS_GetNwkAddress(pIEEE, aNwkAddrCpy);
      srcEndpoint = ZCL_GetEndPointForSpecificCluster(clusterId, FALSE); 
      ZCL_SendStartWarning(gTableZone[i].Endpoint, srcEndpoint,  aNwkAddrCpy,  gStartWarningPayload); 

      currentIndex = i+1;
      if(currentIndex >= gIndexTableZone )
      {
        TMR_FreeTimer(gApplicationTmr);
        gApplicationTmr = gTmrInvalidTimerID_c;
        currentIndex = 0;
      }
      else
        TMR_StartSecondTimer(tmrId, 0x01, ZCL_SendWarningCmdCallback);
      return;
     }
   }
   TMR_FreeTimer(gApplicationTmr);
   gApplicationTmr = gTmrInvalidTimerID_c;
   currentIndex = 0;   
}
