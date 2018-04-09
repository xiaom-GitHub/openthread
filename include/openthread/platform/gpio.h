/*
 *  Copyright (c) 2017, The OpenThread Authors.
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
 * @brief
 *   This file includes the platform abstraction for GPIO.
 */

#ifndef GPIO_H_
#define GPIO_H_

#ifdef OPENTHREAD_CONFIG_FILE
#include OPENTHREAD_CONFIG_FILE
#else
#include <openthread-config-generic.h>
#endif

#include "openthread/types.h"

/**
 * @def OPENTHREAD_EXAMPLES_PLATFROMS
 *
 * OPENTHREAD_EXAMPLES_xxx_1: indicates to configure the IID for the first
 * device of one platform; commenting this marco indicates to configure the
 * IID for the second device.
 *
 * defining this macro is to hardcode the IID for which device will be used.
 *
 * EFR32 and DA15000 only one device
 */
//#define OPENTHREAD_EXAMPLES_CC2538_1
//#define OPENTHREAD_EXAMPLES_CC2650_1
//#define OPENTHREAD_EXAMPLES_KW41Z_1
#define OPENTHREAD_EXAMPLES_EFR32_1

/**
 * @def OPENTHREAD_GPIO_LOGIC_LEVEL
 *
 * logic 1: LEDs are turned on by outputing a high GPIO level(other side is GND);
 * logic 0: LEDs are turned on by outputing a low GPIO level(other side is VDD);
 * 
 * defining this macro is to configure which kind of method will be used.
 */
#ifndef OPENTHREAD_GPIO_LOGIC_LEVEL
#define OPENTHREAD_GPIO_LOGIC_LEVEL   1
#endif

#ifdef OPENTHREAD_EXAMPLES_EFR32
#include "em_gpio.h"

/**
 *  OPENTHREAD_GPIO_LOGIC_LEVEL == 1
 *
 *  @mapping gpio pins with RGB control
 *
 *  Red:   PD8  < - > Exp_Header_3
 *  Green: PD9  < - > Exp_Header_5
 *  Blue:  PD10 < - > Exp_Header_7
 *         GND  < - > Exp_Header_1
 */

/**
 *  OPENTHREAD_GPIO_LOGIC_LEVEL == 0
 *
 *  @mapping gpio pins with RGB control
 *
 *  Red:   PD8  < - > Exp_Header_3
 *  Green: PD9  < - > Exp_Header_5
 *  Blue:  PD10 < - > Exp_Header_7
 *         +5V  < - > Exp_Header_18
 */

#define LED_GPIO_PORT   gpioPortD
#define RED_LED_PIN     8
#define GREEN_LED_PIN   9
#define BLUE_LED_PIN    10

#define INTERRUPT_PORT  0
#define INTERRUPT_PIN   0
#endif // OPENTHREAD_EXAMPLES_EFR32


#ifdef OPENTHREAD_EXAMPLES_CC2538

/**
 *  OPENTHREAD_GPIO_LOGIC_LEVEL == 1
 *
 *  @mapping gpio pins with RGB control
 *
 *  Red:   PB2  < - > RF1.11
 *  Green: PB3  < - > RF1.13
 *  Blue:  PB4  < - > RF1.15
 *         GND: choose any avaiable
 */

/**
 *  OPENTHREAD_GPIO_LOGIC_LEVEL == 0
 *
 *  @mapping gpio pins with RGB control
 *
 *  Red:   PB2  < - > RF1.11
 *  Green: PB3  < - > RF1.13
 *  Blue:  PB4  < - > RF1.15
 *         +5v: < - > P409.1
 */

#define GPIO_FALLING_EDGE       0x00000000  // Interrupt on falling edge
#define GPIO_RISING_EDGE        0x00000004  // Interrupt on rising edge
#define GPIO_BOTH_EDGES         0x00000001  // Interrupt on both edges
#define GPIO_LOW_LEVEL          0x00000002  // Interrupt on low level
#define GPIO_HIGH_LEVEL         0x00000007  // Interrupt on high level

#define GPIO_A_BASE     0x400D9000  // GPIO_A
#define GPIO_B_BASE     0x400DA000  // GPIO_B
#define GPIO_C_BASE     0x400DB000  // GPIO_C 
#define GPIO_D_BASE     0x400DC000  // GPIO_D

#define LED_GPIO_PORT   GPIO_B_BASE // GPIO_PORT_B
#define RED_LED_PIN     0x00000004  // pin 2
#define GREEN_LED_PIN   0x00000008  // pin 3
#define BLUE_LED_PIN    0x00000010  // pin 4

#define INTERRUPT_PORT  GPIO_A_BASE
#define INTERRUPT_PIN   0x00000008  // PA3
#endif // OPENTHREAD_EXAMPLES_CC2538

#ifdef OPENTHREAD_EXAMPLES_CC2650

/**
 *  OPENTHREAD_GPIO_LOGIC_LEVEL == 1
 *
 *  @mapping gpio pins with RGB control
 *
 *  Red:   DIO0
 *  Green: DIO21 // only this pin works for mtd role
 *  Blue:  DIO22
 *         GND   // choose any available
 */

/**
 *  OPENTHREAD_GPIO_LOGIC_LEVEL == 0
 *
 *  @mapping gpio pins with RGB control
 *
 *  Red:   DIO0
 *  Green: DIO21 // only this pin works for mtd role
 *  Blue:  DIO22
 *         +5V   // choose any available +5v
 */

