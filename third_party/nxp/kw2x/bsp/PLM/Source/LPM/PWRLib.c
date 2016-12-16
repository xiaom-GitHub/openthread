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
***************************************************************************/
/*****************************************************************************
 *                               INCLUDED HEADERS                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the headers that this module needs to include.    *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
#include "EmbeddedTypes.h"
#include "PortConfig.h"
#include "NVIC.h"
#include "AppAspInterface.h"
#include "PWRLib.h"
#include "PWR_Configuration.h"
#include "TMR_Interface.h"
#include "Keyboard.h"
#include "MC1324xDrv.h"
#include "MC1324xReg.h"

/*****************************************************************************
 *                               PRIVATE VARIABLES                           *
 *---------------------------------------------------------------------------*
 * Add to this section all the variables and constants that have local       *
 * (file) scope.                                                             *
 * Each of this declarations shall be preceded by the 'static' keyword.      *
 * These variables / constants cannot be accessed outside this module.       *
 *---------------------------------------------------------------------------*
 *****************************************************************************/



/* LPTMR/RTC variables */
#if (cPWR_UsePowerDownMode==1)
static uint32_t mPWRLib_RTIRemainingTicks;
#endif /* #if (cPWR_UsePowerDownMode==1) */

#if (cPWR_UsePowerDownMode==1)
/* RTC variables*/
#if gPWR_EnsureOscStabilized_d
#if gTMR_Enabled_d
static tmrTimerID_t mPWRLib_RTC_OscInitTmrID;
#endif
#endif
#endif /* #if (cPWR_UsePowerDownMode==1) */

static volatile bool_t mPWRLib_RTC_IsOscStarted = FALSE;

/* For LVD function */ 
#if ((cPWR_LVD_Enable == 1) || (cPWR_LVD_Enable == 2))
  #if (cPWR_LVD_Enable == 2)
    uint16_t                   PWRLib_LVD_CollectCounter;
    tmrTimerID_t               PWRLib_LVD_PollIntervalTmrID;
  #endif  /* #if (cPWR_LVD_Enable == 2) */
  PWRLib_LVD_VoltageLevel_t    PWRLib_LVD_SavedLevel;
#endif /* #if ((cPWR_LVD_Enable == 1) || (cPWR_LVD_Enable == 2)) */



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

/* Zigbee STACK status */ 
  PWRLib_StackPS_t PWRLib_StackPS;
  
  /*--- Chip status ---*/
#if (cPWR_UseMCUStatus)
  PWRLib_MCUStatus_t PWRLib_MCUStatus;
#endif  /* #if (cPWR_UseMCUStatus) */
  
#if (cPWR_UseRADIOStatus)
  PWRLib_RADIOStatus_t PWRLib_RADIOStatus;
#endif  /* #if (cPWR_UseRADIOStatus) */

volatile PWRLib_WakeupReason_t PWRLib_MCU_WakeupReason;

#if (cPWR_UsePowerDownMode==1)

/*****************************************************************************
 *                           PRIVATE FUNCTIONS PROTOTYPES                    *
 *---------------------------------------------------------------------------*
 * Add to this section all the functions prototypes that have local (file)   *
 * scope.                                                                    *
 * These functions cannot be accessed outside this module.                   *
 * These declarations shall be preceded by the 'static' keyword.             *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/******************************************************************************
 * Name: PWRLib_RTC_OscInitCallback
 * Description:
 *
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
#if gPWR_EnsureOscStabilized_d
#if gTMR_Enabled_d
static void PWRLib_RTC_OscInitCallback
(
 tmrTimerID_t tmrID
);
#endif
#endif

void PWRLib_DelayMs
(
  uint16_t val
);

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
 * Name: PWRLib_MCU_Enter_WAIT
 * Description: WAIT mode entry routine. Puts the processor into wait mode.
 *              In this mode the core clock is disabled (no code executing), but 
 *              bus clocks are enabled (peripheral modules are operational). 
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
 void PWRLib_MCU_Enter_WAIT
(
  void
)
{
  /* Clear the SLEEPDEEP bit to enable cortex M4 WAIT (sleep) mode */
  SCB_SCR &= ~SCB_SCR_SLEEPDEEP_MASK;
  /* WFI instruction will start entry into WAIT (sleep) mode */
  asm("WFI");
}
   

/*---------------------------------------------------------------------------
 * Name: PWRLib_MCUEnter_STOP
 * Description: Puts the processor into normal stop mode.

                Places chip in static state. Lowest power mode that retains all 
                registers while maintaining LVD protection. NVIC is disabled;
                AWIC is used to wake up from interrupt; peripheral clocks are stopped.

                Mode of operation details:
                 - ARM core enters DeepSleep Mode
                 - ARM core is clock gated (HCLK = OFF)
                 - NVIC is disable (FCLK = OFF)
                 - WIC is used to wake up from interruptions
                 - Platform and peripheral clock are stopped
                 - MCG module can be configured to leave reference clocks running
                 - All SRAM is operating (content retained and I/O states held)

                STOP mode is exited into RUN mode using any enabled interrupt or RESET.

 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_MCU_Enter_STOP
(
  void
)
{
  /*  Set the LPLLSM to 0b000, normal sop mode */
#if (MCU_MK60N512VMD100 == 1)
  MC_PMCTRL = (MC_PMCTRL & (MC_PMCTRL_RUNM_MASK | MC_PMCTRL_LPWUI_MASK)) |
               MC_PMCTRL_LPLLSM(0x0) ;
#elif ((MCU_MK60D10 == 1) || (MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
  SMC_PMCTRL = (SMC_PMCTRL & (SMC_PMCTRL_RUNM_MASK | SMC_PMCTRL_LPWUI_MASK)) |
               SMC_PMCTRL_STOPM(0x0) ;
#endif
  
  /* Set the SLEEPDEEP bit to enable CORTEX M4 deep sleep mode */
  SCB_SCR |= SCB_SCR_SLEEPDEEP_MASK;	

  /* WFI instruction will start entry into deep sleep mode */
  asm("WFI");
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_MCUEnter_VLPS
 * Description: Puts the processor into VLPS (Very Low Power Stop).

                Mode of operation details:
                 - ARM core enters DeepSleep Mode
                 - ARM core is clock gated (HCLK = OFF)
                 - NVIC is disable (FCLK = OFF)
                 - WIC is used to wake up from interruptions
                 - Platform and peripheral clock are stopped
                 - MCG module can be configured to leave reference clocks running
                 - On chip voltage regulator is in a mode that supplies only enough
                   power to run the MCU in a reduced frequency
                 - All SRAM is operating (content retained and I/O states held)

                VLPS mode is exited into RUN mode using any enabled interrupt (with LPWUI =1) or RESET.
                
                The AVLP must be set to 0b1 in MC_PMPROT register in order to allow VPLS mode.

 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_MCU_Enter_VLPS
