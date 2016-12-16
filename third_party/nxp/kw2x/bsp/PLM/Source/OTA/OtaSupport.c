/******************************************************************************
* This is the source file containing the code that enables the OTA protocol
* to load an image received over the air into an external EEPROM memory, using
* the format that the bootloader will understand
*
*
* (c) Copyright 2010, Freescale Semiconductor inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from FreeScale.
*
******************************************************************************/
#include "Embeddedtypes.h"
#include "OtaSupport.h"
#include "Eeprom.h"
#include "NV_Flash.h"


/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/
#define gOtaVerifyWrite             TRUE
#define gStartFlashAddress_c        (0x00)

/* There are 2 flags stored in the internal Flash of the MCU that tells
  1. whether there is a bootable image present in the external EEPROM
  2. whether the load of a bootable image from external EEPROM to internal
Flash has been completed. This second flag is useful in case the MCU is reset
while the loading of image from external EEPROM to internal Flash is in progress
  No matter the platform (MC1320x, MC1321x, MC1323x), these 2 flags are always
located at a fixed address in the internal FLASH */
extern uint32_t __BootFlags_Start__[];
#define gBootImageFlagsAddress_c       ((uint32_t)__BootFlags_Start__)


/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/* Structure containing the 2 boot flags */
typedef struct
{
  uint8_t bNewBootImageAvailable;
  uint8_t bBootProcessCompleted;
  uint8_t padding[2];
}bootInfo_t;



/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/

/* Flag storing we are already in the process of writing an image received
OTA in the EEPROM or not */
static  bool_t    mLoadOtaImageInEepromInProgress = FALSE;
/* Total length of the OTA image that is currently being written in EEPROM */
static  uint32_t  mOtaImageTotalLength = 0;
/* The length of the OTA image that has being written in EEPROM so far */
static  uint32_t  mOtaImageCurrentLength = 0;
/* Current write address in the EEPROM */
static  uint32_t  mCurrentEepromAddress = 0;
/* When a new image is ready the flash flags will be write in idle task */
static  bool_t    mNewImageReady = FALSE;
/* Current CRC value */
/*static uint16_t mCrcCompute  = 0;*/
/* CRC received with image and use for  check */
/*static uint16_t mCrcReceived = 0;*/


/* Variables used by the Bootloader */
#ifdef gUseBootloader_d
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = "BootloaderFlags"
__root const bootInfo_t gBootFlags = 
#elif defined(__GNUC__)
__root const bootInfo_t gBootFlags __attribute__ ((section(".BootloaderFlags"))) = 
#else
__root const bootInfo_t gBootFlags = 
#endif
{gBootValueForFALSE_c, gBootValueForTRUE_c, {0x00, 0x02}};
#endif

/******************************************************************************
*******************************************************************************
* Public Memory
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
*  OTA_StartImage
*
*  This function is called in order to start a session of writing a OTA image.
*
*
*****************************************************************************/
otaResult_t OTA_StartImage(uint32_t length)
{
  /* Check if we already have an operation of writing an OTA image in the EEPROM
  in progess and if yes, deny the current request */
  if(mLoadOtaImageInEepromInProgress)
  {
    return gOtaInvalidOperation_c;
  }
  /* Check if the internal FLASH and the EEPROM have enough room to store
  the image */
  if((length > gFlashParams_MaxImageLength_c) ||
     (length > (gEepromParams_TotalSize_c - gBootData_Image_Offset_c)))
  {
    return gOtaInvalidParam_c;
  }
  /* Try to initialize the EEPROM */
  if(EEPROM_Init() != ee_ok)
  {
    return gOtaExternalFlashError_c;
  }
  /* Save the total length of the OTA image */
  mOtaImageTotalLength = length;
  /* Init the length of the OTA image currently written */
  mOtaImageCurrentLength = 0;
  /* Init the current EEPROM write address */
  mCurrentEepromAddress = gBootData_Image_Offset_c;
  /* Mark that we have started loading an OTA image in EEPROM */
  mLoadOtaImageInEepromInProgress = TRUE;

  return gOtaSucess_c;
}


