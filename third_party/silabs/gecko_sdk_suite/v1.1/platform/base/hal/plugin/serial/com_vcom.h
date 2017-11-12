/***************************************************************************//**
 * @file com_vcom.h
 * @brief VCOM configuration override for COM peripherals
 * @version 0.1.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2017 Silicon Laboratories, Inc, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#if !defined(COM_VCOM_H)
#define COM_VCOM_H

#if BSP_VCOM_USART == HAL_SERIAL_PORT_USART0
#undef BSP_USART0_CTS_PIN
#undef BSP_USART0_CTS_PORT
#undef BSP_USART0_RTS_PIN
#undef BSP_USART0_RTS_PORT
#undef BSP_USART0_RX_PIN
#undef BSP_USART0_RX_PORT
#undef BSP_USART0_TX_PIN
#undef BSP_USART0_TX_PORT
#define BSP_USART0_CTS_PIN     BSP_VCOM_CTS_PIN
#define BSP_USART0_CTS_PORT    BSP_VCOM_CTS_PORT
#define BSP_USART0_RTS_PIN     BSP_VCOM_RTS_PIN
#define BSP_USART0_RTS_PORT    BSP_VCOM_RTS_PORT
#define BSP_USART0_RX_PIN      BSP_VCOM_RX_PIN
#define BSP_USART0_RX_PORT     BSP_VCOM_RX_PORT
#define BSP_USART0_TX_PIN      BSP_VCOM_TX_PIN
#define BSP_USART0_TX_PORT     BSP_VCOM_TX_PORT
#if defined(_USART_ROUTELOC0_MASK)
  #undef BSP_USART0_CTS_LOC
  #undef BSP_USART0_RTS_LOC
  #undef BSP_USART0_RX_LOC
  #undef BSP_USART0_TX_LOC
  #define BSP_USART0_CTS_LOC     BSP_VCOM_CTS_LOC
  #define BSP_USART0_RTS_LOC     BSP_VCOM_RTS_LOC
  #define BSP_USART0_RX_LOC      BSP_VCOM_RX_LOC
  #define BSP_USART0_TX_LOC      BSP_VCOM_TX_LOC
#else
  #undef BSP_USART0_ROUTE_LOC
  #define BSP_USART0_ROUTE_LOC   BSP_VCOM_ROUTE_LOC
#endif
#elif BSP_VCOM_USART == HAL_SERIAL_PORT_USART1
#undef BSP_USART1_CTS_PIN
#undef BSP_USART1_CTS_PORT
#undef BSP_USART1_RTS_PIN
#undef BSP_USART1_RTS_PORT
#undef BSP_USART1_RX_PIN
#undef BSP_USART1_RX_PORT
#undef BSP_USART1_TX_PIN
#undef BSP_USART1_TX_PORT
#define BSP_USART1_CTS_PIN     BSP_VCOM_CTS_PIN
#define BSP_USART1_CTS_PORT    BSP_VCOM_CTS_PORT
#define BSP_USART1_RTS_PIN     BSP_VCOM_RTS_PIN
#define BSP_USART1_RTS_PORT    BSP_VCOM_RTS_PORT
#define BSP_USART1_RX_PIN      BSP_VCOM_RX_PIN
#define BSP_USART1_RX_PORT     BSP_VCOM_RX_PORT
#define BSP_USART1_TX_PIN      BSP_VCOM_TX_PIN
#define BSP_USART1_TX_PORT     BSP_VCOM_TX_PORT
#if defined(_USART_ROUTELOC0_MASK)
  #undef BSP_USART1_CTS_LOC
  #undef BSP_USART1_RTS_LOC
  #undef BSP_USART1_RX_LOC
  #undef BSP_USART1_TX_LOC
  #define BSP_USART1_CTS_LOC     BSP_VCOM_CTS_LOC
  #define BSP_USART1_RTS_LOC     BSP_VCOM_RTS_LOC
  #define BSP_USART1_RX_LOC      BSP_VCOM_RX_LOC
  #define BSP_USART1_TX_LOC      BSP_VCOM_TX_LOC
#else
  #undef BSP_USART1_ROUTE_LOC
  #define BSP_USART1_ROUTE_LOC   BSP_VCOM_ROUTE_LOC
#endif
#elif BSP_VCOM_USART == HAL_SERIAL_PORT_USART2
#undef BSP_USART2_CTS_PIN
#undef BSP_USART2_CTS_PORT
#undef BSP_USART2_RTS_PIN
#undef BSP_USART2_RTS_PORT
#undef BSP_USART2_RX_PIN
#undef BSP_USART2_RX_PORT
#undef BSP_USART2_TX_PIN
#undef BSP_USART2_TX_PORT
#define BSP_USART2_CTS_PIN     BSP_VCOM_CTS_PIN
#define BSP_USART2_CTS_PORT    BSP_VCOM_CTS_PORT
#define BSP_USART2_RTS_PIN     BSP_VCOM_RTS_PIN
#define BSP_USART2_RTS_PORT    BSP_VCOM_RTS_PORT
#define BSP_USART2_RX_PIN      BSP_VCOM_RX_PIN
#define BSP_USART2_RX_PORT     BSP_VCOM_RX_PORT
#define BSP_USART2_TX_PIN      BSP_VCOM_TX_PIN
#define BSP_USART2_TX_PORT     BSP_VCOM_TX_PORT
#if defined(_USART_ROUTELOC0_MASK)
  #undef BSP_USART2_CTS_LOC
  #undef BSP_USART2_RTS_LOC
  #undef BSP_USART2_RX_LOC
  #undef BSP_USART2_TX_LOC
  #define BSP_USART2_CTS_LOC     BSP_VCOM_CTS_LOC
  #define BSP_USART2_RTS_LOC     BSP_VCOM_RTS_LOC
  #define BSP_USART2_RX_LOC      BSP_VCOM_RX_LOC
  #define BSP_USART2_TX_LOC      BSP_VCOM_TX_LOC
#else
  #undef BSP_USART2_ROUTE_LOC
  #define BSP_USART2_ROUTE_LOC   BSP_VCOM_ROUTE_LOC
#endif
#elif BSP_VCOM_USART == HAL_SERIAL_PORT_LEUART0
#undef BSP_LEUART0_CTS_PIN
#undef BSP_LEUART0_CTS_PORT
#undef BSP_LEUART0_RTS_PIN
#undef BSP_LEUART0_RTS_PORT
#undef BSP_LEUART0_RX_PIN
#undef BSP_LEUART0_RX_PORT
#undef BSP_LEUART0_TX_PIN
#undef BSP_LEUART0_TX_PORT
#define BSP_LEUART0_CTS_PIN     BSP_VCOM_CTS_PIN
#define BSP_LEUART0_CTS_PORT    BSP_VCOM_CTS_PORT
#define BSP_LEUART0_RTS_PIN     BSP_VCOM_RTS_PIN
#define BSP_LEUART0_RTS_PORT    BSP_VCOM_RTS_PORT
#define BSP_LEUART0_RX_PIN      BSP_VCOM_RX_PIN
#define BSP_LEUART0_RX_PORT     BSP_VCOM_RX_PORT
#define BSP_LEUART0_TX_PIN      BSP_VCOM_TX_PIN
#define BSP_LEUART0_TX_PORT     BSP_VCOM_TX_PORT
#ifdef _LEUART_ROUTELOC0_MASK
  #undef BSP_LEUART0_CTS_LOC
  #undef BSP_LEUART0_RTS_LOC
  #undef BSP_LEUART0_RX_LOC
  #undef BSP_LEUART0_TX_LOC
  #define BSP_LEUART0_CTS_LOC     BSP_VCOM_CTS_LOC
  #define BSP_LEUART0_RTS_LOC     BSP_VCOM_RTS_LOC
  #define BSP_LEUART0_RX_LOC      BSP_VCOM_RX_LOC
  #define BSP_LEUART0_TX_LOC      BSP_VCOM_TX_LOC
#else
  #undef BSP_LEUART0_ROUTE_LOC
  #define BSP_LEUART0_ROUTE_LOC   BSP_VCOM_ROUTE_LOC
#endif
#elif BSP_VCOM_USART == HAL_SERIAL_PORT_LEUART1
#undef BSP_LEUART1_CTS_PIN
#undef BSP_LEUART1_CTS_PORT
#undef BSP_LEUART1_RTS_PIN
#undef BSP_LEUART1_RTS_PORT
#undef BSP_LEUART1_RX_PIN
#undef BSP_LEUART1_RX_PORT
#undef BSP_LEUART1_TX_PIN
#undef BSP_LEUART1_TX_PORT
#define BSP_LEUART1_CTS_PIN     BSP_VCOM_CTS_PIN
#define BSP_LEUART1_CTS_PORT    BSP_VCOM_CTS_PORT
#define BSP_LEUART1_RTS_PIN     BSP_VCOM_RTS_PIN
#define BSP_LEUART1_RTS_PORT    BSP_VCOM_RTS_PORT
#define BSP_LEUART1_RX_PIN      BSP_VCOM_RX_PIN
#define BSP_LEUART1_RX_PORT     BSP_VCOM_RX_PORT
#define BSP_LEUART1_TX_PIN      BSP_VCOM_TX_PIN
#define BSP_LEUART1_TX_PORT     BSP_VCOM_TX_PORT
#ifdef _LEUART_ROUTELOC0_MASK
  #undef BSP_LEUART1_CTS_LOC
  #undef BSP_LEUART1_RTS_LOC
  #undef BSP_LEUART1_RX_LOC
  #undef BSP_LEUART1_TX_LOC
  #define BSP_LEUART1_CTS_LOC     BSP_VCOM_CTS_LOC
  #define BSP_LEUART1_RTS_LOC     BSP_VCOM_RTS_LOC
  #define BSP_LEUART1_RX_LOC      BSP_VCOM_RX_LOC
  #define BSP_LEUART1_TX_LOC      BSP_VCOM_TX_LOC
#else
  #undef BSP_LEUART1_ROUTE_LOC
  #define BSP_LEUART1_ROUTE_LOC   BSP_VCOM_ROUTE_LOC
#endif
#elif BSP_VCOM_USART == HAL_SERIAL_PORT_USB
#error VCOM over USB not supported currently
#else
#error Selected VCOM port not supported
#endif //BSP_VCOM_USART

#endif //COM_VCOM_H
