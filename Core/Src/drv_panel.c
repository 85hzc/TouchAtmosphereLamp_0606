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
  uint8_t cmd[2];
  uint16_t val = 0;

  if((HAL_GetTick() - tickstart) >= 100)
  {
    tickstart = HAL_GetTick();
    
		memset(cmd, 0, sizeof(cmd));
    //i2c_read(SC09A_ADDR, (char*)cmd, 2);
    val = SC09A_ReadKeyValue();
    Drv_SERIAL_Log("Drv_Panel_Proc [%x %x] %x", cmd[0], cmd[1], val);
/*
    if (cmd[0] & (0x01<<0))
    {
      Drv_SERIAL_Log("D8");
    }
    if (cmd[0] & (0x01<<1))
    {
      Drv_SERIAL_Log("D9");
    }
    if (cmd[0] & (0x01<<2))
    {
      Drv_SERIAL_Log("D10");
    }
    if (cmd[0] & (0x01<<3))
    {
      Drv_SERIAL_Log("D11");
    }
    if (cmd[0] & (0x01<<4))
    {
      Drv_SERIAL_Log("D12");
    }

    if (cmd[1] & (0x80>>0))
    {
      Drv_SERIAL_Log("D7");
    }
    if (cmd[1] & (0x80>>1))
    {
      Drv_SERIAL_Log("D6");
    }
    if (cmd[1] & (0x80>>2))
    {
      Drv_SERIAL_Log("D5");
    }
    if (cmd[1] & (0x80>>3))
    {
      Drv_SERIAL_Log("D4");
    }
*/
  }
}

/* ---------------------------------------------------- */
