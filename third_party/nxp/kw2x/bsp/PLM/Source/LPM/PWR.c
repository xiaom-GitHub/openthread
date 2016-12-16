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
#include "PWR_Interface.h"
#include "TMR_Interface.h"
#include "mcg.h"
#include "Phy.h"
#include "UART_Interface.h"
#include "MC1324xDrv.h"
#include "MC1324xReg.h"
#include "Keyboard.h"

/*****************************************************************************
 *                             PRIVATE MACROS                                *
 *---------------------------------------------------------------------------*
 * Add to this section all the access macros, registers mappings, bit access *
 * macros, masks, flags etc ...                                              *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/* Minimum sleep ticks (16us) in DeepSleepMode 13  */
#define PWR_MINIMUM_SLEEP_TICKS   10

/*****************************************************************************
 *                               PRIVATE VARIABLES                           *
 *---------------------------------------------------------------------------*
 * Add to this section all the variables and constants that have local       *
 * (file) scope.                                                             *
 * Each of this declarations shall be preceded by the 'static' keyword.      *
 * These variables / constants cannot be accessed outside this module.       *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

uint8_t mLPMFlag = gAllowDeviceToSleep_c;

#if (cPWR_UsePowerDownMode)
static uint32_t mPWR_DeepSleepTime = cPWR_DeepSleepDurationMs;
static bool_t   mPWR_DeepSleepTimeInSymbols = FALSE;

#if (gTMR_EnableLowPowerTimers_d)
static uint32_t notCountedTicksBeforeSleep;
#endif //(gTMR_EnableLowPowerTimers_d) 
#endif //(cPWR_UsePowerDownMode)

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

#if (cPWR_UsePowerDownMode)

/*---------------------------------------------------------------------------
 * Name: PWR_HandleDeepSleep
 * Description: - 
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
static PWRLib_WakeupReason_t PWR_HandleDeepSleep
(
  zbClock24_t DozeDuration
);

/*---------------------------------------------------------------------------
 * Name: PWR_HandleSleep
 * Description: - 
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
static PWRLib_WakeupReason_t PWR_HandleSleep
(
  zbClock24_t DozeDuration
);

/*---------------------------------------------------------------------------
 * Name: PWR_SleepAllowed
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
static bool_t PWR_SleepAllowed
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PWR_DeepSleepAllowed
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
static bool_t PWR_DeepSleepAllowed
(
  void
);

/*****************************************************************************
 *                             PRIVATE FUNCTIONS                             *
 *---------------------------------------------------------------------------*
 * Add to this section all the functions that have local (file) scope.       *
 * These functions cannot be accessed outside this module.                   *
 * These definitions shall be preceded by the 'static' keyword.              *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*---------------------------------------------------------------------------
 * Name: PWR_HandleDeepSleep
 * Description: - 
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#if ( (gTargetTWR_K60D100M_d == 1) || \
    ( (gTarget_UserDefined_d == 1) && (MCU_MK60D10 == 1) ) )
static PWRLib_WakeupReason_t PWR_HandleDeepSleep
(
  zbClock24_t DozeDuration
)
{
  PWRLib_WakeupReason_t  Res;
  uint32_t deepSleepTicks = 0;

  /* to avoid unused warning*/
#if (cPWR_DeepSleepMode == 14)  
  Res.AllBits = 0xff | (uint8_t) DozeDuration | (uint8_t) deepSleepTicks | (uint8_t) mPWR_DeepSleepTime | (uint8_t) mPWR_DeepSleepTimeInSymbols;
#else
  Res.AllBits = 0xff | (uint8_t) DozeDuration | (uint8_t) deepSleepTicks | (uint8_t) mPWR_DeepSleepTime | (uint8_t) mPWR_DeepSleepTimeInSymbols | (uint8_t) notCountedTicksBeforeSleep;
