/************************************************************************************
* This file is owned by the platform/application specific software and basically 
* defines how the 802.15.4 FreeScale MAC is configured. The file is included by the
* relevant MAC modules and is necessary for the MAC to compile.
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

#ifndef _PLATFORMTOMACPHYCONFIG_H_
#define _PLATFORMTOMACPHYCONFIG_H_

#ifdef __cplusplus
    extern "C" {
#endif

  // Specify if placing of execution speed demanding variables in fast memory is 
  // supported. Currently the variables for the AES (sequrity engine) are placed
  // in fast RAM if it is supported.
#define gFastRamSupported_d 0  // Set to 1 to notify the MAC/PHY that fast memory is supported.
#define gFastRamBegin_d
#define gFastRamEnd_d

#define gVersionStringCapability_d 0
#define gVersionStringMacBegin_d
#define gVersionStringMacEnd_d
#define gVersionStringPhyBegin_d
#define gVersionStringPhyEnd_d

#ifdef __cplusplus
}
#endif

#endif /* _PLATFORMTOMACPHYCONFIG_H_ */
