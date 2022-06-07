/**
 ******************************************************************************
 * @file    drv_serial.h
 * @author  MEMS Application Team
 * @version V1.1
 * @date    10-August-2016
 * @brief   drv_i2c_peripheral source file
 ******************************************************************************
 */

#ifndef __DRV_SERIAL_H
#define __DRV_SERIAL_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

#define UART_RX_LEN     128


void Drv_SERIAL_Log(const char *format, ...);


#endif


