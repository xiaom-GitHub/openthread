from . import halconfig_types as types
from . import halconfig_dependency as dep

name = "UART"
description = "UART"
compatibility = dep.Dependency()  # all
peripheral = 'UART'
enable = {
    "define": "HAL_UART_ENABLE",
    "description": "Enable UART",
}
options = {
    "HAL_UART_BAUD_RATE": {
        "type": "uint32_t",
        "description": "Baud rate",
        "min": "1",
        "max": "4294967295",
        "defaultValue": "115200",
        "longdescription": "Specify the baud rate of the UART. Must be less than refFreq/oversampling as described in the reference manual",
    },
    "HAL_UART_FLOW_CONTROL": {
        "type": "enum",
        "description": "Flow control mode",
        "values": [types.EnumValue('HAL_UART_FLOW_CONTROL_NONE', 'No flow control'),
                   types.EnumValue('HAL_UART_FLOW_CONTROL_SW', 'Xon-Xoff'),
                   types.EnumValue('HAL_UART_FLOW_CONTROL_HW', 'GPIO-based CTS/RTS'),
                   types.EnumValue('HAL_UART_FLOW_CONTROL_HWUART', 'USART-based CTS/RTS',
                                   dependency=dep.Dependency(platform=dep.Platform.SERIES1))],
        "longdescription": "Configure flow control for the UART",
    },
    "HAL_UART_RXSTOP": {
        "type": "uint16_t",
        "description": "Flow control threshold",
        "min": "0",
        "max": "65535",
        "advanced": True,
        "longdescription": "Flow control assert threshold (flow control is asserted once less than the specified amount of bytes are available in the RX buffer)",
    },
    "HAL_UART_RXSTART": {
        "type": "uint16_t",
        "description": "Release flow control threshold",
        "min": "0",
        "max": "65535",
        "advanced": True,
        "longdescription": "Flow control release threshold (flow control is released once more than the specified amount of bytes are available in the RX buffer). Must be a multiple of the assert flow control threshold",
    },
    "HAL_UART_TX_QUEUE_SIZE": {
        "type": "uint16_t",
        "description": "TX buffer size",
        "min": "0",
        "max": "65535",
        "advanced": True,
        "longdescription": "Transmit buffer size.",
    },
    "HAL_UART_RX_QUEUE_SIZE": {
        "type": "uint16_t",
        "description": "RX buffer size",
        "min": "0",
        "max": "65535",
        "advanced": True,
        "longdescription": "Receive buffer size. Must be a multiple of the assert flow control threshold",
    },
    "BSP_UART_TX": {
        "type": types.Pin(signal="TX"),
        "description": "TX pin",
        "longdescription": "Select pin for the TX signal",
    },
    "BSP_UART_RX": {
        "type": types.Pin(signal="RX"),
        "description": "RX pin",
        "longdescription": "Select pin for the RX signal",
    },
}
