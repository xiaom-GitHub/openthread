/************************************************************************************
*
*(c) Copyright 2010, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
************************************************************************************/

#ifndef _OTA_SUPPORT_H_
#define _OTA_SUPPORT_H_
#ifdef __cplusplus
    extern "C" {
#endif

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef enum
{
  gOtaSucess_c = 0,
  gOtaNoImage_c,
  gOtaUpdated_c,
  gOtaError_c,
  gOtaCrcError_c,
  gOtaInvalidParam_c,
  gOtaInvalidOperation_c,
  gOtaExternalFlashError_c,
  gOtaInternalFlashError_c,
} otaResult_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/


/************************************************************************************
*  Starts the process of writing a new image to the external EEPROM.
*
*  Input parameters:
*  - lenght: the lenght of the image to be written in the EEPROM
*  - receivedCRC: upgrade image CRC 
*  Return:
*  - gOtaInvalidParam_c: the intended lenght is bigger than the FLASH capacity
*  - gOtaInvalidOperation_c: the process is already started (can be cancelled)
*  - gOtaEepromError_c: can not detect extrenal EEPROM
************************************************************************************/
extern otaResult_t OTA_StartImage(uint32_t length);


/************************************************************************************
*  Places the next image chunk into the external FLASH.
*
*  Input parameters:
*  - pData: pointer to the data chunk
*  - length: the length of the data chunk
*  - pImageLength: if it is not null and the function call is sucessfull, it will be
*       filled with the current lenght of the image
*  Return:
*  - gOtaInvalidParam_c: pData is NULL or the resulting image would be bigger than the 
*       final image length specified with OTA_StartImage()
*  - gOtaInvalidOperation_c: the process is not started
************************************************************************************/
extern otaResult_t OTA_PushImageChunk(uint8_t* pData, uint8_t length, uint32_t* pImageLength);

/************************************************************************************
*  Finishes the writing of a new image to the external EEPROM.
*  It will write the image header (signature and lenght) and footer (sector copy bitmap).
*
*  Input parameters:
*  - bitmap: pointer to a  byte array indicating the sector erase pattern for the
*       internal FLASH before the image update.
*  Return:
*  - gOtaInvalidOperation_c: the process is not started,
*  - gOtaEepromError_c: error while trying to write the EEPROM 
************************************************************************************/
extern otaResult_t OTA_CommitImage(uint8_t* bitmap);


/************************************************************************************
*  Cancels the process of writing a new image to the external EEPROM.
*
*  Input parameters:
*  - None
*  Return:
*  - None
************************************************************************************/
extern void OTA_CancelImage(void);
/*****************************************************************************
*  OTA_SetNewImageFlag
*  
*  Input parameters:
*  - None
*  Return:
*  - None
*  It is called to write flags from flash - new image present
*****************************************************************************/
void OTA_SetNewImageFlag(void);
/*****************************************************************************
*  It is called to init external eeprom or flash
*  
*  Input parameters:
*  - None
*  Return:
*  - None
*****************************************************************************/
extern void OTA_InitExternalMemory(void);

/************************************************************************************
*  Read from  the external EEPROM or flash.
*
* *  Input parameters:
*  - pData: pointer to the data chunk
*  - length: the length of the data chunk
*  - address: address in external memory
*  Return:
*  - gOtaInvalidParam_c
************************************************************************************/
extern otaResult_t OTA_ReadExternalMemory(uint8_t* pData, uint8_t length, uint32_t address);

/************************************************************************************
*  Write from the external EEPROM or flash..
*
* *  Input parameters:
*  - pData: pointer to the data chunk
*  - length: the length of the data chunk
*  - address: address in external memory
*  Return:
*  - gOtaInvalidParam_c
************************************************************************************/
extern otaResult_t OTA_WriteExternalMemory(uint8_t* pData, uint8_t length, uint32_t address);
/************************************************************************************
*  OTA_CrcCompute
*
* *  Input parameters:
*  - pData: pointer to the data chunk
*  - lenData: the length of the data chunk
*  - crcValueOld: crc old value
*  Return:
*  - computed crc
************************************************************************************/
extern uint16_t OTA_CrcCompute(uint8_t *pData, uint16_t lenData, uint16_t crcValueOld);
/*****************************************************************************
*  OTA_EraseMemory
*
*  This function is called in order to erase external flash or eeprom
*
*****************************************************************************/
extern otaResult_t OTA_EraseExternalMemory(void);
/*****************************************************************************
*  OTA_EraseBlock
*
*  This function is called in order to erase a block of memory (2k for internal Flash, 4k for external memory)
*
*****************************************************************************/
extern otaResult_t OTA_EraseBlock(uint32_t address);

#ifdef __cplusplus
}
#endif

#endif /* _OTA_SUPPORT_H_ */
