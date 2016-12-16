/******************************************************************************
* This file contains the declarations for different tables used in BeeStack.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/

#ifndef _BeeStackParameters_h
#define _BeeStackParameters_h
#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif
#include "beestack_globals.h"
/******************************************************************************
*******************************************************************************
* Public type declarations
*******************************************************************************
******************************************************************************/

/* includes the APS Information Base (AIB) Table 2.24 */
typedef struct beeStackParameters_tag {

  /* Memory Allocated for AddressMapIndex of ZdoNVM Section */
  uint8_t iAddrMapIndex;

  /* this window size must be the same on both sending and receiving nodes (1-8) */
  uint8_t gApsMaxWindowSize;

  /* ID=0xc9 APS Fragmentation: 0-255, delay in ms between frames */
  uint8_t gApsInterframeDelay;

  /* ID=APS fragmentation, maximum size of fragment for each OTA frame (1-0x4e) */
  uint8_t gApsMaxFragmentLength;

  /* ID=0xc2, should we start up as a coordinator or router? */
  bool_t gfApsDesignatedCoordinator;

  /* ID=0xc6, For Multicast, stack profile 0x02 only */
  uint8_t  gApsNonmemberRadius;

  /* ID=0xc8, should we start up using insecure join? */
  bool_t gfApsUseInsecureJoinDefault;

#if gStandardSecurity_d || gHighSecurity_d
  /*
    053474r20 ZigBee Pro spec Table 4.36
    Name: apsTrustCenterAddress
    Type: Device Address
    Range: Any valid 64-bt address
    Default: --
    AIB Id: 0xab
  */

  /* The short addres must be save on NVM as well. */
  zbNwkAddr_t  apsTrustCenterNwkAddress;

  /*
    053474r20 ZigBee Pro spec Table 4.36
    Name: The period of time a device will wait for an expected security protocol
          frame (in milliseconds)
    Type: Integer
    Range: 0x0000 - 0xffff
    Default: 1000
    AIB Id: 0xac
  */
  uint16_t apsSecurityTimeOutPeriod;
#endif
} beeStackParameters_t;

/******************************************************************************
*******************************************************************************
* Public data declarations
*******************************************************************************
******************************************************************************/

extern beeStackParameters_t gBeeStackParameters;



  /* ID=0xc1, Memory Allocated for Binding table */
  extern apsBindingTable_t gaApsBindingTable[];
  
  /* Memory Allocated for AddressMap table */
  extern zbAddressMap_t gaApsAddressMap[];
  extern uint8_t gaApsAddressMapBitMask[];
extern addrMapIndex_t gGlobalAddressMapCounter;


/* Memory allocated for GroupTable */
  
#if MC13226Included_d
 extern uint8_t gEpMaskSize;
#else
 extern const uint8_t gEpMaskSize;
#endif
	

extern zbGroupTable_t gaApsGroupTable[];

/* Memory allocated for the security material */
/* Only gets included if the compile time option is enable */
#if gApsLinkKeySecurity_d
  extern zbApsDeviceKeyPairSet_t gaApsDeviceKeyPairSet[];
#if MC13226Included_d
  extern uint8_t giApsDeviceKeyPairCount;
#else
  extern const uint8_t giApsDeviceKeyPairCount;
#endif
#endif /*gApsLinkKeySecurity_d*/


extern zbKeyEstablish_t  *apSKKEMaterial[];
/* Memory allocated for the skke state machine. */
/* Only gets included if the compile time option is enable */
extern uint8_t gSKKEStateMachineSize;



/*****************************************************************************************
* WARNING: ANY CHANGE INTO ANY OF THE DATA SETS WILL BE REFLECTED INTO THE FUNCITON WHO
* HANDLES THE NV DATA ZdoNwkMng_SaveToNvm, SO PLEASE ADJUST ANY CHANGE MADE TO DATA SETS
* INTO THE FUNCTION LOCATED AT ZdoNwkManager.c
******************************************************************************************/
typedef uint16_t nvmTableEntryId_t;

/* Beestack Nvm Data  Id's */
typedef enum {
     
     /* Nvm Nwk Data  Id's */
     nvmId_NwkData_c=0,
     nvmId_BeeStackConfig_c,
     nvmId_RouteTable_c,
     nvmId_NeighborTable_c,
     nvmId_ApsAddressMapBitMask_c,
     nvmId_ApsAddressMap_c,
     nvmId_ApsKeySet_c,
     nvmId_ApsDeviceKeyPairSet_c,
     nvmId_IncomingFrameCounterSet1_c,
     nvmId_IncomingFrameCounterSet2_c,
     nvmId_OutgoingFrameCounter1_c,
     nvmId_OutgoingFrameCounter2_c,
     
     /* Nvm Aps Data  Id's */
     nvmId_BeeStackParameters_c,
     nvmId_SAS_Ram_c,
     nvmId_ApsBindingTable_c,
     nvmId_ApsGroupTable_c,
       
     /*********** keep this element at the end ********/
     maxBeestackDataSetId=32
}nvmBeestackDataSetId;

