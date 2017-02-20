/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for the non-volatile storage.
 */

#include <openthread-config.h>
#include <platform/alarm.h>
#include <utils/flash.h>
#include <common/code_utils.hpp>
#include <NV_FlashHAL.h>

enum
{
    FLASH_PAGE_SIZE  = 0x800,
    FLASH_BLOCK_SIZE = 0x80000,
};

NvConfig_t mNvConfig =
{
    gNV_FTFX_REG_BASE_c,      /// 0x40020000
    gNV_PFLASH_BLOCK_BASE_c,  /// 0x00000000
    gNV_PFLASH_BLOCK_SIZE_c,  /// 0x00080000
    gNV_DFLASH_BLOCK_BASE_c,  /// 0x00000000
    gNV_DFLASH_BLOCK_SIZE_c,  /// 0x00000000
    gNV_EERAM_BLOCK_BASE_c,   /// 0x00000000
    gNV_EERAM_BLOCK_SIZE_c,   /// 0x00000000
    gNV_EEE_BLOCK_SIZE_c      /// 0x00000000
};

ThreadError utilsFlashInit(void)
{
    NV_FlashInit(&mNvConfig);

    return kThreadError_None;
}

uint32_t utilsFlashGetSize(void)
{
    return FLASH_BLOCK_SIZE;
}

ThreadError utilsFlashErasePage(uint32_t aAddress)
{
    ThreadError error = kThreadError_None;

    VerifyOrExit(aAddress < utilsFlashGetSize(), error = kThreadError_InvalidArgs);

    error = NV_FlashEraseSector(&mNvConfig, aAddress, FLASH_PAGE_SIZE);

exit:
    return error;
}

ThreadError utilsFlashStatusWait(uint32_t aTimeout)
{
    ThreadError error = kThreadError_None;
    uint32_t start = otPlatAlarmGetNow();
    bool busy = true;

    while (busy && ((otPlatAlarmGetNow() - start) < aTimeout))
    {
        busy = gNV_REG_BIT_TEST(mNvConfig.ftfxRegBase + gNV_FSTAT_OFFSET_c, gNV_FSTAT_CCIF_c);
    }

    VerifyOrExit(!busy, error = kThreadError_Busy);

exit:
    return error;
}

uint32_t utilsFlashWrite(uint32_t aAddress, uint8_t *aData, uint32_t aSize)
{
    uint32_t rval = 0;
    VerifyOrExit(aData, ;);
    VerifyOrExit(aAddress < utilsFlashGetSize(), ;);

    rval = NV_FlashProgramLongword(&mNvConfig, aAddress, aSize, (uint32_t)(aData));

exit:
    return rval;
}

uint32_t utilsFlashRead(uint32_t aAddress, uint8_t *aData, uint32_t aSize)
{
    uint32_t rval = 0;
    VerifyOrExit(aData, ;);
    VerifyOrExit(aAddress < utilsFlashGetSize(), ;);

    NV_FlashRead(aAddress, aData, aSize);
    rval = aSize;

exit:
    return rval;
}
