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
uint32_t mPhyTimeout;

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
 * Name: PhyTimeSetEventTrigger
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeSetEventTrigger
(
  uint16_t startTime
)
{
  uint8_t phyCtrl1Reg, phyCtrl3Reg, irqSts3Reg;

  uint32_t irqMaskRegister;
  

  uint16_t startTimeTmp = startTime;

  irqMaskRegister = IntDisableAll();  
  
  phyCtrl1Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL1);
  phyCtrl3Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL3);
  
  
  phyCtrl3Reg &= ~(cPHY_CTRL3_TMR2CMP_EN);// disable TMR2 compare
  phyCtrl1Reg |= cPHY_CTRL1_TMRTRIGEN;    // enable autosequence start by TC2 match
  
  
  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL3, phyCtrl3Reg);
  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL1, phyCtrl1Reg);


  MC1324xDrv_DirectAccessSPIMultiByteWrite( (uint8_t) T2PRIMECMP_LSB, (uint8_t *) &startTimeTmp, 2);
  
  irqSts3Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS3);
  
  // TC2PRIME_EN must be enabled in PHY_CTRL4 register
  phyCtrl3Reg |= cPHY_CTRL3_TMR2CMP_EN;   // enable TMR2 compare
  irqSts3Reg &= ~(cIRQSTS3_TMR2MSK);      // unmask TMR2 interrupt
  irqSts3Reg &= 0xF0;                     // do not change other IRQs status
  irqSts3Reg |= (cIRQSTS3_TMR2IRQ);       // aknowledge TMR2 IRQ 
  
  
  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL3, phyCtrl3Reg);
  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) IRQSTS3, irqSts3Reg);
    
  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PhyTimeSetEventTimeout
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeSetEventTimeout
(
  zbClock24_t *pEndTime
)
{
  uint8_t phyCtrl4Reg, phyCtrl3Reg, irqSts3Reg;
  uint32_t irqMaskRegister;

#ifdef PHY_PARAMETERS_VALIDATION
  if(NULL == pEndTime)
  {
    return;
  }
#endif // PHY_PARAMETERS_VALIDATION

  irqMaskRegister = IntDisableAll();

  phyCtrl4Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL4);
  phyCtrl3Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL3);

  phyCtrl3Reg &= ~(cPHY_CTRL3_TMR3CMP_EN);// disable TMR3 compare
  phyCtrl4Reg |= cPHY_CTRL4_TC3TMOUT;     // enable autosequence stop by TC3 match

  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL3, phyCtrl3Reg);
  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL4, phyCtrl4Reg);

  mPhyTimeout = *pEndTime & 0x00FFFFFF;
  MC1324xDrv_DirectAccessSPIMultiByteWrite( (uint8_t) T3CMP_LSB, (uint8_t *) pEndTime, 3);

  irqSts3Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS3);

  phyCtrl3Reg |= cPHY_CTRL3_TMR3CMP_EN;   // enable TMR3 compare
  irqSts3Reg &= ~(cIRQSTS3_TMR3MSK);      // unmask TMR3 interrupt
  irqSts3Reg &= 0xF0;                     // do not change IRQ status
  irqSts3Reg |= (cIRQSTS3_TMR3IRQ);       // aknowledge TMR3 IRQ

  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL3, phyCtrl3Reg);
  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) IRQSTS3, irqSts3Reg);

  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PhyTimeGetEventTimeout
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/

uint32_t PhyTimeGetEventTimeout()
{
    return mPhyTimeout;
}

