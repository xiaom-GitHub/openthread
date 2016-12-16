/******************************************************************************
* Filename: UART.h
*
* Description: UART header file for AMR CORTEX-M4 processor
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
#ifndef __UART_H__
#define __UART_H__

#include "PortConfig.h"
#include "UART_Interface.h"

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/
 
/*
 * Name: NumberOfElements()
 * Description: Number of elements in an array
 */
#ifndef NumberOfElements
#define NumberOfElements(array)     ((sizeof(array) / (sizeof(array[0]))))
#endif

/***************************************************************** 
 *                     MCU SPECIFIC DEFINITIONS                  *
 *****************************************************************/
 
 #if ((MCU_MK60D10 == 1) || (MCU_MK60N512VMD100 == 1) || \
      (MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1) || (MCU_MK20D5 == 1))        
      
    /* Error flags masks */
    #define gUART_PARITY_ERROR_FLAG_c           (0x1u)              
    #define gUART_FRAME_ERROR_FLAG_c            (0x2u)
    #define gUART_NOISE_FLAG_c                  (0x4u)
    #define gUART_OVERRUN_FLAG_c                (0x8u)

    /* Parity masks */
    #define gUART_PARITY_EN_BIT_c               (0x2u)
    #define gUART_PARITY_EVEN_BIT_c             (0x1u)

    /* Data bits mask */
    #define gUART_DATA_BITS_BIT_c               (0x8u)

    /* FIFO flush masks */
    #define gUART_TX_FIFO_FLUSH_c               (0x80u)
    #define gUART_RX_FIFO_FLUSH_c               (0x40u)

    /* TX/RX enable bits mask */
    #define gUART_TX_EN_BIT_c                   (0x8u)
    #define gUART_RX_EN_BIT_c                   (0x4u)

    /* UART RX/TX interrupt masks */
    #define gUART_RX_INT_BIT_c                  (0x20u)
    #define gUART_TX_INT_BIT_c                  (0x80u)

    /* UART RX/TX interrupt enable masks */
    #define gUART_TIE_BIT_c                     (0x40u)
    #define gUART_RIE_BIT_c                     (0x20u)

    /* UART HW flow control masks */
    #define gUART_RXRTSE_BIT_c                  (0x8u)
    #define gUART_TXRTSPOL_BIT_c                (0x4u)
    #define gUART_TXRTSE_BIT_c                  (0x2u)
    #define gUART_TXCTSE_BIT_c                  (0x1u)                      

    /* UART baudrate masks */
    #define gUART_BDH_MASK_c                    (0x001Fu)
    #define gUART_BDL_MASK_c                    (0x00FFu)
    #define gUART_BRFA_MASK_c                   (0x1F)

    #define gUART_TX_FIFO_EN_BIT_c              (0x80)     /* transmit FIFO enable bit */  
    #define gUART_TX_FIFO_MASK_c                (0x70)     /* transmit FIFO buffer depth mask */

    #define gUART_RX_FIFO_EN_BIT_c              (0x8)      /* receive FIFO enable bit */
    #define gUART_RX_FIFO_MASK_c                (0x7)      /* receive FIFO buffer depth mask */

    #define gUART_S1_TDRE_MASK_c                (0x80)     /* TDRE bit mask */
    #define gUART_S1_RDRF_MASK_c                (0x20)     /* RDRF bit mask */
    
#endif
 
#if ((MCU_MK21DN512 == 1) || (MCU_MK21DX256 == 1))
    /* Number of UART modules available on chip */
    #define  gUART_HW_MODULES_AVAILABLE_c       4
    
    /* UART module base pointer */
    #define gUART0_BASE_PTR_c                   UART0_BASE_PTR
    #define gUART1_BASE_PTR_c                   UART1_BASE_PTR
    #define gUART2_BASE_PTR_c                   UART2_BASE_PTR
    #define gUART3_BASE_PTR_c                   UART3_BASE_PTR


    /* UART RX/TX and UART ERROR IRQ */
    #define gUART0_IRQ_NUM_c                    (31)
    #define gUART0_ERR_IRQ_NUM_c                (32)   
    #define gUART1_IRQ_NUM_c                    (33)
    #define gUART1_ERR_IRQ_NUM_c                (34)
    #define gUART2_IRQ_NUM_c                    (35)
    #define gUART2_ERR_IRQ_NUM_c                (36)
    #define gUART3_IRQ_NUM_c                    (37)
    #define gUART3_ERR_IRQ_NUM_c                (38)

    /* UART module clock gating register */
    #define gUART0_SIM_SCGC_REG_c               SIM_SCGC4
    #define gUART1_SIM_SCGC_REG_c               SIM_SCGC4
    #define gUART2_SIM_SCGC_REG_c               SIM_SCGC4
    #define gUART3_SIM_SCGC_REG_c               SIM_SCGC4

    /* UART module clock gating bit */
    #define gUART0_SIM_SCGC_BIT_c               (10)
    #define gUART1_SIM_SCGC_BIT_c               (11)    
    #define gUART2_SIM_SCGC_BIT_c               (12)
    #define gUART3_SIM_SCGC_BIT_c               (13)
