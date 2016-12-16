/*! @file 	  ZclGeneral.c
 *
 * @brief	  This source file describes specific functionality implemented for ZCL General 
 *			  functional domain
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
#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif
#include "EmbeddedTypes.h"
#include "NV_Data.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "BeeStackParameters.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "TMR_Interface.h"
#include "ZdoApsInterface.h"
#include "ApsMgmtInterface.h"
#include "BeeAppInit.h"
#include "BeeCommon.h"
#include "HaProfile.h"
#include "ASL_UserInterface.h"
#include "BeeApp.h"
#include "ZCL.h"
#include "ZdpManager.h"
#include "ZclClosures.h"
#include "zclSensing.h"
#include "zclOta.h"
#include "EndPointConfig.h"
#include "ASL_ZdpInterface.h"
#include "EzCommissioning.h"
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/* Basic Cluster Attribute Defaults, not for setting by the OEM */
/* See ZclOptions.h for the list of OEM settable attributes */
#define gHaAttrBasicZCLVersion_c          0x01
#define gHaAttrBasic_StackVersion_c       0x02
#define gZclLevel_OnOffCmd_c              0x01
#define gZclLevel_NotOnOffCmd_c           0x00
#define gZclLevel_UndefinedOnLevel_c      0xFF
#define gZclGroup_NameSupport_c           0x00  /* Group Names not supported */
#if gZclIncludeSceneName_d
#define gZclScene_NameSupport_c           0x01  /* Scene Names supported */
#else
#define gZclScene_NameSupport_c           0x00 /* Scene Names not supported */
#endif
#define DummyEndPoint                     0xFF
#define gpGroupTable gaApsGroupTable

#ifndef gHostApp_d 
#define gNvZclDataSet_c gNvAppDataSet_ID_c
#else
#define gNvZclDataSet_c gNvDataSet_App_ID_c
#endif

bool_t gShadeDevice = FALSE;
gZclShadeStatus_t   statusShadeCfg = {0,0,0,0,0};
bool_t gShadeDeviceActive = FALSE;
#if gZclClusterOptionals_d  
tmrTimerID_t gOnOffReqTimer = gTmrInvalidTimerID_c;
#endif

#if gZclEnablePollControlCluster_d
bool_t gPollControl_SetPollAttr = FALSE;
#endif
/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/
zclSceneTableEntry_t *ZCL_AllocateScene(afDeviceDef_t *pDevice, zclCmdScene_StoreScene_t *pSceneId);
zclSceneTableEntry_t *ZCL_FindScene(afDeviceDef_t *pDevice, zclCmdScene_RecallScene_t *pSceneId);
zbStatus_t ZCL_GroupAddGroupRsp(zbApsdeDataIndication_t *pIndication,zclCmdGroup_AddGroup_t*command);
zbStatus_t ZCL_GroupAddGroupIfIdentify(zbApsdeDataIndication_t *pIndication,zclCmdGroup_AddGroupIfIdentifying_t*command);
zbStatus_t ZCL_GroupRemoveGroupRsp(zbApsdeDataIndication_t *pIndication,zclCmdGroup_RemoveGroup_t*command);
zbStatus_t ZCL_GroupRemoveAllGroups(zbApsdeDataIndication_t *pIndication,zclCmdGroup_RemoveAllGroups_t*command);
zclStatus_t ZCL_GroupViewGroupRsp(zbApsdeDataIndication_t *pIndication,zclCmdGroup_ViewGroup_t* command);
zclStatus_t ZCL_GroupGetGroupMembershipRsp(zbApsdeDataIndication_t *pIndication,zclCmdGroup_GetGroupMembership_t* command);
zbStatus_t ZCL_AddScene(zclSceneTableEntry_t *pScene, zclCmdScene_AddScene_t *pAddSceneReq, uint8_t indicationLen);
zbStatus_t ZCL_ViewScene(zclSceneTableEntry_t *pScene,zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice);
void ZCL_GetSceneMembership(afDeviceDef_t *pDevice, zbGroupId_t aGroupId,zbApsdeDataIndication_t *pIndication);
zbStatus_t ZCL_LevelControlMoveToLevel(zclCmdLevelControl_MoveToLevel_t * pReq, bool_t withOnOff);
zbStatus_t ZCL_LevelControlMove(zclCmdLevelControl_Move_t * pReq, bool_t withOnOff);
zclStatus_t ZCL_LevelControlStep(zclCmdLevelControl_Step_t *pReq, bool_t withOnOff);
zclStatus_t ZCL_LevelControlStop(void);
static zbStatus_t ZCL_IdentifyQueryRspHandler(zbApsdeDataIndication_t *pIndication);

#if gZclEnablePollControlCluster_d
#if gEndDevCapability_d || gComboDeviceCapability_d
zbStatus_t zclPollControl_StartFastPollMode(uint16_t timeout);
void zclPollControl_StopFastPollMode(bool_t timeoutStop);
void ZclPollControl_CheckInCallback(uint8_t tmrId);
void ZclPollControl_FastPollModeCallback(uint8_t tmrId);
static uint16_t ZclPollControl_SearchMaxFastTimeout(void);
#endif
#endif


/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/* for handling OTA group commands */
typedef union zclAnyGroupReq_tag {
  zbApsmeAddGroupReq_t          addGroup;
  zbApsmeRemoveGroupReq_t       removeGroup;
  zbApsmeRemoveAllGroupsReq_t   removeAllGroups;
} zclAnyGroupReq_t;


/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/
#if MC13226Included_d
    extern index_t gApsMaxBindingEntries;
    extern uint8_t gApsMaxGroups;
#else
    extern const index_t gApsMaxBindingEntries;
    extern const uint8_t gApsMaxGroups;
#endif

tmrTimerID_t occupancyTimer;
bool_t gSetAvailableOccupancy = FALSE;

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/******************************
  Basic Cluster Data
  See ZCL Specification Section 3.2
*******************************/

uint8_t gZclBasicZclVersion = gHaAttrBasicZCLVersion_c;
uint8_t gZclBasicApplicationVersion = gZclAttrBasic_ApplicationVersion_c;
uint8_t gZclBasicStackVersion = gHaAttrBasic_StackVersion_c;
uint8_t gZclBasicHWVersion = gZclAttrBasic_HWVersion_c;
uint8_t gZclBasicPowerSource = gZclAttrBasic_PowerSource_c;
/* Manufacturer, Icon#, DeviceID, Device #, Reserved */
#if gZclClusterOptionals_d
char gszZclBasicAttrMfgName[] = gszZclAttrBasic_MfgName_c;
char gszZclBasicAttrModel[] = gszZclAttrBasic_Model_c;
char gszZclBasicAttrDateCode[] = gszZclAttrBasic_DateCode_c;
#endif

// zclStr16_t  gZclBasicAttr_Location;
// uint8_t     gZclBasicAttr_PhysicalEnvironment;
// uint8_t     gZclBasicAttr_AlarmMask;

/*used for Factory Fresh - EZ commissioning*/
const zclCommonAttr_t  gZclCommonAttrDefaultValues = {
#if gZclClusterOptionals_d  
  {0x09, 'F','r','e','e','s','c','a','l','e',' ',' ',' ',' ',' ',' ',' '},
  gZclAttrBasic_PhysicalEnvironment,
  0x00,
#endif  
  0x01
};

zclCommonAttr_t  gZclCommonAttr = {
#if gZclClusterOptionals_d  
  {0x09, 'F','r','e','e','s','c','a','l','e',' ',' ',' ',' ',' ',' ',' '},
  gZclAttrBasic_PhysicalEnvironment,
  0x00,
#endif  
  0x01
};

/* Basic Cluster Attribute Definitions */
const zclAttrDef_t gaZclBasicClusterAttrDef[] = {
  { gZclAttrIdBasicZCLVersionId_c,          gZclDataTypeUint8_c, gZclAttrFlagsInLine_c, sizeof(uint8_t), (void *)&gZclBasicZclVersion},
#if gZclClusterOptionals_d
  { gZclAttrIdBasicApplicationVersionId_c,  gZclDataTypeUint8_c, gZclAttrFlagsInLine_c, sizeof(uint8_t), (void *)&gZclBasicApplicationVersion },
  { gZclAttrIdBasicStackVersionId_c,        gZclDataTypeUint8_c, gZclAttrFlagsInLine_c, sizeof(uint8_t), (void *)&gZclBasicStackVersion },
  { gZclAttrIdBasicHWVersionId_c,           gZclDataTypeUint8_c, gZclAttrFlagsInLine_c, sizeof(uint8_t), (void *)&gZclBasicHWVersion },
  { gZclAttrIdBasicManufacturerNameId_c,    gZclDataTypeStr_c,   gZclAttrFlagsInROM_c,  sizeof(gszZclBasicAttrMfgName)-1, (void *)gszZclBasicAttrMfgName}  ,
  { gZclAttrIdBasicModelIdentifierId_c,     gZclDataTypeStr_c,   gZclAttrFlagsInROM_c,  sizeof(gszZclBasicAttrModel)-1, (void *)gszZclBasicAttrModel },
  { gZclAttrIdBasicDateCodeId_c,            gZclDataTypeStr_c,   gZclAttrFlagsInROM_c,  sizeof(gszZclBasicAttrDateCode)-1, (void *)gszZclBasicAttrDateCode },
#endif
  { gZclAttrIdBasicPowerSourceId_c,         gZclDataTypeEnum8_c, gZclAttrFlagsInLine_c, sizeof(uint8_t), { (void *)&gZclBasicPowerSource } },
#if gZclClusterOptionals_d
  { gZclAttrIdBasicLocationDescriptionId_c, gZclDataTypeStr_c, gZclAttrFlagsNoFlags_c, sizeof(zclStr16_t), { (void *)&gZclCommonAttr.basicLocation } },
  { gZclAttrIdBasicPhysicalEnvironmentId_c, gZclDataTypeEnum8_c, gZclAttrFlagsNoFlags_c, sizeof(uint8_t), { (void *)&gZclCommonAttr.basicPhysicalEnvironment } },
#endif
  { gZclAttrIdBasicDeviceEnabledId_c,       gZclDataTypeBool_c, gZclAttrFlagsNoFlags_c, sizeof(uint8_t), { (void *)&gZclCommonAttr.basicDeviceEnabled } }
#if gZclClusterOptionals_d
  , { gZclAttrIdBasicAlarmMaskId_c,         gZclDataTypeBitmap8_c, gZclAttrFlagsNoFlags_c, sizeof(uint8_t), { (void *)&gZclCommonAttr.basicAlarmMask } }
#endif
};

const zclAttrSet_t gaZclBasicClusterAttrSet[] = {
  {gZclAttrSetBasicDeviceInformation_c, (void *)&gaZclBasicClusterAttrDef, NumberOfElements(gaZclBasicClusterAttrDef)}
};

const zclAttrSetList_t gZclBasicClusterAttrSetList = {
  NumberOfElements(gaZclBasicClusterAttrSet),
  gaZclBasicClusterAttrSet
};

seESIMirrorBasicData_t gZclMirrorBasicAttr;

/* Basic Cluster Attribute Definitions */
const zclAttrDef_t gaZclBasicClusterMirrorAttrDef[] = {
  { gZclAttrIdBasicZCLVersionId_c,          gZclDataTypeUint8_c, gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)&gZclMirrorBasicAttr.ZCLVersion },
  { gZclAttrIdBasicPowerSourceId_c,         gZclDataTypeEnum8_c, gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)&gZclMirrorBasicAttr.iPowerSource },
};

const zclAttrSet_t gaZclBasicClusterMirrorAttrSet[] = {
  {gZclAttrSetBasicDeviceInformation_c, (void *)&gaZclBasicClusterMirrorAttrDef, NumberOfElements(gaZclBasicClusterMirrorAttrDef)}
};

const zclAttrSetList_t gZclBasicClusterMirrorAttrSetList = {
  NumberOfElements(gaZclBasicClusterMirrorAttrSet),
  gaZclBasicClusterAttrSet
};
/******************************
  Power Configuration Cluster Data
  See ZCL Specification Section 3.3
*******************************/
#if gZclClusterOptionals_d
/* Power Configuration Cluster Attribute Definitions */

zclPowerCfgAttrsRAM_t gZclPowerCfgAttrs =
{
  0x0000, /*gZclAttrPwrConfigMainsInfMainsVoltage_c*/
  0x00, /*gZclAttrPwrConfigMainsInfMainsFrequency_c*/
  0x00, /*gZclAttrPwrConfigMainsStgMainsAlarmMask_c*/
  0x0000, /*gZclAttrPwrConfigMainsStgMainsVoltageMinThreshold_c*/
  0x0000, /*gZclAttrPwrConfigMainsStgMainsVoltageMaxThreshold_c*/
  0x0000, /*gZclAttrPwrConfigMainsStgMainsVoltageDwellTripPoint_c*/
  {0x00, 0x00, 0x00}, /*gZclAttrPwrConfigBatteryInfBatteryVoltage_c*/
  0x00, /*gZclAttrPwrConfigBatteryInfBatteryPercentageRemaining_c */
  0x09, 'F','r','e','e','s','c','a','l','e',' ',' ',' ',' ',' ',' ', /*gZclAttrPwrConfigBatteryStgBatteryManufacturer_c*/
  0x00, /*gZclAttrPwrConfigBatteryStgBatterySize_c*/
  0x0000, /*gZclAttrPwrConfigBatteryStgBatteryAHrRating_c*/ 
  0x00, /*gZclAttrPwrConfigBatteryStgBatteryQuantity_c*/
  0x00, /*gZclAttrPwrConfigBatteryStgBatteryRatedVoltage_c*/
  0x00, /*gZclAttrPwrConfigBatteryStgBatteryAlarmMask_c*/
  0x00, /*gZclAttrPwrConfigBatteryStgBatteryVoltageMinThreshold_c*/
  0x00, /*gZclAttrPwrConfigIdBatteryStgBatteryVoltageThreshold1_c*/
  0x00, /*gZclAttrPwrConfigIdBatteryStgBatteryVoltageThreshold2_c*/
  0x00, /*gZclAttrPwrConfigIdBatteryStgBatteryVoltageThreshold3_c*/
  0x00, /*gZclAttrPwrConfigIdBatteryStgBatteryPercentageMinThreshold_c*/
  0x00, /*gZclAttrPwrConfigIdBatteryStgBatteryPercentageThreshold1_c*/
  0x00, /*gZclAttrPwrConfigIdBatteryStgBatteryPercentageThreshold2_c*/
  0x00  /*gZclAttrPwrConfigIdBatteryStgBatteryPercentageThreshold3_c*/
};

const zclAttrDef_t gaZclPowerCfgClusterAttrDef[] = {
   //mains information
  { gZclAttrPwrConfigIdMainsInfMainsVoltage_c,    gZclDataTypeUint16_c, gZclAttrFlagsNoFlags_c ,  sizeof(uint16_t), (void *)&gZclPowerCfgAttrs.mainsVoltage},
  { gZclAttrPwrConfigIdMainsInfMainsFrequency_c,  gZclDataTypeUint8_c,  gZclAttrFlagsNoFlags_c ,    sizeof(uint8_t),(void *)&gZclPowerCfgAttrs.mainsFrequency},
  //mains settings
  { gZclAttrPwrConfigIdMainsStgMainsAlarmMask_c,             gZclDataTypeBitmap8_c,  gZclAttrFlagsNoFlags_c,    sizeof(uint8_t), (void *)&gZclPowerCfgAttrs.mainsAlarmMask},
  { gZclAttrPwrConfigIdMainsStgMainsVoltageMinThreshold_c,   gZclDataTypeUint16_c,   gZclAttrFlagsNoFlags_c,    sizeof(uint16_t),(void *)&gZclPowerCfgAttrs.mainsVoltageMinThreshold},
  { gZclAttrPwrConfigIdMainsStgMainsVoltageMaxThreshold_c,   gZclDataTypeUint16_c,   gZclAttrFlagsNoFlags_c,    sizeof(uint16_t),(void *)&gZclPowerCfgAttrs.mainsVoltageMaxThreshold},
  { gZclAttrPwrConfigIdMainsStgMainsVoltageDwellTripPoint_c, gZclDataTypeUint16_c,   gZclAttrFlagsNoFlags_c,    sizeof(uint16_t),(void *)&gZclPowerCfgAttrs.mainsVoltageDwellTripPoint},
  //battery information
  { gZclAttrPwrConfigIdBatteryInfBatteryVoltage_c,    gZclDataTypeUint8_c,  gZclAttrFlagsReportable_c ,    sizeof(uint8_t), (void *)&gZclPowerCfgAttrs.batteryVoltage},
  { gZclAttrPwrConfigIdBatteryInfBatteryPercentageRemaining_c,    gZclDataTypeUint8_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c ,    sizeof(uint8_t), (void *)&gZclPowerCfgAttrs.batteryPercentageRemaining},
  //battery settings
  { gZclAttrPwrConfigIdBatteryStgBatteryManufacturer_c,       gZclDataTypeStr_c,     gZclAttrFlagsNoFlags_c,   sizeof(zclStr16_t), (void *)&gZclPowerCfgAttrs.batteryMfg},
  { gZclAttrPwrConfigIdBatteryStgBatterySize_c,               gZclDataTypeEnum8_c,   gZclAttrFlagsNoFlags_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batterySize},
  { gZclAttrPwrConfigIdBatteryStgBatteryAHrRating_c,          gZclDataTypeUint16_c,  gZclAttrFlagsNoFlags_c,   sizeof(uint16_t),   (void *)&gZclPowerCfgAttrs.batteryAHrRating},
  { gZclAttrPwrConfigIdBatteryStgBatteryQuantity_c,           gZclDataTypeUint8_c,   gZclAttrFlagsNoFlags_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batteryQuantity},
  { gZclAttrPwrConfigIdBatteryStgBatteryRatedVoltage_c,       gZclDataTypeUint8_c,   gZclAttrFlagsNoFlags_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batteryRatedVoltage},
  { gZclAttrPwrConfigIdBatteryStgBatteryAlarmMask_c,          gZclDataTypeBitmap8_c, gZclAttrFlagsNoFlags_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batteryAlarmMask},
  { gZclAttrPwrConfigIdBatteryStgBatteryVoltageMinThreshold_c,gZclDataTypeUint8_c,   gZclAttrFlagsNoFlags_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batteryVoltageMinThreshold}, 
  { gZclAttrPwrConfigIdBatteryStgBatteryVoltageThreshold1_c,  gZclDataTypeUint8_c,   gZclAttrFlagsNoFlags_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batteryVoltageThreshold1},
  { gZclAttrPwrConfigIdBatteryStgBatteryVoltageThreshold2_c,  gZclDataTypeUint8_c,   gZclAttrFlagsNoFlags_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batteryVoltageThreshold2},  
  { gZclAttrPwrConfigIdBatteryStgBatteryVoltageThreshold3_c,  gZclDataTypeUint8_c,   gZclAttrFlagsNoFlags_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batteryVoltageThreshold3}, 
  { gZclAttrPwrConfigIdBatteryStgBatteryPercentageMinThreshold_c,gZclDataTypeUint8_c,   gZclAttrFlagsNoFlags_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batteryPercentageMinThreshold}, 
  { gZclAttrPwrConfigIdBatteryStgBatteryPercentageThreshold1_c,  gZclDataTypeUint8_c,   gZclAttrFlagsNoFlags_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batteryPercentageThreshold1},
  { gZclAttrPwrConfigIdBatteryStgBatteryPercentageThreshold2_c,  gZclDataTypeUint8_c,   gZclAttrFlagsNoFlags_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batteryPercentageThreshold2},  
  { gZclAttrPwrConfigIdBatteryStgBatteryPercentageThreshold3_c,  gZclDataTypeUint8_c,   gZclAttrFlagsNoFlags_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batteryPercentageThreshold3}
};

const zclAttrSet_t gaZclPowerCfgClusterAttrSet[] = {
  {gZclAttrSetPwrConfig_c, (void *)&gaZclPowerCfgClusterAttrDef, NumberOfElements(gaZclPowerCfgClusterAttrDef)}
};

const zclAttrSetList_t gZclPowerCfgClusterAttrSetList = {
  NumberOfElements(gaZclPowerCfgClusterAttrSet),
  gaZclPowerCfgClusterAttrSet
};
#endif  

/******************************
  Device Temperature Configuration Cluster Data
  See ZCL Specification Section 3.4
*******************************/

/* Device Temperature Configuration Cluster Attribute Definitions */
const zclAttrDef_t gaZclTemperatureCfgClusterAttrDef[] = {
  //device temperature information 
  { gZclAttrIdTempCfgTempInfCurrentTemp_c, gZclDataTypeInt16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(int16_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, currentTemp)}
#if gZclClusterOptionals_d
  , { gZclAttrIdTempCfgTempInfMinTempExperienced_c, gZclDataTypeInt16_c, gZclAttrFlagsInRAM_c, sizeof(int16_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, minTempExperienced)},
  { gZclAttrIdTempCfgTempInfMaxTempExperienced_c,   gZclDataTypeInt16_c, gZclAttrFlagsInRAM_c, sizeof(int16_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, maxTempExperienced)},
  { gZclAttrIdTempCfgTempInfOverTempTotalDwell_c,   gZclDataTypeInt16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(int16_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, overTempTotalDwell)},
#endif  
  //device temperature settings
#if gZclClusterOptionals_d
  { gZclAttrIdTempCfgTempStgDeviceTempAlarmMask_c,    gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t),   (void *)MbrOfs(zclTempCfgAttrsRAM_t, deviceTempAlarmMask)},
  { gZclAttrIdTempCfgTempStgLowTempThreshold_c,       gZclDataTypeInt16_c,   gZclAttrFlagsInRAM_c, sizeof(int16_t),   (void *)MbrOfs(zclTempCfgAttrsRAM_t, lowTempThreshold)},
  { gZclAttrIdTempCfgTempStgHighTempThreshold_c,      gZclDataTypeInt16_c,   gZclAttrFlagsInRAM_c, sizeof(int16_t),   (void *)MbrOfs(zclTempCfgAttrsRAM_t, highTempThreshold)},
  { gZclAttrIdTempCfgTempStgLowTempDwellTripPoint_c,  gZclDataTypeUint24_c,  gZclAttrFlagsInRAM_c, sizeof(SumElem_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, lowTempDwellTripPoint)},
  { gZclAttrIdTempCfgTempStgHighTempDwellTripPoint_c, gZclDataTypeUint24_c,  gZclAttrFlagsInRAM_c, sizeof(SumElem_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, highTempDwellTripPoint)}
#endif 
};

const zclAttrSet_t gaZclTemperatureCfgClusterAttrSet[] = {
  {gZclAttrTempCfgSet_c, (void *)&gaZclTemperatureCfgClusterAttrDef, NumberOfElements(gaZclTemperatureCfgClusterAttrDef)}
};

const zclAttrSetList_t gaZclTemperatureCfgClusterAttrSetList = {
  NumberOfElements(gaZclTemperatureCfgClusterAttrSet),
  gaZclTemperatureCfgClusterAttrSet
};

/******************************
  Identify Cluster Data
  See ZCL Specification Section 3.5
*******************************/

extern uint8_t gZclIdentifyTimerID;
uint16_t gZclIdentifyTime = 0;

uint8_t gZclIdentifyCommissioningState = 0; /* store the EZ mode commissioning state */


/* note: only 1 identify at a time per node: this is to reduce # of timers required in BeeStack */
zbEndPoint_t gZclIdentifyEndPoint;

const zclAttrDef_t gaZclIdentifyClusterAttrDef[] = {
  { gZclAttrIdentify_TimeId_c,  gZclDataTypeUint16_c, gZclAttrFlagsNoFlags_c, sizeof(uint16_t), (void *)&gZclIdentifyTime}
#if gASL_EnableEZCommissioning_d
  ,{ gZclAttrIdentify_CommissioningState_c, gZclDataTypeBitmap8_c, gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)&gZclIdentifyCommissioningState}
#endif
};

const zclAttrSet_t gaZclIdentifyClusterAttrSet[] = {
  {gZclAttrIdentify_TimeId_c, (void *)&gaZclIdentifyClusterAttrDef, NumberOfElements(gaZclIdentifyClusterAttrDef)}
};

const zclAttrSetList_t gZclIdentifyClusterAttrSetList = {
  NumberOfElements(gaZclIdentifyClusterAttrSet),
  gaZclIdentifyClusterAttrSet
};

const zclCmd_t gaZclIdentifyClusterCmdReceivedDef[]={
  // commands received 
  gZclCmdIdentify_c,
  gZclCmdIdentifyQuery_c,
  gZclCmdEzModeInvoke_c,
  gZclCmdUpdateCommissioningState_c
};

const zclCmd_t gaZclIdentifyClusterCmdGeneratedDef[]={
  // commands generated 
  gZclCmdIdentifyQueryRsp_c
};

const zclCommandsDefList_t gZclIdentifyClusterCommandsDefList =
{
   NumberOfElements(gaZclIdentifyClusterCmdReceivedDef), gaZclIdentifyClusterCmdReceivedDef,
   NumberOfElements(gaZclIdentifyClusterCmdGeneratedDef), gaZclIdentifyClusterCmdGeneratedDef
};



/******************************
  Groups Cluster Data
  The groups cluster is concerned with management of the group table on a device.
  See ZCL Specification Section 3.6
*******************************/

uint8_t gZclGroup_NameSupport = gZclGroup_NameSupport_c;

const zclAttrDef_t gaZclGroupClusterAttrDef[] = {                                                  
  { gZclAttrGroup_NameSupport_c, gZclDataTypeBitmap8_c, gZclAttrFlagsInLine_c, sizeof(uint8_t), (void *)&gZclGroup_NameSupport}
};

const zclAttrSet_t gaZclGroupClusterAttrSet[] = {
  {gZclAttrGroup_NameSupport_c, (void *)&gaZclGroupClusterAttrDef, NumberOfElements(gaZclGroupClusterAttrDef)}
};

const zclAttrSetList_t gZclGroupClusterAttrSetList = {
  NumberOfElements(gaZclGroupClusterAttrSet),
  gaZclGroupClusterAttrSet
};

const zclCmd_t gaZclGroupClusterCmdReceivedDef[]={
  // 3.6.2 commands received 
  gZclCmdGroup_AddGroup_c,
  gZclCmdGroup_ViewGroup_c,
  gZclCmdGroup_GetGroupMembership_c,
  gZclCmdGroup_RemoveGroup_c,
  gZclCmdGroup_RemoveAllGroups_c,
  gZclCmdGroup_AddGroupIfIdentifying_c
};

const zclCmd_t gaZclGroupClusterCmdGeneratedDef[]={
  // 3.6.2 commands generated 
  gZclCmdGroup_AddGroupRsp_c,
  gZclCmdGroup_ViewGroupRsp_c,
  gZclCmdGroup_GetGroupMembershipRsp_c,
  gZclCmdGroup_RemoveGroupRsp_c
};

const zclCommandsDefList_t gZclGroupClusterCommandsDefList =
{
   NumberOfElements(gaZclGroupClusterCmdReceivedDef), gaZclGroupClusterCmdReceivedDef,
   NumberOfElements(gaZclGroupClusterCmdGeneratedDef), gaZclGroupClusterCmdGeneratedDef
};


/******************************
  Scenes Cluster Data
  See ZCL Specification Section 3.7
*******************************/

uint8_t gZclScene_NameSupport = gZclScene_NameSupport_c;

const zclAttrDef_t gaZclSceneClusterAttrDef[] = {
  { gZclAttrSceneId_SceneCount_c,       gZclDataTypeUint8_c,   gZclAttrFlagsInSceneData_c | gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zclSceneAttrs_t, sceneCount)   },
  { gZclAttrSceneId_CurrentScene_c,     gZclDataTypeUint8_c,   gZclAttrFlagsInSceneData_c | gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zclSceneAttrs_t, currentScene) },
  { gZclAttrSceneId_CurrentGroup_c,     gZclDataTypeUint16_c,  gZclAttrFlagsInSceneData_c | gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *)MbrOfs(zclSceneAttrs_t, currentGroup) },
  { gZclAttrSceneId_SceneValid_c,       gZclDataTypeBool_c,    gZclAttrFlagsInSceneData_c | gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),  (void *)MbrOfs(zclSceneAttrs_t, sceneValid) },
  { gZclAttrSceneId_NameSupport_c,      gZclDataTypeBitmap8_c, gZclAttrFlagsInLine_c, sizeof(uint8_t),  (void *)&gZclScene_NameSupport }
#if gZclClusterOptionals_d
  , { gZclAttrSceneId_LastConfiguredBy_c, gZclDataTypeIeeeAddr_c, gZclAttrFlagsInSceneData_c | gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zbIeeeAddr_t),  (void *)MbrOfs(zclSceneAttrs_t, lastConfiguredBy) }
#endif
};

const zclAttrSet_t gaZclSceneClusterAttrSet[] = {
  {gZclAttrSceneIdSet_c, (void *)&gaZclSceneClusterAttrDef, NumberOfElements(gaZclSceneClusterAttrDef)}
};

const zclAttrSetList_t gZclSceneClusterAttrSetList = {
  NumberOfElements(gaZclSceneClusterAttrSet),
  gaZclSceneClusterAttrSet
};


const zclCmd_t gaZclSceneClusterCmdReceivedDef[]={
  // 3.7.2 commands received 
  gZclCmdScene_AddScene_c,
  gZclCmdScene_ViewScene_c,
  gZclCmdScene_RemoveScene_c,
  gZclCmdScene_RemoveAllScenes_c,
  gZclCmdScene_StoreScene_c,
  gZclCmdScene_RecallScene_c,
  gZclCmdScene_GetSceneMembership_c
};

const zclCmd_t gaZclSceneClusterCmdGeneratedDef[]={
  // 3.7.2 commands generated 
  gZclCmdScene_AddSceneRsp_c,
  gZclCmdScene_ViewSceneRsp_c,
  gZclCmdScene_RemoveSceneRsp_c,
  gZclCmdScene_RemoveAllScenesRsp_c,
  gZclCmdScene_StoreSceneRsp_c,
  gZclCmdScene_GetSceneMembershipRsp_c
};

const zclCommandsDefList_t gZclSceneClusterCommandsDefList =
{
   NumberOfElements(gaZclSceneClusterCmdReceivedDef), gaZclSceneClusterCmdReceivedDef,
   NumberOfElements(gaZclSceneClusterCmdGeneratedDef), gaZclSceneClusterCmdGeneratedDef
};

/******************************
  On/Off Cluster Data
  See ZCL Specification Section 3.8
*******************************/

/* used by OnOffLight, DimmingLight, and any other device that supports the OnOff cluster server */
const zclAttrDef_t gaZclOnOffClusterAttrDef[] = {
  { gZclAttrIdOnOff_OnOffId_c, gZclDataTypeBool_c,   gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c| gZclAttrFlagsReportable_c | gZclAttrFlagsInSceneTable_c, sizeof(uint8_t), (void *)MbrOfs(zclOnOffAttrsRAM_t, onOff)}
};

const zclAttrSet_t gaZclOnOffClusterAttrSet[] = {
  {gZclAttrOnOffSet_c, (void *)&gaZclOnOffClusterAttrDef, NumberOfElements(gaZclOnOffClusterAttrDef)}
};

const zclAttrSetList_t gZclOnOffClusterAttrSetList = {
  NumberOfElements(gaZclOnOffClusterAttrSet),
  gaZclOnOffClusterAttrSet
};


const zclCmd_t gaZclOnOffClusterCmdReceivedDef[]={
  // commands received 
  gZclCmdOnOff_Off_c,
  gZclCmdOnOff_On_c,
  gZclCmdOnOff_Toggle_c
};

const zclCommandsDefList_t gZclOnOffClusterCommandsDefList =
{
   NumberOfElements(gaZclOnOffClusterCmdReceivedDef), gaZclOnOffClusterCmdReceivedDef,
   0, NULL
};



/******************************
  On/Off Switch Configure Cluster Data
  See ZCL Specification Section 3.9
*******************************/
/* used by OnOffSwitch,Configuration Tool , and any other device that supports the OnOff Switch Configure cluster server */
const zclAttrDef_t gaZclOnOffSwitchConfigureClusterAttrDef[] = {
  { gZclAttrIdOnOffSwitchCfg_SwitchType_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zclOnOffSwitchCfgAttrsRAM_t, SwitchType) },
  { gZclAttrIdOnOffSwitchCfg_SwitchAction_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(zclOnOffSwitchCfgAttrsRAM_t, SwitchAction) }
};

const zclAttrSet_t gaZclOnOffSwitchConfigureClusterAttrSet[] = {    
  { gZclAttrOnOffSwitchCfgSet_c, gaZclOnOffSwitchConfigureClusterAttrDef, NumberOfElements(gaZclOnOffSwitchConfigureClusterAttrDef)}
};


const zclAttrSetList_t gZclOnOffSwitchConfigureClusterAttrSetList = {    
  NumberOfElements(gaZclOnOffSwitchConfigureClusterAttrSet),
  gaZclOnOffSwitchConfigureClusterAttrSet
};

/******************************
  Alarms Cluster Data
  See ZCL Specification Section 3.11
*******************************/
zclAlarmsAttrsRAM_t gZclAlarmAttr;

const zclAttrDef_t gaZclAlarmsClusterAttrDef[] = {
  {gZclAttrIdAlarms_AlarmCount_c, gZclDataTypeUint16_c, gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)&gZclAlarmAttr},
};

const zclAttrSet_t gaZclAlarmsClusterAttrSet[] = {
  {gZclAttrSetAlarms_AlarmInformation_c, (void *)&gaZclAlarmsClusterAttrDef, NumberOfElements(gaZclAlarmsClusterAttrDef)}
};

const zclAttrSetList_t gZclAlarmsClusterAttrSetList = {    
  NumberOfElements(gaZclAlarmsClusterAttrSet),
  gaZclAlarmsClusterAttrSet
};

const zclCmd_t gaZclAlarmsClusterCmdReceivedDef[]={
  //3.11.2.4 commands received 
  gAlarmClusterRxCommandID_ResetAlarm_c,
  gAlarmClusterRxCommandID_ResetAllAlarms_c,
  gAlarmClusterRxCommandID_GetAlarm_c,
  gAlarmClusterRxCommandID_ResetAlarmLog_c
};

const zclCmd_t gaZclAlarmsClusterCmdGeneratedDef[]={
  //3.11.2.5 commands generated 
  gAlarmClusterTxCommandID_Alarm_c,
  gAlarmClusterTxCommandID_GetAlarmResponse_c
};

const zclCommandsDefList_t gZclAlarmsClusterCommandsDefList =
{
   NumberOfElements(gaZclAlarmsClusterCmdReceivedDef),  gaZclAlarmsClusterCmdReceivedDef,
   NumberOfElements(gaZclAlarmsClusterCmdGeneratedDef), gaZclAlarmsClusterCmdGeneratedDef
};


/******************************
  Level Control Cluster Data
  See ZCL Specification Section 3.10
*******************************/
uint8_t             gZclLevel_TransitionTimerID;   /* used for the Level control cluster commands*/
zclLevelValue_t     gZclCurrentLevelTmp = gZclLevel_off;
zclLevelValue_t     gZclNewCurrentLevel = gZclLevel_off;
zclLevelValue_t     gZclLevel_diference = 0;
zclLevelValue_t     gZclLevel_LastCurrentLevel=0;
zclCmd_t            gZclLevel_OnOffState = gZclCmdOnOff_Off_c;
uint16_t            gZclLevel_TimeBetweenChanges=0;
uint8_t             gZclLevel_MoveMode=0;
bool_t              gZclLevel_OnOff=FALSE;
bool_t              gZclLevel_DimmingStopReport=FALSE;
uint8_t             gZclLevel_Step=1;

zbEndPoint_t        gZcl_ep;
tmrTimeInMilliseconds_t gZclLevel_RemainingTimeInMilliseconds=0;
uint8_t       gZclLevelCmd = gZclLevel_NotOnOffCmd_c;


/* Level Cluster Attribute Definitions */
const zclAttrDef_t gaZclLevelCtrlClusterAttrDef[] = {
   { gZclAttrIdLevelControl_CurrentLevelId_c,        gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsReportable_c |gZclAttrFlagsInSceneTable_c, sizeof(uint8_t), (void*)MbrOfs(zclLevelCtrlAttrsRAM_t,currentLevel)}
#if gZclClusterOptionals_d
  ,{ gZclAttrIdLevelControl_RemainingTimeId_c,       gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(zbLevelCtrlTime_t),(void*)MbrOfs(zclLevelCtrlAttrsRAM_t,remainingTime)  }
  ,{ gZclAttrIdLevelControl_OnOffTransitionTimeId_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(zbLevelCtrlTime_t), (void*)MbrOfs(zclLevelCtrlAttrsRAM_t,onOffTransitionTime) }
  ,{ gZclAttrIdLevelControl_OnLevelId_c,             gZclDataTypeUint8_c,  gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void*)MbrOfs(zclLevelCtrlAttrsRAM_t,onLevel) }
  ,{ gZclAttrIdLevelControl_OnTransitionTimeId_c,    gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(zbLevelCtrlTime_t), (void*)MbrOfs(zclLevelCtrlAttrsRAM_t,onTransitionTime) }
  ,{ gZclAttrIdLevelControl_OffTransitionTimeId_c,   gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(zbLevelCtrlTime_t), (void*)MbrOfs(zclLevelCtrlAttrsRAM_t,offTransitionTime) }
  ,{ gZclAttrIdLevelControl_DefaultMoveRateId_c,     gZclDataTypeUint8_c,  gZclAttrFlagsInRAM_c, sizeof(uint8_t),           (void*)MbrOfs(zclLevelCtrlAttrsRAM_t,defaultMoveRate) }
#endif /* optionals */
};

const zclAttrSet_t gaZclLevelCtrlClusterAttrSet[] = {
  {gZclAttrSetLevelCtrl_c, (void *)&gaZclLevelCtrlClusterAttrDef, NumberOfElements(gaZclLevelCtrlClusterAttrDef)}
};

const zclAttrSetList_t gZclLevelCtrlClusterAttrSetList = {
  NumberOfElements(gaZclLevelCtrlClusterAttrSet),
  gaZclLevelCtrlClusterAttrSet
};

const zclCmd_t gaZclLevelCtrlClusterCmdReceivedDef[]={
  // 3.10.2 commands received 
  gZclCmdLevelControl_MoveToLevel_c,
  gZclCmdLevelControl_Move_c,
  gZclCmdLevelControl_Step_c,
  gZclCmdLevelControl_Stop_c,
  gZclCmdLevelControl_MoveToLevelOnOff_c,
  gZclCmdLevelControl_MoveOnOff_c,
  gZclCmdLevelControl_StepOnOff_c,
  gZclCmdLevelControl_StopOnOff_c
};

const zclCommandsDefList_t gZclLevelCtrlClusterCommandsDefList =
{
   NumberOfElements(gaZclLevelCtrlClusterCmdReceivedDef), gaZclLevelCtrlClusterCmdReceivedDef,
   0, NULL
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/******************************
  Basic Cluster
  See ZCL Specification Section 3.2
*******************************/

/*!
 * @fn 		void ZCL_ResetDevice(afDeviceDef_t *pDevice) 
 *
 * @brief	ZCL Reset Device
 *
 */
void ZCL_ResetDevice
  (
  afDeviceDef_t *pDevice  /* IN: device definition to reset */
  )
{
  if (pDevice->pData != NULL) {   
 //   *(uint8_t *)pDevice->pReportList = 0;   /* clear the reporting bits */
  }
}

/*!
 * @fn 		zbStatus_t ZCL_BasicClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Basic Cluster Server. 
 *
 */
zbStatus_t ZCL_BasicClusterServer
(
	zbApsdeDataIndication_t *pIndication,
	afDeviceDef_t *pDevice
)
{
	/* avoid compiler warnings */
	(void)pIndication;
	(void)pDevice;

	return gZclUnsupportedClusterCommand_c;
}

/******************************
  Power Configuration Cluster
  See ZCL Specification Section 3.3
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_PowerCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the PowerConfiguration Cluster Server. 
 *
 */
zbStatus_t ZCL_PowerCfgClusterServer
(
      zbApsdeDataIndication_t *pIndication, 
      afDeviceDef_t *pDevice
)
{
      (void) pIndication;
      (void) pDevice;
      
      return gZclUnsupportedClusterCommand_c;
}

/******************************
  Temperature Configuration Cluster
  See ZCL Specification Section 3.4
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_TempCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Temperature Configuration Cluster Server. 
 *
 */
zbStatus_t ZCL_TempCfgClusterServer
(
      zbApsdeDataIndication_t *pIndication, 
      afDeviceDef_t *pDevice
)
{
      (void) pIndication;
      (void) pDevice;
      
     return gZclUnsupportedClusterCommand_c;
}

/******************************
  Binary Input(Basic) Cluster
  See ZCL Specification Section 3.4
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_BinaryInputClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the BinaryInput Cluster Server. 
 *
 */
zbStatus_t ZCL_BinaryInputClusterServer
(
      zbApsdeDataIndication_t *pIndication, 
      afDeviceDef_t *pDevice
)
{
      (void) pIndication;
      (void) pDevice;
      
     return gZclUnsupportedClusterCommand_c;
}

/******************************
  Identify Cluster
  See ZCL Specification Section 3.5
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_IdentifyClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Identify Cluster Client. 
 *
 */
zbStatus_t ZCL_IdentifyClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  zclCmd_t command;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  
  /* prevent compiler warning */
  (void)pDevice;
  pFrame = (void *)pIndication->pAsdu;
  
  
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	  status = gZclSuccess_c;
  
  /* handle the command */
  command = pFrame->command;
  
  if (command == gZclCmdIdentifyQueryRsp_c)
  {
    (void)ZCL_IdentifyQueryRspHandler(pIndication);
    return status;
  }
  else
    return gZclUnsupportedClusterCommand_c;
}


/*!
 * @fn 		zbStatus_t ZCL_IdentifyClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Identify Cluster Server. 
 *
 */
zbStatus_t ZCL_IdentifyClusterServer
(
	zbApsdeDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDevice                /* IN: */
)
{
    zclFrame_t *pFrame;
    uint16_t time;
    zbStatus_t status = gZclSuccessDefaultRsp_c;
    
    /* avoid compiler warnings */
    (void)pDevice;

    /* check command */
    pFrame = (void *)(pIndication->pAsdu);
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	status = gZclSuccess_c;
	
    switch (pFrame->command) 
    {
      case gZclCmdIdentify_c:
        ZCL_SetIdentifyMode(pIndication->dstEndPoint, (*((uint16_t *)(pFrame + 1))));
        return status; 
      case gZclCmdIdentifyQuery_c:
	/* respond back to caller if in identify mode */
        if(gZclIdentifyTime)
        {
            time = gZclIdentifyTime;
            pFrame->command = gZclCmdIdentifyQueryRsp_c;
            return ZCL_Reply(pIndication, sizeof(time), &time);
	}    
        return status;
      case gZclCmdEzModeInvoke_c:
#if gASL_EnableEZCommissioning_d    	
        EZComissioning_Start(*((uint8_t*)(pFrame + 1)));
#else
        status = gZbFailed_c;
#endif      
        return status;
      case gZclCmdUpdateCommissioningState_c:
#if gASL_EnableEZCommissioning_d    	
        switch(*((uint8_t*)(pFrame + 1)))
        {
          case gZclCmdUpdateCommissioningState_Set_c:
          {
            gZclIdentifyCommissioningState |= (*(((uint8_t*)(pFrame + 1)) + 1));
            break;
          }
          case gZclCmdUpdateCommissioningState_Clear_c:
          {
            gZclIdentifyCommissioningState &= ~(*(((uint8_t*)(pFrame + 1)) + 1));
          }
        }
      #if (!gASL_EnableEZCommissioning_Initiator_d)
        if(gZclIdentifyCommissioningState&gZclCommissioningState_OperationalState_d)
            BeeAppUpdateDevice(0, gZclUI_EZCommissioning_Succesfull_c, 0, 0, NULL);
      
        ZCL_SaveNvmZclData();
      #endif 
#else
       status = gZbFailed_c; 
#endif 
      return status;
    default:
      return gZclUnsupportedClusterCommand_c; 
  }
}

/*!
 * @fn 		void ZCL_IdentifyTimer( uint8_t timerId )
 *
 * @brief	Local command for entering identify mode for a period of time.
 *
 */
void ZCL_IdentifyTimer( uint8_t timerId )
{
    uint16_t time;

    /* avoid compiler warning */
    (void)timerId;

    /* decrement the time */
    if(gZclIdentifyTime && gZclIdentifyTime != 0xffff)
    {
	time = OTA2Native16(gZclIdentifyTime);
	--time;
	gZclIdentifyTime = Native2OTA16(time);
#if gASL_EnableEZCommissioning_d   
        if((gEZCommissioning_LastEvent == gStartIdentify_c)||
           (gEZCommissioning_LastEvent == gUpdatePollRate_c) ||
           (gEZCommissioning_LastEvent == gSendIdentifyReq_c))
        {
          if(gZclIdentifyTime%EZIdQueryTime_c == 0)
          {
            TS_SendEvent(gEzCmsTaskId, gSendIdentifyReq_c);
          }
        }
#endif 
        
        
        if(!gZclIdentifyTime)
        {
            TMR_StopTimer(gZclIdentifyTimerID);
#if gASL_EnableEZCommissioning_d  
            TS_SendEvent(gEzCmsTaskId, gIdentifyEnd_c);
#endif
            BeeAppUpdateDevice(gZclIdentifyEndPoint, gIdentifyOff_c, 0, 0, NULL);
        }
    }
}

/*!
 * @fn 		void ZCL_SetIdentifyMode(zbEndPoint_t endPoint,	uint16_t time) 
 *
 * @brief	Local command for entering identify mode for a period of time. Set to 0xffff
 * 			for forever. Set to 0x0000 to turn identify off.
 *
 */
void ZCL_SetIdentifyMode
(
	zbEndPoint_t endPoint,
	uint16_t time /* IN: in seconds */
)
{
	uint8_t event;

	/* set up identify mode */
	gZclIdentifyEndPoint = endPoint;
	gZclIdentifyTime = time;

	/* turn identify on */
	if(time)
	{
		event = gIdentifyOn_c;
       	TMR_StartTimer(gZclIdentifyTimerID, gTmrLowPowerIntervalMillisTimer_c, 1000 , ZCL_IdentifyTimer);
	}
	/* turn identify off */
	else
	{
		event = gIdentifyOff_c;	
		TMR_StopTimer(gZclIdentifyTimerID);
#if gASL_EnableEZCommissioning_d  
        TS_SendEvent(gEzCmsTaskId, gIdentifyEnd_c);
#endif
	}

	/* let UI know we're now in/out of identify mode */
        BeeAppUpdateDevice(endPoint, event, 0, 0, NULL);
}


/*!
 * @fn 		void ZCL_IdentifyTimeLeft( zbEndPoint_t endPoint )
 *
 * @brief	Local command for determining time left in identify mode
 *
 */
uint16_t ZCL_IdentifyTimeLeft
(
	zbEndPoint_t endPoint
)
{
	(void)endPoint; /* avoid compiler warnings */
	return gZclIdentifyTime;
}

/*!
 * @fn 		zbStatus_t zclIdentify_IdentifyQueryReq(zclIdentifyQueryReq_t *pReq, uint8_t len) 
 *
 * @brief	Sends over-the-air an IdentifyQueryRequest command from the Identify Cluster Client. 
 *
 */
zbStatus_t zclIdentify_IdentifyQueryReq
(
    zclIdentifyQueryReq_t *pReq, uint8_t len
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIdentify_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdIdentifyQuery_c, len,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclIdentify_IdentifyQueryRsp(zclIdentifyQueryRsp_t *pReq, uint8_t len)
 *
 * @brief	Sends over-the-air an IdentifyQueryResponse command from the Identify Cluster Server. 
 *
 */
zbStatus_t zclIdentify_IdentifyQueryRsp
(
    zclIdentifyQueryRsp_t *pReq, uint8_t len
)
{
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIdentify_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdIdentifyQueryRsp_c, len,(zclGenericReq_t *)pReq);
}


/*!
 * @fn 		zbStatus_t zclIdentify_IdentifyQueryReq(zclIdentifyQueryReq_t *pReq, uint8_t len) 
 *
 * @brief	Process the IdentifyQueryResponse command received from the Identify Cluster Server. 
 *
 */
static zbStatus_t ZCL_IdentifyQueryRspHandler(zbApsdeDataIndication_t *pIndication)
{
#if gASL_EnableEZCommissioning_d
  if(gEZCommissioning_LastEvent == gSendIdentifyReq_c)
  {
    FLib_MemCpy(gEZCommissioningTempData, pIndication, sizeof(zbApsdeDataIndication_t));
    TS_SendEvent(gEzCmsTaskId, gReceivedIdentifyRsp_c);
  }
#endif    
  return gZclSuccess_c;
  
}
/******************************
  Groups Cluster
  See ZCL Specification Section 3.6
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_GroupClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Group Cluster Client. 
 *
 */
zbStatus_t ZCL_GroupClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  zclCmd_t command;

  /* prevent compiler warning */
  (void)pDevice;
  pFrame = (void *)pIndication->pAsdu;
  /* handle the command */
  command = pFrame->command;
  
  switch(command) {
    case gZclCmdGroup_AddGroupRsp_c:
    case gZclCmdGroup_ViewGroupRsp_c:
    case gZclCmdGroup_GetGroupMembershipRsp_c:
    case gZclCmdGroup_RemoveGroupRsp_c:
      return (pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)?gZclSuccess_c:gZclSuccessDefaultRsp_c;
  default:
      return gZclUnsupportedClusterCommand_c;
  }
}

/*!
 * @fn 		zbStatus_t ZCL_GroupClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Group Cluster Server. 
 *
 */
zbStatus_t ZCL_GroupClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  zclCmd_t command;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  zbStatus_t (*pGroupFunc)(zbApsdeDataIndication_t *pIndication, void *command);  
  pGroupFunc = NULL;
  
  /* prevent compiler warning */
  (void)pDevice;

  pFrame = (void *)pIndication->pAsdu;
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	   status = gZclSuccess_c;
	
	
  /* handle the command */
  command = pFrame->command;
  
  switch(command) {
    /* add group if identifying */
  	case gZclCmdGroup_AddGroupIfIdentifying_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command))  ZCL_GroupAddGroupIfIdentify;  
      break;

    /* add group to endpoint */
    case gZclCmdGroup_AddGroup_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command)) ZCL_GroupAddGroupRsp;
      status = gZclSuccess_c;
      break;

    /* view the group */
    case gZclCmdGroup_ViewGroup_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command)) ZCL_GroupViewGroupRsp;
      status = gZclSuccess_c;
      break;
    /* check what groups this endpoint belongs to */
  	case gZclCmdGroup_GetGroupMembership_c:
  	  pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command)) ZCL_GroupGetGroupMembershipRsp;
  	  status = gZclSuccess_c;
	  break;

    /* remove the group */
    case gZclCmdGroup_RemoveGroup_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command)) ZCL_GroupRemoveGroupRsp;
      status = gZclSuccess_c;
      break;

    /* remove all groups from the endpoint */
    case gZclCmdGroup_RemoveAllGroups_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command)) ZCL_GroupRemoveAllGroups;
      break;
      
    default:
      return gZclUnsupportedClusterCommand_c;
    }
    if (pGroupFunc)
      (void)pGroupFunc(pIndication, (void *)(pFrame + 1));

  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_GroupAddGroupRsp(zbApsdeDataIndication_t *pIndication, zclCmdGroup_AddGroup_t *command) 
 *
 * @brief	Process AddGroup command and sends over-the-air an AddGroupResponse from the Group Cluster Server. 
 *
 */
zbStatus_t ZCL_GroupAddGroupRsp
(
  zbApsdeDataIndication_t *pIndication,
  zclCmdGroup_AddGroup_t*command
)
{
	zclCmdGroup_AddGroupRsp_t Resp;
	uint8_t iPayloadLen;
	zbApsmeAddGroupReq_t  ApsmeAddGroup;


	Copy2Bytes(ApsmeAddGroup.aGroupId,command->aGroupId);
	Copy2Bytes(Resp.aGroupId,command->aGroupId);
	ApsmeAddGroup.endPoint = pIndication->dstEndPoint;

        if(ApsGroupIsMemberOfEndpoint(Resp.aGroupId,ApsmeAddGroup.endPoint))
        {
          Resp.status = gZclDuplicateExists_c;
        } else
        {
	  Resp.status = (zclStatus_t)APSME_AddGroupRequest(&ApsmeAddGroup);
	  if(gApsTableFull_c == Resp.status)
	    Resp.status = gZclInsufficientSpace_c;

        }
	iPayloadLen = sizeof(zclCmdGroup_AddGroupRsp_t);
	
	return ZCL_Reply(pIndication, iPayloadLen, &Resp);
}

/*!
 * @fn 		zbStatus_t ZCL_GroupAddGroupIfIdentify(zbApsdeDataIndication_t *pIndication, zclCmdGroup_AddGroupIfIdentifying_t *command) 
 *
 * @brief	Process AddGroupIfIdentify command received from the Group Cluster Client. 
 *
 */
zbStatus_t ZCL_GroupAddGroupIfIdentify
(
  zbApsdeDataIndication_t *pIndication,
  zclCmdGroup_AddGroupIfIdentifying_t*command
)
{
  if(gZclIdentifyTime) {
  	zbApsmeAddGroupReq_t  ApsmeAddGroup;
    Copy2Bytes(ApsmeAddGroup.aGroupId,command->aGroupId);
    ApsmeAddGroup.endPoint = pIndication->dstEndPoint;
  	return APSME_AddGroupRequest(&ApsmeAddGroup);
  }
  return gZbSuccess_c;
}

/*!
 * @fn 		zbStatus_t ZCL_GroupRemoveGroupRsp(zbApsdeDataIndication_t *pIndication, zclCmdGroup_RemoveGroup_t *command) 
 *
 * @brief	Process RemoveGroup command and sends over-the-air an RemoveGroupResponse from the Group Cluster Server. 
 *
 */
zbStatus_t ZCL_GroupRemoveGroupRsp
(
  zbApsdeDataIndication_t *pIndication,
  zclCmdGroup_RemoveGroup_t*command
)
{
  zclCmdGroup_RemoveGroupRsp_t Resp;
  zbApsmeRemoveGroupReq_t  ApsmeRemoveGroup;
  uint8_t iPayloadLen;

  Copy2Bytes(ApsmeRemoveGroup.aGroupId,command->aGroupId);
	ApsmeRemoveGroup.endPoint = pIndication->dstEndPoint;
  Resp.status = APSME_RemoveGroupRequest(&ApsmeRemoveGroup);
  if((gApsInvalidGroup_c == Resp.status) || (gApsInvalidParameter_c == Resp.status))
    Resp.status = gZclNotFound_c;  
  Copy2Bytes(Resp.aGroupId,command->aGroupId);

  iPayloadLen = sizeof(Resp);
  return ZCL_Reply(pIndication, iPayloadLen, &Resp);
}

/*!
 * @fn 		zbStatus_t ZCL_GroupRemoveAllGroups(zbApsdeDataIndication_t *pIndication, zclCmdGroup_RemoveAllGroups_t *command) 
 *
 * @brief	Process RemoveAllGroups command received from the Group Cluster Client. 
 *
 */
zbStatus_t ZCL_GroupRemoveAllGroups
(
  zbApsdeDataIndication_t *pIndication,
  zclCmdGroup_RemoveAllGroups_t*command
)
{
  zbApsmeRemoveAllGroupsReq_t  ApsmeRemoveAllGroups;

  (void) command; /*To avoid compile errors*/
  ApsmeRemoveAllGroups.endPoint = pIndication->dstEndPoint;

  return APSME_RemoveAllGroupsRequest(&ApsmeRemoveAllGroups);
}

/*!
 * @fn 		zbStatus_t ZCL_FindGroup(zbGroupId_t aGroupId) 
 *
 * @brief	Return succes if the group is in the group table, otherwise return NotFound
 *
 */
