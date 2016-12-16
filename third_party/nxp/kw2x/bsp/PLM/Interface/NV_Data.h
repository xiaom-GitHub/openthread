/******************************************************************************
* Filename: NV_Data.h
*
* Description: Declarations for the application client of the Enhanced NV  
*              storage module
* 
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
***************************************************************************/

#ifndef _NV_DATA_H_
#define _NV_DATA_H_

#include "EmbeddedTypes.h"

#ifdef gZtcOtapSupport_d
#include "Eeprom.h"
#endif

#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

/* PA level limit for each individual channel */
#ifndef gChannelPa_c
#define gChannelPa_c           { gAspPowerLevel_4dBm,   /* 11 */ \
                                 gAspPowerLevel_4dBm,   /* 12 */ \
                                 gAspPowerLevel_4dBm,   /* 13 */ \
                                 gAspPowerLevel_4dBm,   /* 14 */ \
                                 gAspPowerLevel_4dBm,   /* 15 */ \
                                 gAspPowerLevel_4dBm,   /* 16 */ \
                                 gAspPowerLevel_4dBm,   /* 17 */ \
                                 gAspPowerLevel_4dBm,   /* 18 */ \
                                 gAspPowerLevel_4dBm,   /* 19 */ \
                                 gAspPowerLevel_4dBm,   /* 20 */ \
                                 gAspPowerLevel_4dBm,   /* 21 */ \
                                 gAspPowerLevel_4dBm,   /* 22 */ \
                                 gAspPowerLevel_4dBm,   /* 23 */ \
                                 gAspPowerLevel_4dBm,   /* 24 */ \
                                 gAspPowerLevel_4dBm,   /* 25 */ \
                                 gAspPowerLevel_4dBm }  /* 26 */
#endif

#ifndef gDefaultPowerLevel_c
#define gDefaultPowerLevel_c     gAspPowerLevel_m4dBm
#endif

/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/
#define gaMC1324x_CCA_Threshold_c 0x4B // -75dbm

#ifdef gZtcOtapSupport_d
  #define gDefaultInternalStorage_StartAddress_c gEepromParams_StartOffset_c
#else
  #define gDefaultInternalStorage_StartAddress_c 0xFFFFFFFF
#endif

/*****************************************************************************
******************************************************************************
* Public type definitions
******************************************************************************
*****************************************************************************/

/* The Fresscale version string struct is placed at a specific location in */
/* ROM by the linker command file, and is normally only written during */
/* manufacturing. */
typedef PACKED_STRUCT FreescaleVersionStrings_tag
{
  uint16_t NV_RAM_Version;
  uint8_t MAC_Version[4];
  uint8_t PHY_Version[4];
  uint8_t STACK_Version[4];
  uint8_t APP_Version[4];
  uint8_t HWName_Revision[4];
  uint8_t SerialNumber[4];
  uint16_t ProductionSite;
  uint8_t CountryCode;
  uint8_t ProductionWeekCode;
  uint8_t ProductionYearCode;
  uint8_t MCU_Manufacture;
  uint8_t MCU_Version;
  uint8_t NOT_USED;
} FreescaleVersionStrings_t;

/* The HardwareParameters_t struct is treated specially by both the */
/* linker command file and the startup (crt0.c) code. See the comments */
/* in crt0.c. */
/* The initialString and terminalString are unsigned chars, instead of */
/* uint8_t, because the C standard guarantees the sizeof(unsigned char). */
/* These strings are needed by the startup code to locate the current */
/* copy of the structure in NV storage, if there is one, and must be the */
/* first and last fields in the type. */
#define gaHardwareParametersDelimiterString_c   "Delim"

typedef PACKED_STRUCT HardwareParameters_tag 
{
  unsigned char initialString[ sizeof( gaHardwareParametersDelimiterString_c )];
  FreescaleVersionStrings_t FreescaleVersionStrings;
  uint8_t Bus_Frequency_In_MHz;
  uint8_t MAC_Address[8];
#if gDualPanEnabled_d
  uint8_t MAC_Address_PAN1[8];
#endif  
  uint8_t defaultPowerLevel;
  uint8_t useDualAntenna;
  uint8_t ccaThreshold;
  uint8_t paPowerLevelLimits[16];  
  uint32_t internalStorage_StartAddr;
  unsigned char terminalString[ sizeof( gaHardwareParametersDelimiterString_c )];
} HardwareParameters_t;

/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/

extern HardwareParameters_t const gHardwareParameters;
extern FreescaleVersionStrings_t const gFreescaleVersionStrings;
extern HardwareParameters_t const gHardwareParametersInit;

#ifdef __cplusplus
}
#endif

#endif //_NV_DATA_H_
