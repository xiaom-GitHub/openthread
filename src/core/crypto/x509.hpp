/*
 *  Copyright (c) 2017, The OpenThread Authors.
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
 *   This file includes definitions for implementation for x.509 certificate.
 */

#ifndef X509_HPP_
#define X509_HPP_

#include <mbedtls/x509.h>
#include <mbedtls/x509_crt.h>

namespace ot {
namespace Crypto {

/**
 * @addtogroup core-security
 *
 * @{
 *
 */

/**
 * This class implements x.509 parsing.
 *
 */
class X509
{
public:

    /**
     * Constructor for initializing mbedtls_sha256_context.
     *
     */
    X509();

    /**
     * Destructor for freeing mbedtls_sha256_context.
     *
     */
    ~X509();

    /**
     * This method gets issuer (distinguished name) of the certificate.
     *
     */
    // mbedtls_x509_dn_gets( char *buf, size_t size, const mbedtls_x509_name *dn );
    void GetDN(char *aBuf, size_t aSize, const mbedtls_x509_name *aDN);

    /**
     * This method get the serial number of the certificate..
     *
     * @param[in]  aBuf        A pointer to the input buffer.
     * @param[in]  aBufLength  The length of @p aBuf in bytes.
     *
     */
     int GetSerial(char *aBuf, size_t aSize, const mbedtls_x509_buf *aSerial);

    /**
     * This method parses a der certificate and store to the certificate chain..
     *
     * @param[out]  aHash  A pointer to the output buffer.
     *
     */
     int ParseCert(mbedtls_x509_crt *aChain, const uint8_t *aBuf, size_t aBufLen);

     int VerifyCertSig(mbedtls_x509_crt *aCert, mbedtls_x509_crt *aTrustCa,
                      mbedtls_x509_crl *aCaCrl, const char *aCN, uint32_t *aFlags,
                      int (*aFvrfy)(void *, mbedtls_x509_crt *, int, uint32_t *),
                      void *aPvrfy );

     //int VerifyCrtSigProfile(mbedtls_x509_crt *crt, mbedtls_x509_crt *trust_ca,

     int IsCertRevoked(const mbedtls_x509_crt *aCert, const mbedtls_x509_crl *aCrl);

     void InitCert(mbedtls_x509_crt *aCert);

     void FreeCert(mbedtls_x509_crt *aCert);
private:
};

/**
 * @}
 *
 */

}  // namespace Crypto
}  // namespace ot

#endif  // X509_HPP_