zclStatus_t ZCL_FindGroup
(
  zbGroupId_t  aGroupId
)
{
	zclStatus_t  status;
	zbGroupId_t  InvalidGroup={0xF8,0xFF};
	int i;

	status = gZclInvalidValue_c;  /*In case the group to be search is 0x0000 or 0xFFF7*/	
	if (!IsEqual2Bytes(aGroupId,InvalidGroup) && !Cmp2BytesToZero(aGroupId))
	{
		status = gZclNotFound_c;
		for(i = 0; i < gApsMaxGroups; ++i)
		{
			if (!IsEqual2Bytes(gpGroupTable[i].aGroupId,aGroupId))
				/*If group is no the one we are looking then continue to next one in the list */
				continue;

			/*Only in case that the group is found then stop the loop*/
			status = gZbSuccess_c;
			break;
			}
		}
	return status;
}

/*!
 * @fn 		zbStatus_t ZCL_GroupViewGroupRsp(zbApsdeDataIndication_t *pIndication, zclCmdGroup_ViewGroup_t *command) 
 *
 * @brief	Process ViewGroup command and sends over-the-air the ViewGroupResponse command from the Group Cluster Server. 
 *
 */
zclStatus_t ZCL_GroupViewGroupRsp
(
	zbApsdeDataIndication_t *pIndication,
	zclCmdGroup_ViewGroup_t*command
)
{
	zclCmdGroup_ViewGroupRsp_t Resp;
	uint8_t iPayloadLen;

	Resp.status = ZCL_FindGroup(command->aGroupId);
	/*
	The String Names are optionals in Ha, for size reasons in BeeStack are not supported
	instead a NULL empty string is send out all the time.
	*/
	Resp.szName[0] = 0x00;
	iPayloadLen = sizeof(zclCmdGroup_ViewGroupRsp_t);

	Copy2Bytes(Resp.aGroupId, command->aGroupId);

	return ZCL_Reply(pIndication, iPayloadLen, &Resp);
}

/*!
 * @fn 		zbStatus_t ZCL_GroupGetGroupMembershipRsp(zbApsdeDataIndication_t *pIndication, zclCmdGroup_GetGroupMembership_t *command) 
 *
 * @brief	Process GetGroupMembership command and sends over-the-air the GetGroupMembershipResponse command from the Group Cluster Server. 
 *
 */
zbStatus_t ZCL_GroupGetGroupMembershipRsp
(
	zbApsdeDataIndication_t *pIndication,
	zclCmdGroup_GetGroupMembership_t*command
)
{
	zbIndex_t   iGroupEntry;
	zclCmdGroup_GetGroupMembershipRsp_t   *pResp;
	zbGroupTable_t *pPtr;
	afToApsdeMessage_t *pMsg;

	zbSize_t iPayloadLen;
	uint8_t limit;
	zbCounter_t  count;
	zbCounter_t  NumOfGroups = 0;


	pMsg = AF_MsgAlloc();
	if (!pMsg)
		return gZbNoMem_c;

	BeeUtilZeroMemory(pMsg, gMaxRxTxDataLength_c);
	pResp = (zclCmdGroup_GetGroupMembershipRsp_t *)(((uint8_t *)pMsg) + gAsduOffset_c + sizeof(zclFrame_t));

	count = 0;
	limit = (!(command->count))? 0:1;
	for (; command->count >= limit; command->count--)
	{
		NumOfGroups = 0;
		for(iGroupEntry=0; iGroupEntry<gApsMaxGroups; ++iGroupEntry)
		{
			pPtr = &gpGroupTable[iGroupEntry];

			if (!Cmp2BytesToZero(pPtr->aGroupId))
			{
				if((!limit) || (IsEqual2Bytes(command->aGroupId[count], pPtr->aGroupId)))
				{
					// if the group matches the group table entry
					Copy2Bytes(pResp->aGroupId[pResp->count], pPtr->aGroupId);
					pResp->count++;
				}
				NumOfGroups++;	
			}
		}

		if (!limit)
		{
			break;
		}
		else
		{
			count++;
		}
	}		

	pResp->capacity = gApsMaxGroups - NumOfGroups;

	iPayloadLen = MbrOfs(zclCmdGroup_GetGroupMembershipRsp_t,aGroupId[0])+(pResp->count * sizeof(zbClusterId_t));

	if((pResp->count)||(!limit))
		return ZCL_ReplyNoCopy(pIndication, iPayloadLen, pMsg);
	else
	{
	    zclFrame_t     *pFrame;
	    pFrame = (void *)pIndication->pAsdu;
		return (pFrame->frameControl&gZclFrameControl_DisableDefaultRsp)?gZbSuccess_c:gZclSuccessDefaultRsp_c;
	}
}

/******************************
  Scenes Cluster
  See ZCL Specification Section 3.7
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_SceneClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Scene Cluster Client. 
 *
 */
zbStatus_t ZCL_SceneClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  zclCmd_t command;
  
  /* prevent compiler warning */
  (void)pDevice;
  pFrame = (void *)pIndication->pAsdu;
  /* handle the command */
  command = pFrame->command;
  
  switch(command) {
/* responses to above commands */
  case gZclCmdScene_AddSceneRsp_c:
  case gZclCmdScene_ViewSceneRsp_c:
  case gZclCmdScene_RemoveSceneRsp_c:
  case gZclCmdScene_RemoveAllScenesRsp_c:
  case gZclCmdScene_StoreSceneRsp_c:
  case gZclCmdScene_GetSceneMembershipRsp_c:
    return (pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)?gZclSuccess_c:gZclSuccessDefaultRsp_c;
  default:
    return gZclUnsupportedClusterCommand_c;
  }
}

/*!
 * @fn 		zbStatus_t ZCL_SceneClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Scene Cluster Server. 
 *
 */
zbStatus_t ZCL_SceneClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
{
  zclFrame_t      *pFrame;
  zclSceneTableEntry_t *pScene;          /* scene table entry */
  zclSceneReq_t   *pSceneReq;
  uint8_t         *pData;
  zclSceneAttrs_t *pSceneData;
  zclCmdScene_AddSceneRsp_t sceneRsp; /* generic short response (status, group, sceneID) */
  uint8_t         payloadLen;
  bool_t          fNoReply = FALSE;
  zbStatus_t      status = gZclSuccessDefaultRsp_c;
  
#if  gZclClusterOptionals_d 
  uint8_t         ieeeAddress[8];
  uint8_t         *pSrcLongAddress =  APS_GetIeeeAddress(pIndication->aSrcAddr, ieeeAddress);
  bool_t          bModifyConfiguratorAddr = FALSE;  
#endif


  /* get ptr to device data for this endpoint */
  pData = pDevice->pData;
  pSceneData = (zclSceneAttrs_t*) pDevice->pSceneData;      

  /* get ptr to ZCL frame */
  pFrame = (void *)pIndication->pAsdu;
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	   status = gZclSuccess_c;
  
  
  /* get a pointer to the scene (add, view, restore, etc..) request portion of the zcl frame */
  pSceneReq = (zclSceneReq_t *)(pFrame + 1);

  /* create common reply */
  sceneRsp.status = gZbSuccess_c;
  FLib_MemCpy(&sceneRsp.aGroupId, pSceneReq, sizeof(sceneRsp) - sizeof(sceneRsp.status));
  payloadLen = sizeof(sceneRsp);
  
  
  if (pSceneData)
  {
    /* many commands need to know if the scene exists */
    pScene = ZCL_FindScene(pDevice, (void *)(pSceneReq));  
    if (ZCL_FindGroup(pSceneReq->addScene.aGroupId)!= gZbSuccess_c)
    {
      sceneRsp.status = gZclInvalidField_c;
      if (gZclCmdScene_RemoveAllScenes_c == pFrame->command)
        payloadLen -= sizeof(zclSceneId_t); /* don't send the sceneId for RemoveAllScenesCmd */  
    }
    else{
      /* handle the scene command */
      switch(pFrame->command)
      {
        /* Add a scene */
        case gZclCmdScene_AddScene_c:
    
          status = gZclSuccess_c;	
          /* add the scene (or get the slot if it already exists) */
          pScene = ZCL_AllocateScene(pDevice, (void *)(pSceneReq));
    
          /* no room to add the scene */
          if(!pScene)
            sceneRsp.status = gZclInsufficientSpace_c;
    
          /* add the scene (similar to store scene) */
          else {          
            sceneRsp.status = ZCL_AddScene(pScene, &pSceneReq->addScene, pIndication->asduLength - sizeof(zclFrame_t));
            ZCL_SaveNvmZclData();
            #if  gZclClusterOptionals_d   
            if (gZbSuccess_c == status) 
              bModifyConfiguratorAddr = TRUE;  
            #endif
          }
          break;
    
        /* View scene */
        case gZclCmdScene_ViewScene_c:
          status = gZclSuccess_c;
          if (pScene)
          {
            sceneRsp.status = ZCL_ViewScene(pScene, pIndication, pDevice);
          } else
          { 
            sceneRsp.status = gZclNotFound_c;
          }

          /* reply already sent if no error */
          if(sceneRsp.status == gZbSuccess_c)
            fNoReply = TRUE;
          break;
    
        /* Remove scene */
        case gZclCmdScene_RemoveScene_c:
          status = gZclSuccess_c;
          /* found, remove it */
          if(pScene) {
            BeeUtilZeroMemory(pScene, sizeof(zclSceneTableEntry_t));
            --(pSceneData->sceneCount);
            ZCL_SaveNvmZclData();
          }
          else
            sceneRsp.status = gZclNotFound_c;
          break;
    
        /* Remove all scenes */
        case gZclCmdScene_RemoveAllScenes_c:    
          status = gZclSuccess_c;
          /* reset all the scenes with the specified group id */
          ZCL_ResetScenes(pDevice, sceneRsp.aGroupId);
          /* send back shortened response */
          payloadLen -= sizeof(zclSceneId_t);
          break;
    
        /* Store scene */
        case gZclCmdScene_StoreScene_c:
          status = gZclSuccess_c;
          /* (re)allocate the scene for storage */
          pScene = ZCL_AllocateScene(pDevice, (void *)pSceneReq);
    
          /* store current data to the scene */
          if(pScene) {
            /* copy the attributes to store */
            HA_StoreScene(pIndication->dstEndPoint, pScene);
            ZCL_SaveNvmZclData();
            #if  gZclClusterOptionals_d   
            bModifyConfiguratorAddr = TRUE;  
            #endif
          }
          else {
            sceneRsp.status = gZclInsufficientSpace_c;
          }
          break;
    
        /* recall scene */
        case gZclCmdScene_RecallScene_c:
    
          /* recall scene data */
          if(pScene) {
            HA_RecallScene(pIndication->dstEndPoint, pData, pScene);
            pSceneData->sceneValid = gZclSceneValid_c;
            pSceneData->currentScene = pScene->sceneId;
            Copy2Bytes(pSceneData->currentGroup, pScene->aGroupId);
          }
  
          fNoReply = TRUE;
          break;
    
        /* get scene membership */
        case gZclCmdScene_GetSceneMembership_c:
          status = gZclSuccess_c;
          ZCL_GetSceneMembership(pDevice, pSceneReq->getSceneMembership.aGroupId, pIndication);
          fNoReply = TRUE;
          break;
    
        /* command not supported on this cluster */
        default:
          status = gZclUnsupportedClusterCommand_c;
        }
      }
      
    #if  gZclClusterOptionals_d   
      /* If IEEE address of the source was not found set on 0xFFFFFFFFFFFF */    
      if (bModifyConfiguratorAddr) 
      {      
        if (pSrcLongAddress)
          FLib_MemCpy(&pSceneData->lastConfiguredBy, pSrcLongAddress, sizeof(zbIeeeAddr_t));
        else
          FLib_MemSet(&pSceneData->lastConfiguredBy, 0xFF, sizeof(zbIeeeAddr_t));
      }      
    #endif
  }  
  else
    /* pSceneData not available */
     status = gZclUnsupportedClusterCommand_c;  

  /* send a standard scene response */
  if(!pIndication->fWasBroadcast && (pIndication->dstAddrMode != gZbAddrModeGroup_c) && !fNoReply) {
    return ZCL_Reply(pIndication, payloadLen, &sceneRsp);
  }

  /* worked */
  return status;
}

/*!
 * @fn 		void ZCL_ResetScenes(afDeviceDef_t *pDevice, zbGroupId_t groupId)
 *
 * @brief	Processes the ResetScenes command received on the Scene Cluster Server. 
 *
 */
void ZCL_ResetScenes(afDeviceDef_t *pDevice, zbGroupId_t groupId)
{  
  zclSceneAttrs_t *pSceneData = (zclSceneAttrs_t*) pDevice->pSceneData;
  zclSceneTableEntry_t *pSceneTableEntry;
  index_t i;
   
  for(i=0; i<gHaMaxScenes_c; ++i)
  {
    pSceneTableEntry = (zclSceneTableEntry_t *) ((uint8_t*)pDevice->pSceneData + 
                                                 sizeof(zclSceneAttrs_t) + 
                                                 i*pSceneData->sceneTableEntrySize);
    
    if (IsEqual2Bytes(groupId, pSceneTableEntry->aGroupId))
    {
      BeeUtilZeroMemory(pSceneTableEntry, sizeof(zclSceneTableEntry_t));        
      --(pSceneData->sceneCount);
      pSceneData->currentScene = 0x00;
      pSceneData->sceneValid = gZclSceneInvalid_c;
    }    
  }
      
  
  ZCL_SaveNvmZclData();
}

/*!
 * @fn 		zbStatus_t ZCL_AddScene(zclSceneTableEntry_t *pScene, zclCmdScene_AddScene_t *pAddSceneReq, uint8_t indicationLen)
 *
 * @brief	Processes the AddScene command received on the Scene Cluster Server. 
 *
 */
zbStatus_t ZCL_AddScene(zclSceneTableEntry_t *pScene, zclCmdScene_AddScene_t *pAddSceneReq, uint8_t indicationLen)
{
  uint8_t NameLen;
  uint8_t *pAddSceneData;
  zclSceneOtaData_t *pClusterData;
  uint8_t len;
  uint8_t *pData;

  len = MbrOfs(zclCmdScene_AddScene_t, szSceneName[0]);

  /* copy up through the name */
  FLib_MemCpy(pScene, pAddSceneReq, len);

  /* find the start of the cluster data (after the name) */
  pAddSceneData = (uint8_t *)&pAddSceneReq->szSceneName;
  NameLen = (*pAddSceneData);
#if gZclIncludeSceneName_d
  FLib_MemCpy(pScene->szSceneName, pAddSceneReq->szSceneName, NameLen+1);
#endif

  pData = (uint8_t*)(pAddSceneData + (NameLen+1));

  len +=(NameLen+1);
  
  /* no data to copy */
  if(indicationLen < len)
    return gZclMalformedCommand_c;
  
  
  NameLen = indicationLen - len;

/* Here starts the copying process */
  while(NameLen) 
  {
    pClusterData = (zclSceneOtaData_t*)pData;
    
    len = 3;
    
    //if(NameLen < sizeof(zclSceneOtaData_t) || pClusterData->length > NameLen)
    //  return gZclMalformedCommand_c;

     if (pClusterData->length > 0)
     {       
       if (HA_AddScene(pClusterData,pScene) != gZbSuccess_c)
         return gZclMalformedCommand_c;
       else
         len += pClusterData->length;
     }     
     /* get to the next extended field */ 
     pData += len;
     NameLen -= len;      
  }
  ZCL_SaveNvmZclData();
  return gZbSuccess_c;
  
}

/*!
 * @fn 		zbStatus_t ZCL_ViewScene(zclSceneTableEntry_t *pScene, zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the AddScene command and sends over-the-air an AddSceneResponse command from the Scene Cluster Server. 
 *
 */
zbStatus_t ZCL_ViewScene
  (
  zclSceneTableEntry_t *pScene,        /* IN */
  zbApsdeDataIndication_t *pIndication, /* IN */
  afDeviceDef_t *pDevice  /* IN */
  )
{
  afToApsdeMessage_t *pMsg;
  uint8_t *pPayload;
  uint8_t payloadLen;

(void)pDevice;/*To avoid Compiler errors*/
  /* allocate space for the message */
  pMsg = AF_MsgAlloc();
  if(!pMsg)
    return gZclNoMem_c;

  /* build the reply */
  pPayload = ((uint8_t *)pMsg) + gAsduOffset_c + sizeof(zclFrame_t);

  /* copy in fixed portion of scene */
  payloadLen = MbrOfs(zclCmdScene_AddScene_t, szSceneName);
  FLib_MemCpy(pPayload + 1, pScene, payloadLen);

  /* put in status at beginning */
  pPayload[0] = gZbSuccess_c;
  ++payloadLen;

#if gZclIncludeSceneName_d
  /* copy in string */
  FLib_MemCpy(pPayload + payloadLen, pScene->szSceneName, 1 + *pScene->szSceneName);
  payloadLen += 1 + *pScene->szSceneName;
#else
  /* empty string */
  pPayload[payloadLen++] = 0;
#endif

  /* copy in cluster data */
  payloadLen += HA_ViewSceneData((void *)(pPayload + payloadLen), pScene);

  /* send payload over the air */
  return ZCL_ReplyNoCopy(pIndication, payloadLen, pMsg);
}

/*!
 * @fn 		zbStatus_t ZCL_GetSceneMembership(afDeviceDef_t *pDevice, zbGroupId_t aGroupId, zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Processes the GetSceneMembership command and sends over-the-air an GetSceneMembershipResponse command from the Scene Cluster Server. 
 *
 */
void ZCL_GetSceneMembership
  (
  afDeviceDef_t *pDevice,
  zbGroupId_t aGroupId,                 /* IN */
  zbApsdeDataIndication_t *pIndication  /* IN */
  )
{
  afToApsdeMessage_t *pMsg;
  uint8_t payloadLen;
  uint8_t i;
  uint8_t count;    /* # of scenes found that match this group */
  zclCmdScene_GetSceneMembershipRsp_t *pRsp;
  
  zclSceneAttrs_t *pSceneAttrs;
  zclSceneTableEntry_t *pSceneTableEntry;

  /* allocate space for the message */
  pMsg = AF_MsgAlloc();
  if(!pMsg)
    return;   /* can't send response, no buffers */
  
 
  pSceneAttrs = (zclSceneAttrs_t*) pDevice->pSceneData;  

  /* fill in the response */
  pRsp = (void *)(((uint8_t *)pMsg) + gAsduOffset_c + sizeof(zclFrame_t));

  /* remember how many scenes are available */
  pRsp->status =gZbSuccess_c;
  Copy2Bytes(pRsp->aGroupId, aGroupId);
  pRsp->capacity = gHaMaxScenes_c - pSceneAttrs->sceneCount;

  /* show any that match this group */
  count = 0;
  for(i=0; i<gHaMaxScenes_c; ++i)
  {
    pSceneTableEntry = (zclSceneTableEntry_t *) ((uint8_t*)pDevice->pSceneData + sizeof(zclSceneAttrs_t) + i*pSceneAttrs->sceneTableEntrySize);
    if(IsEqual2Bytes(pSceneTableEntry->aGroupId, aGroupId)) {
      pRsp->scenes[count] = pSceneTableEntry->sceneId;
      ++count;
    }
  }

  if(!count) {
    pRsp->status = gZclInvalidField_c;
    payloadLen = MbrOfs(zclCmdScene_GetSceneMembershipRsp_t,sceneCount);
  }
  else {
    payloadLen = MbrOfs(zclCmdScene_GetSceneMembershipRsp_t, scenes) + count;
    pRsp->sceneCount = count;
  }

  /* send payload over the air */
  (void)ZCL_ReplyNoCopy(pIndication, payloadLen, pMsg);
}

/*!
 * @fn 		zclSceneTableEntry_t *ZCL_AllocateScene(afDeviceDef_t *pDevice, zclCmdScene_StoreScene_t *pSceneId)
 *
 * @brief	Allocate a scene entry. Used for storing and recalling a scene.
 *			Returns NULL if not found, or pointer to entry in scene table if found.
 *
 */
zclSceneTableEntry_t *ZCL_AllocateScene(afDeviceDef_t *pDevice, zclCmdScene_StoreScene_t *pSceneId)
{
  index_t i;
  zclSceneTableEntry_t *pSceneTableEntry;
  zclSceneAttrs_t* pSceneAttrs = (zclSceneAttrs_t*) pDevice->pSceneData;


  /* invalid scene, not found */
  pSceneTableEntry = ZCL_FindScene(pDevice, (void *)pSceneId);
  if(pSceneTableEntry)
  { 
	pSceneAttrs->currentScene = pSceneId->sceneId;
    Copy2Bytes(pSceneAttrs->currentGroup, pSceneId->aGroupId);
    return pSceneTableEntry;
  }

  
  for(i=0; i<gHaMaxScenes_c; ++i)
  {
    pSceneTableEntry = (zclSceneTableEntry_t *) ((uint8_t*)pDevice->pSceneData + sizeof(zclSceneAttrs_t) + i*pSceneAttrs->sceneTableEntrySize);
    if(!pSceneTableEntry->sceneId)
    {
      
      pSceneAttrs->currentScene = pSceneId->sceneId;
      Copy2Bytes(pSceneAttrs->currentGroup, pSceneId->aGroupId);
      FLib_MemCpy(pSceneTableEntry, pSceneId, sizeof(*pSceneId));
      ++(pSceneAttrs->sceneCount);
      return pSceneTableEntry;
    }
  }

  /* not found */
  return NULL;
}

/*!
 * @fn 		zclSceneTableEntry_t *ZCL_FindScene(afDeviceDef_t *pDevice, zclCmdScene_RecallScene_t *pSceneId)
 *
 * @brief	Find a scene based on group id and scene id
 *			Returns NULL if not found, or pointer to entry in scene table if found.
 *
 */
zclSceneTableEntry_t *ZCL_FindScene(afDeviceDef_t *pDevice, zclCmdScene_RecallScene_t *pSceneId)
{
  index_t i;
  zclSceneTableEntry_t *pSceneTableEntry;
  zclSceneAttrs_t* pSceneAttrs = (zclSceneAttrs_t*) pDevice->pSceneData;

  /* invalid scene, not found */
  if(!pSceneId->sceneId)
    return NULL;

  for(i=0; i<gHaMaxScenes_c; ++i)
  {
    pSceneTableEntry = (zclSceneTableEntry_t *) ((uint8_t*)pDevice->pSceneData + sizeof(zclSceneAttrs_t) + i*pSceneAttrs->sceneTableEntrySize);

    if ( FLib_MemCmp((uint8_t*) pSceneId, (uint8_t*) pSceneTableEntry, sizeof(zclCmdScene_RecallScene_t)))          
      return pSceneTableEntry;
  }

  /* not found */
  return NULL;
}


/******************************
  On/Off Cluster
  See ZCL Specification Section 3.8
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_OnOffClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the OnOff Cluster Server. 
 *
 */
zbStatus_t ZCL_OnOffClusterServer
  (
  zbApsdeDataIndication_t *pIndication, /* IN: MUST be set to the indication */
  afDeviceDef_t *pDevice    /* IN: MUST be set to the endpoint's device data */
  )
{
  zclCmd_t command;
  zclFrame_t      *pFrame;
  uint8_t onOffAttr;
  uint8_t event;
  zbEndPoint_t ep;
  zbClusterId_t aClusterId;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  
  /* not used in this function */
  (void)pDevice;
  
  /* get the attribute */
  ep = pIndication->dstEndPoint;
  Copy2Bytes(aClusterId, pIndication->aClusterId);
  (void)ZCL_GetAttribute(ep, aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffAttr, NULL);

  /* determine what to do based on the event */
  event = gZclUI_NoEvent_c;
  command = ((zclFrame_t *)pIndication->pAsdu)->command;

  /* get ptr to ZCL frame */
  pFrame = (void *)pIndication->pAsdu;
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	   status = gZclSuccess_c;
  
  
  if(command == gZclCmdOnOff_Toggle_c) {
    command = onOffAttr ? gZclCmdOnOff_Off_c : gZclCmdOnOff_On_c;
  }
  
  switch (command) {
    case gZclCmdOnOff_Off_c:
        event = gZclUI_Off_c;
        onOffAttr = 0;  /* off */
        break;
    case gZclCmdOnOff_On_c:
        event = gZclUI_On_c;
        onOffAttr = 1;  /* on */
        break;
    default:
      status = gZclUnsupportedClusterCommand_c;      
      break;           
  }


  /* sets the attribute and will report if needed */
  (void)ZCL_SetAttribute(ep, aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffAttr);

  /* send the event to the app */
  BeeAppUpdateDevice(ep, event, 0, 0, NULL);
  /* worked */
  return status;
}


/*!
 * @fn 		zbStatus_t ZCL_GenericReqNoData(afAddrInfo_t *pAddrInfo, zclCmd_t command)
 *
 * @brief	Standard request when there is no payload.Frame control disables default response from receiver.
 *
 */
zbStatus_t ZCL_GenericReqNoData(afAddrInfo_t *pAddrInfo, zclCmd_t command)
{
  uint8_t payloadLen;
  afToApsdeMessage_t *pMsg;

  /* create the frame and copy in payload */
  payloadLen = 0;
  pMsg = ZCL_CreateFrame(pAddrInfo,command,gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp,
    NULL, &payloadLen,NULL);
  if(!pMsg)
    return gZclNoMem_c;

  /* send the frame to the destination */
  return ZCL_DataRequestNoCopy(pAddrInfo, payloadLen, pMsg);
}

/*!
 * @fn 		zbStatus_t ZCL_GenericReqNoDataServer(afAddrInfo_t *pAddrInfo, zclCmd_t command)
 *
 * @brief	Standard request from Server when there is no payload.Frame control disables default response from receiver.
 *
 */
zbStatus_t ZCL_GenericReqNoDataServer(afAddrInfo_t *pAddrInfo, zclCmd_t command)
{
  uint8_t payloadLen;
  afToApsdeMessage_t *pMsg;

  /* create the frame and copy in payload */
  payloadLen = 0;
  pMsg = ZCL_CreateFrame(pAddrInfo, command,gZclFrameControl_FrameTypeSpecific |gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp,
    NULL, &payloadLen,NULL);
  if(!pMsg)
    return gZclNoMem_c;

  /* send the frame to the destination */
  return ZCL_DataRequestNoCopy(pAddrInfo, payloadLen, pMsg);
}


/******************************
  On/Off Switch Configuration Cluster
  See ZCL Specification Section 3.9
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_OnOffSwitchClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the OnOffSwitch Cluster Server. 
 *
 */
zbStatus_t ZCL_OnOffSwitchClusterServer 
(
  zbApsdeDataIndication_t *pIndication,   /* IN: must be non-null */
  afDeviceDef_t *pDevice                  /* IN: must be non-null */
)
{
  (void) pIndication;
  (void) pDevice;
   return gZbSuccess_c;
}

/******************************
  Level Control Cluster
  See ZCL Specification Section 3.10
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_LevelOnOffClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the OnOff Cluster Server.A slight variation of the On/Off Cluster, 
 * 			includes remembering level when turning on/off. 
 *
 */
