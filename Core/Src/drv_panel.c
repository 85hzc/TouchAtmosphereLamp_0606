/**
 ******************************************************************************
 * @file    drv_panel.c
 * @author  MEMS Application Team
 * @version V1.1
 * @date    10-August-2016
 * @brief   drv_panel source file
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "drv_panel.h"
#include "string.h"

#define SC09A_ADDR    0x40

uint8_t key_cmd_flag = 0;
static uint32_t tickstart;

void Drv_Panel_Proc(void)
{
  uint16_t val = 0;

  if((HAL_GetTick() - tickstart) >= 100)
  {
    tickstart = HAL_GetTick();
    
    //i2c_read(SC09A_ADDR, (char*)cmd, 2);
    val = SC09A_ReadKeyValue();
    if (val != 0xffff)
    {
      if (!(val & (0x01<<12))) //CIN0
      {
        Drv_SERIAL_Log("CIN0");
        key_cmd_flag = 12;
      }
      if (!(val & (0x01<<11))) //CIN1
      {
        Drv_SERIAL_Log("CIN1");
        key_cmd_flag = 11;
      }
      if (!(val & (0x01<<10))) //CIN2
      {
        Drv_SERIAL_Log("CIN2");
        key_cmd_flag = 10;
      }
      if (!(val & (0x01<<9))) //CIN3
      {
        Drv_SERIAL_Log("CIN3");
        key_cmd_flag = 9;
      }
      if (!(val & (0x01<<8))) //CIN4
      {
        Drv_SERIAL_Log("CIN4");
        key_cmd_flag = 8;
      }
    }
  }
}

/* ---------------------------------------------------- */
