/***************************************************************************//**
 * @file bmp_config.h
 * @brief Bosch Sensortec BMP280 pressure sensor configuration file
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

#ifndef __BMP_CONFIG_H_
#define __BMP_CONFIG_H_

#define BMP_CONFIG_I2C_BUS_TIMEOUT  (1000)
#define BMP_CONFIG_I2C_DEVICE       (I2C0)
#define BMP_CONFIG_I2C_BUS_ADDRESS  (0x77)

#endif /* __BMP_CONFIG_H_ */
