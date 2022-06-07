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
static uint32_t tickstart;

void Drv_Panel_Proc(void)
{
  uint8_t cmd[2] = 0;

  if((HAL_GetTick() - tickstart) >= 50)
  {
    tickstart = HAL_GetTick();
    
    i2c_read(SC09A_ADDR, (char*)cmd, 2);
    Drv_SERIAL_Log("Drv_Panel_Proc [%x %x]", cmd[0], cmd[1]);
  }
}

/* ---------------------------------------------------- */