#endif

  Res.AllBits = 0;
  PWRLib_MCU_WakeupReason.AllBits = 0;
  /*---------------------------------------------------------------------------*/
  #if (cPWR_DeepSleepMode == 0)
    Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
    Res.AllBits = 0xff | (uint8_t) DozeDuration;  // Last part to avoid unused warning
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 1)
    #if (gTsiSupported_d == TRUE)

      /* configure MCG in FLL Engaged Internal (FEI) mode */
      #if defined(gMCG_FEE_Mode_d)
        MCG_Fee2Fei();
      #else //#if defined(gMCG_FEE_Mode_d)
        MCG_Pee2Fei();
      #endif
      
      /* configure Radio in hibernate mode */
      PWRLib_Radio_Enter_Hibernate();
      
      /* prepare TSI for low power operation */
      TSI_ModuleEnableLowPowerWakeup();
      
      PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO

      /* configure MCU in VLLS2 low power mode */
      PWRLib_MCU_Enter_VLLS2();
      
      /* never returns. VLLSx wakeup goes through Reset sequence. */
      //while(1);
    #else /* #if (gTsiSupported_d) else */
      #error "*** ERROR: gTsiSupported_d has to be set to TRUE"
    #endif /* #if (gTsiSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 2)
    #if (gTsiSupported_d == FALSE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz)

        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif

        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
      
        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cPWR_LPTMRTickTime, DozeDuration);
      
        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO

        /* configure MCU in VLLS2 low power mode */
        PWRLib_MCU_Enter_VLLS2();
      
        /* never returns. VLLSx wakeup goes through Reset sequence. */
        //while(1);
      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Int_LPO_1KHz"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gTsiSupported_d) else */
      #error "*** ERROR: gTsiSupported_d has to be set to FALSE"
    #endif /* #if (gTsiSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 3)
    #if (gTsiSupported_d == FALSE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Ext_ERCLK32K)
    
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif
      
        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();

        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cPWR_LPTMRTickTime, DozeDuration);
      
        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO

        /* configure MCU in VLLS2 low power mode */
        PWRLib_MCU_Enter_VLLS2();
      
        /* never returns. VLLSx wakeup goes through Reset sequence. */
        //while(1);
      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Ext_ERCLK32K"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gTsiSupported_d) else */
      #error "*** ERROR: gTsiSupported_d has to be set to FALSE"
    #endif /* #if (gTsiSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 4)
    #if (gTsiSupported_d == FALSE)
        
      /* configure MCG in FLL Engaged Internal (FEI) mode */
      #if defined(gMCG_FEE_Mode_d)
        MCG_Fee2Fei();
      #else //#if defined(gMCG_FEE_Mode_d)
        MCG_Pee2Fei();
      #endif
      
      /* configure Radio in hibernate mode */
      PWRLib_Radio_Enter_Hibernate();
      
      /* start RTC */
      while(PWRLib_RTC_IsOscStarted() == FALSE){}
      
      PWRLib_RTC_ClockStart(DozeDuration);
      
      PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO

      /* configure MCU in VLLS2 low power mode */
      PWRLib_MCU_Enter_VLLS2();
      
      /* never returns. VLLSx wakeup goes through Reset sequence. */
      //while(1);
    #else /* #if (gTsiSupported_d) else */
      #error "*** ERROR: gTsiSupported_d has to be set to FALSE"
    #endif /* #if (gTsiSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 5)
    #if (gTsiSupported_d == TRUE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz)
    
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif
      
        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
      
        /* prepare TSI for low power operation */
        TSI_ModuleEnableLowPowerWakeup();
      
        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cPWR_LPTMRTickTime, DozeDuration);

        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO
      
        /* configure MCU in VLLS2 low power mode */
        PWRLib_MCU_Enter_VLLS2();
      
        /* never returns. VLLSx wakeup goes through Reset sequence. */
        //while(1);
      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Int_LPO_1KHz"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gTsiSupported_d) else */
      #error "*** ERROR: gTsiSupported_d has to be set to TRUE"
    #endif /* #if (gTsiSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 6)
    #if (gTsiSupported_d == TRUE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Ext_ERCLK32K)
    
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif
      
        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
      
        /* prepare TSI for low power operation */
        TSI_ModuleEnableLowPowerWakeup();
      
        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cPWR_LPTMRTickTime, DozeDuration);

        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO
      
        /* configure MCU in VLLS2 low power mode */
        PWRLib_MCU_Enter_VLLS2();
      
        /* never returns. VLLSx wakeup goes through Reset sequence. */
        //while(1);
      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Ext_ERCLK32K"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gTsiSupported_d) else */
      #error "*** ERROR: gTsiSupported_d has to be set to TRUE"
    #endif /* #if (gTsiSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 7)
    #if (gTsiSupported_d == TRUE)
        
      /* configure MCG in FLL Engaged Internal (FEI) mode */
      #if defined(gMCG_FEE_Mode_d)
        MCG_Fee2Fei();
      #else //#if defined(gMCG_FEE_Mode_d)
        MCG_Pee2Fei();
      #endif
      
      /* configure Radio in hibernate mode */
      PWRLib_Radio_Enter_Hibernate();

      /* prepare TSI for low power operation */
      TSI_ModuleEnableLowPowerWakeup();
      
      /* start RTC */
      while(PWRLib_RTC_IsOscStarted() == FALSE){}
      
      PWRLib_RTC_ClockStart(DozeDuration);

      PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO
      
      /* configure MCU in VLLS2 low power mode */
      PWRLib_MCU_Enter_VLLS2();
      
      /* never returns. VLLSx wakeup goes through Reset sequence. */
      //while(1);
    #else /* #if (gTsiSupported_d) else */
      #error "*** ERROR: gTsiSupported_d has to be set to TRUE"
    #endif /* #if (gTsiSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 8)
    #if (gTsiSupported_d == TRUE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz)
      
        /* converts deep sleep duration from ms/symbols in LPTMR ticks */
        if(!mPWR_DeepSleepTimeInSymbols)
        {
          deepSleepTicks = ( mPWR_DeepSleepTime >> ( ( cLPTMR_PRS_00002ms >> 3 ) + 1) );
        }
        else
        {
          deepSleepTicks = ( ( mPWR_DeepSleepTime / 125 ) >> ( cLPTMR_PRS_00002ms >> 3 ) );
        }
    
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif
      
        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
      
        /* prepare TSI for low power operation */
        TSI_ModuleEnableLowPowerWakeup();
      
        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cLPTMR_PRS_00002ms, deepSleepTicks);
      
        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO

        /* configure MCU in VLLS2 low power mode */
        PWRLib_MCU_Enter_VLLS2();
      
        /* never returns. VLLSx wakeup goes through Reset sequence. */
        //while(1);
      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Int_LPO_1KHz"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gTsiSupported_d) else */
      #error "*** ERROR: gTsiSupported_d has to be set to TRUE"
    #endif /* #if (gTsiSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 9)
    #if (gTsiSupported_d == TRUE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz)
    
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif
          
        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
      
        /* prepare TSI for low power operation */
        TSI_ModuleEnableLowPowerWakeup();
      
        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cPWR_LPTMRTickTime, DozeDuration);

        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO
      
        /* configure MCU in LLS low power mode */
        PWRLib_MCU_Enter_LLS();
        
        /* checks sources of wakeup */
        /* low power timer wakeup */
        if(PWRLib_LLWU_IsLPTMRWakeUpSource() == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromLPTMR = 1;
        }
        
        /* TSI wakeup */
        if(PWRLib_LLWU_IsTSIWakeUpSource() == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1;
        }

        /* stop LPTMR */
        PWRLib_LPTMR_ClockStop();
        
        /* configure Radio in autodoze mode */
        PWRLib_Radio_Enter_AutoDoze();

        /* configure MCG in PEE/FEE mode*/
        #if defined(gMCG_FEE_Mode_d)
          /* FEE @ 48MHz */
          gMCG_coreClkMHz = MCG_Fei2Fee();
        #else //#if defined(gMCG_FEE_Mode_d)
          /* PEE @ 48MHz */
          gMCG_coreClkMHz = MCG_PLLInit();
        #endif //#if defined(gMCG_FEE_Mode_d)
        
        /* Sync. the low power timers */
        #if (gTMR_EnableLowPowerTimers_d)
        {
          uint32_t deepSleepDurationMs;
          
          /* Converts the DozeDuration from PWRLib timer ticks in ms */
          #if(cPWR_LPTMRTickTime == cLPTMR_PRS_00001ms)
            deepSleepDurationMs = DozeDuration - PWRLib_LPTMR_ClockCheck();
          #else
            deepSleepDurationMs = ( DozeDuration - PWRLib_LPTMR_ClockCheck() ) * (1 << ( (cPWR_LPTMRTickTime >> 3) + 1 ));
          #endif

          /* Converts the DozeDuration from ms in software timer ticks and synchronize low power timers */
          TMR_SyncLpmTimers( (TmrTicksFromMilliseconds( ( tmrTimeInMilliseconds_t) deepSleepDurationMs) + notCountedTicksBeforeSleep) );
        }
        #endif
        
        Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
                 
        if( PWRLib_LPTMR_ClockCheck() == 0)
        {
          Res.Bits.DeepSleepTimeout = 1;
          cPWR_DeepSleepWakeupStackProc; // User function called only on timeout
        }
        
        /* prepare TSI for normal operation */
        TSI_ModuleDisableLowPowerWakeup();
        
      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Int_LPO_1KHz"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gTsiSupported_d) else */
      #error "*** ERROR: gTsiSupported_d has to be set to TRUE"
    #endif /* #if (gTsiSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 10)
    #if (gTsiSupported_d == TRUE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Ext_ERCLK32K)
    
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif
      
        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
      
        /* prepare TSI for low power operation */
        TSI_ModuleEnableLowPowerWakeup();
      
        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cPWR_LPTMRTickTime, DozeDuration);

        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO
      
        /* configure MCU in LLS low power mode */
        PWRLib_MCU_Enter_LLS();
        
        /* checks sources of wakeup */
        /* low power timer wakeup */
        if(PWRLib_LLWU_IsLPTMRWakeUpSource() == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromLPTMR = 1;
        }
        
        /* TSI wakeup */
        if(PWRLib_LLWU_IsTSIWakeUpSource() == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1;
        }
        
        /* stop LPTMR */
        PWRLib_LPTMR_ClockStop();
        
        /* configure Radio in autodoze mode */
        PWRLib_Radio_Enter_AutoDoze();
        
        /* configure MCG in PEE/FEE mode*/
        #if defined(gMCG_FEE_Mode_d)
          /* FEE @ 48MHz */
          gMCG_coreClkMHz = MCG_Fei2Fee();
        #else //#if defined(gMCG_FEE_Mode_d)
          /* PEE @ 48MHz */
          gMCG_coreClkMHz = MCG_PLLInit();
        #endif //#if defined(gMCG_FEE_Mode_d)
        
        /* Sync. the low power timers */
        #if (gTMR_EnableLowPowerTimers_d)
        {
          uint32_t deepSleepDurationMs;
          
          /* Converts the DozeDuration from PWRLib timer ticks in ms */
          #if(cPWR_LPTMRTickTime == cLPTMR_PRS_125_div_by_4096ms)
            deepSleepDurationMs = ( ( DozeDuration - PWRLib_LPTMR_ClockCheck() ) * 125 ) >> 12;
          #elif(cPWR_LPTMRTickTime < cLPTMR_PRS_0125ms)
            deepSleepDurationMs = ( ( DozeDuration - PWRLib_LPTMR_ClockCheck() ) * 125 ) >> (11 - (cPWR_LPTMRTickTime >> 3 ) ) ;
          #else
            deepSleepDurationMs = ( ( DozeDuration - PWRLib_LPTMR_ClockCheck() ) * 125 ) << ( ( cPWR_LPTMRTickTime >> 3 ) - 11 );
          #endif

          /* Converts the DozeDuration from ms in software timer ticks and synchronize low power timers */
          TMR_SyncLpmTimers( (TmrTicksFromMilliseconds( ( tmrTimeInMilliseconds_t) deepSleepDurationMs) + notCountedTicksBeforeSleep) );
        }
        #endif
        
        Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
                 
        if( PWRLib_LPTMR_ClockCheck() == 0)
        {
          Res.Bits.DeepSleepTimeout = 1;
          cPWR_DeepSleepWakeupStackProc; // User function called only on timeout
        }
        
        /* prepare TSI for normal operation */
        TSI_ModuleDisableLowPowerWakeup();
        
      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Ext_ERCLK32K"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gTsiSupported_d) else */
      #error "*** ERROR: gTsiSupported_d has to be set to TRUE"
    #endif /* #if (gTsiSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 11)
    #if (gTsiSupported_d == TRUE)
        
      /* configure MCG in FLL Engaged Internal (FEI) mode */
      #if defined(gMCG_FEE_Mode_d)
        MCG_Fee2Fei();
      #else //#if defined(gMCG_FEE_Mode_d)
        MCG_Pee2Fei();
      #endif
      
      /* configure Radio in hibernate mode */
      PWRLib_Radio_Enter_Hibernate();
              
      /* prepare TSI for low power operation */
      TSI_ModuleEnableLowPowerWakeup();
      
      /* start RTC */
      while(PWRLib_RTC_IsOscStarted() == FALSE){}
      
      PWRLib_RTC_ClockStart(DozeDuration);

      PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO
      
      /* configure MCU in LLS low power mode */
      PWRLib_MCU_Enter_LLS();
      
      /* checks sources of wakeup */
      /* real time counter wakeup */
      if(PWRLib_LLWU_IsRTCWakeUpSource() == TRUE)
      {
        PWRLib_MCU_WakeupReason.Bits.FromRTC = 1;
      }
        
      /* TSI wakeup */
      if(PWRLib_LLWU_IsTSIWakeUpSource() == TRUE)
      {
        PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1;
      }
      
      /* stop RTC */
      PWRLib_RTC_ClockStop();
        
      /* configure Radio in autodoze mode */
      PWRLib_Radio_Enter_AutoDoze();
      
      /* configure MCG in PEE/FEE mode*/
      #if defined(gMCG_FEE_Mode_d)
        /* FEE @ 48MHz */
        gMCG_coreClkMHz = MCG_Fei2Fee();
      #else //#if defined(gMCG_FEE_Mode_d)
        /* PEE @ 48MHz */
        gMCG_coreClkMHz = MCG_PLLInit();
      #endif //#if defined(gMCG_FEE_Mode_d)
        
      /* Sync. the low power timers */
      #if (gTMR_EnableLowPowerTimers_d)
      {
        uint32_t deepSleepDurationMs;
        /* Converts the DozeDuration from PWRLib timer ticks in ms */
        deepSleepDurationMs = ( ( DozeDuration - PWRLib_RTC_ClockCheck() ) * 1000 );
        /* Converts the DozeDuration from ms in software timer ticks and synchronize low power timers */
        TMR_SyncLpmTimers( (TmrTicksFromMilliseconds( ( tmrTimeInMilliseconds_t) deepSleepDurationMs) + notCountedTicksBeforeSleep) );
      }
      #endif
      
      Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
                 
      if( PWRLib_RTC_ClockCheck() == 0)
      {
        Res.Bits.DeepSleepTimeout = 1;
        cPWR_DeepSleepWakeupStackProc; // User function called only on timeout
      }
        
      /* prepare TSI for normal operation */
      TSI_ModuleDisableLowPowerWakeup();
        
    #else /* #if (gTsiSupported_d) else */
      #error "*** ERROR: gTsiSupported_d has to be set to TRUE"
    #endif /* #if (gTsiSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 12)
    #if (gTsiSupported_d == TRUE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz)
      
        /* converts deep sleep duration from ms/symbols in LPTMR ticks */
        if(!mPWR_DeepSleepTimeInSymbols)
        {
          deepSleepTicks = ( mPWR_DeepSleepTime >> ( ( cLPTMR_PRS_00002ms >> 3 ) + 1) );
        }
        else
        {
          deepSleepTicks = ( ( mPWR_DeepSleepTime / 125 ) >> ( cLPTMR_PRS_00002ms >> 3 ) );
        }
    
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif
      
        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
      
        /* prepare TSI for low power operation */
        TSI_ModuleEnableLowPowerWakeup();
   
        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cLPTMR_PRS_00002ms, deepSleepTicks);

        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO
        
        /* configure MCU in LLS low power mode */
        PWRLib_MCU_Enter_LLS();

        /* checks sources of wakeup */
        /* low power timer wakeup */
        if(PWRLib_LLWU_IsLPTMRWakeUpSource() == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromLPTMR = 1;
        }
        
        /* TSI wakeup */
        if(PWRLib_LLWU_IsTSIWakeUpSource() == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1;
        }
        
        /* stop LPTMR */
        PWRLib_LPTMR_ClockStop();

        /* configure Radio in autodoze mode */
        PWRLib_Radio_Enter_AutoDoze();
        
        /* configure MCG in PEE/FEE mode*/
        #if defined(gMCG_FEE_Mode_d)
          /* FEE @ 48MHz */
          gMCG_coreClkMHz = MCG_Fei2Fee();
        #else //#if defined(gMCG_FEE_Mode_d)
          /* PEE @ 48MHz */
          gMCG_coreClkMHz = MCG_PLLInit();
        #endif //#if defined(gMCG_FEE_Mode_d)
          
        /* Sync. the low power timers */
        #if (gTMR_EnableLowPowerTimers_d)
        {
          uint32_t deepSleepDurationMs;
          /* Converts the DozeDuration from PWRLib timer ticks in ms */
          deepSleepDurationMs = ( ( deepSleepTicks - PWRLib_LPTMR_ClockCheck() ) * 2 );
          /* Converts the DozeDuration from ms in software timer ticks and synchronize low power timers */
          TMR_SyncLpmTimers( (TmrTicksFromMilliseconds( ( tmrTimeInMilliseconds_t) deepSleepDurationMs) + notCountedTicksBeforeSleep) );
        }
        #endif
        
        Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
                 
        if( PWRLib_LPTMR_ClockCheck() == 0)
        {
          Res.Bits.DeepSleepTimeout = 1;
          cPWR_DeepSleepWakeupStackProc; // User function called only on timeout
        }
        
        /* prepare TSI for normal operation */
        TSI_ModuleDisableLowPowerWakeup();
        
      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Int_LPO_1KHz"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gTsiSupported_d) else */
      #error "*** ERROR: gTsiSupported_d has to be set to TRUE"
    #endif /* #if (gTsiSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 13)
    #if (gTsiSupported_d == TRUE)
    {
      #if ( (gUart1_Enabled_d == TRUE) || (gUart2_Enabled_d == TRUE) )
      {
      zbClock24_t currentTime;
      zbClock24_t absoluteWakeUpTime;
      
      /* converts deep sleep duration from ms to symbols */
      if(!mPWR_DeepSleepTimeInSymbols)
      {
        deepSleepTicks = ( ( ( mPWR_DeepSleepTime / 2 ) * 125 ) + ( ( mPWR_DeepSleepTime & 1 ) * 62 ) ) & 0xFFFFFF;
      }
      else
      {
        deepSleepTicks = mPWR_DeepSleepTime;
      }
      
      if( deepSleepTicks > PWR_MINIMUM_SLEEP_TICKS )
      {
        // Set prescaller to obtain 1 symbol (16us) timebase TODO
        MC1324xDrv_IndirectAccessSPIWrite(TMR_PRESCALE, 0x05);
		
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif
        
        /* disable transceiver CLK_OUT. */
        MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE);
        
        /* configure Radio in Doze mode */
        PWRLib_Radio_Enter_Doze();
        
        /* prepare UART for low power operation */
        Uart_ModuleEnableLowPowerWakeup();
        
        /* prepare TSI for low power operation */
        TSI_ModuleEnableLowPowerWakeup();
        
        /* read current time */
        PhyTimeReadClock(&currentTime);
        
        /* compute absolute end time */
        absoluteWakeUpTime = currentTime + deepSleepTicks;

        /* set absolute wakeup time */
        PhyTimeSetWakeUpTime(&absoluteWakeUpTime);

        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO 
        
        /* configure MCU in LLS low power mode */
        PWRLib_MCU_Enter_VLPS();

        /* checks sources of wakeup */
        /* radio timer wakeup */
        if( PhyTimeIsWakeUpTimeExpired() == TRUE)
        {
          Res.Bits.DeepSleepTimeout = 1;     /* Sleep timeout ran out */
          Res.Bits.FromTimer = 1;            /* Wakeup by radio timer */
          absoluteWakeUpTime = 0;            /* not counted radio timer ticks (16 us) */
        }
        else
        {
          PhyTimeReadClock(&absoluteWakeUpTime);
          absoluteWakeUpTime -= currentTime;                           /* not counted radio timer ticks (16 us) */
        }
        
        /* TSI module wakeup */
        if( TSI_IsWakeUpSource() == TRUE)
        {
          Res.Bits.FromKeyBoard = 1;
        }
        
        /* UART module wakeup */
        if( Uart_IsWakeUpSource() == TRUE)
        {
          Res.Bits.FromUART = 1;
        }

        /* configure Radio in autodoze mode */
        PWRLib_Radio_Enter_AutoDoze();
        
        /* configure MCG in PEE/FEE mode*/
        #if defined(gMCG_FEE_Mode_d)
          /* FEE @ 48MHz */
          gMCG_coreClkMHz = MCG_Fei2Fee();
        #else //#if defined(gMCG_FEE_Mode_d)
          /* PEE @ 48MHz */
          gMCG_coreClkMHz = MCG_PLLInit();
        #endif //#if defined(gMCG_FEE_Mode_d)
        
        /* Sync. the low power timers */
        #if (gTMR_EnableLowPowerTimers_d)
        {
          uint32_t deepSleepDurationMs;
          deepSleepDurationMs = ( ( absoluteWakeUpTime * 10 ) / 625 );  /* convert from radio timer ticks in ms */
          /* Converts the DozeDuration from ms in software timer ticks and synchronize low power timers */
          TMR_SyncLpmTimers( (TmrTicksFromMilliseconds( ( tmrTimeInMilliseconds_t) deepSleepDurationMs) + notCountedTicksBeforeSleep) );
        }
        #endif
        
        /* prepare UART for normal operation */
        Uart_ModuleDisableLowPowerWakeup();
        
        /* prepare TSI for normal operation */
        TSI_ModuleDisableLowPowerWakeup();
      }
      else
      {
        /* Not enough time to program the TRM compare */
         Res.Bits.DeepSleepTimeout = 1;     /* Sleep timeout ran out */
      }
      if(Res.Bits.DeepSleepTimeout == 1) 
      {
        cPWR_DeepSleepWakeupStackProc; // User function called only on timeout
      }
      
      }
      #else //#if ( (gUart1_Enabled_d == TRUE) || (gUart2_Enabled_d == TRUE) )
        #error "*** ERROR: gUart1_Enabled_d or gUart2_Enabled_d has to be set to TRUE"
      #endif    
    }
    #else /* #if (gTsiSupported_d) else */
      #error "*** ERROR: gTsiSupported_d has to be set to TRUE"
    #endif /* #if (gTsiSupported_d) */
    /*---------------------------------------------------------------------------*/
      #elif (cPWR_DeepSleepMode == 14)
        #if ((gTsiSupported_d == TRUE) && (gTMR_Enabled_d == TRUE))
    	
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif
          
        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
      
        /* prepare TSI for low power operation */
        TSI_ModuleEnableLowPowerWakeup();

        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
      
        /* configure MCU in LLS low power mode */
        PWRLib_MCU_Enter_LLS();
        
        /* checks sources of wakeup */
        /* low power timer wakeup */
        if(PWRLib_LLWU_IsLPTMRWakeUpSource() == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromLPTMR = 1;
          PWRLib_MCU_WakeupReason.Bits.DeepSleepTimeout = 1;
        }
        
        /* TSI wakeup */
        if(PWRLib_LLWU_IsTSIWakeUpSource() == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1;
        }
        
        /* configure Radio in autodoze mode */
        PWRLib_Radio_Enter_AutoDoze();

        /* configure MCG in PEE/FEE mode*/
        #if defined(gMCG_FEE_Mode_d)
          /* FEE @ 48MHz */
          gMCG_coreClkMHz = MCG_Fei2Fee();
        #else //#if defined(gMCG_FEE_Mode_d)
          /* PEE @ 48MHz */
          gMCG_coreClkMHz = MCG_PLLInit();
        #endif //#if defined(gMCG_FEE_Mode_d)
                        
        Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
        
        /* prepare TSI for normal operation */
        TSI_ModuleDisableLowPowerWakeup();
        #else /* #if (gTsiSupported_d) else */
          #error "*** ERROR: gTsiSupported_d has to be set to TRUE"
        #endif /* #if (gTsiSupported_d) */    
  /*---------------------------------------------------------------------------*/
  #else
    #error "*** ERROR: Not a valid cPWR_DeepSleepMode chosen"
  #endif
  
  PWRLib_MCU_WakeupReason.AllBits = 0;
  return Res;
}
#elif ( (gTargetTWR_KW21D512_d == 1) || \
        (gTargetTWR_KW21D256_d == 1) || \
        (gTargetTWR_KW22D512_d == 1) || \
        (gTargetTWR_KW24D512_d == 1) || \
        (gTargetKW24D512_USB_d == 1) || \
        ( (gTarget_UserDefined_d == 1) && (MCU_MK21DN512 == 1) ) || \
        ( (gTarget_UserDefined_d == 1) && (MCU_MK21DX256 == 1) ) )  //TODO

