/**
 ******************************************************************************
 * @file    drv_serial.c
 * @author  MEMS Application Team
 * @version V1.1
 * @date    10-August-2016
 * @brief   drv_serial source file
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "drv_serial.h"
#include "stdarg.h"
#include "string.h"

volatile uint8_t rx1_len = 0;  //接收一帧数据的长度
volatile uint8_t rec1_end_flag = 0; //一帧数据接收完成标志
USART_RECEIVETYPE UsartType2;

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

/* Private function prototypes -----------------------------------------------*/
//static uint8_t Drv_SERIAL_Write(uint8_t * pData, uint32_t Timeout);
static uint8_t Drv_SERIAL_Read(uint8_t * pData, uint32_t Timeout);



void UsartReceive_IDLE(UART_HandleTypeDef *huart)
{
    uint32_t temp;

    if((__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE) != RESET))
    {
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        HAL_UART_DMAStop(huart);
        temp = huart->hdmarx->Instance->CNDTR;

        if(huart->Instance == USART2)
        {
            UsartType2.RX_Size = UART_RX_LEN - temp;
            UsartType2.RX_flag = 1;
            HAL_UART_Receive_DMA(huart, UsartType2.RX_pData, UART_RX_LEN);
        }
    }
}

/**
  * @brief  The drv_serial init.
  */
void Drv_SERIAL_Proc(void)
{
  if(UsartType2.RX_flag)
  {
      UsartType2.RX_flag = 0;

      #if 0
      printf("BLE rx len=%d  [",UsartType2.RX_Size);
      for(int i=0; i<UsartType2.RX_Size; i++)
      {
          printf("0x%02x ",UsartType2.RX_pData[i]);
      }
      printf("]\r\n");
      #endif
      HAL_UART_Transmit(&huart2, UsartType2.RX_pData, UsartType2.RX_Size, 0xffff);
      Drv_SERIAL_Log("Drv_SERIAL_Proc rx len = %d",UsartType2.RX_Size);
  }
  
}

void Drv_SERIAL_Log(const char *format, ...)
{
  static char log[UART_RX_LEN];
  char log_len;
  va_list args;

  memset((uint8_t*)log, 0, UART_RX_LEN);
  va_start(args, format);  
  vsnprintf(log, UART_RX_LEN, format, args);
  va_end(args);

  log_len = strlen(log);
  if (log_len){
    log[log_len++] = '\n';
    HAL_UART_Transmit(&huart2, (uint8_t*)log, log_len, HAL_MAX_DELAY);
    //HAL_UART_Transmit_DMA(&huart2, (uint8_t*)log, log_len);
  }
}
/*
static uint8_t Drv_SERIAL_Write(uint8_t * pData, uint32_t Timeout)
{
  return (uint8_t)HAL_UART_Transmit(&huart2, pData, UART_RX_LEN, Timeout);
}
*/


/* ---------------------------------------------------- */
