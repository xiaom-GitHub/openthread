/************************************************************************************
* This file holds type definitions that maps the standard c-types into types
* with guaranteed sizes. The types are target/platform specific and must be edited
* for each new target/platform.
*
* The header file also provides definitions for TRUE, FALSE and NULL.
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

#ifndef _EMBEDDEDTYPES_H_
#define _EMBEDDEDTYPES_H_

#ifdef __cplusplus
    extern "C" {
#endif

/************************************************************************************
*
*       TYPE DEFINITIONS
*
************************************************************************************/
#if defined(__GNUC__)
#define PACKED_STRUCT struct __attribute__ ((__packed__))
#define PACKED_UNION  union __attribute__ ((__packed__))
#elif defined(__IAR_SYSTEMS_ICC__)
#define PACKED_STRUCT __packed struct
#define PACKED_UNION __packed union
#else
#define PACKED_STRUCT struct
#define PACKED_UNION union
#endif


typedef signed    char  int8_t;
typedef unsigned  char  uint8_t;
typedef signed    short int16_t;
typedef unsigned  short uint16_t;
typedef signed    long  int32_t;
typedef unsigned  long  uint32_t;
typedef signed    long long int64_t;
typedef unsigned  long long uint64_t;

typedef signed    char  intn8_t;
typedef unsigned  char  uintn8_t;
typedef signed    short intn16_t;
typedef unsigned  short uintn16_t;
typedef signed    long  intn32_t;
typedef unsigned  long  uintn32_t;
typedef signed    long long intn64_t;
typedef unsigned  long long uintn64_t;

typedef volatile signed char vint8_t;
typedef volatile unsigned char vuint8_t;
typedef volatile signed short vint16_t;
typedef volatile unsigned short vuint16_t;
typedef volatile signed long vint32_t;
typedef volatile unsigned long vuint32_t;

/* boolean types */
typedef uint8_t   bool_t;
typedef uintn8_t  booln_t;
/* used for indexing into an array in the most efficient manner for the platform */
typedef uint8_t   index_t;

#define BIT0    0x00000001UL
#define BIT1    0x00000002UL
#define BIT2    0x00000004UL
#define BIT3    0x00000008UL
#define BIT4    0x00000010UL
#define BIT5    0x00000020UL
#define BIT6    0x00000040UL
#define BIT7    0x00000080UL
#define BIT8    0x00000100UL
#define BIT9    0x00000200UL
#define BIT10   0x00000400UL
#define BIT11   0x00000800UL
#define BIT12   0x00001000UL
#define BIT13   0x00002000UL
#define BIT14   0x00004000UL
#define BIT15   0x00008000UL
#define BIT16   0x00010000UL
#define BIT17   0x00020000UL
#define BIT18   0x00040000UL
#define BIT19   0x00080000UL
#define BIT20   0x00100000UL
#define BIT21   0x00200000UL
#define BIT22   0x00400000UL
#define BIT23   0x00800000UL
#define BIT24   0x01000000UL
#define BIT25   0x02000000UL
#define BIT26   0x04000000UL
#define BIT27   0x08000000UL
#define BIT28   0x10000000UL
#define BIT29   0x20000000UL
#define BIT30   0x40000000UL
#define BIT31   0x80000000UL


#define TRUE 1
#define FALSE 0

#ifndef NULL
#define NULL (( void * )( 0 ))
#endif

/* If parameters of CONCAT() are macros, a second level of indirection is required to get them to expand */
#define _CONCAT(t1, t2) t1 ## t2
#define CONCAT(t1, t2) _CONCAT(t1,t2)

typedef uint32_t zbClock32_t;
typedef uint32_t zbClock24_t;
typedef uint16_t zbClock16_t;


#define Copy2Bytes(aVal1, aVal2)    (FLib_MemCpy((void *)(aVal1),(void *) (aVal2), 2))
void Set2Bytes(void *ptr, uint16_t val);
bool_t IsEqual2BytesInt(void *ptr, uint16_t val);
#define TwoBytesToUint16(aVal)      ( ((uint16_t)(*((uint8_t *)(aVal))))|(((uint16_t)(*(((uint8_t *)(aVal))+1)))<<8) )
#define FourBytesToUint32(aVal)      ( ((uint32_t)TwoBytesToUint16(aVal))|(((uint32_t)TwoBytesToUint16(((uint8_t*)aVal)+2))<<16) )
#define TwoBytes2Byte(aVal)         ( (uint8_t)( *( ( uint8_t * )(aVal) ) ) )
#define Cmp2BytesToZero(aVal)       (!( TwoBytesToUint16(aVal) ))
void Inc4Bytes(uint8_t* aVal1, uint32_t Val2);
#define IsEqual2Bytes(aVal1, aVal2) (TwoBytesToUint16(aVal1) == TwoBytesToUint16(aVal2))

/* allow for routines/variables to be static or not at compile-time. */
#define STATIC

/* add assert codes if needed. Calls assert function. */
typedef uint8_t assertCode_t;

void BeeAppAssert(assertCode_t code);

#define gAssertCode_NoTasks_c   0x01  /* not enough tasks. Increase gTsMaxTasks_c */
#define gAssertCode_NoTimers_c  0x02  /* not enough timers. Increase gTmrApplicationTimers_c */

/* indicate endian-ness of this MCU */
#define gBigEndian_c FALSE
#define gMaxIndex_c  0xfe

#ifdef __cplusplus
}
#endif

#endif /* _EMBEDDEDTYPES_H_ */
