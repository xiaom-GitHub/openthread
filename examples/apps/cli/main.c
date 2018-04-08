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

#include <stddef.h>
#include <string.h>

#include <assert.h>
#include <openthread-core-config.h>
#include <openthread/config.h>

#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/openthread.h>
#include <openthread/platform/logging.h>

#include "openthread/instance.h"
#include "openthread/message.h"
#include "openthread/ip6.h"
#include "openthread/coap.h"
//#include "openthread/platform/platform.h"
#include "openthread/platform/settings.h"
#include "openthread/platform/gpio.h"
#include "utils/code_utils.h"

#include "platform.h"

otInstance *sInstance;

#if OPENTHREAD_EXAMPLES_EFR32 || OPENTHREAD_EXAMPLES_CC2538 || OPENTHREAD_EXAMPLES_KW41Z
#define SWAP32(x) x = (uint32_t)((x >> 24 & 0xff) | (x >> 8 & 0xff00) | \
        (x << 8 & 0xff0000) | (x << 24 & 0xff000000))

enum
{
    otCoapTokenMaxLength = 2,
    otCoapUriMaxLength   = 32,
    otCoapDemoServerPort = 25683,
    otDemoIpAddrBuffSize = 80,
};

bool isAddressFirstAdded = false;
uint32_t mHeartBeatCycle = 2000; // 2s
char mCoapServerIpString[] = "64:ff9b::68c5:c52"; //104.197.12.82

void OTCALL HandleServerResponse(void *aContext, otCoapHeader *aHeader, otMessage *aMessage,
        otMessageInfo *aMessageInfo, otError aResult);
//void HandleTimer(otInstance *aInstance);

bool hasGlobalAddr = false;
#if OPENTHREAD_EXAMPLES_EFR32
void HandleGpioInterrupt(uint8_t pin);
#else
void HandleGpioInterrupt(void);
#endif

otError SendCoapHeartBeat(void);
//void StartHeartBeatTimer(uint32_t aHeartBeatCycle);
//void StopHeartBeatTimer(void);

#if OPENTHREAD_EXAMPLES_EFR32
void HandleGpioInterrupt(uint8_t pin)
{
     (void)pin;
     if (hasGlobalAddr)
     {
         SendCoapHeartBeat();
     }
     else
     {
         otPlatLog(OT_LOG_LEVEL_CRIT, OT_LOG_REGION_API, "No global Addr, abort sending \r\n");
     }

     otPlatGpioIntClear(INTERRUPT_PORT, INTERRUPT_PIN);
}
#else
void HandleGpioInterrupt(void)
{
     if (hasGlobalAddr)
     {
         SendCoapHeartBeat();
     }
     else
     {
         otPlatLog(OT_LOG_LEVEL_CRIT, OT_LOG_REGION_API, "No global Addr, abort sending \r\n");
     }

     otPlatGpioIntClear(INTERRUPT_PORT, INTERRUPT_PIN);
}
#endif

otError SendCoapHeartBeat(void)
{
     otError error = OT_ERROR_NONE;
     otMessage *message = NULL;
     otMessageInfo messageInfo;
     otCoapHeader header;
     uint8_t ipBuf[otDemoIpAddrBuffSize];
     uint8_t ipAddressNum = 0;
     const otNetifAddress *cur = NULL;

     char coapUri[otCoapUriMaxLength] = "hb";
     otCoapType coapType = OT_COAP_TYPE_CONFIRMABLE;
     otCoapCode coapCode = OT_COAP_CODE_POST;
     otIp6Address coapDestinationIp;

     otCoapHeaderInit(&header, coapType, coapCode);
     otCoapHeaderGenerateToken(&header, otCoapTokenMaxLength);
     otEXPECT_ACTION(otCoapHeaderAppendUriPathOptions(&header, coapUri) == OT_ERROR_NONE,
                     error = OT_ERROR_NO_BUFS);
     otCoapHeaderSetPayloadMarker(&header);

     message = otCoapNewMessage(sInstance, &header);
     otEXPECT_ACTION(message != NULL, error = OT_ERROR_NO_BUFS);

     uint8_t *ptr = ipBuf;
     ipAddressNum = (otThreadGetDeviceRole(sInstance) == OT_DEVICE_ROLE_LEADER) ? 5 : 4;

     for (cur = otIp6GetUnicastAddresses(sInstance); cur; cur = cur->mNext)
     {
         memcpy(ptr, cur->mAddress.mFields.m8, OT_IP6_ADDRESS_SIZE);
         ptr += OT_IP6_ADDRESS_SIZE;
     }

     otEXPECT_ACTION(otMessageAppend(message, ipBuf, OT_IP6_ADDRESS_SIZE * ipAddressNum) == OT_ERROR_NONE,
                     error = OT_ERROR_NO_BUFS);

     otEXPECT_ACTION(otIp6AddressFromString(mCoapServerIpString, &coapDestinationIp) == OT_ERROR_NONE,
                     error = OT_ERROR_NO_BUFS);

     memset(&messageInfo, 0, sizeof(messageInfo));
     messageInfo.mPeerAddr = coapDestinationIp;
     messageInfo.mPeerPort = otCoapDemoServerPort;
     messageInfo.mInterfaceId = OT_NETIF_INTERFACE_ID_THREAD;

     error = otCoapSendRequest(sInstance, message, &messageInfo, (otCoapResponseHandler)HandleServerResponse, NULL);

     otPlatLog(OT_LOG_LEVEL_CRIT, OT_LOG_REGION_API, "Sending CoAP message: \r\n");

exit:
     if ((error != OT_ERROR_NONE) && (message != NULL))
     {
         otMessageFree(message);
     }
     return error;
}

