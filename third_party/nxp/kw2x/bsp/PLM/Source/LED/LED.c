/******************************************************************************
* Filename: LED.c
*
* Description: LED implementation file for ARM CORTEX-M4 processor
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

#include "Led.h"
#include "TMR_Interface.h"
#include "GPIO_Interface.h"
#include "FunctionLib.h"

/******************************************************************************
 *******************************************************************************
 * Private macros
 *******************************************************************************
 ******************************************************************************/

/******************************************************************************
 *******************************************************************************
 * Private type definitions
 *******************************************************************************
 ******************************************************************************/

/******************************************************************************
 *******************************************************************************
 * Public memory definitions
 *******************************************************************************
 ******************************************************************************/

#if (gLEDSupported_d == TRUE)

/******************************************************************************
 *******************************************************************************
 * Private memory definitions
 *******************************************************************************
 ******************************************************************************/

/******************************************************************************
 *******************************************************************************
 * Private function prototypes
 *******************************************************************************
 ******************************************************************************/

/******************************************************************************
 * Name: LED_FlashTimeout
 * Description: timer callback function that is called each time the timer 
 *              expires
 * Param(s): [IN] timerId - the timer ID
 * Return: -
 ******************************************************************************/
#if gTMR_Enabled_d
static void LED_FlashTimeout
(
		uint8_t timerId
);
#endif

/******************************************************************************
 * Name: LED_ExitSerialFlash
 * Description: Stop serial flashing mode, if serial flash mode is active. 
 *              Turns all LEDs off.
 * Param(s): -
 * Return: -
 ******************************************************************************/
static void LED_ExitSerialFlash
(
		void
);

/******************************************************************************
 * Name: LED_DecrementBlip
 * Description: Decrements the blink count
 * Param(s): -
 * Return: -
 ******************************************************************************/
#if gLEDBlipEnabled_d
static void LED_DecrementBlip
(
		void
);
#endif

/******************************************************************************
 *******************************************************************************
 * Private type definitions
 *******************************************************************************
 ******************************************************************************/

/******************************************************************************
 *******************************************************************************
 * Private Memory Declarations
 *******************************************************************************
 ******************************************************************************/

/*
 * Name: mfLedInSerialMode
 * Description: indicates if the LED module is in serial mode or not
 */
static bool_t mfLedInSerialMode;      /* doing serial lights */

/*
 * Name: mLedFlashingLEDs
 * Description: indicates how many LEDs are in flashing mode 
 */
static uint8_t mLedFlashingLEDs = 0;  /* flashing LEDs */

/*
 * Name: mLEDTimerID
 * Description: LED timer ID
 */
#if gTMR_Enabled_d   
static tmrTimerID_t mLEDTimerID = gTmrInvalidTimerID_c;
#endif

/*
 * Name: mLedBlips
 * Description: array that stores the LEDs blip information
 */		
#if gLEDBlipEnabled_d
static uint8_t mLedBlips[gLEDsOnTargetBoardCnt_c];
#endif


/******************************************************************************
 ******************************************************************************
 * Public functions
 ******************************************************************************
 *****************************************************************************/

/******************************************************************************
 * Name: LED_Init
 * Description: Initialize the LED module
 * Parameters: -
 * Return: -
 ******************************************************************************/
