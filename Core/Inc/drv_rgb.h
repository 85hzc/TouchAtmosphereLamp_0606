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

#define delay100 {\
\
}

#define delay900 {\
__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");\
__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");\
__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");\
__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");\
__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");\
__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");\
__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");\
__ASM("nop");\
}

#define delay600 {\
__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");\
__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");\
__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");\
__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");\
__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");\
}

#define delay300 {\
__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");\
__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");\
}

#define CUBE_COL_SIZE        (32)

#if 0
#define LINE_1_SIZE          (24)
#define LINE_2_SIZE          (27)
#define LINE_3_SIZE          (10)
#define LINE_4_SIZE          (10)
#define LINE_5_SIZE          (10)
#define LINE_6_SIZE          (10)
#define LINE_7_SIZE          (10)
#else
#define LINE_1_SIZE          (10)
#define LINE_2_SIZE          (12)
#define LINE_3_SIZE          (10)
#define LINE_4_SIZE          (19)
#define LINE_5_SIZE          (21)
#define LINE_6_SIZE          (20)
#define LINE_7_SIZE          (20)
#define LINE_8_SIZE          (20)
#define LINE_9_SIZE          (16)
#endif

#define RGB_IO_1_PIN_H       (RGB_IO_1_GPIO_Port->BSRR = RGB_IO_1_Pin)                        // 输出高电平
#define RGB_IO_1_PIN_L       (RGB_IO_1_GPIO_Port->BSRR = (uint32_t)RGB_IO_1_Pin << 16)        // 输出低电平 
#define RGB_IO_2_PIN_H       (RGB_IO_2_GPIO_Port->BSRR = RGB_IO_2_Pin)                        // 输出高电平
#define RGB_IO_2_PIN_L       (RGB_IO_2_GPIO_Port->BSRR = (uint32_t)RGB_IO_2_Pin << 16)        // 输出低电平 
#define RGB_IO_3_PIN_H       (RGB_IO_3_GPIO_Port->BSRR = RGB_IO_3_Pin)                        // 输出高电平
#define RGB_IO_3_PIN_L       (RGB_IO_3_GPIO_Port->BSRR = (uint32_t)RGB_IO_3_Pin << 16)        // 输出低电平 
#define RGB_IO_4_PIN_H       (RGB_IO_4_GPIO_Port->BSRR = RGB_IO_4_Pin)                        // 输出高电平
#define RGB_IO_4_PIN_L       (RGB_IO_4_GPIO_Port->BSRR = (uint32_t)RGB_IO_4_Pin << 16)        // 输出低电平 
#define RGB_IO_5_PIN_H       (RGB_IO_5_GPIO_Port->BSRR = RGB_IO_5_Pin)                        // 输出高电平
#define RGB_IO_5_PIN_L       (RGB_IO_5_GPIO_Port->BSRR = (uint32_t)RGB_IO_5_Pin << 16)        // 输出低电平 
#define RGB_IO_6_PIN_H       (RGB_IO_6_GPIO_Port->BSRR = RGB_IO_6_Pin)                        // 输出高电平
#define RGB_IO_6_PIN_L       (RGB_IO_6_GPIO_Port->BSRR = (uint32_t)RGB_IO_6_Pin << 16)        // 输出低电平 
#define RGB_IO_7_PIN_H       (RGB_IO_7_GPIO_Port->BSRR = RGB_IO_7_Pin)                        // 输出高电平
#define RGB_IO_7_PIN_L       (RGB_IO_7_GPIO_Port->BSRR = (uint32_t)RGB_IO_7_Pin << 16)        // 输出低电平 
#define RGB_IO_8_PIN_H       (RGB_IO_8_GPIO_Port->BSRR = RGB_IO_8_Pin)                        // 输出高电平
#define RGB_IO_8_PIN_L       (RGB_IO_8_GPIO_Port->BSRR = (uint32_t)RGB_IO_8_Pin << 16)        // 输出低电平 
#define RGB_IO_9_PIN_H       (RGB_IO_9_GPIO_Port->BSRR = RGB_IO_9_Pin)                        // 输出高电平
#define RGB_IO_9_PIN_L       (RGB_IO_9_GPIO_Port->BSRR = (uint32_t)RGB_IO_9_Pin << 16)        // 输出低电平 

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
	
		white_time,
} color_e;

/* Private function prototypes -----------------------------------------------*/

void Drv_RGB_Proc(void);

#endif

