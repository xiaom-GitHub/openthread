/******************************************************************************
* ZclOptions.h
*
* Include this file after BeeOptions.h
*
* (c) Copyright 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
* Documents used in this specification:
* [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
* [R2] - docs-07-5123-04-0afg-zigbee-cluster-library-specification.pdf
******************************************************************************/
#ifndef _ZCLOPTIONS_H
#define _ZCLOPTIONS_H

#ifdef __IAR_SYSTEMS_ICC__
#ifdef PROCESSOR_KINETIS
#include "AppToPlatformConfig.h"
#else
#include "OtaSupport.h"
#endif
#endif



/* default 16 scenes per device */
#ifndef gHaMaxScenes_c
#define gHaMaxScenes_c    2
#endif

/* The maximum size in bytes for the storable scene */
/* The Scene data array:
              -> For the OnOff scenes it only needs 1 byte (0x01).
              -> For the Dimmer light it only needs 11 bytes (0x0B).
              -> For the Thermostat it needs the gHaMaxSceneSize_c
                 which is 45 bytes (0x2D).
*/
#ifndef gHaMaxSceneSize_c
#define gHaMaxSceneSize_c    45
#endif

/* Enable optionals to enable all optional clusters/attributes */
#ifndef gZclClusterOptionals_d
#define gZclClusterOptionals_d    TRUE
#endif

/* Enable to include scene name in scene table */
#ifndef gZclIncludeSceneName_d
#define gZclIncludeSceneName_d    TRUE
#endif

/* Foundation commands */


/* enable reporting of attributes (saves about 1.4K ROM) */
#ifndef gZclEnableReporting_c
#define gZclEnableReporting_c     TRUE
#endif

/* enable long attr types Reporting */
#ifndef gZclEnableOver32BitAttrsReporting_c
#define gZclEnableOver32BitAttrsReporting_c     TRUE
#endif

/* enable sending configure reporting from ztc */
#ifndef gASL_ZclZtcConfigureReporting_d
#define gASL_ZclZtcConfigureReporting_d     TRUE
#endif
/* enable long string types handling */
#ifndef gZclEnableLongStringTypes_c
#define gZclEnableLongStringTypes_c     FALSE
#endif


/* enable Discover Attributes request */
#ifndef gZclDiscoverAttrReq_d
#define gZclDiscoverAttrReq_d   TRUE
#endif

/* enable reply to Discover Attributes */
#ifndef gZclDiscoverAttrRsp_d
#define gZclDiscoverAttrRsp_d   FALSE
#endif


/* enable Discover Attributes Extended request */
#ifndef gZclDiscoverAttrExtendedReq_d
#define gZclDiscoverAttrExtendedReq_d   FALSE
#endif

/* enable reply to Discover Attributes Extended response */
#ifndef gZclDiscoverAttrExtendedRsp_d
#define gZclDiscoverAttrExtendedRsp_d   FALSE
#endif

/* enable reply to Discover Commands (Generated/Received) */
#ifndef gZclDiscoverCommandsReg_d
#define gZclDiscoverCommandsReg_d   FALSE
#endif

/* enable reply to Discover Commands Received Response */
#ifndef gZclDiscoverCommandsRsp_d
#define gZclDiscoverCommandsRsp_d   FALSE
#endif

/* enable  Partition Cluster */
#ifndef gZclEnablePartition_d
#define gZclEnablePartition_d    FALSE
#endif

/* enable direction on foundation commands */
#ifndef gZclEnableDirection_d
#define gZclEnableDirection_d    FALSE
#endif

/* Set the Disable Default Response bit to 0 */
#ifndef gZclDisableDefaultResponse_d
#define gZclDisableDefaultResponse_d    TRUE
#endif

/* enable Power Profile cluster client functionality */
#ifndef gZclEnablePwrProfileClusterClient_d
#define gZclEnablePwrProfileClusterClient_d    FALSE
#endif

/* enable Power Profile cluster Server functionality */
#ifndef gZclEnablePwrProfileClusterServer_d
#define gZclEnablePwrProfileClusterServer_d    FALSE
#endif


#ifndef gZclZtcEnableApplianceCtrlClientSupport_d
#define gZclZtcEnableApplianceCtrlClientSupport_d    FALSE
#endif

/* enable Partition Cluster for PowerProfile commands */
#ifndef gZclEnablePartitionPwrProfile_d
#define gZclEnablePartitionPwrProfile_d    FALSE
#endif

