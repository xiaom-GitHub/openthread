/************************************************************************************
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
************************************************************************************/
/*****************************************************************************
 *                               INCLUDED HEADERS                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the headers that this module needs to include.    *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
#include "EmbeddedTypes.h"
#include "PortConfig.h"
#include "MC1324xDrv.h"
#include "MC1324xReg.h"
#include "Phy.h"
#include "NVIC.h"

#include "Phydebug.h"

/*****************************************************************************
 *                               PRIVATE VARIABLES                           *
 *---------------------------------------------------------------------------*
 * Add to this section all the variables and constants that have local       *
 * (file) scope.                                                             *
 * Each of this declarations shall be preceded by the 'static' keyword.      *
 * These variables / constants cannot be accessed outside this module.       *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
#define PHY_IRQSTS1_INDEX_c     0x00
#define PHY_IRQSTS2_INDEX_c     0x01
#define PHY_IRQSTS3_INDEX_c     0x02
#define PHY_CTRL1_INDEX_c       0x03
#define PHY_CTRL2_INDEX_c       0x04
#define PHY_CTRL3_INDEX_c       0x05
#define PHY_RX_FRM_LEN_INDEX_c  0x06
#define PHY_CTRL4_INDEX_c       0x07

static phyRxParams_t *mpRxParams = NULL;
static phyPacket_t *mpRxData = NULL;

#if (gUseStandaloneCCABeforeTx_d == 1)
static phyTxParams_t mTxParams;
#endif

#if gPhyReadPBinISR_d && gUsePBTransferThereshold_d
static uint8_t mPhyWatermarkLevel;
//#define mPhyGetPBTransferThreshold(len) (((len)*93)/100)
#define mPhyGetPBTransferThreshold(len) ((len)-2)
#endif

uint8_t mStatusAndControlRegs[10];
uint8_t mPhyLastRxLQI = 0;

/*****************************************************************************
 *                               PUBLIC VARIABLES                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the variables and constants that have global      *
 * (project) scope.                                                          *
 * These variables / constants can be accessed outside this module.          *
 * These variables / constants shall be preceded by the 'extern' keyword in  *
 * the interface header.                                                     *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
void (*pTimer1Isr)(void) = NULL;

/*****************************************************************************
 *                           PRIVATE FUNCTIONS PROTOTYPES                    *
 *---------------------------------------------------------------------------*
 * Add to this section all the functions prototypes that have local (file)   *
 * scope.                                                                    *
 * These functions cannot be accessed outside this module.                   *
 * These declarations shall be preceded by the 'static' keyword.             *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*****************************************************************************
 *                                PRIVATE FUNCTIONS                          *
 *---------------------------------------------------------------------------*
 * Add to this section all the functions that have local (file) scope.       *
 * These functions cannot be accessed outside this module.                   *
 * These definitions shall be preceded by the 'static' keyword.              *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
static uint8_t Phy_LqiConvert
(
    uint8_t hwLqi
)
{
    uint32_t tmpLQI;

    /* LQI Saturation Level */
    if (hwLqi >= 230)
    {
        return 0xFF;
    }
    else
    {
        /* Rescale the LQI values from min to saturation to the 0x00 - 0xFF range */
        /* The LQI value mst be multiplied by ~1.1087 */
        /* tmpLQI =  hwLqi * 7123 ~= hwLqi * 65536 * 0.1087 = hwLqi * 2^16 * 0.1087*/
        tmpLQI = ((uint32_t)hwLqi * (uint32_t)7123);
        /* tmpLQI =  (tmpLQI / 2^16) + hwLqi */
        tmpLQI = (uint32_t)(tmpLQI >> 16) + (uint32_t)hwLqi;

        return (uint8_t)tmpLQI;
    }
}

