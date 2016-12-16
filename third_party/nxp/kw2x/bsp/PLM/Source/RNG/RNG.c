/**************************************************************************
* Filename: RNG.c
*
* Description: RNG implementation file for ARM CORTEX-M4 processor
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
#include "RNG_Interface.h"
#include "MMCAU_Interface.h"

#ifndef gRNG_HwSupport_d
#define gRNG_HwSupport_d 0
#endif

#if (gRNG_HwSupport_d == 1)
#if !(defined(MCU_MK21DN512) || defined (MCU_MK21DX256) || defined(MCU_MK60D10) || defined(MCU_MK60N512VMD100))
#warning The RNG module is not supported on this processor!
#endif
#else /* gRNG_HwSupport_d == 1 */
extern void PhyGetRandomNo(uint32_t *pRandomNo);
static uint32_t mRandomNumber;
#endif


/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define mPRNG_NoOfBits_c      (160)
#define mPRNG_NoOfBytes_c     (mPRNG_NoOfBits_c/8)
#define mPRNG_NoOfLongWords_c (mPRNG_NoOfBits_c/32)

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static uint32_t XKEY[mPRNG_NoOfLongWords_c];
static uint32_t mPRNG_Requests = gRngMaxRequests_d;

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/******************************************************************************
 * Name: RNG_Init()
 * Description: Initialize the RNG HW module
 * Parameter(s): -
 * Return: Status of the RNG module
 ******************************************************************************/
uint8_t RNG_Init()
{
#if (gRNG_HwSupport_d == 1)

    /* Start module clock */
#if (defined(MCU_MK21DN512) || defined(MCU_MK21DX256))
    SIM_SCGC6 |= SIM_SCGC6_RNGA_MASK;
#elif defined(MCU_MK60D10)
    SIM_SCGC3 |= SIM_SCGC3_RNGA_MASK;
#elif defined(MCU_MK60N512VMD100)
    SIM_SCGC3 |= SIM_SCGC3_RNGB_MASK;
#endif

#if (defined(MCU_MK60D10) || defined(MCU_MK21DN512) || defined(MCU_MK21DX256))
    /* Mask Interrupts and start RNG */
    RNG_CR = RNG_CR_INTM_MASK | RNG_CR_HA_MASK | RNG_CR_GO_MASK;
#elif defined(MCU_MK60N512VMD100)
    /* Execute a SW Reset */
    RNG_CMD |= RNG_CMD_SR_MASK;

    /* Mask Interrupts */
    RNG_CR = RNG_CR_MASKDONE_MASK | RNG_CR_MASKERR_MASK;

    /* Start Self Test and Seed Generation */
    RNG_CMD = RNG_CMD_ST_MASK | RNG_CMD_GS_MASK;

    /* Wait for Self Test and Seed Generation to be done*/
    while (RNG_CMD & (RNG_CMD_ST_MASK | RNG_CMD_GS_MASK));

    /* Enable RNG Auto-Reseed */
    RNG_CR |= RNG_CR_AR_MASK;

    /* Check for Errors */
    if (RNG_SR & RNG_SR_ERR_MASK)
    {
        return (uint8_t)(RNG_ESR);
    }

#endif
#else /* gRNG_HwSupport_d == 1 */

    PhyGetRandomNo(&mRandomNumber);

#endif /* gRNG_HwSupport_d == 1 */

    /* Init Successfull */
    return gRngSuccess_d;
}


/******************************************************************************
 * Name: RNG_HwGetRandomNo()
 * Description: Read a random number from the HW RNG module
 * Parameter(s): [OUT] pRandomNo - pointer to location where the RN will be stored
 * Return: status of the RNG module
 ******************************************************************************/
#if gRNG_HwSupport_d
static uint8_t RNG_HwGetRandomNo(uint32_t *pRandomNo)
{
    /* Check for NULL pointers */
    if (NULL == pRandomNo)
    {
        return gRngNullPointer_d;
    }

#if (defined(MCU_MK60D10) || defined(MCU_MK21DN512) || defined(MCU_MK21DX256))

    /* If output register is empty, wait for a new random number */
    while (((RNG_SR & RNG_SR_OREG_LVL_MASK) >> RNG_SR_OREG_LVL_SHIFT) == 0);

    /* Copy the output of RNG module */
    *pRandomNo = RNG_OR;

#elif defined(MCU_MK60N512VMD100)

    /* Check for Errors */
    if (RNG_SR & RNG_SR_ERR_MASK)
    {
        return (uint8_t)(RNG_ESR);
    }

    /* If output FIFO is empty, wait for a new random number */
    while (((RNG_SR & RNG_SR_FIFO_LVL_MASK) >> RNG_SR_FIFO_LVL_SHIFT) == 0);

    /* Copy the output of RNG module */
    *pRandomNo = RNG_OUT;
#endif

    return gRngSuccess_d;
}
#endif /* gRNG_HwSupport_d */