void LED_Init
(
		void
)
{
  
#if (gTarget_UserDefined_d == 0) // no user defined target
  
#if gLEDsOnTargetBoardCnt_c > 0  
  GpioPinConfigAttr_t Led1PinAttr;
#endif
#if gLEDsOnTargetBoardCnt_c > 1
  GpioPinConfigAttr_t Led2PinAttr;
#endif
#if gLEDsOnTargetBoardCnt_c > 2
  GpioPinConfigAttr_t Led3PinAttr;
#endif  
#if gLEDsOnTargetBoardCnt_c > 3
  GpioPinConfigAttr_t Led4PinAttr;
#endif  
      
#if gLEDsOnTargetBoardCnt_c > 0
	/* enable port clock */
        GPIO_EnablePortClockGating((GpioPortID_t)gLED1_PORT_c);
        /* pin MUX configuration */
        FLib_MemSet(&Led1PinAttr, 0, sizeof(Led1PinAttr));
        Led1PinAttr.muxCtrl = gGpioAlternate1_c;
        GPIO_SetPinPortAttr((GpioPortID_t)gLED1_PORT_c, (GpioPin_t)gLED1_PIN_c, &Led1PinAttr);
        /* pin direction -> output */
        GPIO_SetPinDir((GpioPortID_t)gLED1_PORT_c, (GpioPin_t)gLED1_PIN_c, gGpioDirOut_c);
#endif
#if gLEDsOnTargetBoardCnt_c > 1
        /* enable port clock */
        GPIO_EnablePortClockGating((GpioPortID_t)gLED2_PORT_c);
        /* pin MUX configuration */
        FLib_MemSet(&Led2PinAttr, 0, sizeof(Led2PinAttr));
        Led2PinAttr.muxCtrl = gGpioAlternate1_c;
        GPIO_SetPinPortAttr((GpioPortID_t)gLED2_PORT_c, (GpioPin_t)gLED2_PIN_c, &Led2PinAttr);
        /* pin direction -> output */
        GPIO_SetPinDir((GpioPortID_t)gLED2_PORT_c, (GpioPin_t)gLED2_PIN_c, gGpioDirOut_c);
#endif        
#if gLEDsOnTargetBoardCnt_c > 2
        /* enable port clock */
        GPIO_EnablePortClockGating((GpioPortID_t)gLED3_PORT_c);
        /* pin MUX configuration */
        FLib_MemSet(&Led3PinAttr, 0, sizeof(Led3PinAttr));
        Led3PinAttr.muxCtrl = gGpioAlternate1_c;
        GPIO_SetPinPortAttr((GpioPortID_t)gLED3_PORT_c, (GpioPin_t)gLED3_PIN_c, &Led3PinAttr);
        /* pin direction -> output */
        GPIO_SetPinDir((GpioPortID_t)gLED3_PORT_c, (GpioPin_t)gLED3_PIN_c, gGpioDirOut_c);
#endif  
#if gLEDsOnTargetBoardCnt_c > 3
        /* enable port clock */
        GPIO_EnablePortClockGating((GpioPortID_t)gLED4_PORT_c);
        /* pin MUX configuration */
        FLib_MemSet(&Led4PinAttr, 0, sizeof(Led4PinAttr));
        Led4PinAttr.muxCtrl = gGpioAlternate1_c;
        GPIO_SetPinPortAttr((GpioPortID_t)gLED4_PORT_c, (GpioPin_t)gLED4_PIN_c, &Led4PinAttr);
        /* pin direction -> output */
        GPIO_SetPinDir((GpioPortID_t)gLED4_PORT_c, (GpioPin_t)gLED4_PIN_c, gGpioDirOut_c);
#endif  

        /* turn off all LEDs */

#if ((gTargetTWR_K60D100M_d == 1) || (gTargetTWR_K60N512_d == 1)  || \
     (gTargetTWR_KW21D512_d == 1) || (gTargetTWR_KW21D256_d == 1) || \
     (gTargetTWR_KW22D512_d == 1) || (gTargetTWR_KW24D512_d == 1) || \
     (gTargetKW24D512_USB_d == 1) )
	#if gLEDsOnTargetBoardCnt_c > 0        
        GPIO_SetPinData((GpioPortID_t)gLED1_PORT_c, (GpioPin_t)gLED1_PIN_c, gGpioPinHigh_c);
        #endif
        #if gLEDsOnTargetBoardCnt_c > 1        
        GPIO_SetPinData((GpioPortID_t)gLED2_PORT_c, (GpioPin_t)gLED2_PIN_c, gGpioPinHigh_c);
        #endif
        #if gLEDsOnTargetBoardCnt_c > 2        
        GPIO_SetPinData((GpioPortID_t)gLED3_PORT_c, (GpioPin_t)gLED3_PIN_c, gGpioPinHigh_c);
        #endif
        #if gLEDsOnTargetBoardCnt_c > 3        
        GPIO_SetPinData((GpioPortID_t)gLED4_PORT_c, (GpioPin_t)gLED4_PIN_c, gGpioPinHigh_c);
        #endif
#elif (gTargetTWR_K20D50M_d == 1)
        /* turn off all LEDs */                
        #if gLEDsOnTargetBoardCnt_c > 0        
        GPIO_SetPinData((GpioPortID_t)gLED1_PORT_c, (GpioPin_t)gLED1_PIN_c, gGpioPinHigh_c);
        #endif
        #if gLEDsOnTargetBoardCnt_c > 1        
        GPIO_SetPinData((GpioPortID_t)gLED2_PORT_c, (GpioPin_t)gLED2_PIN_c, gGpioPinHigh_c);
        #endif
        #if gLEDsOnTargetBoardCnt_c > 2       
        GPIO_SetPinData((GpioPortID_t)gLED3_PORT_c, (GpioPin_t)gLED3_PIN_c, gGpioPinLow_c);
        #endif
        #if gLEDsOnTargetBoardCnt_c > 3        
        GPIO_SetPinData((GpioPortID_t)gLED4_PORT_c, (GpioPin_t)gLED4_PIN_c, gGpioPinLow_c);
        #endif       
#endif
        
#endif /* gTarget_UserDefined_d == 0 */
        
	/* allocate a timer for use in flashing LEDs */
#if gTMR_Enabled_d
	mLEDTimerID = TMR_AllocateTimer();
#endif        
}

/******************************************************************************
 * Name: Un-initialize the LED module
 * Description: turns off all the LEDs and disables clock gating for LED port
 * Parameters: -
 * Return: -
 ******************************************************************************/
void LED_UnInit
(
		void
)
{
  /* turn off all LEDs */
  
#if (gTarget_UserDefined_d == 0) // no user defined target  

#if ((gTargetTWR_K60D100M_d == 1) || (gTargetTWR_K60N512_d == 1)  || \
     (gTargetTWR_KW21D512_d == 1) || (gTargetTWR_KW21D256_d == 1) || \
     (gTargetTWR_KW22D512_d == 1) || (gTargetTWR_KW24D512_d == 1) || \
     (gTargetKW24D512_USB_d == 1) )
	#if gLEDsOnTargetBoardCnt_c > 0        
        GPIO_SetPinData((GpioPortID_t)gLED1_PORT_c, (GpioPin_t)gLED1_PIN_c, gGpioPinHigh_c);
        #endif
        #if gLEDsOnTargetBoardCnt_c > 1        
        GPIO_SetPinData((GpioPortID_t)gLED2_PORT_c, (GpioPin_t)gLED2_PIN_c, gGpioPinHigh_c);
        #endif
        #if gLEDsOnTargetBoardCnt_c > 2        
        GPIO_SetPinData((GpioPortID_t)gLED3_PORT_c, (GpioPin_t)gLED3_PIN_c, gGpioPinHigh_c);
        #endif
        #if gLEDsOnTargetBoardCnt_c > 3        
        GPIO_SetPinData((GpioPortID_t)gLED4_PORT_c, (GpioPin_t)gLED4_PIN_c, gGpioPinHigh_c);
        #endif
