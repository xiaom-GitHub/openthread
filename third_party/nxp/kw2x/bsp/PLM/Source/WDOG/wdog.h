/******************************************************************************
* Filename: WDOG.h
*
* Description: Header file for WDOG functions set for ARM CORTEX-M4 
*              processor
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

#ifndef _WDOG_H_
#define _WDOG_H_

#include "PortConfig.h"
#include "WDOG_Interface.h"

#ifdef __cplusplus
    extern "C" {
#endif
   
/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/

/*
* Name: gWDOGUnlockKey1_c, gWDOGUnlockKey2_c
* Description: WDT unlock keys
*/
#define gWDOGUnlockKey1_c    0xC520U    
#define gWDOGUnlockKey2_c    0xD928U

/*
* Name: gWDOGRefreshKey1_c, gWDOGRefreshKey2_c
* Description: WDT refresh keys
*/   
#define gWDOGRefreshKey1_c   0xA602U
#define gWDOGRefreshKey2_c   0xB480U

/*
* Name: gWDOGTimeoutRegMask_c
* Description: WDT timeout register mask
*/   
#define gWDOGTimeoutRegMask_c  0x0000FFFFU
   
/*
* Name: gWDOGWindowRegMask_c
* Description: WDT window register mask
*/   
#define gWDOGWindowRegMask_c   0x0000FFFFU

/*
* Name: gWDOGPrescalerValMax_c
* Description: WDT prescaler maximum value (guard)
*/   
#define gWDOGPrescalerValMax_c     7

/*
* Name: see below
* Description: Registers mapping
*/   

#define gWDOGStatusCtrlRegHi_c    WDOG_STCTRLH
#define gWDOGStatusCtrlRegLo_c    WDOG_STCTRLL
#define gWDOGTimeoutValRegHi_c    WDOG_TOVALH
#define gWDOGTimeoutValRegLo_c    WDOG_TOVALL
#define gWDOGWindowRegHi_c        WDOG_WINH
#define gWDOGWindowRegLo_c        WDOG_WINL
#define gWDOGRefreshReg_c         WDOG_REFRESH
#define gWDOGUnlockReg_c          WDOG_UNLOCK
#define gWDOGTmrOutputRegHi_c     WDOG_TMROUTH
#define gWDOGTmrOutputRegLo_c     WDOG_TMROUTL
#define gWDOGResetCntReg_c        WDOG_RSTCNT
#define gWDOGPrescalerReg_c       WDOG_PRESC

/*
* Name: gWDOG_UNLOCK
* Description: WDT Unlock macro definition
*/      
#define gWDOG_UNLOCK()             gWDOGUnlockReg_c = gWDOGUnlockKey1_c; gWDOGUnlockReg_c = gWDOGUnlockKey2_c;
                                    
/*
* Name: gWDOG_REFRESH
* Description: WDT refresh macro definition
*/                                     
#define gWDOG_REFRESH()            gWDOGRefreshReg_c = gWDOGRefreshKey1_c; gWDOGRefreshReg_c = gWDOGRefreshKey2_c;
   
   
/*****************************************************************************
******************************************************************************
* Private type definitions
******************************************************************************
*****************************************************************************/

/* NONE */

#ifdef __cplusplus
}
#endif

#endif /* _WDOG_H_ */
