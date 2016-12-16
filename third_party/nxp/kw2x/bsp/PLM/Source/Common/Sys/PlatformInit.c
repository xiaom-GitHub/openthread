/**************************************************************************
* Filename: PlatformInit.c
*
* Description: Platform specific implementation file
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

/***************************************************************************
* Includes
****************************************************************************/
#include "EmbeddedTypes.h"
#include "PortConfig.h"
#include "PlatformInit.h"
#include "MC1324xDrv.h"
#include "MC1324xReg.h"
#include "NV_Data.h"
#include "AppToPlatformConfig.h"

#if gMAC_PHY_INCLUDED_c
#include "AppAspInterface.h"
#include "MacPhyInit.h"
#include "Phy.h"
#endif

#include "NVIC.h"
#include "mcg.h"


/***************************************************************************
 ****************************************************************************
 * External platform references
 ****************************************************************************
 ****************************************************************************/

/***************************************************************************
 ****************************************************************************
 * Private macros
 ****************************************************************************
 ****************************************************************************/

#if( (gMAC_PHY_INCLUDED_c == 0) && defined(gMCG_FEE_Mode_d) )
  #error "*** ERROR: Invalid clocking scheme selected. "
#endif //#if( (gMAC_PHY_INCLUDED_c == 0) && defined(gMCG_FEE_Mode_d) )

#if ( !defined(gTargetTWR_KW22N512_d) && !defined(gTargetTWR_KW21D512_d) && !defined(gTargetTWR_KW21D256_d) && !defined(gTargetTWR_KW22D512_d) && !defined(gTargetTWR_KW24D512_d) && !defined(gTargetKW24D512_USB_d))
  #if( defined(gMCG_FEE_Mode_d) )
    #error "*** ERROR: Invalid clocking scheme selected. "
  #endif  //#if( defined(gMCG_FEE_Mode_d) )
#endif //( !defined(gTargetTWR_KW22D512_d) && !defined(gTargetTWR_K21WX256_d) )

/*
 * Name: mClkSwitchDelayTime_c
 * Description: Time in milliseconds required by the transceiver to switch
 *              the CLK_OUT clock frequency (in our case from 32KHz to 4 MHz)
 */
#define mClkSwitchDelayTime_c           50
#define mRst_B_AssertTime_c             50

/*
 * Name: mCLK_OUT_DIV_4MHz_c
 * Description: CLK_OUT_DIV field value for 4 MHz clock out frequency
 */
#define mCLK_OUT_DIV_4MHz_c             3

/***************************************************************************
 ****************************************************************************
 * Private functions
 ****************************************************************************
 ****************************************************************************/

#if gMAC_PHY_INCLUDED_c

static void delayMs(uint16_t val)
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

#endif //if gMAC_PHY_INCLUDED_c

/***************************************************************************
 ****************************************************************************
 * Public function(s)
 ****************************************************************************
 ****************************************************************************/

/****************************************************************************
 * Name: PlatformPortInit
 *
 * Description: Perform the complete GPIO ports initialization
 *
 * Interface assumptions:
 *   NONE
 *
 * Parameter(s):
 *   NONE
 *
 * Return value:
 *   NONE
 *
 *****************************************************************************/
void PlatformPortInit
(
		void
)
{
  // Platform ports default configuration/initialization
  
  // Setup port A
  mSETUP_PORT_A

  // Setup Port B
  mSETUP_PORT_B

  // Setup port C
  mSETUP_PORT_C

  // Setup port D
  mSETUP_PORT_D

  // Setup port E
  mSETUP_PORT_E
  
#if (gTarget_UserDefined_d == 1)   
  mSETUP_SWICH_PORT
  mSETUP_LED_PORT
  mSETUP_UART
#endif 
}

/************************************************************************************
 * Name: Platform_Init
 * Description: Initialize platform specific stuff
 *
 * Interface assumptions:
 *   NONE
 *
 * Parameter(s):
 *   NONE
 *
 * Return value:
 *   NONE
 *
 ************************************************************************************/