#elif (gTargetTWR_K20D50M_d == 1)
        /* turn off all LEDs */                
        #if gLEDsOnTargetBoardCnt_c > 0        
        GPIO_SetPinData((GpioPortID_t)gLED1_PORT_c, (GpioPin_t)gLED1_PIN_c, gGpioPinHigh_c);
        #endif
        #if gLEDsOnTargetBoardCnt_c > 1        
        GPIO_SetPinData((GpioPortID_t)gLED2_PORT_c, (GpioPin_t)gLED2_PIN_c, gGpioPinHigh_c);
        #endif
        #if gLEDsOnTargetBoardCnt_c > 2       
        GPIO_SetPinData((GpioPortID_t)gLED3_PORT_c, (GpioPin_t)gLED3_PIN_c, gGpioPinLow_c);
        #endif
        #if gLEDsOnTargetBoardCnt_c > 3        
        GPIO_SetPinData((GpioPortID_t)gLED4_PORT_c, (GpioPin_t)gLED4_PIN_c, gGpioPinLow_c);
        #endif       
#endif       
  	
#if gLEDsOnTargetBoardCnt_c > 0
	/* disable port clock gating */
        GPIO_DisablePortClockGating((GpioPortID_t)gLED1_PORT_c);
#endif
#if gLEDsOnTargetBoardCnt_c > 1
        GPIO_DisablePortClockGating((GpioPortID_t)gLED2_PORT_c);
#endif        
#if gLEDsOnTargetBoardCnt_c > 2
        GPIO_DisablePortClockGating((GpioPortID_t)gLED3_PORT_c);
#endif  
#if gLEDsOnTargetBoardCnt_c > 3
        GPIO_DisablePortClockGating((GpioPortID_t)gLED4_PORT_c);
#endif
      
#endif /* #if gTarget_UserDefined_d == 0 */  
  
#if gTMR_Enabled_d        
        /* free the timer used for flashing mode */
	TMR_FreeTimer(mLEDTimerID);
#endif        
}

/******************************************************************************
 * Name: LED_TurnOnLed
 * Description: Turns ON the specified LED(s)
 * Parameters: LEDNr - LED number(s) to be turned ON
 * Return: 
 ******************************************************************************/
void LED_TurnOnLed
(
		LED_t LEDNr
)
{
	if (LEDNr & LED1)
		Led1On(); /* turn ON LED 1 */
	if (LEDNr & LED2)
		Led2On(); /* turn ON LED 2 */
	if (LEDNr & LED3)
		Led3On(); /* turn ON LED 3 */
	if (LEDNr & LED4)
		Led4On(); /* turn ON LED 4 */
}

/******************************************************************************
 * Name: LED_TurnOffLed
 * Description: Turns OFF the specified LED(s)
 * Parameters: LEDNr - LED number(s) to be turned OFF
 * Return: 
 ******************************************************************************/
void LED_TurnOffLed
(
		LED_t LEDNr
)
{
	if (LEDNr & LED1)
		Led1Off(); /* turn OFF LED 1 */
	if (LEDNr & LED2)
		Led2Off(); /* turn OFF LED 2 */
	if (LEDNr & LED3)
		Led3Off(); /* turn OFF LED 3 */
	if (LEDNr & LED4)
		Led4Off(); /* turn OFF LED 4 */
}

/******************************************************************************
 * Name: LED_ToggleLed
 * Description: Toggles the specified LED(s)
 * Parameters:  LEDNr - LED number(s) to be toggled
 * Return: -
 ******************************************************************************/
void LED_ToggleLed
(
		LED_t LEDNr
)
{
	if (LEDNr & LED1)
		Led1Toggle(); /* toggle LED 1 */
	if (LEDNr & LED2)
		Led2Toggle(); /* toggle LED 2 */
	if (LEDNr & LED3)
		Led3Toggle(); /* toggle LED 3 */
	if (LEDNr & LED4)
		Led4Toggle(); /* toggle LED 4 */
}

/******************************************************************************
 * Name: LED_TurnOffAllLeds
 * Description: Turns OFF all LEDs
 * Parameters: -
 * Return: -
 ******************************************************************************/
void LED_TurnOffAllLeds
(
		void
)
{
	LED_TurnOffLed(LED_ALL);
}

/******************************************************************************
 * Name: LED_TurnOnAllLeds
 * Description: Turns ON all LEDs
 * Parameters: -
 * Return: -
 ******************************************************************************/