(
  void
)
{
  /*  Set the LPLLSM to 0b000, very low power stop (VLPS) */
  /*  Set the LPWUI to 0b1, The voltage regulator exits stop regulation on an interrupt*/
#if (MCU_MK60N512VMD100 == 1)
  MC_PMCTRL = (MC_PMCTRL & (MC_PMCTRL_RUNM_MASK | MC_PMCTRL_LPWUI_MASK)) |
               MC_PMCTRL_LPLLSM(0x2) | 
               MC_PMCTRL_LPWUI_MASK ;
#elif ((MCU_MK60D10 == 1) || (MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
  SMC_PMCTRL = (SMC_PMCTRL & (SMC_PMCTRL_RUNM_MASK | SMC_PMCTRL_LPWUI_MASK)) |
               SMC_PMCTRL_STOPM(0x2) | 
               SMC_PMCTRL_LPWUI_MASK ;
#endif
  
  /* Set the SLEEPDEEP bit to enable CORTEX M4 deep sleep mode */
  SCB_SCR |= SCB_SCR_SLEEPDEEP_MASK;	

  /* WFI instruction will start entry into deep sleep mode */
  asm("WFI");
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_MCUEnter_LLS
 * Description: Puts the processor into LLS (Low Leakage Stop).

                Mode of operation details:
                 - ARM core enters Deep Sleep Mode
                 - ARM core is clock gated (HCLK = OFF)
                 - NVIC is disable (FCLK = OFF)
                 - LLWU is used to wake up from interruptions
                 - Platform and peripheral clock are stopped
                 - MCG module can be configured to leave reference clocks running
                 - On chip voltage regulator is in a mode that supplies only enough 
                   power to run the MCU in a reduced frequency
                 - All SRAM is operating (content retained and I/O states held)
                 - Most of peripherals are in state retention mode (cannot operate)

                LLS mode is exited into RUN mode using LLWU module or RESET.

                The ALLP must be set to 0b1 in MC_PMPROT register in order to allow LLS mode.

 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_MCU_Enter_LLS
(
  void
)
{

  /* Set the LPLLSM field to 0b011 for LLS mode  */
#if (MCU_MK60N512VMD100 == 1)
  MC_PMCTRL = (MC_PMCTRL & (MC_PMCTRL_RUNM_MASK | MC_PMCTRL_LPWUI_MASK)) |
               MC_PMCTRL_LPLLSM(0x3) ;
#elif ((MCU_MK60D10 == 1) || (MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
  SMC_PMCTRL = (SMC_PMCTRL & (SMC_PMCTRL_RUNM_MASK | SMC_PMCTRL_LPWUI_MASK)) |
               SMC_PMCTRL_STOPM(0x3) ;  
#endif

  /* Set the SLEEPDEEP bit to enable CORTEX M4 deep sleep mode */
  SCB_SCR |= SCB_SCR_SLEEPDEEP_MASK;	
  
  /* WFI instruction will start entry into deep sleep mode */
  asm("WFI");
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_MCUEnter_VLLS3
 * Description: Puts the processor into VLLS3 (Very Low Leakage Stop3).

                Mode of operation details:
                 - ARM core enters SleepDeep Mode
                 - ARM core is clock gated (HCLK = OFF)
                 - NVIC is disable (FCLK = OFF)
                 - LLWU should configure by user to enable the desire wake up source
                 - Platform and peripheral clock are stopped
                 - MCG module can be configured to leave reference clocks running
                 - On chip voltage regulator is in a mode that supplies only enough
                   power to run the MCU in a reduced frequency
                 - All SRAM is operating (content retained and I/O states held)
                 - Most modules are disabled

                VLLS3 mode is exited into RUN mode using LLWU module or RESET.
                All wakeup goes through Reset sequence.

                The AVLLS3 must be set to 0b1 in MC_PMPROT register in order to allow VLLS3 mode.

 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_MCU_Enter_VLLS3
(
  void
)
{
  /* Set the LPLLSM field to 0b101 for VLLS3 mode */
#if (MCU_MK60N512VMD100 == 1)
  MC_PMCTRL = (MC_PMCTRL & (MC_PMCTRL_RUNM_MASK | MC_PMCTRL_LPWUI_MASK)) |
               MC_PMCTRL_LPLLSM(0x5) ;
#elif ((MCU_MK60D10 == 1)  || (MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
  SMC_PMCTRL = (SMC_PMCTRL & (SMC_PMCTRL_RUNM_MASK | SMC_PMCTRL_LPWUI_MASK)) |
               SMC_PMCTRL_STOPM(0x4) ;
  SMC_VLLSCTRL = SMC_VLLSCTRL_VLLSM(0x3);
#endif
  
  /* Set the SLEEPDEEP bit to enable CORTEX M4 deep sleep mode */
  SCB_SCR |= SCB_SCR_SLEEPDEEP_MASK;	
  
  /* WFI instruction will start entry into deep sleep mode */
  asm("WFI");
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_MCUEnter_VLLS2
 * Description: Puts the processor into VLLS2 (Very Low Leakage Stop2).

                Mode of operation details:
                 - ARM core enters SleepDeep Mode
                 - ARM core is clock gated (HCLK = OFF)
                 - NVIC is disable (FCLK = OFF)
                 - LLWU should configure by user to enable the desire wake up source
                 - Platform and peripheral clock are stopped
                 - MCG module can be configured to leave reference clocks running
                 - On chip voltage regulator is in a mode that supplies only enough
                   power to run the MCU in a reduced frequency
                 - SRAM_L is powered off. A portion of SRAM_U remains powered on (content retained and I/O state held).
                 - Most modules are disabled

                VLLS2 mode is exited into RUN mode using LLWU module or RESET.
                All wakeup goes through Reset sequence.

                The AVLLS2 must be set to 0b1 in MC_PMPROT register in order to allow VLLS2 mode.

 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_MCU_Enter_VLLS2
(
  void
)
{
  /* Set the LPLLSM field to 0b110 for VLLS2 mode */
#if (MCU_MK60N512VMD100 == 1)
  MC_PMCTRL = (MC_PMCTRL & (MC_PMCTRL_RUNM_MASK | MC_PMCTRL_LPWUI_MASK)) |
               MC_PMCTRL_LPLLSM(0x6) ;
#elif ((MCU_MK60D10 == 1)  || (MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
  SMC_PMCTRL = (SMC_PMCTRL & (SMC_PMCTRL_RUNM_MASK | SMC_PMCTRL_LPWUI_MASK)) |
               SMC_PMCTRL_STOPM(0x4) ;
  SMC_VLLSCTRL = (SMC_VLLSCTRL_VLLSM(0x2) | cPWR_RAM2PowerOption );
#endif
  
  /* Set the SLEEPDEEP bit to enable CORTEX M4 deep sleep mode */
  SCB_SCR |= SCB_SCR_SLEEPDEEP_MASK;	
  
  /* WFI instruction will start entry into deep sleep mode */
  asm("WFI");
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_MCUEnter_VLLS1
 * Description: Puts the processor into VLLS1 (Very Low Leakage Stop2).

                Mode of operation details:
                 - ARM core enters SleepDeep Mode
                 - ARM core is clock gated (HCLK = OFF)
                 - NVIC is disable (FCLK = OFF)
                 - LLWU should configure by user to enable the desire wake up source
                 - Platform and peripheral clock are stopped
                 - MCG module can be configured to leave reference clocks running
                 - On chip voltage regulator is in a mode that supplies only enough
                   power to run the MCU in a reduced frequency
                 - SRAM_L and SRAM_H is powered off.
                 - Most modules are disabled

                VLLS1 mode is exited into RUN mode using LLWU module or RESET.
                All wakeup goes through Reset sequence.

                The AVLLS1 must be set to 0b1 in MC_PMPROT register in order to allow VLLS2 mode.

 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_MCU_Enter_VLLS1
(
  void
)
{
  /* Set the LPLLSM field to 0b110 for VLLS2 mode */
#if (MCU_MK60N512VMD100 == 1)
  MC_PMCTRL = (MC_PMCTRL & (MC_PMCTRL_RUNM_MASK | MC_PMCTRL_LPWUI_MASK)) |
               MC_PMCTRL_LPLLSM(0x7) ;
#elif ((MCU_MK60D10 == 1)  || (MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
  SMC_PMCTRL = (SMC_PMCTRL & (SMC_PMCTRL_RUNM_MASK | SMC_PMCTRL_LPWUI_MASK)) |
               SMC_PMCTRL_STOPM(0x4) ;
  SMC_VLLSCTRL = SMC_VLLSCTRL_VLLSM(0x1);
#endif
  
  /* Set the SLEEPDEEP bit to enable CORTEX M4 deep sleep mode */
  SCB_SCR |= SCB_SCR_SLEEPDEEP_MASK;	
  
  /* WFI instruction will start entry into deep sleep mode */
  asm("WFI");
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_Radio_Enter_Doze
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_Radio_Enter_Doze
(
  void
)
{
  uint32_t irqMaskRegister;
  uint8_t phyCtrl1Reg, irqSts1Reg, pwrModesReg;
  irqMaskRegister = IntDisableAll();

  pwrModesReg = MC1324xDrv_DirectAccessSPIRead( (uint8_t) PWR_MODES);
  /* disable autodoze mode. sets PMC in low-power mode */
  pwrModesReg &= (uint8_t) ~( cPWR_MODES_AUTODOZE | cPWR_MODES_PMC_MODE );
  /* check if 32 MHz crystal oscillator is enabled (current state is hibernate mode) */
  if( (pwrModesReg & cPWR_MODES_XTALEN ) != cPWR_MODES_XTALEN )
  {
    /* enable 32 MHz crystal oscillator */
    pwrModesReg |= (uint8_t) cPWR_MODES_XTALEN;
    MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PWR_MODES, pwrModesReg);
    /* wait for crystal oscillator to complet its warmup */
    while( ( MC1324xDrv_DirectAccessSPIRead( (uint8_t) PWR_MODES) & cPWR_MODES_XTAL_READY ) != cPWR_MODES_XTAL_READY);
    /* wait for radio wakeup from hibernate interrupt */
    while( ( MC1324xDrv_DirectAccessSPIRead( (uint8_t) IRQSTS2) & (cIRQSTS2_WAKE_IRQ | cIRQSTS2_TMRSTATUS) ) != (cIRQSTS2_WAKE_IRQ | cIRQSTS2_TMRSTATUS) );
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) IRQSTS2, (uint8_t) (cIRQSTS2_WAKE_IRQ));
  }
  else
  {
    /* checks if packet processor is in idle state. otherwise abort any ongoing sequence */
    phyCtrl1Reg = MC1324xDrv_DirectAccessSPIRead( (uint8_t) PHY_CTRL1 );
    if( (phyCtrl1Reg & cPHY_CTRL1_XCVSEQ) != 0x00 )
    {
      /* abort any ongoing sequence */
      /* make sure that we abort in HW only if the sequence was actually started (tmr triggered) */
      if( ( 0 != ( MC1324xDrv_DirectAccessSPIRead( (uint8_t) PHY_CTRL1) & cPHY_CTRL1_XCVSEQ ) ) && ((MC1324xDrv_DirectAccessSPIRead(SEQ_STATE)&0x1F) != 0))
      {
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
        MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL1, phyCtrl1Reg);
        while ((MC1324xDrv_DirectAccessSPIRead(SEQ_STATE) & 0x1F) != 0);
      }
      /* clear sequence-end interrupt */ 
      irqSts1Reg = MC1324xDrv_DirectAccessSPIRead( (uint8_t) IRQSTS1);
      irqSts1Reg |= (uint8_t) cIRQSTS1_SEQIRQ;
      MC1324xDrv_DirectAccessSPIWrite( (uint8_t) IRQSTS1, irqSts1Reg);
    }
    MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PWR_MODES, pwrModesReg);
  }
  
  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_Radio_Enter_AutoDoze
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_Radio_Enter_AutoDoze
(
  void
)
{
  uint32_t irqMaskRegister;
  uint8_t pwrModesReg;
  irqMaskRegister = IntDisableAll();

  pwrModesReg = MC1324xDrv_DirectAccessSPIRead( (uint8_t) PWR_MODES);
  /* enable autodoze mode. */
  pwrModesReg |= (uint8_t) cPWR_MODES_AUTODOZE;
  /* check if 32 MHz crystal oscillator is enabled (current state is hibernate mode) */
  if( (pwrModesReg & cPWR_MODES_XTALEN ) != cPWR_MODES_XTALEN )
  {
    /* enable 32 MHz crystal oscillator */
    pwrModesReg |= (uint8_t) cPWR_MODES_XTALEN;
    MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PWR_MODES, pwrModesReg);
    /* wait for crystal oscillator to complet its warmup */
    while( ( MC1324xDrv_DirectAccessSPIRead( (uint8_t) PWR_MODES) & cPWR_MODES_XTAL_READY ) != cPWR_MODES_XTAL_READY);
    /* wait for radio wakeup from hibernate interrupt */
    while( ( MC1324xDrv_DirectAccessSPIRead( (uint8_t) IRQSTS2) & (cIRQSTS2_WAKE_IRQ | cIRQSTS2_TMRSTATUS) ) != (cIRQSTS2_WAKE_IRQ | cIRQSTS2_TMRSTATUS) );
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) IRQSTS2, (uint8_t) (cIRQSTS2_WAKE_IRQ));
  }
  else
  {
    MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PWR_MODES, pwrModesReg);
  }
  
  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_Radio_Enter_Idle
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_Radio_Enter_Idle
(
  void
)
{
  uint32_t irqMaskRegister;
  uint8_t phyCtrl1Reg, irqSts1Reg, pwrModesReg;
  irqMaskRegister = IntDisableAll();

  pwrModesReg = MC1324xDrv_DirectAccessSPIRead( (uint8_t) PWR_MODES);
  /* disable autodoze mode. sets PMC in high-power mode */
  pwrModesReg &= (uint8_t) ~( cPWR_MODES_AUTODOZE );
  pwrModesReg |= (uint8_t) cPWR_MODES_PMC_MODE;
  /* check if 32 MHz crystal oscillator is enabled (current state is hibernate mode) */
  if( (pwrModesReg & cPWR_MODES_XTALEN ) != cPWR_MODES_XTALEN )
  {
    /* enable 32 MHz crystal oscillator */
    pwrModesReg |= (uint8_t) cPWR_MODES_XTALEN;
    MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PWR_MODES, pwrModesReg);
    /* wait for crystal oscillator to complet its warmup */
    while( ( MC1324xDrv_DirectAccessSPIRead( (uint8_t) PWR_MODES) & cPWR_MODES_XTAL_READY ) != cPWR_MODES_XTAL_READY);
    /* wait for radio wakeup from hibernate interrupt */
    while( ( MC1324xDrv_DirectAccessSPIRead( (uint8_t) IRQSTS2) & (cIRQSTS2_WAKE_IRQ | cIRQSTS2_TMRSTATUS) ) != (cIRQSTS2_WAKE_IRQ | cIRQSTS2_TMRSTATUS) );
    MC1324xDrv_DirectAccessSPIWrite((uint8_t) IRQSTS2, (uint8_t) (cIRQSTS2_WAKE_IRQ));
  }
  else
  {
    /* checks if packet processor is in idle state. otherwise abort any ongoing sequence */
    phyCtrl1Reg = MC1324xDrv_DirectAccessSPIRead( (uint8_t) PHY_CTRL1 );
    if( (phyCtrl1Reg & cPHY_CTRL1_XCVSEQ) != 0x00 )
    {
      /* abort any ongoing sequence */
      /* make sure that we abort in HW only if the sequence was actually started (tmr triggered) */
      if( ( 0 != ( MC1324xDrv_DirectAccessSPIRead( (uint8_t) PHY_CTRL1) & cPHY_CTRL1_XCVSEQ ) ) && ((MC1324xDrv_DirectAccessSPIRead(SEQ_STATE)&0x1F) != 0))
      {
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
        MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL1, phyCtrl1Reg);
        while ((MC1324xDrv_DirectAccessSPIRead(SEQ_STATE) & 0x1F) != 0);
      }
      /* clear sequence-end interrupt */ 
      irqSts1Reg = MC1324xDrv_DirectAccessSPIRead( (uint8_t) IRQSTS1);
      irqSts1Reg |= (uint8_t) cIRQSTS1_SEQIRQ;
      MC1324xDrv_DirectAccessSPIWrite( (uint8_t) IRQSTS1, irqSts1Reg);
    }
    MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PWR_MODES, pwrModesReg);
  }
  
  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_Radio_Enter_Hibernate
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_Radio_Enter_Hibernate
(
  void
)
{
  uint32_t irqMaskRegister;
  uint8_t phyCtrl1Reg, irqSts1Reg, pwrModesReg;
  irqMaskRegister = IntDisableAll();
  
  /* checks if packet processor is in idle state. otherwise abort any ongoing sequence */
  phyCtrl1Reg = MC1324xDrv_DirectAccessSPIRead( (uint8_t) PHY_CTRL1 );
  if( (phyCtrl1Reg & cPHY_CTRL1_XCVSEQ) != 0x00 )
  {
      /* abort any ongoing sequence */
      /* make sure that we abort in HW only if the sequence was actually started (tmr triggered) */
      if( ( 0 != ( MC1324xDrv_DirectAccessSPIRead( (uint8_t) PHY_CTRL1) & cPHY_CTRL1_XCVSEQ ) ) && ((MC1324xDrv_DirectAccessSPIRead(SEQ_STATE)&0x1F) != 0))
      {
        phyCtrl1Reg &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
        MC1324xDrv_DirectAccessSPIWrite(PHY_CTRL1, phyCtrl1Reg);
        while ((MC1324xDrv_DirectAccessSPIRead(SEQ_STATE) & 0x1F) != 0);
      }
      /* clear sequence-end interrupt */ 
      irqSts1Reg = MC1324xDrv_DirectAccessSPIRead( (uint8_t) IRQSTS1);
      irqSts1Reg |= (uint8_t) cIRQSTS1_SEQIRQ;
      MC1324xDrv_DirectAccessSPIWrite( (uint8_t) IRQSTS1, irqSts1Reg);
  }
  
  pwrModesReg = MC1324xDrv_DirectAccessSPIRead( (uint8_t) PWR_MODES);
  /* disable autodoze mode. disable 32 MHz crystal oscillator. sets PMC in low-power mode */
  pwrModesReg &= (uint8_t) ~( cPWR_MODES_AUTODOZE | cPWR_MODES_XTALEN | cPWR_MODES_PMC_MODE );

  MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PWR_MODES, pwrModesReg);
  