void Platform_Init
(
		void
)
{

/* Write to PMPROT to allow low-power run or stop mode */
#if (MCU_MK60N512VMD100 == 1)
  MC_PMPROT |= (  MC_PMPROT_AVLP_MASK
                | MC_PMPROT_ALLS_MASK
                | MC_PMPROT_AVLLS3_MASK
                | MC_PMPROT_AVLLS2_MASK
                | MC_PMPROT_AVLLS1_MASK );
#elif ((MCU_MK60D10 == 1) || (MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
  SMC_PMPROT |= (  SMC_PMPROT_AVLP_MASK
                 | SMC_PMPROT_ALLS_MASK
                 | SMC_PMPROT_AVLLS_MASK );
#endif

  /* Initialize MCU specifics: GPIO ports, SPI, etc */
  PlatformPortInit();

#if gMAC_PHY_INCLUDED_c

  /* Initialize the transceiver SPI driver */
  MC1324xDrv_SPIInit();
  /* Configure the transceiver IRQ_B port */  
  MC1324xDrv_IRQ_PortConfig();
  
  /* Configure the transceiver RST_B port */ 
  MC1324xDrv_RST_B_PortConfig();
  
  /* Transceiver Hard/RST_B RESET */
  MC1324xDrv_RST_B_Assert();
  delayMs(mRst_B_AssertTime_c);
  MC1324xDrv_RST_B_Deassert();
  
  /* Wait for transceiver to deassert IRQ pin */
  while(MC1324xDrv_IsIrqPending()) ;
  /* Wait for transceiver wakeup from POR iterrupt */
  while(!MC1324xDrv_IsIrqPending()) ;
  
  /* Enable transceiver SPI interrupt request */
  NVIC_EnableIRQ(MC1324x_Irq_Number);
  
  NVIC_SetPriority(MC1324x_Irq_Number, MC1324x_Irq_Priority);
  
  /* Enable the transceiver IRQ_B interrupt request */
  MC1324xDrv_IRQ_Enable();
  
#if defined(gMCG_FEE_Mode_d)
  MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_32_78_KHz);
#else
  MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_4_MHz);
#endif //#if defined(gMCG_FEE_Mode_d)
  
  /* wait until the external reference clock is stable */    
  delayMs(mClkSwitchDelayTime_c);

#if ( (gTargetTWR_K60D100M_d == 1) || \
      (gTargetTWR_K60N512_d == 1)  || \
      (gTargetTWR_K21D50M_d == 1)  || \
      ( (gTarget_UserDefined_d == 1) && (MCU_MK60D10 == 1) ) || \
      ( (gTarget_UserDefined_d == 1) && (MCU_MK60N512VMD100 == 1) ) )  
  /* disable transceiver CLK_OUT for selected targets */
  MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE); //TODO
#endif

#endif // gMAC_PHY_INCLUDED_c
  
  /* Ramp-up the system clock */
#if defined(gMCG_FEE_Mode_d)
  
  /* FEE @ 48MHz */
  gMCG_coreClkMHz = MCG_Fei2Fee();
  
#else //#if defined(gMCG_FEE_Mode_d)
  
  /* PEE @ 48MHz */
  gMCG_coreClkMHz = MCG_PLLInit();
  
#endif //#if defined(gMCG_FEE_Mode_d)
  
  /*
  * Use the value obtained from the pll_init function to define variables
  * for the core clock in kHz and also the peripheral clock. These
  * variables can be used by other functions that need awareness of the
  * system frequency.
  */
  
  gMCG_coreClkKHz = gMCG_coreClkMHz * 1000;
  gMCG_periphClkKHz = gMCG_coreClkKHz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> 24)+ 1);    
}

/************************************************************************************
 * Name: Init_802_15_4
 *
 * Description: Initialize platform and stack. This function is the main initialization
 * procedure, which will be called from the startup code of the project.
 * If the code is build as a stand-alone application, this function is
 * called from main() below.
 *
 * Interface assumptions:
 *   NONE
 *
 * Parameter(s):
 *   NONE
 *
 * Return value:
 *   NONE
 *
 ************************************************************************************/
void Init_802_15_4
(
		void
)
{  
#if gMAC_PHY_INCLUDED_c
  
  bool_t relaxedTiming;

  relaxedTiming = gHardwareParameters.Bus_Frequency_In_MHz<12;   

  MacPhyInit_Initialize(relaxedTiming, 0, gHardwareParameters.defaultPowerLevel, 
                        (uint8_t*)gHardwareParameters.paPowerLevelLimits, FALSE);

  PhyPpSetCcaThreshold(gHardwareParameters.ccaThreshold);
  PhyPlmeSetPwrLevelRequest(gHardwareParameters.defaultPowerLevel);

  MacPhyInit_WriteExtAddress((uint8_t*)&gHardwareParameters.MAC_Address[0]);
#if gDualPanEnabled_d
  MacPhyInit_WriteExtAddress_PAN1((uint8_t*)&gHardwareParameters.MAC_Address_PAN1[0]);
#endif

#endif //gMAC_PHY_INCLUDED_c
}

/************************************************************************************
 * Name: ResetMCU
 *
 * Description: Software Resets the CPU.
 *
 * Interface assumptions:
 *   NONE
 *
 * Parameter(s):
 *   NONE
 *
 * Return value:
 *   NONE
 *
 ************************************************************************************/
void ResetMCU(void)
{
  SCB_AIRCR = SCB_AIRCR_VECTKEY(0x5FA) | SCB_AIRCR_SYSRESETREQ_MASK;
  while(1);   
}
