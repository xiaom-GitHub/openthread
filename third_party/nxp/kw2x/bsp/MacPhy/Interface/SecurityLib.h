/************************************************************************************
* SecurityLib implements AES and CCM* functionality. The functions are avaliable to
* all higher layers. The AES module is not reentrant.
*
* Copyright (c) 2014, Freescale Semiconductor, Inc. 
* All rights reserved.
*
* Freescale Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
************************************************************************************/

#ifndef _SECURITY_LIB_H_
#define _SECURITY_LIB_H_

#ifdef __cplusplus
    extern "C" {
#endif
/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

#define gCcmNonceLength_c 13 // Length of nonce used in ccm star.
#define gCcmBlockSize_c 16   // Block size in CCM star (128 bit)

#define CCM_AesCipherInplace(pKey, pData) SecLib_Aes(pData, (uint8_t *)pKey, pData)

/************************************************************************************
*************************************************************************************
* Public types
*************************************************************************************
************************************************************************************/

enum {
  gCcmEncode_c,
  gCcmDecode_c
};
typedef uint8_t gCcmDirection_t;


/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************

it will process the result in about 8000 cycles on the current platform. Please note that the function:
1) Always uses 128 bit (16 bytes) inputs/output.
2) Is not re-entrant (and is not re-entrant with execution of the CCMStar function either).
3) It is allowed to set the ReturnData pointer to the same memory as either Data or Key if wanted.

************************************************************************************/


/************************************************************************************
* Calculate CCM* as defined for ZigBee
*
* Interface assumptions:
*
* Header, Message and Integrity code have to be located in memory as they appear in
* the packet. That is, as a concatenated string in that order.
*
* For levels with encryption Message is encrypted in-place (overwriting Message).
*
* Depending on the security level the function does:
*
* Level  Action                                   CCM* engine inputs
* 0:	   Nothing.
* 1,2,3: Integrity only based on Header||Message. a is Header||Message, m is empty
* 4:	   Encryption of Message only.              a is empty, m is Message
* 5,6,7: "Normal" CCM on Header and Message.      a is Header, m is Message
*
* Please note that the function is not re-entrant.
*
* Return value:
*   Return status of the operation (always ok = 0 for encoding)
* 
************************************************************************************/
uint8_t SecLib_CcmStar
  (
  uint8_t * pHeader,           // IN/OUT: start of data to perform CCM-star on
  uint8_t headerLength,        // IN: Length of header field
  uint8_t messageLength,       // IN: Length of data field
  const uint8_t key[16],       // IN: 128 bit key
  const uint8_t nonce[13],     // IN: 802.15.4/Zigbee specific nonce
  const uint8_t securityLevel, // IN: Security level 0-7
  gCcmDirection_t direction    // IN: Direction of CCM: gCcmEncode_c, gCcmDecode_c
  );


/************************************************************************************
* Calculate C(i) := E(key, A(i) ^ M(i)) for i=1,...,t
*
* Interface assumptions:
*   None
*
* Return value:
*   None
* 
************************************************************************************/
extern void SecLib_Aes(const uint8_t *pData,
                       const uint8_t *pKey,
                       uint8_t *pReturnData);

/************************************************************************************
* Performs AES ECB on a block of bytes.
*
* Interface assumptions:
*   None
*
* Return value:
*   None
* 
************************************************************************************/
extern void SecLib_AesEcb(uint8_t* pData,
                          const uint8_t* pKey,
                          uint8_t* pReturnData,
                          uint8_t blockCount);

/************************************************************************************
* Calculate XOR of individual byte pairs in two uint8_t arrays. I.e.
* pDst[i] := pDst[i] ^ pSrc[i] for i=0 to n-1
*
* Interface assumptions:
*   None
*
* Return value:
*   None
* 
************************************************************************************/
extern void CCM_XorN
  (
  uint8_t *pDst, // First operand and result of XOR operation
  uint8_t *pSrc, // Second operand. Not modified.
  uint8_t n      // Number of bytes in input arrays.
  );

#ifdef __cplusplus
}
#endif

#endif // _SECURITY_LIB_H_