#if (gZclEnablePartitionPwrProfile_d && (!gZclEnablePartition_d))
#error " Please enable gZclEnablePartition_d "
#endif


/* enable ApplianceStatistics Cluster*/
#ifndef gZclEnableApplianceStatistics_d
#define gZclEnableApplianceStatistics_d    FALSE
#endif

/* enable ApplianceStatistics Cluster Server optional functionality */
#ifndef gZclEnableApplianceStatisticsServerOptionals_d
#define gZclEnableApplianceStatisticsServerOptionals_d    FALSE
#endif

/* enable ApplianceAlertsAndEvents Cluster functionality */
#ifndef gZclEnableApplianceEventsAlerts_d
#define gZclEnableApplianceEventsAlerts_d    FALSE
#endif

/* enable ApplianceAlertsAndEvents Unsolicited Command */
#ifndef gZclEnableApplianceEventsAlertsUnsolicitedCmd_d
#define gZclEnableApplianceEventsAlertsUnsolicitedCmd_d    FALSE
#endif


/* enable ApplianceIdentification Cluster functionality */
#ifndef gZclEnableApplianceIdentification_d
#define gZclEnableApplianceIdentification_d    FALSE
#endif

/* enable Meter Identification Cluster */
#ifndef gZclEnableMeterIdentification_d
#define gZclEnableMeterIdentification_d    FALSE
#endif

/* enable Metering Cluster Server */
#ifndef gZclEnableMeteringServer_d
#define gZclEnableMeteringServer_d    FALSE
#endif

/* enable for Over the Air  (OTA)  Upgrade Cluster Server */
#ifndef gZclEnableOTAServer_d
#define gZclEnableOTAServer_d    FALSE
#endif

/* enable for Over the Air  (OTA)  Upgrade Cluster Client */
#ifndef gZclEnableOTAClient_d
#define gZclEnableOTAClient_d    FALSE
#endif

/* enable Discovery Server Process after ota client device joined */
#ifndef gZclOTADiscoveryServerProcess_d
#define gZclOTADiscoveryServerProcess_d    TRUE
#endif

/* Upgrade Image On Current Device */
#ifndef gUpgradeImageOnCurrentDevice_d
 #define gUpgradeImageOnCurrentDevice_d        FALSE   
#endif

/* enable only for client application the LEDs progress display */
#if (gZclEnableOTAClient_d==TRUE)
#define gZclEnableOTAProgressReport_d    TRUE
#else
#define gZclEnableOTAProgressReport_d    FALSE
#endif

/*enable ota progress report by sending data to external device/App*/
#ifndef gZclEnableOTAProgressReportToExternalApp_d
#define gZclEnableOTAProgressReportToExternalApp_d    TRUE
#endif

/*enable ECDSA Signature Procedure for Client OTA. 
 * If this define is enabled it is necessary to have an ECC library*/
#ifndef gZclEnableOTAClientECCLibrary_d
#define gZclEnableOTAClientECCLibrary_d    FALSE
#endif

/* enable Image integrity code validation for Client OTA */
/* if the gZclEnableOTAClientECCLibrary_d it is enabled the 
   gZclOtaClientImgIntegrityCodeValidation_d should be false */
#ifndef gZclOtaClientImgIntegrityCodeValidation_d
#define gZclOtaClientImgIntegrityCodeValidation_d    FALSE
#endif

#if (gZclOtaClientImgIntegrityCodeValidation_d && gZclEnableOTAClientECCLibrary_d)
#error "Cannot set both Validation procedure for a OTA Image"
#endif

/* enable Image integrity code validation for Server OTA */
#ifndef gZclOtaServerImgIntegrityCodeValidation_d
#define gZclOtaServerImgIntegrityCodeValidation_d    FALSE
#endif


/*enable for Server OTA the possibility to use external memory in the OTA process*/
#ifndef gZclOtaExternalMemorySupported_d 
#define gZclOtaExternalMemorySupported_d    FALSE
#endif



#ifdef PROCESSOR_KINETIS
#if (gZclEnableOTAClient_d && gZclEnableOTAServer_d && gUseInternalFlashForOta_c && gZclOtaExternalMemorySupported_d)
#error "The external memory is used to store the image received via OTA Cluster, Server should be in dongle mode. Please disable gZclOtaExternalMemorySupported_d"
#endif
#else
#if (gZclEnableOTAClient_d && gZclEnableOTAServer_d && (!gUseInternalFlashForOta_c) && gZclOtaExternalMemorySupported_d)
#error "The external memory is used to store the image received via OTA Cluster, Server should be in dongle mode. Please disable gZclOtaExternalMemorySupported_d"
#endif
#endif /* PROCESSOR_KINETIS */


