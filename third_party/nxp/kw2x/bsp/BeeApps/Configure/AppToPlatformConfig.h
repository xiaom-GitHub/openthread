/************************************************************************************
* This file is owned by the platform/application specific software and basically 
* defines how the 802.15.4 Freescale MAC is configured. The file is included by the
* relevant MAC modules and is necessary for the MAC to compile.
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

#ifndef _APPTOPLATFORMCONFIG_H_
#define _APPTOPLATFORMCONFIG_H_

#ifdef __cplusplus
    extern "C" {
#endif

//**********************************************************************************

#ifndef gMAC_PHY_INCLUDED_c
#define gMAC_PHY_INCLUDED_c 1
#endif


#ifndef gTarget_UserDefined_d
#define gTarget_UserDefined_d 0
#endif

#ifndef gTargetTWR_K60D100M_d
#define gTargetTWR_K60D100M_d 0
#endif

#ifndef gTargetTWR_K20D50M_d
#define gTargetTWR_K20D50M_d 0
#endif

#ifndef gTargetTWR_K60N512_d
#define gTargetTWR_K60N512_d 0
#endif

#ifndef gTargetTWR_KW22D512_d
#define gTargetTWR_KW22D512_d 0
#endif

#ifndef gTargetTWR_KW21D512_d
#define gTargetTWR_KW21D512_d 0
#endif

#ifndef gTargetTWR_KW21D256_d
#define gTargetTWR_KW21D256_d 0
#endif


//**********************************************************************************
// TASKs PRIORITIES
//**********************************************************************************

#ifndef gTsUartTaskPriority_c
#define gTsUartTaskPriority_c               0x2A
#endif

#ifndef gTsiTaskPriority_c
#define gTsiTaskPriority_c                  0x07
#endif

#ifndef gTsI2CTaskPriority_c
#define gTsI2CTaskPriority_c                0x05
#endif

#ifndef gTsSpiTaskPriority_c
#define gTsSpiTaskPriority_c                0x10
#endif

#ifndef gTsTimerTaskPriority_c
#define gTsTimerTaskPriority_c	            0xFE
#endif

//**********************************************************************************
//  Max length of MPDU/PSDU without CRC
//  Maximum length for buffer to allow for req and payload for all layers
//**********************************************************************************

#ifndef gMaxRxTxDataLength_c
#define gMaxRxTxDataLength_c                (163)
#endif

//**********************************************************************************
//gUseInternalFlashForOta_c set to 0 will require an external FLASH for image storage
//gUseInternalFlashForOta_c set to 1 will store the received image in the internal FLASH
#ifndef gUseInternalFlashForOta_c
#define gUseInternalFlashForOta_c         FALSE
#endif

#if gUseInternalFlashForOta_c
#define gNvDisableIntCmdSeq_c    (TRUE)
#endif      
      
#if (gTargetTWR_KW21D256_d == 1)
  #define gNvUseFlexNVM_d TRUE
#endif

#ifdef __cplusplus
}
#endif

#endif /* _APPTOPLATFORMCONFIG_H_ */
