/************************************************************************************
* This is a global header file for setting up the functionalities that should be
* included in the build.
*
* Copyright (c) 2014, Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of Freescale Semiconductor, Inc. nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
************************************************************************************/

#ifndef _FUNCTIONALITY_DEFINES_H_
#define _FUNCTIONALITY_DEFINES_H_

#include "PlatformToMacPhyConfig.h"
#include "AppToMacPhyConfig.h"

#ifdef __cplusplus
    extern "C" {
#endif

#define gDeviceTypeRFDNBNS_d  1
#define gDeviceTypeRFDNBNV_d  2
#define gDeviceTypeRFDNB_d    3
#define gDeviceTypeRFD_d      4
#define gDeviceTypeFFDNBNS_d  5
#define gDeviceTypeFFDNBNV_d  6
#define gDeviceTypeFFDNB_d    7
#define gDeviceTypeFFDNGTS_d  8
#define gDeviceTypeFFD_d      9
#define gDeviceTypeFFDPNBNV_d 10

#define gDeviceTypeFFDZSNGTSNV_d        11
#define gDeviceTypeFFDZSNGTSNVNS_d      12
#define gDeviceTypeFFDNGTSNVNS_d        13
#define gDeviceTypeFFDZSNBNV_d          14
#define gDeviceTypeFFDZSNBNVNS_d        15
#define gDeviceTypeFFDZSNBNVNSCE_d      16
#define gDeviceTypeFFDZSNBNVNSCE_PROM_d 17
#define gDeviceTypeFFDNBNVNS_d          18

#define gDeviceTypeFFDPZSNGTSNV_d   19
#define gDeviceTypeFFDPZSNGTSNVNS_d 20
#define gDeviceTypeFFDPNGTSNVNS_d   21
#define gDeviceTypeFFDPZSNBNV_d     22
#define gDeviceTypeFFDPZSNBNVNS_d   23
#define gDeviceTypeFFDPNBNVNS_d     24

#define gDeviceTypeRFDZSNV_d        25
#define gDeviceTypeRFDZSNVNS_d      26
#define gDeviceTypeRFDNVNS_d        27
#define gDeviceTypeRFDZSNBNV_d      28
#define gDeviceTypeRFDZSNBNVNS_d    29
#define gDeviceTypeRFDNBNVNS_d      30
#define gDeviceTypeRFDZSNBNVNSCE_d  31

#define gDeviceTypeFFDNBZP_d        32

// Target specific compiler defines (no value) "Type_..."

#ifdef Type_RFD
	#define gDeviceType_d gDeviceTypeRFD_d
	#define DEVICE_TYPE "RFD  "
#endif //Type_RFD

#ifdef Type_FFDNB
	#define gDeviceType_d gDeviceTypeFFDNB_d
	#define DEVICE_TYPE "FFDNB"
#endif //Type_FFDNB

#ifdef Type_FFD
	#define gDeviceType_d gDeviceTypeFFD_d
	#define DEVICE_TYPE "FFD  "
#endif //Type_FFD
        
#ifdef Type_FFDNBZP
    #define gDeviceType_d gDeviceTypeFFDNBZP_d
    #define DEVICE_TYPE "FFDNBZP"
#endif /* Type_FFDNBZP */

#ifndef gDeviceType_d // If device type is not predefined use the following definitions:

  #define DEVICE_TYPE "XXXXXXX" // Max size

  /**********************************************************************************
  ***********************************************************************************
  * Role capability definitions
  ***********************************************************************************
  **********************************************************************************/

    // Set if the device need to be able to take on the device role
  #define gDeviceCapability_d 1

    // Set if the device need to be able to take on the coordinator role
  #define gCoorCapability_d 1

  /**********************************************************************************
  ***********************************************************************************
  * Functionality inclusion definitions that depend on device type
  ***********************************************************************************
  **********************************************************************************/

    // Set if the device need to be able to take on the pan coordinator role
    // Is never set unless coordinator capability is set
  #define gPanCoorCapability_d (1 && gCoorCapability_d)

    // Set if device (in role of either device or ccordinator) supports beaconed
    // operation. Note that ...
  #define gBeaconedCapability_d 1

    // Set if device (in role of either device or PAN cordinator) supports GTS in
    // beaconed operation. Is never set unless beaconed operation is supported
  #define gGtsCapability_d (1 && gBeaconedCapability_d && (gDeviceCapability_d || gPanCoorCapability_d))

    // SCAN functionalities - optional for RFD and below (=spec. RFD)
  #define gEnergyScanCapability_d 1
  #define gActiveScanCapability_d 1

    // MCPS-PURGE.request capabilities
  #define gPurgeCapability_d 1
  
    // Set if security is supported in the MAC/PHY
  #define gSecurityCapability_d 1

    // Define to 0 to disabled or 1 to enable verification of NWK to MLME primitives
  #define gVerifyParamCapability_d 1

    // Set if the common security library should be included.
  #define gSecurityLibraryCapability_d 0

  // Set if the disassociation capability should be included
  #define gDisassociateCapability_d 1

#else // If gDeviceType_d is defined use one of the following presets:

  #if gDeviceType_d == gDeviceTypeFFD_d
    #define gDeviceCapability_d           1
    #define gCoorCapability_d             1
    #define gPanCoorCapability_d          1
    #define gBeaconedCapability_d         1
    #define gGtsCapability_d              1
    #define gEnergyScanCapability_d       1
    #define gActiveScanCapability_d       1
    #define gPurgeCapability_d            1
    #define gSecurityCapability_d         1
    #define gVerifyParamCapability_d      1
    #define gSecurityLibraryCapability_d  0
    #define gDisassociateCapability_d     1
    #define gRxEnableCapability_d         1
  #elif gDeviceType_d == gDeviceTypeFFDNB_d
    #define gDeviceCapability_d           1
    #define gCoorCapability_d             1
    #define gPanCoorCapability_d          1
    #define gBeaconedCapability_d         0
    #define gGtsCapability_d              0
    #define gEnergyScanCapability_d       1
    #define gActiveScanCapability_d       1
    #define gPurgeCapability_d            1
    #define gSecurityCapability_d         1
    #define gVerifyParamCapability_d      1
    #define gSecurityLibraryCapability_d  0
    #define gDisassociateCapability_d     1
    #define gRxEnableCapability_d         1
  #elif gDeviceType_d == gDeviceTypeRFD_d
    #define gDeviceCapability_d           1
    #define gCoorCapability_d             0
    #define gPanCoorCapability_d          0
    #define gBeaconedCapability_d         1
    #define gGtsCapability_d              0
    #define gEnergyScanCapability_d       0
    #define gActiveScanCapability_d       1
    #define gPurgeCapability_d            0
    #define gSecurityCapability_d         1
    #define gVerifyParamCapability_d      1
    #define gSecurityLibraryCapability_d  0
    #define gDisassociateCapability_d     1
    #define gRxEnableCapability_d         0
  #elif gDeviceType_d == gDeviceTypeFFDNBZP_d
    #define gDeviceCapability_d           1
    #define gCoorCapability_d             1
    #define gPanCoorCapability_d          1
    #define gBeaconedCapability_d         0
    #define gGtsCapability_d              0
    #define gEnergyScanCapability_d       1
    #define gActiveScanCapability_d       1
    #define gPurgeCapability_d            1
    #define gSecurityCapability_d         1
    #define gVerifyParamCapability_d      1
    #define gSecurityLibraryCapability_d  0
    #define gDisassociateCapability_d     1
    #define gRxEnableCapability_d         1
  #else
    #error Invalid gDeviceType_d define value
  #endif // gDeviceType_d
#endif // !gDeviceType_d

/************************************************************************************
*************************************************************************************
* Functionality inclusion definitions that are not directly dependant on device type
*************************************************************************************
************************************************************************************/

  // Set to one to support the BeeStack scheduler
#ifndef gSchedulerIntegration_d
#define gSchedulerIntegration_d 1
#endif /* gSchedulerIntegration_d */

  // Set if ASP features are supported
#define gAspPowerSaveCapability_d   (1 && gAspCapability_d)
#define gAspEventCapability_d       (1 && gAspCapability_d)
#define gAspHwCapability_d          (1 && gAspCapability_d)
#define gAspPowerLevelCapability_d  (1 && gAspCapability_d)
#define gAspFADCapability_d         (1 && gAspCapability_d)         // Fast Antenna Diversity

  // Set to 1 if new disassociate request primitive must be supported. Does not require additional code.
#define gNewDisassociateReq_d 0

  // Set to 0 if you do not want random initialisation of macDSN, macBSN, and the random backoff seed
#if !defined gInitializeWithRandomSeed_d
#define gInitializeWithRandomSeed_d 1
#endif /* gInitializeWithRandomSeed_d */

#if gGtsCapability_d==1
#undef gGtsCapability_d
#define gGtsCapability_d 1   // Master switch for GTS feature
#endif // gGtsCapability_d

// Enable MAX ACK wait time allowed by the 802.15.4 standard (54 symbols)
#define gStandardAckWindow_d TRUE

/************************************************************************************
*************************************************************************************
* Proprietary stuff... 
*************************************************************************************
************************************************************************************/

  // Treemode capability
#define gTreemodeCapability_d (1 && gCoorCapability_d && gDeviceCapability_d && gBeaconedCapability_d)

  // Beacon start indications
#define gBeaconStartIndicationCapability_d 0 // Used to be: (0 || gTreemodeCapability_d)

#ifdef __cplusplus
}
#endif

#endif /* _FUNCTIONALITY_DEFINES_H_ */