static PWRLib_WakeupReason_t PWR_HandleDeepSleep
(
  zbClock24_t DozeDuration
)
{
  PWRLib_WakeupReason_t  Res;
  uint32_t deepSleepTicks = 0;
 
  /* to avoid unused warning*/
#if gTMR_EnableLowPowerTimers_d  
  Res.AllBits = 0xff | (uint8_t) DozeDuration | (uint8_t) deepSleepTicks | (uint8_t) mPWR_DeepSleepTime | (uint8_t) mPWR_DeepSleepTimeInSymbols |(uint8_t) notCountedTicksBeforeSleep;  
#else
  Res.AllBits = 0xff | (uint8_t) DozeDuration | (uint8_t) deepSleepTicks | (uint8_t) mPWR_DeepSleepTime | (uint8_t) mPWR_DeepSleepTimeInSymbols;
#endif

  Res.AllBits = 0;
  PWRLib_MCU_WakeupReason.AllBits = 0;
  /*---------------------------------------------------------------------------*/
  #if (cPWR_DeepSleepMode == 0)
    Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
    Res.AllBits = 0xff | (uint8_t) DozeDuration;  // Last part to avoid unused warning
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 1)
    #if (gKeyBoardSupported_d == TRUE)

      /* configure MCG in FLL Engaged Internal (FEI) mode */
      #if defined(gMCG_FEE_Mode_d)
        MCG_Fee2Fei();
      #else //#if defined(gMCG_FEE_Mode_d)
        MCG_Pee2Fei();
      #endif
      
      /* disable transceiver CLK_OUT. */
      MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE);

      /* configure Radio in hibernate mode */
      PWRLib_Radio_Enter_Hibernate();
      
      PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO

      /* configure MCU in VLLS2 low power mode */
      PWRLib_MCU_Enter_VLLS2();
      
      /* never returns. VLLSx wakeup goes through Reset sequence. */
      //while(1);
    #else /* #if (gKeyBoardSupported_d) else */
      #error "*** ERROR: gKeyBoardSupported_d has to be set to TRUE"
    #endif /* #if (gKeyBoardSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 2)
    #if (gKeyBoardSupported_d == FALSE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz)

        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif

        /* disable transceiver CLK_OUT. */
        MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE);

        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
      
        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cPWR_LPTMRTickTime, DozeDuration);
      
        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO

        /* configure MCU in VLLS2 low power mode */
        PWRLib_MCU_Enter_VLLS2();
      
        /* never returns. VLLSx wakeup goes through Reset sequence. */
        //while(1);
      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Int_LPO_1KHz"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gKeyBoardSupported_d) else */
      #error "*** ERROR: gKeyBoardSupported_d has to be set to FALSE"
    #endif /* #if (gKeyBoardSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 3)
    #if (gKeyBoardSupported_d == FALSE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Ext_ERCLK32K)
    
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif

        /* disable transceiver CLK_OUT. */
        MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE);
      
        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
        
        #if gPWR_EnsureOscStabilized_d
          /* start 32KHz OSC */
          while(PWRLib_RTC_IsOscStarted() == FALSE){}
        #endif
          
        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cPWR_LPTMRTickTime, DozeDuration);
      
        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO

        /* configure MCU in VLLS2 low power mode */
        PWRLib_MCU_Enter_VLLS2();
      
        /* never returns. VLLSx wakeup goes through Reset sequence. */
        //while(1);
      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Ext_ERCLK32K"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gKeyBoardSupported_d) else */
      #error "*** ERROR: gKeyBoardSupported_d has to be set to FALSE"
    #endif /* #if (gKeyBoardSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 4)
    #if (gKeyBoardSupported_d == FALSE)
        
      /* configure MCG in FLL Engaged Internal (FEI) mode */
      #if defined(gMCG_FEE_Mode_d)
        MCG_Fee2Fei();
      #else //#if defined(gMCG_FEE_Mode_d)
        MCG_Pee2Fei();
      #endif

      /* disable transceiver CLK_OUT. */
      MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE);
      
      /* configure Radio in hibernate mode */
      PWRLib_Radio_Enter_Hibernate();
      
      #if gPWR_EnsureOscStabilized_d
        /* start RTC */
        while(PWRLib_RTC_IsOscStarted() == FALSE){}
      #endif
      
      PWRLib_RTC_ClockStart(DozeDuration);
      
      PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO

      /* configure MCU in VLLS2 low power mode */
      PWRLib_MCU_Enter_VLLS2();
      
      /* never returns. VLLSx wakeup goes through Reset sequence. */
      //while(1);
    #else /* #if (gKeyBoardSupported_d) else */
      #error "*** ERROR: gKeyBoardSupported_d has to be set to FALSE"
    #endif /* #if (gKeyBoardSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 5)
    #if (gKeyBoardSupported_d == TRUE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz)
    
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif

        /* disable transceiver CLK_OUT. */
        MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE);
      
        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
      
        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cPWR_LPTMRTickTime, DozeDuration);

        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO
      
        /* configure MCU in VLLS2 low power mode */
        PWRLib_MCU_Enter_VLLS2();
      
        /* never returns. VLLSx wakeup goes through Reset sequence. */
        //while(1);
      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Int_LPO_1KHz"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gKeyBoardSupported_d) else */
      #error "*** ERROR: gKeyBoardSupported_d has to be set to TRUE"
    #endif /* #if (gKeyBoardSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 6)
    #if (gKeyBoardSupported_d == TRUE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Ext_ERCLK32K)
    
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif
      
        /* disable transceiver CLK_OUT. */
        MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE);
      
        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
        
        #if gPWR_EnsureOscStabilized_d
          /* start 32KHz OSC */
          while(PWRLib_RTC_IsOscStarted() == FALSE){}
        #endif
          
        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cPWR_LPTMRTickTime, DozeDuration);

        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO
      
        /* configure MCU in VLLS2 low power mode */
        PWRLib_MCU_Enter_VLLS2();
      
        /* never returns. VLLSx wakeup goes through Reset sequence. */
        //while(1);
      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Ext_ERCLK32K"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gKeyBoardSupported_d) else */
      #error "*** ERROR: gKeyBoardSupported_d has to be set to TRUE"
    #endif /* #if (gKeyBoardSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 7)
    #if (gKeyBoardSupported_d == TRUE)
        
      /* configure MCG in FLL Engaged Internal (FEI) mode */
      #if defined(gMCG_FEE_Mode_d)
        MCG_Fee2Fei();
      #else //#if defined(gMCG_FEE_Mode_d)
        MCG_Pee2Fei();
      #endif
      
      /* disable transceiver CLK_OUT. */
      MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE);
      
      /* configure Radio in hibernate mode */
      PWRLib_Radio_Enter_Hibernate();

      #if gPWR_EnsureOscStabilized_d
        /* start RTC */
        while(PWRLib_RTC_IsOscStarted() == FALSE){}
      #endif
        
      PWRLib_RTC_ClockStart(DozeDuration);

      PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO
      
      /* configure MCU in VLLS2 low power mode */
      PWRLib_MCU_Enter_VLLS2();
      
      /* never returns. VLLSx wakeup goes through Reset sequence. */
      //while(1);
    #else /* #if (gKeyBoardSupported_d) else */
      #error "*** ERROR: gKeyBoardSupported_d has to be set to TRUE"
    #endif /* #if (gKeyBoardSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 8)
    #if (gKeyBoardSupported_d == TRUE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz)
      
        /* converts deep sleep duration from ms/symbols in LPTMR ticks */
        if(!mPWR_DeepSleepTimeInSymbols)
        {
          deepSleepTicks = ( mPWR_DeepSleepTime >> ( ( cLPTMR_PRS_00002ms >> 3 ) + 1) );
        }
        else
        {
          deepSleepTicks = ( ( mPWR_DeepSleepTime / 125 ) >> ( cLPTMR_PRS_00002ms >> 3 ) );
        }
    
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif
      
        /* disable transceiver CLK_OUT. */
        MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE);
      
        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
      
        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cLPTMR_PRS_00002ms, deepSleepTicks);
      
        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO

        /* configure MCU in VLLS2 low power mode */
        PWRLib_MCU_Enter_VLLS2();
      
        /* never returns. VLLSx wakeup goes through Reset sequence. */
        //while(1);
      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Int_LPO_1KHz"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gKeyBoardSupported_d) else */
      #error "*** ERROR: gKeyBoardSupported_d has to be set to TRUE"
    #endif /* #if (gKeyBoardSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 9)
    #if (gKeyBoardSupported_d == TRUE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz)
    
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif
          
        /* disable transceiver CLK_OUT. */
        MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE);
      
        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
      
        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cPWR_LPTMRTickTime, DozeDuration);

        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO
      
        /* configure MCU in LLS low power mode */
        PWRLib_MCU_Enter_LLS();
        
        /* checks sources of wakeup */
        /* low power timer wakeup */
        if(PWRLib_LLWU_IsLPTMRWakeUpSource() == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromLPTMR = 1;
        }
        
        /* GPIO wakeup */
        if(PWRLib_LLWU_IsGPIOWakeUpSource(gLLWU_WakeUp_PIN_Number_c) == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1;
        }

        /* stop LPTMR */
        PWRLib_LPTMR_ClockStop();
        
        /* configure Radio in autodoze mode */
        PWRLib_Radio_Enter_AutoDoze();

        #if defined(gMCG_FEE_Mode_d)
          MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_32_78_KHz);
        #else
          MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_4_MHz);
        #endif //#if defined(gMCG_FEE_Mode_d)

        /* configure MCG in PEE/FEE mode*/
        #if defined(gMCG_FEE_Mode_d)
          /* FEE @ 48MHz */
          gMCG_coreClkMHz = MCG_Fei2Fee();
        #else //#if defined(gMCG_FEE_Mode_d)
          /* PEE @ 48MHz */
          gMCG_coreClkMHz = MCG_PLLInit();
        #endif //#if defined(gMCG_FEE_Mode_d)
        
        /* Sync. the low power timers */
        #if (gTMR_EnableLowPowerTimers_d)
        {
          uint32_t deepSleepDurationMs;
          
          /* Converts the DozeDuration from PWRLib timer ticks in ms */
          #if(cPWR_LPTMRTickTime == cLPTMR_PRS_00001ms)
            deepSleepDurationMs = DozeDuration - PWRLib_LPTMR_ClockCheck();
          #else
            deepSleepDurationMs = ( DozeDuration - PWRLib_LPTMR_ClockCheck() ) * (1 << ( (cPWR_LPTMRTickTime >> 3) + 1 ));
          #endif

          /* Converts the DozeDuration from ms in software timer ticks and synchronize low power timers */
          TMR_SyncLpmTimers( (TmrTicksFromMilliseconds( ( tmrTimeInMilliseconds_t) deepSleepDurationMs) + notCountedTicksBeforeSleep) );
        }
        #endif
        
                 
        if( PWRLib_LPTMR_ClockCheck() == 0)
        {
          PWRLib_MCU_WakeupReason.Bits.DeepSleepTimeout = 1;
          cPWR_DeepSleepWakeupStackProc; // User function called only on timeout
        }
        Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
        
      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Int_LPO_1KHz"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gKeyBoardSupported_d) else */
      #error "*** ERROR: gKeyBoardSupported_d has to be set to TRUE"
    #endif /* #if (gKeyBoardSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 10)
    #if (gKeyBoardSupported_d == TRUE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Ext_ERCLK32K)
    
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif
      
        /* disable transceiver CLK_OUT. */
        MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE);
      
        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
        
        #if gPWR_EnsureOscStabilized_d
          /* start 32KHz OSC */
          while(PWRLib_RTC_IsOscStarted() == FALSE){}
        #endif
      
        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cPWR_LPTMRTickTime, DozeDuration);

        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO

        /* configure MCU in LLS low power mode */
        PWRLib_MCU_Enter_LLS();

        /* checks sources of wakeup */
        /* low power timer wakeup */
        if(PWRLib_LLWU_IsLPTMRWakeUpSource() == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromLPTMR = 1;
        }
        
        /* GPIO wakeup */
        if(PWRLib_LLWU_IsGPIOWakeUpSource(gLLWU_WakeUp_PIN_Number_c) == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1;
        }
        
        /* stop LPTMR */
        PWRLib_LPTMR_ClockStop();
        
        /* configure Radio in autodoze mode */
        PWRLib_Radio_Enter_AutoDoze();
        
        #if defined(gMCG_FEE_Mode_d)
          MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_32_78_KHz);
        #else
          MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_4_MHz);
        #endif //#if defined(gMCG_FEE_Mode_d)

        /* configure MCG in PEE/FEE mode*/
        #if defined(gMCG_FEE_Mode_d)
          /* FEE @ 48MHz */
          gMCG_coreClkMHz = MCG_Fei2Fee();
        #else //#if defined(gMCG_FEE_Mode_d)
          /* PEE @ 48MHz */
          gMCG_coreClkMHz = MCG_PLLInit();
        #endif //#if defined(gMCG_FEE_Mode_d)
        
        /* Sync. the low power timers */
        #if (gTMR_EnableLowPowerTimers_d)
        {
          uint32_t deepSleepDurationMs;

          /* Converts the DozeDuration from PWRLib timer ticks in ms */
          #if(cPWR_LPTMRTickTime == cLPTMR_PRS_125_div_by_4096ms)
            deepSleepDurationMs = ( ( DozeDuration - PWRLib_LPTMR_ClockCheck() ) * 125 ) >> 12;
          #elif(cPWR_LPTMRTickTime < cLPTMR_PRS_0125ms)
            deepSleepDurationMs = ( ( DozeDuration - PWRLib_LPTMR_ClockCheck() ) * 125 ) >> (11 - (cPWR_LPTMRTickTime >> 3 ) ) ;
          #else
            deepSleepDurationMs = ( ( DozeDuration - PWRLib_LPTMR_ClockCheck() ) * 125 ) << ( ( cPWR_LPTMRTickTime >> 3 ) - 11 );
          #endif
          
          /* Converts the DozeDuration from ms in software timer ticks and synchronize low power timers */
          TMR_SyncLpmTimers( (TmrTicksFromMilliseconds( ( tmrTimeInMilliseconds_t) deepSleepDurationMs) + notCountedTicksBeforeSleep) );
          
        }
        #endif
        
                 
        if( PWRLib_LPTMR_ClockCheck() == 0)
        {
          PWRLib_MCU_WakeupReason.Bits.DeepSleepTimeout = 1;
          cPWR_DeepSleepWakeupStackProc; // User function called only on timeout
        }
        Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;

      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Ext_ERCLK32K"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gKeyBoardSupported_d) else */
      #error "*** ERROR: gKeyBoardSupported_d has to be set to TRUE"
    #endif /* #if (gKeyBoardSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 11)
    #if (gKeyBoardSupported_d == TRUE)
        
      /* configure MCG in FLL Engaged Internal (FEI) mode */
      #if defined(gMCG_FEE_Mode_d)
        MCG_Fee2Fei();
      #else //#if defined(gMCG_FEE_Mode_d)
        MCG_Pee2Fei();
      #endif
      
      /* disable transceiver CLK_OUT. */
      MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE);
      
      /* configure Radio in hibernate mode */
      PWRLib_Radio_Enter_Hibernate();
              
      #if gPWR_EnsureOscStabilized_d   
        /* start RTC */
        while(PWRLib_RTC_IsOscStarted() == FALSE){}
      #endif
      
      PWRLib_RTC_ClockStart(DozeDuration);

      PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO
     
      /* configure MCU in LLS low power mode */
      PWRLib_MCU_Enter_LLS();

      /* checks sources of wakeup */
      /* real time counter wakeup */
      if(PWRLib_LLWU_IsRTCWakeUpSource() == TRUE)
      {
        PWRLib_MCU_WakeupReason.Bits.FromRTC = 1;
      }
        
      /* GPIO wakeup */
      if(PWRLib_LLWU_IsGPIOWakeUpSource(gLLWU_WakeUp_PIN_Number_c) == TRUE)
      {
        PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1;
      }

      /* stop RTC */
      PWRLib_RTC_ClockStop();
        
      /* configure Radio in autodoze mode */
      PWRLib_Radio_Enter_AutoDoze();
      
      #if defined(gMCG_FEE_Mode_d)
        MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_32_78_KHz);
      #else
        MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_4_MHz);
      #endif //#if defined(gMCG_FEE_Mode_d)

      /* configure MCG in PEE/FEE mode*/
      #if defined(gMCG_FEE_Mode_d)
        /* FEE @ 48MHz */
        gMCG_coreClkMHz = MCG_Fei2Fee();
      #else //#if defined(gMCG_FEE_Mode_d)
        /* PEE @ 48MHz */
        gMCG_coreClkMHz = MCG_PLLInit();
      #endif //#if defined(gMCG_FEE_Mode_d)
        
      /* Sync. the low power timers */
      #if (gTMR_EnableLowPowerTimers_d)
      {
        uint32_t deepSleepDurationMs;
        /* Converts the DozeDuration from PWRLib timer ticks in ms */
        deepSleepDurationMs = ( ( DozeDuration - PWRLib_RTC_ClockCheck() ) * 1000 );
        /* Converts the DozeDuration from ms in software timer ticks and synchronize low power timers */
        TMR_SyncLpmTimers( (TmrTicksFromMilliseconds( ( tmrTimeInMilliseconds_t) deepSleepDurationMs) + notCountedTicksBeforeSleep) );
      }
      #endif
      
                 
      if( PWRLib_RTC_ClockCheck() == 0)
      {
        PWRLib_MCU_WakeupReason.Bits.DeepSleepTimeout = 1;
        cPWR_DeepSleepWakeupStackProc; // User function called only on timeout
      }
      Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
        
    #else /* #if (gKeyBoardSupported_d) else */
      #error "*** ERROR: gKeyBoardSupported_d has to be set to TRUE"
    #endif /* #if (gKeyBoardSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 12)
    #if (gKeyBoardSupported_d == TRUE)
      #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz)
      
        /* converts deep sleep duration from ms/symbols in LPTMR ticks */
        if(!mPWR_DeepSleepTimeInSymbols)
        {
          deepSleepTicks = ( mPWR_DeepSleepTime >> ( ( cLPTMR_PRS_00002ms >> 3 ) + 1) );
        }
        else
        {
          deepSleepTicks = ( ( mPWR_DeepSleepTime / 125 ) >> ( cLPTMR_PRS_00002ms >> 3 ) );
        }
    
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif

        /* disable transceiver CLK_OUT. */
        MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE);
      
        /* configure Radio in hibernate mode */
        PWRLib_Radio_Enter_Hibernate();
      
        /* start LPTMR */
        PWRLib_LPTMR_ClockStart(cLPTMR_PRS_00002ms, deepSleepTicks);

        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO
        
        /* configure MCU in LLS low power mode */
        PWRLib_MCU_Enter_LLS();

        /* checks sources of wakeup */
        /* low power timer wakeup */
        if(PWRLib_LLWU_IsLPTMRWakeUpSource() == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromLPTMR = 1;
        }
        
        /* GPIO wakeup */
        if(PWRLib_LLWU_IsGPIOWakeUpSource(gLLWU_WakeUp_PIN_Number_c) == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1;
        }

        /* stop LPTMR */
        PWRLib_LPTMR_ClockStop();

        /* configure Radio in autodoze mode */
        PWRLib_Radio_Enter_AutoDoze();
        
        #if defined(gMCG_FEE_Mode_d)
          MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_32_78_KHz);
        #else
          MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_4_MHz);
        #endif //#if defined(gMCG_FEE_Mode_d)

        /* configure MCG in PEE/FEE mode*/
        #if defined(gMCG_FEE_Mode_d)
          /* FEE @ 48MHz */
          gMCG_coreClkMHz = MCG_Fei2Fee();
        #else //#if defined(gMCG_FEE_Mode_d)
          /* PEE @ 48MHz */
          gMCG_coreClkMHz = MCG_PLLInit();
        #endif //#if defined(gMCG_FEE_Mode_d)
          
        /* Sync. the low power timers */
        #if (gTMR_EnableLowPowerTimers_d)
        {
          uint32_t deepSleepDurationMs;
          /* Converts the DozeDuration from PWRLib timer ticks in ms */
          deepSleepDurationMs = ( ( deepSleepTicks - PWRLib_LPTMR_ClockCheck() ) * 2 );
          /* Converts the DozeDuration from ms in software timer ticks and synchronize low power timers */
          TMR_SyncLpmTimers( (TmrTicksFromMilliseconds( ( tmrTimeInMilliseconds_t) deepSleepDurationMs) + notCountedTicksBeforeSleep) );
        }
        #endif
        
                 
        if( PWRLib_LPTMR_ClockCheck() == 0)
        {
          PWRLib_MCU_WakeupReason.Bits.DeepSleepTimeout = 1;
          cPWR_DeepSleepWakeupStackProc; // User function called only on timeout
        }
        Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
        
      #else /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) else */
        #error  "*** ERROR: cPWR_LPTMRClockSource has to be set to cLPTMR_Source_Int_LPO_1KHz"
      #endif /* #if (cPWR_LPTMRClockSource == cLPTMR_Source_Int_LPO_1KHz) */
    #else /* #if (gKeyBoardSupported_d) else */
      #error "*** ERROR: gKeyBoardSupported_d has to be set to TRUE"
    #endif /* #if (gKeyBoardSupported_d) */
  /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 13)
    #if (gKeyBoardSupported_d == TRUE)
    {
      #if ( (gUart1_Enabled_d == TRUE) || (gUart2_Enabled_d == TRUE) )
      {
      zbClock24_t currentTime;
      zbClock24_t absoluteWakeUpTime;
      #if (gTMR_EnableLowPowerTimers_d)
      uint32_t    deepSleepDurationMs;
      #endif
      
      /* converts deep sleep duration from ms to symbols */
      if(!mPWR_DeepSleepTimeInSymbols)
      {
        deepSleepTicks = ( ( ( mPWR_DeepSleepTime / 2 ) * 125 ) + ( ( mPWR_DeepSleepTime & 1 ) * 62 ) ) & 0xFFFFFF;
      }
      else
      {
        deepSleepTicks = mPWR_DeepSleepTime;
      }
      
      if( deepSleepTicks > PWR_MINIMUM_SLEEP_TICKS )
      {
        // Set prescaller to obtain 1 symbol (16us) timebase TODO
        MC1324xDrv_IndirectAccessSPIWrite(TMR_PRESCALE, 0x05);
		
        /* configure MCG in FLL Engaged Internal (FEI) mode */
        #if defined(gMCG_FEE_Mode_d)
          MCG_Fee2Fei();
        #else //#if defined(gMCG_FEE_Mode_d)
          MCG_Pee2Fei();
        #endif

        /* disable transceiver CLK_OUT. */
        MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE);
        
        /* configure Radio in Doze mode */
        PWRLib_Radio_Enter_Doze();
        
        /* prepare UART for low power operation */
        Uart_ModuleEnableLowPowerWakeup();
        
        /* read current time */
        PhyTimeReadClock(&currentTime);
        
        /* compute absolute end time */
        absoluteWakeUpTime = (zbClock24_t)((currentTime + deepSleepTicks) & 0xFFFFFF);

        /* set absolute wakeup time */
        PhyTimeSetWakeUpTime(&absoluteWakeUpTime);

        PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; //TODO 
        
        /* configure MCU in LLS low power mode */
        PWRLib_MCU_Enter_VLPS();
        
        PhyTimeReadClock(&absoluteWakeUpTime);
        /* checks sources of wakeup */
        /* radio timer wakeup */
        if( PhyTimeIsWakeUpTimeExpired() == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.DeepSleepTimeout = 1;     /* Sleep timeout ran out */
          PWRLib_MCU_WakeupReason.Bits.FromTimer = 1;            /* Wakeup by radio timer */
        }
        
        #if (gTMR_EnableLowPowerTimers_d)
        if (absoluteWakeUpTime >= (zbClock24_t)((currentTime + deepSleepTicks) & 0xFFFFFF) )
        {
          deepSleepDurationMs = ( ( deepSleepTicks * 10 ) / 625 );
        }
        else
        {
          deepSleepDurationMs = ( ( ((absoluteWakeUpTime - currentTime) & 0xFFFFFF) * 10 ) / 625 );
        }
        #endif
        
        /* TSI module wakeup */
        if( KBD_IsWakeUpSource() == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1;
        }
        
        /* UART module wakeup */
        if( Uart_IsWakeUpSource() == TRUE)
        {
          PWRLib_MCU_WakeupReason.Bits.FromUART = 1;
        }

        /* configure Radio in autodoze mode */
        PWRLib_Radio_Enter_AutoDoze();

        #if defined(gMCG_FEE_Mode_d)
          MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_32_78_KHz);
        #else
          MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_4_MHz);
        #endif //#if defined(gMCG_FEE_Mode_d)

        /* configure MCG in PEE/FEE mode*/
        #if defined(gMCG_FEE_Mode_d)
          /* FEE @ 48MHz */
          gMCG_coreClkMHz = MCG_Fei2Fee();
        #else //#if defined(gMCG_FEE_Mode_d)
          /* PEE @ 48MHz */
          gMCG_coreClkMHz = MCG_PLLInit();
        #endif //#if defined(gMCG_FEE_Mode_d)
        
        /* Sync. the low power timers */
        #if (gTMR_EnableLowPowerTimers_d)
          /* Converts the DozeDuration from ms in software timer ticks and synchronize low power timers */
          TMR_SyncLpmTimers( (TmrTicksFromMilliseconds( ( tmrTimeInMilliseconds_t) deepSleepDurationMs) + notCountedTicksBeforeSleep) );
        #endif
        
        /* prepare UART for normal operation */
        Uart_ModuleDisableLowPowerWakeup();
      }
      else
      {
        /* Not enough time to program the TRM compare */
        PWRLib_MCU_WakeupReason.Bits.DeepSleepTimeout = 1;     /* Sleep timeout ran out */
      }
      if(Res.Bits.DeepSleepTimeout == 1) 
      {
        cPWR_DeepSleepWakeupStackProc; // User function called only on timeout
      }
      Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
      
      }
      #else //#if ( (gUart1_Enabled_d == TRUE) || (gUart2_Enabled_d == TRUE) )
        #error "*** ERROR: gUart1_Enabled_d or gUart2_Enabled_d has to be set to TRUE"
      #endif    
    }
    #else /* #if (gTsiSupported_d) else */
      #error "*** ERROR: gKeyBoardSupported_d has to be set to TRUE"
    #endif /* #if (gTsiSupported_d) */
    /*---------------------------------------------------------------------------*/
  #elif (cPWR_DeepSleepMode == 14)
    #if ((gKeyBoardSupported_d == TRUE) && (gTMR_Enabled_d == TRUE))
      	
    /* configure MCG in FLL Engaged Internal (FEI) mode */
    #if defined(gMCG_FEE_Mode_d)
      MCG_Fee2Fei();
    #else //#if defined(gMCG_FEE_Mode_d)
      MCG_Pee2Fei();
    #endif

    /* disable transceiver CLK_OUT. */
    MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_DISABLE);

    /* configure Radio in hibernate mode */
    PWRLib_Radio_Enter_Hibernate();
    
    #if gPWR_EnsureOscStabilized_d
      /* start 32KHz OSC */
      while(PWRLib_RTC_IsOscStarted() == FALSE){}
    #endif

    PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

    /* configure MCU in LLS low power mode */
    PWRLib_MCU_Enter_LLS();

    /* checks sources of wakeup */
    /* low power timer wakeup */
    if(PWRLib_LLWU_IsLPTMRWakeUpSource() == TRUE)
    {
      PWRLib_MCU_WakeupReason.Bits.FromLPTMR = 1;
      PWRLib_MCU_WakeupReason.Bits.DeepSleepTimeout = 1;
    }
    
    /* GPIO wakeup */
    if(PWRLib_LLWU_IsGPIOWakeUpSource(gLLWU_WakeUp_PIN_Number_c) == TRUE)
    {
      PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1;
    }
                    
    /* configure Radio in autodoze mode */
    PWRLib_Radio_Enter_AutoDoze();
    
    #if defined(gMCG_FEE_Mode_d)
      MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_32_78_KHz);
    #else
      MC1324xDrv_Set_CLK_OUT_Freq(gCLK_OUT_FREQ_4_MHz);
    #endif //#if defined(gMCG_FEE_Mode_d)

    /* configure MCG in PEE/FEE mode*/
    #if defined(gMCG_FEE_Mode_d)
      /* FEE @ 48MHz */
      gMCG_coreClkMHz = MCG_Fei2Fee();
    #else //#if defined(gMCG_FEE_Mode_d)
      /* PEE @ 48MHz */
      gMCG_coreClkMHz = MCG_PLLInit();
    #endif //#if defined(gMCG_FEE_Mode_d)
                    
    Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;      
	#else /* #if (gKeyBoardSupported_d) else */
	  #error "*** ERROR: gKeyBoardSupported_d and gTMR_Enabled_d has to be set to TRUE"
	#endif /* #if (gKeyBoardSupported_d) */    
  /*---------------------------------------------------------------------------*/
  #else
    #error "*** ERROR: Not a valid cPWR_DeepSleepMode chosen"
  #endif
  
  return Res;
}
#else
PWRLib_WakeupReason_t PWR_HandleDeepSleep
(
  zbClock24_t DozeDuration
)
{
  PWRLib_WakeupReason_t  Res;
  (void) DozeDuration;
  Res.AllBits = 0;
  return Res;
}
#endif