/* Used for storing the NWK and APS data set in NVM */

#define gNWK_DEFAULT_DATA_SET_FOR_NVM\
  {&gNwkData,                   1,                                   sizeof(gNwkData),                      nvmId_NwkData_c},\
  {&gBeeStackConfig,            1,                                   sizeof(beeStackConfigParams_t),        nvmId_BeeStackConfig_c},\
   /* gaNvNTDataSet */\
  {gaNeighborTable,             gNwkInfobaseMaxNeighborTableEntry_c, sizeof(neighborTable_t),               nvmId_NeighborTable_c},\
  /* gaNvAddrMapDataSet */\
  {gaApsAddressMapBitMask,      gApsAddressMapBitMaskInBits_c,       sizeof(uint8_t),                       nvmId_ApsAddressMapBitMask_c},\
  {gaApsAddressMap,             gApsMaxAddrMapEntries_c,             sizeof(zbAddressMap_t),                nvmId_ApsAddressMap_c}

#if gRnplusCapability_d
#define gNWK_ROUTETABLE_DATA_SET_FOR_NVM\
   ,{gaRouteTable,                gNwkInfobaseMaxRouteTableEntry_c,    sizeof(routeTable_t),                nvmId_RouteTable_c}
#else
#define gNWK_ROUTETABLE_DATA_SET_FOR_NVM   
#endif   
  
#if gApsLinkKeySecurity_d
#define gAPS_LINK_KEY_SECURITY_DATA_SET_FOR_NVM\
  ,{gaApsKeySet,                 gApsMaxLinkKeys_c,                   sizeof(zbAESKey_t),                   nvmId_ApsKeySet_c},\
  {gaApsDeviceKeyPairSet,       gApsMaxSecureMaterialEntries_c,      sizeof(zbApsDeviceKeyPairSet_t),      nvmId_ApsDeviceKeyPairSet_c}
#else
#define gAPS_LINK_KEY_SECURITY_DATA_SET_FOR_NVM
#endif
  
#if gNwkSecSaveIncomingCounters_c
#define gNWK_DATA_SEC_INCOMING_FRAME_COUNTER_SET\
  ,{gaIncomingFrameCounterSet1,  gNwkInfobaseMaxNeighborTableEntry_c, sizeof(zbIncomingFrameCounterSet_t), nvmId_IncomingFrameCounterSet1_c},\
  {gaIncomingFrameCounterSet2,  gNwkInfobaseMaxNeighborTableEntry_c, sizeof(zbIncomingFrameCounterSet_t), nvmId_IncomingFrameCounterSet2_c}
#else
#define gNWK_DATA_SEC_INCOMING_FRAME_COUNTER_SET
#endif
  
#if (gStandardSecurity_d || gHighSecurity_d)
#define gNWK_SECURITY_DATA_SET_FOR_NVM\
  gNWK_DATA_SEC_INCOMING_FRAME_COUNTER_SET\
  ,{&(gNwkData.aNwkSecurityMaterialSet[0].outgoingFrameCounter),1,    sizeof(zbFrameCounter_t),            nvmId_OutgoingFrameCounter1_c},\
  {&(gNwkData.aNwkSecurityMaterialSet[1].outgoingFrameCounter),1,    sizeof(zbFrameCounter_t),            nvmId_OutgoingFrameCounter2_c}
#else
#define gNWK_SECURITY_DATA_SET_FOR_NVM
#endif  
  
  
#define gNWK_DATA_SET_FOR_NVM\
  /* gaNvNwkDataSet */\
  gNWK_DEFAULT_DATA_SET_FOR_NVM\
  gNWK_ROUTETABLE_DATA_SET_FOR_NVM\
  gAPS_LINK_KEY_SECURITY_DATA_SET_FOR_NVM\
  gNWK_SECURITY_DATA_SET_FOR_NVM
    

  

/* Used for storing the APS data set in NVM */
#ifndef gHostApp_d

#define gAPS_DATA_SET_FOR_NVM\
      {&gBeeStackParameters,  1,                                 sizeof(beeStackParameters_t),        nvmId_BeeStackParameters_c},\
      {&gSAS_Ram,             1,                                 sizeof(zbCommissioningAttributes_t), nvmId_SAS_Ram_c},\
      {gaApsBindingTable,     gMaximumApsBindingTableEntries_c,  sizeof(apsBindingTable_t),           nvmId_ApsBindingTable_c},\
      {gaApsGroupTable,       gApsMaxGroups_c,                   sizeof(zbGroupTable_t),              nvmId_ApsGroupTable_c}

#else

#define gAPS_DATA_SET_FOR_NVM\
      {&gSAS_Ram,             1,                                 sizeof(zbCommissioningAttributes_t), nvmId_SAS_Ram_c},\
      {gaApsGroupTable,       gApsMaxGroups_c,                   sizeof(zbGroupTable_t),              nvmId_ApsGroupTable_c}

#endif /* gHostApp_d */
      
#endif                                  /* #ifndef _BeeStackParameters_h */      