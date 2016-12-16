/************************************************************************************
* The MAC/PHY Platform Library contains MAC/PHY specific functionality that needs 
* very efficient execution.
*
* The primitives in the library must all be implemented as macros.
* If no processor speed/timing problems are seen with the MAC/PHY execution then
* these macros can just contain the default C-expressions.
* However, if timing problems are seen, then the default C-expressions should be
* replaced with assembly language instructions.
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

#ifndef _MACPHYPLATFORMLIB_H_
#define _MACPHYPLATFORMLIB_H_

#ifndef aUnitBackoffPeriod_s
#define aUnitBackoffPeriod_s    20  // [symbols]
#endif

/************************************************************************************
* This primitive must be implemented as a macro.
*
* The macro must perform the following operation on the uint8_t variable "Length":
*
*   Length = (Length / UnitBackoff)*UnitBackoff + UnitBackoff
*
* The UnitBackoff parameter is a uint8_t variable.
*
* If no processor speed/timing problems are seen with the MAC/PHY execution then
* this macro can just contain the default C-expression:
*  #define MPPLib_RoundLengthToBackoff(Length, UnitBackoff) \
*    (Length=((Length / UnitBackoff)*UnitBackoff + UnitBackoff))
*
* However, if timing problems are seen, then the default C-expression should be
* replaced with assembly language.
* 
* Interface assumptions:
*   None
*   
* Return value:
*   The result of the operation must be stored back into Length.
* 
* 
************************************************************************************/
#define MPPLib_RoundLengthToBackoff(Length, UnitBackoff) (Length=((Length / UnitBackoff)*UnitBackoff + UnitBackoff))

/************************************************************************************
* This primitive must be implemented as a macro.
*
* The macro must perform the following operation on "Clock24" which is a pointer to
* a uint32_t variable:
*
*   remainder = (uint8_t)((0xFFFFFF & *pClock24)%aUnitBackoffPeriod_s)
*
* The aUnitBackoffPeriod_s is the 20 symbol unit backoff period in the 802.15.4
* specification.
*
* If no processor speed/timing problems are seen with the MAC/PHY execution then
* this macro can just contain the default C-expression:
*   #define MPPLib_Uint24ModUnitBackoffPeriod() \
*     (remainder = (uint8_t)((0xFFFFFF & *pClock24)%aUnitBackoffPeriod_s))
*
* However, if timing problems are seen, then the default C-expression should be
* replaced with assembly language instructions.
* 
* Interface assumptions:
*   This macro assumes that remainder and pClock24 exist.
*   
* Return value:
*   The result of the operation must be stored into remainder.
* 
* 
************************************************************************************/
#define MPPLib_Uint24ModUnitBackoffPeriod() (remainder = (uint8_t)((0xFFFFFF & *pClock24)%aUnitBackoffPeriod_s))

/************************************************************************************
* This primitive must be implemented as a macro.
*
* The macro must perform the following operation on the uint8_t variable 
* "timeToNextXcv":
*
*   timeToNextXcv = ((timeToNextXcv-1) / aUnitBackoffPeriod_b)*aUnitBackoffPeriod_b
*                     + aUnitBackoffPeriod_b;
*
* The aUnitBackoffPeriod_s is the 20 symbol unit backoff period in the 802.15.4
* specification.
*
* If no processor speed/timing problems are seen with the MAC/PHY execution then
* this macro can just contain the default C-expression:
*   #define MPPLib_ComputeSlottedBoundary() \
*    (timeToNextXcv = ((timeToNextXcv-1) / aUnitBackoffPeriod_b)*aUnitBackoffPeriod_b\
*    + aUnitBackoffPeriod_b)
*
* However, if timing problems are seen, then the default C-expression should be
* replaced with assembly language instructions.
* 
* Interface assumptions:
*   This macro assumes that timeToNextXcv exist.
*   
* Return value:
*   The result of the operation must be stored into timeToNextXcv.
*  
************************************************************************************/
#define MPPLib_ComputeSlottedBoundary() (timeToNextXcv = ((timeToNextXcv-1) / aUnitBackoffPeriod_b)*aUnitBackoffPeriod_b + aUnitBackoffPeriod_b)


#endif /* _MACPHYPLATFORMLIB_H_ */
