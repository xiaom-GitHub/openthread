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
 *   This file implements the OpenThread platform abstraction for radio communication.
 *
 */

#include <platform/alarm.h>
#include <platform/radio.h>
#include <common/code_utils.hpp>
#include <common/debug.hpp>
#include <common/logging.hpp>

#include "platform-kw2x.h"
#include <Phy.h>
#include <MC1324xDrv.h>
#include <MC1324xReg.h>
#include <Interrupt.h>

static PhyState sState = kStateDisabled;
static phyPacket_t pReceiveFrame;
static phyPacket_t pTransmitFrame;
static phyRxParams_t pRxParams;

static RadioPacket sReceiveFrame;
static RadioPacket sTransmitFrame;
static ThreadError sTransmitError = kThreadError_None;
static ThreadError sReceiveError = kThreadError_None;

static bool sIsReceiverEnabled = false;
static bool sIsPromiscuousEnabled = false;
static bool sReceiveFrameDone = false;
static bool sTransmitFrameDone = false;
static uint8_t sChannel = 0;

extern void delayMs(uint16_t val);

enum
{
    IEEE802154_MIN_LENGTH = 5,
    IEEE802154_MAX_LENGTH = 127,
    IEEE_EUI64 = 0x00280028, //TODO:use NXP EUI
    IEEE802154_ACK_REQUEST = 1 << 5,
};

enum
{
    kmClkSwitchDelayTime = 50,
    kmRstBAssertTime = 50,
};

#if 0
static void PhyResetSequenceStatus(void)
{
    uint8_t phyReg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL1);

    // Abort current SEQ
    phyReg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
    MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL1, phyReg);

    // wait for Sequence Idle
    while ((MC1324xDrv_DirectAccessSPIRead(SEQ_STATE) & 0x1F) != 0);
}
#endif

static void PhyDoze(void)
{
    uint8_t phyPwrModesReg = 0;
    phyPwrModesReg = MC1324xDrv_DirectAccessSPIRead(PWR_MODES);
    phyPwrModesReg |= (0x10);  /* XTALEN = 1 */
    phyPwrModesReg &= (0xFE);  /* PMC_MODE = 0 */
    MC1324xDrv_DirectAccessSPIWrite(PWR_MODES, phyPwrModesReg);
}

ThreadError startReceiveSequence(uint8_t aChannel)
{
    ThreadError error = kThreadError_None;

    // start R sequence:
    // 1. beginning
    // 2. finished T sequence
    // 3. finished R sequence
    if (!sIsReceiverEnabled)// || (sState == kStateReceive && sTransmitFrameDone))
    {
        //assert(state == gIdle_c || state == gRX_c);
        if (PhyGetSeqState() != gIdle_c)
        {
            //PhyResetSequenceStatus();
            PhyPlmeForceTrxOffRequest();
        }

        VerifyOrExit(PhyPlmeSetCurrentChannelRequestPAN0(aChannel) == gPhySuccess_c, error = kThreadError_Busy);
        VerifyOrExit(PhyPlmeRxRequest(&pReceiveFrame, 0, &pRxParams) == gPhySuccess_c,
                     error = kThreadError_Busy);

        //if (!sIsReceiverEnabled)
        //{
        sIsReceiverEnabled = true;
        otLogInfoPlat("Enabling receiver", NULL);
        //}
    }

exit:
    return error;
}

void otPlatRadioSetPanId(otInstance *aInstance, uint16_t aPanId)
{
    (void)aInstance;

    otLogInfoPlat("PANID=%X", aPanId);

    uint8_t buf[2];
    buf[0] = aPanId >> 0;
    buf[1] = aPanId >> 8;
    PhyPpSetPanIdPAN0(buf);
}

void otPlatRadioSetExtendedAddress(otInstance *aInstance, uint8_t *aExtendedAddress)
{
    (void)aInstance;

    otLogInfoPlat("ExtAddr=%X%X%X%X%X%X%X%X",
                  aExtendAddress[7], aExtendAddress[6], aExtendAddress[5], aExtendAddress[4],
                  aExtendAddress[3], aExtendAddress[2], aExtendAddress[1], aExtendAddress[0]);

    PhyPpSetLongAddrPAN0(aExtendedAddress);
}

void otPlatRadioSetShortAddress(otInstance *aInstance, uint16_t aShortAddress)
{
    (void)aInstance;

    otLogInfoPlat("ShortAddr=%X", aShortAddress);

    uint8_t buf[2];
    buf[0] = aShortAddress >> 0;
    buf[1] = aShortAddress >> 8;
    PhyPpSetShortAddrPAN0(buf);
}