void OTCALL HandleServerResponse(void *aContext, otCoapHeader *aHeader, otMessage *aMessage,
                                  otMessageInfo *aMessageInfo, otError aResult)
{
     (void)aContext;
     otError error = OT_ERROR_NONE;
     otCoapHeader responseHeader;
     otMessage *responseMessage;
     otCoapCode responseCode = OT_COAP_CODE_EMPTY;
     char responseContent = '0';

     if (aResult != OT_ERROR_NONE)
     {
         otPlatLog(OT_LOG_LEVEL_CRIT, OT_LOG_REGION_API, "Error: Received CoAP response: %d", aResult);
         goto exit;
     }
     else
     {
         otPlatLog(OT_LOG_LEVEL_CRIT, OT_LOG_REGION_API, "Received CoAP response successfully ");
     }

#if 0
     otPlatLog(OPENTHREAD_LOG_LEVEL_INFO, kLogRegionApi,
               "Received CoAP response from [%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x]: ",
               aMessageInfo->mSockAddr.mFields.m8[15]);
#endif

     // fetch the payload
     uint32_t heartBeatCycle = 0;
     uint16_t length = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);
     otMessageRead(aMessage, otMessageGetOffset(aMessage), (uint8_t *)&heartBeatCycle, length);
     
     SWAP32(heartBeatCycle);
     
     otPlatLog(OT_LOG_LEVEL_CRIT, OT_LOG_REGION_API, "Received Heart Beat Cycle: [%x]ms \r\n", heartBeatCycle);
     otPlatLog(OT_LOG_LEVEL_CRIT, OT_LOG_REGION_API, "Received Heart Beat Cycle: [%x]s \r\n", (heartBeatCycle / 1000));
     
     if (heartBeatCycle > 0)
     {
         mHeartBeatCycle = heartBeatCycle;
     }

//#ifdef OPENTHREAD_EXAMPLES_EFR32
//     StopHeartBeatTimer();
//     StartHeartBeatTimer(mHeartBeatCycle);
//#endif

     if ((otCoapHeaderGetType(aHeader) == OT_COAP_TYPE_CONFIRMABLE) || otCoapHeaderGetCode(aHeader) == OT_COAP_CODE_GET)
     {
         if (otCoapHeaderGetCode(aHeader) == OT_COAP_CODE_GET)
         {
             responseCode = OT_COAP_CODE_CONTENT;
         }
         else
         {
             responseCode = OT_COAP_CODE_VALID;
         }

         otCoapHeaderInit(&responseHeader, OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode);
         otCoapHeaderSetMessageId(&responseHeader, otCoapHeaderGetMessageId(aHeader));

         otCoapHeaderSetToken(&responseHeader, otCoapHeaderGetToken(aHeader), otCoapHeaderGetTokenLength(aHeader));

         if (otCoapHeaderGetCode(aHeader) == OT_COAP_CODE_GET)
         {
             otCoapHeaderSetPayloadMarker(&responseHeader);
         }

         responseMessage = otCoapNewMessage(sInstance, &responseHeader);
         otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);

         if (otCoapHeaderGetCode(aHeader) == OT_COAP_CODE_GET)
         {
             otEXPECT_ACTION((otMessageAppend(responseMessage, &responseContent, sizeof(responseContent)) == OT_ERROR_NONE),
                             error = OT_ERROR_NO_BUFS);
         }

         otEXPECT_ACTION((otCoapSendResponse(sInstance, responseMessage, aMessageInfo)) == OT_ERROR_NONE,
                         error = OT_ERROR_NO_BUFS);
     }

exit:
     if (error != OT_ERROR_NONE && responseMessage != NULL)
     {
         otPlatLog(OT_LOG_LEVEL_CRIT, OT_LOG_REGION_API, "Cannot send CoAP response message: Error %d\r\n", error);
         otMessageFree(responseMessage);
     }
     else
     {
         if (responseCode >= OT_COAP_CODE_RESPONSE_MIN)
         {
             otPlatLog(OT_LOG_LEVEL_CRIT, OT_LOG_REGION_API, "CoAP response sent successfully!\r\n");
         }
     }
}