/* enable Poll Control Cluster*/
#ifndef gZclEnablePollControlCluster_d
#define gZclEnablePollControlCluster_d    FALSE
#endif

/*enable for Binary Input Cluster*/
#ifndef gZclEnableBinaryInput_c
#define gZclEnableBinaryInput_c  FALSE
#endif

/*enable for Occupancy Sensor Cluster*/
#ifndef gZclEnableOccupancySensor_c
#define gZclEnableOccupancySensor_c   FALSE
#endif

/*enable for Thermostat Cluster*/
#ifndef gZclEnableThermostat_c 
#define gZclEnableThermostat_c  FALSE
#endif

/* Enable Trust Center Swap-Out Server functionality */
#ifndef gZclSETrustCenterSwapOutServer_d
#define gZclSETrustCenterSwapOutServer_d FALSE
#endif

/* Enable Trust Center Swap-Out Client functionality */
#ifndef gZclSETrustCenterSwapOutClient_d
#define gZclSETrustCenterSwapOutClient_d FALSE
#endif

/* Enable Multi-Dwelling Unit functionality */
#ifndef gZclSE_MDUPairing_d
#define gZclSE_MDUPairing_d FALSE
#endif

/* Enable Device Management functionality */
#ifndef gZclSE_DevMgmt_d
#define gZclSE_DevMgmt_d FALSE
#endif

/* Enable Signature On Commands */
#ifndef gZclSE_EnableCommandSignature_d
#define gZclSE_EnableCommandSignature_d TRUE
#endif


/***************************************
  Simple Metering Cluster
****************************************/

#ifndef gZclFastPollMode_d
#define gZclFastPollMode_d TRUE
#endif

#ifndef gZclFastPollUpdatePeriod_d
#define gZclFastPollUpdatePeriod_d 5
#endif

#ifndef gZclMirroring_d
#define gZclMirroring_d TRUE
#endif

#ifndef gZclMirroringAutoCfg_d
#define gZclMirroringAutoCfg_d FALSE
#endif

/***************************************
  Key Establishment Cluster
****************************************/

#ifndef gZclAcceptLongCert_d
#define gZclAcceptLongCert_d  TRUE
#endif

#ifndef gZclKeyEstabDebugMode_d
#define gZclKeyEstabDebugMode_d FALSE
#endif

#ifndef gZclKeyEstabDebugTimer_d
#define gZclKeyEstabDebugTimer_d FALSE
#endif

#ifndef gZclKeyEstabDebugTimeInterval_d
#define gZclKeyEstabDebugTimeInterval_d 240000
#endif

#ifndef gZclKeyEstabDebugInhibitInitRsp_d
#define gZclKeyEstabDebugInhibitInitRsp_d FALSE
#endif

#ifndef gZclKeyEstabDebugBypassMACCheck_d
#define gZclKeyEstabDebugBypassMACCheck_d FALSE
#endif

#ifndef gZclSeSecurityCheck_d
#define gZclSeSecurityCheck_d FALSE
#endif

#ifndef gZclSeUseInstallCodes_d
#define gZclSeUseInstallCodes_d FALSE
#endif

#ifndef gZclSeEnableAutoJoining_d
#define gZclSeEnableAutoJoining_d FALSE
#endif
/***************************************
  Basic Cluster Defaults
****************************************/

/* Basic Cluster Attribute Defaults, for setting by the OEM */

/* SW version of the application (set by OEM) */
#ifndef gZclAttrBasic_ApplicationVersion_c
#define gZclAttrBasic_ApplicationVersion_c  0x01
#endif

/* HW version of the board (set by OEM) */
#ifndef gZclAttrBasic_HWVersion_c
#define gZclAttrBasic_HWVersion_c           0x03
#endif

/* Power Source - 0x01 = mains, see gZclAttrBasicPowerSource_Mains_c */
#ifndef gZclAttrBasic_PowerSource_c
#define gZclAttrBasic_PowerSource_c         0x01
#endif

/* up to 32 characters, manufacturers name */
#ifndef gszZclAttrBasic_MfgName_c           
#define gszZclAttrBasic_MfgName_c           "Freescale"
#endif

