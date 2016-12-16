/************************************************************************************
* This header file is provided as part of the interface to the freeScale 802.15.4
* MAC and PHY layer.
*
* The file gives access to the generic function library used by the MAC/PHY.
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

#ifndef _MACPHYFLIB_H_
#define _MACPHYFLIB_H_

#include "FunctionLib.h"

#ifdef __cplusplus
    extern "C" {
#endif

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

#define FLib_Cmp8Bytes(dst, src) FLib_MemCmp((dst), (src), 8);
#define LongToByteArray(uint32, arr4) {arr4[0] = (uint8_t)((uint32)>>0);  arr4[1] = (uint8_t)((uint32)>>8);  \
                                       arr4[2] = (uint8_t)((uint32)>>16); arr4[3] = (uint8_t)((uint32)>>24); }

#define ByteArrayToShort(arr2, uint16) ((uint16) = (uint16_t)((arr2)[0] | ((arr2)[1] << 8)))

#define ByteArrayToLong(arr4, uint32) {              \
  uint16_t uint16a = arr4[0] | (uint16_t)arr4[1]<<8; \
  uint16_t uint16b = arr4[2] | (uint16_t)arr4[3]<<8; \
  uint32 = ((uint32_t)uint16b<<16) | uint16a; }

#define ByteArrayToZbClock24(arr3, zbClock24) {               \
  zbClock16_t zbClock16a = arr3[0] | (zbClock16_t)arr3[1]<<8; \
  zbClock16_t zbClock16b = (zbClock16_t)arr3[2];              \
  zbClock24 = (zbClock24_t)(((zbClock24_t)zbClock16b<<16) | zbClock16a); }

#define ZbClock24ToByteArray(zbClock24, arr3) \
{ \
  arr3[0] = (uint8_t)(zbClock24 >> 0);  \
  arr3[1] = (uint8_t)(zbClock24 >> 8);  \
  arr3[2] = (uint8_t)(zbClock24 >> 16); \
}

#define FLib_AddrModeToLen(addrMode) ((uint8_t)FLib_AddrModeToLength((uint8_t)(addrMode))) 


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
* Convert an 802.15.4 address mode to a length in bytes, Input values must be 0, 2,
* or 3. Other values will return either 0 or 2.
* 
* Interface assumptions:
*   None
*   
* Return value:
*   0, 2, or 8.
* 
************************************************************************************/
uint8_t FLib_AddrModeToLength
  (
  uint8_t addrMode // IN: 802.15.4 address mode (0, 2, or 3)
  );

/************************************************************************************
* Copy a number of bytes to a destination array and increment destination pointer 
* accordingly.
* 
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
************************************************************************************/
void FLib_ArrayAddArray
  (
  uint8_t **ppDestArray, //OUT: Address of destination array pointer
  uint8_t *pArray, // IN: The byte array to be added
  uint8_t length   // IN: Number of bytes to be added
  );

/************************************************************************************
* Add the value of a specific PIB attribute to the destination Array and increment
* destination pointer accordingly. 
* 
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
************************************************************************************/
void FLib_ArrayAddPibAttribute
  (
  uint8_t **ppDestArray, //OUT: Address of destination array pointer
  uint8_t pibAttribute // IN: The PIB attribute (e.g. gMacPanId_c)
  );

/************************************************************************************
* Add one byte to the destination array and increment destination array pointer
* accordingly
* 
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
************************************************************************************/
void FLib_ArrayAddByte
  (
  uint8_t **ppDestArray, //OUT: Address of destination array pointer
  uint8_t byte // IN: Byte to add to the Array
  );

#ifdef __cplusplus
}
#endif

#endif /* _FUNCTION_LIB_H_ */
