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
 *   This file implements x.509 operation.
 */

#include "x509.hpp"

namespace ot {
namespace Crypto {

X509::X509()
{
    //mbedtls_x509_crt_init(mbedtls_x509_crt *crt);
}

X509::~X509()
{
    //mbedtls_sha256_free(&mContext);
}

void X509::GetDN(char *aBuf, size_t aSize, const mbedtls_x509_name *aDN)
{
    mbedtls_x509_dn_gets(aBuf, aSize, aDN);
}

int X509::GetSerial(char *aBuf, size_t aSize, const mbedtls_x509_buf *aSerial)
{
    return mbedtls_x509_serial_gets(aBuf, aSize, aSerial);
}

int X509::ParseCert(mbedtls_x509_crt *aChain, const unsigned char *aBuf, size_t aBufLen)
{
    return mbedtls_x509_crt_parse(aChain, aBuf, aBufLen);
}

int X509::VerifyCertSig(mbedtls_x509_crt *aCert, mbedtls_x509_crt *aTrustCa,
                       mbedtls_x509_crl *aCaCrl, const char *aCN, uint32_t *aFlags,
                       int (*aFvrfy)(void *, mbedtls_x509_crt *, int, uint32_t *),
                       void *aPvrfy)
{
    return mbedtls_x509_crt_verify(aCert, aTrustCa, aCaCrl, aCN, aFlags,
                                   aFvrfy,
                                   aPvrfy);

}

int X509::IsCertRevoked(const mbedtls_x509_crt *aCert, const mbedtls_x509_crl *aCrl)
{
    return mbedtls_x509_crt_is_revoked(aCert, aCrl);
}

void X509::InitCert(mbedtls_x509_crt *aCert)
{
    return mbedtls_x509_crt_init(aCert);
}

void X509::FreeCert(mbedtls_x509_crt *aCert)
{
    return mbedtls_x509_crt_free(aCert);
}

}  // namespace Crypto
}  // namespace ot