void LED_TurnOnAllLeds
(
		void
)
{
	LED_TurnOnLed(LED_ALL);
}

/******************************************************************************
 * Name: LED_StopFlashingAllLeds
 * Description: Stops flashing and turns OFF all LEDs
 * Parameters: -
 * Return: -
 ******************************************************************************/
void LED_StopFlashingAllLeds(void)
{
	LED_SetLed(LED_ALL, gLedOff_c);
}

/******************************************************************************
 * LED_StartFlash
 *
 * 
 *******************************************************************************/
/******************************************************************************
 * Name: LED_StartFlash
 * Description: Starts flashing one or more LEDs 
 * Parameters: [IN] LED_t LEDNr - LED Number (may be an OR of the list)
 * Return: -
 ******************************************************************************/
void LED_StartFlash
(
		LED_t LEDNr
)
{
	/* indicate which LEDs are flashing */
	mLedFlashingLEDs |= LEDNr;

#if gTMR_Enabled_d        
	/* start the timer */
	TMR_StartIntervalTimer(mLEDTimerID, mLEDInterval_c, LED_FlashTimeout);
#else
#warning "The TIMER component is not enabled and therefore the LED flashing function is disabled"
#endif
}

#if gLEDBlipEnabled_d
/******************************************************************************
 * Name: LED_StartBlip
 * Description: Set up for blinking one or more LEDs once 
 * Parameters: [IN] LED_t LEDNr - LED Number (may be an OR of the list)
 * Return: -
 ******************************************************************************/
void LED_StartBlip
(
		LED_t LEDNr
)
{
	uint8_t iLedIndex;

	/* set up for blinking one or more LEDs once */
	for(iLedIndex = 0; iLedIndex < gLEDsOnTargetBoardCnt_c; ++iLedIndex) {
		if(LEDNr & (1 << iLedIndex))
			mLedBlips[iLedIndex] = 2;   /* blink on, then back off */
	}

	/* start flashing */
	LED_StartFlash(LEDNr);
}
#endif

/******************************************************************************
 * Name: LED_StopFlash
 * Description: Stop an LED from flashing. 
 * Parameters: [IN] LED_t LEDNr - LED Number (may be an OR of the list)
 * Return: -
 ******************************************************************************/
void LED_StopFlash
(
		LED_t LEDNr
)
{
	/* leave stopped LEDs in the off state */
	LED_TurnOffLed(LEDNr);

	/* stop flashing on one or more LEDs */
	mLedFlashingLEDs &= (~LEDNr);

	/* if ALL LEDs have stopped flashing, then stop timer */
	if(!mLedFlashingLEDs)
		TMR_StopTimer(mLEDTimerID);
}

/******************************************************************************
 * Name: LED_StartSerialFlash
 * Description: starts serial flashing LEDs
 * Parameters: -
 * Return: -
 ******************************************************************************/
void LED_StartSerialFlash
(
		void
)
{
	/* indicate going into flashing mode (must be done first) */
	LED_StartFlash(0);

	/* set up for serial lights */
	LED_TurnOffAllLeds();
	Led1On();
	mLedFlashingLEDs = LED1 | LED2; /* toggle these to on first flash */

	/* indicate in serial flash mode */
	mfLedInSerialMode = TRUE;
}

/******************************************************************************
 * Name: LED_SetHex
 * Description: Sets a specified hex value on the LEDs 
 * Parameters: [IN] hexValue - the value to be set on LEDs
 * Return: -
 ******************************************************************************/
void LED_SetHex
(
		uint8_t hexValue
)
{
	LED_SetLed(LED_ALL, gLedOff_c);
	LED_SetLed(hexValue, gLedOn_c);
}

/******************************************************************************
 * Name: LED_SetLed
 * Description: This function can set the specified LED(s) in one of the  
 *              following states: On, Off, Toggle, Flashing or StopFlash
 * Parameters: [IN] LEDNr - LED(s) to
 * 			   [IN] LedState_t state - one of the possible states listed above	  
 * Return: 
 ******************************************************************************/