//  {
//    uint8_t tmpReg;
//    tmpReg = MC1324xDrv_DirectAccessSPIRead( (uint8_t) PWR_MODES);
//    while( cPWR_MODES_XTAL_READY == ( tmpReg & cPWR_MODES_XTAL_READY ) )
//    {
//      MC1324xDrv_DirectAccessSPIWrite( (uint8_t) PWR_MODES, pwrModesReg);
//      tmpReg = MC1324xDrv_DirectAccessSPIRead( (uint8_t) PWR_MODES);
//    }
//  }
  
  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_WakeupPinEnable
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LLWU_WakeupPinEnable
(
  PWRLib_LLWU_WakeupPin_t       wakeupPin,
  PWRLib_LLWU_WakeupPinConfig_t edgeDetection
)
{
  volatile uint8_t * pinEnableRegBase = (volatile uint8_t *) &LLWU_PE1;
  uint8_t regIdx, pinIdx;
  
  regIdx = (uint8_t)((uint8_t) wakeupPin >> 2);
  pinIdx = (uint8_t)(((uint8_t) wakeupPin & 3) << 1);
  
  pinEnableRegBase[regIdx] &= (uint8_t) ~(0x03 << pinIdx);
  pinEnableRegBase[regIdx] |= (uint8_t) ((uint8_t) edgeDetection << pinIdx);
  
}
  
