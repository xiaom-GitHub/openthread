/***************************************************************************//**
 * @file si7210_config.h
 * @brief Si7210 Hall Effect Sensor configuration file
 * @version 5.2.1
 *******************************************************************************
 * # License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __SI7210_CONFIG_H_
#define __SI7210_CONFIG_H_

#define SI7210_CONFIG_I2C_DEVICE        I2C0
#define SI7210_CONFIG_I2C_BUS_TIMEOUT   1000
#define SI7210_CONFIG_I2C_BUS_ADDRESS   0x30
#define SI7210_CONFIG_DEVICE_ID         0x11

#define SI7210_CONFIG_PORT_OUT1         gpioPortB
#define SI7210_CONFIG_PIN_OUT1          11

#endif /* __SI7210_CONFIG_H_ */