/*---------------------------------------------------------------------------
 * Name: PhyTimeDisableEventTimeout
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeDisableEventTimeout
(
  void
)
{
  uint8_t phyCtrl4Reg, phyCtrl3Reg, irqSts3Reg;
  
  uint32_t irqMaskRegister;
  irqMaskRegister = IntDisableAll();
  
  PHY_DEBUG_LOG(PDBG_DISABLE_EVENT_TIMEOUT);
  
  phyCtrl4Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL4);
  phyCtrl3Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL3);
  
  phyCtrl4Reg &= ~(cPHY_CTRL4_TC3TMOUT);  // disable autosequence stop by TC3 match
  phyCtrl3Reg &= ~(cPHY_CTRL3_TMR3CMP_EN);// disable TMR3 compare
  
  irqSts3Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS3);
  irqSts3Reg |= cIRQSTS3_TMR3IRQ;         // aknowledge TMR3 IRQ
  
  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL3, phyCtrl3Reg);
  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL4, phyCtrl4Reg);
  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) IRQSTS3, irqSts3Reg);
  
  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PhyTimeReadClock
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeReadClock
(
  zbClock24_t *pRetClk
)
{
  uint32_t irqMaskRegister;
#ifdef PHY_PARAMETERS_VALIDATION
  if(NULL == pRetClk)
  {
    return;
  }
#endif // PHY_PARAMETERS_VALIDATION

  irqMaskRegister = IntDisableAll();
  
  PHY_DEBUG_LOG(PDBG_READ_CLOCK);
  
  MC1324xDrv_DirectAccessSPIMultiByteRead( (uint8_t) EVENT_TMR_LSB, (uint8_t *) pRetClk, 3);
  *(((uint8_t *)pRetClk) + 3) = 0;

  IntRestoreAll(irqMaskRegister);
  
}

/*---------------------------------------------------------------------------
 * Name: PhyTimeInitEventTimer
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeInitEventTimer
(
  zbClock24_t *pAbsTime
)
{
  uint8_t phyCtrl4Reg;
  uint32_t irqMaskRegister;
  
#ifdef PHY_PARAMETERS_VALIDATION
  if(NULL == pAbsTime)
  {
    return;
  }
#endif // PHY_PARAMETERS_VALIDATION

  irqMaskRegister = IntDisableAll();
  
  PHY_DEBUG_LOG(PDBG_INIT_EVENT_TIMER);

  phyCtrl4Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL4);
  phyCtrl4Reg |= cPHY_CTRL4_TMRLOAD; // self clearing bit
  
  MC1324xDrv_DirectAccessSPIMultiByteWrite( (uint8_t) T1CMP_LSB, (uint8_t *) pAbsTime, 3);
  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL4, phyCtrl4Reg);
  
  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PhyTimeSetWakeUpTime
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeSetWakeUpTime
(
  zbClock24_t *pWakeUpTime
)
{
  uint8_t phyCtrl3Reg, irqSts3Reg;
  
  uint32_t irqMaskRegister;
  irqMaskRegister = IntDisableAll();
  
  phyCtrl3Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL3);
  
  
  phyCtrl3Reg &= ~(cPHY_CTRL3_TMR4CMP_EN);// disable TMR4 compare
  
  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL3, phyCtrl3Reg);

  MC1324xDrv_DirectAccessSPIMultiByteWrite( (uint8_t) T4CMP_LSB, (uint8_t *) pWakeUpTime, 3);
  
  irqSts3Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS3);
  
  
  phyCtrl3Reg |= cPHY_CTRL3_TMR4CMP_EN;   // enable TMR4 compare
  irqSts3Reg &= ~(cIRQSTS3_TMR4MSK);      // unmask TMR4 interrupt
  irqSts3Reg &= 0xF0;                     // do not change other IRQs status
  irqSts3Reg |= (cIRQSTS3_TMR4IRQ);       // aknowledge TMR2 IRQ 
  
  
  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL3, phyCtrl3Reg);
  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) IRQSTS3, irqSts3Reg);
  
  IntRestoreAll(irqMaskRegister);

}

/*---------------------------------------------------------------------------
 * Name: PhyTimeIsWakeUpTimeExpired
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PhyTimeIsWakeUpTimeExpired
(
  void
)
{
  bool_t wakeUpIrq = FALSE;
  uint8_t phyCtrl3Reg, irqSts3Reg;
  uint32_t irqMaskRegister;
  irqMaskRegister = IntDisableAll();
  
  phyCtrl3Reg = MC1324xDrv_DirectAccessSPIRead(PHY_CTRL3);
  irqSts3Reg = MC1324xDrv_DirectAccessSPIRead(IRQSTS3);
  
  phyCtrl3Reg &= ~(cPHY_CTRL3_TMR4CMP_EN);// disable TMR4 compare
  
  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL3, phyCtrl3Reg);
  
  if( (irqSts3Reg & cIRQSTS3_TMR4IRQ) == cIRQSTS3_TMR4IRQ )
  {
    wakeUpIrq = TRUE;
  }
  
  irqSts3Reg &= ~(cIRQSTS3_TMR4MSK);      // unmask TMR4 interrupt
  irqSts3Reg &= 0xF0;                     // do not change other IRQs status
  irqSts3Reg |= (cIRQSTS3_TMR4IRQ);       // aknowledge TMR2 IRQ 

  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) IRQSTS3, irqSts3Reg);
  
  IntRestoreAll(irqMaskRegister);
  
  return wakeUpIrq;

}