/* up to 32 characters, model ID (any set of characters) */
#ifndef gszZclAttrBasic_Model_c             
#define gszZclAttrBasic_Model_c             "0001"
#endif

/* up to 16 characters (date code, must be in yyyymmdd format */
#ifndef gszZclAttrBasic_DateCode_c          
#define gszZclAttrBasic_DateCode_c          "20120823"
#endif

/*--------------------------------------- ZCL Definitions ----------------------------------------*/

/* Identify Cluster */
#ifndef gASL_ZclIdentifyQueryReq_d
#define gASL_ZclIdentifyQueryReq_d    TRUE
#endif

#ifndef gASL_ZclIdentifyReq_d
#define gASL_ZclIdentifyReq_d   TRUE 
#endif

#ifndef gASL_ZclCmdEzModeInvokeReq_d
#define gASL_ZclCmdEzModeInvokeReq_d    FALSE
#endif

#ifndef gASL_ZclCmdUpdateCommissioningStateReq_d
#define gASL_ZclCmdUpdateCommissioningStateReq_d    FALSE
#endif

/* EZ commissioning */
#ifndef gASL_EnableEZCommissioning_d
#define gASL_EnableEZCommissioning_d    FALSE
#endif

/* select the EZ commissiioning Role: Initiator or Target */
#ifndef gASL_EnableEZCommissioning_Initiator_d
#define gASL_EnableEZCommissioning_Initiator_d    FALSE
#endif
/* Add Group support for EZ commissioning procedure */
#ifndef gASL_EzCommissioning_EnableGroupBindCapability_d
#define gASL_EzCommissioning_EnableGroupBindCapability_d    FALSE
#endif

#if (gASL_EzCommissioning_EnableGroupBindCapability_d && (!gASL_EnableEZCommissioning_Initiator_d))
#error "The group support for EZ-mode commissioning is enabled only if the device is an Initiator"
#endif

/* Groups Cluster */
#ifndef gASL_ZclGroupAddGroupReq_d
#define gASL_ZclGroupAddGroupReq_d    TRUE
#endif

#ifndef gASL_ZclGroupViewGroupReq_d
#define gASL_ZclGroupViewGroupReq_d    TRUE
#endif

#ifndef gASL_ZclGetGroupMembershipReq_d
#define gASL_ZclGetGroupMembershipReq_d    TRUE
#endif

#ifndef gASL_ZclRemoveGroupReq_d
#define gASL_ZclRemoveGroupReq_d    TRUE
#endif

#ifndef gASL_ZclRemoveAllGroupsReq_d
#define gASL_ZclRemoveAllGroupsReq_d    TRUE
#endif

#ifndef gASL_ZclStoreSceneReq_d
#define gASL_ZclStoreSceneReq_d    TRUE
#endif

#ifndef gASL_ZclRecallSceneReq_d
#define gASL_ZclRecallSceneReq_d    TRUE
#endif

#ifndef gASL_ZclGetSceneMembershipReq_d
#define gASL_ZclGetSceneMembershipReq_d    TRUE
#endif

#ifndef gASL_ZclOnOffReq_d
#define gASL_ZclOnOffReq_d    TRUE
#endif

/* Thermostat cluster commands */

#ifndef gASL_ZclSetWeeklyScheduleReq
#define gASL_ZclSetWeeklyScheduleReq    FALSE
#endif

#ifndef gASL_ZclGetWeeklyScheduleReq
#define gASL_ZclGetWeeklyScheduleReq    FALSE
#endif

#ifndef gASL_ZclClearWeeklyScheduleReq
#define gASL_ZclClearWeeklyScheduleReq    FALSE
#endif

#ifndef gASL_ZclSetpointRaiseLowerReq
#define gASL_ZclSetpointRaiseLowerReq    FALSE
#endif

#ifndef gASL_ZclDoorLockReq_d
#define gASL_ZclDoorLockReq_d    FALSE
#endif


#ifndef gASL_ZclLevelControlReq_d
#define gASL_ZclLevelControlReq_d    TRUE
#endif

#ifndef gASL_ZclSceneAddSceneReq_d
#define gASL_ZclSceneAddSceneReq_d    TRUE
#endif

#ifndef gASL_ZclViewSceneReq_d
#define gASL_ZclViewSceneReq_d    TRUE
#endif