void kw2xTransceiverInit(void)
{
    /* Initialize the transceiver SPI driver */
    MC1324xDrv_SPIInit();
    /* Configure the transceiver IRQ_B port */
    MC1324xDrv_IRQ_PortConfig();

    /* Configure the transceiver RST_B port */
    MC1324xDrv_RST_B_PortConfig();
    /* Transceiver Hard/RST_B RESET */
    MC1324xDrv_RST_B_Assert();
    delayMs(kmRstBAssertTime);
    MC1324xDrv_RST_B_Deassert();

    /* Wait for transceiver to deassert IRQ pin */
    while (MC1324xDrv_IsIrqPending());

    /* Wait for transceiver wakeup from POR iterrupt */
    while (!MC1324xDrv_IsIrqPending());

    /* Enable transceiver SPI interrupt request */
    NVIC_EnableIRQ(MC1324x_Irq_Number);
    NVIC_SetPriority(MC1324x_Irq_Number, MC1324x_Irq_Priority);

    /* Enable the transceiver IRQ_B interrupt request */
    MC1324xDrv_IRQ_Enable();

    MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_4_MHz);

    /* wait until the external reference clock is stable */
    delayMs(kmClkSwitchDelayTime);
}

void kw2xRadioInit(void)
{
    sTransmitFrame.mLength = 0;
    sTransmitFrame.mPsdu = pTransmitFrame.data;
    sReceiveFrame.mLength = 0;
    sReceiveFrame.mPsdu = pReceiveFrame.data;

    kw2xTransceiverInit();
    PhyInit();
    PhyPlmeSetLQIModeRequest(1);  // LQI Based on RSSI

    otLogInfoPlat("Initialized", NULL);
}

void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
    uint8_t *eui64 = (uint8_t *)IEEE_EUI64;
    (void)aInstance;

    for (uint8_t i = 0; i < OT_EXT_ADDRESS_SIZE; i++)
    {
        aIeeeEui64[i] = eui64[7 - i];
    }
}

bool otPlatRadioIsEnabled(otInstance *aInstance)
{
    (void)aInstance;
    return (sState != kStateDisabled) ? true : false;
}

ThreadError otPlatRadioEnable(otInstance *aInstance)
{
    ThreadError error = kThreadError_None;

    if (!otPlatRadioIsEnabled(aInstance))
    {
        MC1324xDrv_IRQ_Disable();
        VerifyOrExit(sState == kStateDisabled, error = kThreadError_Busy);
        sState = kStateSleep;
        otLogDebgPlat("State=kStateSleep", NULL);
    }

exit:
    MC1324xDrv_IRQ_Enable();
    return error;
}

ThreadError otPlatRadioDisable(otInstance *aInstance)
{
    if (otPlatRadioIsEnabled(aInstance))
    {
        MC1324xDrv_IRQ_Disable();
        PhyAbort();
        PhyDoze();
        otLogDebgPlat("State=kStateDisabled", NULL);
        sState = kStateDisabled;
        MC1324xDrv_IRQ_Enable();
    }

    return kThreadError_None;
}

ThreadError otPlatRadioSleep(otInstance *aInstance)
{
    ThreadError error = kThreadError_None;
    (void)aInstance;

    MC1324xDrv_IRQ_Disable();
    VerifyOrExit(sState == kStateSleep || sState == kStateReceive, error = kThreadError_Busy);
    PhyDoze();
    otLogDebgPlat("State=kStateSleep", NULL);
    sState = kStateSleep;
    sIsReceiverEnabled = false;

exit:
    MC1324xDrv_IRQ_Enable();
    return error;
}

ThreadError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    ThreadError error = kThreadError_None;
    (void)aInstance;

    MC1324xDrv_IRQ_Disable();

    if (sState != kStateDisabled)
    {
        sState = kStateReceive;
        error = kThreadError_None;
        sChannel = aChannel;
        sReceiveFrame.mChannel = aChannel;

        VerifyOrExit(startReceiveSequence(aChannel) == kThreadError_None, error = kThreadError_Busy);

        otLogDebgPlat("State=kStateReceive", NULL);
    }

exit:
    MC1324xDrv_IRQ_Enable();
    return error;
}

bool radioPacketGetAckRequest(RadioPacket *aPacket)
{
    return (aPacket->mPsdu[0] & IEEE802154_ACK_REQUEST) != 0;
}

uint8_t GetPhyTxMode(RadioPacket *aPacket)
{
    return radioPacketGetAckRequest(aPacket) ?
           gDataReq_Ack_Cca_Unslotted_c : gDataReq_NoAck_Cca_Unslotted_c;
}

