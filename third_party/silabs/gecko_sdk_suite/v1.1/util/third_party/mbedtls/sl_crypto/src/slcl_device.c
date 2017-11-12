/*
 *  Silicon Labs CRYPTO/AES device interface.
 *
 *  Copyright (C) 2016, Silicon Labs, http://www.silabs.com
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "slcl_device_internal.h"
#include "em_assert.h"
#include <stdlib.h>

/* Table of pointers to mbedtls device contexts of each CRYPTO instance. */
mbedtls_device_context *p_mbedtls_device_context[MBEDTLS_DEVICE_COUNT] = { 0 };

/* Initialize an mbedtls device context. See slcl_device.h for details. */
void mbedtls_device_init( mbedtls_device_context* ctx )
{
#if defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION)
    SLDP_DeviceInit( &ctx->sldp_dev_ctx );
#endif

#if defined( MBEDTLS_DEVICE_YIELD_WHEN_BUSY )
    EFM_ASSERT(SLPAL_InitCompletion(&ctx->operation) == 0);
#else
    (void) ctx;
#endif
}

/* Clear an mbedtls device context. See slcl_device.h for details. */
void mbedtls_device_free( mbedtls_device_context* ctx )
{
#if defined( MBEDTLS_DEVICE_YIELD_WHEN_BUSY )
    int ret = SLPAL_FreeCompletion(&ctx->operation);
    EFM_ASSERT(ret == 0);
#endif

#if defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION)
    SLDP_DeviceFree( &ctx->sldp_dev_ctx );
#endif
    
    /* Clear pointer to context in device context table. */
    p_mbedtls_device_context[ctx->devno] = 0;
    
    /* Free up device specifics */
    mbedtls_device_specific_free(ctx->devno);
}

/* Set device instance of an mbedtls device context.
   See slcl_device.h for details. */
int mbedtls_device_set_instance( mbedtls_device_context* ctx,
                                 unsigned int devno )
{
    int ret;
  
    if (devno >= MBEDTLS_DEVICE_COUNT)
        return( MBEDTLS_ERR_DEVICE_NOT_SUPPORTED );

    /* Setup device specifics */
    ret = mbedtls_device_specific_init(devno);
    if (ret != 0)
      goto exit;

    /* Store device instance number */
    ctx->devno = devno;

#if defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION)
    SLDP_DeviceHandleSet( &ctx->sldp_dev_ctx,
                          mbedtls_device_handle_get(devno) );
    SLDP_DeviceEnableFuncSet( &ctx->sldp_dev_ctx,
                              mbedtls_device_enable );
    SLDP_DeviceDisableFuncSet( &ctx->sldp_dev_ctx,
                               mbedtls_device_disable );
    SLDP_DeviceContextStoreFuncSet( &ctx->sldp_dev_ctx,
                                    mbedtls_device_context_store );
    SLDP_DeviceContextLoadFuncSet( &ctx->sldp_dev_ctx,
                                   mbedtls_device_context_load );
#endif
    
    /* Store pointer to context in device context table. */
    p_mbedtls_device_context[devno] = ctx;

 exit:
    return( ret );
}

#if defined( MBEDTLS_CRYPTO_DEVICE_PREEMPTION )

/***************************************************************************//**
 * @brief
 *   Get state of a mbedtls device instance.
 *
 * @details
 *   Checks if a CRYPTO/AES device is idle and ready for new operation, or busy
 *   running an ongoing operation.
 *
 * @return
 *   MBEDTLS_ECODE_CRYPTODRV_BUSY if CRYPTO is busy running an ongoing operation.
 *   0 if idle and ready for new operation.
 ******************************************************************************/
int mbedtls_device_state_get( unsigned int devno )
{
  /* The owner pointer indicates whether someone is already running a
     CRYPTO operation. */
  return p_mbedtls_device_context[devno]->sldp_dev_ctx.pOwner ?
    MBEDTLS_ERR_DEVICE_BUSY : 0;
}

#endif