#elif ((MCU_MK60D10 == 1) || (MCU_MK60N512VMD100 == 1))
    /* Number of UART modules available on chip */
    #define  gUART_HW_MODULES_AVAILABLE_c       6
    
    /* UART module base pointer */
    #define gUART0_BASE_PTR_c                   UART0_BASE_PTR
    #define gUART1_BASE_PTR_c                   UART1_BASE_PTR
    #define gUART2_BASE_PTR_c                   UART2_BASE_PTR
    #define gUART3_BASE_PTR_c                   UART3_BASE_PTR
    #define gUART4_BASE_PTR_c                   UART4_BASE_PTR
    #define gUART5_BASE_PTR_c                   UART5_BASE_PTR
    
    /* UART RX/TX and UART ERROR IRQ */
    #define gUART0_IRQ_NUM_c                    (45)
    #define gUART0_ERR_IRQ_NUM_c                (46)
    #define gUART1_IRQ_NUM_c                    (47)
    #define gUART1_ERR_IRQ_NUM_c                (48)
    #define gUART2_IRQ_NUM_c                    (49)
    #define gUART2_ERR_IRQ_NUM_c                (50)
    #define gUART3_IRQ_NUM_c                    (51)
    #define gUART3_ERR_IRQ_NUM_c                (52)
    #define gUART4_IRQ_NUM_c                    (53)
    #define gUART4_ERR_IRQ_NUM_c                (54)
    #define gUART5_IRQ_NUM_c                    (55)
    #define gUART5_ERR_IRQ_NUM_c                (56)

    /* UART module clock gating register */
    #define gUART0_SIM_SCGC_REG_c               SIM_SCGC4
    #define gUART1_SIM_SCGC_REG_c               SIM_SCGC4
    #define gUART2_SIM_SCGC_REG_c               SIM_SCGC4
    #define gUART3_SIM_SCGC_REG_c               SIM_SCGC4
    #define gUART4_SIM_SCGC_REG_c               SIM_SCGC1
    #define gUART5_SIM_SCGC_REG_c               SIM_SCGC1

    /* UART module clock gating bit */
    #define gUART0_SIM_SCGC_BIT_c               (10)
    #define gUART1_SIM_SCGC_BIT_c               (11)
    #define gUART2_SIM_SCGC_BIT_c               (12)
    #define gUART3_SIM_SCGC_BIT_c               (13)
    #define gUART4_SIM_SCGC_BIT_c               (10)
    #define gUART5_SIM_SCGC_BIT_c               (11)
#elif (MCU_MK20D5 == 1)    
    /* Number of UART modules available on chip */
    #define  gUART_HW_MODULES_AVAILABLE_c       3
    
    /* UART module base pointer */
    #define gUART0_BASE_PTR_c                   UART0_BASE_PTR
    #define gUART1_BASE_PTR_c                   UART1_BASE_PTR
    #define gUART2_BASE_PTR_c                   UART2_BASE_PTR
    
    /* UART RX/TX and UART ERROR IRQ */
    #define gUART0_IRQ_NUM_c                    (16)
    #define gUART0_ERR_IRQ_NUM_c                (17)
    #define gUART1_IRQ_NUM_c                    (18)
    #define gUART1_ERR_IRQ_NUM_c                (19)
    #define gUART2_IRQ_NUM_c                    (20)
    #define gUART2_ERR_IRQ_NUM_c                (21)

    /* UART module clock gating register */
    #define gUART0_SIM_SCGC_REG_c               SIM_SCGC4
    #define gUART1_SIM_SCGC_REG_c               SIM_SCGC4
    #define gUART2_SIM_SCGC_REG_c               SIM_SCGC4

    /* UART module clock gating bit */
    #define gUART0_SIM_SCGC_BIT_c               (10)
    #define gUART1_SIM_SCGC_BIT_c               (11)
    #define gUART2_SIM_SCGC_BIT_c               (12)    