#define LED_GPIO_PORT   0x40022000 
#define RED_LED_PIN     0  // DIO0
#define GREEN_LED_PIN   21 // DIO21
#define BLUE_LED_PIN    22 // DIO22
#endif // OPENTHREAD_EXAMPLES_CC2650

#ifdef OPENTHREAD_EXAMPLES_NRF52840
#undef PACKAGE

/**
 *  OPENTHREAD_GPIO_LOGIC_LEVEL == 1
 *
 *  @mapping gpio pins with RGB control
 *
 *  Red:   P1.13  < - > D11
 *  Green: P1.14  < - > D12
 *  Blue:  P1.15  < - > D13
 *         GDN    // choose any available
 */

/**
 *  OPENTHREAD_GPIO_LOGIC_LEVEL == 0
 *
 *  @mapping gpio pins with RGB control
 *
 *  Red:   P1.13  < - > D11
 *  Green: P1.14  < - > D12
 *  Blue:  P1.15  < - > D13
 *         VDD    // choose any available 
 */

#define LED_GPIO_PORT   0x50000300UL
#define RED_LED_PIN     45  // 13 + 32 (NRF_P1)
#define GREEN_LED_PIN   46  // 14 + 32 (NRF_P1)
#define BLUE_LED_PIN    47  // 15 + 32 (NRF_P1)
#endif // OPENTHREAD_EXAMPLES_NRF52840

#ifdef OPENTHREAD_EXAMPLES_DA15000
/**
 *  OPENTHRAD_GPIO_LOGIC_LEVEL == 1
 *
 *  @mapping gpio pins with RGB control
 *
 *  Red:   P3_0
 *  Green: P3_1
 *  Blue:  P3_2
 *         GND (J3.1)
 */

/**
 *  OPENTHRAD_GPIO_LOGIC_LEVEL == 0
 *
 *  @mapping gpio pins with RGB control
 *
 *  Red:   P3_0
 *  Green: P3_1
 *  Blue:  P3_2
 *         V3_3 (J3.25)
 */

#define LED_GPIO_PORT   3 // HW_GPIO_PORT_3
#define RED_LED_PIN     0 // HW_GPIO_PIN_0
#define GREEN_LED_PIN   1 // HW_GPIO_PIN_1
#define BLUE_LED_PIN    2 // HW_GPIO_PIN_2
#endif // OPENTHREAD_EXAMPLES_DA15000

#ifdef OPENTHREAD_EXAMPLES_KW41Z

/**
 *  OPENTHREAD_GPIO_LOGIC_LEVEL == 1
 *
 *  @mapping gpio pins with RGB control
 *
 *  Red:   J2.4   < - > PTA16 // ensure Jumper J28 is on
 *  Green: J2.5   < - > PTA17 // ensure Jumper J29 is on
 *  Blue:  J2.9   < - > PTC3
 *  GND:   J2.7   < - > GND
 */

/**
 *  OPENTHREAD_GPIO_LOGIC_LEVEL == 0
 *
 *  @mapping gpio pins with RGB control
 *
 *  Red:   J2.4   < - > PTA16 // ensure Jumper J28 is on
 *  Green: J2.5   < - > PTA17 // ensure Jumper J29 is on
 *  Blue:  J2.9   < - > PTC3
 *  GND:   J2.8   < - > +3.3V
 */

#define LED_GPIO_PORT   0x400FF000u
#define RED_LED_PIN     16 
#define GREEN_LED_PIN   17
#define BLUE_LED_PIN    3

#define INTERRUPT_PORT  0x400FF080u//0x400FF000u
#define INTERRUPT_PIN   4
#endif // OPENTHREAD_EXAMPLES_KW41Z

#if (OPENTHREAD_GPIO_LOGIC_LEVEL == 1)
enum
{
    GPIO_LOGIC_HIGH = 1,
    GPIO_LOGIC_LOW  = 0,
};
#else
enum
{
    GPIO_LOGIC_HIGH = 0,
    GPIO_LOGIC_LOW  = 1,
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup gpio GPIO
 * @ingroup platform
 *
 * @brief
 *   This module includes the platform abstraction to GPIO.
 *
 * @{
 *
 */

#if OPENTHREAD_EXAMPLES_EFR32
typedef void (*otPlatGpioIntCallback)(uint8_t pin);
#else
typedef void (*otPlatGpioIntCallback)(void);
#endif

/**
 * Init GPIO module.
 *
 */
void otPlatGpioInit(void);

/**
 * Set logic high for output pin.
 *
 */
void otPlatGpioOutSet(uint32_t port, uint8_t pin);

/**
 * Set logic low for output pin.
 *
 */
void otPlatGpioOutClear(uint32_t port, uint8_t pin);

/**
 * Toggle output pin.
 *
 */
void otPlatGpioOutToggle(uint32_t port, uint8_t pin);

/**
 * Read the value of output pin.
 *
 */
uint8_t otPlatGpioOutGet(uint32_t port, uint8_t pin);

/**
 * Register a callback for GPIO interrupt.
 *
 */
void otPlatGpioRegisterCallback(uint32_t port, otPlatGpioIntCallback aCallback);

/**
 * Enable GPIO interrupt.
 *
 */
void otPlatGpioIntEnable(uint32_t port, uint8_t pin);

/**
 * Clear GPIO interrupt.
 *
 */
void otPlatGpioIntClear(uint32_t port, uint8_t pin);

/**
 * @}
 *
 */

#ifdef __cplusplus
}  // end of extern "C"
#endif

#endif  // GPIO_H_