/*****************************************************************************
*  OTA_PushImageChunk
*
*  Put image chunck on external memory
*****************************************************************************/
otaResult_t OTA_PushImageChunk(uint8_t* pData, uint8_t length, uint32_t* pImageLength)
{

  /* Cannot add a chunk without a prior call to OTA_StartImage() */
  if(mLoadOtaImageInEepromInProgress == FALSE)
  {
    return gOtaInvalidOperation_c;
  }

  /* Validate parameters */
  if((length == 0) || (pData == NULL))
  {
    return gOtaInvalidParam_c;
  }

  /* Check if the chunck does not extend over the boundaries of the image */
  if(mOtaImageCurrentLength + length > mOtaImageTotalLength)
  {
    return gOtaInvalidParam_c;
  }

  /* Try to write the data chunk into the external EEPROM */
  if(EEPROM_WriteData((uint16_t)length, mCurrentEepromAddress, pData) != ee_ok)
  {
    return gOtaExternalFlashError_c;
  }

#if gOtaVerifyWrite == TRUE
  {
    uint8_t i, readData[256];

    EEPROM_ReadData((uint16_t)length, mCurrentEepromAddress, readData);
    for (i=0; i<length; i++)
      if (readData[i] != pData[i])
      {
        return gOtaExternalFlashError_c;
      }
  }
#endif

  /* Data chunck successfully writtem into EEPROM
     Update operation parameters */
  mCurrentEepromAddress   += length;
  mOtaImageCurrentLength += length;

  /* Return the currenlty written length of the OTA image to the caller */
  if(pImageLength != NULL)
  {
    *pImageLength = mOtaImageCurrentLength;
  }


  return gOtaSucess_c;
}


/*****************************************************************************
*  OTA_CommitImage
*
*  Check image length and checksum, set bitmap and upgrade image flags or
*  signature needed by bootloader
*****************************************************************************/

otaResult_t OTA_CommitImage(uint8_t* pBitmap)
{
  /* Cannot commit a image without a prior call to OTA_StartImage() */
  if(mLoadOtaImageInEepromInProgress == FALSE)
  {
    return gOtaInvalidOperation_c;
  }
  /* If the currently written image length in EEPROM is not the same with
  the one initially set, commit operation fails */
  if(mOtaImageCurrentLength != mOtaImageTotalLength)
  {
    return gOtaInvalidOperation_c;
  }

  /* To write image length into the EEPROM */
  if(EEPROM_WriteData(sizeof(uint32_t), gBootData_ImageLength_Offset_c,(uint8_t *)&mOtaImageCurrentLength) != ee_ok)
  {
    return gOtaExternalFlashError_c;
  }

  /* To write the sector bitmap into the EEPROM */
  if(EEPROM_WriteData(gBootData_SectorsBitmap_Size_c, gBootData_SectorsBitmap_Offset_c, pBitmap) != ee_ok)
  {
    return gOtaExternalFlashError_c;
  }

  /* Flash flags will be write in next instance of idle task */
  mNewImageReady = TRUE;
  
  /* End the load of OTA in EEPROM process */
  mLoadOtaImageInEepromInProgress = FALSE;

  return gOtaSucess_c;
}
/*****************************************************************************
*  OTA_SetNewImageFlag
*
*   Is is called to set the the new image flag
*****************************************************************************/
void OTA_SetNewImageFlag(void)
{
  /* OTA image successfully writen in EEPROM. Set the flag that indicates that at the next
  boot, the image from FLASH will be updated with the one from EEPROM */
#ifdef gUseBootloader_d

  uint32_t newImageFlags = (0xFFFFFF00 | gBootValueForTRUE_c);

  if(TRUE == mNewImageReady)
  {
    if (NV_FlashProgramLongword(&mNvConfig, (uint32_t)&gBootFlags, sizeof(newImageFlags), (uint32_t)&newImageFlags))
    {
      return;
    }
    mNewImageReady = FALSE;
  }
#else
  (void)mNewImageReady;
#endif
}

