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

//2405   2410    2415    2420    2425    2430    2435    2440    2445    2450    2455    2460    2465    2470    2475    2480
static const uint8_t  pll_int[16] =  {0x0B,   0x0B,   0x0B,   0x0B,   0x0B,   0x0B,   0x0C,   0x0C,   0x0C,   0x0C,   0x0C,   0x0C,   0x0D,   0x0D,   0x0D,   0x0D};
static const uint16_t pll_frac[16] = {0x2800, 0x5000, 0x7800, 0xA000, 0xC800, 0xF000, 0x1800, 0x4000, 0x6800, 0x9000, 0xB800, 0xE000, 0x0800, 0x3000, 0x5800, 0x8000};

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

static uint8_t gPhyCurrentChannelPAN0 = 0x0B;
static uint8_t gPhyCurrentChannelPAN1 = 0x0B;

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
 * Name: PhyPdDataRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#if (gUseStandaloneCCABeforeTx_d == 1)

uint8_t PhyPdDataRequest
(
    phyPacket_t *pTxPacket,
    uint8_t phyTxMode,
    phyRxParams_t *pRxParams
)
{
    uint8_t phyCtrl1Reg, phyCtrl2Reg, phyCtrl4Reg, irqSts1Reg, irqSts2Reg, irqSts3Reg;
    phyTxParams_t txParam;

    PHY_DEBUG_LOG(PDBG_DATA_REQUEST);

#ifdef PHY_PARAMETERS_VALIDATION

    // null pointer
    if (NULL == pTxPacket)
    {
        return gPhyInvalidParam_c;
    }

    // cannot have packets shorter than FCS field (2 byte)
    if (pTxPacket->frameLength < 2)
    {
        return gPhyInvalidParam_c;
    }

    // if CCA required ...
    if (gCcaEnMask_c & phyTxMode)
    {
        // ... cannot perform other types than MODE1 and MODE2
        if ((gCcaCCA_MODE1_c != (gCcaTypeMask_c & phyTxMode)) && (gCcaCCA_MODE2_c != (gCcaTypeMask_c & phyTxMode)))
        {
            return gPhyInvalidParam_c;
        }

        // ... cannot perform Continuous CCA on T or TR sequences
        if (gContinuousEnMask_c & phyTxMode)
        {
            return gPhyInvalidParam_c;
        }
    }

#endif // PHY_PARAMETERS_VALIDATION

    if (gIdle_c != PhyGetSeqState())
    {
        return gPhyBusy_c;
    }

    txParam.useStandaloneCcaBeforeTx = FALSE;
    txParam.numOfCca                 = 1;
    txParam.phyTxMode                = phyTxMode;

    MC1324xDrv_PB_SPIBurstWrite((uint8_t *) pTxPacket, (uint8_t)(pTxPacket->frameLength - 1));

    phyCtrl1Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL1);
    phyCtrl2Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL2);
    phyCtrl4Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL4);

    // perform CCA before TX if required by phyTxMode parameter
    if (gCcaEnMask_c & phyTxMode)
    {
        phyCtrl4Reg &= (uint8_t) ~(cPHY_CTRL4_CCATYPE << cPHY_CTRL4_CCATYPE_Shift_c);
        phyCtrl4Reg |= (uint8_t)((gCcaTypeMask_c & phyTxMode) << (cPHY_CTRL4_CCATYPE_Shift_c - gCcaTypePos_c));
    }
    else
    {
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_CCABFRTX);
        phyCtrl4Reg &= (uint8_t) ~(cPHY_CTRL4_CCATYPE << cPHY_CTRL4_CCATYPE_Shift_c);
    }

    // slotted operation
    if (gSlottedEnMask_c & phyTxMode)
    {
        txParam.numOfCca = 2;
        phyCtrl1Reg     |= (uint8_t)(cPHY_CTRL1_SLOTTED);
    }
    else
    {
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_SLOTTED);
    }

    // perform TxRxAck sequence if required by phyTxMode
    if (gAckReqMask_c & phyTxMode)
    {
        PhyPassRxParams(pRxParams);
        PhyPassRxDataPtr(NULL);

        phyCtrl1Reg |= (uint8_t)(cPHY_CTRL1_RXACKRQD);
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
        phyCtrl1Reg |=  gTR_c;
    }
    else
    {
        PhyPassRxParams(NULL);
        PhyPassRxDataPtr(NULL);

        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_RXACKRQD);
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
        phyCtrl1Reg |=  gTX_c;
    }

    phyCtrl2Reg &= (uint8_t) ~(cPHY_CTRL2_SEQMSK); // unmask SEQ interrupt

    irqSts1Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS1);
    irqSts2Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS2);
    irqSts3Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS3);

    irqSts3Reg |= (uint8_t)(cIRQSTS3_TMR3MSK);    // mask TMR3 interrupt

    // ensure that no spurious interrupts are raised
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) IRQSTS1, irqSts1Reg);
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) IRQSTS2, irqSts2Reg);
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) IRQSTS3, irqSts3Reg);

    MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL2, phyCtrl2Reg);
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL4, phyCtrl4Reg);

    if (gCcaEnMask_c & phyTxMode)
    {
        // start the CCA or ED sequence (this depends on CcaType used)
        // immediately or by TC2', depending on a previous PhyTimeSetEventTrigger() call)
        txParam.useStandaloneCcaBeforeTx = TRUE;
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
        phyCtrl1Reg |= gCCA_c;
        // at the end of the scheduled sequence, an interrupt will occur:
        // CCA , SEQ or TMR3
    }

    PhyPassTxParams(&txParam);

    // start the CCA, TX or TRX sequence
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL1, phyCtrl1Reg);

    return gPhySuccess_c;
}

