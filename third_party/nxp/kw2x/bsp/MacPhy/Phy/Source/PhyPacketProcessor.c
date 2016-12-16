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
#include "MC1324xOverwrites.h"
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

// Address mode indentifiers. Used for both network and MAC interfaces
#define gPhyAddrModeNoAddr_c        (0)
#define gPhyAddrModeInvalid_c       (1)
#define gPhyAddrMode16BitAddr_c     (2)
#define gPhyAddrMode64BitAddr_c     (3)

#define PHY_MIN_RNG_DELAY 4

static uint8_t mPhyCurrentSamLvl = 12;
static bool_t  mActivProm = FALSE;

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
 * Name: PhySetDtsMode
 * Description: Set Tx Data Source Selector
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhySetDtsMode(uint8_t mode)
{
    uint8_t phyReg;

    phyReg = MC1324xDrv_IndirectAccessSPIRead(TX_MODE_CTRL);
    phyReg &= ~cTX_MODE_CTRL_DTS_MASK;   // Clear DTS_MODE
    phyReg |= mode; // Set new DTS_MODE
    MC1324xDrv_IndirectAccessSPIWrite(TX_MODE_CTRL, phyReg);
}

/*---------------------------------------------------------------------------
 * Name: PhyEnableBER
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyEnableBER()
{
    uint8_t phyReg;

    phyReg = MC1324xDrv_IndirectAccessSPIRead(DTM_CTRL1);
    phyReg |= cDTM_CTRL1_DTM_EN;
    MC1324xDrv_IndirectAccessSPIWrite(DTM_CTRL1, phyReg);

    phyReg = MC1324xDrv_IndirectAccessSPIRead(TESTMODE_CTRL);
    phyReg |= cTEST_MODE_CTRL_CONTINUOUS_EN | cTEST_MODE_CTRL_IDEAL_PFC_EN;
    MC1324xDrv_IndirectAccessSPIWrite(TESTMODE_CTRL, phyReg);
}

/*---------------------------------------------------------------------------
 * Name: PhyDisableBER
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyDisableBER()
{
    uint8_t phyReg;

    phyReg = MC1324xDrv_IndirectAccessSPIRead(DTM_CTRL1);
    phyReg &= ~cDTM_CTRL1_DTM_EN;
    MC1324xDrv_IndirectAccessSPIWrite(DTM_CTRL1, phyReg);

    phyReg = MC1324xDrv_IndirectAccessSPIRead(TESTMODE_CTRL);
    phyReg &= ~(cTEST_MODE_CTRL_CONTINUOUS_EN | cTEST_MODE_CTRL_IDEAL_PFC_EN);
    MC1324xDrv_IndirectAccessSPIWrite(TESTMODE_CTRL, phyReg);
}

/*---------------------------------------------------------------------------
 * Name: PhySetActivePromiscuous()
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhySetActivePromiscuous(bool_t state)
{
    uint8_t phyReg;

    if (mActivProm == state)
    {
        return;
    }

    mActivProm = state;

    /* if Prom is set */
    if (TRUE == state)
    {
        phyReg = MC1324xDrv_DirectAccessSPIRead((uint8_t) PHY_CTRL4);

        if (phyReg & cPHY_CTRL4_PROMISCUOUS)
        {
            /* Disable Promiscuous mode */
            phyReg &= ~(cPHY_CTRL4_PROMISCUOUS);
            MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL4, phyReg);

            /* Enable Active Promiscuous mode */
            phyReg = MC1324xDrv_IndirectAccessSPIRead(RX_FRAME_FILTER);
            phyReg |= cRX_FRAME_FLT_ACTIVE_PROMISCUOUS;
            MC1324xDrv_IndirectAccessSPIWrite(RX_FRAME_FILTER, phyReg);
        }
    }
    else
    {
        phyReg = MC1324xDrv_IndirectAccessSPIRead(RX_FRAME_FILTER);

        if (phyReg & cRX_FRAME_FLT_ACTIVE_PROMISCUOUS)
        {
            /* Disable Active Promiscuous mode */
            phyReg &= ~(cRX_FRAME_FLT_ACTIVE_PROMISCUOUS);
            MC1324xDrv_IndirectAccessSPIWrite(RX_FRAME_FILTER, phyReg);

            /* Enable Promiscuous mode */
            phyReg = MC1324xDrv_DirectAccessSPIRead((uint8_t) PHY_CTRL4);
            phyReg |= cPHY_CTRL4_PROMISCUOUS;
            MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL4, phyReg);
        }
    }
}

