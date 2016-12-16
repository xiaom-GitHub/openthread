/**************************************************************************
* Filename: UART_Interface.h
*
* Description: UART export interface file for ARM CORTEX-M4 processor
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

#ifndef __UART_INTERFACE_H__
#define __UART_INTERFACE_H__

#include "EmbeddedTypes.h"
#include "AppToPlatformConfig.h"
#include "TS_Interface.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
 *****************************************************************************
 * Public macros
 *****************************************************************************
 *****************************************************************************/
 
/*
 * Name: gUart1_Enabled_d
 * Description: Specifies if the UART_1 module interface is used
 */
#ifndef gUart1_Enabled_d
#define gUart1_Enabled_d               FALSE
#endif

/*
 * Name: gUart2_Enabled_d
 * Description: Specifies if the UART_2 module interface is used
 */
#ifndef gUart2_Enabled_d
#define gUart2_Enabled_d               FALSE
#endif

/*
 * Name: UART_USE_TX_FIFO
 * Description: enable / disable UART TX FIFO
 */
#ifndef gUART_USE_TX_FIFO_d
#define gUART_USE_TX_FIFO_d            (TRUE)
#endif

/*
 * Name: gUART_USE_RX_FIFO_d
 * Description: enable / disable UART RX FIFO
 */
#ifndef gUART_USE_RX_FIFO_d
#define gUART_USE_RX_FIFO_d            (TRUE)
#endif

/*
 * Name: gUART_TX_THRESHOLD_c
 * Description: UART TX threshold value
 */
#ifndef gUART_TX_THRESHOLD_c
#define gUART_TX_THRESHOLD_c           4
#endif

/*
 * Name: gUART_RX_THRESHOLD_c
 * Description: UART RX threshold value
 */
#ifndef gUART_RX_THRESHOLD_c
#define gUART_RX_THRESHOLD_c           1
#endif

/*
 * Name: gRxSci1Event_c, gRxSci2Event_c, gTxSci1Event_c, gTxSci2Event_c
 * Description: events for UART task
 */
#define gRxSci1Event_c                 (1<<0)
#define gRxSci2Event_c                 (1<<1)
#define gTxSci1Event_c                 (1<<2)
#define gTxSci2Event_c                 (1<<3)

/*
 * Name: gUart_PortDefault_d
 * Description: specifies the default used port
 */
#ifndef gUart_PortDefault_d
#if (gUart1_Enabled_d == TRUE)
#define gUart_PortDefault_d            1
#elif (gUart2_Enabled_d == TRUE)
#define gUart_PortDefault_d            2
#endif
#endif

/*
 * Name: gUart1_EnableHWFlowControl_d
 * Description: Enable/Disable UART_1 hardware flow control
 */
#ifndef gUart1_EnableHWFlowControl_d
#define gUart1_EnableHWFlowControl_d        FALSE
#endif /* gUart1_EnableHWFlowControl_d */

/*
 * Name: gUart2_EnableHWFlowControl_d
 * Description: Enable/Disable UART_2 hardware flow control
 */
#ifndef gUart2_EnableHWFlowControl_d
#define gUart2_EnableHWFlowControl_d        FALSE
#endif /* gUart2_EnableHWFlowControl_d */

/*
 * Name: gUart1_Hw_Sw_FlowControl_d
 * Description: UART_1 flow control pins controlled software
 *              Used for UART with FIFO not activated
 */

#ifndef gUart1_Hw_Sw_FlowControl_d
#define gUart1_Hw_Sw_FlowControl_d          FALSE
#endif

/*
 * Name: gUart2_Hw_Sw_FlowControl_d
 * Description: UART_2 flow control pins controlled software
 *              Used for UART with FIFO not activated
 */
#ifndef gUart2_Hw_Sw_FlowControl_d
#define gUart2_Hw_Sw_FlowControl_d          FALSE
#endif

/*
 * Flow Control configuration check
 */
#if (gUart1_EnableHWFlowControl_d == TRUE) && (gUart1_Hw_Sw_FlowControl_d == TRUE)
#error "Cannot set both software and hardware flow control at the same time"
#endif