#endif

#if ((gUart1_c >= gUART_HW_MODULES_AVAILABLE_c) || (gUart2_c >= gUART_HW_MODULES_AVAILABLE_c))
#error "The UART hardware module is not implemented on the selected chip. Please verify gUart1_c and gUart2_c settings"
#endif

/**********
 * UART_1 *
 **********/
#if (gUart1_c == gUART_HW_MOD_0_c)
#define gUART_1_SIM_SCG_c                   gUART0_SIM_SCGC_REG_c           /* UART0 clock gating register */
#define gUART_1_SIM_SCG_BIT_c               gUART0_SIM_SCGC_BIT_c           /* UART0 clock gating bit */
#define gUART_1_IRQ_NUM_c                   gUART0_IRQ_NUM_c                /* UART0 interrupt source number */
#define gUART_1_ERR_IRQ_NUM_c               gUART0_ERR_IRQ_NUM_c            /* UART0 error interrupt source number */
#elif (gUart1_c == gUART_HW_MOD_1_c)
#define gUART_1_SIM_SCG_c                   gUART1_SIM_SCGC_REG_c           /* UART1 clock gating register */
#define gUART_1_SIM_SCG_BIT_c               gUART1_SIM_SCGC_BIT_c           /* UART1 clock gating bit */
#define gUART_1_IRQ_NUM_c                   gUART1_IRQ_NUM_c                /* UART1 interrupt source number */
#define gUART_1_ERR_IRQ_NUM_c               gUART1_ERR_IRQ_NUM_c            /* UART1 error interrupt source number */
#elif (gUart1_c == gUART_HW_MOD_2_c)
#define gUART_1_SIM_SCG_c                   gUART2_SIM_SCGC_REG_c           /* UART2 clock gating register */
#define gUART_1_SIM_SCG_BIT_c               gUART2_SIM_SCGC_BIT_c           /* UART2 clock gating bit */
#define gUART_1_IRQ_NUM_c                   gUART2_IRQ_NUM_c                /* UART2 interrupt source number */
#define gUART_1_ERR_IRQ_NUM_c               gUART2_ERR_IRQ_NUM_c            /* UART2 error interrupt source number */
#elif (gUart1_c == gUART_HW_MOD_3_c)
#define gUART_1_SIM_SCG_c                   gUART3_SIM_SCGC_REG_c           /* UART3 clock gating register */
#define gUART_1_SIM_SCG_BIT_c               gUART3_SIM_SCGC_BIT_c           /* UART3 clock gating bit */
#define gUART_1_IRQ_NUM_c                   gUART3_IRQ_NUM_c                /* UART3 interrupt source number */
#define gUART_1_ERR_IRQ_NUM_c               gUART3_ERR_IRQ_NUM_c            /* UART3 error interrupt source number */
#elif (gUart1_c == gUART_HW_MOD_4_c)
#define gUART_1_SIM_SCG_c                   gUART4_SIM_SCGC_REG_c           /* UART4 clock gating register */
#define gUART_1_SIM_SCG_BIT_c               gUART4_SIM_SCGC_BIT_c           /* UART4 clock gating bit */
#define gUART_1_IRQ_NUM_c                   gUART4_IRQ_NUM_c                /* UART4 interrupt source number */
#define gUART_1_ERR_IRQ_NUM_c               gUART4_ERR_IRQ_NUM_c            /* UART4 error interrupt source number */
#else
#define gUART_1_SIM_SCG_c                   gUART5_SIM_SCGC_REG_c           /* UART5 clock gating register */
#define gUART_1_SIM_SCG_BIT_c               gUART5_SIM_SCGC_BIT_c           /* UART5 clock gating bit */
#define gUART_1_IRQ_NUM_c                   gUART5_IRQ_NUM_c                /* UART5 interrupt source number */
#define gUART_1_ERR_IRQ_NUM_c               gUART5_ERR_IRQ_NUM_c            /* UART5 error interrupt source number */
#endif