/*---------------------------------------------------------------------------
 * Name: PhyGetActivePromiscuous()
 * Description: - returns the state of ActivePromiscuous feature (Enabled/Disabled)
 * Parameters: -
 * Return: - TRUE/FALSE
 *---------------------------------------------------------------------------*/
bool_t PhyGetActivePromiscuous(void)
{
    return mActivProm;
}

/*---------------------------------------------------------------------------
 * Name: PhyGetRandomNo
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/

void PhyGetRandomNo(uint32_t *pRandomNo)
{
    uint8_t i = 4, prevRN = 0;
    uint8_t *ptr = (uint8_t *)pRandomNo;
    uint32_t startTime, endTime;

    MC1324xDrv_IRQ_Disable();
    MC1324xDrv_IRQ_Clear();

    while (i--)
    {
        PhyTimeReadClock(&startTime);

        // Program a new sequence
        MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL1, 0x01);

        // wait a variable number of symbols */
        do
        {
            PhyTimeReadClock(&endTime);
        }
        while (((endTime - startTime) & 0x00FFFFFF) < (PHY_MIN_RNG_DELAY + (prevRN >> 4)));

        // Abort the sequence
        MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL1, 0x00);

        // Read new 8 bit random number
        prevRN = MC1324xDrv_IndirectAccessSPIRead((uint8_t)RNG);
        *ptr++ = prevRN;
    }

    MC1324xDrv_IRQ_Enable();
}


/*---------------------------------------------------------------------------
 * Name: PhyPpSetDualPanAuto
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetDualPanAuto
(
    bool_t mode
)
{
    uint8_t phyReg, phyReg2;

    phyReg = MC1324xDrv_IndirectAccessSPIRead((uint8_t) DUAL_PAN_CTRL);

    if (TRUE == mode)
    {
        phyReg2 = phyReg | (cDUAL_PAN_CTRL_DUAL_PAN_AUTO);
    }
    else
    {
        phyReg2 = phyReg & (~cDUAL_PAN_CTRL_DUAL_PAN_AUTO);
    }

    /* Write the new value only if it has changed */
    if (phyReg2 != phyReg)
    {
        MC1324xDrv_IndirectAccessSPIWrite((uint8_t) DUAL_PAN_CTRL, phyReg2);
    }
}

/*---------------------------------------------------------------------------
 * Name: PhyPpSetDualPanDwell
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetDualPanDwell
(
    uint8_t dwell
)
{
    MC1324xDrv_IndirectAccessSPIWrite((uint8_t) DUAL_PAN_DWELL, dwell);
}

/*---------------------------------------------------------------------------
 * Name: PhyPpGetDualPanRemain
 * Description: -
 * Parameters: -
 * Return: - the remaining Dwell time
 *---------------------------------------------------------------------------*/
uint8_t PhyPpGetDualPanRemain()
{
    return (MC1324xDrv_IndirectAccessSPIRead(DUAL_PAN_STS) & cDUAL_PAN_STS_DUAL_PAN_REMAIN);
}

/*---------------------------------------------------------------------------
 * Name: PhyPpSetDualPanSamLvl
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetDualPanSamLvl
(
    uint8_t level
)
{
    uint8_t phyReg;
#ifdef PHY_PARAMETERS_VALIDATION

    if (level > 12)
    {
        return;
    }

#endif
    phyReg = MC1324xDrv_IndirectAccessSPIRead((uint8_t) DUAL_PAN_CTRL);

    phyReg &= ~cDUAL_PAN_CTRL_DUAL_PAN_SAM_LVL_MSK; // clear current lvl
    phyReg |= level << cDUAL_PAN_CTRL_DUAL_PAN_SAM_LVL_Shift_c; // set new lvl

    MC1324xDrv_IndirectAccessSPIWrite((uint8_t) DUAL_PAN_CTRL, phyReg);
    mPhyCurrentSamLvl = level;
}

/*---------------------------------------------------------------------------
 * Name: PhyPpGetDualPanSamLvl
 * Description: -
 * Parameters: -
 * Return:
 *---------------------------------------------------------------------------*/
