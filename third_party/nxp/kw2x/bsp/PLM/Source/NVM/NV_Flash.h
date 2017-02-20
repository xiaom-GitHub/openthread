/******************************************************************************
* Filename: NV_Flash.h
*
* Description: Private header file of the non-volatile storage module 
*              for CORTEX-M4 processor     
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

#ifndef _NV_FLASH_H_
#define _NV_FLASH_H_

#include "EmbeddedTypes.h"
#include "NVM_Interface.h"
#include "NV_FlashHAL.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
 ******************************************************************************
 * Public macros
 ******************************************************************************
 ******************************************************************************/

/*
 * Name: gNvInactivePageHeader_c
 * Description: self explanatory
 */
#define gNvInactivePageHeader_c        0xFFFFFFFFuL

/*
 * Name: gNvInvalidMetaInfoAddress_c
 * Description: self explanatory
 */
#define gNvInvalidMetaInfoAddress_c    0xFFFFFFFFuL

/*
 * Name: gNvInvalidRecordAddress_c
 * Description: self explanatory
 */
#define gNvInvalidRecordAddress_c      0xFFFFFFFFuL

/*
 * Name: gNvEmptyMetaValue_c
 * Description: self explanatory
 */
#define gNvEmptyMetaValue_c            0xFFFFFFFFuL

/*
 * Name: gNvErasedFlashCellValue_c
 * Description: self explanatory
 */
#define gNvErasedFlashCellValue_c      0xFF

/*
 * Name: gNvInvalidTableEntryIndex_c
 * Description: self explanatory
 */
#define gNvInvalidTableEntryIndex_c    0xFFFF

/*
 * Name: gNvInvalidElementIndex_c
 * Description: self explanatory
 */
#define gNvInvalidElementIndex_c       0xFFFF

/*
 * Name: gValidationByteSingleRecord_c
 * Description: the value of validation byte used in meta tag to mark a single record type
 */
#define gValidationByteSingleRecord_c  0xAA

/*
 * Name: gValidationByteAllRecords_c
 * Description: the value of validation byte used in meta tag to mark an entire table entry type
 */
#define gValidationByteAllRecords_c    0x55

/*
 * Name: gPageCounterMaxValue_c
 * Description: self explanatory
 */
#define gPageCounterMaxValue_c         0xFFFFFFFFUL

/*
 * Name: gEmptyPageMetaAddress_c
 * Description: the value of a page that has no records yet
 */
#define gEmptyPageMetaAddress_c        0x3FFF0000UL

/*
 * Name: gFifoOverwriteEnabled_c
 * Description: if set to TRUE, it enables the pending saves queue to be overwritten
 */
#define gFifoOverwriteEnabled_c        FALSE

/*
 * Name: gNvCopyAll_c
 * Description: definition used when a full page copy is requested
 */
#define gNvCopyAll_c                   0xFFFFU

/*
 * Name: gNvFlexFormatBufferSize_c
 * Description: the size of the buffer used for FlexNVM formating. The FlexRAM
 *              is "formatted" in blocks of gNvFlexFormatBufferSize_c bytes
 */    
#define gNvFlexFormatBufferSize_c      32
    
/*
 * Name: gNvFlexGuardValue_c
 * Description: value of the guard space between meta info and records
 *              (four 0xFF bytes)
 */   
#define gNvFlexGuardValue_c            0xFFFFFFFFUL

/*
 * Name: gRAMTable_c
 * Description: Macro used to map the RAM table (see NvGetTableSize())
 */
#define gRAMTable_c                    0

/*
 * Name: gFLASHTable_c
 * Description: Macro used to map the FLASH table (see NvGetTableSize())
 */
#define gFLASHTable_c                  1

/*****************************************************************************
 ******************************************************************************
 * Public type declarations
 ******************************************************************************
 ******************************************************************************/

/*
 * Name: NVM_RecordMetaInfo_t
 * Description: NVM record meta information type definition
 */
#pragma pack(1)
typedef union NVM_RecordMetaInfo_tag
{
    uint64_t rawValue;
    struct 
    {
        uint8_t NvValidationStartByte;
        uint16_t NvmDataEntryID; 
        uint16_t NvmElementIndex; 
        uint16_t NvmRecordOffset; 
        uint8_t NvValidationEndByte;
    } fields;
} NVM_RecordMetaInfo_t;
#pragma pack()

/*
 * Name: NVM_VirtualPageID_t
 * Description: virtual page ID type definition
 */
typedef enum NVM_VirtualPageID_tag 
{
    gFirstVirtualPage_c = 0,
    gSecondVirtualPage_c,
    gVirtualPageNone_c
} NVM_VirtualPageID_t;

/*
 * Name: NVM_VirtualPageProperties_t
 * Description: virtual page properties type definition
 */
typedef struct NVM_VirtualPageProperties_tag
{
    uint32_t NvRawSectorStartAddress;
    uint32_t NvRawSectorEndAddress;
    uint8_t NvRawSectorsCount;
    uint32_t NvTotalPageSize;
    uint32_t NvLastMetaInfoAddress;
} NVM_VirtualPageProperties_t;

typedef struct NVM_ErasePageCmdStatus_tag
{
    bool_t NvErasePending;
    NVM_VirtualPageID_t NvPageToErase;
    uint32_t NvSectorAddress;
} NVM_ErasePageCmdStatus_t;

/*
 * Name: NVM_TableEntryInfo_t
 * Description: table entry indexes type definition
 */
typedef struct NVM_TableEntryInfo_tag
{
    NvTableEntryId_t entryId;
    uint16_t elementIndex;
    bool_t saveRestoreAll;
} NVM_TableEntryInfo_t;

/*
 * Name: NVM_SaveQueue_t
 * Description: Circular queue used for pending saves data type definition
 */
typedef struct NVM_SaveQueue_tag
{
    NVM_TableEntryInfo_t  QData[gNvPendigSavesQueueSize_c];  /* queue data */
    index_t  Head;    /* read index */    
    index_t  Tail;    /* write index */
    uint8_t EntriesCount; /* entries count */
} NVM_SaveQueue_t;

/*
 * Name: NVM_FlexMetaInfo_t
 * Description: FlexNVM meta information type definition 
 */
#if gNvUseFlexNVM_d
typedef union NVM_FlexMetaInfo_tag
{
  uint32_t rawValue;
  struct
  {
    uint16_t NvDataEntryID;
    uint16_t NvDataOffset;
  } fields;
} NVM_FlexMetaInfo_t;
#endif

/*****************************************************************************
 ******************************************************************************
 * Public memory declarations
 ******************************************************************************
 ******************************************************************************/

extern NvConfig_t mNvConfig;

#ifdef __cplusplus
}
#endif

#endif /* _NV_FLASH_H_ */