/*---------------------------------------------------------------------------
 * Name: PWR_HandleSleep
 * Description: - 
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
static PWRLib_WakeupReason_t PWR_HandleSleep
(
  zbClock24_t DozeDuration
)
{
  PWRLib_WakeupReason_t  Res;
  
  Res.AllBits = 0;
  (void) DozeDuration;

  /*---------------------------------------------------------------------------*/
  #if (cPWR_SleepMode==0)
    Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
    PWRLib_MCU_WakeupReason.AllBits = 0;
    return Res;

  /*---------------------------------------------------------------------------*/
  #elif (cPWR_SleepMode==1)
    Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
    /* radio in autodoze mode by default. mcu in wait mode */
    PWRLib_MCU_Enter_WAIT();
    Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
    Res.Bits.SleepTimeout = 1;
    PWRLib_MCU_WakeupReason.AllBits = 0;
    return Res;
  /*---------------------------------------------------------------------------*/
  #else
    #error "*** ERROR: Not a valid cPWR_SleepMode chosen"
  #endif
}

/*---------------------------------------------------------------------------
 * Name: PWR_SleepAllowed
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
static bool_t PWR_SleepAllowed
(
  void
)
{
  #if (cPWR_UsePowerModuleStandAlone == 1)
    return TRUE;
  #else  
  if((PWRLib_GetCurrentZigbeeStackPowerState == StackPS_Sleep) ||  \
    (PWRLib_GetCurrentZigbeeStackPowerState == StackPS_DeepSleep) )
  {
    if((PWRLib_GetMacStateReq()== gAspMacStateNotEmpty_c) || \
      (PWRLib_GetMacStateReq()== gAspMacStateIdle_c) )
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
    return FALSE;
  }
  #endif //#if (cPWR_UsePowerModuleStandAlone)
}

/*---------------------------------------------------------------------------
 * Name: PWR_DeepSleepAllowed
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
static bool_t PWR_DeepSleepAllowed
(
  void
)
{
  #if (cPWR_UsePowerModuleStandAlone == 1)
    return TRUE;
  #else
  if (PWRLib_GetCurrentZigbeeStackPowerState == StackPS_DeepSleep) {
  #if (cPWR_DeepSleepMode != 13)
    /* DeepSleepMode 13 allows the radio to be active during low power */
    if ( PWRLib_GetMacStateReq() == gAspMacStateIdle_c)
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  #else /* #if (cPWR_DeepSleepMode != 13) */
    return TRUE;
  #endif /* #if (cPWR_DeepSleepMode != 13) */
  }
  else
  {
    return FALSE;
  }
  #endif //#if (cPWR_UsePowerModuleStandAlone)
}

