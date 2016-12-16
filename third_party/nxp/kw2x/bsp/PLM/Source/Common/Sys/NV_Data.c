/******************************************************************************
* Filename: NV_Data.c
*
* Description: Data definitions for the application client of the Enhanced NV  
*              storage module (ENVM)
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

#include "EmbeddedTypes.h"
#include "AppToPlatformConfig.h"
#include "NVM_Interface.h"
#include "portconfig.h"
#include "NV_Data.h"
#include "FunctionLib.h"
#include "Platforminit.h"
#include "AppAspInterface.h"

#ifndef gMacStandAlone_d
#define gMacStandAlone_d  0
#endif

/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/

/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/

/* The linker command file places this structure in a fixed location, to
 * make it easy to find it during manufacturing.
 * WARNING: The size of the FREESCALE_VERSION_STRINGS section in the linker
 * command file must be manually coordinated with the size of the
 * gFreescaleVersionStrings struct.
 */

/*NOTE: DO NOT CHANGE ORDER OF THESE 2 structures*/
#if (defined(__GNUC__))
	HardwareParameters_t const gHardwareParameters __attribute__ ((section ("FREESCALE_PROD_DATA"))) =
#elif (defined (__IAR_SYSTEMS_ICC__))
	#pragma location = "FREESCALE_PROD_DATA"
	HardwareParameters_t const gHardwareParameters = 
#else
	#warning Unknown tool chain
	HardwareParameters_t const gHardwareParameters =			
#endif	
/* Used to initialise gNvMacDataSet if no NV storage copy is found. */
 {
    gaHardwareParametersDelimiterString_c,    /* initialString */
    
    {      
      0xFFFF,                     /* uint16_t NV_RAM_Version; */
      {0x01,0xFF,0xFF,0xFF},      /* uint8_t MAC_Version[4]; */
      {0x02,0xFF,0xFF,0xFF},      /* uint8_t PHY_Version[4]; */
      {0x03,0xFF,0xFF,0xFF},      /* uint8_t STACK_Version[4]; */
      {0x04,0xFF,0xFF,0xFF},      /* uint8_t APP_Version[4]; */
      {0x05,0xFF,0xFF,0xFF},      /* uint8_t HWName_Revision[4]; */
      {0x06,0x02,0x03,0x04},      /* uint8_t SerialNumber[4]; */
      0x020F,                     /* uint16_t ProductionSite; */
      0x01,                       /* uint8_t CountryCode; */
      0x04,                       /* uint8_t ProductionWeekCode; */
      0x04,                       /* uint8_t ProductionYearCode; */
      0x00,                       /* uint8_t MCU_Manufacture; */
      0x02,                       /* uint8_t MCU_Version; */
      0xFF                        /* padding */
    },

    48, /* Bus_Frequency_In_MHz */

    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, /* uint8_t MAC_Address[8]; */
#if gDualPanEnabled_d
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, /* uint8_t MAC_Address1[8]; */
#endif
    gDefaultPowerLevel_c,                    /* uint8_t defaultPowerLevel; */
    FALSE,                                   /* uint8_t useDualAntenna; */
    gaMC1324x_CCA_Threshold_c,               /* ccaThreshold */
    gChannelPa_c,                            /* PA level for each channel */
    gDefaultInternalStorage_StartAddress_c,  /* uint32_t start address of the internal storage */
    gaHardwareParametersDelimiterString_c,   /* terminalString */
  };


/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/

/*
 * Name: NvDataTable
 * Description: NVM data table. Contains entries of datasets.
 *              Defined by appication.
 */
extern NVM_DataEntry_t NvDataTable[];

/*
 * Name: pNVM_DataTable
 * Description: Pointer to NVM table. The content of the table
 * is defined by the application code. See NvDataTable.
 */
NVM_DataEntry_t* pNVM_DataTable = &NvDataTable[0];

