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
#include "drv_serial.h"
#include "stdarg.h"
#include "string.h"

volatile uint8_t rx1_len = 0;  //接收一帧数据的长度
volatile uint8_t rec1_end_flag = 0; //一帧数据接收完成标志
uint8_t rx1_buffer[MAX_LOG_LEN]={0};  //接收数据缓存数组


/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

/* Private function prototypes -----------------------------------------------*/
//static uint8_t Drv_SERIAL_Write(uint8_t * pData, uint32_t Timeout);
static uint8_t Drv_SERIAL_Read(uint8_t * pData, uint32_t Timeout);
void Usart2_Handle();

/**
  * @brief  The drv_serial init.
  */
void Drv_SERIAL_Proc(void)
{
  /* read the uart buffer... */
  memset(rx1_buffer, 0, MAX_LOG_LEN);
  if (HAL_OK == Drv_SERIAL_Read(rx1_buffer, 0))
  {
    Drv_SERIAL_Log("[%s]", rx1_buffer);
  }
  Drv_SERIAL_Log("Drv_SERIAL_Proc %d", rec1_end_flag);
  /*
  if(rec1_end_flag)  //判断是否接收到1帧数据
  {
    Usart2_Handle();  //前往数据处理函数处理接收到的数据。
  }
  */
}

void Drv_SERIAL_Log(const char *format, ...)
{
  static char log[MAX_LOG_LEN];
  char log_len;
  va_list args;

  memset((uint8_t*)log, 0, MAX_LOG_LEN);
  va_start(args, format);  
  vsnprintf(log, MAX_LOG_LEN, format, args);
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
  return (uint8_t)HAL_UART_Transmit(&huart2, pData, MAX_LOG_LEN, Timeout);
}
*/
static uint8_t Drv_SERIAL_Read(uint8_t * pData, uint32_t Timeout)
{
  uint8_t re = HAL_ERROR, flag = 0;

  //flag = __HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE);
  //if (flag == 1)
  {
    re = (uint8_t)HAL_UART_Receive(&huart2, pData, 5, Timeout);
    if (re == (uint8_t)HAL_OK)
    {
      
    }
    Drv_SERIAL_Log("flag 1 re %d", re);
    return HAL_OK;
  }
  Drv_SERIAL_Log("re %d", re);
  return re;
}
#if 0
void Usart2_IDLE(void)      //USART1的IDLE接收
{   
	uint32_t tmp_flag = 0;   
	uint32_t temp;
  tmp_flag =__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE); //获取IDLE标志位
  if((tmp_flag != RESET))//idle标志被置位
  {
    __HAL_UART_CLEAR_IDLEFLAG(&huart2);//清除标志位
    HAL_UART_DMAStop(&huart2); //  停止DMA传输，防止
    temp = __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);// 获取DMA中未传输的数据个数   
    rx1_len = MAX_LOG_LEN - temp; //总计数减去未传输的数据个数，得到已经接收的数据个数
    rec1_end_flag = 1;	// 接受完成标志位置1	
  }
}

void DMA_Usart2_Send(uint8_t *buf,uint8_t len) //串口发送封装
{   
	if(HAL_UART_Transmit_DMA(&huart2,buf,len)!= HAL_OK) //判断是否发送正常，如果出现异常则进入异常中断函数
	{
  		Error_Handler();
 	}
}

void Usart2_Handle()     //USART1对接收的一帧数据进行处理
{
   DMA_Usart2_Send(rx1_buffer, rx1_len);  //将接收到的数据回发给发送端
   rx1_len = 0;//清除计数
   rec1_end_flag = 0;//清除接收结束标志位
   HAL_UART_Receive_DMA(&huart2, rx1_buffer, MAX_LOG_LEN);//重新打开DMA接收
}
#endif



/* ---------------------------------------------------- */
