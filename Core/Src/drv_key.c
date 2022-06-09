/**
 ******************************************************************************
 * @file    drv_key.c
 * @author  MEMS Application Team
 * @version V1.1
 * @date    10-August-2016
 * @brief   drv_key source file
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "drv_key.h"
#include "drv_led.h"


/* Private variables ---------------------------------------------------------*/
static uint32_t tickstart;

extern uint8_t key_cmd_flag;

/* Private function prototypes -----------------------------------------------*/


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin==KEY_SCAN_Pin)     //判断中断由哪一个引脚触发
	{
		//press
		if(HAL_GPIO_ReadPin(KEY_SCAN_GPIO_Port,KEY_SCAN_Pin)==0)
		{
			static uint32_t lasttime = 0;

			if (HAL_GetTick() - lasttime > 200)
			{
				key_cmd_flag = 10;
				DRV_LED2_TOGGLE;
				lasttime = HAL_GetTick();
			}
			__HAL_GPIO_EXTI_CLEAR_IT(KEY_SCAN_Pin);    //清除标志位！！
		}
		/*if(HAL_GPIO_ReadPin(KEY_SCAN_GPIO_Port,KEY_SCAN_Pin)==1)
		{
			//HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,1);
			DRV_LED2_TOGGLE;
			__HAL_GPIO_EXTI_CLEAR_IT(KEY_SCAN_Pin);    //清除标志位！！
		}*/
	}
}

