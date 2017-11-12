/*
 * Common configuration for Silicon Labs stacks in multiprotocol
 *
 */

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

#include <stddef.h>
// bg_calloc and bg_free are implemented in bgcommon
void bg_free(void *pv);
void *bg_calloc(size_t num, size_t size);

#if !defined( NO_CRYPTO_ACCELERATION )
/* SiliconLabs plugins with CRYPTO acceleration support. */
#define MBEDTLS_SLCL_PLUGINS
#define MBEDTLS_ECP_DEVICE_ALT
#define MBEDTLS_ECP_DOUBLE_JAC_ALT
#define MBEDTLS_ECP_DEVICE_ADD_MIXED_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT
#define MBEDTLS_MPI_MODULAR_DIVISION_ALT
#define MBEDTLS_AES_ALT
#define MBEDTLS_CCM_ALT
#define MBEDTLS_CMAC_ALT
#define MBEDTLS_ECP_ALT
#define MBEDTLS_ECP_GROUP_LOAD_ALT
#define MBEDTLS_CRYPTO_DEVICE_PREEMPTION
#endif

/* mbed TLS modules */
#define MBEDTLS_ECP_C
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECDH_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_CMAC_C
#define MBEDTLS_AES_C
#define MBEDTLS_CCM_C
#define MBEDTLS_CIPHER_MODE_CTR
#undef MBEDTLS_FS_IO
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_CALLOC_MACRO  bg_calloc /**< Default allocator macro to use, can be undefined */
#define MBEDTLS_PLATFORM_FREE_MACRO    bg_free /**< Default free macro to use, can be undefined */

/* Save RAM at the expense of ROM */
#define MBEDTLS_AES_ROM_TABLES

/* Save RAM by adjusting to our exact needs */
#define MBEDTLS_ECP_MAX_BITS   256
#define MBEDTLS_MPI_MAX_SIZE    32 // 384 bits is 48 bytes

/* 
   Set MBEDTLS_ECP_WINDOW_SIZE to configure
   ECC point multiplication window size, see ecp.h:
   2 = Save RAM at the expense of speed
   3 = Improve speed at the expense of RAM
   4 = Optimize speed at the expense of RAM
*/
#define MBEDTLS_ECP_WINDOW_SIZE        2
#define MBEDTLS_ECP_FIXED_POINT_OPTIM  0

/* Significant speed benefit at the expense of some ROM */
#define MBEDTLS_ECP_NIST_OPTIM

/*
 * You should adjust this to the exact number of sources you're using: default
 * is the "mbedtls_platform_entropy_poll" source, but you may want to add other ones.
 * Minimum is 2 for the entropy test suite.
 */
#define MBEDTLS_ENTROPY_MAX_SOURCES 2

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */
