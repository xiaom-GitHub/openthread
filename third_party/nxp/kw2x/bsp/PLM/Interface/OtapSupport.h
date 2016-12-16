/************************************************************************************
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

#ifndef _OTAP_SUPPORT_H_
#define _OTAP_SUPPORT_H_

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
  gOtapSucess_c = 0,
  gOtapNoImage_c,
  gOtapUpdated_c,
  gOtapError_c,
  gOtapCrcError_c,
  gOtapInvalidParam_c,
  gOtapInvalidOperation_c,
  gOtapExternalFlashError_c,
  gOtapInternalFlashError_c,
} otapResult_t;

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
*  - gOtapInvalidParam_c: the intended lenght is bigger than the FLASH capacity
*  - gOtapInvalidOperation_c: the process is already started (can be cancelled)
*  - gOtapEepromError_c: can not detect extrenal EEPROM
************************************************************************************/
extern otapResult_t OTAP_StartImage(uint32_t length, uint32_t receivedCrc);

/************************************************************************************
*  Starts the process of writing a new image to the external EEPROM.
*
*  Input parameters:
*  - lenght: the lenght of the image to be written in the EEPROM
*  Return:
*  - gOtapInvalidParam_c: the intended lenght is bigger than the FLASH capacity
*  - gOtapInvalidOperation_c: the process is already started (can be cancelled)
*  - gOtapEepromError_c: can not detect extrenal EEPROM
************************************************************************************/
extern otapResult_t OTAP_StartImage_NoCRC(uint32_t length);

/************************************************************************************
*  Places the next image chunk into the external FLASH.
*
*  Input parameters:
*  - pData: pointer to the data chunk
*  - length: the length of the data chunk
*  - pImageLength: if it is not null and the function call is sucessfull, it will be
*       filled with the current lenght of the image
*  Return:
*  - gOtapInvalidParam_c: pData is NULL or the resulting image would be bigger than the 
*       final image length specified with OTAP_StartImage()
*  - gOtapInvalidOperation_c: the process is not started
************************************************************************************/
extern otapResult_t OTAP_PushImageChunk(uint8_t* pData, uint8_t length, uint32_t* pImageLength);

/************************************************************************************
*  Places the next image chunk into the external FLASH. The CRC will not be computed.
*
*  Input parameters:
*  - pData: pointer to the data chunk
*  - length: the length of the data chunk
*  - pImageLength: if it is not null and the function call is sucessfull, it will be
*       filled with the current lenght of the image
*  Return:
*  - gOtapInvalidParam_c: pData is NULL or the resulting image would be bigger than the 
*       final image length specified with OTAP_StartImage()
*  - gOtapInvalidOperation_c: the process is not started
************************************************************************************/
extern otapResult_t OTAP_PushImageChunk_NoCRC(uint8_t* pData, uint8_t length, uint32_t* pImageLength);

/************************************************************************************
*  Finishes the writing of a new image to the external EEPROM.
*  It will write the image header (signature and lenght) and footer (sector copy bitmap).
*
*  Input parameters:
*  - bitmap: pointer to a  byte array indicating the sector erase pattern for the
*       internal FLASH before the image update.
*  Return:
*  - gOtapInvalidOperation_c: the process is not started,
*  - gOtapEepromError_c: error while trying to write the EEPROM 
************************************************************************************/
extern otapResult_t OTAP_CommitImage(uint8_t* bitmap);

/************************************************************************************
*  Finishes the writing of a new image to the external EEPROM.
*  It will write the image header (signature and lenght) and footer (sector copy bitmap).
*
*  Input parameters:
*  - bitmap: pointer to a  byte array indicating the sector erase pattern for the
*       internal FLASH before the image update.
*  Return:
*  - gOtapInvalidOperation_c: the process is not started,
*  - gOtapEepromError_c: error while trying to write the EEPROM 
************************************************************************************/
extern otapResult_t OTAP_CommitImage_NoCRC(uint8_t* bitmap);

/************************************************************************************
*  Cancels the process of writing a new image to the external EEPROM.
*
*  Input parameters:
*  - None
*  Return:
*  - None
************************************************************************************/
extern void OTAP_CancelImage(void);
/*****************************************************************************
*  OTAP_WriteNewImageFlashFlags
*  
*  Input parameters:
*  - None
*  Return:
*  - None
*  It is called in idle task to write flags from flash - new image present
*****************************************************************************/
void OTAP_WriteNewImageFlashFlags(void);
/*****************************************************************************
*  It is called to init external eeprom or flash
*  
*  Input parameters:
*  - None
*  Return:
*  - None
*****************************************************************************/
extern void OTAP_InitExternalMemory(void);

/************************************************************************************
*  Read from  the external EEPROM or flash.
*
* *  Input parameters:
*  - pData: pointer to the data chunk
*  - length: the length of the data chunk
*  - address: address in external memory
*  Return:
*  - gOtapInvalidParam_c
************************************************************************************/
extern otapResult_t OTAP_ReadExternalMemory(uint8_t* pData, uint8_t length, uint32_t address);

/************************************************************************************
*  Write from the external EEPROM or flash..
*
* *  Input parameters:
*  - pData: pointer to the data chunk
*  - length: the length of the data chunk
*  - address: address in external memory
*  Return:
*  - gOtapInvalidParam_c
************************************************************************************/
extern otapResult_t OTAP_WriteExternalMemory(uint8_t* pData, uint8_t length, uint32_t address);
/************************************************************************************
*  OTAP_CrcCompute
*
* *  Input parameters:
*  - pData: pointer to the data chunk
*  - lenData: the length of the data chunk
*  - crcValueOld: crc old value
*  Return:
*  - computed crc
************************************************************************************/
extern uint16_t OTAP_CrcCompute(uint8_t *pData, uint16_t lenData, uint16_t crcValueOld);
/*****************************************************************************
*  OTAP_EraseMemory
*
*  This function is called in order to erase external flash or eeprom
*
*****************************************************************************/
extern otapResult_t OTAP_EraseExternalMemory(void);

#ifdef __cplusplus
}
#endif

#endif /* _OTAP_SUPPORT_H_ */