#ifndef gASL_ZclRemoveSceneReq_d
#define gASL_ZclRemoveSceneReq_d    TRUE
#endif

#ifndef gASL_ZclRemoveAllScenesReq_d
#define gASL_ZclRemoveAllScenesReq_d    TRUE
#endif

#ifndef gASL_ZclStoreSceneHandler_d
#define gASL_ZclStoreSceneHandler_d    TRUE
#endif

#ifndef gASL_ZclAddGroupHandler_d
#define gASL_ZclAddGroupHandler_d    TRUE
#endif

#ifndef gASL_ZclCommissioningRestartDeviceRequest_d
#define gASL_ZclCommissioningRestartDeviceRequest_d FALSE
#endif

#ifndef gASL_ZclCommissioningSaveStartupParametersRequest_d
#define gASL_ZclCommissioningSaveStartupParametersRequest_d FALSE
#endif

#ifndef gASL_ZclCommissioningRestoreStartupParametersRequest_d
#define gASL_ZclCommissioningRestoreStartupParametersRequest_d FALSE
#endif

#ifndef gASL_ZclCommissioningResetStartupParametersRequest_d
#define gASL_ZclCommissioningResetStartupParametersRequest_d FALSE
#endif

/* Alarms cluster commands */

#ifndef gASL_ZclAlarms_ResetAlarm_d
#define gASL_ZclAlarms_ResetAlarm_d    TRUE
#endif

#ifndef gASL_ZclAlarms_ResetAllAlarms_d
#define gASL_ZclAlarms_ResetAllAlarms_d    TRUE
#endif

#ifndef gASL_ZclAlarms_Alarm_d
#define gASL_ZclAlarms_Alarm_d    TRUE
#endif

#ifndef gASL_ZclAlarms_GetAlarm_d
#define gASL_ZclAlarms_GetAlarm_d    TRUE
#endif

/* IAS Zone Cluster */
#ifndef gASL_ZclIASZoneReq_d
#define gASL_ZclIASZoneReq_d    FALSE
#endif

/* IAS ACE Cluster */
#ifndef gASL_ZclIASACEReq_d
#define gASL_ZclIASACEReq_d    FALSE
#endif

/* IAS WD Cluster */
#ifndef gASL_ZclIASWDReq_d
#define gASL_ZclIASWDReq_d    FALSE
#endif


/* SE commands and responses */

#ifndef gASL_ZclDisplayMsgReq_d
#define gASL_ZclDisplayMsgReq_d TRUE
#endif

#ifndef gASL_ZclCancelMsgReq_d
#define gASL_ZclCancelMsgReq_d TRUE
#endif

#ifndef gASL_ZclGetLastMsgReq_d
#define gASL_ZclGetLastMsgReq_d TRUE
#endif

#ifndef gASL_ZclMsgConfReq_d
#define gASL_ZclMsgConfReq_d TRUE
#endif

#ifndef gASL_ZclInterPanDisplayMsgReq_d
#define gASL_ZclInterPanDisplayMsgReq_d TRUE
#endif

#ifndef gASL_ZclInterPanCancelMsgReq_d
#define gASL_ZclInterPanCancelMsgReq_d TRUE
#endif

#ifndef gASL_ZclInterPanGetLastMsgReq_d
#define gASL_ZclInterPanGetLastMsgReq_d TRUE
#endif

#ifndef gASL_ZclInterPanMsgConfReq_d
#define gASL_ZclInterPanMsgConfReq_d TRUE
#endif

#ifndef gASL_ZclSmplMet_GetProfReq_d
#define gASL_ZclSmplMet_GetProfReq_d TRUE
#endif

#ifndef gASL_ZclSmplMet_GetProfRsp_d
#define gASL_ZclSmplMet_GetProfRsp_d TRUE
#endif

#ifndef gASL_ZclSmplMet_FastPollModeReq_d
#define gASL_ZclSmplMet_FastPollModeReq_d TRUE
#endif

#ifndef gASL_ZclSmplMet_FastPollModeRsp_d
#define gASL_ZclSmplMet_FastPollModeRsp_d TRUE
#endif

#ifndef gASL_ZclSmplMet_AcceptFastPollModeReq_d
#define gASL_ZclSmplMet_AcceptFastPollModeReq_d TRUE
#endif

#ifndef gASL_ZclSmplMet_GetSnapshotReq_d
#define gASL_ZclSmplMet_GetSnapshotReq_d TRUE
#endif

