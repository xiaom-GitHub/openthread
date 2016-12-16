/******************************************************************************
* Interface definition for EEPROM driver
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
#ifndef _EEPROM_H_
#define _EEPROM_H_

#include "EmbeddedTypes.h"
#include "AppToPlatformConfig.h"
#include "FunctionLib.h"

#ifdef __cplusplus
    extern "C" {
#endif
/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/* The maximum amount of MCU Flash memory */
extern uint32_t __region_ROM_end__[];
#define gFlashParams_MaxImageLength_c      ((uint32_t)__region_ROM_end__ + 1)

/* List of the EEPROM devices used on each of the FSL development boards */
#define gEepromDevice_InternalFlash_c 0
#define gEepromDevice_AT45DB161E_c    1 /* TWR-KW2x */
#define gEepromDevice_AT26DF081A_c    2 /* TWR-MEM */

/* Define the Eeprom Type used */
#if gTargetKW24D512_USB_d == 1
  #define gEepromType_d gEepromDevice_InternalFlash_c
#else
  #define gEepromType_d gEepromDevice_AT45DB161E_c
#endif

/* Characteristics of the EEPROM device */
#if (gEepromType_d == gEepromDevice_AT26DF081A_c)
  #define gEepromParams_TotalSize_c           0x100000 /* 1 MByte */
  #define gEepromParams_StartOffset_c         0xFFFFFFFF /* Not defined */
  
#elif (gEepromType_d == gEepromDevice_AT45DB161E_c)
  #define gEepromParams_TotalSize_c           0x200000 /* 16 Mbit */
  #define gEepromParams_StartOffset_c         0xFFFFFFFF /* Not defined */

#elif (gEepromType_d == gEepromDevice_InternalFlash_c)
  extern uint32_t INT_STORAGE_START[];
  extern uint32_t INT_STORAGE_END[];
  #define gEepromParams_StartOffset_c          ((uint32_t)INT_STORAGE_END)
  #define gEepromParams_TotalSize_c            ((uint32_t)INT_STORAGE_START - (uint32_t)INT_STORAGE_END)

#else
  #warning "No EEPROM type defined. Defaulting to zero size..."
  #define gEepromParams_TotalSize_c            0x00000
#endif


#define gBootValueForTRUE_c                 0x00
#define gBootValueForFALSE_c                0xFF

#define gBootData_ImageLength_Offset_c      0x00
#define gBootData_ImageLength_Size_c        0x04
#define gBootData_SectorsBitmap_Offset_c   (gBootData_ImageLength_Offset_c + \
					    gBootData_ImageLength_Size_c)
#define gBootData_SectorsBitmap_Size_c      32
#define gBootData_Image_Offset_c           (gBootData_SectorsBitmap_Offset_c + \
                                            gBootData_SectorsBitmap_Size_c)


/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
typedef enum
{
  ee_ok,
  ee_too_big,
  ee_not_aligned,
  ee_busy,
  ee_error
} ee_err_t;

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/******************************************************************************
* NAME: EEPROM_Init
* DESCRIPTION: Initializes the EEPROM peripheral
* PARAMETERS: None
* RETURN: ee_ok - if the EEPROM is initialized successfully
*         ee_error - otherwise
******************************************************************************/
ee_err_t EEPROM_Init
(
  void /*IN: No Input Parameters*/
);


/******************************************************************************
* NAME: EEPROM_ReadData
* DESCRIPTION: Reads the specified number of bytes from EEPROM memory
* PARAMETERS: [IN] NoOfBytes - number of bytes to be read
*             [IN] Addr - EEPROM address to start reading from
*             [OUT] inbuf - a pointer to a memory location where the
*                           data read out from EEPROM will be stored
* RETURN: ee_ok - if the read operation completed successfully
*         ee_too_big - if the provided address is out of range
*         ee_error - otherwise
******************************************************************************/
ee_err_t EEPROM_ReadData
(
  uint16_t NoOfBytes,/* IN: No of bytes to read */
  uint32_t Addr,		 /* IN: EEPROM address to start reading from */
  uint8_t  *inbuf		 /* OUT:Pointer to read buffer */
);

/******************************************************************************
* NAME: EEPROM_WriteData
* DESCRIPTION: Writes the specified number of bytes to EEPROM memory
* PARAMETERS: [IN] NoOfBytes - number of bytes to be written.
*             [IN] Addr - EEPROM address to start writing at.
*             [IN] Outbuf - a pointer to a memory location where the
*                           data to be written is stored.
* RETURN: ee_ok - if the write operation completed successfully
*         ee_too_big - if the provided address is out of range
*         ee_error - if the write operation fails
******************************************************************************/
ee_err_t EEPROM_WriteData
(
  uint32_t  NoOfBytes,/* IN: No of bytes to write */
  uint32_t Addr,		 /* IN: EEPROM address to start writing at. */
  uint8_t  *Outbuf	 /* IN: Pointer to data to write to EEPROM  */
);

/******************************************************************************
* NAME: EEPROM_isBusy
* DESCRIPTION: This function tests if the EEPROM is busy
* PARAMETERS: None
* RETURN: TRUE if EEPROM is busy or FALSE if EEPROM is ready(idle)
******************************************************************************/
uint8_t EEPROM_isBusy
(
  void /*IN: No Input Parameters*/
);


/******************************************************************************
* NAME: EEPROM_ErasePage
* DESCRIPTION: Erase the specified page
* PARAMETERS: None
* RETURN: ee_busy - the EEPROM is busy
*         ee_too_big - the specified page is out of range
*         ee_ok - the erase operation completed successfully
******************************************************************************/
ee_err_t EEPROM_ErasePage
(
    uint16_t pageAddress
);

/******************************************************************************
* NAME: EEPROM_EraseBlock
* DESCRIPTION:  This function erase a block of 4Kbytes of memory
* PARAMETERS: None
* RETURN: ee_busy - the EEPROM is busy
*         ee_too_big - the specified address is out of range
*         ee_ok - the block erase operation completed successfully
******************************************************************************/
ee_err_t EEPROM_EraseBlock
(
  uint32_t Addr, /* IN: EEPROM address within the 4Kbyte block to be erased. */
  uint32_t size
);

/******************************************************************************
* NAME: EEPROM_ChipErase
* DESCRIPTION:  This function erase the entire EEPROM memory
* PARAMETERS: None
* RETURN: ee_busy - the EEPROM is busy         
*         ee_ok - the block erase operation completed successfully
******************************************************************************/
ee_err_t EEPROM_ChipErase
(
  void /*IN: No Input Parameters*/
);

#ifdef __cplusplus
}
#endif

#endif /* _EEPROM_H_ */