#if (gUart2_EnableHWFlowControl_d == TRUE) && (gUart2_Hw_Sw_FlowControl_d == TRUE)
#error "Cannot set both software and hardware flow control at the same time"
#endif

/*
 * Name: gUart_TransmitBuffers_c
 * Description: Number of entries in the transmit-buffers-in-waiting list
 */
#ifndef gUart_TransmitBuffers_c
#define gUart_TransmitBuffers_c         3
#endif /* gUart_TransmitBuffers_c */

/*
 * Name: gUart1_ReceiveBufferSize_c
 * Description: Size of the UART_1 driver Rx circular buffer. These buffers are used to
 *              hold received bytes until the application can retrieve them via the
 *              UartX_GetBytesFromRxBuffer() functions, and are not otherwise accessible
 *              from outside the driver. The size DOES NOT need to be a power of two.
 */
#ifndef gUart1_ReceiveBufferSize_c
#define gUart1_ReceiveBufferSize_c      128
#endif /* gUart1_ReceiveBufferSize_c */

/*
 * Name: gUart2_ReceiveBufferSize_c
 * Description: Size of the UART_2 driver Rx circular buffer. These buffers are used to
 *              hold received bytes until the application can retrieve them via the
 *              UartX_GetBytesFromRxBuffer() functions, and are not otherwise accessible
 *              from outside the driver. The size DOES NOT need to be a power of two.
 */
#ifndef gUart2_ReceiveBufferSize_c
#define gUart2_ReceiveBufferSize_c      128
#endif /* gUart2_ReceiveBufferSize_c */

/*
 * Name: gUart1_RxFlowControlSkew_d
 * Description: UART_1 SKEW Control.
 *              If flow control is used, there is a delay before telling the far side
 *              to stop and the far side actually stopping. When there are SKEW bytes
 *              remaining in the driver's Rx buffer, tell the far side to stop
 *              transmitting.
 */
#ifndef gUart1_RxFlowControlSkew_d
#define gUart1_RxFlowControlSkew_d      32
#endif

/*
 * Name: gUart2_RxFlowControlSkew_d
 * Description: UART_2 SKEW Control.
 *              If flow control is used, there is a delay before telling the far side
 *              to stop and the far side actually stopping. When there are SKEW bytes
 *              remaining in the driver's Rx buffer, tell the far side to stop
 *              transmitting.
 */
#ifndef gUart2_RxFlowControlSkew_d
#define gUart2_RxFlowControlSkew_d      32
#endif

/*
 * Name: gUart1_RxFlowControlResume_d
 * Description: Number of bytes left in the UART_1 RX buffer when hardware flow control is
 *              deasserted.
 */
#ifndef gUart1_RxFlowControlResume_d
#define gUart1_RxFlowControlResume_d    32
#endif

/*
 * Name: gUart2_RxFlowControlResume_d
 * Description: Number of bytes left in the UART_1 RX buffer when hardware flow control is
 *              deasserted.
 */
#ifndef gUart2_RxFlowControlResume_d
#define gUart2_RxFlowControlResume_d    32
#endif

/*
 * Name: gUARTBaudRateXXXX_c
 * Description: UART standard baudrates definitions
 */

#define gUARTBaudRate1200_c             1200UL
#define gUARTBaudRate2400_c             2400UL
#define gUARTBaudRate4800_c             4800UL
#define gUARTBaudRate9600_c             9600UL
#define gUARTBaudRate19200_c            19200UL
#define gUARTBaudRate38400_c            38400UL
#define gUARTBaudRate57600_c            57600UL
#define gUARTBaudRate115200_c           115200UL
#define gUARTBaudRate230400_c           230400UL

/*******************************************************************************************************************
 * UART_1 configuration
 *
 * gUART1_Parity_c - must be one of the following: gUartParityNone_c, gUartParityEven_c, gUartParityOdd_c
 * gUART1_DataBits_c - must be one of the following: gUart_8bits_c, gUart_9bits_c
 * gUART1_Baudrate_c - any standard or non standard baudrate (bps) 
 * gUart1_RTS_ActiveHigh_c - must be one of the following: gRtsActiveLow_c, gRtsActiveHi_c
 *******************************************************************************************************************/

#if gUart1_Enabled_d