uint8_t PhyPpGetDualPanSamLvl()
{
    return mPhyCurrentSamLvl;
}

/*---------------------------------------------------------------------------
 * Name: PhyPpSetDualPanActiveNwk
 * Description: - Select Active PAN
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetDualPanActiveNwk
(
    uint8_t nwk
)
{
    uint8_t phyReg, phyReg2;

    phyReg = MC1324xDrv_IndirectAccessSPIRead((uint8_t) DUAL_PAN_CTRL);

    if (TRUE == nwk)
    {
        phyReg2 = phyReg | cDUAL_PAN_CTRL_ACTIVE_NETWORK;
    }
    else
    {
        phyReg2 = phyReg & (~cDUAL_PAN_CTRL_ACTIVE_NETWORK);
    }

    /* Write the new value only if it has changed */
    if (phyReg2 != phyReg)
    {
        MC1324xDrv_IndirectAccessSPIWrite((uint8_t) DUAL_PAN_CTRL, phyReg2);
    }
}

/*---------------------------------------------------------------------------
 * Name: PhyPpGetDualPanActiveNwk
 * Description: -
 * Parameters: -
 * Return: - the Active PAN
 *---------------------------------------------------------------------------*/
uint8_t PhyPpGetDualPanActiveNwk()
{
    uint8_t phyReg;

    phyReg = MC1324xDrv_IndirectAccessSPIRead((uint8_t) DUAL_PAN_CTRL);
    return (phyReg & cDUAL_PAN_CTRL_CURRENT_NETWORK) > 0;
}

/*---------------------------------------------------------------------------
 * Name: PhyPpGetDualPanNwkOfRxPacket
 * Description: -
 * Parameters: -
 * Return: - the Active PAN
 *---------------------------------------------------------------------------*/
uint8_t PhyPpGetPanOfRxPacket()
{
    uint8_t phyReg;

    phyReg = MC1324xDrv_IndirectAccessSPIRead((uint8_t) DUAL_PAN_STS);
    phyReg &= cDUAL_PAN_STS_RECD_ON_PAN1 | cDUAL_PAN_STS_RECD_ON_PAN0;

    return phyReg;
}

/*---------------------------------------------------------------------------
 * Name: PhyPpSetPromiscuous
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetPromiscuous
(
    bool_t mode
)
{
    uint8_t rxFrameFltReg, phyCtrl4Reg;

    PHY_DEBUG_LOG(PDBG_PP_SET_PROMISC);

    rxFrameFltReg = MC1324xDrv_IndirectAccessSPIRead((uint8_t) RX_FRAME_FILTER);
    phyCtrl4Reg = MC1324xDrv_DirectAccessSPIRead((uint8_t) PHY_CTRL4);

    if (TRUE == mode)
    {
        /* FRM_VER[1:0] = b00. 00: Any FrameVersion accepted (0,1,2 & 3) */
        /* All frame types accepted*/
        if (FALSE == mActivProm)
        {
            phyCtrl4Reg |= cPHY_CTRL4_PROMISCUOUS;
        }
        else
        {
            rxFrameFltReg |= cRX_FRAME_FLT_ACTIVE_PROMISCUOUS;
        }

        rxFrameFltReg &= ~(cRX_FRAME_FLT_FRM_VER);
        rxFrameFltReg |= (cRX_FRAME_FLT_ACK_FT | cRX_FRAME_FLT_NS_FT);
    }
    else
    {
        phyCtrl4Reg &= ~cPHY_CTRL4_PROMISCUOUS;
        /* FRM_VER[1:0] = b11. Accept FrameVersion 0 and 1 packets, reject all others */
        /* Beacon, Data and MAC command frame types accepted */
        rxFrameFltReg &= ~(cRX_FRAME_FLT_FRM_VER |
                           cRX_FRAME_FLT_ACK_FT  |
                           cRX_FRAME_FLT_NS_FT   |
                           cRX_FRAME_FLT_ACTIVE_PROMISCUOUS);
        rxFrameFltReg |= (0x03 << cRX_FRAME_FLT_FRM_VER_Shift_c);
    }

    MC1324xDrv_IndirectAccessSPIWrite((uint8_t) RX_FRAME_FILTER, rxFrameFltReg);
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL4, phyCtrl4Reg);
}