zbStatus_t ZCL_LevelOnOffClusterServer
  (
  zbApsdeDataIndication_t *pIndication, /* IN: MUST be set to the indication */
  afDeviceDef_t *pDevice    /* IN: MUST be set to the endpoint's device data */
  )
{
  zclCmd_t command;
  uint8_t onOffAttr;
  zbClusterId_t aClusterShadeId;
  zclFrame_t      *pFrame;

#if gZclClusterOptionals_d  
  uint8_t onlevel;
#endif  
  zbEndPoint_t ep;
  zbClusterId_t aClusterId;
  zbClusterId_t aLevelClusterId;
  zclCmdLevelControl_MoveToLevel_t Req;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  Set2Bytes(aClusterShadeId, gZclClusterShadeCfg_c);
  /* not used in this function */
  (void)pDevice;
   if(gShadeDevice == TRUE)
   {
     gShadeDeviceActive = TRUE; 
    (void)ZCL_GetAttribute(gZcl_ep, aClusterShadeId, gZclAttrShadeCfgInfStatus_c, gZclServerAttr_c, &statusShadeCfg, NULL);
   }
  gZcl_ep = pIndication->dstEndPoint;
  gZclLevelCmd = gZclLevel_OnOffCmd_c;

  /* level control cluster */
  Set2Bytes(aClusterId,gZclClusterOnOff_c);
  Set2Bytes(aLevelClusterId, gZclClusterLevelControl_c);
  
  /* get the attribute */
  ep = pIndication->dstEndPoint;
  (void)ZCL_GetAttribute(ep, aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffAttr, NULL);
  
  /* get ptr to ZCL frame */
  pFrame = (void *)pIndication->pAsdu;
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	   status = gZclSuccess_c;
  
  /* determine what to do based on the event */
  command = ((zclFrame_t *)pIndication->pAsdu)->command;

  if(command == gZclCmdOnOff_Toggle_c) {
    command = onOffAttr ? gZclCmdOnOff_Off_c : gZclCmdOnOff_On_c;
  }
  
  switch (command) {
  
    case gZclCmdOnOff_Off_c:
      if (onOffAttr == gZclCmdOnOff_Off_c)
      {
        uint8_t displayStatus = 1;
        BeeAppUpdateDevice(gZcl_ep, gZclUI_GoToLevel_c, 0, 0, (void*) &displayStatus);
        return status;
      }
      /* Fill the request for the off case */
      Req.level = gZclLevelMinValue; /* Set to the minimum level suported */    
      /* Keep a copy of current level*/
      #if gZclClusterOptionals_d
          (void)ZCL_GetAttribute(ep, aLevelClusterId, gZclAttrLevelControl_OnLevelId_c, gZclServerAttr_c, &onlevel, NULL);
          if (onlevel == gZclLevel_UndefinedOnLevel_c) {
            
            (void)ZCL_GetAttribute(ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &gZclLevel_LastCurrentLevel, NULL);      
          }
          else {       
            gZclLevel_LastCurrentLevel = gZclLevelMinValue;
          }
      #else      
          (void)ZCL_GetAttribute(ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &gZclLevel_LastCurrentLevel, NULL);      
      #endif      
      break;
  
    case gZclCmdOnOff_On_c:
     /* Fill the request for the on case */
#if gZclClusterOptionals_d
      (void)ZCL_GetAttribute(ep, aLevelClusterId, gZclAttrLevelControl_OnLevelId_c, gZclServerAttr_c, &onOffAttr, NULL);
      if (onOffAttr == gZclLevel_UndefinedOnLevel_c)
      {
    	if(!gZclLevel_LastCurrentLevel)
    		gZclLevel_LastCurrentLevel = gZclLevelMaxValue;
        Req.level = gZclLevel_LastCurrentLevel; /* Set to the stored value */    
      }
      else
        Req.level = onOffAttr;                  /* Set to the OnLevel value */          
#else
      Req.level = gZclLevel_LastCurrentLevel;   /* Set to the stored value */    
#endif    
      break;

    default:
      return gZclUnsupportedClusterCommand_c;      
  }
  
  Req.transitionTime = 0xFFFF;   /* Set to 0xFFFF to use the OnOffTransition time */
  
  /* sets the attribute and will report if needed */
  gZclLevel_OnOffState = command;
  (void)ZCL_LevelControlMoveToLevel(&Req, TRUE);
  /* worked */
  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_LevelControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Level Control Cluster Server. 
 *
 */
zbStatus_t ZCL_LevelControlClusterServer
  (
  zbApsdeDataIndication_t *pIndication,   /* IN: must be non-null */
  afDeviceDef_t *pDevice                  /* IN: must be non-null */
  ) 
{ 
  zclFrame_t *pFrame;
  zclLevelControlCmd_t Command; 
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  bool_t withOnOff = FALSE;
  zbClusterId_t           aClusterShadeId;
  Set2Bytes(aClusterShadeId, gZclClusterShadeCfg_c);
  (void)pDevice;

  gZclLevelCmd = gZclLevel_NotOnOffCmd_c;

  /* ZCL frame */
  pFrame = (zclFrame_t*)pIndication->pAsdu;
  gZcl_ep = pIndication->dstEndPoint;

  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	   status = gZclSuccess_c;
    
  
  /* make local copy of command (might be move to level, step, etc...) */
  FLib_MemCpy(&Command,(pFrame + 1), sizeof(Command));
  if(gShadeDevice == TRUE)
  {
     gShadeDeviceActive = TRUE; 
    (void)ZCL_GetAttribute(gZcl_ep, aClusterShadeId, gZclAttrShadeCfgInfStatus_c, gZclServerAttr_c, &statusShadeCfg, NULL);
  }
  /* handle the LevelControl commands */
  switch(pFrame->command)
  {
    /* Move to Level Commands */
    case gZclCmdLevelControl_MoveToLevelOnOff_c:
      withOnOff = TRUE;
    case gZclCmdLevelControl_MoveToLevel_c:        
      /* set on level and current level */
      (void)ZCL_LevelControlMoveToLevel(&Command.MoveToLevelCmd, withOnOff);
      break;

    /* Move commands */
    case gZclCmdLevelControl_MoveOnOff_c:    
      withOnOff = TRUE;    
    case gZclCmdLevelControl_Move_c:
      (void)ZCL_LevelControlMove(&Command.MoveCmd, withOnOff);
      break;

    /* Step Commands */
    case gZclCmdLevelControl_StepOnOff_c:
      withOnOff = TRUE;    
    case gZclCmdLevelControl_Step_c:
      (void)ZCL_LevelControlStep(&Command.StepCmd, withOnOff);
      break;

    /* Stop Commands */
    case gZclCmdLevelControl_StopOnOff_c:
    case gZclCmdLevelControl_Stop_c:
      (void)ZCL_LevelControlStop();
      break;      

    /* command not supported on this cluster */
    default:
      return gZclUnsupportedClusterCommand_c;
  }
  return status;
}

/*!
 * @fn 		void ZCL_LevelControlTimer( uint8_t timerId )
 *
 * @brief	Callback used to update the current level. Sends events to application.
 *
 */
void ZCL_LevelControlTimer( uint8_t timerId )
{
  zbClusterId_t           aLevelClusterId;
  zbClusterId_t           aClusterId;
  zbClusterId_t           aClusterShadeId;
  uint8_t                 OnOffStatus; 
  bool_t                  updateOnOffStatus = FALSE;
  (void) timerId;
  
  Set2Bytes(aLevelClusterId, gZclClusterLevelControl_c);
  Set2Bytes(aClusterId, gZclClusterOnOff_c);
  Set2Bytes(aClusterShadeId, gZclClusterShadeCfg_c);
  
  (void)ZCL_GetAttribute(gZcl_ep, aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &OnOffStatus,NULL);
  (void)ZCL_GetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &gZclCurrentLevelTmp, NULL);
  gZclLevel_DimmingStopReport = FALSE;
 
  /* Update the CurrentLevel Attribute */
  if (gZclLevel_diference >= gZclLevel_Step)      
  {
    gZclLevel_diference -= gZclLevel_Step;
    if (zclMoveMode_Down == gZclLevel_MoveMode)
    {
      if(gShadeDevice == TRUE){
          statusShadeCfg.ShadeDirection = 0;
          statusShadeCfg.ShadeForwardDir = 0;
      }
      gZclCurrentLevelTmp -=gZclLevel_Step;
    }
    else
    {
      if(gShadeDevice == TRUE){
          statusShadeCfg.ShadeDirection = 1;
          statusShadeCfg.ShadeForwardDir = 1;
      }
      gZclCurrentLevelTmp +=gZclLevel_Step;
    }
  }
  else
  {
      gZclLevel_diference = 0;
      gZclCurrentLevelTmp = gZclNewCurrentLevel;
  }
  (void)ZCL_SetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c,&gZclCurrentLevelTmp);
  if (0 == gZclLevel_diference)
    gZclLevel_DimmingStopReport = TRUE;

  
  /*Recalculate the remaining time, substracting the time Between Changes*/
  if (gZclLevel_RemainingTimeInMilliseconds)
  {
    if ((gZclLevel_diference == 0) &&(gZclLevel_RemainingTimeInMilliseconds > gZclLevel_TimeBetweenChanges))
      gZclLevel_TimeBetweenChanges = (uint16_t)gZclLevel_RemainingTimeInMilliseconds;
    else
      gZclLevel_RemainingTimeInMilliseconds -= gZclLevel_TimeBetweenChanges;               
  }
   
    #if gZclClusterOptionals_d 
    {
      uint16_t remainingTime =  Native2OTA16((uint16_t) (gZclLevel_RemainingTimeInMilliseconds / 100));    
      (void)ZCL_SetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_RemainingTimeId_c, gZclServerAttr_c,&remainingTime);
    }
    #endif

  
  if(gZclLevel_diference) 
  {
    /* We didn't get to the specified level, so restart the timer */
    TMR_StartSingleShotTimer(gZclLevel_TransitionTimerID,(tmrTimeInMilliseconds_t)gZclLevel_TimeBetweenChanges, ZCL_LevelControlTimer);
  }
  else 
  {
    updateOnOffStatus = TRUE;
    
    #if gZclClusterOptionals_d 
    {
    uint16_t remainingTime = 0;
   (void)ZCL_SetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_RemainingTimeId_c, gZclServerAttr_c,&remainingTime);
    }
    #endif
    
    
    if (gZclLevelCmd == gZclLevel_OnOffCmd_c) 
    {
      uint8_t currentLevelValue = 0; 	
      /* There was an OnOff command, update the OnOff attribute */
      (void)ZCL_SetAttribute(gZcl_ep, aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &gZclLevel_OnOffState);
      if(gZclLevel_OnOffState == 0x00)
      {
    	  currentLevelValue  = 0x00;
      }
      else
      {
#if gZclClusterOptionals_d				        
    	  (void)ZCL_GetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_OnLevelId_c, gZclServerAttr_c, &OnOffStatus, NULL);        
#else
    	  OnOffStatus = gZclLevel_UndefinedOnLevel_c;
#endif
    	if (OnOffStatus == gZclLevel_UndefinedOnLevel_c)  
    		currentLevelValue = gZclLevel_LastCurrentLevel;
    	else
    		currentLevelValue = OnOffStatus;   
      }
      (void)ZCL_SetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &currentLevelValue);

    }
    else if (gZclLevel_OnOff)
    {         
      /* There was an Move or MoveToLevel with OnOff option, so update the OnOff attribute, based on the current level */   
      OnOffStatus = gZclCmdOnOff_Off_c;
      if (gZclCurrentLevelTmp > gZclLevelMinValue)
        OnOffStatus = gZclCmdOnOff_On_c;
        
      (void)ZCL_SetAttribute(gZcl_ep, aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &OnOffStatus);
      if(gShadeDevice == TRUE)
         gShadeDeviceActive = FALSE;           
    }
  }
  BeeAppUpdateDevice(gZcl_ep, gZclUI_GoToLevel_c, 0, 0, (void*) &updateOnOffStatus);
}

/*!
 * @fn 		zbStatus_t ZCL_LevelControlMoveToLevel(zclCmdLevelControl_MoveToLevel_t *pReq, bool_t withOnOff) 
 *
 * @brief	On receipt of this command, a device shall move from its current level to the value given in the Level
 * 			field. The meaning of ‘level’ is device dependent – e.g. for a light it may mean brightness level.
 * 			The movement shall be continuous, i.e. not a step function, and the time taken to move to the new level
 * 			shall be equal to the Transition time field, in seconds.
 *
 */
zbStatus_t ZCL_LevelControlMoveToLevel
(
  zclCmdLevelControl_MoveToLevel_t * pReq,
  bool_t withOnOff
)
{
	zbClusterId_t           aLevelClusterId;
	uint16_t                TransitionTime = 0;
	uint16_t                OnOffTransitionTime = 0;
	uint16_t 				OnTransitionTime = 0;
	uint16_t 				OffTransitionTime = 0;
	Set2Bytes(aLevelClusterId, gZclClusterLevelControl_c);

	(void)ZCL_GetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &gZclCurrentLevelTmp, NULL);
	TransitionTime = OTA2Native16(pReq->transitionTime);
	if (TransitionTime == 0xFFFF){
        #if gZclClusterOptionals_d
          (void) ZCL_GetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_OnOffTransitionTimeId_c, gZclServerAttr_c, &OnOffTransitionTime, NULL);
          (void) ZCL_GetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_OnTransitionTimeId_c, gZclServerAttr_c, &OnTransitionTime, NULL);
          (void) ZCL_GetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_OffTransitionTimeId_c, gZclServerAttr_c, &OffTransitionTime, NULL);
          OnOffTransitionTime = OTA2Native16(OnOffTransitionTime);
          OnTransitionTime = OTA2Native16(OnTransitionTime);
          OffTransitionTime = OTA2Native16(OffTransitionTime);
        #endif     
          if (gZclCurrentLevelTmp <= pReq->level)
        	  TransitionTime = (OnTransitionTime==0xFFFF)?OnOffTransitionTime:OnTransitionTime; 
          else	  
        	  TransitionTime = (OffTransitionTime==0xFFFF)?OnOffTransitionTime:OffTransitionTime; 
        }
        gZclLevel_MoveMode = zclMoveMode_Down;
        gZclLevel_OnOff = withOnOff;
        gZclLevel_diference = gZclCurrentLevelTmp - pReq->level;	
        if (gZclCurrentLevelTmp <= pReq->level) {
          gZclLevel_MoveMode = zclMoveMode_Up;
          gZclLevel_diference = pReq->level - gZclCurrentLevelTmp;
        }
	gZclLevel_Step = 1;
	gZclNewCurrentLevel = pReq->level;
	
	gZclLevel_RemainingTimeInMilliseconds = (tmrTimeInMilliseconds_t)(TransitionTime *100);	
  #if gZclClusterOptionals_d 
  {
  uint16_t remainingTime =  Native2OTA16((uint16_t) (gZclLevel_RemainingTimeInMilliseconds / 100));
    
	(void)ZCL_SetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_RemainingTimeId_c, gZclServerAttr_c,&remainingTime);
  }
  #endif		
	
	
	gZclLevel_TimeBetweenChanges = (uint16_t)(gZclLevel_RemainingTimeInMilliseconds/gZclLevel_diference);
	if(gZclLevel_TimeBetweenChanges != 0x00)
		TMR_StartSingleShotTimer(gZclLevel_TransitionTimerID,gZclLevel_TimeBetweenChanges, ZCL_LevelControlTimer);
	else
		ZCL_LevelControlTimer(gZclLevel_TransitionTimerID);
	return gZbSuccess_c;
}

/*!
 * @fn 		zbStatus_t ZCL_LevelControlMove(zclCmdLevelControl_Move_t *pReq, bool_t withOnOff) 
 *
 * @brief	On receipt of this command, a device shall move from its current level in an up 
 * 			or down direction in a continuous fashion, as detailed by the mode:
 *
 * 			Up:      Increase the device’s level at the rate given in the Rate field. If the level
 *					 reaches the maximum allowed for the device, stop. If the device is currently
 *					 powered off, do not power it on.
 *			Up with OnOff: If the device requires powering on, do so, then proceed as for the Up mode.
 *			Down:    Decrease the device’s level at the rate given in the Rate field. If the level
 *					 reaches the minimum allowed for the device, stop. If the device is currently
 *					 powered off, do not power it on.
 *			Down with OnOff: Decrease the device’s level at the rate given in the Rate field. If the level
 *					 reaches the minimum allowed for the device, stop, then, if the device can be
 *					 powered off, do so.
 *
 * 			The Rate field specifies the rate of movement in steps per second. A step is a change 
 * 			in the device’s level of one unit.
 *
 */
zbStatus_t ZCL_LevelControlMove 
(
  zclCmdLevelControl_Move_t * pReq,
  bool_t withOnOff
)
{
  zbClusterId_t           aLevelClusterId;
  zbClusterId_t           aOnOffClusterId;
  zclLevelValue_t         CurrentLevel;
  uint8_t                 OnOffStatus;  
  uint8_t                 defaultRate = gZclLevelControl_DefaultMoveRate_d;

  
  Set2Bytes(aLevelClusterId, gZclClusterLevelControl_c);
  Set2Bytes(aOnOffClusterId, gZclClusterOnOff_c);
  
  (void)ZCL_GetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &CurrentLevel, NULL);
  (void)ZCL_GetAttribute(gZcl_ep, aOnOffClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c,&OnOffStatus, NULL);
  
  gZclLevel_MoveMode =pReq->moveMode;
  gZclLevel_OnOff = withOnOff;
  gZclLevel_Step = 1;
  
  if ((gZclLevel_MoveMode == zclMoveMode_Up) && withOnOff && (!OnOffStatus)){
    OnOffStatus = 1;
    (void)ZCL_SetAttribute(gZcl_ep, aOnOffClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c,&OnOffStatus);
  }
  
  switch (gZclLevel_MoveMode){
  case zclMoveMode_Up:
          gZclLevel_diference = gZclLevel_high- CurrentLevel;
          gZclNewCurrentLevel = gZclLevel_high;
          break;
  case zclMoveMode_Down:
          gZclLevel_diference = CurrentLevel - gZclLevel_off;
          gZclNewCurrentLevel = gZclLevel_off;
          break;
  }
  

  if(pReq->rate == 0xFF)
  {
    /* use default rate */
 #if gZclClusterOptionals_d     
    (void)ZCL_GetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_DefaultMoveRateId_c, gZclServerAttr_c,&defaultRate, NULL);
#endif
    pReq->rate = (defaultRate == 0xFF)?250:defaultRate; 
  }
  
  gZclLevel_TimeBetweenChanges = (uint16_t)(1000/pReq->rate);
  gZclLevel_RemainingTimeInMilliseconds = (tmrTimeInMilliseconds_t)(gZclLevel_TimeBetweenChanges*gZclLevel_diference);
  
  #if gZclClusterOptionals_d 
  {
  uint16_t remainingTime =  Native2OTA16((uint16_t) (gZclLevel_RemainingTimeInMilliseconds / 100));
    
	(void)ZCL_SetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_RemainingTimeId_c, gZclServerAttr_c,&remainingTime);
  }
  #endif	
  
  TMR_StartSingleShotTimer(gZclLevel_TransitionTimerID,gZclLevel_TimeBetweenChanges, ZCL_LevelControlTimer);
  
  return gZbSuccess_c;
}

/*!
 * @fn 		zbStatus_t ZCL_LevelControlStep(zclCmdLevelControl_Step_t *pReq, bool_t withOnOff) 
 *
 * @brief	On receipt of this command, a device shall move from its current level in an up 
 * 			or down direction in a continuous fashion, as detailed by the mode:
 *
 * 			Up:      Increase the device’s level by the number of units indicated in the Amount
 *    				 field. If the level is already at the maximum allowed for the device, then do
 *    				 nothing. If the value specified in the Amount field would cause the
 *    				 maximum value to be exceeded, then move to the maximum value using the
 *    				 full transition time. If the device is currently powered off, do not power it on.
 *			Up with OnOff: If the device requires powering on, do so, then proceed as for the Up mode.
 *			Down:    Decrease the device’s level by the number of units indicated in the Amount
 *     				 field. If the level is already at the minimum allowed for the device, then do
 *     				 nothing. If the value specified in the Amount field would cause the
 *     				 minimum value to be exceeded, then move to the minimum value using the
 *     				 full transition time. If the device is currently powered off, do not power it on.
 *			Down with OnOff: Carry out the Down action. If the new level is at or below the minimum
 *                 	 allowed for the device, and the device can be powered off, then do so.
 *
 * 			The Transition time field specifies the time, in 1/10ths of a second, the time that shall be taken to
 * 			perform the step. A step is a change in the device’s level by the number of units specified in the
 * 			Amount field.
 *
 */
zclStatus_t ZCL_LevelControlStep
(
  zclCmdLevelControl_Step_t *pReq,
  bool_t withOnOff
)
{
	zbClusterId_t           aLevelClusterId;
	zbClusterId_t           aOnOffClusterId;
	uint16_t                TransitionTime = 0;
	zclLevelValue_t         CurrentLevel;
	uint8_t                 OnOffStatus;

	Set2Bytes(aLevelClusterId, gZclClusterLevelControl_c);
	Set2Bytes(aOnOffClusterId, gZclClusterOnOff_c);

	(void)ZCL_GetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &CurrentLevel, NULL);
	(void)ZCL_GetAttribute(gZcl_ep, aOnOffClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &OnOffStatus, NULL);

	TransitionTime = OTA2Native16(pReq->transitionTime);
	gZclLevel_diference = pReq->stepSize;
	gZclLevel_Step = 1;
	gZclLevel_MoveMode = pReq->stepMode;
    gZclLevel_OnOff = withOnOff;	
	if ((gZclLevel_MoveMode == zclMoveMode_Up) && withOnOff && (!OnOffStatus)){
		OnOffStatus = 1;
		(void)ZCL_SetAttribute(gZcl_ep, aOnOffClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c,&OnOffStatus);
	}
	if (OnOffStatus){
		switch (gZclLevel_MoveMode){
			case zclMoveMode_Up:
				gZclNewCurrentLevel = CurrentLevel + pReq->stepSize;
				if ((gZclLevel_high - CurrentLevel) < pReq->stepSize){
					gZclNewCurrentLevel = gZclLevel_high;
					gZclLevel_diference = gZclLevel_high - CurrentLevel;
				}
				break;
			case zclMoveMode_Down:
				gZclNewCurrentLevel = CurrentLevel - pReq->stepSize;
				if ((CurrentLevel - gZclLevel_off) < pReq->stepSize){
					gZclNewCurrentLevel = gZclLevel_off;
					gZclLevel_diference = CurrentLevel - gZclLevel_off;
				}
				break;
			}
		gZclLevel_Step = 1;
		gZclLevel_RemainingTimeInMilliseconds = (tmrTimeInMilliseconds_t)(TransitionTime*100);
		#if gZclClusterOptionals_d 
    {
      uint16_t remainingTime =  Native2OTA16((uint16_t) (gZclLevel_RemainingTimeInMilliseconds / 100));          
  	  (void)ZCL_SetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_RemainingTimeId_c, gZclServerAttr_c,&remainingTime);
    }
    #endif		
		gZclLevel_TimeBetweenChanges = (uint16_t)(gZclLevel_RemainingTimeInMilliseconds/gZclLevel_diference);
		TMR_StartSingleShotTimer(gZclLevel_TransitionTimerID,gZclLevel_TimeBetweenChanges, ZCL_LevelControlTimer);
		}
	return gZbSuccess_c;
}

/*!
 * @fn 		zclStatus_t ZCL_LevelControlStop(void) 
 *
 * @brief	On receipt of this command, a device shall stop any level control actions
 * 		    (move, move to level, step) that are in progress 
 */
zclStatus_t ZCL_LevelControlStop(void) 
{
  TMR_StopTimer(gZclLevel_TransitionTimerID);
  return gZbSuccess_c;  
}

/******************************
  Alarms Cluster
  See ZCL Specification Section 3.11
*******************************/
extern gZclAlarmTable_t gAlarmsTable[MaxAlarmsPermitted];

/*!
 * @fn 		zbStatus_t ZCL_AlarmsClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alarm Cluster Client. 
 *
 */
zbStatus_t ZCL_AlarmsClusterClient
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
        case gAlarmClusterTxCommandID_Alarm_c:
        case gAlarmClusterTxCommandID_GetAlarmResponse_c:

          /* These should be passed up to a host*/
          return status;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
} 

/*!
 * @fn 		zbStatus_t ZCL_AlarmsClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alarm Cluster Server. 
 *
 */