/*****************************************************************************
 *                             PUBLIC FUNCTIONS                              *
 *---------------------------------------------------------------------------*
 * Add to this section all the functions that have global (project) scope.   *
 * These functions can be accessed outside this module.                      *
 * These functions shall have their declarations (prototypes) within the     *
 * interface header file and shall be preceded by the 'extern' keyword.      *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*---------------------------------------------------------------------------
 * Name: PhyPassTxParams()
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPassTxParams
(
    phyTxParams_t *pTxParam
)
{
#if (gUseStandaloneCCABeforeTx_d == 1)
    mTxParams.useStandaloneCcaBeforeTx = pTxParam->useStandaloneCcaBeforeTx;
    mTxParams.numOfCca                 = pTxParam->numOfCca;
    mTxParams.phyTxMode                = pTxParam->phyTxMode;
#endif
}

/*---------------------------------------------------------------------------
 * Name: PhyPassRxParams()
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPassRxParams
(
    phyRxParams_t *pRxParam
)
{
    mpRxParams = pRxParam;
}

/*---------------------------------------------------------------------------
 * Name: PhyPassRxDataPtr()
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPassRxDataPtr
(
    phyPacket_t *pRxData
)
{
    mpRxData = pRxData;
}

/*---------------------------------------------------------------------------
 * Name: PhyIsrSeqCleanup
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyIsrSeqCleanup
(
    void
)
{
    mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] &= 0xF0;
    mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] &= (uint8_t) ~(cIRQSTS3_TMR3MSK);   // unmask TMR3 interrupt
    mStatusAndControlRegs[PHY_CTRL2_INDEX_c]   |= (uint8_t)(cPHY_CTRL2_CCAMSK | \
                                                            cPHY_CTRL2_RXMSK | \
                                                            cPHY_CTRL2_TXMSK | \
                                                            cPHY_CTRL2_SEQMSK);
    mStatusAndControlRegs[PHY_CTRL1_INDEX_c]   &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);

    // clear transceiver interrupts, mask SEQ, RX, TX and CCA interrupts and set the PHY sequencer back to IDLE
    MC1324xDrv_DirectAccessSPIMultiByteWrite(IRQSTS1, mStatusAndControlRegs, 5);

}

/*---------------------------------------------------------------------------
 * Name: PhyIsrTimeoutCleanup
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyIsrTimeoutCleanup
(
    void
)
{
    mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] &= 0xF0;
    mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] |= (uint8_t)(cIRQSTS3_TMR3MSK | \
                                                            cIRQSTS3_TMR3IRQ); // mask and clear TMR3 interrupt
    mStatusAndControlRegs[PHY_CTRL2_INDEX_c]   |= (uint8_t)(cPHY_CTRL2_CCAMSK | \
                                                            cPHY_CTRL2_RXMSK | \
                                                            cPHY_CTRL2_TXMSK | \
                                                            cPHY_CTRL2_SEQMSK);
    mStatusAndControlRegs[PHY_CTRL1_INDEX_c]   &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);

    // disable TMR3 comparator and timeout
    mStatusAndControlRegs[PHY_CTRL3_INDEX_c]   &= (uint8_t) ~(cPHY_CTRL3_TMR3CMP_EN);
    mStatusAndControlRegs[PHY_CTRL4_INDEX_c]   &= (uint8_t) ~(cPHY_CTRL4_TC3TMOUT);

    MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL3, mStatusAndControlRegs[PHY_CTRL3_INDEX_c]);
    MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL4, mStatusAndControlRegs[PHY_CTRL4_INDEX_c]);

    // clear transceiver interrupts, mask mask SEQ, RX, TX, TMR3 and CCA interrupts interrupts and set the PHY sequencer back to IDLE
    MC1324xDrv_DirectAccessSPIMultiByteWrite(IRQSTS1, mStatusAndControlRegs, 5);
}

/*---------------------------------------------------------------------------
 * Name: Phy_GetEnergyLevel
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t Phy_GetEnergyLevel
(
    void
)
{
    uint8_t energyLevel;
    energyLevel = MC1324xDrv_DirectAccessSPIRead((uint8_t) CCA1_ED_FNL);

    if (energyLevel >= 90)
    {
        /* ED value is below minimum. Return 0x00. */
        energyLevel = 0x00;
    }
    else if (energyLevel <= 26)
    {
        /* ED value is above maximum. Return 0xFF. */
        energyLevel = 0xFF;

    }
    else
    {
        /* Energy level (-90 dBm to -26 dBm ) --> varies form 0 to 64 */
        energyLevel = (90 - energyLevel);
        /* Rescale the energy level values to the 0x00-0xff range (0 to 64 translates in 0 to 255) */
        /* energyLevel * 3.9844 ~= 4 */
        /* Multiply with 4=2^2 by shifting left.
        The multiplication will not overflow beacause energyLevel has values between 0 and 63 */
        energyLevel <<= 2;
    }

    return energyLevel;
}

