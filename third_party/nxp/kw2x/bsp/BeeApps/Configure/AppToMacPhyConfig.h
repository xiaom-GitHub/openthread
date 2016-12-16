/************************************************************************************
* This file defines how the MAC/PHY is configured by the Application. This includes
* setting up the type of device (RFD, FFD, etc) and the number of buffers available
* for the MAC/PHY.
*
*
* (c) Copyright 2012, Freescale, Inc.  All rights reserved.
*
*
* No part of this document may be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/

#ifndef _APPTOMACPHYCONFIG_H_
#define _APPTOMACPHYCONFIG_H_

#ifdef __cplusplus
    extern "C" {
#endif

//**********************************************************************************
// Define MACtype library
//**********************************************************************************
  
#ifndef gMAC2006_d
#define gNumAclEntryDescriptors_c       4 // Number of ACL entries in MAC PIB
#else
#define gNumKeyTableEntries_c           2
  
#define gNumKeyIdLookupEntries_c        2
#define gNumKeyDeviceListEntries_c      2
#define gNumKeyUsageListEntries_c       2

#define gNumDeviceTableEntries_c        2
#define gNumSecurityLevelTableEntries_c 2
#endif  //gMAC2006_d 

  // Allows application to select if ASP shall be included
#define gAspCapability_d                1

  // Telec functionality for the Japanese market
#ifdef NO_TELEC
  #define gAspJapanTelecCapability_d    0
#else
  #define gAspJapanTelecCapability_d    (1 && gAspCapability_d)
#endif

  // Instruct the MAC if we are running with a task scheduler or with a mainloop (Mlme_Main)
#ifdef NO_SCHEDULER
#define gSchedulerIntegration_d 0
#else
#define gSchedulerIntegration_d 1
#endif

  // Inform the Application that 
#ifndef gDualPanEnabled_d
#define gDualPanEnabled_d 0
#endif

#ifdef __cplusplus
}
#endif

#endif //_APPTOMACPHYCONFIG_H_