#ifndef gASL_ZclMet_TakeSnapshotReq_d
#define gASL_ZclMet_TakeSnapshotReq_d TRUE
#endif

#ifndef gASL_ZclMet_RequestMirrorReq_d
#define gASL_ZclMet_RequestMirrorReq_d FALSE
#endif

#ifndef gASL_ZclMet_RemoveMirrorReq_d
#define gASL_ZclMet_RemoveMirrorReq_d FALSE
#endif

#ifndef gASL_ZclDmndRspLdCtrl_ReportEvtStatus_d
#define gASL_ZclDmndRspLdCtrl_ReportEvtStatus_d TRUE
#endif

#ifndef gASL_ZclDmndRspLdCtrl_GetScheduledEvtsReq_d
#define gASL_ZclDmndRspLdCtrl_GetScheduledEvtsReq_d TRUE
#endif

#ifndef gASL_ZclDmndRspLdCtrl_ScheduledServerEvts_d
#define gASL_ZclDmndRspLdCtrl_ScheduledServerEvts_d TRUE
#endif

#ifndef gASL_ZclDmndRspLdCtrl_LdCtrlEvtReq_d
#define gASL_ZclDmndRspLdCtrl_LdCtrlEvtReq_d TRUE
#endif

#ifndef gASL_ZclDmndRspLdCtrl_CancelLdCtrlEvtReq_d
#define gASL_ZclDmndRspLdCtrl_CancelLdCtrlEvtReq_d TRUE
#endif

#ifndef gASL_ZclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_d
#define gASL_ZclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_d TRUE
#endif

#ifndef gASL_ZclPrice_GetCurrPriceReq_d
#define gASL_ZclPrice_GetCurrPriceReq_d TRUE
#endif

#ifndef gASL_ZclPrice_GetSheduledPricesReq_d
#define gASL_ZclPrice_GetSheduledPricesReq_d TRUE
#endif

#ifndef gASL_ZclPrice_GetBlockPeriodsReq_d
#define gASL_ZclPrice_GetBlockPeriodsReq_d TRUE
#endif

#ifndef gASL_ZclPrice_GetPriceMatrixReq_d
#define gASL_ZclPrice_GetPriceMatrixReq_d TRUE
#endif

#ifndef gASL_ZclPrice_GetTierLabelsReq_d
#define gASL_ZclPrice_GetTierLabelsReq_d TRUE
#endif

#ifndef gASL_ZclPrice_GetBlockThresholdsReq_d
#define gASL_ZclPrice_GetBlockThresholdsReq_d TRUE
#endif
#ifndef gASL_ZclPrice_GetConversionFactorReq_d
#define gASL_ZclPrice_GetConversionFactorReq_d TRUE
#endif

#ifndef gASL_ZclPrice_GetCO2ValueReq_d
#define gASL_ZclPrice_GetCO2ValueReq_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishCO2ValueRsp_d
#define gASL_ZclPrice_PublishCO2ValueRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_GetCalorificValueReq_d
#define gASL_ZclPrice_GetCalorificValueReq_d TRUE
#endif

#ifndef gASL_ZclPrice_GetTariffInformationReq_d
#define gASL_ZclPrice_GetTariffInformationReq_d TRUE
#endif

#ifndef gASL_ZclPrice_GetBillingPeriodReq_d
#define gASL_ZclPrice_GetBillingPeriodReq_d TRUE
#endif

#ifndef gASL_ZclPrice_GetConsolidatedBillReq_d
#define gASL_ZclPrice_GetConsolidatedBillReq_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishConversionFactorRsp_d
#define gASL_ZclPrice_PublishConversionFactorRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishCO2ValueRsp_d
#define gASL_ZclPrice_PublishCO2ValueRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishPriceMatrixRsp_d
#define gASL_ZclPrice_PublishPriceMatrixRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishTierLabelsRsp_d
#define gASL_ZclPrice_PublishTierLabelsRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishBlockThresholdsRsp_d
#define gASL_ZclPrice_PublishBlockThresholdsRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishCalorificValueRsp_d
#define gASL_ZclPrice_PublishCalorificValueRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishBlockPeriodsRsp_d
#define gASL_ZclPrice_PublishBlockPeriodsRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishBillingPeriodRsp_d
#define gASL_ZclPrice_PublishBillingPeriodRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishCPPEventRsp_d
#define gASL_ZclPrice_PublishCPPEventRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishTariffInformationRsp_d
#define gASL_ZclPrice_PublishTariffInformationRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishConsolidatedBillRsp_d
#define gASL_ZclPrice_PublishConsolidatedBillRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishPriceRsp_d
#define gASL_ZclPrice_PublishPriceRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishExtendedPriceRsp_d
#define gASL_ZclPrice_PublishExtendedPriceRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_CancelTariff_d
#define gASL_ZclPrice_CancelTariff_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishCreditPaymentRsp_d
#define gASL_ZclPrice_PublishCreditPaymentRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_GetCurrencyConversionReq_d
#define gASL_ZclPrice_GetCurrencyConversionReq_d TRUE
#endif