zbStatus_t ZCL_AlarmsClusterServer
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command = 0;
    zclCmdAlarmInformation_ResetAlarm_t CmdResetAlarm;
    zclAlarms_GetAlarmResponse_t *pGetAlarmResponse;
    zbClusterId_t aClusterId;
    uint8_t  i=0, min=0;
    uint16_t count = 0;
    uint32_t minTimeStamp = 0;
    zbStatus_t status = gZclSuccessDefaultRsp_c;
    
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;
    
    Copy2Bytes(aClusterId, pIndication->aClusterId);
    
    /* handle the command */
    (void)ZCL_GetAttribute( pIndication->dstEndPoint, aClusterId, gZclAttrAlarms_AlarmCount_c, gZclServerAttr_c, &count, NULL);
    count = OTA2Native16(count);
    command = pFrame->command;  
    switch(command)
    {
        case gAlarmClusterRxCommandID_ResetAlarm_c:
          FLib_MemCpy(&CmdResetAlarm ,(pFrame + 1), sizeof(zclCmdAlarmInformation_ResetAlarm_t));  
          ResetAlarm(CmdResetAlarm, pIndication->dstEndPoint);
          return status;
        case gAlarmClusterRxCommandID_ResetAllAlarms_c:
          ResetAllAlarm(pIndication->dstEndPoint);
          return status;
        case gAlarmClusterRxCommandID_GetAlarm_c:
          pGetAlarmResponse = MSG_Alloc(sizeof(zclCmdAlarmInformation_ResetAlarm_t)); 
          pGetAlarmResponse->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
          Copy2Bytes(pGetAlarmResponse->addrInfo.dstAddr.aNwkAddr, pIndication->aSrcAddr); 
          pGetAlarmResponse->addrInfo.dstEndPoint = pIndication->dstEndPoint;
          pGetAlarmResponse->addrInfo.srcEndPoint = pIndication->srcEndPoint;
          pGetAlarmResponse->addrInfo.txOptions = 0;
          pGetAlarmResponse->addrInfo.radiusCounter = afDefaultRadius_c;
          if(count > 1)
          {
            //minTimeStamp = gAlarmsTable[0].TimeStamp;
            FLib_MemCpy(&minTimeStamp, &gAlarmsTable[0].TimeStamp, 4);  
            for(i=0; i<(count-1); i++)
              if(minTimeStamp > gAlarmsTable[i+1].TimeStamp)
              {
            	min = i+1;  
                //minTimeStamp = gAlarmsTable[min].TimeStamp;
            	FLib_MemCpy(&minTimeStamp, &gAlarmsTable[i+1].TimeStamp, 4);       
              }
          }
          else
            min = 0;        
          pGetAlarmResponse->cmdFrame.AlarmCode = gAlarmsTable[min].AlarmCode;
          Copy2Bytes(&pGetAlarmResponse->cmdFrame.ClusterID, &gAlarmsTable[min].ClusterID);
          FLib_MemCpy(&pGetAlarmResponse->cmdFrame.TimeStamp, &gAlarmsTable[min].TimeStamp, 4);   
          pGetAlarmResponse->cmdFrame.Status = (count == 0x00)?STATUS_NOT_FOUND_c:STATUS_SUCCESS_c;
		  /* update alarm table */
          if(count > 0)
            for(i=min;i<count-1;i++)
        	//gAlarmsTable[i] = gAlarmsTable[i+1];
        	FLib_MemCpy(&gAlarmsTable[i], &gAlarmsTable[i+1], sizeof(gZclAlarmTable_t)); 
          /*update alarm count attribut*/
          count--;
          count = Native2OTA16(count);
          (void)ZCL_SetAttribute( pIndication->dstEndPoint, aClusterId, gZclAttrAlarms_AlarmCount_c, gZclServerAttr_c, &count);			
          return Alarms_GetAlarmResponse(pGetAlarmResponse);   
        case gAlarmClusterRxCommandID_ResetAlarmLog_c:
          count = 0;
          (void)ZCL_SetAttribute( pIndication->dstEndPoint, aClusterId, gZclAttrAlarms_AlarmCount_c, gZclServerAttr_c, &count);
          return status;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t Alarms_ResetAlarm(zclAlarmInformation_ResetAlarm_t *pReq) 
 *
 * @brief	Sends over-the-air a ResetAlarm command from the Alarm Cluster Client. 
 *
 */
zbStatus_t Alarms_ResetAlarm
(
zclAlarmInformation_ResetAlarm_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendClientReqSeqPassed(gAlarmClusterRxCommandID_ResetAlarm_c, sizeof(zclCmdAlarmInformation_ResetAlarm_t), (zclGenericReq_t *)pReq);

}

/*!
 * @fn 		void ResetAlarm(zclCmdAlarmInformation_ResetAlarm_t CmdResetAlarm, uint8_t endpoint)
 *
 * @brief	Process reset alarm command received on the Alarm Cluster Server. 
 *
 */
void ResetAlarm(zclCmdAlarmInformation_ResetAlarm_t CmdResetAlarm, uint8_t endpoint)
{
#if (gASL_ZclMet_Optionals_d && gZclEnableMeteringServer_d)  ||  gZclEnableThermostat_c 
#if gASL_ZclMet_Optionals_d && gZclEnableMeteringServer_d
   zbClusterId_t aClusterIdZone = {gaZclClusterSmplMet_c};
   uint16_t attrId = {gZclAttrMetASGenericAlarmMask_c};
   uint16_t alarmMask;
#else   
  zbClusterId_t aClusterIdZone = {gaZclClusterThermostat_c};
  uint16_t attrId = {gZclAttrThermostat_AlarmMaskId_c};
  uint8_t alarmMask;
#endif  
  if(FLib_Cmp2Bytes(&CmdResetAlarm.ClusterID, &aClusterIdZone[0]) == TRUE){
    
    (void)ZCL_GetAttribute(endpoint, aClusterIdZone, attrId, gZclServerAttr_c, &alarmMask, NULL);
     alarmMask = (~(1<<CmdResetAlarm.AlarmCode)) & alarmMask;
    (void)ZCL_SetAttribute(endpoint, aClusterIdZone, attrId, gZclServerAttr_c, &alarmMask);      
  }
#endif  
}

/*!
 * @fn 		zbStatus_t Alarms_ResetAllAlarms(zclAlarmInformation_NoPayload_t *pReq) 
 *
 * @brief	Sends over-the-air a ResetAllAlarm command from the Alarm Cluster Client. 
 *
 */
zbStatus_t Alarms_ResetAllAlarms
(
zclAlarmInformation_NoPayload_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendClientReqSeqPassed(gAlarmClusterRxCommandID_ResetAllAlarms_c, 0, (zclGenericReq_t *)pReq);

}

/*!
 * @fn 		zbStatus_t Alarms_ResetAlarmLog(zclAlarmInformation_NoPayload_t *pReq) 
 *
 * @brief	Sends over-the-air a ResetAlarmLog command from the Alarm Cluster Client. 
 *
 */
zbStatus_t Alarms_ResetAlarmLog
(
zclAlarmInformation_NoPayload_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendClientReqSeqPassed(gAlarmClusterRxCommandID_ResetAlarmLog_c, 0, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t Alarms_GetAlarm(zclAlarmInformation_NoPayload_t *pReq) 
 *
 * @brief	Sends over-the-air a GetAlarm command from the Alarm Cluster Client. 
 *
 */
zbStatus_t Alarms_GetAlarm
(
zclAlarmInformation_NoPayload_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendClientReqSeqPassed(gAlarmClusterRxCommandID_GetAlarm_c, 0, (zclGenericReq_t *)pReq);

}
/*!
 * @fn 		void ResetAllAlarm(uint8_t endpoint)
 *
 * @brief	Process reset all alarm command received on the Alarm Cluster Server. 
 *
 */
void ResetAllAlarm(uint8_t endpoint)
{
#if (gASL_ZclMet_Optionals_d && gZclEnableMeteringServer_d)  ||  gZclEnableThermostat_c 
#if gASL_ZclMet_Optionals_d  && gZclEnableMeteringServer_d
   zbClusterId_t aClusterIdZone[2] = {gaZclClusterALarms_c, gaZclClusterSmplMet_c};
   uint16_t attrId = {gZclAttrMetASGenericAlarmMask_c};
   uint16_t valueAttr = 0;
#else     
   zbClusterId_t aClusterIdZone[2] = {gaZclClusterALarms_c, gaZclClusterThermostat_c};
   uint16_t attrId = {gZclAttrThermostat_AlarmMaskId_c};
   uint8_t  valueAttr = 0;
#endif    
   uint16_t count = 0, i=0;
   bool_t testedCluster = FALSE;

   (void)ZCL_GetAttribute(endpoint, aClusterIdZone[0], gZclAttrAlarms_AlarmCount_c, gZclServerAttr_c, &count, NULL);
   count = Native2OTA16(count);
   for(i=0; i< count; i++)
   {
    if(FLib_Cmp2Bytes(&gAlarmsTable[i].ClusterID,&aClusterIdZone[1]) == TRUE)
      if(testedCluster == FALSE)
      {
         valueAttr = 0;
        (void)ZCL_SetAttribute(endpoint, aClusterIdZone[1], attrId, gZclServerAttr_c, &valueAttr);  
         testedCluster = TRUE;
      }
   }  
#endif  
}

/*!
 * @fn 		zbStatus_t Alarms_Alarm(zclAlarmInformation_Alarm_t *pReq) 
 *
 * @brief	Sends over-the-air a Alarm command from the Alarm Cluster Server. 
 *
 */
zbStatus_t Alarms_Alarm
(
zclAlarmInformation_Alarm_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendServerReqSeqPassed(gAlarmClusterTxCommandID_Alarm_c, sizeof(zclCmdAlarmInformation_Alarm_t), (zclGenericReq_t *)pReq);

}

/*!
 * @fn 		zbStatus_t Alarms_GetAlarmResponse(zclAlarms_GetAlarmResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a GetAlarmResponse command from the Alarm Cluster Server. 
 *
 */
zbStatus_t Alarms_GetAlarmResponse
(
  zclAlarms_GetAlarmResponse_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendServerRspSeqPassed(gAlarmClusterTxCommandID_GetAlarmResponse_c,sizeof(zclCmdAlarmInformation_GetAlarmResponse_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		uint8_t TestAlarmStatus(uint8_t index, uint8_t endPoint)
 *
 * @brief	Check the Alarm Mask bit.
 *
 */
uint8_t TestAlarmStatus(uint8_t index, uint8_t endPoint)
{
#if gASL_ZclMet_Optionals_d && gZclEnableMeteringServer_d 
   zbClusterId_t aClusterIdZone = {gaZclClusterSmplMet_c};
   uint16_t alarmMask = 0x00;
   uint16_t attrId = {gZclAttrMetASGenericAlarmMask_c};
#else   
   zbClusterId_t aClusterIdZone = {gaZclClusterThermostat_c};
   uint8_t alarmMask = 0x00;
   uint16_t attrId = {gZclAttrThermostat_AlarmMaskId_c};
#endif
   if(IsEqual2BytesInt(aClusterIdZone, gAlarmsTable[index].ClusterID) == TRUE)
   {    
      (void)ZCL_GetAttribute(endPoint, aClusterIdZone, attrId, gZclServerAttr_c, &alarmMask, NULL);        
      if((alarmMask & (1<<gAlarmsTable[index].AlarmCode)) >> gAlarmsTable[index].AlarmCode == 1)
       return STATUS_SUCCESS_c;
   }
   return STATUS_NOT_FOUND_c;
}

/******************************
  Time Cluster
  See ZCL Specification (075123r02) Section 3.12
*******************************/

ZCLTimeServerAttrsRAM_t gZclTimeServerAttrs = {
  0x00000000, /* gZclAttrTime_c */
  (zclTimeStatusMaster | zclTimeStatusMasterZoneDst), /* gZclAttrTimeStatus_c */
  0x00000000, /* gZclAttrTimeZone_c */
  0x00000000, /* gZclAttrDstStart_c */
  0x00000000, /* gZclAttrDstEnd_c */
  0x00000000, /* gZclAttrDstShift_c */
  0x00000000, /* gZclAttrStandardTime_c */
  0x00000000, /* gZclAttrLocalTime_c */
  0xFFFFFFFF, /* gZclAttrLastSetTime_c */
  0xFFFFFFFF, /* gZclAttrValidUntilTime_c */
};

/* note 
- by setting zclTimeStatusMaster and zclTimeStatusMasterZoneDst all Time attributes becomes read only
ajustmets to the time and time zones must be done from the backend.
*/

const zclAttrDef_t gaZclTimeServerAttrDef[] = {
  { gZclAttrIdTime_c,           gZclDataTypeUTCTime_c,  gZclAttrFlagsNoFlags_c, sizeof(uint32_t),  (void *)&gZclTimeServerAttrs.Time },
  { gZclAttrIdTimeStatus_c,     gZclDataTypeBitmap8_c,  gZclAttrFlagsNoFlags_c, sizeof(uint8_t),   (void *)&gZclTimeServerAttrs.TimeStatus },
  { gZclAttrIdTimeZone_c,       gZclDataTypeInt32_c,    gZclAttrFlagsNoFlags_c, sizeof(int32_t),   (void *)&gZclTimeServerAttrs.TimeZone },
  { gZclAttrIdDstStart_c,       gZclDataTypeUint32_c,   gZclAttrFlagsNoFlags_c, sizeof(uint32_t),  (void *)&gZclTimeServerAttrs.DstStart },
  { gZclAttrIdDstEnd_c,         gZclDataTypeUint32_c,   gZclAttrFlagsNoFlags_c, sizeof(uint32_t),  (void *)&gZclTimeServerAttrs.DstEnd },
  { gZclAttrIdDstShift_c,       gZclDataTypeInt32_c,    gZclAttrFlagsNoFlags_c, sizeof(int32_t),   (void *)&gZclTimeServerAttrs.DstShift },
  { gZclAttrIdStandardTime_c,   gZclDataTypeUint32_c,   gZclAttrFlagsRdOnly_c, sizeof(uint32_t),  (void *)&gZclTimeServerAttrs.StandardTime },
  { gZclAttrIdLocalTime_c,      gZclDataTypeUint32_c,   gZclAttrFlagsRdOnly_c, sizeof(uint32_t),  (void *)&gZclTimeServerAttrs.LocalTime },
  { gZclAttrIdLastSetTime_c,    gZclDataTypeUTCTime_c,  gZclAttrFlagsRdOnly_c, sizeof(uint32_t),  (void *)&gZclTimeServerAttrs.LastSetTime},
  { gZclAttrIdValidUntilTime_c, gZclDataTypeUTCTime_c,  gZclAttrFlagsNoFlags_c, sizeof(uint32_t),  (void *)&gZclTimeServerAttrs.ValidUntilTime}
};

const zclAttrSet_t gaZclTimeServerClusterAttrSet[] = {
  {gZclAttrSetTime_c, (void *)&gaZclTimeServerAttrDef, NumberOfElements(gaZclTimeServerAttrDef)}
};

const zclAttrSetList_t gZclTimeServerClusterAttrSetList = {
  NumberOfElements(gaZclTimeServerClusterAttrSet),
  gaZclTimeServerClusterAttrSet
};

const uint8_t mYearTable[2][12] = {
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

tmrTimerID_t zclTimeSecondTimer = gTmrInvalidTimerID_c;

/*!
 * @fn 		void App_InitTimeCluster(void)
 *
 * @brief	Init Time Cluster using default application values.
 *
 */
void App_InitTimeCluster(void)
{
  ZCLTimeConf_t defaultTimeConf;

  defaultTimeConf.Time = mDefaultValueOfTimeClusterAppTime_c;
  defaultTimeConf.Time = Native2OTA32( defaultTimeConf.Time);
  defaultTimeConf.TimeStatus = zclTimeStatusMaster;
  defaultTimeConf.TimeZone = mDefaultValueOfTimeClusterAppTimeZone_c;
  defaultTimeConf.TimeZone = Native2OTA32(defaultTimeConf.TimeZone);
  defaultTimeConf.DstStart = mDefaultValueOfTimeClusterAppDstStart_c;
  defaultTimeConf.DstStart = Native2OTA32(defaultTimeConf.DstStart);
  defaultTimeConf.DstEnd = mDefaultValueOfTimeClusterAppDstEnd_c;
  defaultTimeConf.DstEnd = Native2OTA32(defaultTimeConf.DstEnd);
  defaultTimeConf.DstShift = mDefaultValueOfClusterAppDstShift_c;
  defaultTimeConf.DstShift = Native2OTA32(defaultTimeConf.DstShift);
  defaultTimeConf.ValidUntilTime = mDefaultValueOfTimeClusterAppValidUntilTime_c;
  defaultTimeConf.ValidUntilTime = Native2OTA32(defaultTimeConf.ValidUntilTime);
  ZCL_TimeInit(&defaultTimeConf);
}

/*!
 * @fn 		zclTime_GetTimeInf_t App_GetTimeInf(ZCLTime_t timestamp)
 *
 * @brief	Get time information(day, month, hour, minute, year) based on timestamp value
 *
 */
zclTime_GetTimeInf_t App_GetTimeInf(ZCLTime_t timestamp)
{
  zclTime_GetTimeInf_t timeInf;
  uint32_t dayclock, dayno;
  uint16_t year = gTime_StartZigbeeYearTime_c;
 
  
  timestamp = OTA2Native32(timestamp);
 
  dayclock =  timestamp % gTime_SecondsDay_c;
  dayno =  timestamp / gTime_SecondsDay_c;
  
  timeInf.timeMinute  = (uint8_t)((dayclock % gTime_SecondsHour_c) / gTime_SecondsMinute_c);
  timeInf.timeHour	= (uint8_t)(dayclock / gTime_SecondsHour_c);
  timeInf.timeWeekDay = (dayno + gTime_StartZigbeeWeekDay_c) % gTime_NoOfDays_c; 
  
  while (dayno >= Time_YearSize(year)) 
  {
	dayno -= Time_YearSize(year);
	year++;
  }
	  
  timeInf.timeYear = year;  

  timeInf.timeMonth = 1;	/* start from January */
  while (dayno >= mYearTable[Time_LeapYear(year)][timeInf.timeMonth]) 
  {
	dayno -= mYearTable[Time_LeapYear(year)][timeInf.timeMonth];
	timeInf.timeMonth++;
  }
  
  timeInf.timeday = (uint8_t)(dayno); 
  
  return timeInf;	
}

/*!
 * @fn 		void ZCL_TimeUpdate(void)
 *
 * @brief	Update the zcl Time attributes. Shall be called every second
 *
 */
void ZCL_TimeUpdate(void)
{
  /* swap endianess OTA 2 native*/
  uint32_t nativeTime = OTA2Native32(gZclTimeServerAttrs.Time); 
  int32_t nativeTimeZone = OTA2Native32(gZclTimeServerAttrs.TimeZone); 
  uint32_t nativeDstStart = OTA2Native32(gZclTimeServerAttrs.DstStart); 
  uint32_t nativeDstEnd = OTA2Native32(gZclTimeServerAttrs.DstEnd); 
  uint32_t nativeDstShift = OTA2Native32(gZclTimeServerAttrs.DstShift); 
  uint32_t nativeLocalTime = OTA2Native32(gZclTimeServerAttrs.LocalTime); 
  uint32_t nativeStandardTime = OTA2Native32(gZclTimeServerAttrs.StandardTime); 

  
  /* do calculations*/
  nativeTime++;
  
  /*Standard Time = Time + TimeZone*/
  nativeStandardTime = nativeTime + nativeTimeZone; 
  
  /*
    Local Time = Standard Time + DstShift (if DstStart <= Time <= DstEnd)
    Local Time = Standard Time (if Time < DstStart or Time > DstEnd)
  */
  if ((nativeDstStart <= nativeTime) && (nativeTime <= nativeDstEnd))
    nativeLocalTime = nativeStandardTime + nativeDstShift; 
  else
    nativeLocalTime = nativeStandardTime; 

  
  /*save and swap back to OTA endianness */
  gZclTimeServerAttrs.Time = Native2OTA32(nativeTime);
  gZclTimeServerAttrs.LocalTime = Native2OTA32(nativeLocalTime); 
  gZclTimeServerAttrs.StandardTime = Native2OTA32(nativeStandardTime); 
}

/*!
 * @fn 		void ZCL_TimeSecondTimerCallback(tmrTimerID_t timerid)
 *
 * @brief	Callback for the time cluster.
 *
 */
void ZCL_TimeSecondTimerCallback(tmrTimerID_t timerid)
{
(void) timerid;
  /* restart timer */
  TMR_StartLowPowerTimer (zclTimeSecondTimer, gTmrSecondTimer_c, TmrSeconds(1), ZCL_TimeSecondTimerCallback);

  /* update the Zcl Time cluster*/ 
  ZCL_TimeUpdate();
}

/*!
 * @fn 		void ZCL_TimeInit(ZCLTimeConf_t *pTimeConf)
 *
 * @brief	Init Time Cluster using pTimeConf parameter. Note input must be little endian.
 *
 */
void ZCL_TimeInit(ZCLTimeConf_t *pTimeConf)
{
  /*Initialize the Time cluster atributes */
  gZclTimeServerAttrs.Time = pTimeConf->Time;
  gZclTimeServerAttrs.TimeStatus = pTimeConf->TimeStatus;
  gZclTimeServerAttrs.TimeZone = pTimeConf->TimeZone;
  gZclTimeServerAttrs.DstStart = pTimeConf->DstStart;
  gZclTimeServerAttrs.DstEnd = pTimeConf->DstEnd;
  gZclTimeServerAttrs.DstShift = pTimeConf->DstShift;
  gZclTimeServerAttrs.LastSetTime = pTimeConf->Time;
  gZclTimeServerAttrs.ValidUntilTime = pTimeConf->ValidUntilTime;

  /* start the timer */
  if(zclTimeSecondTimer == gTmrInvalidTimerID_c)
	  zclTimeSecondTimer =  TMR_AllocateTimer();
  if(zclTimeSecondTimer != gTmrInvalidTimerID_c)
	  TMR_StartLowPowerTimer (zclTimeSecondTimer, gTmrSecondTimer_c, TmrSeconds(1),  ZCL_TimeSecondTimerCallback);
}

/*!
 * @fn 		void ZCL_SetUTCTime(uint32_t time)
 *
 * @brief	Set UTC time
 *
 */
void ZCL_SetUTCTime(uint32_t time)
{
  gZclTimeServerAttrs.Time = Native2OTA32(time);
  gZclTimeServerAttrs.LastSetTime = Native2OTA32(time);
}

/*!
 * @fn 		uint32_t ZCL_GetUTCTime(void)
 *
 * @brief	Get current UTC time
 *
 */
uint32_t ZCL_GetUTCTime(void)
{
  return OTA2Native32(gZclTimeServerAttrs.Time);
}

/*!
 * @fn 		void ZCL_SetTimeZone(int32_t timeZone)
 *
 * @brief	Set time zone
 *
 */
void ZCL_SetTimeZone(int32_t timeZone)
{
  gZclTimeServerAttrs.TimeZone = Native2OTA32(timeZone);
}

/*!
 * @fn 		void ZCL_SetTimeDst(uint32_t DstStart, uint32_t DstEnd, int32_t DstShift)
 *
 * @brief	Set DST Start, DST End and DST Shift time
 *
 */
void ZCL_SetTimeDst(uint32_t DstStart, uint32_t DstEnd, int32_t DstShift)
{
  gZclTimeServerAttrs.DstStart = Native2OTA32(DstStart);
  gZclTimeServerAttrs.DstEnd = Native2OTA32(DstEnd);
  gZclTimeServerAttrs.DstShift = Native2OTA32(DstShift);
}

/******************************
  RSSI Cluster
  See ZCL Specification Section 3.13
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_RSSILocationClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the RSSI Location Cluster Client. 
 *
 */
zbStatus_t ZCL_RSSILocationClusterClient
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
        case gZclCmdTxRSSI_DeviceConfigurationResponse_c:
        case gZclCmdTxRSSI_LocationDataResponse_c:
        case gZclCmdTxRSSI_LocationDataNotification_c:
        case gZclCmdTxRSSI_CompactLocationDataNotification_c:
        case gZclCmdTxRSSI_RSSIping_c:
          return gZbSuccess_c;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t ZCL_RSSILocationClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the RSSI Location Cluster Server. 
 *
 */
zbStatus_t ZCL_RSSILocationClusterServer
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
        case gZclCmdRxRSSI_SetAbsoluteLocation_c:
        case gZclCmdRxRSSI_SetDeviceConfiguration_c:
        case gZclCmdRxRSSI_GetDeviceConfiguration_c:
        case gZclCmdRxRSSI_GetLocationData_c:

          /* These should be passed up to a host*/
          return status;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t RSSILocation_SetAbsoluteLocation(zclCmdRSSI_AbsoluteLocation_t *pReq) 
 *
 * @brief	Sends over-the-air an SetAbsoluteLocation command from the RSSI Location Cluster client. 
 *
 */
zbStatus_t RSSILocation_SetAbsoluteLocation
(
zclCmdRSSI_AbsoluteLocation_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxRSSI_SetAbsoluteLocation_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t RSSILocation_SetDeviceConfiguration(zclCmdRSSI_SetDeviceConfiguration_t *pReq) 
 *
 * @brief	Sends over-the-air an SetDeviceConfiguration command from the RSSI Location Cluster client. 
 *
 */
zbStatus_t RSSILocation_SetDeviceConfiguration
(
zclCmdRSSI_SetDeviceConfiguration_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxRSSI_SetDeviceConfiguration_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t RSSILocation_GetDeviceConfiguration(zclCmdRSSI_GetDeviceConfiguration_t *pReq) 
 *
 * @brief	Sends over-the-air an GetDeviceConfiguration command from the RSSI Location Cluster client. 
 *
 */
zbStatus_t RSSILocation_GetDeviceConfiguration
(
zclCmdRSSI_GetDeviceConfiguration_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxRSSI_GetDeviceConfiguration_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t RSSILocation_GetLocationData(zclCmdRSSI_GetLocationData_t *pReq) 
 *
 * @brief	Sends over-the-air an GetLocationData command from the RSSI Location Cluster client. 
 *
 */
zbStatus_t RSSILocation_GetLocationData
(
zclCmdRSSI_GetLocationData_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxRSSI_GetLocationData_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t RSSILocation_DeviceConfigurationResponse(zclCmdRSSI_DeviceConfigurationResponse_t *pReq) 
 *
 * @brief	Sends over-the-air an DeviceConfigurationResponse command from the RSSI Location Cluster Server. 
 *
 */
zbStatus_t RSSILocation_DeviceConfigurationResponse
(
zclCmdRSSI_DeviceConfigurationResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxRSSI_DeviceConfigurationResponse_c,0,(zclGenericReq_t *)pReq);
}


/*!
 * @fn 		zbStatus_t RSSILocation_LocationDataResponse(zclCmdRSSI_LocationDataResponse_t *pReq) 
 *
 * @brief	Sends over-the-air an LocationDataResponse command from the RSSI Location Cluster Server. 
 *
 */
zbStatus_t RSSILocation_LocationDataResponse
(
zclCmdRSSI_LocationDataResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxRSSI_LocationDataResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t RSSILocation_LocationDataNotification(zclCmdRSSI_LocationDataNotification_t *pReq) 
 *
 * @brief	Sends over-the-air an LocationDataNotification command from the RSSI Location Cluster Server. 
 *
 */
zbStatus_t RSSILocation_LocationDataNotification
(
zclCmdRSSI_LocationDataNotification_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxRSSI_LocationDataNotification_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t RSSILocation_CompactLocationDataNotification(zclCmdRSSI_CompactLocationDataNotification_t *pReq) 
 *
 * @brief	Sends over-the-air an CompactLocationDataNotification command from the RSSI Location Cluster Server. 
 *
 */
zbStatus_t RSSILocation_CompactLocationDataNotification
(
zclCmdRSSI_CompactLocationDataNotification_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxRSSI_CompactLocationDataNotification_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t RSSILocation_RSSIping(zclCmdRSSI_RSSIping_t *pReq) 
 *
 * @brief	Sends over-the-air an RSSIping command from the RSSI Location Cluster Client. 
 *
 */
zbStatus_t RSSILocation_RSSIping
(
zclCmdRSSI_RSSIping_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxRSSI_RSSIping_c,0,(zclGenericReq_t *)pReq);
}


/******************************
  Binary Input(Basic)Cluster
  See ZCL Specification  Section 3.14.4
*******************************/

/* Binary Input(basic) Cluster Attribute Definitions */
const zclAttrDef_t gaZclBinaryInputClusterFirstSetAttrDef[] = {
#if gZclClusterOptionals_d
  { gZclAttrIdBinaryInputActiveText_c,   gZclDataTypeStr_c, gZclAttrFlagsInRAM_c, sizeof(zclStr16_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, activeText)},
  { gZclAttrIdBinaryInputDescription_c,  gZclDataTypeStr_c, gZclAttrFlagsInRAM_c, sizeof(zclStr16_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, description)},
  { gZclAttrIdBinaryInputInactiveText_c, gZclDataTypeStr_c, gZclAttrFlagsInRAM_c, sizeof(zclStr16_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, inactiveText)},
#endif 
  { gZclAttrIdBinaryInputOutOfService_c, gZclDataTypeBool_c , gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, outOfService)},
#if gZclClusterOptionals_d
  { gZclAttrIdBinaryInputPolarity_c, gZclDataTypeEnum8_c , gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, polarity)},
#endif 
  { gZclAttrIdBinaryInputPresentValue_c, gZclDataTypeBool_c , gZclAttrFlagsInRAM_c| gZclAttrFlagsReportable_c, sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t,presentValue)},
#if gZclClusterOptionals_d
  { gZclAttrIdBinaryInputReliability_c, gZclDataTypeEnum8_c , gZclAttrFlagsInRAM_c , sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t,reliability)},
#endif 
  { gZclAttrIdBinaryInputStatusFlags_c, gZclDataTypeBitmap8_c , gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c, sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, statusFlags)}
};

#if gZclClusterOptionals_d
const zclAttrDef_t gaZclBinaryInputClusterSecondSetAttrDef[] = {
  { gZclAttrIdBinaryInputApplicationType_c,gZclDataTypeUint32_c  , gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c ,sizeof(uint8_t),(void *)MbrOfs(zclBinaryInputAttrsRAM_t,applicationType)}
};
#endif 

const zclAttrSet_t gaZclBinaryInputClusterAttrSet[] = {
  {gZclAttrBinaryInputFirstSet_c, (void *)&gaZclBinaryInputClusterFirstSetAttrDef, NumberOfElements(gaZclBinaryInputClusterFirstSetAttrDef)}
#if gZclClusterOptionals_d  
  ,{gZclAttrBinaryInputSecondSet_c, (void *)&gaZclBinaryInputClusterSecondSetAttrDef, NumberOfElements(gaZclBinaryInputClusterSecondSetAttrDef)}
#endif 
};

const zclAttrSetList_t gZclBinaryInputClusterAttrSetList = {
  NumberOfElements(gaZclBinaryInputClusterAttrSet),
  gaZclBinaryInputClusterAttrSet
};

/******************************
  Commissioning Cluster
  See Commisioning cluser Specification 064699r12
*******************************/
tmrTimerID_t CommisioningTimer;
void LeaveTimerCallback(tmrTimerID_t timerid);
void StartTimerCallback(tmrTimerID_t timerid);
static uint8_t RestartDelay;
static uint16_t RestartJitter;
static bool_t RestartFromAttributeSet;
#include "TMR_Interface.h"
#include "ZdoApsInterface.h"

zbCommissioningAttributes_t gCommisioningServerAttrsData = {
/* initialize like ROM set..*/
  /*** Startup Attribute Set (064699r12, section 6.2.2.1) ***/
  {mDefaultNwkShortAddress_c},        /* x shortAddress (default 0xff,0xff) */
  {mDefaultNwkExtendedPANID_c},       /* x nwkExtendedPANId */
  {mDefultApsUseExtendedPANID_c},     /* x apsUSeExtendedPANId */
  {mDefaultValueOfPanId_c},           /* x panId */
  { (uint8_t)(mDefaultValueOfChannel_c & 0xff),
    (uint8_t)((mDefaultValueOfChannel_c>>8) & 0xff),
    (uint8_t)((mDefaultValueOfChannel_c>>16) & 0xff),
    (uint8_t)((mDefaultValueOfChannel_c>>24) &0xff)
  },                                  /* x channelMask */
   mNwkProtocolVersion_c,            /* x protocolVersion, always 0x02=ZigBee 2006, 2007 */
   gDefaultValueOfNwkStackProfile_c, /* x stackProfile 0x01 or 0x02 */
   gStartupControl_Associate_c,      /* startupControl */
  {mDefaultValueOfTrustCenterLongAddress_c},  /* x trustCenterAddress */
  {mDefaultValueOfTrustCenterMasterKey_c},  /* trustCenterMasterKey */
  {mDefaultValueOfNetworkKey_c},      /* x networkKey */
   gApsUseInsecureJoinDefault_c,     /* x useInsecureJoin */
  {mDefaultValueOfTrustCenterLinkKey_c},  /* preconfiguredLinkKey (w/ trust center) */
   mDefaultValueOfNwkActiveKeySeqNumber_c, /* x networkKeySeqNum */
   mDefaultValueOfNwkKeyType_c,      /* x networkKeyType */
  {gNwkManagerShortAddr_c},           /* x networkManagerAddress, little endian */

  /*** Join Parameters Attribute Set (064699r12, section 6.2.2.2) ***/
   mDefaultValueOfNwkScanAttempts_c, /* x # of scan attempts */
  { (mDefaultValueOfNwkTimeBwnScans_c & 0xff),
    (mDefaultValueOfNwkTimeBwnScans_c >> 8)
  },                                  /* x time between scans(ms) */
  {(mDefaultValueOfRejoinInterval_c&0xff),
   (mDefaultValueOfRejoinInterval_c>>8)
  },                                  /* x rejoin interval (sec) */
  {(mDefaultValueOfMaxRejoinInterval_c & 0xff),
   (mDefaultValueOfMaxRejoinInterval_c >> 8)
  },                                  /* x maxRejoinInterval (sec) */

  /*** End-Device Parameters Attribute Set (064699r12, section 6.2.2.3) ***/
  {(mDefaultValueOfIndirectPollRate_c & 0xff),
   (mDefaultValueOfIndirectPollRate_c >> 8)
  },                                  /* x indirectPollRate(ms) */
  gMaxNwkLinkRetryThreshold_c,      /* x parentRetryThreshold */

  /*** Concentrator Parameters Attribute Set (064699r12, section 6.2.2.4) ***/
  gConcentratorFlag_d,              /* x concentratorFlag */
  gConcentratorRadius_c,            /* x concentratorRadius */
  gConcentratorDiscoveryTime_c,     /* x concentratorDiscoveryTime */
};

const zclAttrDef_t gaZclCommisioningServerAttrDef[] = {
/* Server attributes */
  { gZclAttrIdCommissioning_ShortAddressID_c,              gZclDataTypeUint16_c,        gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint16_t),       (void *)MbrOfs(zbCommissioningAttributes_t, aShortAddress) },
  { gZclAttrIdCommissioning_ExtendedPANIdID_c,             gZclDataTypeIeeeAddr_c,      gZclAttrFlagsInRAM_c,                           sizeof(zbIeeeAddr_t),   (void *)MbrOfs(zbCommissioningAttributes_t, aNwkExtendedPanId) },
  { gZclAttrIdCommissioning_PanIdID_c,                     gZclDataTypeUint16_c,        gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint16_t),       (void *)MbrOfs(zbCommissioningAttributes_t, aPanId) },
  { gZclAttrIdCommissioning_ChannelMaskID_c,               gZclDataTypeBitmap32_c,      gZclAttrFlagsInRAM_c,                           sizeof(zbChannels_t),   (void *)MbrOfs(zbCommissioningAttributes_t, aChannelMask) },
  { gZclAttrIdCommissioning_ProtocolVersionID_c,           gZclDataTypeUint8_c,         gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, protocolVersion) },
  { gZclAttrIdCommissioning_StackProfileID_c,              gZclDataTypeUint8_c,         gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, stackProfile) },
  { gZclAttrIdCommissioning_StartupControlID_c,            gZclDataTypeEnum8_c,         gZclAttrFlagsInRAM_c,                           sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, startupControl) },
  { gZclAttrIdCommissioning_TrustCenterAddressID_c,        gZclDataTypeIeeeAddr_c,      gZclAttrFlagsInRAM_c,                           sizeof(zbIeeeAddr_t),   (void *)MbrOfs(zbCommissioningAttributes_t, aTrustCenterAddress) },
  { gZclAttrIdCommissioning_TrustCenterMasterKeyID_c,      gZclDataTypeSecurityKey_c,   gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(zbAESKey_t),     (void *)MbrOfs(zbCommissioningAttributes_t, aTrustCenterMasterKey) },
  { gZclAttrIdCommissioning_NetworkKeyID_c,                gZclDataTypeSecurityKey_c,   gZclAttrFlagsInRAM_c,                           sizeof(zbAESKey_t),     (void *)MbrOfs(zbCommissioningAttributes_t, aNetworkKey) },
  { gZclAttrIdCommissioning_UseInsecureJoinID_c,           gZclDataTypeUint8_c,         gZclAttrFlagsInRAM_c,                           sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, fUseInsecureJoin) },
  { gZclAttrIdCommissioning_PreconfiguredLinkKeyID_c,      gZclDataTypeSecurityKey_c,   gZclAttrFlagsInRAM_c,                           sizeof(zbAESKey_t),     (void *)MbrOfs(zbCommissioningAttributes_t, aPreconfiguredTrustCenterLinkKey) },
  { gZclAttrIdCommissioning_NetworkKeySeqNumID_c,          gZclDataTypeUint8_c,         gZclAttrFlagsInRAM_c,                           sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, activeNetworkKeySeqNum) },
  { gZclAttrIdCommissioning_NetworkKeyTypeID_c,            gZclDataTypeUint8_c,         gZclAttrFlagsInRAM_c,                           sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, networkKeyType) },
  { gZclAttrIdCommissioning_NetworkManagerAddressID_c,     gZclDataTypeUint16_c,        gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint16_t),       (void *)MbrOfs(zbCommissioningAttributes_t, aNetworkManagerAddress) },
  { gZclAttrIdCommissioning_ScanAttemptsID_c,              gZclDataTypeUint8_c,         gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, scanAttempts) },
  { gZclAttrIdCommissioning_TimeBetweenScansID_c,          gZclDataTypeUint16_c,        gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint16_t),       (void *)MbrOfs(zbCommissioningAttributes_t, aTimeBetweenScans) },
  { gZclAttrIdCommissioning_RejoinIntervalID_c,            gZclDataTypeUint16_c,        gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint16_t),       (void *)MbrOfs(zbCommissioningAttributes_t, aRejoinInterval) },
  { gZclAttrIdCommissioning_MaxRejoinIntervalID_c,         gZclDataTypeUint16_c,        gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint16_t),       (void *)MbrOfs(zbCommissioningAttributes_t, aMaxRejoinInterval) },
  { gZclAttrIdCommissioning_IndirectPollRateID_c,          gZclDataTypeUint16_c,        gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint16_t),       (void *)MbrOfs(zbCommissioningAttributes_t, aIndirectPollRate) },
  { gZclAttrIdCommissioning_ParentRetryThreshoID_c,        gZclDataTypeUint8_c,         gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, parentLinkRetryThreshold) },
  { gZclAttrIdCommissioning_ConcentratorFlagID_c,          gZclDataTypeBool_c,          gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(bool_t),         (void *)MbrOfs(zbCommissioningAttributes_t, fConcentratorFlag) },
  { gZclAttrIdCommissioning_ConcentratorRadiusID_c,        gZclDataTypeUint8_c,         gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, concentratorRadius) },
  { gZclAttrIdCommissioning_ConcentratorDiscoveryTimeID_c, gZclDataTypeUint8_c,         gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, concentratorDiscoveryTime) }
};

