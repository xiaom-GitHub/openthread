/************************************************************************************
* This header file is for Ha RangeExtender application Interface
*
* (c) Copyright 2005, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/
#ifndef _BAPP_H_
#define _BAPP_H_

#include "BeeStack_Globals.h"
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "keyboard.h"
#include "zcl.h"
/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/
#define gSeMeteringDevice_d 1

#define gIdentifyTimeSecs_d 10

#define BeeKitGroupDefault_d  0x01,0x00

#define BeeKitSceneDefault_d 0x01 

#define SmartEnergyApplication_d TRUE

/* The top 5 events are reserved by BeeStack. Apps may use bits 0-10 */
#define gAppEvtDataConfirm_c      (1 << 15)
#define gAppEvtDataIndication_c   (1 << 14) 
#define gAppEvtStoreScene_c       (1 << 12) /*Store Scene event */
#define gAppEvtAddGroup_c         (1 << 13) /*Add group event */
#define gAppEvtSyncReq_c          (1 << 11)
#define gInterPanAppEvtDataConfirm_c      (1 << 10)
#define gInterPanAppEvtDataIndication_c   (1 << 9) 

#define gAppRadiusCounter_c      gNwkMaximumDepth_c * 2

#define gBeeAppsAsynchronousDeviceToSettle_c  150  

/* Default Aps security option - smart energy uses aps security*/
#if gApsLinkKeySecurity_d
  #if gZDPNwkSec_d
    #define zdoTxOptionsDefault_c (gApsTxOptionUseNwkKey_c)
  #else
    #define zdoTxOptionsDefault_c (gApsTxOptionSecEnabled_c)
  #endif
  #define afTxOptionsDefault_c (gApsTxOptionSecEnabled_c)
#else
/* default to no APS layer security (both ZigBee and ZigBee Pro) */
  #define afTxOptionsDefault_c gApsTxOptionNone_c
  #define zdoTxOptionsDefault_c gApsTxOptionNone_c
#endif

/* App Nvm Data  Id's */
typedef enum {   
     nvmId_ZclCommonAttr=maxBeestackDataSetId,
     nvmId_AslData_c,
     nvmId_ZclSE_KeyEstablished
}nvmAppDataSetId;

#define gAPP_DATA_SET_FOR_NVM\
      {&gZclCommonAttr,            1,         sizeof(zclCommonAttr_t),        nvmId_ZclCommonAttr},      /* scenes, location, etc... */\
      {&gAslData,                  1,         sizeof(ASL_Data_t),             nvmId_AslData_c},           /* state of ASL */\
      {&gZclSE_KeyEstablished,     1,         sizeof (gZclSE_KeyEstablished), nvmId_ZclSE_KeyEstablished}
      /* insert any user data for NVM here.... */
/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/
extern bool_t gZclSE_KeyEstablished;

/******************************************************************************
*******************************************************************************
* Public Prototypes
*******************************************************************************
******************************************************************************/
void BeeAppInit(void);
void BeeAppHandleKeys(key_event_t events);
void BeeAppUpdateDevice(zbEndPoint_t endPoint, zclUIEvent_t event, zclAttrId_t attrId, zbClusterId_t clusterId, void *pData);

/**********************************************************************************/
#endif /* _BAPP_H_ */