#ifndef gUART1_Parity_c
#define gUART1_Parity_c                     gUartParityNone_c
#endif

#ifndef gUART1_DataBits_c
#define gUART1_DataBits_c                   gUart_8bits_c
#endif

#ifndef gUART1_Baudrate_c
#define gUART1_Baudrate_c                   gUARTBaudRate115200_c
#endif

#ifndef gUart1_RTS_ActiveState_c
#define gUart1_RTS_ActiveState_c            gRtsActiveHi_c
#endif

#endif

/*******************************************************************************************************************
 * UART_2 configuration
 *
 * gUART2_Parity_c - must be one of the following: gUartParityNone_c, gUartParityEven_c, gUartParityOdd_c
 * gUART2_DataBits_c - must be one of the following: gUart_8bits_c, gUart_9bits_c
 * gUART2_Baudrate_c - any standard or non standard baudrate (bps)
 * gUart2_RTS_ActiveHigh_c - must be one of the following: gRtsActiveLow_c, gRtsActiveHi_c
 *******************************************************************************************************************/

#if gUart2_Enabled_d

#ifndef gUART2_Parity_c
#define gUART2_Parity_c                     gUartParityNone_c
#endif

#ifndef gUART2_DataBits_c
#define gUART2_DataBits_c                   gUart_8bits_c
#endif

#ifndef gUART2_Baudrate_c
#define gUART2_Baudrate_c                   gUARTBaudRate115200_c
#endif

#ifndef gUart2_RTS_ActiveState_c
#define gUart2_RTS_ActiveState_c            gRtsActiveHi_c
#endif

#endif

/*******************************************************************************************************************/

/*
 * Name: gUart1_InterruptPriority_c
 * Description: UART_1 IRQ priority
 */
#ifndef gUart1_InterruptPriority_c
#define gUart1_InterruptPriority_c          (0x5u)
#endif

/*
 * Name: gUart2_InterruptPriority_c
 * Description: UART_2 IRQ priority
 */
#ifndef gUart2_InterruptPriority_c
#define gUart2_InterruptPriority_c          (0x5u)
#endif
   
/*
 * Name: gUart1_ErrInterruptPriority_c
 * Description: UART_1 Error IRQ priority
 */
#ifndef gUart1_ErrInterruptPriority_c
#define gUart1_ErrInterruptPriority_c       (0x4u)
#endif

/*
 * Name: gUart2_ErrInterruptPriority_c
 * Description: UART_2 Error IRQ priority
 */
#ifndef gUart2_ErrInterruptPriority_c
#define gUart2_ErrInterruptPriority_c       (0x4u)
#endif

/*****************************************************************************
 *****************************************************************************
 * Public type definitions
 *****************************************************************************
 *****************************************************************************/

/*
 * Name: UartBaudRate_t
 * Description: data type definition for UART baudrate
 */
typedef uint32_t UartBaudRate_t;

/*
 * Name: UartParityMode_t
 * Description: This data type enumerates the possible values of UART parity modes
 */
typedef enum UartParity_tag
{
    gUartParityNone_c = 0,
    gUartParityEven_c,
    gUartParityOdd_c,
    gUartParityMax_c
} UartParityMode_t;

/*
 * Name: UartDataBits_t
 * Description: This data type enumerates the possible values of UART data bits
 */
typedef enum UartDataBits_tag
{
    gUart_8bits_c = 0,
    gUart_9bits_c,
    gUartDataBitsMax_c
} UartDataBits_t;

/*
 * Name: UartRTSActiveState_t
 * Description: RTS signal active state
 */
typedef enum UartRTSActiveState_tag
{
    gRtsActiveLow_c = 0,
    gRtsActiveHi_c
} UartRTSActiveState_t;

/*****************************************************************************
 *****************************************************************************
 * Public prototypes
 *****************************************************************************
 *****************************************************************************/

#if (gUart1_Enabled_d == FALSE) && (gUart2_Enabled_d == FALSE)
/*
 * Name: see section below
 * Description: stub functions if both UART ports are disabled
 */

