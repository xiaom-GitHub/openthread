/******************************************************************************
* Filename: Aes.c
*
* Description: 
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
#include "PortConfig.h"
#include "MMCAU_Interface.h"
#include "FunctionLib.h"


/******************************************************************************
* Calculate C(i) := E(key, A(i) ^ M(i)) for i=1,...,t
*
* Interface assumptions:
*   All Input/Output pointers must refer to a memory address alligned to 4 bytes!
*
* Return value:
*   None
* 
*******************************************************************************/
void SecLib_Aes(const uint8_t *pData, const uint8_t *pKey, uint8_t *pReturnData)
{
#if gMMCAU_Support_d
  uint8_t key_expansion[44*4];
  uint8_t alignedIn[16];
  uint8_t alignedOut[16];
  uint8_t *pIn, *pOut;

  /* Check if pKey is 4 bytes aligned */
  if ((uint32_t)pKey & 0x00000003) 
  {
    FLib_MemCpyAligned32bit((uint8_t*)pKey, alignedIn , sizeof(alignedIn));
    pIn = alignedIn;
  }
  else
    pIn = (uint8_t*)pKey;
    
  /* Expand Key */  
  mmcau_aes_set_key(pIn, AES128, key_expansion);
  
  /* Check if pData is 4 bytes aligned */
  if ((uint32_t)pData & 0x00000003) 
  {
    FLib_MemCpyAligned32bit((uint8_t*)pData, alignedIn , sizeof(alignedIn));
    pIn = alignedIn;
  }
  else
    pIn = (uint8_t*)pData;
  
  /* Check if pReturnData is 4 bytes aligned */
  if ((uint32_t)pReturnData & 0x00000003) 
    pOut = alignedOut;
  else
    pOut = (uint8_t*)pReturnData;

  /* Encript data */
  mmcau_aes_encrypt (pIn, key_expansion, AES128_ROUNDS, pOut);
  
  if (pOut == alignedOut)
    FLib_MemCpyAligned32bit(alignedOut,(uint8_t*)pReturnData, sizeof(alignedOut));
  
#else /* gMMCAU_Support_d */

  sw_Aes128(pData, pKey, pReturnData);
#endif /* gMMCAU_Support_d */
}


/*! *********************************************************************************
* \brief  This function performs AES ECB on a block of bytes of the specified length
*
* \param[in]  pData Pointer to the data to be secured.
*
* \param[in]  pKey Pointer to the key used for securing the data.
*
* \param[out]  pReturnData Pointer to the location the secured data willl be written
*
* \param[in]  blockCount Number of 128 bit (16 byte) blocks to be encrypted
*
* \pre All Input/Output pointers must refer to a memory address alligned to 4 bytes!
*
********************************************************************************** */

void SecLib_AesEcb (uint8_t* pData,
                    const uint8_t* pKey,
                    uint8_t* pReturnData,
                    uint8_t blockCount)
{   
    while (blockCount)
    {
        SecLib_Aes (pData,
                    pKey,
                    pReturnData);
        blockCount--;
        pReturnData += 16;
        pData += 16;
    }
}


/******************************************************************************
* Calculate XOR of individual byte pairs in two uint8_t arrays. I.e.
* pDst[i] := pDst[i] ^ pSrc[i] for i=0 to n-1
*
* Interface assumptions:
*   None
*
* Return value:
*   None
* 
*******************************************************************************/
void CCM_XorN(uint8_t *pDst, uint8_t *pSrc, uint8_t n)
{
  while(n) {
    *pDst = *pDst ^ *pSrc;
    pDst = pDst + 1;
    pSrc = pSrc + 1;
    n--;
  }
}
/********************************** EOF ***************************************/
