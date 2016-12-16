/******************************************************************************
* This is the Source file for the AT26DF081A FLASH driver
*														
* (c) Copyright 2011, FreeScale Semiconductor inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from FreeScale.
*
******************************************************************************/

#include "Eeprom.h"
#include "PortConfig.h"
#include "NV_Flash.h"

#if gEepromType_d == gEepromDevice_InternalFlash_c
/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/
static ee_err_t EEPROM_PrepareForWrite(uint32_t NoOfBytes, uint32_t Addr);

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/
static uint8_t mEepromEraseBitmap[32];

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
*  EEPROM_Init
*
*  Initializes the EEPROM peripheral
*
*****************************************************************************/
ee_err_t EEPROM_Init(void)
{
  FLib_MemSet (mEepromEraseBitmap, 0x00, 32);
  return ee_ok;
}

/*****************************************************************************
*  EEPROM_ChipErase
*
*  Erase all memory to 0xFF
*
*****************************************************************************/
ee_err_t EEPROM_ChipErase(void)
{
  return ee_ok;
}

/*****************************************************************************
*  EEPROM_EraseSector4K
*
*  Erase 4K of memory to 0xFF
*
*****************************************************************************/
ee_err_t EEPROM_EraseBlock(uint32_t Addr, uint32_t size)
{
  if (size != 2048)
    return ee_error;

  if (NV_FlashEraseSector(&mNvConfig, gEepromParams_StartOffset_c + Addr, size))
    return ee_error;

  return ee_ok;
}

/*****************************************************************************
*  EEPROM_WriteData
*
*  Writes a data buffer into EEPROM, at a given address
*
*****************************************************************************/
ee_err_t EEPROM_WriteData(uint32_t NoOfBytes, uint32_t Addr, uint8_t *Outbuf)
{
  ee_err_t retval = ee_ok;

  if (NoOfBytes == 0)
    return ee_ok;

  retval = EEPROM_PrepareForWrite(NoOfBytes, Addr);
  if (retval != ee_ok)
    return retval;

  if (NV_FlashProgramUnalignedLongword(&mNvConfig, gEepromParams_StartOffset_c + Addr, NoOfBytes, (uint32_t)Outbuf))
    return ee_error;

  return retval;
}


/*****************************************************************************
*  EEPROM_ReadData
*
*  Reads a data buffer from EEPROM, from a given address
*
*****************************************************************************/
ee_err_t EEPROM_ReadData(uint16_t NoOfBytes, uint32_t Addr, uint8_t *inbuf)
{
  FLib_MemCpy(inbuf, (void*)(gEepromParams_StartOffset_c + Addr), NoOfBytes);
  return ee_ok;
}

/*****************************************************************************
*  EEPROM_ReadStatusReq
*
*
*****************************************************************************/
uint8_t EEPROM_isBusy(void)
{
  return FALSE;
}

/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/


/*****************************************************************************
*  EEPROM_WriteData
*
*  Writes a data buffer into the External Memory, at a given address
*
*****************************************************************************/
static ee_err_t EEPROM_PrepareForWrite(uint32_t NoOfBytes, uint32_t Addr)
{
  uint32_t i;
  uint32_t startBlk, endBlk;

  /* Obtain the first and last block that need to be erased */
  startBlk = Addr >> 11; //(X>>11 == X/2048)
  endBlk   = (Addr + NoOfBytes) >> 11;

  /* Check if the block was previousley erased */
  for(i = startBlk; i <= endBlk; i++)
    if ( (mEepromEraseBitmap[i/8] & (1 << (i%8) ) ) == 0)
    {
      if (EEPROM_EraseBlock(i * 2048, 2048) != ee_ok)
        return ee_error;
      mEepromEraseBitmap[i/8] |= 1 << (i%8);
    }

  return ee_ok;
}

#endif /* gEepromDevice_InternalFlash_c */