#ifndef gASL_ZclPrice_PublishCurrencyConversionRsp_d
#define gASL_ZclPrice_PublishCurrencyConversionRsp_d TRUE
#endif

#ifndef gASL_ZclPrice_GetCreditPaymentReq_d
#define gASL_ZclPrice_GetCreditPaymentReq_d TRUE
#endif

#ifndef gASL_ZclPrice_InterPanGetCurrPriceReq_d
#define gASL_ZclPrice_InterPanGetCurrPriceReq_d TRUE
#endif

#ifndef gASL_ZclPrice_InterPanGetSheduledPricesReq_d
#define gASL_ZclPrice_InterPanGetSheduledPricesReq_d TRUE
#endif

#ifndef gASL_ZclPrice_InterPanPublishPriceRsp_d
#define gASL_ZclPrice_InterPanPublishPriceRsp_d TRUE
#endif

/* SE Tunneling requires fragmentation to be enabled (gFragmentationCapability_d TRUE) */
#ifndef gASL_ZclSETunneling_d
#define gASL_ZclSETunneling_d TRUE
#endif

#ifndef gASL_ZclSETunnelingOptionals_d
#define gASL_ZclSETunnelingOptionals_d FALSE
#endif

#ifndef gASL_ZclZtcSETunnelingTesting_d
#define gASL_ZclZtcSETunnelingTesting_d TRUE
#endif

#ifndef gASL_ZclZtcSETunnelingTestingBufferSize_c
#define gASL_ZclZtcSETunnelingTestingBufferSize_c 128
#endif

#ifndef gASL_ZclPrepayment_d
#define gASL_ZclPrepayment_d TRUE
#endif

#ifndef gASL_ZclPrepayment_Optionals_d
#define gASL_ZclPrepayment_Optionals_d FALSE
#endif

#ifndef gASL_ZclPrepayment_TopUpOptionals_d
#define gASL_ZclPrepayment_TopUpOptionals_d TRUE
#endif

#ifndef gASL_ZclPrepayment_DebtOptionals_d
#define gASL_ZclPrepayment_DebtOptionals_d TRUE
#endif

#ifndef gASL_ZclPrepayment_SupplyOptionals_d
#define gASL_ZclPrepayment_SupplyOptionals_d TRUE
#endif
#ifndef gASL_ZclPrepayment_SelAvailEmergCreditReq_d
#define gASL_ZclPrepayment_SelAvailEmergCreditReq_d TRUE
#endif

#ifndef gASL_ZclPrepayment_ChangeSupplyReq_d
#define gASL_ZclPrepayment_ChangeSupplyReq_d FALSE
#endif

#ifndef gASL_ZclPrepayment_ChangeDebtReq_d
#define gASL_ZclPrepayment_ChangeDebtReq_d TRUE
#endif

#ifndef gASL_ZclPrepayment_EmergencyCreditSetupReq_d
#define gASL_ZclPrepayment_EmergencyCreditSetupReq_d TRUE
#endif

#ifndef gASL_ZclPrepayment_ConsumerTopUpReq_d
#define gASL_ZclPrepayment_ConsumerTopUpReq_d TRUE
#endif

#ifndef gASL_ZclPrepayment_CreditAdjustmentReq_d
#define gASL_ZclPrepayment_CreditAdjustmentReq_d TRUE
#endif

#ifndef gASL_ZclPrepayment_ChangePaymentModeReq_d
#define gASL_ZclPrepayment_ChangePaymentModeReq_d TRUE
#endif

#ifndef gASL_ZclPrepayment_GetPrepaySnapshotReq_d
#define gASL_ZclPrepayment_GetPrepaySnapshotReq_d TRUE
#endif

