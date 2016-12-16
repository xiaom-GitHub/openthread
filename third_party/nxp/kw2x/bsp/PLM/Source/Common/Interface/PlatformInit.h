/**************************************************************************
* Filename: PlatformInit.h
*
* Description: Platform specific header file
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

#ifndef _PLATFORMINIT_H_
#define _PLATFORMINIT_H_

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
*******************************************************************************
* Public function prototypes
*******************************************************************************
******************************************************************************/


/****************************************************************************
* Name: PlatformPortInit
*
* Description: Perform the complete GPIO port initialization
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
extern void PlatformPortInit
(
		void
);

/************************************************************************************
* Name: Platform_Init
* Description: Initialize platform specific stuff
*
* Interface assumptions:
*
*   NOTE!!! This First section of this function code is added to ensure that the symbols
*   are included in the build. The linker will remove these symbols if not referenced.
*   Hope to find a better way 21.10.03 MVC
*
* Parameter(s):
*   NONE
*
* Return value:
*   NONE
*
************************************************************************************/
extern void Platform_Init
(
    void
);


/************************************************************************************
* Name:
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
extern void Init_802_15_4
(
		void
);

#ifdef __cplusplus
}
#endif

#endif /* _PLATFORMINIT_H_  */