/*---------------------------------------------------------------------------
 * Name: PhyGetLastRxLqiValue
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/

uint8_t PhyGetLastRxLqiValue(void)
{
    return mPhyLastRxLQI;
}

/*---------------------------------------------------------------------------
 * Name: PHY_InterruptHandler
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
void PHY_InterruptHandler
(
    void
)
{
    uint8_t xcvseqCopy;

    // disable and clear transceiver(IRQ_B) interrupt
    MC1324xDrv_IRQ_Disable();
    MC1324xDrv_IRQ_Clear();

    // read transceiver interrupt status and control registers
    MC1324xDrv_DirectAccessSPIMultiByteRead(IRQSTS1, mStatusAndControlRegs, 8);

    xcvseqCopy = mStatusAndControlRegs[PHY_CTRL1] & cPHY_CTRL1_XCVSEQ;

    /* debugg */
    if (((mStatusAndControlRegs[PHY_IRQSTS2_INDEX_c] & cIRQSTS2_WAKE_IRQ) == cIRQSTS2_WAKE_IRQ)
        && ((mStatusAndControlRegs[PHY_CTRL3_INDEX_c] & cPHY_CTRL3_WAKE_MSK) != cPHY_CTRL3_WAKE_MSK))
    {
        // clear transceiver interrupts
        MC1324xDrv_DirectAccessSPIMultiByteWrite(IRQSTS1, mStatusAndControlRegs, 3);

#ifdef MAC_PHY_DEBUG
        PhyUnexpectedTransceiverReset();
#endif
        MC1324xDrv_IRQ_Enable();
        return;
    }

    if (((mStatusAndControlRegs[PHY_IRQSTS1_INDEX_c] & cIRQSTS1_FILTERFAIL_IRQ) == cIRQSTS1_FILTERFAIL_IRQ)
        && ((mStatusAndControlRegs[PHY_CTRL2_INDEX_c] & cPHY_CTRL2_FILTERFAIL_MSK) != cPHY_CTRL2_FILTERFAIL_MSK))
    {
#if gPhyReadPBinISR_d && gUsePBTransferThereshold_d
        /* Reset the RX_WTR_MARK level since packet was dropped. */
        mPhyWatermarkLevel = 0;
        MC1324xDrv_IndirectAccessSPIWrite(RX_WTR_MARK, mPhyWatermarkLevel);
#endif
        PhyPlmeFilterFailRx();
        MC1324xDrv_DirectAccessSPIWrite(IRQSTS1, (uint8_t)(cIRQSTS1_FILTERFAIL_IRQ | cIRQSTS1_RXWTRMRKIRQ));
    }
    else if (((mStatusAndControlRegs[PHY_IRQSTS1_INDEX_c] & cIRQSTS1_RXWTRMRKIRQ) == cIRQSTS1_RXWTRMRKIRQ)
             && ((mStatusAndControlRegs[PHY_CTRL2_INDEX_c] & cPHY_CTRL2_RX_WMRK_MSK) != cPHY_CTRL2_RX_WMRK_MSK))
    {
        PHY_DEBUG_LOG(PDBG_WTRMRK_VECT_INTERRUPT);
#if gPhyReadPBinISR_d && gUsePBTransferThereshold_d

        if (0 == mPhyWatermarkLevel)
        {
            /* Check if this is a standalone RX because we could end up here during a TR sequence also. */
            if (xcvseqCopy == gRX_c)
            {
                /* Set the thereshold packet length at which to start the PB Burst Read.*/
                mPhyWatermarkLevel = mPhyGetPBTransferThreshold(mStatusAndControlRegs[PHY_RX_FRM_LEN_INDEX_c]);
                MC1324xDrv_IndirectAccessSPIWrite(RX_WTR_MARK, mPhyWatermarkLevel);
            }

#endif
            PhyPlmeRxSfdDetect(mStatusAndControlRegs[PHY_RX_FRM_LEN_INDEX_c]);
#if gPhyReadPBinISR_d && gUsePBTransferThereshold_d
        }
        else
        {
            /* Reset RX_WTR_MARK here, because if the FCS fails, no other IRQ will arrive
             * and the RX will restart automatically. */
            mPhyWatermarkLevel = 0;
            MC1324xDrv_IndirectAccessSPIWrite(RX_WTR_MARK, mPhyWatermarkLevel);

            if (NULL != mpRxData)
            {
                /* Read data from PB */
                MC1324xDrv_PB_SPIBurstRead((uint8_t *)(&mpRxData->data[0]),
                                           (uint8_t)(mStatusAndControlRegs[PHY_RX_FRM_LEN_INDEX_c] - 2));

                PhyPlmePreprocessData(mpRxData->data);
            }
        }

#endif
        MC1324xDrv_DirectAccessSPIWrite(IRQSTS1, (uint8_t) cIRQSTS1_RXWTRMRKIRQ);
    }

    // sequencer interrupt, the autosequence has completed
    if ((mStatusAndControlRegs[PHY_IRQSTS1_INDEX_c] & cIRQSTS1_SEQIRQ) == cIRQSTS1_SEQIRQ)
    {
        /* PLL unlock, the autosequence has been aborted due to PLL unlock */
        if ((mStatusAndControlRegs[PHY_IRQSTS1_INDEX_c] & cIRQSTS1_PLL_UNLOCK_IRQ) == cIRQSTS1_PLL_UNLOCK_IRQ)
        {
            PhyIsrSeqCleanup();

            PHY_DEBUG_LOG(PDBG_SYNC_LOSS_INDICATION);

            PhyPlmeSyncLossIndication();
            MC1324xDrv_IRQ_Enable();
            return;
        }

        /* TMR3 timeout, the autosequence has been aborted due to TMR3 timeout */
        if (((mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] & cIRQSTS3_TMR3IRQ) == cIRQSTS3_TMR3IRQ)
            && ((mStatusAndControlRegs[PHY_IRQSTS1_INDEX_c] & cIRQSTS1_RXIRQ)   != cIRQSTS1_RXIRQ))
        {
            PHY_DEBUG_LOG(PDBG_TIMEOUT_INDICATION);
            PhyIsrTimeoutCleanup();
            PhyTimeRxTimeoutIndication();
            MC1324xDrv_IRQ_Enable();
            return;
        }

        PhyIsrSeqCleanup();

        switch (xcvseqCopy)
        {
        case gTX_c:
        {
            if (((mStatusAndControlRegs[PHY_IRQSTS2_INDEX_c] & cIRQSTS2_CCA)        == cIRQSTS2_CCA)
                && ((mStatusAndControlRegs[PHY_CTRL1_INDEX_c]   & cPHY_CTRL1_CCABFRTX) == cPHY_CTRL1_CCABFRTX))
            {
                PHY_DEBUG_LOG(PDBG_CCA_CONFIRM_BUSY);
                PhyPlmeCcaConfirm(gChannelBusy_c);
            }
            else
            {
                PHY_DEBUG_LOG(PDBG_DATA_CONFIRM);
                PhyPdDataConfirm();
            }
        }
        break;

        case gTR_c:
        {
            if (((mStatusAndControlRegs[PHY_IRQSTS2_INDEX_c] & cIRQSTS2_CCA)        == cIRQSTS2_CCA)
                && ((mStatusAndControlRegs[PHY_CTRL1_INDEX_c]   & cPHY_CTRL1_CCABFRTX) == cPHY_CTRL1_CCABFRTX))
            {
                PHY_DEBUG_LOG(PDBG_CCA_CONFIRM_BUSY);
                PhyPlmeCcaConfirm(gChannelBusy_c);
            }
            else
            {
                if (NULL != mpRxParams)
                {
                    // reports value of 0x00 for -105 dBm of received input power and 0xFF for 0 dBm of received input power
                    mpRxParams->linkQuality = MC1324xDrv_DirectAccessSPIRead((uint8_t) LQI_VALUE);
                    mpRxParams->linkQuality = Phy_LqiConvert(mpRxParams->linkQuality);
                    mPhyLastRxLQI = mpRxParams->linkQuality;
                    MC1324xDrv_DirectAccessSPIMultiByteRead((uint8_t) TIMESTAMP_LSB, (uint8_t *) &mpRxParams->timeStamp, 3);
                }

                PHY_DEBUG_LOG(PDBG_DATA_CONFIRM);
                PhyPdDataConfirm();
            }
        }
        break;

        case gRX_c:
        {
            if (NULL != mpRxParams)
            {
                // reports value of 0x00 for -105 dBm of received input power and 0xFF for 0 dBm of received input power
                mpRxParams->linkQuality = MC1324xDrv_DirectAccessSPIRead((uint8_t) LQI_VALUE);
                mpRxParams->linkQuality = Phy_LqiConvert(mpRxParams->linkQuality);
                mPhyLastRxLQI = mpRxParams->linkQuality;
                MC1324xDrv_DirectAccessSPIMultiByteRead((uint8_t) TIMESTAMP_LSB, (uint8_t *) &mpRxParams->timeStamp, 3);
            }

            if (NULL != mpRxData)
            {
#if gPhyReadPBinISR_d && !gUsePBTransferThereshold_d
                /* If PB Transfer at Thereshold is used, there is no need to read the PB at DataIndication anymore. */
                MC1324xDrv_PB_SPIBurstRead((uint8_t *)(&mpRxData->data[0]),
                                           (uint8_t)(mStatusAndControlRegs[PHY_RX_FRM_LEN_INDEX_c] - 2));
#endif
                mpRxData->frameLength = (uint8_t)(mStatusAndControlRegs[PHY_RX_FRM_LEN_INDEX_c]); //Including FCS (2 bytes)
            }

            PHY_DEBUG_LOG(PDBG_DATA_INDICATION);
            PhyPdDataIndication();
        }
        break;

        case gCCA_c:
        {
            if ((mStatusAndControlRegs[PHY_CTRL4_INDEX_c] & (cPHY_CTRL4_CCATYPE << cPHY_CTRL4_CCATYPE_Shift_c)) ==
                (gCcaED_c << cPHY_CTRL4_CCATYPE_Shift_c))
            {
                // Ed
                PhyPlmeEdConfirm(Phy_GetEnergyLevel());
            }
            else
            {
                // CCA
#if (gUseStandaloneCCABeforeTx_d == 1)
                if (((mStatusAndControlRegs[PHY_IRQSTS2_INDEX_c] & cIRQSTS2_CCA) == cIRQSTS2_CCA))
                {
                    if (TRUE == mTxParams.useStandaloneCcaBeforeTx)
                    {
                        mTxParams.useStandaloneCcaBeforeTx = FALSE;
                        mTxParams.numOfCca                 = 1;
                    }

                    PHY_DEBUG_LOG(PDBG_CCA_CONFIRM_BUSY);

                    PhyPlmeCcaConfirm(gChannelBusy_c);
                }
                else
                {
                    if (FALSE == mTxParams.useStandaloneCcaBeforeTx)
                    {
                        PHY_DEBUG_LOG(PDBG_CCA_CONFIRM_IDLE);

                        PhyPlmeCcaConfirm(gChannelIdle_c);
                    }
                    else
                    {
                        if (mTxParams.numOfCca > 0)
                        {
                            mTxParams.numOfCca--;
                        }

                        if (mTxParams.numOfCca == 0)
                        {
                            uint8_t phyCtrl1Reg, phyCtrl2Reg;

                            phyCtrl1Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL1);
                            phyCtrl2Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL2);

                            mTxParams.useStandaloneCcaBeforeTx = FALSE;
                            mTxParams.numOfCca                 = 1;

                            // perform TxRxAck sequence if required by phyTxMode
                            if (gAckReqMask_c & mTxParams.phyTxMode)
                            {
                                phyCtrl1Reg |= (uint8_t)(cPHY_CTRL1_RXACKRQD);
                                phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
                                phyCtrl1Reg |=  gTR_c;
                            }
                            else
                            {
                                phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_RXACKRQD);
                                phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
                                phyCtrl1Reg |=  gTX_c;
                            }

                            phyCtrl2Reg &= (uint8_t) ~(cPHY_CTRL2_SEQMSK); // unmask SEQ interrupt

                            // start immediately the TX or TRX sequence
                            MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL2, phyCtrl2Reg);
                            MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL1, phyCtrl1Reg);
                        }
                        else
                        {
                            uint8_t phyCtrl1Reg, phyCtrl2Reg;

                            phyCtrl1Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL1);
                            phyCtrl2Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL2);

                            phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
                            phyCtrl1Reg |= gCCA_c;

                            phyCtrl2Reg &= (uint8_t) ~(cPHY_CTRL2_SEQMSK); // unmask SEQ interrupt

                            // start immediately the second CCA sequence
                            MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL2, phyCtrl2Reg);
                            MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL1, phyCtrl1Reg);
                        }
                    }
                }