void LED_SetLed
(
		LED_t LEDNr,
		LedState_t state
)
{
	/* turning off flashing same as off state */
	if(state == gLedStopFlashing_c)
		state = gLedOff_c;

	/* turn off serial lights if in serial mode */
	LED_ExitSerialFlash();

	/* flash LED */
	if(state == gLedFlashing_c)
		LED_StartFlash(LEDNr);

#if gLEDBlipEnabled_d
	/* blink LEDs */
	else if(state == gLedBlip_c)
		LED_StartBlip(LEDNr);
#endif

	/* On, Off or Toggle Led*/
	else {

		/* on, off or toggle will stop flashing on the LED. Also exits serial mode */
		if(mfLedInSerialMode || (mLedFlashingLEDs & LEDNr))
			LED_StopFlash(LEDNr);

		/* Select the operation to be done on the port */
		if(state == gLedOn_c)
			LED_TurnOnLed(LEDNr);
		if(state == gLedOff_c)
			LED_TurnOffLed(LEDNr);
		if(state == gLedToggle_c)
			LED_ToggleLed(LEDNr);
	}
}

/******************************************************************************
 *******************************************************************************
 * Private functions
 *******************************************************************************
 ******************************************************************************/

/******************************************************************************
 * Name: LED_ExitSerialFlash
 * Description: Stop serial flashing mode, if serial flash mode is active. 
 *              Turns all LEDs off.
 * Param(s): -
 * Return: -
 ******************************************************************************/
static void LED_ExitSerialFlash
(
		void
)
{
	if(mfLedInSerialMode) 
	{
		mLedFlashingLEDs = 0;   /* no LEDs are currently flashing */
		LED_TurnOffAllLeds();
		TMR_StopTimer(mLEDTimerID);
		mfLedInSerialMode = FALSE;
	}
}

#if gLEDBlipEnabled_d
/******************************************************************************
 * Name: LED_DecrementBlip
 * Description: Decrements the blink count
 * Param(s): -
 * Return: -
 ******************************************************************************/
static void LED_DecrementBlip(void)
{
	uint8_t iLedIndex;	

	for(iLedIndex = 0; iLedIndex < gLEDsOnTargetBoardCnt_c; ++iLedIndex) 
	{
		if(mLedBlips[iLedIndex]) 
		{
			--mLedBlips[iLedIndex];
			if(!mLedBlips[iLedIndex])
			{
				mLedFlashingLEDs &= ~(1 << iLedIndex);
			}
		}
	}

	/* done, stop the timer */
	if(!mLedFlashingLEDs)
	{
		TMR_StopTimer(mLEDTimerID);
	}
}
#endif

/******************************************************************************
 * Name: LED_FlashTimeout
 * Description: timer callback function that is called each time the timer 
 *              expires
 * Param(s): [IN] timerId - the timer ID
 * Return: -
 ******************************************************************************/
#if gTMR_Enabled_d        
static void LED_FlashTimeout
(
		uint8_t timerId /* IN: TimerID. */
)
{
	(void)timerId;  /* prevent compiler warning */

	if(mLedFlashingLEDs & LED1)
		Led1Toggle();
	if(mLedFlashingLEDs & LED2)
		Led2Toggle();
	if(mLedFlashingLEDs & LED3)
		Led3Toggle();
	if(mLedFlashingLEDs & LED4)
		Led4Toggle();

#if gLEDBlipEnabled_d
	/* decrement blips */
	LED_DecrementBlip();
#endif

	/* if serial lights, move on to next light */
	if(mfLedInSerialMode)
	{
		mLedFlashingLEDs = mLedFlashingLEDs << 1;
		if(mLedFlashingLEDs & (LED4 << 1))  /* wrap around */
		{
			mLedFlashingLEDs &= LED_ALL;
			mLedFlashingLEDs |= LED1;
		}
	}
}
#endif /* gTMR_Enabled_d */
#endif /* gLEDSupported_d */