/**********
 * UART_2 *
 **********/
#if (gUart2_c == gUART_HW_MOD_0_c)
#define gUART_2_SIM_SCG_c                   gUART0_SIM_SCGC_REG_c           /* UART0 clock gating register */
#define gUART_2_SIM_SCG_BIT_c               gUART0_SIM_SCGC_BIT_c           /* UART0 clock gating bit */
#define gUART_2_IRQ_NUM_c                   gUART0_IRQ_NUM_c                /* UART0 interrupt source number */
#define gUART_2_ERR_IRQ_NUM_c               gUART0_ERR_IRQ_NUM_c            /* UART0 error interrupt source number */
#elif (gUart2_c == gUART_HW_MOD_1_c)
#define gUART_2_SIM_SCG_c                   gUART1_SIM_SCGC_REG_c           /* UART1 clock gating register */
#define gUART_2_SIM_SCG_BIT_c               gUART1_SIM_SCGC_BIT_c           /* UART1 clock gating bit */
#define gUART_2_IRQ_NUM_c                   gUART1_IRQ_NUM_c                /* UART1 interrupt source number */
#define gUART_2_ERR_IRQ_NUM_c               gUART1_ERR_IRQ_NUM_c            /* UART1 error interrupt source number */
#elif (gUart2_c == gUART_HW_MOD_2_c)
#define gUART_2_SIM_SCG_c                   gUART2_SIM_SCGC_REG_c           /* UART2 clock gating register */
#define gUART_2_SIM_SCG_BIT_c               gUART2_SIM_SCGC_BIT_c           /* UART2 clock gating bit */
#define gUART_2_IRQ_NUM_c                   gUART2_IRQ_NUM_c                /* UART2 interrupt source number */
#define gUART_2_ERR_IRQ_NUM_c               gUART2_ERR_IRQ_NUM_c            /* UART2 error interrupt source number */
#elif (gUart2_c == gUART_HW_MOD_3_c)
#define gUART_2_SIM_SCG_c                   gUART3_SIM_SCGC_REG_c           /* UART3 clock gating register */
#define gUART_2_SIM_SCG_BIT_c               gUART3_SIM_SCGC_BIT_c           /* UART3 clock gating bit */
#define gUART_2_IRQ_NUM_c                   gUART3_IRQ_NUM_c                /* UART3 interrupt source number */
#define gUART_2_ERR_IRQ_NUM_c               gUART3_ERR_IRQ_NUM_c            /* UART3 error interrupt source number */
#elif (gUart2_c == gUART_HW_MOD_4_c)
#define gUART_2_SIM_SCG_c                   gUART4_SIM_SCGC_REG_c           /* UART4 clock gating register */
#define gUART_2_SIM_SCG_BIT_c               gUART4_SIM_SCGC_BIT_c           /* UART4 clock gating bit */
#define gUART_2_IRQ_NUM_c                   gUART4_IRQ_NUM_c                /* UART4 interrupt source number */
#define gUART_2_ERR_IRQ_NUM_c               gUART4_ERR_IRQ_NUM_c            /* UART4 error interrupt source number */
#else
#define gUART_2_SIM_SCG_c                   gUART5_SIM_SCGC_REG_c           /* UART5 clock gating register */
#define gUART_2_SIM_SCG_BIT_c               gUART5_SIM_SCGC_BIT_c           /* UART5 clock gating bit */
#define gUART_2_IRQ_NUM_c                   gUART5_IRQ_NUM_c                /* UART5 interrupt source number */
#define gUART_2_ERR_IRQ_NUM_c               gUART5_ERR_IRQ_NUM_c            /* UART5 error interrupt source number */
#endif

/* END OF SECTION: READ ONLY */

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/* 
 * Name: UartReadStatus_t
 * Description: This data type enumerates the possible read operation status 
 */
typedef enum 
{
    gUartReadStatusComplete_c = 0,
    gUartReadStatusCanceled_c ,
    gUartReadStatusError_c,
    gUartReadStatusMax_c
} UartReadStatus_t;

/* 
 * Name: UartErr_t
 * Description: This data type enumerates the UART API calls return values 
 */