#define Uart_ModuleInit()
#define Uart_ModuleUninit()
#define Uart_ClearErrors()
#define Uart1_SetBaud(baudRate)
#define Uart2_SetBaud(baudRate)
#define Uart1_Transmit(pBuf, bufLen, pfCallBack)    FALSE
#define Uart2_Transmit(pBuf, bufLen, pfCallBack)    FALSE
#define Uart1_IsTxActive()                          FALSE
#define Uart2_IsTxActive()                          FALSE
#define Uart1_SetRxCallBack(pfCallBack)
#define Uart2_SetRxCallBack(pfCallBack)
#define Uart1_GetByteFromRxBuffer(pDst)             FALSE
#define Uart2_GetByteFromRxBuffer(pDst)             FALSE
#define Uart1_UngetByte(byte)
#define Uart2_UngetByte(byte)
#define Uart1_RxBufferByteCount                     0
#define Uart2_RxBufferByteCount                     0

#else /* at least one UART driver is enabled */

/*
 * Name: gUartTaskId
 * Description: ID for UART task
 * VALUES: see definition of tsTaskID_t
 */
extern  tsTaskID_t gUartTaskId;

/*---------------------------------------------------------------------------
 * Name: Uart_ModuleInit
 * Description: Initialize the serial port(s) and module internal variables
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
extern void Uart_ModuleInit
(
        void
);

/*---------------------------------------------------------------------------
 * Name: Uart_ModuleUninit
 * Description: Shut down the serial port(s)
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
extern void Uart_ModuleUninit
(
        void
);

/*---------------------------------------------------------------------------
 * Name: Uart_ClearErrors
 * Description: Clear any error flags seen by the UART driver. In rare
 *              conditions, an error flag can be set without triggering an
 *              error interrupt, and will prevent Rx and/or Tx interrupts
 *              from occurring. The most likely cause is a breakpoint set
 *              during debugging, when a UART port is active.
 *              Calling this function occasionally allows the UART code
 *              to recover from these errors.
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
extern void Uart_ClearErrors
(
        void
);

/*---------------------------------------------------------------------------
 * Name: Uart1_GetByteFromRxBuffer
 * Description: Retrieve a byte from the driver's UART1 Rx buffer and store
 *              it at *pDst
 * Parameters: [OUT] pDst - destination pointer
 * Return: TRUE if a byte was retrieved / FALSE if the Rx buffer is empty
 *---------------------------------------------------------------------------*/
extern bool_t Uart1_GetByteFromRxBuffer
(
        unsigned char *pDst
);

/*---------------------------------------------------------------------------
 * Name: Uart2_GetByteFromRxBuffer
 * Description: Retrieve a byte from the driver's UART2 Rx buffer and store
 *              it at *pDst
 * Parameters: [OUT] pDst - destination pointer
 * Return: TRUE if a byte was retrieved / FALSE if the Rx buffer is empty
 *---------------------------------------------------------------------------*/
extern bool_t Uart2_GetByteFromRxBuffer
(
        unsigned char *pDst
);

/*---------------------------------------------------------------------------
 * Name: Uart1_IsTxActive
 * Description: checks if UART_1 transmitter is active
 * Parameters: -
 * Return: TRUE if there is data still not sent
 *---------------------------------------------------------------------------*/
extern bool_t Uart1_IsTxActive
(
        void
);

/*---------------------------------------------------------------------------
 * Name: Uart2_IsTxActive
 * Description: checks if UART_1 transmitter is active
 * Parameters: -
 * Return: TRUE if there is data still not sent
 *---------------------------------------------------------------------------*/
extern bool_t Uart2_IsTxActive
(
        void
);



/*---------------------------------------------------------------------------
 * Name: Uart1_SetRxCallBack
 * Description: Set the receive side callback function. This function will
 *              be called whenever a byte is received by the driver.
 *              The callback's bytesWaiting parameter is the number of bytes
 *              available in the driver's Rx buffer.
 *              The callback function will be called in interrupt context,
 *              so it should be kept very short.
 *              If the callback pointer is set to NULL, the Rx interrupt will
 *              be turned off, and all data in the driver's Rx buffer will be
 *              discarded.
 * Parameters: [IN] pfCallBack - pointer to callback function
 * Return: -
 *---------------------------------------------------------------------------*/
extern void Uart1_SetRxCallBack
(
        void (*pfCallBack)(void)
);

