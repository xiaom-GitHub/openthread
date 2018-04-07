/**************************************************************************//**
* Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
*
* http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
*****************************************************************************
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software.@n
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.@n
* 3. This notice may not be removed or altered from any source distribution.@n
* 4. The source and compiled code may only be used on Energy Micro "EFM32"
*    microcontrollers and "EFR4" radios.
*
* DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
* obligation to support this Software. Energy Micro AS is providing the
* Software "AS IS", with no express or implied warranties of any kind,
* including, but not limited to, any implied warranties of merchantability
* or fitness for any particular purpose or warranties against infringement
* of any proprietary rights of a third party.
*
* Energy Micro AS will not be liable for any consequential, incidental, or
* special damages, or any other relief, or for any claim by any third party,
* arising from your use of this Software.
*
******************************************************************************/
#ifndef __RTCDRV_CONFIG_H__
#define __RTCDRV_CONFIG_H__

// Define how many timers RTCDRV will provide.
#define EMDRV_RTCDRV_NUM_TIMERS     (1)

// Uncomment the following line to include the wallclock functionality.
//#define EMDRV_RTCDRV_WALLCLOCK_CONFIG

// Uncomment the following line to enable integration with SLEEP driver.
//#define EMDRV_RTCDRV_SLEEPDRV_INTEGRATION

#endif // __RTCDRV_CONFIG_H__