#ifndef gASL_ZclPrepayment_GetTopUpLogReq_d
#define gASL_ZclPrepayment_GetTopUpLogReq_d TRUE
#endif

#ifndef gASL_ZclPrepayment_SetLowCreditWarningLevelReq_d
#define gASL_ZclPrepayment_SetLowCreditWarningLevelReq_d TRUE
#endif

#ifndef gASL_ZclPrepayment_GetDebtRepaymentLogReq_d
#define gASL_ZclPrepayment_GetDebtRepaymentLogReq_d TRUE
#endif

#ifndef gASL_ZclKeyEstab_InitKeyEstabReq_d
#define gASL_ZclKeyEstab_InitKeyEstabReq_d TRUE
#endif

#ifndef gASL_ZclKeyEstab_EphemeralDataReq_d
#define gASL_ZclKeyEstab_EphemeralDataReq_d TRUE
#endif

#ifndef gASL_ZclKeyEstab_ConfirmKeyDataReq_d
#define gASL_ZclKeyEstab_ConfirmKeyDataReq_d TRUE
#endif

#ifndef gASL_ZclKeyEstab_TerminateKeyEstabServer_d
#define gASL_ZclKeyEstab_TerminateKeyEstabServer_d TRUE
#endif

#ifndef gASL_ZclKeyEstab_InitKeyEstabRsp_d
#define gASL_ZclKeyEstab_InitKeyEstabRsp_d TRUE
#endif

#ifndef gASL_ZclKeyEstab_EphemeralDataRsp_d
#define gASL_ZclKeyEstab_EphemeralDataRsp_d TRUE
#endif

#ifndef gASL_ZclKeyEstab_ConfirmKeyDataRsp_d
#define gASL_ZclKeyEstab_ConfirmKeyDataRsp_d TRUE
#endif

#ifndef gASL_ZclKeyEstab_TerminateKeyEstabClient_d
#define gASL_ZclKeyEstab_TerminateKeyEstabClient_d TRUE
#endif

#ifndef gASL_ZclSE_RegisterDevice_d
#define gASL_ZclSE_RegisterDevice_d TRUE
#endif

#ifndef gASL_ZclSE_DeRegisterDevice_d
#define gASL_ZclSE_DeRegisterDevice_d TRUE
#endif

#ifndef gASL_ZclSE_InitTime_d
#define gASL_ZclSE_InitTime_d TRUE
#endif

/*****************************
    Optional Attributes
*****************************/

#if !gZclClusterOptionals_d
#define gASL_ZclMet_Optionals_d TRUE
#define gASL_ZclPrice_Optionals_d FALSE
#endif 

/* Cluster Configuration */

#ifndef gASL_ZclMet_Optionals_d
#define gASL_ZclMet_Optionals_d TRUE
#endif

#ifndef gASL_ZclPrice_Optionals_d
#define gASL_ZclPrice_Optionals_d FALSE
#endif

#ifndef gASL_ZclPrepayment_Optionals_d
#define gASL_ZclPrepayment_Optionals_d FALSE
#endif

#ifndef gASL_ZclTouCalendar_Optionals_d
#define gASL_ZclTouCalendar_Optionals_d TRUE
#endif

#ifndef gASL_ZclSE_12_Features_d    
#define gASL_ZclSE_12_Features_d    FALSE
#endif

/* Cluster Sets Configuration */

#ifndef gASL_ZclSE_TiersNumber_d
#define gASL_ZclSE_TiersNumber_d 15
#endif

#ifndef gASL_ZclSE_ExtendedPriceTiersNumber_d
#define gASL_ZclSE_ExtendedPriceTiersNumber_d 1
#endif

#ifndef gASL_ZclPrice_BlockThresholdNumber_d
#define gASL_ZclPrice_BlockThresholdNumber_d 10
#endif

#ifndef gASL_ZclSE_BlocksNumber_d
#define gASL_ZclSE_BlocksNumber_d (gASL_ZclPrice_BlockThresholdNumber_d + 1)//max 16
#endif

#ifndef gASL_ZclPrice_BlockPriceInfoNumber_d
#define gASL_ZclPrice_BlockPriceInfoNumber_d gASL_ZclSE_BlocksNumber_d * (gASL_ZclSE_TiersNumber_d + 1)
#endif


/***************************************
  Define the set of clusters used by each device
****************************************/

extern zbApsTxOption_t const gZclTxOptions;

#endif /* _ZCLOPTIONS_H */