/*---------------------------------------------------------------------------
 * Name: PhyPpSetPanIdPAN0
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetPanIdPAN0
(
    uint8_t *pPanId
)
{
    PHY_DEBUG_LOG(PDBG_PP_SET_PANID_PAN0);

#ifdef PHY_PARAMETERS_VALIDATION

    if (NULL == pPanId)
    {
        return;
    }

#endif // PHY_PARAMETERS_VALIDATION

    MC1324xDrv_IndirectAccessSPIMultiByteWrite((uint8_t) MACPANID0_LSB, pPanId, 2);
}

/*---------------------------------------------------------------------------
 * Name: PhyPpSetPanIdPAN1
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetPanIdPAN1
(
    uint8_t *pPanId
)
{
    PHY_DEBUG_LOG(PDBG_PP_SET_PANID_PAN1);

#ifdef PHY_PARAMETERS_VALIDATION

    if (NULL == pPanId)
    {
        return;
    }

#endif // PHY_PARAMETERS_VALIDATION

    MC1324xDrv_IndirectAccessSPIMultiByteWrite((uint8_t) MACPANID1_LSB, pPanId, 2);
}

/*---------------------------------------------------------------------------
 * Name: PhyPpSetShortAddrPAN0
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetShortAddrPAN0
(
    uint8_t *pShortAddr
)
{
    PHY_DEBUG_LOG(PDBG_PP_SET_SHORTADDR_PAN0);

#ifdef PHY_PARAMETERS_VALIDATION

    if (NULL == pShortAddr)
    {
        return;
    }

#endif // PHY_PARAMETERS_VALIDATION

    MC1324xDrv_IndirectAccessSPIMultiByteWrite((uint8_t) MACSHORTADDRS0_LSB, pShortAddr, 2);
}

/*---------------------------------------------------------------------------
 * Name: PhyPpSetShortAddrPAN1
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetShortAddrPAN1
(
    uint8_t *pShortAddr
)
{
    PHY_DEBUG_LOG(PDBG_PP_SET_SHORTADDR_PAN1);

#ifdef PHY_PARAMETERS_VALIDATION

    if (NULL == pShortAddr)
    {
        return;
    }

#endif // PHY_PARAMETERS_VALIDATION

    MC1324xDrv_IndirectAccessSPIMultiByteWrite((uint8_t) MACSHORTADDRS1_LSB, pShortAddr, 2);
}

/*---------------------------------------------------------------------------
 * Name: PhyPpSetLongAddrPAN0
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetLongAddrPAN0
(
    uint8_t *pLongAddr
)
{
    PHY_DEBUG_LOG(PDBG_PP_SET_LONGADDR_PAN0);

#ifdef PHY_PARAMETERS_VALIDATION

    if (NULL == pLongAddr)
    {
        return;
    }

#endif // PHY_PARAMETERS_VALIDATION

    MC1324xDrv_IndirectAccessSPIMultiByteWrite((uint8_t) MACLONGADDRS0_0, pLongAddr, 8);
}

/*---------------------------------------------------------------------------
 * Name: PhyPpSetLongAddrPAN1
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetLongAddrPAN1
(
    uint8_t *pLongAddr
)
{
    PHY_DEBUG_LOG(PDBG_PP_SET_LONGADDR_PAN1);

#ifdef PHY_PARAMETERS_VALIDATION

    if (NULL == pLongAddr)
    {
        return;
    }

#endif // PHY_PARAMETERS_VALIDATION

    MC1324xDrv_IndirectAccessSPIMultiByteWrite((uint8_t) MACLONGADDRS1_0, pLongAddr, 8);
}

/*---------------------------------------------------------------------------
 * Name: PhyPpSetMacRolePAN0
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetMacRolePAN0
(
    bool_t macRole
)
{
    uint8_t phyCtrl4Reg;

    PHY_DEBUG_LOG(PDBG_PP_SET_MACROLE_PAN0);

    phyCtrl4Reg = MC1324xDrv_DirectAccessSPIRead((uint8_t) PHY_CTRL4);

    if (gMacRole_PanCoord_c == macRole)
    {
        phyCtrl4Reg |=  cPHY_CTRL4_PANCORDNTR0;
    }
    else
    {
        phyCtrl4Reg &= ~cPHY_CTRL4_PANCORDNTR0;
    }

    MC1324xDrv_DirectAccessSPIWrite((uint8_t) PHY_CTRL4, phyCtrl4Reg);
}

/*---------------------------------------------------------------------------
 * Name: PhyPpSetMacRolePAN1
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetMacRolePAN1
(
    bool_t macRole
)
{
    uint8_t dualPanCtrlReg;

    PHY_DEBUG_LOG(PDBG_PP_SET_MACROLE_PAN1);

    dualPanCtrlReg = MC1324xDrv_IndirectAccessSPIRead((uint8_t) DUAL_PAN_CTRL);

    if (gMacRole_PanCoord_c == macRole)
    {
        dualPanCtrlReg |=  cDUAL_PAN_CTRL_PANCORDNTR1;
    }
    else
    {
        dualPanCtrlReg &= ~cDUAL_PAN_CTRL_PANCORDNTR1;
    }

    MC1324xDrv_IndirectAccessSPIWrite((uint8_t) DUAL_PAN_CTRL, dualPanCtrlReg);
}

/*---------------------------------------------------------------------------
 * Name: PhyPpIsTxAckDataPending
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PhyPpIsTxAckDataPending
(
    void
)
{
    uint8_t srcCtrlReg;

    srcCtrlReg = MC1324xDrv_DirectAccessSPIRead(SRC_CTRL);

    if (srcCtrlReg & cSRC_CTRL_SRCADDR_EN)
    {
        uint8_t irqsts2Reg;

        irqsts2Reg = MC1324xDrv_DirectAccessSPIRead((uint8_t) IRQSTS2);

        if (irqsts2Reg & cIRQSTS2_SRCADDR)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return ((srcCtrlReg & cSRC_CTRL_ACK_FRM_PND) == cSRC_CTRL_ACK_FRM_PND);
    }
}

/*---------------------------------------------------------------------------
 * Name: PhyPpIsRxAckDataPending
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PhyPpIsRxAckDataPending
(
    void
)
{
    uint8_t irqsts1Reg;
    irqsts1Reg = MC1324xDrv_DirectAccessSPIRead((uint8_t) IRQSTS1);

    if (irqsts1Reg & cIRQSTS1_RX_FRM_PEND)
    {
        return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------
 * Name: PhyPpIsPollIndication
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PhyPpIsPollIndication
(
    void
)
{
    uint8_t irqsts2Reg;
    irqsts2Reg = MC1324xDrv_DirectAccessSPIRead((uint8_t) IRQSTS2);

    if (irqsts2Reg & cIRQSTS2_PI)
    {
        return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------
 * Name: PhyPpSetCcaThreshold
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetCcaThreshold(uint8_t ccaThreshold)
{
    MC1324xDrv_IndirectAccessSPIWrite((uint8_t) CCA1_THRESH, (uint8_t) ccaThreshold);
}

/*---------------------------------------------------------------------------
 * Name: PhyPp_AddToIndirect
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPp_AddToIndirect
(
    uint8_t index,
    uint8_t *pPanId,
    uint8_t *pAddr,
    uint8_t AddrMode
)
{
    uint16_t srcAddressCheckSum;
    uint8_t srcCtrlReg;

    PHY_DEBUG_LOG1(PDBG_PP_ADD_INDIRECT, index);

#ifdef PHY_PARAMETERS_VALIDATION

    if ((NULL == pPanId) || (NULL == pAddr))
    {
        return;
    }

    if ((gPhyAddrMode64BitAddr_c != AddrMode) && (gPhyAddrMode16BitAddr_c != AddrMode))
    {
        return;
    }

#endif // PHY_PARAMETERS_VALIDATION

    srcAddressCheckSum = (uint16_t)((uint16_t) pPanId[0] | (uint16_t)(pPanId[1] << 8));
    srcAddressCheckSum += (uint16_t)((uint16_t) pAddr[0] | (uint16_t)(pAddr[1] << 8));

    if (AddrMode == gPhyAddrMode64BitAddr_c)
    {
        srcAddressCheckSum += (uint16_t)((uint16_t) pAddr[2] | (uint16_t)(pAddr[3] << 8));
        srcAddressCheckSum += (uint16_t)((uint16_t) pAddr[4] | (uint16_t)(pAddr[5] << 8));
        srcAddressCheckSum += (uint16_t)((uint16_t) pAddr[6] | (uint16_t)(pAddr[7] << 8));
    }

    srcCtrlReg = (uint8_t)((index & cSRC_CTRL_INDEX) << cSRC_CTRL_INDEX_Shift_c);

    MC1324xDrv_DirectAccessSPIWrite((uint8_t) SRC_CTRL, srcCtrlReg);

    MC1324xDrv_DirectAccessSPIMultiByteWrite((uint8_t) SRC_ADDRS_SUM_LSB, (uint8_t *) &srcAddressCheckSum, 2);

    srcCtrlReg |= (cSRC_CTRL_SRCADDR_EN | cSRC_CTRL_INDEX_EN);

    MC1324xDrv_DirectAccessSPIWrite((uint8_t) SRC_CTRL, srcCtrlReg);
}

/*---------------------------------------------------------------------------
 * Name: PhyPp_RemoveFromIndirect
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPp_RemoveFromIndirect
(
    uint8_t index
)
{
    uint8_t srcCtrlReg;

    PHY_DEBUG_LOG1(PDBG_PP_REMOVE_INDIRECT, index);

    srcCtrlReg = (uint8_t)(((index & cSRC_CTRL_INDEX) << cSRC_CTRL_INDEX_Shift_c)
                           | (cSRC_CTRL_SRCADDR_EN)
                           | (cSRC_CTRL_INDEX_DISABLE));

    MC1324xDrv_DirectAccessSPIWrite((uint8_t) SRC_CTRL, srcCtrlReg);
}

/*---------------------------------------------------------------------------
 * Name: PhyPpReadLatestIndex
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPpReadLatestIndex
(
    void
)
{
    uint8_t srcCtrlReg;

    srcCtrlReg = MC1324xDrv_DirectAccessSPIRead((uint8_t) SRC_CTRL);

    srcCtrlReg = (srcCtrlReg >> cSRC_CTRL_INDEX_Shift_c);

    PHY_DEBUG_LOG1(PDBG_PP_READ_LATEST_INDEX, srcCtrlReg);

    return srcCtrlReg;
}

/*---------------------------------------------------------------------------
 * Name: PhyPpGetState
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPpGetState
(
    void
)
{
    return (uint8_t)(MC1324xDrv_DirectAccessSPIRead((uint8_t) PHY_CTRL1) & cPHY_CTRL1_XCVSEQ);
}

/*---------------------------------------------------------------------------
 * Name: PhyAbort
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyAbort(void)
{
    uint8_t phyReg;
    volatile uint8_t currentTime = 0;

    //asm(" MRS %0,PRIMASK" : "=r" (irqMaskRegister) );
    //asm(" CPSID i");
    ProtectFromMC1324xInterrupt();

    PHY_DEBUG_LOG(PDBG_PHY_ABORT);

    // Disable timer trigger (for scheduled XCVSEQ)
    phyReg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL1);

    if (phyReg & cPHY_CTRL1_TMRTRIGEN)
    {
        phyReg &= (uint8_t) ~(cPHY_CTRL1_TMRTRIGEN);
        MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL1, phyReg);

        // give the FSM enough time to start if it was triggered
        currentTime = (uint8_t)(MC1324xDrv_DirectAccessSPIRead(EVENT_TMR_LSB) + (uint8_t) 2);

        while (MC1324xDrv_DirectAccessSPIRead(EVENT_TMR_LSB) != (uint8_t)(currentTime));

        phyReg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL1);
    }

    if ((phyReg & cPHY_CTRL1_XCVSEQ) != gIdle_c)
    {
        // Abort current SEQ
        phyReg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
        MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL1, phyReg);

        // wait for Sequence Idle (if not already)
        while ((MC1324xDrv_DirectAccessSPIRead(SEQ_STATE) & 0x1F) != 0);
    }

    // mask SEQ interrupt
    phyReg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL2);
    phyReg |= (uint8_t)(cPHY_CTRL2_SEQMSK);
    MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL2, phyReg);

    // stop timers
    phyReg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL4);
    phyReg &= (uint8_t) ~(cPHY_CTRL4_TC3TMOUT);
    MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL4, phyReg);

    phyReg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL3);
    phyReg &= (uint8_t) ~(cPHY_CTRL3_TMR2CMP_EN | cPHY_CTRL3_TMR3CMP_EN);
    MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL3, phyReg);

    PhyPassRxParams(NULL);
    PhyPassRxDataPtr(NULL);

    // clear all PP IRQ bits to avoid unexpected interrupts
    phyReg = MC1324xDrv_DirectAccessSPIRead(IRQSTS1);
    MC1324xDrv_DirectAccessSPIWrite(IRQSTS1, phyReg);

    phyReg = MC1324xDrv_DirectAccessSPIRead(IRQSTS2);
    MC1324xDrv_DirectAccessSPIWrite(IRQSTS2, phyReg);

    phyReg = MC1324xDrv_DirectAccessSPIRead(IRQSTS3);
    phyReg |= (uint8_t)(cIRQSTS3_TMR2MSK | cIRQSTS3_TMR3MSK);
    MC1324xDrv_DirectAccessSPIWrite(IRQSTS3, phyReg);

    //asm(" MSR PRIMASK, %0" : : "r" (irqMaskRegister) );
    UnprotectFromMC1324xInterrupt();
}

/*---------------------------------------------------------------------------
 * Name: PhyInit
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyInit(void)
{
    uint8_t index;
    uint8_t phyReg;

#if (gUseStandaloneCCABeforeTx_d == 1)
    phyTxParams_t txParam;

    txParam.useStandaloneCcaBeforeTx = FALSE;
    PhyPassTxParams(&txParam);
#endif


    PHY_DEBUG_LOG(PDBG_PHY_INIT);

    //Disable Tristate on COCO MISO for SPI reads
    MC1324xDrv_IndirectAccessSPIWrite((uint8_t) MISC_PAD_CTRL, (uint8_t) 0x02);

    PhyPassRxParams(NULL);
    PhyPassRxDataPtr(NULL);

    // PHY_CTRL4 unmask global TRX interrupts, enable 16 bit mode for TC2 - TC2 prime EN
    MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL4, (uint8_t)(cPHY_CTRL4_TC2PRIME_EN | \
                                                         (gCcaCCA_MODE1_c << cPHY_CTRL4_CCATYPE_Shift_c)));

    // clear all PP IRQ bits to avoid unexpected interrupts immediately after init, disable all timer interrupts
    MC1324xDrv_DirectAccessSPIWrite(IRQSTS1, (uint8_t)(cIRQSTS1_PLL_UNLOCK_IRQ | \
                                                       cIRQSTS1_FILTERFAIL_IRQ | \
                                                       cIRQSTS1_RXWTRMRKIRQ | \
                                                       cIRQSTS1_CCAIRQ | \
                                                       cIRQSTS1_RXIRQ | \
                                                       cIRQSTS1_TXIRQ | \
                                                       cIRQSTS1_SEQIRQ));

    MC1324xDrv_DirectAccessSPIWrite(IRQSTS2, (uint8_t)(cIRQSTS2_ASM_IRQ | \
                                                       cIRQSTS2_PB_ERR_IRQ | \
                                                       cIRQSTS2_WAKE_IRQ));

    MC1324xDrv_DirectAccessSPIWrite(IRQSTS3, (uint8_t)(cIRQSTS3_TMR4MSK | \
                                                       cIRQSTS3_TMR3MSK | \
                                                       cIRQSTS3_TMR2MSK | \
                                                       cIRQSTS3_TMR1MSK | \
                                                       cIRQSTS3_TMR4IRQ | \
                                                       cIRQSTS3_TMR3IRQ | \
                                                       cIRQSTS3_TMR2IRQ | \
                                                       cIRQSTS3_TMR1IRQ));

    //  PHY_CTRL1 default HW settings  + AUTOACK enabled
    MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL1, (uint8_t)(cPHY_CTRL1_AUTOACK));

    //  PHY_CTRL2 : disable all interrupts
    MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL2, (uint8_t)(cPHY_CTRL2_CRC_MSK | \
                                                         cPHY_CTRL2_PLL_UNLOCK_MSK | \
                                                         cPHY_CTRL2_FILTERFAIL_MSK | \
                                                         cPHY_CTRL2_RX_WMRK_MSK | \
                                                         cPHY_CTRL2_CCAMSK | \
                                                         cPHY_CTRL2_RXMSK | \
                                                         cPHY_CTRL2_TXMSK | \
                                                         cPHY_CTRL2_SEQMSK));

    //  PHY_CTRL3 : disable all timers and remaining interrupts
    MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL3, (uint8_t)(cPHY_CTRL3_ASM_MSK | \
                                                         cPHY_CTRL3_PB_ERR_MSK | \
                                                         cPHY_CTRL3_WAKE_MSK));
    //  SRC_CTRL
    MC1324xDrv_DirectAccessSPIWrite(SRC_CTRL, (uint8_t)(cSRC_CTRL_ACK_FRM_PND | \
                                                        (cSRC_CTRL_INDEX << cSRC_CTRL_INDEX_Shift_c)));
    //  RX_FRAME_FILTER
    //  FRM_VER[1:0] = b11. Accept FrameVersion 0 and 1 packets, reject all others
    MC1324xDrv_IndirectAccessSPIWrite(RX_FRAME_FILTER, (uint8_t)(cRX_FRAME_FLT_FRM_VER | \
                                                                 cRX_FRAME_FLT_BEACON_FT | \
                                                                 cRX_FRAME_FLT_DATA_FT | \
                                                                 cRX_FRAME_FLT_CMD_FT));
    mActivProm = FALSE;

    // Direct register overwrites
    for (index = 0; index < sizeof(overwrites_direct) / sizeof(overwrites_t); index++)
    {
        MC1324xDrv_DirectAccessSPIWrite(overwrites_direct[index].address, overwrites_direct[index].data);
    }

    // Indirect register overwrites
    for (index = 0; index < sizeof(overwrites_indirect) / sizeof(overwrites_t); index++)
    {
        MC1324xDrv_IndirectAccessSPIWrite(overwrites_indirect[index].address, overwrites_indirect[index].data);
    }

    PhyPlmeSetCurrentChannelRequest(0x0B); //2405 MHz

#if gDualPanEnabled_d
    PhyPlmeSetCurrentChannelRequestPAN1(0x0B); //2405 MHz

    // Split the HW Indirect hash table in two
    PhyPpSetDualPanSamLvl(6);
#else
    // Assign HW Indirect hash table to PAN0
    PhyPpSetDualPanSamLvl(12);
#endif

    PhyPlmeSetPwrLevelRequest(0x17); // set the power level to 0dBm
    PhyPpSetCcaThreshold(0x4B);      // set CCA threshold to -75 dBm

    // Set prescaller to obtain 1 symbol (16us) timebase
    MC1324xDrv_IndirectAccessSPIWrite(TMR_PRESCALE, 0x05);

    phyReg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL2);

    phyReg &= (uint8_t)~(cPHY_CTRL2_FILTERFAIL_MSK);

    phyReg &= (uint8_t)~(cPHY_CTRL2_RX_WMRK_MSK);

    MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL2, phyReg);

    MC1324xDrv_IndirectAccessSPIWrite(RX_WTR_MARK, 0);

    /* enable autodoze mode. */
    phyReg = MC1324xDrv_DirectAccessSPIRead((uint8_t) PWR_MODES);
    phyReg |= (uint8_t) cPWR_MODES_AUTODOZE;
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) PWR_MODES, phyReg);

    MC1324xDrv_IRQ_Clear();
    NVIC_ClearPendingIRQ(MC1324x_Irq_Number);
}

