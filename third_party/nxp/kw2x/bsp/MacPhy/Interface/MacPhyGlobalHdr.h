/************************************************************************************
* The global include file for the MAC and PHY stack.
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

#ifndef _MACPHYGLOBALHDR_H_
#define _MACPHYGLOBALHDR_H_

#include "EmbeddedTypes.h"
#include "AppToMacPhyConfig.h"

#include "FunctionalityDefines.h"

#ifdef __cplusplus
    extern "C" {
#endif

/************************************************************************************
*************************************************************************************
* Code profiling functionality
*************************************************************************************
************************************************************************************/

#ifdef RXFILTER_PROFILING_ENABLED
  #define RXFILTER_PROFILING_ENABLE_PINS Dbg_EnablePins()
  #define RXFILTER_PROFILING_SETPIN_0    Dbg_SetPin(gDbgPin0_c, gDbgOn_c)
  #define RXFILTER_PROFILING_SETPIN_1    Dbg_SetPin(gDbgPin1_c, gDbgOn_c)
  #define RXFILTER_PROFILING_SETPIN_2    Dbg_SetPin(gDbgPin2_c, gDbgOn_c)
  #define RXFILTER_PROFILING_SETPIN_3    Dbg_SetPin(gDbgPin3_c, gDbgOn_c)

  #define RXFILTER_PROFILING_CLRPIN_0    Dbg_SetPin(gDbgPin0_c, gDbgOff_c)
  #define RXFILTER_PROFILING_CLRPIN_1    Dbg_SetPin(gDbgPin1_c, gDbgOff_c)
  #define RXFILTER_PROFILING_CLRPIN_2    Dbg_SetPin(gDbgPin2_c, gDbgOff_c)
  #define RXFILTER_PROFILING_CLRPIN_3    Dbg_SetPin(gDbgPin3_c, gDbgOff_c)

  #define RXFILTER_PROFILING_TGLPIN_0    Dbg_SetPin(gDbgPin0_c, gDbgToggle_c)
  #define RXFILTER_PROFILING_TGLPIN_1    Dbg_SetPin(gDbgPin1_c, gDbgToggle_c)
  #define RXFILTER_PROFILING_TGLPIN_2    Dbg_SetPin(gDbgPin2_c, gDbgToggle_c)
  #define RXFILTER_PROFILING_TGLPIN_3    Dbg_SetPin(gDbgPin3_c, gDbgToggle_c)
#else
  #define RXFILTER_PROFILING_ENABLE_PINS
  #define RXFILTER_PROFILING_SETPIN_0
  #define RXFILTER_PROFILING_SETPIN_1
  #define RXFILTER_PROFILING_SETPIN_2
  #define RXFILTER_PROFILING_SETPIN_3

  #define RXFILTER_PROFILING_CLRPIN_0
  #define RXFILTER_PROFILING_CLRPIN_1
  #define RXFILTER_PROFILING_CLRPIN_2
  #define RXFILTER_PROFILING_CLRPIN_3

  #define RXFILTER_PROFILING_TGLPIN_0
  #define RXFILTER_PROFILING_TGLPIN_1
  #define RXFILTER_PROFILING_TGLPIN_2
  #define RXFILTER_PROFILING_TGLPIN_3
#endif /* RXFILTER_PROFILING_ENABLED */


#ifdef CODE_PROFILING_ENABLED
  #define CODE_PROFILING_ENABLE_PINS  Dbg_EnablePins() 
  #define CODE_PROFILING_SETPIN_0     Dbg_SetPin(gDbgPin0_c, gDbgOn_c) 
  #define CODE_PROFILING_SETPIN_1     Dbg_SetPin(gDbgPin1_c, gDbgOn_c) 
  #define CODE_PROFILING_SETPIN_2     Dbg_SetPin(gDbgPin2_c, gDbgOn_c) 
  #define CODE_PROFILING_SETPIN_3     Dbg_SetPin(gDbgPin3_c, gDbgOn_c) 

  #define CODE_PROFILING_CLRPIN_0     Dbg_SetPin(gDbgPin0_c, gDbgOff_c) 
  #define CODE_PROFILING_CLRPIN_1     Dbg_SetPin(gDbgPin1_c, gDbgOff_c) 
  #define CODE_PROFILING_CLRPIN_2     Dbg_SetPin(gDbgPin2_c, gDbgOff_c) 
  #define CODE_PROFILING_CLRPIN_3     Dbg_SetPin(gDbgPin3_c, gDbgOff_c) 

  #define CODE_PROFILING_TGLPIN_0     Dbg_SetPin(gDbgPin0_c, gDbgToggle_c) 
  #define CODE_PROFILING_TGLPIN_1     Dbg_SetPin(gDbgPin1_c, gDbgToggle_c) 
  #define CODE_PROFILING_TGLPIN_2     Dbg_SetPin(gDbgPin2_c, gDbgToggle_c) 
  #define CODE_PROFILING_TGLPIN_3     Dbg_SetPin(gDbgPin3_c, gDbgToggle_c) 
