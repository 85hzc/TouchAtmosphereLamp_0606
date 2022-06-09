/**
 ******************************************************************************
 * @file    drv_rgb.h
 * @author  MEMS Application Team
 * @version V1.1
 * @date    10-August-2016
 * @brief   drv_rgb source file
 ******************************************************************************
 */

#ifndef __DRV_RGB_H
#define __DRV_RGB_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "main.h"

#define LINE_1_SIZE          (24)
#define LINE_2_SIZE          (27)
#define LINE_3_SIZE          (26)
#define LINE_4_SIZE          (26)
#define LINE_5_SIZE          (26)
#define LINE_6_SIZE          (26)
#define LINE_7_SIZE          (25)

#define RGB_IO_1_PIN_H       (GPIOA->BSRR = RGB_IO_1_Pin)                        // 输出高电平
#define RGB_IO_1_PIN_L       (GPIOA->BSRR = (uint32_t)RGB_IO_1_Pin << 16)        // 输出低电平 
#define RGB_IO_5_PIN_H       (GPIOA->BSRR = RGB_IO_5_Pin)                        // 输出高电平
#define RGB_IO_5_PIN_L       (GPIOA->BSRR = (uint32_t)RGB_IO_5_Pin << 16)        // 输出低电平 
#define RGB_IO_6_PIN_H       (GPIOA->BSRR = RGB_IO_6_Pin)                        // 输出高电平
#define RGB_IO_6_PIN_L       (GPIOA->BSRR = (uint32_t)RGB_IO_6_Pin << 16)        // 输出低电平 

#define RGB_IO_2_PIN_H       (GPIOB->BSRR = RGB_IO_2_Pin)                        // 输出高电平
#define RGB_IO_2_PIN_L       (GPIOB->BSRR = (uint32_t)RGB_IO_2_Pin << 16)        // 输出低电平 
#define RGB_IO_3_PIN_H       (GPIOB->BSRR = RGB_IO_3_Pin)                        // 输出高电平
#define RGB_IO_3_PIN_L       (GPIOB->BSRR = (uint32_t)RGB_IO_3_Pin << 16)        // 输出低电平 
#define RGB_IO_4_PIN_H       (GPIOB->BSRR = RGB_IO_4_Pin)                        // 输出高电平
#define RGB_IO_4_PIN_L       (GPIOB->BSRR = (uint32_t)RGB_IO_4_Pin << 16)        // 输出低电平 
#define RGB_IO_7_PIN_H       (GPIOB->BSRR = RGB_IO_7_Pin)                        // 输出高电平
#define RGB_IO_7_PIN_L       (GPIOB->BSRR = (uint32_t)RGB_IO_7_Pin << 16)        // 输出低电平 

//color data struct
typedef struct{
 unsigned char R;
 unsigned char G;
 unsigned char B;
 int16_t       l;
} COLOR_RGB;
 
//hsv data struct
typedef struct{
 float H;
 float S;
 float V;
} COLOR_HSV;

typedef enum
{
    red_time,
    green_time,
    blue_time,

    yellow_time,
    pink_time,
} color_e;

/* Private function prototypes -----------------------------------------------*/

void Drv_RGB_Proc(void);

#endif