void OTCALL HandleNetifStateChanged(uint32_t aFlags, void *aContext)
{
     (void)aContext;

     const otNetifAddress *cur = NULL;

     otEXPECT(otThreadGetDeviceRole(sInstance) != OT_DEVICE_ROLE_DISABLED);

     if ((aFlags & OT_CHANGED_IP6_ADDRESS_ADDED) != 0)
     {
         for (cur = otIp6GetUnicastAddresses(sInstance); cur; cur = cur->mNext)
         {
#if 1
             otPlatLog(OT_LOG_LEVEL_CRIT, OT_LOG_REGION_API,
                       "IPv6 Address: [%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x] \r\n",
                       cur->mAddress.mFields.m8[15]);
#endif

             if (cur->mAddress.mFields.m8[0] == 0xfd && cur->mAddress.mFields.m8[1] == 0x11 &&
                 cur->mAddress.mFields.m8[2] == 0x00 && cur->mAddress.mFields.m8[3] == 0x22)
             {
                 if (!isAddressFirstAdded)
                 {
                     // global IPv6 address has beed added
                     // send CoAP Heart beat to server
//#ifdef OPENTHREAD_EXAMPLES_EFR32
//                     SendCoapHeartBeat();
//                     StartHeartBeatTimer(mHeartBeatCycle);
//#else
                     hasGlobalAddr = true;
//#endif
                     isAddressFirstAdded = true;
                     goto exit;
                 }
             }
         }

         if (!isAddressFirstAdded)
         {
             otPlatLog(OT_LOG_LEVEL_CRIT, OT_LOG_REGION_API, "No Available global IP6 Address\r\n");
         }
     }

exit:
     return;
}
#endif

#if OPENTHREAD_ENABLE_MULTIPLE_INSTANCES
void *otPlatCAlloc(size_t aNum, size_t aSize)
{
    return calloc(aNum, aSize);
}

void otPlatFree(void *aPtr)
{
    free(aPtr);
}
#endif

void otTaskletsSignalPending(otInstance *aInstance)
{
    (void)aInstance;
}

int main(int argc, char *argv[])
{
    //otInstance *sInstance;

#if OPENTHREAD_ENABLE_MULTIPLE_INSTANCES
    size_t   otInstanceBufferLength = 0;
    uint8_t *otInstanceBuffer       = NULL;
#endif

pseudo_reset:

    PlatformInit(argc, argv);

    //otInstanceFactoryReset(sInstance);
    //otPlatSettingsWipe(sInstance);

#if OPENTHREAD_ENABLE_MULTIPLE_INSTANCES
    // Call to query the buffer size
    (void)otInstanceInit(NULL, &otInstanceBufferLength);

    // Call to allocate the buffer
    otInstanceBuffer = (uint8_t *)malloc(otInstanceBufferLength);
    assert(otInstanceBuffer);

    // Initialize OpenThread with the buffer
    sInstance = otInstanceInit(otInstanceBuffer, &otInstanceBufferLength);
#else
    sInstance = otInstanceInitSingle();
#endif
    assert(sInstance);

    otCliUartInit(sInstance);

#if OPENTHREAD_ENABLE_DIAG
    otDiagInit(sInstance);
#endif

#if OPENTHREAD_EXAMPLES_EFR32 || OPENTHREAD_EXAMPLES_CC2538 || OPENTHREAD_EXAMPLES_KW41Z
     // register netif changed callback
     otSetStateChangedCallback(sInstance, HandleNetifStateChanged, NULL);

     // register gpio interrupt
     otPlatGpioIntEnable(INTERRUPT_PORT, INTERRUPT_PIN);
     otPlatGpioRegisterCallback(INTERRUPT_PORT, HandleGpioInterrupt);
#endif

    // start thread network interface
    otIp6SetEnabled(sInstance, true);
    
    // start thread stack
    otThreadSetEnabled(sInstance, true);

    while (!PlatformPseudoResetWasRequested())
    {
        otTaskletsProcess(sInstance);
        PlatformProcessDrivers(sInstance);
    }

    otInstanceFinalize(sInstance);
#if OPENTHREAD_ENABLE_MULTIPLE_INSTANCES
    free(otInstanceBuffer);
#endif

    goto pseudo_reset;

    return 0;
}

/*
 * Provide, if required an "otPlatLog()" function
 */
#if OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_APP
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    OT_UNUSED_VARIABLE(aLogLevel);
    OT_UNUSED_VARIABLE(aLogRegion);
    OT_UNUSED_VARIABLE(aFormat);

    va_list ap;
    va_start(ap, aFormat);
    otCliPlatLogv(aLogLevel, aLogRegion, aFormat, ap);
    va_end(ap);
}
#endif
