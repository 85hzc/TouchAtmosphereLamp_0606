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

typedef struct
{
    uint8_t RX_flag:1;                //IDLE receive flag
    uint16_t RX_Size;                 //receive length
    uint8_t RX_pData[UART_RX_LEN];    //DMA receive buffer
}USART_RECEIVETYPE;


void Drv_SERIAL_Log(const char *format, ...);


#endif