ThreadError otPlatRadioTransmit(otInstance *aInstance, RadioPacket *aPacket)
{
    ThreadError error = kThreadError_None;
    (void)aInstance;

    MC1324xDrv_IRQ_Disable();

    VerifyOrExit(sState == kStateReceive, error = kThreadError_Busy);

    if (PhyGetSeqState() != gIdle_c && sIsReceiverEnabled)
    {
         PhyPlmeForceTrxOffRequest();
    }

    //VerifyOrExit(aPacket->mLength <= IEEE802154_MAX_LENGTH - 2, error = kThreadError_InvalidArgs);
    sState = kStateTransmit;
    sTransmitFrame = *aPacket;
    pTransmitFrame.frameLength = aPacket->mLength;
    memcpy(pTransmitFrame.data, aPacket->mPsdu, aPacket->mLength);

    VerifyOrExit(PhyPlmeSetCurrentChannelRequestPAN0(aPacket->mChannel) == gPhySuccess_c, error = kThreadError_Busy);
    VerifyOrExit(PhyPdDataRequest(&pTransmitFrame, GetPhyTxMode(&sTransmitFrame), NULL) == gPhySuccess_c,
                 error = kThreadError_Busy);

    otLogDebgPlat("Transmitted %d bytes", aPacket->mLength);

exit:
    assert(error == kThreadError_None);
    MC1324xDrv_IRQ_Enable();
    return error;
}

ThreadError otPlatRadioHandleTransmitDone(bool *aFramePending)
{
    VerifyOrExit(sState != kStateDisabled, ;);
    assert(sState == kStateTransmit);
    //sState = kStateReceive; //return listening state
    *aFramePending = PhyPpIsRxAckDataPending();

exit:
    return sTransmitError;
}

ThreadError otPlatRadioHandleReceiveDone()
{
    VerifyOrExit(sState != kStateDisabled, ;);
    assert(sState == kStateReceive || sState == kStateTransmit);
    //sState = kStateSleep;
    sIsReceiverEnabled = false;

    otLogDebgPlat("Received %d bytes", sReceiveFrame.mLength);

exit:
    return sReceiveError;
}

void kw2xRadioProcess(otInstance *aInstance)
{
    ThreadError error;
    bool rxPending = false;

    if (sReceiveFrameDone && sReceiveFrame.mLength > 0)
    {
        MC1324xDrv_IRQ_Disable();
        error = otPlatRadioHandleReceiveDone();
        otPlatRadioReceiveDone(aInstance, &sReceiveFrame, error);
        sReceiveFrame.mLength = 0;
        //sIsReceiverEnabled = false;
        //startReceiveSequence(sChannel);
        PhyPlmeRxRequest(&pReceiveFrame, 0, &pRxParams);
        sReceiveFrameDone = false;
        MC1324xDrv_IRQ_Enable();
    }

    if (sTransmitFrameDone)
    {
        MC1324xDrv_IRQ_Disable();
        error = otPlatRadioHandleTransmitDone(&rxPending);
        otPlatRadioTransmitDone(aInstance, &sTransmitFrame, rxPending, error);
        sState = kStateReceive;
        //sTransmitFrame.mLength = 0; //eable this, will casue enter PHY_Interrupt()
        //startReceiveSequence(sChannel);
        PhyPlmeRxRequest(&pReceiveFrame, 0, &pRxParams);
        sTransmitFrameDone = false;
        MC1324xDrv_IRQ_Enable();
    }

    //otPlatRadioReceive(aInstance, sChannel);
}

RadioPacket *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    (void)aInstance;
    return &sTransmitFrame;
}

void PhyPlmeSyncLossIndication()
{
    switch (sState)
    {
    case kStateDisabled:
    case kStateSleep:
        break;

    case kStateReceive:
        //sReceiveError = kThreadError_Abort;
        PhyAbort();
        PhyPlmeRxRequest(&pReceiveFrame, 0, &pRxParams);
        //startReceiveSequence(sChannel);
        //sReceiveFrameDone = true;
        break;

    case kStateTransmit:
        sTransmitError = kThreadError_Abort;
        PhyAbort();
        PhyPdDataRequest(&pTransmitFrame, GetPhyTxMode(&sTransmitFrame), NULL); 
        //sTransmitFrameDone = true;
        break;

    default:
        assert(false);
        break;
    }
}

void PhyTimeRxTimeoutIndication()
{
    assert(false);
}

void PhyTimeStartEventIndication()
{
    assert(false);
}

void PhyPlmeCcaConfirm(bool_t aChannelInUse)
{
    switch (sState)
    {
    case kStateDisabled:
    case kStateSleep:
       break;

    case kStateTransmit:
        if (aChannelInUse)
        {
            sTransmitError = kThreadError_ChannelAccessFailure;
            sTransmitFrameDone = true;
            //PhyResetSequenceStatus();
        }

        break;

    default:
        assert(false);
        break;
    }
}

void PhyPlmeEdConfirm(uint8_t energyLevel)
{
    (void)energyLevel;

    assert(false);
}