/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_WakeupModuleEnable
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LLWU_WakeupModuleEnable
(
  PWRLib_LLWU_WakeupModule_t wakeupModule
)
{
  LLWU_ME |= (uint8_t)(0x01 << (uint8_t) wakeupModule);
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_WakeupPinDisable
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LLWU_WakeupPinDisable
(
  PWRLib_LLWU_WakeupPin_t wakeupPin
)
{
  volatile uint8_t * pinEnableRegBase = (volatile uint8_t *) &LLWU_PE1;
  uint8_t regIdx, pinIdx;
  
  regIdx = (uint8_t)((uint8_t) wakeupPin >> 2);
  pinIdx = (uint8_t)(((uint8_t) wakeupPin & 3) << 1);
  
  pinEnableRegBase[regIdx] &= (uint8_t) ~(0x03 << pinIdx); 
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_WakeupModuleDisable
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LLWU_WakeupModuleDisable
(
  PWRLib_LLWU_WakeupModule_t wakeupModule
)
{
  LLWU_ME &= (uint8_t) ~(0x01 << (uint8_t) wakeupModule);
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_IsLPTMRWakeUpSource
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PWRLib_LLWU_IsLPTMRWakeUpSource
(
  void
)
{
  /* low power timer wakeup */
  if( (LLWU_ME & ( (uint8_t) (1 << gPWRLib_LLWU_WakeupModule_LPTMR_c ) )) == ( (uint8_t) (1 << gPWRLib_LLWU_WakeupModule_LPTMR_c ) ) )
  {
    if( (LLWU_F3 & ( (uint8_t) (1 << gPWRLib_LLWU_WakeupModule_LPTMR_c ) )) == ( (uint8_t) (1 << gPWRLib_LLWU_WakeupModule_LPTMR_c ) ) )
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_IsRTCWakeUpSource
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PWRLib_LLWU_IsRTCWakeUpSource
(
  void
)
{
  /* real time counter wakeup */
  if( (LLWU_ME & ( (uint8_t) (1 << gPWRLib_LLWU_WakeupModule_RTC_Alarm_c ) )) == ( (uint8_t) (1 << gPWRLib_LLWU_WakeupModule_RTC_Alarm_c ) ) )
  {
    if( (LLWU_F3 & ( (uint8_t) (1 << gPWRLib_LLWU_WakeupModule_RTC_Alarm_c ) )) == ( (uint8_t) (1 << gPWRLib_LLWU_WakeupModule_RTC_Alarm_c ) ) )
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_IsTSIWakeUpSource
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PWRLib_LLWU_IsTSIWakeUpSource
(
  void
)
{
#if (MCU_MK60D10 == 1) || (MCU_MK20D5 == 1) || (MCU_MK60N512VMD100 == 1)
  /* TSI wakeup */
  if( (LLWU_ME & ( (uint8_t) (1 << gPWRLib_LLWU_WakeupModule_TSI_c ) )) == ( (uint8_t) (1 << gPWRLib_LLWU_WakeupModule_TSI_c ) ) )
  {
    if( (LLWU_F3 & ( (uint8_t) (1 << gPWRLib_LLWU_WakeupModule_TSI_c ) )) == ( (uint8_t) (1 << gPWRLib_LLWU_WakeupModule_TSI_c ) ) )
    {
      return TRUE;
    }
  }
#endif
  return FALSE;
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_IsGPIOWakeUpSource
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PWRLib_LLWU_IsGPIOWakeUpSource
(
  uint8_t pinNumber
)
{
  uint32_t llwuPinEnableReg;
  uint16_t llwuFlagReg;

  llwuPinEnableReg = (uint32_t) (LLWU_PE1 << 0);
  llwuPinEnableReg |= (uint32_t) (LLWU_PE2 << 8);
  llwuPinEnableReg |= (uint32_t) (LLWU_PE3 << 16);
  llwuPinEnableReg |= (uint32_t) (LLWU_PE4 << 24);

  llwuFlagReg = (uint32_t) (LLWU_F1 << 0);
  llwuFlagReg |= (uint32_t) (LLWU_F2 << 8);
  if( ( llwuPinEnableReg & (uint32_t) (0x3 << ( pinNumber << 1 ) ) ) != 0)
  {
    if( (llwuFlagReg & (uint16_t) (1 << pinNumber) ) != 0)
    {
       return TRUE;
    }
  }
  return FALSE;
}


/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_ClockStart
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LPTMR_ClockStart
(
  uint8_t  ClkMode,
  uint32_t Ticks
)
{
  uint32_t irqMaskRegister;
  
  irqMaskRegister = IntDisableAll();
  /* Turn on clock to LPTMR module */
  SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;
  
  /* Disable LPTMR */
  LPTMR0_CSR &= ~(LPTMR_CSR_TEN_MASK);
  
  /* Set compare value */
  if(Ticks != 0)
  {
    LPTMR0_CMR = (uint16_t) (Ticks - 1);
  }
  else
  {
    LPTMR0_CMR = (uint16_t) Ticks;
  }
  
  /* Use specified tick count */
  mPWRLib_RTIRemainingTicks = Ticks;
  
  /* Configure prescaler, bypass prescaler and clck source */
  LPTMR0_PSR = (ClkMode | cPWR_LPTMRClockSource);
  
  /* Start counting */
  LPTMR0_CSR = ( LPTMR_CSR_TCF_MASK |
                 LPTMR_CSR_TIE_MASK | 
                 LPTMR_CSR_TFC_MASK);
  LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;
  
  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_ClockCheck
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint32_t PWRLib_LPTMR_ClockCheck
(
  void
)
{
  uint32_t irqMaskRegister;
  uint32_t remTicks;
  
  irqMaskRegister = IntDisableAll();
  
  remTicks = mPWRLib_RTIRemainingTicks;
  
  /* LPTMR is still running */
  if(LPTMR0_CSR & LPTMR_CSR_TEN_MASK)
  {
    /* timer compare flag is set */
    if(LPTMR0_CSR & LPTMR_CSR_TCF_MASK)
    {
      remTicks = 0;
    }
    else
    {
      LPTMR0_CNR = LPTMR0_CNR;
      remTicks = mPWRLib_RTIRemainingTicks - LPTMR0_CNR;
    }
  }
 
  IntRestoreAll(irqMaskRegister);
  return remTicks;
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_ResetTicks
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LPTMR_ResetTicks
(
  void
)
{
  uint32_t irqMaskRegister;
  irqMaskRegister = IntDisableAll();
  mPWRLib_RTIRemainingTicks = 0;
  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_ClockStop
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LPTMR_ClockStop
(
  void
)
{
  uint32_t irqMaskRegister;
  irqMaskRegister = IntDisableAll();
  
  /* LPTMR is still running */
  if(LPTMR0_CSR & LPTMR_CSR_TEN_MASK)
  {
    /* timer compare flag is set */
    if(LPTMR0_CSR & LPTMR_CSR_TCF_MASK)
    {
      mPWRLib_RTIRemainingTicks = 0;
    }
    else
    {
      LPTMR0_CNR = LPTMR0_CNR;
      mPWRLib_RTIRemainingTicks = mPWRLib_RTIRemainingTicks - LPTMR0_CNR;
    }
  }
  /* Stop LPTMR */
  LPTMR0_CSR &= ~(LPTMR_CSR_TEN_MASK);
  
  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_RTC_Init
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_RTC_Init
(
 void
)
{
  uint32_t irqMaskRegister;
  
  irqMaskRegister = IntDisableAll();
  
  /* Turn on clock to RTC module */
  SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;
  /* Reset RTC register */
  RTC_CR |= RTC_CR_SWR_MASK;
  RTC_CR &= ~(RTC_CR_SWR_MASK);
  /* Disable RTC */
  RTC_SR &=  ~(RTC_SR_TCE_MASK);
  /* Allow non-supervisor mode access*/
  RTC_CR |= RTC_CR_SUP_MASK;
  /* Clear all pending interrupts */
  RTC_TAR = 0;
  RTC_TSR = 0;
  /* Disable all RTC interrupts */
  RTC_IER &= ~(RTC_IER_TAIE_MASK |
               RTC_IER_TSIE_MASK |
               RTC_IER_TOIE_MASK |
               RTC_IER_TIIE_SHIFT);
  /* Enable 32 KHz oscillator */
  RTC_CR |= RTC_CR_OSCE_MASK;
  
  mPWRLib_RTC_IsOscStarted = FALSE;
  
#if gPWR_EnsureOscStabilized_d
#if gTMR_Enabled_d
  /* Allocate a platform timer */
  mPWRLib_RTC_OscInitTmrID = TMR_AllocateTimer();	
  
  if(gTmrInvalidTimerID_c != mPWRLib_RTC_OscInitTmrID)
  {	
    TMR_StartTimer(mPWRLib_RTC_OscInitTmrID, gTmrSingleShotTimer_c, 1000, PWRLib_RTC_OscInitCallback);
  }	
#else
  {
    /* Wait the oscilator startup time */
    PWRLib_DelayMs(cRTC_OSC_STARTUP_TIME);
    mPWRLib_RTC_IsOscStarted = TRUE;
  }
#endif  
#endif
    
  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_RTC_IsOscStarted
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PWRLib_RTC_IsOscStarted
(
 void
)
{
  uint32_t irqMaskRegister;
  bool_t isOscStarted = FALSE;
  
  irqMaskRegister = IntDisableAll();
  
  isOscStarted = mPWRLib_RTC_IsOscStarted;
  
  IntRestoreAll(irqMaskRegister);
  
  return isOscStarted;
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_RTC_ClockStart
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_RTC_ClockStart
(
  uint32_t Ticks
)
{
  uint32_t irqMaskRegister;
  
  irqMaskRegister = IntDisableAll();

  /* Disable RTC */
  RTC_SR &=  ~(RTC_SR_TCE_MASK);
  
  /* Set compare value and clear alaram interrupt*/
  if(Ticks != 0)
  {
    RTC_TAR = Ticks - 1;
  }
  else
  {
    RTC_TAR = Ticks;
  }
  
  RTC_TSR = 0;
  
  /* Enable alarm interrupt*/
  RTC_IER |= RTC_IER_TAIE_MASK;
  /* Enable RTC */
  RTC_SR |=  RTC_SR_TCE_MASK;
  
  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_RTC_ClockCheck
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint32_t PWRLib_RTC_ClockCheck
(
  void
)
{
  uint32_t irqMaskRegister;
  uint32_t remTicks;
  
  irqMaskRegister = IntDisableAll();
  
  remTicks = mPWRLib_RTIRemainingTicks;
  
  /* RTC is still running */
  if(RTC_SR & RTC_SR_TCE_MASK)
  {
    /* Time Alarm Flag */
    if(RTC_SR & RTC_SR_TAF_MASK)
    {
      remTicks = 0;
    }
    else
    {
      remTicks = mPWRLib_RTIRemainingTicks - RTC_TSR;
    }
  }
  
  IntRestoreAll(irqMaskRegister);
  return remTicks;
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_RTC_ResetTicks
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_RTC_ResetTicks
(
  void
)
{
  uint32_t irqMaskRegister;
  irqMaskRegister = IntDisableAll();
  mPWRLib_RTIRemainingTicks = 0;
  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_RTC_ClockStop
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_RTC_ClockStop
(
  void
)
{
  uint32_t irqMaskRegister;
  irqMaskRegister = IntDisableAll();
  
  /* RTC is still running */
  if(RTC_SR & RTC_SR_TCE_MASK)
  {
    /* Time Alarm Flag */
    if(RTC_SR & RTC_SR_TAF_MASK)
    {
      mPWRLib_RTIRemainingTicks = 0;
    }
    else
    {
      mPWRLib_RTIRemainingTicks = mPWRLib_RTIRemainingTicks - RTC_TSR;
    }
  }
  /* Stop RTC */
  RTC_SR &=  ~(RTC_SR_TCE_MASK);
  /* Disable alarm interrupt*/
  RTC_IER &= ~(RTC_IER_TAIE_MASK);
  /* Clear all pending interrupts */
  RTC_TAR = 0;
  RTC_TSR = 0;
  
  IntRestoreAll(irqMaskRegister);
}

/******************************************************************************
 * Name: PWRLib_RTC_OscInitCallback
 * Description:
 *
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
#if gPWR_EnsureOscStabilized_d
#if gTMR_Enabled_d
static void PWRLib_RTC_OscInitCallback
(
 tmrTimerID_t tmrID
)
{
  if(tmrID == mPWRLib_RTC_OscInitTmrID)
  {
    /* RTC OSC is started */
    mPWRLib_RTC_IsOscStarted = TRUE;
    
    /* free timer */
    TMR_FreeTimer(tmrID);			
  }		
}
#endif
#endif

void PWRLib_DelayMs
(
  uint16_t val
)
{
    /* Turn on clock to LPTMR module */
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;
    
      /* Disable LPTMR */
    LPTMR0_CSR &= ~(LPTMR_CSR_TEN_MASK);
    
    /* Set compare value */
    LPTMR0_CMR = val;
    
    /* Use 1Khz LPO clock and bypass prescaler */
    LPTMR0_PSR = LPTMR_PSR_PCS(1)|LPTMR_PSR_PBYP_MASK;
    
    /* Start counting */
    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;
    
    /* Wait for counter to reach compare value */
    while (!(LPTMR0_CSR & LPTMR_CSR_TCF_MASK)) {}
    
    /* Clear Timer Compare Flag */
    LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;
    
    /* Turn off clock to LPTMR module */
    SIM_SCGC5 &= ~SIM_SCGC5_LPTIMER_MASK;	
}

#if (cPWR_UsePowerModuleStandAlone == 0)

/******************************************************************************
 * Name: PWRLib_GetMacStateReq
 * Description: Get status from MAC. Functions just as Asp_GetMacStateReq().
 *
 * Parameter(s): - none
 * Return: - gAspMacStateIdle_c     : MAC ready for Sleep or DeepSleep
 *           gAspMacStateBusy_c     : Don't sleep
 *           gAspMacStateNotEmpty_c : MAC allows Wait
 ******************************************************************************/
uint8_t PWRLib_GetMacStateReq
(
  void
)
{
  return Asp_GetMacStateReq();
}

#endif /* (cPWR_UsePowerModuleStandAlone == 0) */

#endif /* #if (cPWR_UsePowerDownMode==1) */

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_Isr
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif 
void PWRLib_LLWU_Isr
(
  void
)
{

#if (MCU_MK60N512VMD100 == 1)
  SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
#elif (MCU_MK60D10 == 1)
  SIM_SCGC4 |= (uint32_t) (1 << 28);
#endif

#if  (MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1)
  if (LLWU_F2 & LLWU_F2_WUF8_MASK)
  {
    PORTC_PCR4 |= PORT_PCR_ISF_MASK;
    LLWU_F2 |= LLWU_F2_WUF8_MASK;   // write one to clear the flag
	PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1;
  }
  if (LLWU_F2 & LLWU_F2_WUF9_MASK)
  {
    PORTC_PCR5 |= PORT_PCR_ISF_MASK;
    LLWU_F2 |= LLWU_F2_WUF9_MASK;   // write one to clear the flag
	PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1;
  }
  if (LLWU_F2 & LLWU_F2_WUF10_MASK)
  {
    PORTC_PCR6 |= PORT_PCR_ISF_MASK;
    LLWU_F2 |= LLWU_F2_WUF10_MASK;   // write one to clear the flag
	PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1;
  }
#endif
  
  /* Clear external pins wakeup interrupts */
  LLWU_F1 = LLWU_F1;
  LLWU_F2 = LLWU_F2;
  
  /* Clear wakeup from error interrupt */
  LLWU_F3 |= LLWU_F3_MWUF7_MASK;

#if (MCU_MK60D10 == 1) || (MCU_MK20D5 == 1) || (MCU_MK60N512VMD100 == 1)
  /* TSI is wakeup source */
  if(LLWU_F3 & LLWU_F3_MWUF4_MASK)
  {
    /* Clear OUT OF RANGE, END OF SCAN flags */
    //gTSI_GENCS_REG |= ( gTSI_OUT_OF_RANGE_MASK_c | gTSI_EOS_FLAG_MASK_c );
    TSI0_GENCS |= (TSI_GENCS_OUTRGF_MASK | TSI_GENCS_EOSF_MASK); 
    
    /* Clear touch sensing error flags */
#if (MCU_MK60N512VMD100 == 1)
    TSI0_STATUS = 0xFFFFFFFF;
#elif (MCU_MK60D10 == 1)
           
#endif
    PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1;
  }
#endif
  
  /* LPTMR is wakeup source */
  if(LLWU_F3 & LLWU_F3_MWUF0_MASK)
  {
    /* Clear LPTMR interrupt */
    LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;
    PWRLib_MCU_WakeupReason.Bits.FromLPTMR = 1;
  }
  
  /* RTC alarm is wakeup source */
  if(LLWU_F3 & LLWU_F3_MWUF5_MASK)
  {
    /* Stop RTC */
    RTC_SR &=  ~(RTC_SR_TCE_MASK);
    /* Disable alarm interrupt*/
    RTC_IER &= ~(RTC_IER_TAIE_MASK);
    /* Clear all pending interrupts */
    RTC_TAR = 0;
    RTC_TSR = 0;
    PWRLib_MCU_WakeupReason.Bits.FromRTC = 1;
  }
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_Isr
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#if (gTMR_EnableHWLowPowerTimers_d == FALSE)
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
 void PWRLib_LPTMR_Isr
(
  void
)
{
  /* Stop LPTMR */
  LPTMR0_CSR &= ~(LPTMR_CSR_TEN_MASK);
  /* Clear LPTMR interrupt */
  LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;
#if (cPWR_UsePowerDownMode==1)  
  mPWRLib_RTIRemainingTicks = 0;
#endif /* #if (gTMR_EnableHWLowPowerTimers_d == FALSE) */
}
#endif

/*---------------------------------------------------------------------------
 * Name: PWRLib_LVD_CollectLevel
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
PWRLib_LVD_VoltageLevel_t PWRLib_LVD_CollectLevel
(
  void
)
{
#if ((cPWR_LVD_Enable == 1) || (cPWR_LVD_Enable == 2))
  
  /* Check low detect voltage 1.6V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(0);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(0);
  PMC_LVDSC1 = PMC_LVDSC1_LVDACK_MASK;
  if(PMC_LVDSC1 & PMC_LVDSC1_LVDF_MASK)
  {
    /* Low detect voltage reached */
    PMC_LVDSC1 = PMC_LVDSC1_LVDACK_MASK;
    return(PWR_LEVEL_CRITICAL);
  }
  
  /* Check low trip voltage 1.8V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(0);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(0);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_1_8V);
  }
  
  /* Check low trip voltage 1.9V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(0);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(1);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_1_9V);
  }
  /* Check low trip voltage 2.0V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(0);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(2);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_2_0V);
  }
  
  /* Check low trip voltage 2.1V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(0);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(3);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_2_1V);
  }
  
  /* Check low detect voltage (high range) 2.56V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(1); /* Set high trip voltage and clear warning flag */
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(0);
  PMC_LVDSC1 |= PMC_LVDSC1_LVDACK_MASK;
  if(PMC_LVDSC1 & PMC_LVDSC1_LVDF_MASK)
  {
    /* Low detect voltage reached */
    PMC_LVDSC1 = PMC_LVDSC1_LVDACK_MASK; /* Set low trip voltage and clear warning flag */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_2_56V);
  }
  
  /* Check high trip voltage 2.7V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(1);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(0);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_2_7V);
  }
  
  /* Check high trip voltage 2.8V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(1);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(1);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_2_8V);
  }
  
  /* Check high trip voltage 2.9V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(1);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(2);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_2_9V);
  }
  
  /* Check high trip voltage 3.0V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(1);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(3);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_3_0V);
  }
  
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(0);
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
#endif  /* #if ((cPWR_LVD_Enable == 1) || (cPWR_LVD_Enable == 2)) */

  /*--- Voltage level is okay > 3.0V */
  return(PWR_ABOVE_LEVEL_3_0V);
}

/******************************************************************************
 * Name: PWRLib_LVD_PollIntervalCallback
 * Description:
 *
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
#if (cPWR_LVD_Enable == 2)
static void PWRLib_LVD_PollIntervalCallback
(
  tmrTimerID_t tmrID
)
{
  if(tmrID == PWRLib_LVD_PollIntervalTmrID)
  {
    PWRLib_LVD_SavedLevel = PWRLib_LVD_CollectLevel();
    /* re-start the timer */
    //TMR_StartMinuteTimer(PWRLib_LVD_PollIntervalTmrID, PWRLib_LVD_CollectCounter, PWRLib_LVD_PollIntervalCallback);
    TMR_StartLowPowerTimer(PWRLib_LVD_PollIntervalTmrID, gTmrLowPowerMinuteTimer_c, PWRLib_LVD_CollectCounter, PWRLib_LVD_PollIntervalCallback);
  }
}
#endif

/*---------------------------------------------------------------------------
 * Name: PWRLib_RTC_Isr
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif 
void PWRLib_RTC_Isr
(
  void
)
{
  /* Stop RTC */
  RTC_SR &=  ~(RTC_SR_TCE_MASK);
  /* Disable alarm interrupt*/
  RTC_IER &= ~(RTC_IER_TAIE_MASK);
  /* Clear all pending interrupts */
  RTC_TAR = 0;
  RTC_TSR = 0;
#if (cPWR_UsePowerDownMode==1)
  mPWRLib_RTIRemainingTicks = 0;
#endif /* #if (cPWR_UsePowerDownMode==1) */
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_GetSystemResetStatus
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint16_t PWRLib_GetSystemResetStatus
(
  void
)
{
  uint16_t resetStatus = 0;
#if (MCU_MK60N512VMD100 == 1)
  resetStatus = (uint16_t) (MC_SRSL);
  resetStatus |= (uint16_t)(MC_SRSH << 8);
#elif ((MCU_MK60D10 == 1) || (MCU_MK20D5 == 1) || (MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
  resetStatus = (uint16_t) (RCM_SRS0);
  resetStatus |= (uint16_t)(RCM_SRS1 << 8);
#endif
  return resetStatus;
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_Init
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_Init
(
  void
)
{

#if (cPWR_UsePowerDownMode == 1)
  /* enable clock to LLWU module */
#if (MCU_MK60N512VMD100 == 1)	
  SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
#elif (MCU_MK60D10 == 1)
  SIM_SCGC4 |= (uint32_t) (1 << 28);
#endif

#if ( (cPWR_DeepSleepMode == 4) || (cPWR_DeepSleepMode == 7) || (cPWR_DeepSleepMode == 11) )
  PWRLib_RTC_Init();
  /* configure NVIC for RTC alarm Isr */
  NVIC_EnableIRQ(gRTC_IRQ_Number_c);
  /* enable RTC as wakeup source for LLWU module */
  PWRLib_LLWU_WakeupModuleEnable(gPWRLib_LLWU_WakeupModule_RTC_Alarm_c);
#endif

#if ( (cPWR_DeepSleepMode == 2) || (cPWR_DeepSleepMode == 3) || (cPWR_DeepSleepMode == 5) || (cPWR_DeepSleepMode == 6) || (cPWR_DeepSleepMode == 8) || (cPWR_DeepSleepMode == 9) || (cPWR_DeepSleepMode == 10) || (cPWR_DeepSleepMode == 12))  
  /* configure NVIC for LPTMR Isr */
  NVIC_EnableIRQ(gLPTMR_IRQ_Number_c);
  /* enable LPTMR as wakeup source for LLWU module */
  PWRLib_LLWU_WakeupModuleEnable(gPWRLib_LLWU_WakeupModule_LPTMR_c);
#endif
  
#if (cPWR_DeepSleepMode == 14)
  /* enable LPTMR as wakeup source for LLWU module */
  PWRLib_LLWU_WakeupModuleEnable(gPWRLib_LLWU_WakeupModule_LPTMR_c);
#endif

#if ( (cPWR_DeepSleepMode != 0) && (cPWR_DeepSleepMode != 2) && (cPWR_DeepSleepMode != 3) && (cPWR_DeepSleepMode != 4) )

#if ((MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
  /* enable GPIO as wakeup source for LLWU module */
  PWRLib_LLWU_WakeupPinEnable( (PWRLib_LLWU_WakeupPin_t) gLLWU_WakeUp_PIN_Number_c, gPWRLib_LLWU_WakeupPin_AnyEdge_c);
#elif (MCU_MK60D10 == 1)
  /* enable TSI as wakeup source for LLWU module */
  PWRLib_LLWU_WakeupModuleEnable(gPWRLib_LLWU_WakeupModule_TSI_c);
#endif

#endif
  
#if ( (cPWR_DeepSleepMode != 0) && (cPWR_DeepSleepMode != 13) )
  /* configure NVIC for LLWU Isr */
  NVIC_EnableIRQ(gLLWU_IRQ_Number_c);
#endif
  
#if ( (cPWR_DeepSleepMode == 3) || (cPWR_DeepSleepMode == 6) || (cPWR_DeepSleepMode == 10) )
  
#if (MCU_MK60N512VMD100 == 1)	
  SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL_MASK;
#elif ((MCU_MK60D10 == 1)  || (MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
  SIM_SOPT1 &= ~SIM_SOPT1_OSC32KSEL_MASK;
  SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL(2);
#endif // (MCU_MK60N512VMD100 == 1)	
 PWRLib_RTC_Init(); 

#endif

#endif /* #if (cPWR_UsePowerDownMode==1) */
    
  /* LVD_Init TODO */
#if (cPWR_LVD_Enable == 0)
//  PMC_LVDSC1 &= (uint32_t) ~( PMC_LVDSC1_LVDRE_MASK | PMC_LVDSC1_LVDIE_MASK );
  PMC_LVDSC1 &= (uint32_t) ~( PMC_LVDSC1_LVDIE_MASK );
  PMC_LVDSC2 &= (uint32_t) ~( PMC_LVDSC2_LVWIE_MASK );
#elif ((cPWR_LVD_Enable == 1) || (cPWR_LVD_Enable == 2))
//  PMC_LVDSC1 &= (uint32_t) ~( PMC_LVDSC1_LVDRE_MASK | PMC_LVDSC1_LVDIE_MASK );
  PMC_LVDSC1 &= (uint32_t) ~( PMC_LVDSC1_LVDIE_MASK );
  PMC_LVDSC2 &= (uint32_t) ~( PMC_LVDSC2_LVWIE_MASK );
#elif (cPWR_LVD_Enable==3)
  PMC_LVDSC1 &= (uint32_t) ~( PMC_LVDSC1_LVDIE_MASK );
  PMC_LVDSC2 &= (uint32_t) ~( PMC_LVDSC2_LVWIE_MASK );
  PMC_LVDSC1 |= (uint32_t) ( PMC_LVDSC1_LVDRE_MASK );
#endif /* #if (cPWR_LVD_Enable) */
  
#if ((cPWR_LVD_Enable == 1) || (cPWR_LVD_Enable == 2))
  PWRLib_LVD_SavedLevel = PWR_ABOVE_LEVEL_3_0V;
#endif  /* #if ((cPWR_LVD_Enable == 1) || (cPWR_LVD_Enable == 2)) */
#if (cPWR_LVD_Enable == 2)
  PWRLib_LVD_CollectCounter = cPWR_LVD_Ticks;
#endif  /* #if (cPWR_LVD_Enable==2) */
  
#if (cPWR_LVD_Enable == 2)
  
  /* Allocate a platform timer */
  PWRLib_LVD_PollIntervalTmrID = TMR_AllocateTimer();	
  
  if(gTmrInvalidTimerID_c != PWRLib_LVD_PollIntervalTmrID)
  {	
    /* start the timer */
    TMR_StartLowPowerTimer(PWRLib_LVD_PollIntervalTmrID, gTmrLowPowerMinuteTimer_c, PWRLib_LVD_CollectCounter, PWRLib_LVD_PollIntervalCallback);
  }
#endif  /* #if (cPWR_LVD_Enable==2) */
  
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_Reset
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_Reset
(
  void
)
{
  SCB_AIRCR = (uint32_t) (SCB_AIRCR_VECTKEY(0x5FA) | SCB_AIRCR_SYSRESETREQ_MASK);
  while(1);
}