/*---------------------------------------------------------------------------
 * Name: Uart2_SetRxCallBack
 * Description: Set the receive side callback function. This function will
 *              be called whenever a byte is received by the driver.
 *              The callback's bytesWaiting parameter is the number of bytes
 *              available in the driver's Rx buffer.
 *              The callback function will be called in interrupt context,
 *              so it should be kept very short.
 *              If the callback pointer is set to NULL, the Rx interrupt will
 *              be turned off, and all data in the driver's Rx buffer will be
 *              discarded.
 * Parameters: [IN] pfCallBack - pointer to callback function
 * Return: -
 *---------------------------------------------------------------------------*/
extern void Uart2_SetRxCallBack
(
        void (*pfCallBack)(void)
);

/*---------------------------------------------------------------------------
 * Name: Uart1_SetBaud
 * Description: Sets the UART_1 baud rate
 * Parameters: [IN] baudRate - baudrate value (bps)
 * Return: -
 *---------------------------------------------------------------------------*/
extern void Uart1_SetBaud
(
        uint32_t baudRate
);

/*---------------------------------------------------------------------------
 * Name: Uart2_SetBaud
 * Description: Sets the UART_1 baud rate
 * Parameters: [IN] baudRate - baudrate value (bps)
 * Return: -
 *---------------------------------------------------------------------------*/
extern void Uart2_SetBaud
(
        uint32_t baudRate
);

/*---------------------------------------------------------------------------
 * Name: Uart1_Transmit
 * Description: Transmit bufLen bytes of data from pBuffer over UART_1 port.
 *              It calls *pfCallBack() when the entire buffer has been sent.
 *              Returns FALSE if there are no more available Tx buffer slots,
 *              TRUE otherwise. The caller must ensure that the buffer remains
 *              available until the call back function is called.
 *              pfCallBack must not be NULL.
 *              The callback function will be called in interrupt context,
 *              so it should be kept very short.
 * Parameters: [IN] pBuf - pointer to the buffer to be transmitted
 *             [IN] bufLen - buffer length
 *             [IN] pfCallBack - pointer to the callback function
 * Return: Returns FALSE if there are no more slots in the buffer reference
 *         table
 *---------------------------------------------------------------------------*/
extern bool_t Uart1_Transmit
(
        unsigned char const *pBuf,
        index_t bufLen,
        void (*pfCallBack)(unsigned char const *pBuf)
);

/*---------------------------------------------------------------------------
 * Name: Uart2_Transmit
 * Description: Transmit bufLen bytes of data from pBuffer over UART_2 port.
 *              It calls *pfCallBack() when the entire buffer has been sent.
 *              Returns FALSE if there are no more available Tx buffer slots,
 *              TRUE otherwise. The caller must ensure that the buffer remains
 *              available until the call back function is called.
 *              pfCallBack must not be NULL.
 *              The callback function will be called in interrupt context,
 *              so it should be kept very short.
 * Parameters: [IN] pBuf - pointer to the buffer to be transmitted
 *             [IN] bufLen - buffer length
 *             [IN] pfCallBack - pointer to the callback function
 * Return: Returns FALSE if there are no more slots in the buffer reference
 *         table
 *---------------------------------------------------------------------------*/
extern bool_t Uart2_Transmit
(
        unsigned char const *pBuf,
        index_t bufLen,
        void (*pfCallBack)(unsigned char const *pBuf)
);

/*---------------------------------------------------------------------------
 * Name: Uart1_UngetByte
 * Description: Unget a byte: store the byte so that it will be returned by
 *              the next call to the get byte function.
 *              Also call the Rx callback
 * Parameters: [IN] byte - the byte to be stored
 * Return: -
 *---------------------------------------------------------------------------*/
extern void Uart1_UngetByte
(
        unsigned char byte
);

/*---------------------------------------------------------------------------
 * Name: Uart2_UngetByte
 * Description: Unget a byte: store the byte so that it will be returned by
 *              the next call to the get byte function.
 *              Also call the Rx callback
 * Parameters: [IN] byte - the byte to be stored
 * Return: -
 *---------------------------------------------------------------------------*/