typedef enum {
    gUartErrNoError_c = 0,                 
    gUartErrUartAlreadyOpen_c,                 
    gUartErrUartNotOpen_c,
    gUartErrReadOngoing_c,
    gUartErrWriteOngoing_c,
    gUartErrInvalidClock_c,
    gUartErrNullPointer_c,
    gUartErrInvalidNrBytes_c,
    gUartErrInvalidBaudrate_c,
    gUartErrInvalidParity_c,
    gUartErrInvalidStop_c,
    gUartErrInvalidCTS_c,
    gUartErrInvalidThreshold_c,
    gUartErrWrongUartNumber_c,    
    gUartErrMax_c
} UartErr_t;

/*
 * Name: UartConfig_t
 * Description: This data type describes the configuration of the UART module
 */
typedef struct 
{    
    uint32_t	            UartBaudRate;
    UartParityMode_t 	    UartParity;
    UartDataBits_t   	    UartDataBits;
    bool_t                  UartHWFlowCtrl;       
    UartRTSActiveState_t    UartRTSActiveState;    
} UartConfig_t;

/*
 * Name: UartReadErrorFlags_t
 * Description: This data type describes the possible errors passed to the read callback function
 */
typedef struct 
{
    uint32_t   UartRecvOverrunError:1;
    uint32_t   UartParityError:1;
    uint32_t   UartFrameError:1;
    uint32_t   UartNoiseError:1;
    uint32_t   Reserved:4;
} UartReadErrorFlags_t;

/*
 * Name: UartReadCallbackArgs_t
 * Description: This data type describes the parameter passed to the read callback function 
 * Note: Do not modify the order of the members in the structure,  because it is optimized for lower space consumption
 * 
 */
typedef struct 
{
    UartReadStatus_t            UartReadStatus;
    uint16_t                    UartNumberBytesReceived;    
    UartReadErrorFlags_t        UartReadError;
} UartRxStatus_t;

/* 
 * Name: SciTxBufRef_t
 * Description: The transmit code keeps a circular list of buffers to be sent.
 *              Each SCI port has it's own list.
 */
typedef struct SciTxBufRef_tag 
{
    unsigned char const *pBuf;
    void (*pfCallBack)(unsigned char const *pBuf);
} SciTxBufRef_t;

/*
 * Name: UartModule_t
 * Description: This data type describes the internal variables used for one UART driver instance
 */
typedef struct {
    /* Variable which indicates if the UART peripheral is opened */ 
    uint32_t UartIsOpen:1; 
    /* variable which indicates if a write operation is ongoing */ 
    volatile uint32_t UartWriteOnGoing:1;
    /* variable which indicates if a read operation is ongoing */ 
    volatile uint32_t UartReadOnGoing:1;
    /* reserved for alignment purposes */
    uint32_t Reserved_01:5;
    /* variable which indicates the RX FIFO threshold */
    uint32_t UartRxThreshold:8; 
    /* variable which indicates the TX FIFO threshold */
    uint32_t UartTxThreshold:8;   
    /* pointer to a structure which maps over the UART hardware register */
    UART_MemMapPtr  UartRegs;
    /* variable which contains status of the RX operation(s) */ 
    UartRxStatus_t UartRxStatus;    
    /* pointer to UART driver read buffer */
    uint8_t* pUartRxBuffer;    
    /* variable which indicates the number of characters the UART driver should receive */
    uint32_t UartNumberCharsToReceive;    
    /* variable which contains the value of the UART status register */
    uint32_t UartUstatRegister1;
    uint32_t UartUstatRegister2;
    /* variable which contains the value of the platform clock in KHz */
    uint32_t UartPlatformClock;    
    /* Rx circular buffer leading (head) index */
    uint32_t UartBufferLeadIndex;
    /* Rx circular buffer trailing (tail) index */
    uint32_t UartBufferTrailIndex;
    /* unread bytes count within the Rx circular buffer */
    uint32_t UartUnreadBytesNumber;    
    /* UART RX FIFO size */
    uint8_t  UartRxFifoSize;
    /* UART TX FIFO size */
    uint8_t	 UartTxFifoSize;
} UartModule_t;

#ifdef __cplusplus
}
#endif

#endif /* __UART_H__ */