#else
  #define CODE_PROFILING_ENABLE_PINS 
  #define CODE_PROFILING_SETPIN_0 
  #define CODE_PROFILING_SETPIN_1 
  #define CODE_PROFILING_SETPIN_2 
  #define CODE_PROFILING_SETPIN_3 

  #define CODE_PROFILING_CLRPIN_0 
  #define CODE_PROFILING_CLRPIN_1 
  #define CODE_PROFILING_CLRPIN_2 
  #define CODE_PROFILING_CLRPIN_3 

  #define CODE_PROFILING_TGLPIN_0 
  #define CODE_PROFILING_TGLPIN_1 
  #define CODE_PROFILING_TGLPIN_2 
  #define CODE_PROFILING_TGLPIN_3 
#endif /* CODE_PROFILING_ENABLED */

  // Used for measuring timing for security
#ifdef SECURITY_PROFILING_ENABLED
  #define SECURITY_PROFILING_ENABLE_PINS  Dbg_EnablePins() 
  #define SECURITY_PROFILING_SETPIN_0     Dbg_SetPin(gDbgPin0_c, gDbgOn_c) 
  #define SECURITY_PROFILING_SETPIN_1     Dbg_SetPin(gDbgPin1_c, gDbgOn_c) 
  #define SECURITY_PROFILING_SETPIN_2     Dbg_SetPin(gDbgPin2_c, gDbgOn_c) 
  #define SECURITY_PROFILING_SETPIN_3     Dbg_SetPin(gDbgPin3_c, gDbgOn_c) 

  #define SECURITY_PROFILING_CLRPIN_0     Dbg_SetPin(gDbgPin0_c, gDbgOff_c) 
  #define SECURITY_PROFILING_CLRPIN_1     Dbg_SetPin(gDbgPin1_c, gDbgOff_c) 
  #define SECURITY_PROFILING_CLRPIN_2     Dbg_SetPin(gDbgPin2_c, gDbgOff_c) 
  #define SECURITY_PROFILING_CLRPIN_3     Dbg_SetPin(gDbgPin3_c, gDbgOff_c) 

  #define SECURITY_PROFILING_TGLPIN_0     Dbg_SetPin(gDbgPin0_c, gDbgToggle_c) 
  #define SECURITY_PROFILING_TGLPIN_1     Dbg_SetPin(gDbgPin1_c, gDbgToggle_c) 
  #define SECURITY_PROFILING_TGLPIN_2     Dbg_SetPin(gDbgPin2_c, gDbgToggle_c) 
  #define SECURITY_PROFILING_TGLPIN_3     Dbg_SetPin(gDbgPin3_c, gDbgToggle_c) 
#else
  #define SECURITY_PROFILING_ENABLE_PINS
  #define SECURITY_PROFILING_SETPIN_0
  #define SECURITY_PROFILING_SETPIN_1
  #define SECURITY_PROFILING_SETPIN_2
  #define SECURITY_PROFILING_SETPIN_3

  #define SECURITY_PROFILING_CLRPIN_0
  #define SECURITY_PROFILING_CLRPIN_1
  #define SECURITY_PROFILING_CLRPIN_2
  #define SECURITY_PROFILING_CLRPIN_3

  #define SECURITY_PROFILING_TGLPIN_0
  #define SECURITY_PROFILING_TGLPIN_1
  #define SECURITY_PROFILING_TGLPIN_2
  #define SECURITY_PROFILING_TGLPIN_3
#endif /* SECURITY_PROFILING_ENABLED */

#define Dbg_Init()
#define Dbg_Assert(x)
#define Dbg_Printf(x)
#define Dbg_Printf1(x,y)
#define Dbg_Printf2(x,y,z)

  // Module specific assert switches. Undefine or set
  // to ASSERT_NONE to disable asserts for a module.
#define ASSERT_NONE   0
#define ASSERT_STOP   1
#define PHY_ASSERT    ASSERT_NONE
#define SEQ_ASSERT    ASSERT_STOP
#define MEM_ASSERT    ASSERT_STOP
#define MAC_ASSERT    ASSERT_STOP
#define ASP_ASSERT    ASSERT_STOP
#define SMAPI_ASSERT  ASSERT_STOP


#if PHY_ASSERT == ASSERT_STOP
  #define PhyAssert(regexp) Dbg_Assert(regexp)
#else
  #define PhyAssert(regexp)
#endif /* PHY_ASSERT == ASSERT_STOP */

#if MAC_ASSERT == ASSERT_STOP
  #define MacAssert(regexp) Dbg_Assert(regexp)
#else
  #define MacAssert(regexp)
#endif /* MAC_ASSERT == ASSERT_STOP */

#if SEQ_ASSERT == ASSERT_STOP
  #define SeqAssert(regexp) Dbg_Assert(regexp)
#else
  #define SeqAssert(regexp)
#endif /* Seq_ASSERT == ASSERT_STOP */

#if MEM_ASSERT == ASSERT_STOP
  #define MemAssert(regexp) Dbg_Assert(regexp)
#else
  #define MemAssert(regexp)
#endif /* MEM_ASSERT == ASSERT_STOP */

#if ASP_ASSERT == ASSERT_STOP
  #define AspAssert(regexp) Dbg_Assert(regexp)
#else
  #define AspAssert(regexp)
#endif /* ASP_ASSERT == ASSERT_STOP */

#if SMAPI_ASSERT == ASSERT_STOP
  #define SmApiAssert(regexp) Dbg_Assert(regexp)
#else
  #define SmApiAssert(regexp)
#endif /* SMAPI_ASSERT == ASSERT_STOP */

#ifdef __cplusplus
}
#endif

#endif /* _MACPHYGLOBALHDR_H_ */
