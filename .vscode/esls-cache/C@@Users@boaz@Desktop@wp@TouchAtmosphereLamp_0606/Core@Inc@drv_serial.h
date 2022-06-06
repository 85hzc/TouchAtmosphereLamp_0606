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

#define MAX_LOG_LEN     64

void Drv_SERIAL_Log(const char *format, ...);


#endif