extern void Uart2_UngetByte
(
        unsigned char byte
);

/*---------------------------------------------------------------------------
 * Name: Uart1_RxBufferByteCount
 * Description: Return unread bytes number from UART_1 buffer
 * Parameters: -
 * Return: the number of unread bytes
 *---------------------------------------------------------------------------*/
extern uint32_t Uart1_RxBufferByteCount
(
        void
);

/*---------------------------------------------------------------------------
 * Name: Uart2_RxBufferByteCount
 * Description: Return unread bytes number from UART_2 buffer
 * Parameters: -
 * Return: the number of unread bytes
 *---------------------------------------------------------------------------*/
extern uint32_t Uart2_RxBufferByteCount
(
        void
);

/*---------------------------------------------------------------------------
 * Name: Uart1Isr
 * Description: UART_1 Interrupt Service Routine
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
extern void Uart1Isr
(
        void
);

/*---------------------------------------------------------------------------
 * Name: Uart2Isr
 * Description: UART_2 Interrupt Service Routine
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
extern void Uart2Isr
(
        void
);

/*---------------------------------------------------------------------------
 * Name: Uart1ErrorIsr
 * Description: Clear enabled UART_1 errors
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
extern void Uart1ErrorIsr
(
        void
);

/*---------------------------------------------------------------------------
 * Name: Uart2ErrorIsr
 * Description: Clear enabled UART_2 errors
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
extern void Uart2ErrorIsr
(
        void
);

/*---------------------------------------------------------------------------
 * Name: Uart_ModuleEnableLowPowerWakeup
 * Description: Enables the Receive input active edge, RXEDGIF, to generate interrupt requests.
 *              The receive input active edge detect circuit is still active on low power modes
 *              (wait and stop). An active edge on the receive input brings the CPU out of low
 *              power mode if the interrupt is not masked.
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
extern void Uart_ModuleEnableLowPowerWakeup
(
    void
);

/*---------------------------------------------------------------------------
 * Name: Uart_ModuleDisableLowPowerWakeup
 * Description: Disables the Receive input active edge, RXEDGIF, to generate interrupt requests.
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
extern void Uart_ModuleDisableLowPowerWakeup
(
    void
);

/*---------------------------------------------------------------------------
 * Name: Uart_IsWakeUpSource
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
extern bool_t Uart_IsWakeUpSource
(
    void
);

/*---------------------------------------------------------------------------
 * Name: UartFlowControlISR
 * Description: UART CTS flow control interrupt handler
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
extern void UartFlowControlISR
(
    void
);

/*
 * Declare the default API.
 * If gUart_PortDefault_d is not defined, there is
 * no default API.
 */

#if gUart_PortDefault_d == 1
#define UartX_Transmit              Uart1_Transmit
#define UartX_IsTxActive            Uart1_IsTxActive
#define UartX_SetRxCallBack         Uart1_SetRxCallBack
#define UartX_GetByteFromRxBuffer   Uart1_GetByteFromRxBuffer
#define UartX_SetBaud               Uart1_SetBaud
#define UartX_UngetByte             Uart1_UngetByte
#define UartX_TxCompleteFlag        Uart1_TxCompleteFlag
#define UartX_RxBufferByteCount     Uart1_RxBufferByteCount
#define UartX_DefaultBaud           gUART1_Baudrate_c
#endif

#if gUart_PortDefault_d == 2
#define UartX_Transmit              Uart2_Transmit
#define UartX_IsTxActive            Uart2_IsTxActive
#define UartX_SetRxCallBack         Uart2_SetRxCallBack
#define UartX_GetByteFromRxBuffer   Uart2_GetByteFromRxBuffer
#define UartX_SetBaud               Uart2_SetBaud
#define UartX_UngetByte             Uart2_UngetByte
#define UartX_TxCompleteFlag        Uart2_TxCompleteFlag
#define UartX_RxBufferByteCount     Uart2_RxBufferByteCount
#define UartX_DefaultBaud           gUART2_Baudrate_c
#endif

#endif /* (gUart1_Enabled_d == FALSE) && (gUart2_Enabled_d == FALSE) */

#ifdef __cplusplus
}
#endif

#endif /* UART_INTERFACE_H_ */