#endif /* #if (cPWR_UsePowerDownMode) */

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
 * Name: PWR_CheckForAndEnterNewPowerState_Init
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_CheckForAndEnterNewPowerState_Init
(
  void
)
{
#if (cPWR_UsePowerDownMode)

  PWRLib_Init();

#endif  /* #if (cPWR_UsePowerDownMode) */
}

/*---------------------------------------------------------------------------
 * Name: PWR_SetDeepSleepTimeInMs
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_SetDeepSleepTimeInMs
(
  uint32_t deepSleepTimeTimeMs
)
{
#if (cPWR_UsePowerDownMode)
 if(deepSleepTimeTimeMs == 0) 
 {
  return;
 }
 mPWR_DeepSleepTime = deepSleepTimeTimeMs;
 mPWR_DeepSleepTimeInSymbols = FALSE;
#else
 (void) deepSleepTimeTimeMs;
#endif
}

/*---------------------------------------------------------------------------
 * Name: PWR_SetDeepSleepTimeInSymbols
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_SetDeepSleepTimeInSymbols
(
  uint32_t deepSleepTimeTimeSym
)
{
#if (cPWR_UsePowerDownMode)
 if(deepSleepTimeTimeSym == 0) 
 {
  return;
 }
 mPWR_DeepSleepTime = deepSleepTimeTimeSym;
 mPWR_DeepSleepTimeInSymbols = TRUE;
#else
 (void) deepSleepTimeTimeSym;
#endif 
}

/*---------------------------------------------------------------------------
 * Name: PWR_AllowDeviceToSleep
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_AllowDeviceToSleep
(
  void
)
{
  uint32_t irqMaskRegister;
  irqMaskRegister = IntDisableAll();
  
  if( mLPMFlag != 0 ){    
    mLPMFlag--;
  }
  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PWR_DisallowDeviceToSleep
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_DisallowDeviceToSleep
(
  void
)
{
  uint32_t irqMaskRegister;
  irqMaskRegister = IntDisableAll();
  mLPMFlag++;
  IntRestoreAll(irqMaskRegister);
}

/*---------------------------------------------------------------------------
 * Name: PWR_CheckIfDeviceCanGoToSleep
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PWR_CheckIfDeviceCanGoToSleep
(
  void
)
{
  uint32_t irqMaskRegister;
  bool_t   returnValue;
  irqMaskRegister = IntDisableAll();
  returnValue = mLPMFlag == 0 ? TRUE : FALSE;
  IntRestoreAll(irqMaskRegister);
  return returnValue;
}




/*---------------------------------------------------------------------------
 * Name: PWR_CheckForAndEnterNewPowerState
 * Description: - 
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
PWRLib_WakeupReason_t PWR_CheckForAndEnterNewPowerState
(
  PWR_CheckForAndEnterNewPowerState_t NewPowerState,
  zbClock24_t DozeDuration
)
{
  PWRLib_WakeupReason_t ReturnValue;
  ReturnValue.AllBits = 0;
  
#if (cPWR_UsePowerDownMode)
  if ( NewPowerState == PWR_Run)
  {
    /* ReturnValue = 0; */
  }
  else if( NewPowerState == PWR_OFF)
  {
    /* configure MCG in FLL Engaged Internal (FEI) mode */
#if defined(gMCG_FEE_Mode_d)
    MCG_Fee2Fei();
#else //#if defined(gMCG_FEE_Mode_d)
    MCG_Pee2Fei();
#endif
    /* puts radio RESET */
    MC1324xDrv_RST_B_Assert();
    
    /* configure MCU in VLLS1 mode */
    PWRLib_MCU_Enter_VLLS1();
    /* Never returns */
    for(;;){}
    
  }
  else if( NewPowerState == PWR_Reset)
  {
    /* Never returns */
    PWRLib_Reset();
  }
  
  else if(( NewPowerState == PWR_DeepSleep) && PWR_DeepSleepAllowed())
  {
    ReturnValue = PWR_HandleDeepSleep(DozeDuration);
  } 
  else if(( NewPowerState == PWR_Sleep) && PWR_SleepAllowed())
  {
    ReturnValue = PWR_HandleSleep(DozeDuration);
  }
  else
  {
    /* ReturnValue = FALSE; */
  }
  /* Clear wakeup reason */
  