#else // if (gUseStandaloneCCABeforeTx_d == 1)

uint8_t PhyPdDataRequest
(
    phyPacket_t *pTxPacket,
    uint8_t phyTxMode,
    phyRxParams_t *pRxParams
)
{
    uint8_t phyCtrl1Reg, phyCtrl2Reg, phyCtrl4Reg, irqSts1Reg, irqSts2Reg, irqSts3Reg;

    PHY_DEBUG_LOG(PDBG_DATA_REQUEST);

#ifdef PHY_PARAMETERS_VALIDATION

    // null pointer
    if (NULL == pTxPacket)
    {
        return gPhyInvalidParam_c;
    }


    // cannot have packets shorter than FCS field (2 byte)
    if (pTxPacket->frameLength < 2)
    {
        return gPhyInvalidParam_c;
    }

    // if CCA required ...
    if (gCcaEnMask_c & phyTxMode)
    {
        // ... cannot perform other types than MODE1 and MODE2
        if ((gCcaCCA_MODE1_c != (gCcaTypeMask_c & phyTxMode)) && (gCcaCCA_MODE2_c != (gCcaTypeMask_c & phyTxMode)))
        {
            return gPhyInvalidParam_c;
        }

        // ... cannot perform Continuous CCA on T or TR sequences
        if (gContinuousEnMask_c & phyTxMode)
        {
            return gPhyInvalidParam_c;
        }
    }

#endif // PHY_PARAMETERS_VALIDATION

    if (gIdle_c != PhyGetSeqState())
    {
        return gPhyBusy_c;
    }

    MC1324xDrv_PB_SPIBurstWrite((uint8_t *) pTxPacket, (uint8_t)(pTxPacket->frameLength - 1));

    phyCtrl1Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL1);
    phyCtrl2Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL2);
    phyCtrl4Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL4);

    // perform CCA before TX if required by phyTxMode parameter
    if (gCcaEnMask_c & phyTxMode)
    {
        phyCtrl1Reg |= (uint8_t)(cPHY_CTRL1_CCABFRTX);
        phyCtrl4Reg &= (uint8_t) ~(cPHY_CTRL4_CCATYPE << cPHY_CTRL4_CCATYPE_Shift_c);
        phyCtrl4Reg |= (uint8_t)((gCcaTypeMask_c & phyTxMode) << (cPHY_CTRL4_CCATYPE_Shift_c - gCcaTypePos_c));
    }
    else
    {
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_CCABFRTX);
        phyCtrl4Reg &= (uint8_t) ~(cPHY_CTRL4_CCATYPE << cPHY_CTRL4_CCATYPE_Shift_c);
    }

    // slotted operation
    if (gSlottedEnMask_c & phyTxMode)
    {
        phyCtrl1Reg |= (uint8_t)(cPHY_CTRL1_SLOTTED);
    }
    else
    {
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_SLOTTED);
    }

    // perform TxRxAck sequence if required by phyTxMode
    if (gAckReqMask_c & phyTxMode)
    {
        PhyPassRxParams(pRxParams);
        PhyPassRxDataPtr(NULL);

        phyCtrl1Reg |= (uint8_t)(cPHY_CTRL1_RXACKRQD);
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
        phyCtrl1Reg |=  gTR_c;
    }
    else
    {
        PhyPassRxParams(NULL);
        PhyPassRxDataPtr(NULL);

        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_RXACKRQD);
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
        phyCtrl1Reg |=  gTX_c;
    }

    phyCtrl2Reg &= (uint8_t) ~(cPHY_CTRL2_SEQMSK); // unmask SEQ interrupt

    irqSts1Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS1);
    irqSts2Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS2);
    irqSts3Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS3);

    irqSts3Reg |= (uint8_t)(cIRQSTS3_TMR3MSK);    // mask TMR3 interrupt

    // ensure that no spurious interrupts are raised
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) IRQSTS1, irqSts1Reg);
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) IRQSTS2, irqSts2Reg);
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) IRQSTS3, irqSts3Reg);

    MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL2, phyCtrl2Reg);
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL4, phyCtrl4Reg);

    // start the TX or TRX sequence
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL1, phyCtrl1Reg);

    return gPhySuccess_c;
}