void PhyPdDataConfirm(void)
{
    assert(sState == kStateTransmit);
    sTransmitError = kThreadError_None;
    sTransmitFrameDone = true;
}

void PhyPdDataIndication(void)
{
    //uint8_t rssi;
    //uint8_t lqi;

    switch (sState)
    {
    case kStateDisabled:
    case kStateSleep:
        break;

    case kStateReceive:

        //lqi = pRxParams.linkQuality;
        //rssi = ((rssi * 105) / 255) - 105;
        //rssi = PhyPlmeGetRSSILevelRequest();

        if (pReceiveFrame.frameLength > 0)
        {
            sReceiveFrame.mPower = PhyPlmeGetRSSILevelRequest(); //rssi
            sReceiveFrame.mLqi = (pRxParams.linkQuality/3) - 100; //LQI(dBm)
            sReceiveFrame.mLength = pReceiveFrame.frameLength;

            sReceiveError = kThreadError_None;
            sReceiveFrameDone = true;
        }
        break;

    default:
        assert(false);
    }
}

void PhyPlmeFilterFailRx(void)
{
    switch (sState)
    {
    case kStateDisabled:
    case kStateSleep:
        PhyAbort();
        break;

    case kStateReceive:
        PhyAbort();
        PhyPlmeRxRequest(&pReceiveFrame, 0, &pRxParams);
        //startReceiveSequence(sChannel);
        break;

    case kStateTransmit:
        break;

    default:
        assert(false);
        break;
    }
}

void PhyPlmeRxSfdDetect(uint8_t aFrameLength)
{
    (void)aFrameLength;

    switch (sState)
    {
    case kStateDisabled:
    case kStateSleep:
        break;

    case kStateReceive:
        break;

    case kStateTransmit:
        break;

    default:
        assert(false);
        break;
    }
}

int8_t otPlatRadioGetRssi(otInstance *aInstance)
{
    (void)aInstance;
    return 0;
}

otRadioCaps otPlatRadioGetCaps(otInstance *aInstance)
{
    (void)aInstance;
    return kRadioCapsNone;
}

bool otPlatRadioGetPromiscuous(otInstance *aInstance)
{
    (void)aInstance;
    return sIsPromiscuousEnabled;
}

void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnable)
{
    (void)aInstance;

    otLogInfoPlat("PromiscuousMode=%d", aEnable ? 1 : 0);

    sIsPromiscuousEnabled = aEnable;
    PhyPpSetPromiscuous(aEnable);
}

void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable)
{
    (void)aInstance;
    (void)aEnable;

    otLogInfoPlat("EnableSrcMatch=%d", aEnable ? 1 : 0);
}

ThreadError otPlatRadioAddSrcMatchShortEntry(otInstance *aInstance, const uint16_t aShortAddress)
{
    ThreadError error = kThreadError_NotImplemented;
    (void)aInstance;
    (void)aShortAddress;

    otLogDebgPlat("Add ShortAddr entry: %d", entry);

    return error;
}

ThreadError otPlatRadioAddSrcMatchExtEntry(otInstance *aInstance, const uint8_t *aExtAddress)
{
    ThreadError error = kThreadError_NotImplemented;
    (void)aInstance;
    (void)aExtAddress;

    otLogDebgPlat("Add ExtAddr entry: %d", entry);

    return error;
}

ThreadError otPlatRadioClearSrcMatchShortEntry(otInstance *aInstance, const uint16_t aShortAddress)
{
    ThreadError error = kThreadError_NotImplemented;
    (void)aInstance;
    (void)aShortAddress;

    otLogDebgPlat("Clear ShortAddr entry: %d", entry);

    return error;
}

ThreadError otPlatRadioClearSrcMatchExtEntry(otInstance *aInstance, const uint8_t *aExtAddress)
{
    ThreadError error = kThreadError_NotImplemented;
    (void)aInstance;
    (void)aExtAddress;

    otLogDebgPlat("Clear ExtAddr entry: %d", entry);

    return error;
}

void otPlatRadioClearSrcMatchShortEntries(otInstance *aInstance)
{
    (void)aInstance;

    otLogDebgPlat("Clear ShortAddr entries", NULL);
}

void otPlatRadioClearSrcMatchExtEntries(otInstance *aInstance)
{
    (void)aInstance;

    otLogDebgPlat("Clear ExtAddr entries", NULL);
}

ThreadError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration)
{
    (void)aInstance;
    (void)aScanChannel;
    (void)aScanDuration;
    return kThreadError_NotImplemented;
}

# if 0
void PhyUnexpectedTransceiverReset(void)
{
    sPhyEvents[sPhyEventsCur++] = 10;

    if (sPhyEventsCur >= sizeof(sPhyEvents))
    {
        sPhyEventsCur = 0;
    }

    assert(false);
}
#endif