#else
    /* To remove warning for variabels in functioncall */
  ReturnValue.AllBits = 0xff | (NewPowerState > 0) | (DozeDuration > 0); 
#endif  /* #if (cPWR_UsePowerDownMode) */
  
  return ReturnValue;
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LVD_ReportLevel
 * Description: - 
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
PWRLib_LVD_VoltageLevel_t PWRLib_LVD_ReportLevel
(
  void
)
{
  PWRLib_LVD_VoltageLevel_t   Level;
#if ((cPWR_LVD_Enable == 0) || (cPWR_LVD_Enable == 3))
  Level = PWR_ABOVE_LEVEL_3_0V;
#elif (cPWR_LVD_Enable==1)
  Level = PWRLib_LVD_CollectLevel();
#elif (cPWR_LVD_Enable==2)
  Level = PWRLib_LVD_SavedLevel;
#else
#error "*** ERROR: Illegal value for cPWR_LVD_Enable"
#endif /* #if (cPWR_LVD_Enable) */
  return Level;
}

/*---------------------------------------------------------------------------
 * Name: PWR_EnterLowPower
 * Description: - 
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
PWRLib_WakeupReason_t PWR_EnterLowPower
(
  void
)
{
  
  uint32_t irqMaskRegister;
  PWRLib_WakeupReason_t ReturnValue;  
  ReturnValue.AllBits = 0;
  
  if (PWRLib_LVD_ReportLevel() == PWR_LEVEL_CRITICAL)
  {
    /* Voltage <= 1.8V so enter power-off state - to disable false Tx'ing(void)*/
    ReturnValue = PWR_CheckForAndEnterNewPowerState( PWR_OFF, 0);
  }
  irqMaskRegister = IntDisableAll();
  if (TS_PendingEvents() == FALSE)
  {
#if (cPWR_UsePowerModuleStandAlone == 0)
    PWRLib_SetCurrentZigbeeStackPowerState(StackPS_DeepSleep);
#endif
    if (TMR_AreAllTimersOff())  /*No timer running*/
    {
      /* if power lib is enabled */	
#if (cPWR_UsePowerDownMode)
      /* if Low Power Capability is enabled */
#if (gTMR_EnableLowPowerTimers_d) 
      /* if more low power timers are running, stop the hardware timer
      and save the spend time in ticks that wasn't counted.
      */
      notCountedTicksBeforeSleep = TMR_NotCountedTicksBeforeSleep();
#endif /* #if (gTMR_EnableLowPowerTimers_d)  */
#endif /* #if (cPWR_UsePowerDownMode)  */
      
      ReturnValue = PWR_CheckForAndEnterNewPowerState (PWR_DeepSleep, cPWR_TMRTicks);
    }
    else /*timers are running*/
    { 	 
      ReturnValue = PWR_CheckForAndEnterNewPowerState (PWR_Sleep, 0);
    }
  }
  /* enable irq's if there is pending evens */
  IntRestoreAll(irqMaskRegister);
  
  return ReturnValue;
}