#else //  (gUseStandaloneCCABeforeTx_d == 1)

                if (((mStatusAndControlRegs[PHY_IRQSTS2_INDEX_c] & cIRQSTS2_CCA) == cIRQSTS2_CCA))
                {

                    PHY_DEBUG_LOG(PDBG_CCA_CONFIRM_BUSY);
                    PhyPlmeCcaConfirm(gChannelBusy_c);
                }
                else
                {

                    PHY_DEBUG_LOG(PDBG_CCA_CONFIRM_IDLE);

                    PhyPlmeCcaConfirm(gChannelIdle_c);
                }

#endif //   (gUseStandaloneCCABeforeTx_d == 1)
            }
        }
        break;

        case gCCCA_c:
        {
            PHY_DEBUG_LOG(PDBG_CCA_CONFIRM_IDLE);

            PhyPlmeCcaConfirm(gChannelIdle_c);
        }
        break;

        default:
        {
            PHY_DEBUG_LOG(PDBG_SYNC_LOSS_INDICATION);

            PhyPlmeSyncLossIndication();
        }
        break;
        }
    }
    // timers interrupt
    else
    {
        if ((mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] & cIRQSTS3_TMR2IRQ) == cIRQSTS3_TMR2IRQ)
        {
            // mask and clear TMR2 interrupt
            mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] &= 0xF0;
            mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] |= (uint8_t)(cIRQSTS3_TMR2MSK | \
                                                                    cIRQSTS3_TMR2IRQ);

            MC1324xDrv_DirectAccessSPIWrite(IRQSTS3, mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c]);

            // disable TMR2 comparator and time triggered action
            mStatusAndControlRegs[PHY_CTRL3_INDEX_c]   &= (uint8_t) ~(cPHY_CTRL3_TMR2CMP_EN);
            mStatusAndControlRegs[PHY_CTRL1_INDEX_c]   &= (uint8_t) ~(cPHY_CTRL1_TMRTRIGEN);



            MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL3, mStatusAndControlRegs[PHY_CTRL3_INDEX_c]);
            MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL4, mStatusAndControlRegs[PHY_CTRL4_INDEX_c]);
            MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL1, mStatusAndControlRegs[PHY_CTRL1_INDEX_c]);

            PHY_DEBUG_LOG(PDBG_START_INDICATION);

            PhyTimeStartEventIndication();

        }

        if ((gIdle_c == PhyGetSeqState()) &&
            (mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] & cIRQSTS3_TMR3IRQ) == cIRQSTS3_TMR3IRQ)
        {
            // mask and clear TMR3 interrupt
            mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] &= 0xF0;
            mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] |= (uint8_t)(cIRQSTS3_TMR3MSK | \
                                                                    cIRQSTS3_TMR3IRQ);

            MC1324xDrv_DirectAccessSPIWrite(IRQSTS3, mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c]);

            // disable TMR3 comparator and timeout
            mStatusAndControlRegs[PHY_CTRL3_INDEX_c]   &= (uint8_t) ~(cPHY_CTRL3_TMR3CMP_EN);
            mStatusAndControlRegs[PHY_CTRL4_INDEX_c]   &= (uint8_t) ~(cPHY_CTRL4_TC3TMOUT);

            MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL3, mStatusAndControlRegs[PHY_CTRL3_INDEX_c]);
            MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL4, mStatusAndControlRegs[PHY_CTRL4_INDEX_c]);

            /* Ensure that we're not issuing TimeoutIndication while the Automated sequence is still in progress */
            /* TMR3 can expire during R-T turnaround for example, case in which the sequence is not interrupted */

            PHY_DEBUG_LOG(PDBG_TIMEOUT_INDICATION);

            PhyTimeRxTimeoutIndication();
        }

        if ((mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] & cIRQSTS3_TMR1IRQ) == cIRQSTS3_TMR1IRQ)
        {
            // mask and clear TMR1 interrupt
            mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] &= 0xF0;
            mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] |= (uint8_t)(cIRQSTS3_TMR1MSK | \
                                                                    cIRQSTS3_TMR1IRQ);

            MC1324xDrv_DirectAccessSPIWrite(IRQSTS3, mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c]);

            // disable TMR1 comparator
            mStatusAndControlRegs[PHY_CTRL3_INDEX_c]   &= (uint8_t) ~(cPHY_CTRL3_TMR1CMP_EN);

            MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL3, mStatusAndControlRegs[PHY_CTRL3_INDEX_c]);

            // Tmr1 Callback
            if (pTimer1Isr)
            {
                pTimer1Isr();
            }
        }

        if ((mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] & cIRQSTS3_TMR4IRQ) == cIRQSTS3_TMR4IRQ)
        {

            // mask and clear TMR4 interrupt
            mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] &= 0xF0;
            mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c] |= (uint8_t)(cIRQSTS3_TMR4MSK | \
                                                                    cIRQSTS3_TMR4IRQ);

            MC1324xDrv_DirectAccessSPIWrite(IRQSTS3, mStatusAndControlRegs[PHY_IRQSTS3_INDEX_c]);

            // disable TMR4 comparator
            mStatusAndControlRegs[PHY_CTRL3_INDEX_c]   &= (uint8_t) ~(cPHY_CTRL3_TMR4CMP_EN);

            MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL3, mStatusAndControlRegs[PHY_CTRL3_INDEX_c]);
        }
    }

    MC1324xDrv_IRQ_Enable();
}