/******************************************************************************
 * Name: RNG_GetRandomNo()
 * Description: Read a random number from RNG module or from 802.15.4 PHY
 * Parameter(s): [OUT] pRandomNo - pointer to location where the RN will be stored
 * Return: none
 ******************************************************************************/
void RNG_GetRandomNo(uint32_t *pRandomNo)
{
#if (gRNG_HwSupport_d == 1)
    (void)RNG_HwGetRandomNo(pRandomNo);
#else
    mRandomNumber = (mRandomNumber * 6075) + 1283;
    *pRandomNo = mRandomNumber;
#endif
}

/******************************************************************************
 * Name: RNG_SetPseudoRandomNoSeed()
 * Description: Initialize seed for the PRNG algorithm.
 * Parameter(s):
 *      pSeed - pointer to a buffer containing 20 bytes (160 bits).
 *             Can be set using the RNG_GetRandomNo() function.
 * Return: None
 ******************************************************************************/
void RNG_SetPseudoRandomNoSeed(uint8_t *pSeed)
{
    uint8_t i;

    mPRNG_Requests = 1;

    for (i = 0; i < mPRNG_NoOfBytes_c; i++)
    {
        ((uint8_t *)XKEY)[i] = pSeed[i];
    }
}

/******************************************************************************
 * Name: RNG_GetRandomNo()
 *
 * Description: Pseudo Random Number Generator (PRNG) implementation
 *              according to NIST FIPS Publication 186-2, APPENDIX 3
 *
 * Let x be the signer's private key.  The following may be used to generate m values of x:
 *   Step 1. Choose a new, secret value for the seed-key, XKEY.
 *   Step 2. In hexadecimal notation let
 *     t = 67452301 EFCDAB89 98BADCFE 10325476 C3D2E1F0.
 *     This is the initial value for H0 || H1 || H2 || H3 || H4 in the SHS.
 *   Step 3. For j = 0 to m - 1 do
 *     a. XSEEDj = optional user input.
 *     b. XVAL = (XKEY + XSEEDj) mod 2^b
 *     c. xj = G(t,XVAL) mod q
 *     d. XKEY = (1 + XKEY + xj) mod 2^b
 *
 * Parameter(s):
 *      pOut - pointer to the output buffer
 *      outBytes - the number of bytes to be copyed (1-20)
 *      pXSEED - optional user SEED. Should be NULL if not used.
 *
 * Return: The number of bytes copied or -1 if reseed is needed
 ******************************************************************************/
int16_t RNG_GetPseudoRandomNo(uint8_t *pOut, uint8_t outBytes, uint8_t *pXSEED)
{
    uint32_t i;
    uint8_t M[64];
    uint32_t out[mPRNG_NoOfLongWords_c];

    if (mPRNG_Requests == gRngMaxRequests_d)
    {
        return -1;
    }
    else
    {
        mPRNG_Requests++;
    }

    /* a. XSEEDj = optional user input. */
    if (pXSEED)

        /* b. XVAL = (XKEY + XSEEDj) mod 2^b */
        for (i = 0; i < 20; i++)
        {
            M[i] = ((uint8_t *)XKEY)[i] + pXSEED[i];
        }
    else
        for (i = 0; i < 20; i++)
        {
            M[i] = ((uint8_t *)XKEY)[i];
        }

    /* c. xj = G(t,XVAL) mod q
    ***************************/
    /* The first b (160) bits of M contain c (XVAL),
       and the remaining bits (512-160) are cleared */
    for (i = 20; i < 64; i++)
    {
        M[i] = 0;
    }

    /* Prepare block for SHA1 */
    M[20] = 0x80; // append 1 bit to the end of the message
    M[63] = 160;  // add the size of the message (in bits)

    /* Apply SHA1 */
#if gMMCAU_Support_d
    mmcau_sha1_initialize_output(out);
    mmcau_sha1_hash(M, out);
    /* OR
    mmcau_sha1_update ((uint8_t*)M, 1, (uint32_t*)res); */
#else
    sw_sha1_initialize_output(out);
    sw_sha1_hash(M, out);
    /* OR
    sw_sha1_update ((uint8_t*)M, 1, (uint32_t*)res); */
#endif

    /* d. XKEY = (1 + XKEY + xj) mod 2^b */
    XKEY[0] += 1;

    for (i = 0; i < mPRNG_NoOfLongWords_c; i++)
    {
        XKEY[i] += out[i];
    }

    /* Check if the length provided exceeds the output data size */
    if (outBytes > mPRNG_NoOfBytes_c)
    {
        outBytes = mPRNG_NoOfBytes_c;
    }

    /* Copy the generated number */
    for (i = 0; i < outBytes; i++)
    {
        pOut[i] = ((uint8_t *)out)[i];
    }

    return outBytes;
}
/********************************** EOF ***************************************/