#endif // if (gUseStandaloneCCABeforeTx_d == 1)


/*---------------------------------------------------------------------------
 * Name: PhyPlmeRxRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeRxRequest
(
    phyPacket_t *pRxData,
    uint8_t phyRxMode,
    phyRxParams_t *pRxParams
)
{
    uint8_t phyCtrl1Reg, phyCtrl2Reg, irqSts1Reg, irqSts2Reg, irqSts3Reg;

    PHY_DEBUG_LOG(PDBG_PLME_RX_REQUEST);

#ifdef PHY_PARAMETERS_VALIDATION

    if (NULL == pRxData)
    {
        return gPhyInvalidParam_c;
    }

#endif // PHY_PARAMETERS_VALIDATION

    if (gIdle_c != PhyGetSeqState())
    {
        return gPhyBusy_c;
    }

    PhyPassRxParams(pRxParams);
    PhyPassRxDataPtr(pRxData);

    phyCtrl1Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL1);
    phyCtrl2Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL2);

    // slotted operation
    if (gSlottedEnMask_c & phyRxMode)
    {
        phyCtrl1Reg |= (uint8_t)(cPHY_CTRL1_SLOTTED);
    }
    else
    {
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_SLOTTED);
    }

    // program the RX sequence
    phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
    phyCtrl1Reg |=  gRX_c;

    phyCtrl2Reg &= (uint8_t) ~(cPHY_CTRL2_SEQMSK); // unmask SEQ interrupt

    irqSts1Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS1);
    irqSts2Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS2);
    irqSts3Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS3);

    irqSts3Reg |= (uint8_t)(cIRQSTS3_TMR3MSK);    // mask TMR3 interrupt

    // ensure that no spurious interrupts are raised
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) IRQSTS1, irqSts1Reg);
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) IRQSTS2, irqSts2Reg);
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) IRQSTS3, irqSts3Reg);

    MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL2, phyCtrl2Reg);

    // start the RX sequence
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL1, phyCtrl1Reg);

    return gPhySuccess_c;
}

/*---------------------------------------------------------------------------
 * Name: PhyPlmeCcaEdRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeCcaEdRequest
(
    uint8_t ccaParam
)
{
    uint8_t phyCtrl1Reg, phyCtrl2Reg, phyCtrl4Reg, irqSts1Reg, irqSts2Reg, irqSts3Reg;

    PHY_DEBUG_LOG(PDBG_PLME_CCA_ED_REQUEST);

#ifdef PHY_PARAMETERS_VALIDATION

    // illegal CCA type
    if (gInvalidCcaType_c == (gCcaTypeMask_c & ccaParam))
    {
        return gPhyInvalidParam_c;
    }

    // cannot perform Continuous CCA using ED type
    if ((gContinuousEnMask_c & ccaParam) && (gCcaED_c == (gCcaTypeMask_c & ccaParam)))
    {
        return gPhyInvalidParam_c;
    }

    // cannot perform ED request using other type than gCcaED_c
    if ((gCcaEnMask_c & ccaParam) && (gCcaED_c != (gCcaTypeMask_c & ccaParam)))
    {
        return gPhyInvalidParam_c;
    }

    // cannot perform ED request using Continuous mode
    if ((gContinuousEnMask_c & ccaParam) && ((gCcaEnMask_c & ccaParam) == 0))
    {
        return gPhyInvalidParam_c;
    }

#endif // PHY_PARAMETERS_VALIDATION

    if (gIdle_c != PhyGetSeqState())
    {
        return gPhyBusy_c;
    }

    phyCtrl1Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL1);
    phyCtrl2Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL2);
    phyCtrl4Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL4);

    // write in PHY CTRL4 the desired type of CCA
    phyCtrl4Reg &= (uint8_t) ~(cPHY_CTRL4_CCATYPE << cPHY_CTRL4_CCATYPE_Shift_c);
    phyCtrl4Reg |= (uint8_t)((gCcaTypeMask_c & ccaParam) << (cPHY_CTRL4_CCATYPE_Shift_c - gCcaTypePos_c));

    MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL4, phyCtrl4Reg);

    // slotted operation
    if (gSlottedEnMask_c & ccaParam)
    {
        phyCtrl1Reg |= (uint8_t)(cPHY_CTRL1_SLOTTED);
    }
    else
    {
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_SLOTTED);
    }

    // continuous CCA
    if (gContinuousEnMask_c & ccaParam)
    {
        // start the continuous CCA sequence
        // immediately or by TC2', depending on a previous PhyTimeSetEventTrigger() call)
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
        phyCtrl1Reg |= gCCCA_c;
        // at the end of the scheduled sequence, an interrupt will occur:
        // CCA , SEQ or TMR3
    }
    // normal CCA (not continuous)
    else
    {
        // start the CCA or ED sequence (this depends on CcaType used)
        // immediately or by TC2', depending on a previous PhyTimeSetEventTrigger() call)
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
        phyCtrl1Reg |= gCCA_c;
        // at the end of the scheduled sequence, an interrupt will occur:
        // CCA , SEQ or TMR3
    }

    phyCtrl2Reg &= (uint8_t) ~(cPHY_CTRL2_SEQMSK); // unmask SEQ interrupt

    irqSts1Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS1);
    irqSts2Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS2);
    irqSts3Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS3);

    irqSts3Reg |= (uint8_t)(cIRQSTS3_TMR3IRQ);    // mask TMR3 interrupt

    // ensure that no spurious interrupts are raised
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) IRQSTS1, irqSts1Reg);
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) IRQSTS2, irqSts2Reg);
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) IRQSTS3, irqSts3Reg);

    // start the CCA/ED or CCCA sequence
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL2, phyCtrl2Reg);
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL1, phyCtrl1Reg);

    return gPhySuccess_c;
}

/*---------------------------------------------------------------------------
 * Name: PhyPlmeSetCurrentChannelRequestPAN0
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeSetCurrentChannelRequestPAN0
(
    uint8_t channel
)
{
    PHY_DEBUG_LOG(PDBG_PLME_SET_CHAN_REQUEST);

#ifdef PHY_PARAMETERS_VALIDATION

    if ((channel < 11) || (channel > 26))
    {
        return gPhyInvalidParam_c;
    }

#endif // PHY_PARAMETERS_VALIDATION

    uint8_t rval = PhyGetSeqState();

    if (gIdle_c != rval)
    {
        return gPhyBusy_c;
    }

    gPhyCurrentChannelPAN0 = channel;
    MC1324xDrv_DirectAccessSPIWrite(PLL_INT0, pll_int[channel - 11]);
    MC1324xDrv_DirectAccessSPIMultiByteWrite(PLL_FRAC0_LSB, (uint8_t *) &pll_frac[channel - 11], 2);
    return gPhySuccess_c;
}

/*---------------------------------------------------------------------------
 * Name: PhyPlmeSetCurrentChannelRequestPAN1
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeSetCurrentChannelRequestPAN1
(
    uint8_t channel
)
{
    PHY_DEBUG_LOG(PDBG_PLME_SET_CHAN_REQUEST);

#ifdef PHY_PARAMETERS_VALIDATION

    if ((channel < 11) || (channel > 26))
    {
        return gPhyInvalidParam_c;
    }

#endif // PHY_PARAMETERS_VALIDATION

    if (gIdle_c != PhyGetSeqState())
    {
        return gPhyBusy_c;
    }

    gPhyCurrentChannelPAN1 = channel;
    MC1324xDrv_IndirectAccessSPIWrite(PLL_INT1, pll_int[channel - 11]);
    MC1324xDrv_IndirectAccessSPIMultiByteWrite(PLL_FRAC1_LSB, (uint8_t *) &pll_frac[channel - 11], 2);
    return gPhySuccess_c;
}

/*---------------------------------------------------------------------------
 * Name: PhyPlmeGetCurrentChannelRequestPAN0
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeGetCurrentChannelRequestPAN0
(
    void
)
{
    return gPhyCurrentChannelPAN0;
}

/*---------------------------------------------------------------------------
 * Name: PhyPlmeGetCurrentChannelRequestPAN1
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeGetCurrentChannelRequestPAN1
(
    void
)
{
    return gPhyCurrentChannelPAN1;
}

/*---------------------------------------------------------------------------
 * Name: PhyPlmeSetPwrLevelRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeSetPwrLevelRequest
(
    uint8_t pwrStep
)
{
    PHY_DEBUG_LOG(PDBG_PLME_SET_POWER_REQUEST);

#ifdef PHY_PARAMETERS_VALIDATION

    if ((pwrStep < 3) || (pwrStep > 31)) //-40 dBm to 16 dBm
    {
        return gPhyInvalidParam_c;
    }

#endif // PHY_PARAMETERS_VALIDATION

    MC1324xDrv_DirectAccessSPIWrite(PA_PWR, (uint8_t)(pwrStep & 0x1F));
    return gPhySuccess_c;
}

/*---------------------------------------------------------------------------
 * Name: PhyPlmeSetFADStateRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeSetFADStateRequest(bool_t state)
{
    uint8_t currentState;

    currentState = MC1324xDrv_IndirectAccessSPIRead(ANT_AGC_CTRL);
    state ? (currentState |= cANT_AGC_CTRL_FAD_EN_Mask_c) : (currentState &= (~((uint8_t)cANT_AGC_CTRL_FAD_EN_Mask_c)));
    MC1324xDrv_IndirectAccessSPIWrite(ANT_AGC_CTRL, currentState);

    return gPhySuccess_c;
}

/*---------------------------------------------------------------------------
 * Name: PhyPlmeSetFADThresholdRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeSetFADThresholdRequest(uint8_t FADThreshold)
{
    MC1324xDrv_IndirectAccessSPIWrite(FAD_THR, FADThreshold);
    return gPhySuccess_c;
}

/*---------------------------------------------------------------------------
 * Name: PhyPlmeSetANTXStateRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeSetANTXStateRequest(bool_t state)
{
    uint8_t currentState;

    currentState = MC1324xDrv_IndirectAccessSPIRead(ANT_AGC_CTRL);
    state ? (currentState |= cANT_AGC_CTRL_ANTX_Mask_c) : (currentState &= (~((uint8_t)cANT_AGC_CTRL_ANTX_Mask_c)));
    MC1324xDrv_IndirectAccessSPIWrite(ANT_AGC_CTRL, currentState);

    return gPhySuccess_c;
}

/*---------------------------------------------------------------------------
 * Name: PhyPlmeGetANTXStateRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeGetANTXStateRequest(void)
{
    uint8_t currentState;

    currentState = MC1324xDrv_IndirectAccessSPIRead(ANT_AGC_CTRL);

    return ((currentState & cANT_AGC_CTRL_ANTX_Mask_c) >> cANT_AGC_CTRL_ANTX_Shift_c);
}

/*---------------------------------------------------------------------------
 * Name: PhyPlmeSetLQIModeRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeSetLQIModeRequest(uint8_t lqiMode)
{
    uint8_t currentMode;

    currentMode = MC1324xDrv_IndirectAccessSPIRead(CCA_CTRL);
    lqiMode ? (currentMode |= cCCA_CTRL_LQI_RSSI_NOT_CORR) : (currentMode &= (~((uint8_t)cCCA_CTRL_LQI_RSSI_NOT_CORR)));
    MC1324xDrv_IndirectAccessSPIWrite(CCA_CTRL, currentMode);

    return gPhySuccess_c;
}

/*---------------------------------------------------------------------------
 * Name: PhyPlmeGetRSSILevelRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeGetRSSILevelRequest(void)
{
    uint8_t rssiLevel;

    rssiLevel = MC1324xDrv_IndirectAccessSPIRead(RSSI);

    return rssiLevel;
}

/*******************************************************************************/