/*****************************************************************************
*  OTA_CancelImage
*
*  Cancel upgrade image write process
*****************************************************************************/
void OTA_CancelImage()
{
  mLoadOtaImageInEepromInProgress = FALSE;
  mNewImageReady = FALSE;
}
/************************************************************************************
*  Updates the CRC based on the received data to process.
*  Updates the global CRC value. This was determined to be optimel from a resource
*  consumption POV.
*
*  Input parameters:
*  - None
*  Return:
*  - None
************************************************************************************/
uint16_t OTA_CrcCompute(uint8_t *pData, uint16_t lenData, uint16_t crcValueOld)
{

  uint8_t i;

  while(lenData--)
  {
    crcValueOld ^= *pData++ << 8;
    for( i = 0; i < 8; ++i )
    {
      if( crcValueOld & 0x8000 )
      {
	crcValueOld = (crcValueOld << 1) ^ 0x1021;
      }
      else
      {
        crcValueOld = crcValueOld << 1;
      }
    }
  }
  return crcValueOld;
}

/*****************************************************************************
*  OTA_InitExternalMemory
*
*  This function is called in order to initialize all elements needed for
*  external memory access
*
*****************************************************************************/
void OTA_InitExternalMemory(void)
{
  static bool_t mEepromInitStatus = FALSE;
 
  if(mEepromInitStatus == FALSE) 
  {
    if(EEPROM_Init() != ee_ok)
    {
      for(;;);
    }
    mEepromInitStatus = TRUE;
  }
}

/*****************************************************************************
*  OTA_EraseMemory
*
*  Function used when external memory must be erased before writing
*
*****************************************************************************/
otaResult_t OTA_EraseExternalMemory(void)
{
  if (EEPROM_ChipErase() != ee_ok)
    return gOtaExternalFlashError_c;


  return gOtaSucess_c;
}

/*****************************************************************************
*  OTA_ReadExternalMemory
*
*  This function is called in order to read from external flash or eeprom
*
*****************************************************************************/
otaResult_t OTA_ReadExternalMemory(uint8_t* pData, uint8_t length, uint32_t address)
{

  if(ee_ok != EEPROM_ReadData(length,address,pData))
  {
    return gOtaExternalFlashError_c;
  }

  return gOtaSucess_c;
}

/*****************************************************************************
*  OTA_WriteFlash
*
*  This function is called in order to write in external flash or eeprom
*
*****************************************************************************/
otaResult_t OTA_WriteExternalMemory(uint8_t* pData, uint8_t length, uint32_t address)
{

  if(ee_ok != EEPROM_WriteData(length, address, pData))
  {
   
    return gOtaExternalFlashError_c;
  }
    
#if gOtaVerifyWrite == TRUE
  {
    uint8_t i, readData[256];

    EEPROM_ReadData((uint16_t)length, address, readData);
    for (i=0; i<length; i++)
      if (readData[i] != pData[i])
      {
        return gOtaExternalFlashError_c;
      }
  }
#endif
  return gOtaSucess_c;
}

/*****************************************************************************
*  OTA_EraseBlock
*
*  This function is called in order to erase a block of memory (2k for internal Flash, 4k for external memory)
*
*****************************************************************************/
otaResult_t OTA_EraseBlock(uint32_t address)
{
  uint32_t sectorSize;
#if(gUseInternalFlashForOta_c)  
  sectorSize = 2048;
#else
  sectorSize = 4096;
#endif
  if(EEPROM_EraseBlock(address, sectorSize)!= ee_ok)
    return gOtaExternalFlashError_c;
  return gOtaSucess_c;
}
/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