const zclAttrSet_t gZclCommissioningServerClusterAttrSet[] = {
  {gZclAttrSetBasicDeviceInformation_c, (void *)&gaZclCommisioningServerAttrDef, NumberOfElements(gaZclCommisioningServerAttrDef)}
};

const zclAttrSetList_t gZclCommissioningServerClusterAttrSetList = {
  NumberOfElements(gZclCommissioningServerClusterAttrSet),
  gZclCommissioningServerClusterAttrSet
};

ZdoStartMode_t   gStartUpMode_DeviceType = 0;
/*!
 * @fn 		zbStatus_t ZCL_CommisioningClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Commissioning Cluster Server. 
 *			This cluster allows over-the-air updates of key commissioning values such as PAN ID, extended PAN 
 * 			ID and preconfigured security keys. It can also restart the remote node. This allows a 
 * 			commissioning tool to set up a ZigBee node on a "commissioning" network, then tell the node to go
 * 			join the "final" network.
 *
 */
zbStatus_t ZCL_CommisioningClusterServer
(
  zbApsdeDataIndication_t *pIndication,   /* IN: must be non-null */
  afDeviceDef_t *pDevice                  /* IN: must be non-null */
)
{
  zclFrame_t *pFrame;
  zclCommissioningCmd_t *Command; 
  zbStatus_t status = gZclSuccess_c; 
  (void)pDevice;

  /* ZCL frame */
  pFrame = (zclFrame_t*)pIndication->pAsdu;
  gZcl_ep = pIndication->dstEndPoint;
  Command = (zclCommissioningCmd_t *) (&(pFrame->command) + sizeof(uint8_t));

  switch (pFrame->command)
  {
    case gZclCmdCommissiong_RestartDeviceRequest_c:
      if((Command->RestartDeviceCmd.Options & gCommisioningClusterRestartDeviceRequestOptions_StartUpModeSubField_Mask_c) ==
          gCommisioningClusterRestartDeviceRequestOptions_ModeSubField_RestartUsingCurrentStartupParameters_c) 
      {
       /*
         Consistency check is done on:
         Start up control (form network) if device is not capable of being a coordinator
         Start up control (silent start) if stack profile is 1.
         Pan ID is checked whether it is within range
       */

#if (!(gComboDeviceCapability_d || gCoordinatorCapability_d))
        /* Form network startup only allowed for coordinator or combo device */
        if(gCommisioningServerAttrsData.startupControl == gStartupControl_Form_c) {
          status = gZclInconsistentStatupState_c;
        }
#endif
        // Check that PAN ID is set to 0xFFFF or below 0xFFFE
        if ((gCommisioningServerAttrsData.aPanId[0] != 0xFF) && (gCommisioningServerAttrsData.aPanId[1] != 0xFF))
        {
          if(gCommisioningServerAttrsData.aPanId[0] > gaPIDUpperLimit[0]) 
          {
            status = gZclInconsistentStatupState_c;
          }
        }

        if (gCommisioningServerAttrsData.startupControl == gStartupControl_SilentStart_c)
        {
#if (gDefaultValueOfNwkStackProfile_c == 0x01)
          /* consistency check for stack profile 1 */
          status = gZclInconsistentStatupState_c;
#else
          status = gZclSuccess_c;
#endif
        }

        RestartFromAttributeSet = TRUE;
      } else

        if((Command->RestartDeviceCmd.Options & gCommisioningClusterRestartDeviceRequestOptions_StartUpModeSubField_Mask_c) ==
            gCommisioningClusterRestartDeviceRequestOptions_ModeSubField_RestartUsingCurrentStackParameter_c) 
        {
          RestartFromAttributeSet = FALSE;
        }
        else
        {
          // invalid command, return error code.
          status = gZclUnsupportedClusterCommand_c;
        }

        /*
          if the device is a combo then determine the Type of Devices for the
          start mode.
        */
        gStartUpMode_DeviceType = gCommisioningServerAttrsData.startupControl == gStartupControl_Form_c ? gZdoStartMode_Zc_c:gZdoStartMode_Zr_c;

        /* Set up start delay and just and initiate leave */
        RestartDelay = Command->RestartDeviceCmd.Delay;
        RestartJitter = GetRandomRange(0,Command->RestartDeviceCmd.Jitter) * 80;
        CommisioningTimer = TMR_AllocateTimer();
        status = gZclInconsistentStatupState_c;
        if (CommisioningTimer != gTmrInvalidTimerID_c)
        {
          /*Start timer.with +200 milliseconds so the ZCL response is sent before leave is initiated.*/
          TMR_StartSingleShotTimer(CommisioningTimer,200,LeaveTimerCallback);
          status = gZclSuccess_c;
        }

        if (status != gZclSuccess_c) 
        {
          (void) ZCL_Reply(pIndication, sizeof(status), &status);
          return gZclSuccess_c;
        }
    break;

    case gZclCmdCommissiong_SaveStartupParametersRequest_c:
      // Optional - not supported.
      status = gZclUnsupportedClusterCommand_c;
    break;

    case gZclCmdCommissiong_RestoreStartupParametersRequest_c:
      // Optional - not supported.
      status = gZclUnsupportedClusterCommand_c;
    break;

    case gZclCmdCommissiong_ResetStartupParametersRequest_c:
      if ((Command->ResetStartupParameterCmd.Options & gCommisioningClusterResetStartupParametersRequestOptions_ResetCurrentSubField_Mask_c) ||
          (Command->ResetStartupParameterCmd.Options & gCommisioningClusterResetStartupParametersRequestOptions_ResetAllSubField_Mask_c)) 
      {
        /*Copy set from ROM to ram */ 
        FLib_MemCpy(&gCommisioningServerAttrsData, (void *) &gSAS_Rom, sizeof(gSAS_Rom));
      }
      else
        if (Command->ResetStartupParameterCmd.Options & gCommisioningClusterResetStartupParametersRequestOptions_EraseIndexSubField_Mask_c) 
        {
          /* any other combination return error */  
          status = gZclUnsupportedClusterCommand_c; 
        }
    break;
    
  default:
    return gZclUnsupportedClusterCommand_c;
  }

  (void)ZCL_Reply(pIndication, sizeof(status), &status);
  return gZclSuccess_c;
}

/*!
 * @fn 		zbStatus_t ZCL_CommisioningClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Commissioning Cluster Client. 
 *			This cluster allows over-the-air updates of key commissioning values such as PAN ID, extended PAN 
 * 			ID and preconfigured security keys. It can also restart the remote node. This allows a 
 * 			commissioning tool to set up a ZigBee node on a "commissioning" network, then tell the node to go
 * 			join the "final" network.
 *
 */
zbStatus_t ZCL_CommisioningClusterClient
(
  zbApsdeDataIndication_t *pIndication,   /* IN: must be non-null */
  afDeviceDef_t *pDevice                  /* IN: must be non-null */
)
{
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccessDefaultRsp_c; 
  /*  zclCmdCommissiong_response_t *Command; */
 
  (void)pDevice;

  /* ZCL frame */
  pFrame = (zclFrame_t*)pIndication->pAsdu;
  gZcl_ep = pIndication->dstEndPoint;
/*  Command = (zclCmdCommissiong_response_t *) (&(pFrame->command) + sizeof(uint8_t));*/
  
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	   status = gZclSuccess_c;  

  switch (pFrame->command)
  {
    case gZclCmdCommissiong_RestartDeviceResponse_c:
    case gZclCmdCommissiong_SaveStartupParametersResponse_c:
    case gZclCmdCommissiong_RestoreStartupParametersResponse_c:
    case gZclCmdCommissiong_ResetStartupParametersResponse_c:
      return status;   
    default:
      return gZclUnsupportedClusterCommand_c;      
  }
  }


/* Commissioning cluster Client commands */
#if gASL_ZclCommissioningRestartDeviceRequest_d
/*!
 * @fn 		zbStatus_t ZCL_Commisioning_RestartDeviceReq(zclCommissioningRestartDeviceReq_t *pReq) 
 *
 * @brief	Sends over-the-air a RestartDeviceRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t  ZCL_Commisioning_RestartDeviceReq
(
	zclCommissioningRestartDeviceReq_t *pReq
)
{
	Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterCommissioning_c);
   return ZCL_GenericReq(gZclCmdCommissiong_RestartDeviceRequest_c, sizeof(zclCmdCommissiong_RestartDeviceRequest_t),(zclGenericReq_t *)pReq);	
}
#endif


#if gASL_ZclCommissioningSaveStartupParametersRequest_d
/*!
 * @fn 		zbStatus_t ZCL_Commisioning_SaveStartupParametersReq(zclCommissioningSaveStartupParametersReq_t *pReq) 
 *
 * @brief	Sends over-the-air a SaveStartupParametersRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t  ZCL_Commisioning_SaveStartupParametersReq
(
	zclCommissioningSaveStartupParametersReq_t *pReq
)
{
	Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterCommissioning_c);
   return ZCL_GenericReq(gZclCmdCommissiong_SaveStartupParametersRequest_c, sizeof(zclCmdCommissiong_SaveStartupParametersRequest_t),(zclGenericReq_t *)pReq);	
}
#endif


#if gASL_ZclCommissioningRestoreStartupParametersRequest_d
/*!
 * @fn 		zbStatus_t ZCL_Commisioning_RestoreStartupParametersReq(zclCommissioningRestoreStartupParametersReq_t *pReq) 
 *
 * @brief	Sends over-the-air a RestoreStartupParametersRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t  ZCL_Commisioning_RestoreStartupParametersReq
(
	zclCommissioningRestoreStartupParametersReq_t *pReq
)
{
	Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterCommissioning_c);
   return ZCL_GenericReq(gZclCmdCommissiong_RestoreStartupParametersRequest_c, sizeof(zclCmdCommissiong_RestoreStartupParametersRequest_t),(zclGenericReq_t *)pReq);	
}
#endif



#if gASL_ZclCommissioningResetStartupParametersRequest_d
/*!
 * @fn 		zbStatus_t ZCL_Commisioning_ResetStartupParametersReq(zclCommissioningResetStartupParametersReq_t *pReq) 
 *
 * @brief	Sends over-the-air a ResetStartupParametersRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t  ZCL_Commisioning_ResetStartupParametersReq
(
	zclCommissioningResetStartupParametersReq_t *pReq
)
{
	Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterCommissioning_c);
   return ZCL_GenericReq(gZclCmdCommissiong_ResetStartupParametersRequest_c, sizeof(zclCmdCommissiong_ResetStartupParametersRequest_t),(zclGenericReq_t *)pReq);	
} 
#endif


/**************************************************************************
	Alpha-Secure Key Establishment Cluster (Health Care Profile Annex A.2)
***************************************************************************/
/*!
 * @fn 		zbStatus_t ZCL_ASKEClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ZCL_ASKEClusterClient
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
        case gZclCmdTxASKE_ConfigureSDResponse_c:
        case gZclCmdTxASKE_UpdateRevocationListResponse_c:
        case gZclCmdTxASKE_RemoveSDResponse_c:
        case gZclCmdTxASKE_ReadSDResponse_c:
        case gZclCmdTxASKE_InitiateASKEResponse_c:
        case gZclCmdTxASKE_ConfirmASKEkeyResponse_c:
        case gZclCmdRxASKE_TerminateASKE_c:
        case gZclCmdTxASKE_GenerateAMKResponse_c:
        case gZclCmdTxASKE_ReportRevokedNode_c:
        case gZclCmdTxASKE_RequestConfiguration_c:

          /* These should be passed up to a host*/
          return status;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t ZCL_ASKEClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ZCL_ASKEClusterServer
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
        case gZclCmdRxASKE_ConfigureSD_c:
        case gZclCmdRxASKE_UpdateRevocationList_c:
        case gZclCmdRxASKE_RemoveSD_c:
        case gZclCmdRxASKE_ReadSD_c:
        case gZclCmdRxASKE_InitiateASKE_c:
        case gZclCmdRxASKE_ConfirmASKEkey_c:
        case gZclCmdRxASKE_TerminateASKE_c:
        case gZclCmdRxASKE_GenerateAMK_c:
          /* These should be passed up to a host*/
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t ASKE_ConfigureSD(zclCmdASKE_ConfigureSD_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureSD command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_ConfigureSD
(
zclCmdASKE_ConfigureSD_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_ConfigureSD_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_UpdateRevocationList(zclCmdASKE_UpdateRevocationList_t *pReq) 
 *
 * @brief	Sends over-the-air a UpdateRevocationList command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_UpdateRevocationList
(
zclCmdASKE_UpdateRevocationList_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_UpdateRevocationList_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_RemoveSD(zclCmdASKE_RemoveSD_t *pReq) 
 *
 * @brief	Sends over-the-air a RemoveSD command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_RemoveSD
(
zclCmdASKE_RemoveSD_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_RemoveSD_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_ReadSD(zclCmdASKE_ReadSD_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadSD command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_ReadSD
(
zclCmdASKE_ReadSD_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_ReadSD_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_InitiateASKE(zclCmdASKE_InitiateASKE_t *pReq) 
 *
 * @brief	Sends over-the-air a InitiateASKE command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_InitiateASKE
(
zclCmdASKE_InitiateASKE_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_InitiateASKE_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_ConfirmASKEkey(zclCmdASKE_ConfirmASKEkey_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfirmASKEkey command from the Alpha-Secure Key Establishment Cluster Client.  
 *
 */
zbStatus_t ASKE_ConfirmASKEkey
(
zclCmdASKE_ConfirmASKEkey_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_ConfirmASKEkey_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_TerminateASKE(zclCmdASKE_TerminateASKE_t *pReq) 
 *
 * @brief	Sends over-the-air a TerminateASKE command from the Alpha-Secure Key Establishment Cluster Client.  
 *
 */
zbStatus_t ASKE_TerminateASKE
(
zclCmdASKE_TerminateASKE_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_TerminateASKE_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_GenerateAMK(zclCmdASKE_GenerateAMK_t *pReq) 
 *
 * @brief	Sends over-the-air a GenerateAMK command from the Alpha-Secure Key Establishment Cluster Client.  
 *
 */
zbStatus_t ASKE_GenerateAMK
(
zclCmdASKE_GenerateAMK_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_GenerateAMK_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_ConfigureSDresponse(zclCmdASKE_ConfigureSDresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureSDresponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_ConfigureSDresponse
(
zclCmdASKE_ConfigureSDresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_ConfigureSDResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_UpdateRevocationListResponse(zclCmdASKE_UpdateRevocationListResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a UpdateRevocationListResponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_UpdateRevocationListResponse
(
zclCmdASKE_UpdateRevocationListResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_UpdateRevocationListResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_RemoveSDresponse(zclCmdASKE_RemoveSDresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a RemoveSDresponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_RemoveSDresponse
(
zclCmdASKE_RemoveSDresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_RemoveSDResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_ReadSDresponse(zclCmdASKE_ReadSDresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadSDresponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_ReadSDresponse
(
zclCmdASKE_ReadSDresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_ReadSDResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_InitiateASKEresponse(zclCmdASKE_InitiateASKEresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a InitiateASKEresponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_InitiateASKEresponse
(
zclCmdASKE_InitiateASKEresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_InitiateASKEResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_ConfirmASKEkeyResponse(zclCmdASKE_ConfirmASKEkeyResponse_t *pReq);
 *
 * @brief	Sends over-the-air a ConfirmASKEkeyResponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_ConfirmASKEkeyResponse
(
zclCmdASKE_ConfirmASKEkeyResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_ConfirmASKEkeyResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_GenerateAMKresponse(zclCmdASKE_GenerateAMKresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a GenerateAMKresponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_GenerateAMKresponse
(
zclCmdASKE_GenerateAMKresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_GenerateAMKResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_ReportRevokedNode(zclCmdASKE_ReportRevokedNode_t *pReq) 
 *
 * @brief	Sends over-the-air a ReportRevokedNode command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_ReportRevokedNode
(
zclCmdASKE_ReportRevokedNode_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_ReportRevokedNode_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_RequestConfiguration(zclCmdASKE_RequestConfiguration_t *pReq) 
 *
 * @brief	Sends over-the-air a RequestConfiguration command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_RequestConfiguration
(
zclCmdASKE_RequestConfiguration_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_RequestConfiguration_c,0,(zclGenericReq_t *)pReq);
}


/**************************************************************************
	Alpha-Secure Access Control Cluster (Health Care Profile Annex A.3)
***************************************************************************/
/*!
 * @fn 		zbStatus_t ZCL_ASACClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alpha-Secure Access Control Cluster Client. 
 *
 */
zbStatus_t ZCL_ASACClusterClient
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
        case gZclCmdTxASAC_ConfigureSDresponse_c:
        case gZclCmdTxASAC_UpdateRevocationListResponse_c:
        case gZclCmdTxASAC_ConfigureACpoliciesResponse_c:
        case gZclCmdTxASAC_ReadSDresponse_c:
        case gZclCmdTxASAC_ReadACpoliciesResponse_c:
        case gZclCmdTxASAC_RemoveACresponse_c:
        case gZclCmdTxASAC_ACpropertiesResponse_c:
        case gZclCmdTxASAC_TSreport_c:
        case gZclCmdTxASAC_InitiateASACresponse_c:
        case gZclCmdTxASAC_ConfirmASACkeyResponse_c:
        case gZclCmdTxASAC_TerminateASAC_c:
        case gZclCmdTxASAC_GenerateAMKresponse_c:
        case gZclCmdTxASAC_LDCtransportResponse_c:
        case gZclCmdTxASAC_RequestConfiguration_c:

          /* These should be passed up to a host*/
          return status;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t ZCL_ASACClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alpha-Secure Access Control Cluster Server. 
 *
 */
zbStatus_t ZCL_ASACClusterServer
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
        case gZclCmdRxASAC_ConfigureSD_c:
        case gZclCmdRxASAC_UpdateRevocationList_c:
        case gZclCmdRxASAC_ConfigureACpolicies_c:
        case gZclCmdRxASAC_ReadSD_c:
        case gZclCmdRxASAC_ReadACpolicies_c:
        case gZclCmdRxASAC_RemoveAC_c:
        case gZclCmdRxASAC_ACpropertiesReq_c:
        case gZclCmdRxASAC_TSreportResponse_c:
        case gZclCmdRxASAC_InitiateASAC_c:
        case gZclCmdRxASAC_ConfirmASACkey_c:
        case gZclCmdRxASAC_TerminateASAC_c:
        case gZclCmdRxASAC_GenerateAMK_c:
        case gZclCmdRxASAC_LDCtransport_c:
        
          /* These should be passed up to a host*/
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t ASAC_ConfigureSD(zclCmdASAC_ConfigureSD_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureSD command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ConfigureSD
(
  zclCmdASAC_ConfigureSD_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ConfigureSD_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_UpdateRevocationList(zclCmdASAC_UpdateRevocationList_t *pReq) 
 *
 * @brief	Sends over-the-air a UpdateRevocationList command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_UpdateRevocationList
(
  zclCmdASAC_UpdateRevocationList_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_UpdateRevocationList_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ConfigureACpolicies(zclCmdASAC_ConfigureACpolicies_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureACpolicies command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ConfigureACpolicies
(
  zclCmdASAC_ConfigureACpolicies_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ConfigureACpolicies_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ReadSD(zclCmdASAC_ReadSD_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadSD command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ReadSD
(
  zclCmdASAC_ReadSD_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ReadSD_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ReadACpolicies(zclCmdASAC_ReadACpolicies_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadACpolicies command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ReadACpolicies
(
  zclCmdASAC_ReadACpolicies_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ReadACpolicies_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_RemoveAC(zclCmdASAC_RemoveAC_t *pReq) 
 *
 * @brief	Sends over-the-air a RemoveAC command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_RemoveAC
(
  zclCmdASAC_RemoveAC_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_RemoveAC_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ACpropertiesRequest(void) 
 *
 * @brief	Sends over-the-air a ACpropertiesRequest command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ACpropertiesRequest(void)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
//    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ACpropertiesReq_c,0,(zclGenericReq_t *)pReq);
  return gZbSuccess_c;
}

/*!
 * @fn 		zbStatus_t ASAC_TSReportResponse(void) 
 *
 * @brief	Sends over-the-air a TSReportResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_TSReportResponse(void)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
//    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_TSreportResponse_c,0,(zclGenericReq_t *)pReq);
return gZbSuccess_c;
}

/*!
 * @fn 		zbStatus_t ASAC_InitiateASAC(zclCmdASAC_InitiateASAC_t *pReq) 
 *
 * @brief	Sends over-the-air a InitiateASAC command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_InitiateASAC
(
  zclCmdASAC_InitiateASAC_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_InitiateASAC_c,0,(zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t ASAC_ConfirmASACkey(zclCmdASAC_ConfirmASAC_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfirmASACkey command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ConfirmASACkey
(
  zclCmdASAC_ConfirmASAC_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ConfirmASACkey_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_TerminateASAC(zclCmdASAC_TerminateASAC_t *pReq) 
 *
 * @brief	Sends over-the-air a TerminateASAC command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_TerminateASAC
(
  zclCmdASAC_TerminateASAC_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_TerminateASAC_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_GenerateAMK(zclCmdASAC_GenerateAMK_t *pReq) 
 *
 * @brief	Sends over-the-air a GenerateAMK command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_GenerateAMK
(
  zclCmdASAC_GenerateAMK_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_GenerateAMK_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_LDCtransport(zclCmdASAC_LDCtransport_t *pReq) 
 *
 * @brief	Sends over-the-air a LDCtransport command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_LDCtransport
(
  zclCmdASAC_LDCtransport_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_LDCtransport_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ConfigureSDresponse(zclCmdASAC_ConfigureSDresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureSDresponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ConfigureSDresponse
(
  zclCmdASAC_ConfigureSDresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ConfigureSDresponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_UpdateRevocationListResponse(zclCmdASAC_UpdateRevocationListResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a UpdateRevocationListResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_UpdateRevocationListResponse
(
  zclCmdASAC_UpdateRevocationListResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_UpdateRevocationListResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ConfigureACpoliciesResponse(zclCmdASAC_ConfigureACpoliciesResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureACpoliciesResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ConfigureACpoliciesResponse
(
  zclCmdASAC_ConfigureACpoliciesResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ConfigureACpoliciesResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ReadSDresponse(zclCmdASAC_ReadSDresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadSDresponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ReadSDresponse
(
  zclCmdASAC_ReadSDresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ReadSDresponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ReadACpoliciesResponse(zclCmdASAC_ReadACpoliciesResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadACpoliciesResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ReadACpoliciesResponse
(
  zclCmdASAC_ReadACpoliciesResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ReadACpoliciesResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_RemoveACresponse(zclCmdASAC_RemoveACresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a RemoveACresponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_RemoveACresponse
(
  zclCmdASAC_RemoveACresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_RemoveACresponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ACpropertiesResponse(zclCmdASAC_ACpropertiesResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ACpropertiesResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ACpropertiesResponse
(
  zclCmdASAC_ACpropertiesResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ACpropertiesResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_TSReport(zclCmdASAC_TSreport_t *pReq) 
 *
 * @brief	Sends over-the-air a TSReport command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_TSReport
(
  zclCmdASAC_TSreport_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_TSreport_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_InitiateASACresponse(zclCmdASAC_InitiateASACresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a InitiateASACresponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_InitiateASACresponse
(
  zclCmdASAC_InitiateASACresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_InitiateASACresponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ConfirmASACkeyResponse(zclCmdASAC_ConfirmASACkeyResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfirmASACkeyResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ConfirmASACkeyResponse
(
  zclCmdASAC_ConfirmASACkeyResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ConfirmASACkeyResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_GenerateAMKresponse(zclCmdASAC_GenerateAMKresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a GenerateAMKrespons command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_GenerateAMKresponse
(
  zclCmdASAC_GenerateAMKresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_GenerateAMKresponse_c,0,(zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t ASAC_LDCtransportResponse(zclCmdASAC_LDCtransportResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a LDCtransportResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_LDCtransportResponse
(
  zclCmdASAC_LDCtransportResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_LDCtransportResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_RequestConfiguration(zclCmdASAC_RequestConfiguration_t *pReq) 
 *
 * @brief	Sends over-the-air a RequestConfiguration command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_RequestConfiguration
(
  zclCmdASAC_RequestConfiguration_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_RequestConfiguration_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void LeaveTimerCallback(tmrTimerID_t timerid) 
 *
 * @brief	Leave timer callback, executes the leave commands, and set up a timer to restart ZDO.
 *
 */
void LeaveTimerCallback(tmrTimerID_t timerid) 
{

  /* Leave network, but keep the current in RAM defaults (for example, node type in a Zx Combo device) */
  ZDO_StopEx(gZdoStopMode_Announce_c | gZdoStopMode_ResetNvm_c);

  /*setup timer for rejoining, a 200 msec is added to insure that the leave command has happend*/  
  TMR_StartSingleShotTimer(timerid,((tmrTimeInMilliseconds_t)(RestartDelay) * 1000) + RestartJitter, StartTimerCallback);
}

/*!
 * @fn 		void StartTimerCallback(tmrTimerID_t timerid) 
 *
 * @brief	Start timer callback, frees timer used, and restart ZDO
 *
 */
void StartTimerCallback(tmrTimerID_t timerid) 
{
 
 
  if (ZDO_GetState() != gZdoInitialState_c)
  {
    /*
      if ZDO state machine has not yet left the network and moved to initial state then,
      check status again after 50 milliseconds
    */
    TMR_StartSingleShotTimer(timerid,(tmrTimeInMilliseconds_t)50,StartTimerCallback);
    return;
  }

  /*Free timer, as we do not need it anymore*/
  TMR_FreeTimer(timerid);

  /* Start restart the ZDO with the commisioning cluster settings */ 
  if (RestartFromAttributeSet)
  {
    ASL_SetCommissioningSAS(&gCommisioningServerAttrsData);
    /* Note whether rejoin or associate should be done is determined by the ZDO when a SAS set is used
    */
    ZDO_Start(gStartUpMode_DeviceType | gZdoStartMode_SasSet_c | gZdoStartMode_Associate_c);
  }
  else
  {
    ASL_SetCommissioningSAS(NULL);
    ZDO_Start(gStartUpMode_DeviceType | gZdoStartMode_RamSet_c | gZdoStartMode_Associate_c);
  }
}



/******************************
  Poll Control Cluster 
  See Zigbee Home Automation profile 1.2(053520r29) Section 9.5 [R1]
*******************************/
#if gZclEnablePollControlCluster_d
tmrTimerID_t gPollControl_CheckInIntervalTimer = gTmrInvalidTimerID_c; 
tmrTimerID_t gPollControl_FastPollTimeoutTimer = gTmrInvalidTimerID_c; 

zclPollControlAttrsRAM_t gPollControlAttrs = {
    gZclPollControl_CheckInInterval_Test_c,       /* checkInInterval = 360 quarterseconds */
    gZclPollControl_LongPollInterval_Test_c,      /* longPollInterval = 5 seconds */
    gZclPollControl_ShortPollInterval_Test_c,     /* shortPollInterval = (2*1/4) seconds */
    gZclPollControl_FastPollTimeout_Test_c        /* fastPollTimeout = 10 seconds */
#if gZclClusterOptionals_d 
    ,gZclPollControl_CheckInIntervalMin_Test_c,   /* checkInIntervalMin: 180 quarterseconds */
    gZclPollControl_LongPollIntervalMin_Test_c,   /* longPollIntervalMin: 12 quarterSeconds */
    gZclPollControl_FastPollTimeoutMax_Test_c,    /* fastPollTimeoutMax: 240 quarterSeconds */
#endif
};
static zclPollControlClientInf_t mCurrentPollControlClientInf = {
    FALSE,
    gZclPollControl_FastPollTimeoutClient_Test_c  //30 seconds
};

#if gEndDevCapability_d || gComboDeviceCapability_d
static uint8_t mPollControl_BindingTableEntries = 0;
static zclPollControlServerInf_t mCurrentPollControlServerInfList[gMaximumApsBindingTableEntries_c];
static uint8_t mPollControl_CurentIndex = 0;
static uint8_t mIndexCheckInRsp = 0;
#endif


/* Poll Control Cluster Attribute Definitions */
const zclAttrDef_t gaZclPollControlClusterAttrDef[] = {
  { gZclAttrIdPollControl_CheckInInterval_c,       gZclDataTypeUint32_c,      gZclAttrFlagsCommon_c,                         sizeof(uint32_t),   &gPollControlAttrs.checkInInterval},
  { gZclAttrIdPollControl_LongPollInterval_c,      gZclDataTypeUint32_c,      gZclAttrFlagsCommon_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t),   &gPollControlAttrs.longPollInterval},
  { gZclAttrIdPollControl_ShortPollInterval_c,     gZclDataTypeUint16_c,      gZclAttrFlagsCommon_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),   &gPollControlAttrs.shortPollInterval},
  { gZclAttrIdPollControl_FastPollTimeout_c,       gZclDataTypeUint16_c,      gZclAttrFlagsCommon_c,                         sizeof(uint16_t),   &gPollControlAttrs.fastPollTimeout}
#if gZclClusterOptionals_d
  ,{ gZclAttrIdPollControl_CheckInIntervalMin_c,   gZclDataTypeUint32_c,      gZclAttrFlagsCommon_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t),   &gPollControlAttrs.checkInIntervalMin},
  { gZclAttrIdPollControl_LongPollIntervalMin_c,   gZclDataTypeUint32_c,      gZclAttrFlagsCommon_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t),   &gPollControlAttrs.longPollIntervalMin},
  { gZclAttrIdPollControl_FastPollTimeoutMax_c,    gZclDataTypeUint16_c,      gZclAttrFlagsCommon_c | gZclAttrFlagsRdOnly_c,  sizeof(uint16_t),  &gPollControlAttrs.fastPollTimeoutMax}  
#endif
 };

const zclAttrDefList_t gZclPollControlClusterAttrDefList = {
  NumberOfElements(gaZclPollControlClusterAttrDef),
  gaZclPollControlClusterAttrDef
};

const zclAttrSet_t gaZclPollControlClusterAttrSet[] = {
  {gZclAttrSetPollControl_c, (void *)&gaZclPollControlClusterAttrDef, NumberOfElements(gaZclPollControlClusterAttrDef)}
};

const zclAttrSetList_t gZclPollControlClusterAttrSetList = {
  NumberOfElements(gaZclPollControlClusterAttrSet),
  gaZclPollControlClusterAttrSet
};

const zclCmd_t gaZclPollControlClusterCmdReceivedDef[]={
  // commands received 
  gZclCmdPollControl_CheckInRsp_c,
  gZclCmdPollControl_FastPollStop_c,
  gZclCmdPollControl_SetLongPollInterval_c,
  gZclCmdPollControl_SetShortPollInterval_c
};
const zclCmd_t gaZclPollControlClusterCmdGeneratedDef[]={
  // commands generated 
  gZclCmdPollControl_CheckIn_c
};

const zclCommandsDefList_t gZclPollControlClusterCommandsDefList =
{
   NumberOfElements(gaZclPollControlClusterCmdReceivedDef), gaZclPollControlClusterCmdReceivedDef,
   NumberOfElements(gaZclPollControlClusterCmdGeneratedDef), gaZclPollControlClusterCmdGeneratedDef
};

/*!
 * @fn 		zbStatus_t ZCL_PollControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Poll Control Cluster Server. 
 *
 */
zbStatus_t ZCL_PollControlClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zbStatus_t status = gZclUnsupportedClusterCommand_c;
#if gEndDevCapability_d || gComboDeviceCapability_d
 #if gComboDeviceCapability_d
  if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
 #endif
  zclFrame_t *pFrame;
  afAddrInfo_t addrInfo;

  /* to prevent compiler warning */
  (void)pDevice;
  
   status = gZclSuccessDefaultRsp_c;  
   pFrame = (void *)pIndication->pAsdu;

  /*Create the destination address*/
   AF_PrepareForReply(&addrInfo, pIndication); 
   if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
      status = gZclSuccess_c;
   
   switch (pFrame->command) 
   {
      case gZclCmdPollControl_CheckInRsp_c:
        { 
          uint8_t i;
          zclCmdPollControl_CheckInRsp_t cmdPayload = *(zclCmdPollControl_CheckInRsp_t *)(pFrame+1);
          mCurrentPollControlServerInfList[mIndexCheckInRsp].enableFastPool = cmdPayload.startFastPolling;
          mCurrentPollControlServerInfList[mIndexCheckInRsp].fastPollTimeout = OTA2Native16(cmdPayload.fastPollTimeout);
          Copy2Bytes(mCurrentPollControlServerInfList[mIndexCheckInRsp].aNwkAddr, pIndication->aSrcAddr);
          mIndexCheckInRsp++;
      
          if(mIndexCheckInRsp == mPollControl_BindingTableEntries)
          {
            mIndexCheckInRsp = 0;
            zclPollControl_StopFastPollMode(TRUE);
            for(i=0;i<mPollControl_BindingTableEntries;i++)
            {
              if(mCurrentPollControlServerInfList[i].enableFastPool == TRUE)
              {
                /* start fast polling process  */
                status = zclPollControl_StartFastPollMode(ZclPollControl_SearchMaxFastTimeout());
                break;
              }
            }
          }
          if(status == gZclSuccess_c)
          {
        	  status = (pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)? gZclSuccess_c:gZclSuccessDefaultRsp_c;
          }
          break;
        }
      case gZclCmdPollControl_FastPollStop_c:
        {
          if(IsEqual2Bytes(pIndication->aSrcAddr, mCurrentPollControlServerInfList[mPollControl_CurentIndex].aNwkAddr) == TRUE)
            zclPollControl_StopFastPollMode(FALSE);
          break;
        }
#if gZclClusterOptionals_d        
      case gZclCmdPollControl_SetLongPollInterval_c:  
        {
          uint32_t longPollInterval;
          status = gZclInvalidValue_c;
          FLib_MemCpy(&longPollInterval, (pFrame+1), sizeof(uint32_t));
          longPollInterval = OTA2Native32(longPollInterval);
          if((longPollInterval <= gZclPollControl_MaxLongPollInterval_c)
             &&(longPollInterval >= gZclPollControl_MinLongPollInterval_c))
          {
            if((longPollInterval >= OTA2Native32(gPollControlAttrs.longPollIntervalMin))&&
               (longPollInterval >= OTA2Native16(gPollControlAttrs.shortPollInterval))&& 
                 (longPollInterval <= OTA2Native32(gPollControlAttrs.checkInInterval)))
            {
              uint32_t currentPollRate = NlmeGetRequest(gNwkIndirectPollRate_c);
              
              if(currentPollRate == OTA2Native32(gPollControlAttrs.longPollInterval*1000/4))
              {
                /* device is not in the fast Poll Rate => change Poll Rate*/
                (void)ZDO_NLME_ChangePollRate((uint16_t)(gPollControlAttrs.longPollInterval*1000/4));
              }
              gPollControlAttrs.longPollInterval = Native2OTA32(longPollInterval);
              gPollControl_SetPollAttr = TRUE;
              status = (pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)? gZclSuccess_c:gZclSuccessDefaultRsp_c;
            }
          }
          break;
        }
      case gZclCmdPollControl_SetShortPollInterval_c:  
        {
          uint16_t shortPollInterval = *(uint16_t *)(pFrame+1);
          uint32_t currentPollRate = NlmeGetRequest(gNwkIndirectPollRate_c);
          
          shortPollInterval = OTA2Native16(shortPollInterval);

          if((shortPollInterval >= gZclPollControl_MinShortPollInterval_c)&&
             (shortPollInterval <= OTA2Native32(gPollControlAttrs.longPollInterval))&&
               (shortPollInterval <= OTA2Native32(gPollControlAttrs.checkInInterval)))
            
          {
            if(currentPollRate == OTA2Native16(gPollControlAttrs.shortPollInterval))
              ZDO_NLME_ChangePollRate((uint16_t)(OTA2Native16(gPollControlAttrs.shortPollInterval)*1000/4));
            gPollControlAttrs.shortPollInterval = Native2OTA16(shortPollInterval);
            gPollControl_SetPollAttr = TRUE;
          }
          else
            status = gZclInvalidValue_c;
          break;
        }
#endif        
      default:
        return  gZclUnsupportedClusterCommand_c;                     
  }
 #if gComboDeviceCapability_d
  }
 #endif /* gComboDeviceCapability_d */
#endif
   return status;
}

/*!
 * @fn 		zbStatus_t ZCL_PollControlClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Poll Control Cluster Client. 
 *
 */
zbStatus_t ZCL_PollControlClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  afAddrInfo_t addrInfo;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  /* to prevent compiler warning */
  (void)pDevice;
  
   pFrame = (void *)pIndication->pAsdu;
  
   /*Create the destination address*/
   AF_PrepareForReply(&addrInfo, pIndication); 
   if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
         status = gZclSuccess_c;
   switch (pFrame->command) 
   {
      case gZclCmdPollControl_CheckIn_c:  
        {
          /* to include the case when the client is not able to send a CheckInRsp Cmd*/
          if(mCurrentPollControlClientInf.enableFastPooling != 0xFF)
          {
            zclPollControl_CheckInRsp_t commandRsp;
            FLib_MemCpy(&commandRsp.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
            commandRsp.zclTransactionId = pFrame->transactionId;
            commandRsp.cmdFrame.startFastPolling = mCurrentPollControlClientInf.enableFastPooling;
            commandRsp.cmdFrame.fastPollTimeout = mCurrentPollControlClientInf.fastPollTimeout;
            commandRsp.addrInfo.radiusCounter = afDefaultRadius_c;
            return zclPollControl_CheckInRsp(&commandRsp);
          }
          break;
        }
      default:
        return  gZclUnsupportedClusterCommand_c;                     
  }
  return status;
}


/*!
 * @fn 		zbStatus_t zclPollControl_CheckIn(zclPollControl_CheckIn_t *pReq) 
 *
 * @brief	Sends over-the-air a CheckIn command from the PollControl Cluster Server. 
 *
 */
zbStatus_t zclPollControl_CheckIn
( 
    zclPollControl_CheckIn_t *pReq
) 
{
  
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPollControl_c);   
    return ZCL_SendServerReqSeqPassed(gZclCmdPollControl_CheckIn_c, 0, (zclGenericReq_t *)pReq);
  
}

/*!
 * @fn 		zbStatus_t zclPollControl_CheckInRsp(zclPollControl_CheckInRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a CheckInRsp command from the PollControl Cluster Client. 
 *
 */
zbStatus_t zclPollControl_CheckInRsp
( 
    zclPollControl_CheckInRsp_t *pReq
) 
{
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPollControl_c); 
    return ZCL_SendClientReqSeqPassed(gZclCmdPollControl_CheckInRsp_c, sizeof(zclCmdPollControl_CheckInRsp_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclPollControl_FastPollStop(zclPollControl_FastPollStop_t *pReq) 
 *
 * @brief	Sends over-the-air a FastPollStop command from the PollControl Cluster Client. 
 *
 */
zbStatus_t zclPollControl_FastPollStop
( 
    zclPollControl_FastPollStop_t *pReq
) 
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPollControl_c);  
    return ZCL_SendClientReqSeqPassed(gZclCmdPollControl_FastPollStop_c, 0, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclPollControl_SetLongPollInterval(zclPollControl_SetLongPollInterval_t *pReq) 
 *
 * @brief	Sends over-the-air a SetLongPollInterval command from the PollControl Cluster Client. 
 *
 */
zbStatus_t zclPollControl_SetLongPollInterval
( 
    zclPollControl_SetLongPollInterval_t *pReq
) 
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPollControl_c);  
    return ZCL_SendClientReqSeqPassed(gZclCmdPollControl_SetLongPollInterval_c, sizeof(zclCmdPollControl_SetLongPollInterval_t), (zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t zclPollControl_SetShortPollInterval(zclPollControl_SetShortPollInterval_t *pReq) 
 *
 * @brief	Sends over-the-air a SetShortPollInterval command from the PollControl Cluster Client. 
 *
 */
zbStatus_t zclPollControl_SetShortPollInterval
( 
    zclPollControl_SetShortPollInterval_t *pReq
) 
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPollControl_c);  
    return ZCL_SendClientReqSeqPassed(gZclCmdPollControl_SetShortPollInterval_c, sizeof(zclCmdPollControl_SetShortPollInterval_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclPollControl_ClusterInit(void)
 *
 * @brief	Init Poll Control cluster functionality 
 *
 */
zbStatus_t zclPollControl_ClusterInit()
{
    zbStatus_t status = gZclSuccess_c;
#if gEndDevCapability_d || gComboDeviceCapability_d  
    if(gPollControl_CheckInIntervalTimer == gTmrInvalidTimerID_c)
    {
      gPollControl_CheckInIntervalTimer = TMR_AllocateTimer();
   
      if(gPollControl_CheckInIntervalTimer == gTmrInvalidTimerID_c)
    	  return gZclNoMem_c;	
    
      (void)ZDO_NLME_ChangePollRate((uint16_t)(Native2OTA32(gPollControlAttrs.longPollInterval)*1000/4));
      if(gPollControlAttrs.checkInInterval != 0x00)
      {       
    	  /* checkInInterval - measured in quarter seconds */
    	  TMR_StartTimer(gPollControl_CheckInIntervalTimer, gTmrSingleShotTimer_c, Native2OTA32(gPollControlAttrs.checkInInterval)*(1000/4), ZclPollControl_CheckInCallback);
      }
    }
#endif    
    return status;
}

#if gEndDevCapability_d || gComboDeviceCapability_d 
/*!
 * @fn 		void ZclPollControl_CheckInCallback(uint8_t tmrId)
 *
 * @brief	Callback used to send checkIn command, based on CheckInInterval Attribute.
 *
 */
void ZclPollControl_CheckInCallback(uint8_t tmrId)
{
    zclPollControl_CheckIn_t cmd;
    afAddrInfo_t addrInfo = {gZbAddrModeIndirect_c, {0x00, 0x00}, 0x08, {gaZclClusterPollControl_c}, 0x08, gApsTxOptionNone_c, 1};  
    bool_t sendCheckInCmd = FALSE;
    uint8_t i = 0, j = 0;
   
    
    addrInfo.radiusCounter = afDefaultRadius_c;
    mPollControl_BindingTableEntries = 0;
    
    FLib_MemCpy(&cmd.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
    
    if(gZclCommonAttr.basicDeviceEnabled)
    {
      for(i=0; i< gMaximumApsBindingTableEntries_c; i++)
      {
    	  for(j= 0; j< gaApsBindingTable[i].iClusterCount; j++)
    		  if(IsEqual2BytesInt(gaApsBindingTable[i].aClusterList[j], gZclClusterPollControl_c)== TRUE)
    		  {	
    			  sendCheckInCmd = TRUE;
    			  cmd.addrInfo.dstEndPoint = gaApsBindingTable[i].dstEndPoint;
    			  cmd.addrInfo.srcEndPoint = gaApsBindingTable[i].srcEndPoint;
    			  mPollControl_BindingTableEntries++;
    			  break;
    		  }
    	  
    	  mCurrentPollControlServerInfList[i].fastPollTimeout = 0x00;
    	  mCurrentPollControlServerInfList[i].enableFastPool = 0x00;
      }
    }
    if(sendCheckInCmd)   
    {
       /* send CheckIn command */
       /* start fast poll mode until the device receive all CheckInRsp */
       (void)zclPollControl_StartFastPollMode((uint16_t)(gZclPollControl_CheckInRspMaxWaitDuraration_c*4));
       (void)zclPollControl_CheckIn(&cmd);
       
    }
    TMR_StartTimer(gPollControl_CheckInIntervalTimer, gTmrSingleShotTimer_c, Native2OTA32(gPollControlAttrs.checkInInterval)*1000/4, ZclPollControl_CheckInCallback);
    (void)tmrId;
}

/*!
 * @fn 		zbStatus_t zclPollControl_StartFastPollMode(uint16_t timeout)
 *
 * @brief	Start fast poll mode
 *
 */
zbStatus_t zclPollControl_StartFastPollMode(uint16_t timeout)
{
  uint32_t currentTimeOut = 0;
  uint16_t BindingPollTimeout = Native2OTA16(gPollControlAttrs.shortPollInterval)*1000/4; // seconds 
  
  currentTimeOut = (uint32_t)((timeout == 0)? OTA2Native16(gPollControlAttrs.fastPollTimeout):timeout);
  
#if gZclClusterOptionals_d  
  if(currentTimeOut >  OTA2Native16(gPollControlAttrs.fastPollTimeoutMax))
	  return gZclInvalidField_c;
#endif
  
  gPollControl_FastPollTimeoutTimer = TMR_AllocateTimer();
  if(gPollControl_FastPollTimeoutTimer == gTmrInvalidTimerID_c)
      return gZclNoMem_c;	
  /* checkInInterval - measured in quarter seconds */
  TMR_StartTimer(gPollControl_FastPollTimeoutTimer, gTmrSingleShotTimer_c, (currentTimeOut*1000)/4, ZclPollControl_FastPollModeCallback);
  (void)ZDO_NLME_ChangePollRate((uint16_t)BindingPollTimeout);
  return gZclSuccess_c;
}

/*!
 * @fn 		void ZclPollControl_FastPollModeCallback(uint8_t tmrId)
 *
 * @brief	Callback used to stop fast poll mode
 *
 */
void ZclPollControl_FastPollModeCallback(uint8_t tmrId)
{
  (void)tmrId;
  mIndexCheckInRsp = 0;
  zclPollControl_StopFastPollMode(TRUE);
}

/*!
 * @fn 		static uint16_t ZclPollControl_SearchMaxFastTimeout(void)
 *
 * @brief	Get maximum fast timeout interval
 *
 */
static uint16_t ZclPollControl_SearchMaxFastTimeout(void)
{
  uint16_t maxFastPollTimeout = mCurrentPollControlServerInfList[0].fastPollTimeout;
  uint8_t i;
  mPollControl_CurentIndex = 0;

  for(i = 1; i< mPollControl_BindingTableEntries;i++)
  {
    if(mCurrentPollControlServerInfList[i].enableFastPool == FALSE)
      continue;
    if(maxFastPollTimeout < mCurrentPollControlServerInfList[i].fastPollTimeout)
    {
      maxFastPollTimeout = mCurrentPollControlServerInfList[i].fastPollTimeout;
      mPollControl_CurentIndex = i;
    }
  }
  return maxFastPollTimeout;
}

/*!
 * @fn 		void zclPollControl_StopFastPollMode(bool_t timeoutStop)
 *
 * @brief	Stop Fast Poll mode function
 *
 */
void zclPollControl_StopFastPollMode(bool_t timeoutStop)
{
  uint8_t i = 0;
  uint32_t remainingTime = 0, maxRemainingTimeout = 0, remainingTimeout = 0;
  uint8_t index = 0;
  
  TMR_StopTimer(gPollControl_FastPollTimeoutTimer);

  if(!timeoutStop)
  {
    /* check that every start request from paired device has been stopped */
    for(i=0; i< gMaximumApsBindingTableEntries_c; i++)
    {
      if((mCurrentPollControlServerInfList[i].enableFastPool == FALSE)||(i == mPollControl_CurentIndex))
        continue;
      /* get remaining Time - miliseconds */
      remainingTime = TMR_GetRemainingTime(gPollControl_FastPollTimeoutTimer); 
      if(mCurrentPollControlServerInfList[i].fastPollTimeout*1000/4 > 
         mCurrentPollControlServerInfList[mPollControl_CurentIndex].fastPollTimeout*1000/4 - remainingTime)
      {
          remainingTimeout = (mCurrentPollControlServerInfList[i].fastPollTimeout*1000/4 - \
            (mCurrentPollControlServerInfList[mPollControl_CurentIndex].fastPollTimeout*1000/4 - remainingTime));
          if(remainingTimeout >= maxRemainingTimeout)
          {
            maxRemainingTimeout = remainingTimeout;
            index = i;
          }
      }
    }    
  }
  
  if(maxRemainingTimeout)
  {
    mPollControl_CurentIndex = index;
    TMR_StartTimer(gPollControl_FastPollTimeoutTimer, gTmrSingleShotTimer_c, maxRemainingTimeout, ZclPollControl_FastPollModeCallback);
    return;
  }
  TMR_FreeTimer(gPollControl_FastPollTimeoutTimer);
  /* restore to the last poll interval -- LongPollInterval */
  (void)ZDO_NLME_ChangePollRate((uint16_t)(Native2OTA32(gPollControlAttrs.longPollInterval)*(1000/4)));
}
#endif

/*!
 * @fn 		void ZclPollControl_WriteAttribute(void *pData)
 *
 * @brief	Poll control write attributes function
 *
 */
void ZclPollControl_WriteAttribute(void *pData)
{  
  zclCmdWriteAttrRecord_t *pRecord = (zclCmdWriteAttrRecord_t*) pData;  
    
  switch (pRecord->attrId)
  {        
    case gZclAttrPollControl_CheckInInterval_c:
      {
         uint32_t attrData;
         
         FLib_MemCpy(&attrData, pRecord->aData, sizeof(attrData));
         attrData = OTA2Native32(attrData);
         if(attrData > 0x00)
         { 
             if(attrData < OTA2Native32(gPollControlAttrs.longPollInterval))    
                return;
#if gZclClusterOptionals_d  
             if (attrData < OTA2Native32(gPollControlAttrs.checkInIntervalMin))
            	return;
#endif  			
            	
                    
#if gEndDevCapability_d || gComboDeviceCapability_d  
        	TMR_StartTimer(gPollControl_CheckInIntervalTimer, gTmrSingleShotTimer_c, (attrData*1000)/4, ZclPollControl_CheckInCallback);
#endif
         }
#if gEndDevCapability_d || gComboDeviceCapability_d          
         else
           TMR_StopTimer(gPollControl_CheckInIntervalTimer);
#endif         
         gPollControlAttrs.checkInInterval = Native2OTA32(attrData);
         break;
      }
    case gZclAttrPollControl_FastPollTimeout_c:
      {
         uint16_t attrData;
#if gZclClusterOptionals_d          
         uint16_t fastPollTimeoutMax = OTA2Native16(gPollControlAttrs.fastPollTimeoutMax);
#endif         
         
         FLib_MemCpy(&attrData, &pRecord->aData, sizeof(uint16_t));
         attrData = OTA2Native16(attrData);
         gPollControlAttrs.fastPollTimeout = Native2OTA16(attrData);
         if((attrData < gZclPollControl_MinShortPollInterval_c)
#if gZclClusterOptionals_d  
           ||((attrData >fastPollTimeoutMax)&&(fastPollTimeoutMax > 0))
#endif            
           )
         {
#if gZclClusterOptionals_d            
           gPollControlAttrs.fastPollTimeout = gPollControlAttrs.fastPollTimeoutMax; 
#else              
           gPollControlAttrs.fastPollTimeout =  gZclPollControl_MinShortPollInterval_c + 10;
#endif            
         }
         break;
      }
    default:
       break;
  }
} 

/*!
 * @fn 		bool_t  ZclPollControlValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
 *
 * @brief	Poll control validate attributes function
 *
 */
bool_t  ZclPollControlValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
{  
  zclCmdWriteAttrRecord_t *pRecord = (zclCmdWriteAttrRecord_t*) pData;  
 (void) endPoint;
 (void) clusterId;
    
  switch (pRecord->attrId)
  {        
  	case gZclAttrPollControl_CheckInInterval_c:
      {
        uint32_t attrData;
          
        FLib_MemCpy(&attrData, pRecord->aData, sizeof(attrData));    
        attrData = OTA2Native32(attrData);
        if(attrData < OTA2Native32(gPollControlAttrs.longPollInterval) && attrData!=0x00) 	
           return FALSE; 
#if gZclClusterOptionals_d  
        if(attrData < OTA2Native32(gPollControlAttrs.checkInIntervalMin) && attrData!=0x00) 
           return FALSE; 
#endif   
         			
      }
  default:
    return TRUE;
  }
} 

/*!
 * @fn 		zbStatus_t  ztcPollControl_SetClientInf(zclPollControlClientInf_t * setClientInf)
 *
 * @brief	Set client poll control information
 *
 */
zbStatus_t ztcPollControl_SetClientInf 
(
    zclPollControlClientInf_t * setClientInf
)
{
  mCurrentPollControlClientInf.enableFastPooling = setClientInf->enableFastPooling;
  mCurrentPollControlClientInf.fastPollTimeout  = setClientInf->fastPollTimeout;
  return gZclSuccess_c;
}



#endif /* gZclEnablePollControlCluster_d */
/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/*!
 * @fn 		zbStatus_t ZCL_ReplyNoCopy(zbApsdeDataIndication_t *pIndication, uint8_t payloadLen, afToApsdeMessage_t *pMsg )
 *
 * @brief	Standard request used for longer replies that must be built in-place.
 *
 */
zbStatus_t ZCL_ReplyNoCopy
  (
  zbApsdeDataIndication_t *pIndication, /* IN: indication that came from other node */
  uint8_t payloadLen,       /* IN: payload length, not counting ZCL frame (3-byte hdr)  */
  afToApsdeMessage_t *pMsg  /* IN: message, with payload at gAsduOffset_c + sizeof(zclFrame_t) */
  )
{
  afAddrInfo_t addrInfo;
  uint8_t frameControl;

  /* get address ready for reply */
  AF_PrepareForReply(&addrInfo, pIndication);
  addrInfo.radiusCounter = gDefaultRadiusCounter;
  
  frameControl = ((zclFrame_t *)pIndication->pAsdu)->frameControl;
  
  /* setup the reply frame */
  ZCL_SetupFrame((void *)(((uint8_t *)pMsg) + gAsduOffset_c), (void *)pIndication->pAsdu);
  
  if (frameControl & gZclFrameControl_MfgSpecific)
    payloadLen += sizeof(zclMfgFrame_t);
  else
    payloadLen += sizeof(zclFrame_t);

  /* send it over the air */
  return ZCL_DataRequestNoCopy(&addrInfo, payloadLen, pMsg);
}

/*!
 * @fn 		zbStatus_t ZCL_Reply(zbApsdeDataIndication_t *pIndication, uint8_t payloadLen,   void *pPayload  )
 *
 * @brief	Standard request used for short replies (that do not need to be built in-place)
 *
 */
zbStatus_t ZCL_Reply
  (
  zbApsdeDataIndication_t *pIndication,   /* IN: */
  uint8_t payloadLen,                     /* IN: */
  void *pPayload                          /* IN: */
  )
{
  afToApsdeMessage_t *pMsg;

  /* allocate space for the message */
  pMsg = AF_MsgAlloc();
  if(!pMsg)
    return gZclNoMem_c;

  /* copy in payload */
  FLib_MemCpy(((uint8_t *)pMsg) + gAsduOffset_c + sizeof(zclFrame_t),
    pPayload, payloadLen);

  /* send it over the air */
  return ZCL_ReplyNoCopy(pIndication, payloadLen, pMsg);
}

/*!
 * @fn 		uint8_t *ZCL_InterPanCreatePayload(zclCmd_t command,zclFrameControl_t frameControl, uint8_t *pPayloadLen,  uint8_t *pPayload)
 *
 * @brief	Inter Pan Create Payload function
 *
 */
uint8_t *ZCL_InterPanCreatePayload
  (
  zclCmd_t command,               /* IN: command */
  zclFrameControl_t frameControl, /* IN: frame control field */
  uint8_t *pPayloadLen,           /* IN/OUT: length of payload (then adjusted to length of asdu) */
  uint8_t *pPayload                  /* IN: payload after frame (first byte from pPayload is transactionID)*/
  )
{
  uint8_t *pMsg;
  zclFrame_t *pFrame;
  *pPayloadLen += sizeof(zclFrame_t);
  /* allocate space for the message */
  pMsg = MSG_Alloc(*pPayloadLen);
  if(!pMsg)
    return NULL;
  BeeUtilZeroMemory(pMsg, *pPayloadLen);
  /* set up the frame */
  pFrame = (zclFrame_t *)pMsg;
  pFrame->frameControl = frameControl;
  pFrame->transactionId = pPayload[0];
  pFrame->command = command;
  if(pPayload && (*pPayloadLen))
    /* copy the payload, skip over first byte which is the zcl transaction ID*/
    FLib_MemCpy((pFrame + 1), (pPayload+1), (*pPayloadLen - sizeof(zclFrame_t)));
  /* return ptr to msg buffer */
  return pMsg;
}

/*!
 * @fn 		zclStatus_t ZCL_GenericReq(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
 *
 * @brief	This request works for many of the request types that don't need to parse the data before sending.
 *			Returns the gHaNoMem_c if not enough memory to send.
 *
 */
zclStatus_t ZCL_GenericReq(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
{
  afToApsdeMessage_t *pMsg;

  /* create a ZCL frame and copy in the payload */
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),command,gZclFrameControl_FrameTypeSpecific,
    NULL, &iPayloadLen,&(pReq->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}

/*!
 * @fn 		zclStatus_t ZCL_SendClientReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
 *
 * @brief	Send Req from Client to Server with Zcl Transaction Sequence Id passed as parameter
 *
 */
zclStatus_t ZCL_SendClientReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
{
  afToApsdeMessage_t *pMsg;

  /* create a ZCL frame and copy in the payload */
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo), command, gZclFrameControl_FrameTypeSpecific,
         &(pReq->cmdFrame),&iPayloadLen, ((uint8_t*)&(pReq->cmdFrame)+1));
  if(!pMsg)
    return gZclNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}

/*!
 * @fn 		zclStatus_t ZCL_SendInterPanClientReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void *pReq)
 *
 * @brief	Send InterPan Req from Client to Server with Zcl Transaction Sequence Id passed as parameter 
 *
 */
zclStatus_t ZCL_SendInterPanClientReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void *pReq)
{
 
  uint8_t *pPayload;
  zclStatus_t status;
  
  /* create a ZCL frame */
  pPayload = ZCL_InterPanCreatePayload(command, gZclFrameControl_FrameTypeSpecific ,
                                       &iPayloadLen, ((uint8_t *)pReq + sizeof(InterPanAddrInfo_t)) );
  if(!pPayload)
    return gZclNoMem_c;
  
  /* send packet over the air */
  status = AF_InterPanDataRequest((InterPanAddrInfo_t *)pReq, iPayloadLen, pPayload, NULL);
  
  /* Free the buffer allocated for the payload*/
  if (pPayload)
    MSG_Free(pPayload);
    
  return status;
}
/*!
 * @fn 		zclStatus_t ZCL_SendServerReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
 *
 * @brief	Send Req From Server to Client with Zcl Transaction Sequence Id passed as parameter
 *
 */
zclStatus_t ZCL_SendServerReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
{
  afToApsdeMessage_t *pMsg;

  /* create a ZCL frame and copy in the payload */
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),command, (gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp),
          &(pReq->cmdFrame), &iPayloadLen, ((uint8_t*)&(pReq->cmdFrame)+1));
  if(!pMsg)
    return gZclNoMem_c;
  
  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}
/*!
 * @fn 		zclStatus_t ZCL_SendInterPanServerReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void *pReq)
 *
 * @brief	Send InterPan Req from Server to Client with Zcl Transaction Sequence Id passed as parameter
 *
 */
zclStatus_t ZCL_SendInterPanServerReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void *pReq)
{
 
  uint8_t *pPayload;
  zclStatus_t status;
  
  /* create a ZCL frame */
  pPayload = ZCL_InterPanCreatePayload(command, (gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp) ,
                                       &iPayloadLen, ((uint8_t *)pReq + sizeof(InterPanAddrInfo_t)) );
  if(!pPayload)
    return gZclNoMem_c;
  /* send packet over the air */
  status = AF_InterPanDataRequest((InterPanAddrInfo_t *)pReq, iPayloadLen, pPayload, NULL);
  
  /* Free the buffer allocated for the payload*/
  if (pPayload)
    MSG_Free(pPayload);
    
  return status;
    
}
/*!
 * @fn 		zclStatus_t ZCL_SendServerRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
 *
 * @brief	Send Rsp From Server To Client with Zcl Transaction Sequence Id passed as parameter
 *
 */
zclStatus_t ZCL_SendServerRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
{
  afToApsdeMessage_t *pMsg;

  /* create a ZCL frame and copy in the payload */
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),command, (gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp |gZclFrameControl_DisableDefaultRsp) ,
    &(pReq->cmdFrame), &iPayloadLen, ((uint8_t*)&(pReq->cmdFrame)+1));
  if(!pMsg)
    return gZclNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}

/*!
 * @fn 		zclStatus_t ZCL_SendInterPanServerRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void*pReq)
 *
 * @brief	Send InterPan Rsp From Server To Client with Zcl Transaction Sequence Id passed as parameter 
 *
 */
zclStatus_t ZCL_SendInterPanServerRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void*pReq)
{
  uint8_t *pPayload;
  zclStatus_t status;
  
  /* create a ZCL frame */
  pPayload = ZCL_InterPanCreatePayload(command, (gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp |gZclFrameControl_DisableDefaultRsp) ,
        &iPayloadLen, ((uint8_t *)pReq + sizeof(InterPanAddrInfo_t)) );
  if(!pPayload)
    return gZclNoMem_c;
  /* send packet over the air */
  status = AF_InterPanDataRequest((InterPanAddrInfo_t *)pReq, iPayloadLen, pPayload, NULL);
  
  /* Free the buffer allocated for the payload*/
  if (pPayload)
    MSG_Free(pPayload);
    
  return status;
}

/*!
 * @fn 		zclStatus_t ZCL_SendClientRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
 *
 * @brief	Send Rsp From Client to Server with Zcl Transaction Sequence Id passed as parameter 
 *
 */
zclStatus_t ZCL_SendClientRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
{
  afToApsdeMessage_t *pMsg;
  
  /* create a ZCL frame and copy in the payload */
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo), command,(gZclFrameControl_FrameTypeSpecific |gZclFrameControl_DisableDefaultRsp),
          &(pReq->cmdFrame), &iPayloadLen, ((uint8_t*)&(pReq->cmdFrame)+1));
  if(!pMsg)
    return gZclNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}

/*!
 * @fn 		zclStatus_t ZCL_SendInterPanClientRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void*pReq)
 *
 * @brief	Send InterPan Rsp From Client To Server with Zcl Transaction Sequence Id passed as paramete
 *
 */
zclStatus_t ZCL_SendInterPanClientRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void*pReq)
{
  uint8_t *pPayload;
  zclStatus_t status;
  
  /* create a ZCL frame */
  pPayload = ZCL_InterPanCreatePayload(command, (gZclFrameControl_FrameTypeSpecific |gZclFrameControl_DisableDefaultRsp) ,
        &iPayloadLen, ((uint8_t *)pReq + sizeof(InterPanAddrInfo_t)) );
  if(!pPayload)
    return gZclNoMem_c;
  /* send packet over the air */
  status = AF_InterPanDataRequest((InterPanAddrInfo_t *)pReq, iPayloadLen, pPayload, NULL);
  
  /* Free the buffer allocated for the payload*/
  if (pPayload)
    MSG_Free(pPayload);
    
  return status;
}


/*****************************************************************************/
#if gZclClusterOptionals_d
/*!
 * @fn 		zbStatus_t ZCL_SetValueAttr( zclSetAttrValue_t *pSetAttrValueReq)
 *
 * @brief	Local ZTC function used to set the value for some attributes.
 *
 */
zbStatus_t ZCL_SetValueAttr 
  (
  zclSetAttrValue_t *pSetAttrValueReq
  )
{
  afDeviceDef_t  *pDeviceDef;
  afClusterDef_t *pClusterDef;
  zclAttrDef_t   *pAttrDef;
  zbClusterId_t  aClusterId;
  
#if gZclEnableThermostat_c || (gASL_ZclMet_Optionals_d && gZclEnableMeteringServer_d)
  uint8_t alarmCode = 0;
#endif

#if gZclEnableThermostat_c
  uint8_t alarmMask = 0;
  uint8_t valueAttr = 0;
#endif  
  
#if (gASL_ZclMet_Optionals_d && gZclEnableMeteringServer_d)
  uint16_t alarmMask = 0;
  uint16_t valueAttr = 0;
#endif  
  
  
#if gZclEnableOccupancySensor_c    
  uint8_t sensorType, thresholdOccupancy, occupancyState;
  uint16_t delayOccupancy;
#endif  
  
  /* does the endpoint exist? (and have a device definition?) */
  pDeviceDef = AF_GetEndPointDevice(pSetAttrValueReq->ep);
  if(!pDeviceDef)
    return gZclUnsupportedAttribute_c;

  /* does the cluster exist on this endpoint? */
  pClusterDef = ZCL_FindCluster(pDeviceDef, pSetAttrValueReq->clusterId);
  if(!pClusterDef)
    return gZclUnsupportedAttribute_c;

  /* does the attribute exist? */
  pAttrDef = ZCL_FindAttr(pClusterDef, pSetAttrValueReq->attrID, pSetAttrValueReq->direction);
  if(!pAttrDef)
   return gZclUnsupportedAttribute_c;
  
#if gZclEnableBinaryInput_c
 /*used only for Binary input Cluster*/ 
   Set2Bytes(aClusterId, gZclClusterBinaryInput_c);
   if(FLib_Cmp2Bytes(pSetAttrValueReq->clusterId,aClusterId) == TRUE){
      if((pSetAttrValueReq->attrID == gZclAttrBinaryInputStatusFlags_c)||(pSetAttrValueReq->attrID == gZclAttrBinaryInputApplicationType_c))
        (void)ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, gZclServerAttr_c, &(pSetAttrValueReq->valueAttr)); 
      BeeAppUpdateDevice(pSetAttrValueReq->ep, gZclUI_ChangeNotification, 0, aClusterId, NULL);
      return gZbSuccess_c;
  } 
#endif
   
#if gASL_ZclIASZoneReq_d
  Set2Bytes(aClusterId, gZclClusterIASZone_c);
  if(FLib_Cmp2Bytes(pSetAttrValueReq->clusterId,aClusterId) == TRUE){
      if(pSetAttrValueReq->attrID == gZclAttrZoneInformationZoneStatus_c)
      {
        (void)ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrZoneInformationZoneStatus_c, gZclServerAttr_c, &(pSetAttrValueReq->valueAttr));
        BeeAppUpdateDevice(pSetAttrValueReq->ep, gZclUI_ChangeNotification, 0, 0, NULL);
      }
       return gZbSuccess_c;
  }
#endif

#if gZclEnableOccupancySensor_c  
  Set2Bytes(aClusterId, gZclClusterOccupancy_c);
  if(FLib_Cmp2Bytes(pSetAttrValueReq->clusterId,aClusterId) == TRUE){
    if(pSetAttrValueReq->attrID == gZclAttrOccupancySensing_OccupancyId_c){  /* attribute = occupancy */
      (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_OccupancySensorTypeId_c, gZclServerAttr_c, &sensorType, NULL);
      (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_OccupancyId_c, gZclServerAttr_c, &occupancyState, NULL);
      if(pSetAttrValueReq->valueAttr[0] != occupancyState) //(size for occupancyState attribute = 1 byte)
      {
        gSetAvailableOccupancy = FALSE;
        occupancyTimer = TMR_AllocateTimer();
        if(sensorType == gZclTypeofSensor_PIR_c)
        {        
          (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedThresholdId_c, gZclServerAttr_c, &thresholdOccupancy, NULL);
          if(occupancyState == 1) /*occupied*/
            (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_PIROccupiedToUnoccupiedDelayId_c, gZclServerAttr_c, &delayOccupancy, NULL);
          else /*unoccupied*/
            (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedDelayId_c, gZclServerAttr_c, &delayOccupancy, NULL);      
        }
        else
          if(sensorType == gZclTypeofSensor_Ultrasonic_c)
          {
            (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedThresholdId_c, gZclServerAttr_c, &thresholdOccupancy, NULL);
             if(occupancyState == 1) /*occupied*/
              (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_UltrasonicOccupiedToUnoccupiedDelayId_c, gZclServerAttr_c, &delayOccupancy, NULL);
            else /*unoccupied*/
              (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedDelayId_c, gZclServerAttr_c, &delayOccupancy, NULL);      
        }
	delayOccupancy = OTA2Native16(delayOccupancy);
        TMR_StartSecondTimer( pSetAttrValueReq->ep, delayOccupancy, TmrOccupancyCallBack);       
      }
       return gZbSuccess_c;
    }
    else
      if(pSetAttrValueReq->attrID == gZclAttrOccupancySensing_OccupancySensorTypeId_c) 
      (void)ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, gZclServerAttr_c, &(pSetAttrValueReq->valueAttr));
     return gZbSuccess_c;
  }
#endif
  
#if gZclEnableThermostat_c || (gASL_ZclMet_Optionals_d && gZclEnableMeteringServer_d)  
#if gZclEnableThermostat_c  
  Set2Bytes(aClusterId, gZclClusterThermostat_c);
#else
  Set2Bytes(aClusterId, gZclClusterSmplMet_c); 
#endif
  if(FLib_Cmp2Bytes(pSetAttrValueReq->clusterId,aClusterId) == TRUE){
#if gZclEnableThermostat_c      
    if(pSetAttrValueReq->attrID == gZclAttrThermostat_AlarmMaskId_c){
      valueAttr = pSetAttrValueReq->valueAttr[0];
#else
    if(pSetAttrValueReq->attrID == gZclAttrMetASGenericAlarmMask_c){  
      FLib_MemCpy(&valueAttr, &pSetAttrValueReq->valueAttr, pSetAttrValueReq->attrSize);
#endif          
      (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, gZclServerAttr_c, &alarmMask, NULL);   
      //if it is the same value - can't detect the correct code alarm 
      if((valueAttr != 0)&&(valueAttr != alarmMask))
      {
          alarmCode = (uint8_t)(valueAttr & (~alarmMask)); //sizeof(alarm code) = 1Byte
          if(alarmCode != 0x00)
          {
            alarmCode--;
            BeeAppUpdateDevice(pSetAttrValueReq->ep, gZclUI_AlarmGenerate_c, 0, aClusterId, &alarmCode);
          }
      }
      return ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, gZclServerAttr_c, &valueAttr);      
     }
  } 
#endif
#if gZclEnablePollControlCluster_d
    Set2Bytes(aClusterId, gZclClusterPollControl_c);
    if(FLib_Cmp2Bytes(pSetAttrValueReq->clusterId,aClusterId) == TRUE){
        if(pSetAttrValueReq->attrID == gZclAttrPollControl_LongPollInterval_c)
        {
          FLib_MemCpy(&gPollControlAttrs.longPollInterval, &pSetAttrValueReq->valueAttr, sizeof(uint32_t));
          gPollControl_SetPollAttr = TRUE;
          (void)ZDO_NLME_ChangePollRate((uint16_t)(Native2OTA32(gPollControlAttrs.longPollInterval)*1000/4));
          return gZbSuccess_c;
         
        }
    }   
    
#endif    
    
   Copy2Bytes(aClusterId, pSetAttrValueReq->clusterId);
   return ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, pSetAttrValueReq->direction, &(pSetAttrValueReq->valueAttr));    
}

/*!
 * @fn 		zbStatus_t ZCL_ProcessUnsolicitedCommand(zclProcessUnsolicitedCommand_t *pProcessUnsolicitedCommand)  
 *
 * @brief	Local ZTC function that manage some unsolicited commands.
 *
 */
zbStatus_t ZCL_ProcessUnsolicitedCommand(zclProcessUnsolicitedCommand_t *pProcessUnsolicitedCommand)  
{
  afDeviceDef_t  *pDeviceDef;
  afClusterDef_t *pClusterDef;
  zbClusterId_t  aClusterId;
  
  /* does the endpoint exist? (and have a device definition?) */
  pDeviceDef = AF_GetEndPointDevice(pProcessUnsolicitedCommand->endpoint);
  if(!pDeviceDef)
    return gZclNotFound_c;

  /* does the cluster exist on this endpoint? */
  pClusterDef = ZCL_FindCluster(pDeviceDef, pProcessUnsolicitedCommand->clusterId);
  if(!pClusterDef)
    return gZclNotFound_c;
  
#if gZclEnableApplianceStatistics_d  && gZclEnableApplianceStatisticsServerOptionals_d
  Set2Bytes(aClusterId, gZclClusterApplianceStatistics_c);
  if(FLib_Cmp2Bytes(pProcessUnsolicitedCommand->clusterId, aClusterId) == TRUE)
  {
    return Zcl_ApplianceStatistics_UnsolicitedCommandHandler(pProcessUnsolicitedCommand->commandId, pProcessUnsolicitedCommand->data);
  }
#endif  
  
#if gZclEnableApplianceEventsAlerts_d &&  gZclEnableApplianceEventsAlertsUnsolicitedCmd_d
  Set2Bytes(aClusterId, gZclClusterApplianceEventsAlerts_c);
  if(FLib_Cmp2Bytes(pProcessUnsolicitedCommand->clusterId, aClusterId) == TRUE)
  {
    return Zcl_ApplianceEventsAlerts_UnsolicitedCommandHandler( pProcessUnsolicitedCommand->commandId, 
                 pProcessUnsolicitedCommand->state,  pProcessUnsolicitedCommand->data);
  }
#endif  
  
#if gZclEnablePwrProfileClusterServer_d  
  Set2Bytes(aClusterId, gZclClusterPowerProfile_c);
  if(FLib_Cmp2Bytes(pProcessUnsolicitedCommand->clusterId, aClusterId) == TRUE)
  {
    return Zcl_PowerProfile_UnsolicitedCommandHandler(pProcessUnsolicitedCommand->commandId, pProcessUnsolicitedCommand->data);   
  }
#endif  
  
  (void)aClusterId;
  return gZbFailed_c;
}

#if gZclEnableOccupancySensor_c  
/*!
 * @fn 		void TmrOccupancyCallBack(tmrTimerID_t tmrid)
 *
 * @brief	Occupancy Cluster Callback
 *
 */
void TmrOccupancyCallBack(tmrTimerID_t tmrid)
{
  zbClusterId_t   aClusterId = {gaZclClusterOccupancySensor_c};
  uint8_t  occupancyState = 0;
  (void) tmrid;
  gSetAvailableOccupancy = TRUE;
  (void)ZCL_GetAttribute(0x08, aClusterId, gZclAttrOccupancySensing_OccupancyId_c, gZclServerAttr_c, &occupancyState, NULL);
  occupancyState = (occupancyState == 0x00)?0x01:0x00; 
  (void)ZCL_SetAttribute(0x08, aClusterId, gZclAttrOccupancySensing_OccupancyId_c, gZclServerAttr_c, &occupancyState);
  TMR_FreeTimer(occupancyTimer);
}
#endif //gZclEnableOccupancySensor_c
#endif //gZclClusterOptionals_d

/*!
 * @fn 		bool_t InterpretMatchDescriptor(zbNwkAddr_t  aDestAddress, uint8_t endPoint)
 *
 * @brief	This fucntion return TRUE only if the MatchDescriptor Req was generated by a OTA Cluster command
 *
 */
bool_t InterpretMatchDescriptor(zbNwkAddr_t  aDestAddress, uint8_t endPoint)
{
  bool_t status = FALSE;
#if (gZclEnableOTAClient_d)  
  status = InterpretOtaMatchDescriptor(aDestAddress, endPoint);
#endif
  return status;  
}

/*!
 * @fn 		void ZCL_SaveNvmZclData(void)
 *
 * @brief	General function used to save NVM ZCL data
 *
 */
void ZCL_SaveNvmZclData(void)
{
  uint8_t index = 0;
  NVM_DataEntry_t  zcl_DataTable[] =
  {
    gAPP_DATA_SET_FOR_NVM,
    {NULL,0,0}  /* Required end-of-table marker. */
  };
  
  while(zcl_DataTable[index].pData)
  {
    if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
      (void)NvSaveOnIdle(zcl_DataTable[index].pData, TRUE); 
    else
      NvSaveOnInterval(zcl_DataTable[index].pData);
    index++;
  }
}

/*!
 * @fn 		zbEndPoint_t ZCL_GetEndPointForSpecificCluster(zbClusterId_t clusterId, bool_t isClusterServer)
 *
 * @brief	 GetEndPoint for a specific cluster based on Endpoint List informations
 *
 */
zbEndPoint_t ZCL_GetEndPointForSpecificCluster(zbClusterId_t clusterId, bool_t isClusterServer)
{
#if gNum_EndPoints_c != 0     

  uint8_t i, j;
  for(i=0;i<gNum_EndPoints_c;i++)
  {
   if(isClusterServer) 
   {
    for(j=0;j<endPointList[i].pEndpointDesc->pSimpleDesc->appNumInClusters;j++)
      if(IsEqual2Bytes(clusterId, &endPointList[i].pEndpointDesc->pSimpleDesc->pAppInClusterList[j*2]))
        return endPointList[i].pEndpointDesc->pSimpleDesc->endPoint;
   }
   else
   {
     for(j=0;j<endPointList[i].pEndpointDesc->pSimpleDesc->appNumOutClusters;j++)
      if(IsEqual2Bytes(clusterId, &endPointList[i].pEndpointDesc->pSimpleDesc->pAppOutClusterList[j*2]))
        return endPointList[i].pEndpointDesc->pSimpleDesc->endPoint;
   }
     
  }
#endif  
  return 0xFF;
}

