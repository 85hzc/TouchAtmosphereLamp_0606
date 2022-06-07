/**
 ******************************************************************************
 * @file    drv_rgb.c
 * @author  MEMS Application Team
 * @version V1.1
 * @date    10-August-2016
 * @brief   drv_rgb source file
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "drv_rgb.h"

/* Private variables ---------------------------------------------------------*/
static uint32_t tickstart;
uint8_t                         buff_G[IO_SIZE][CHIP_SIZE];
uint8_t                         buff_R[IO_SIZE][CHIP_SIZE];
uint8_t                         buff_B[IO_SIZE][CHIP_SIZE];

/* Private function prototypes -----------------------------------------------*/
void Send_s203_oneString(uint8_t *r,uint8_t *g,uint8_t *b);
void Send_e133_oneString(uint8_t *r,uint8_t *g,uint8_t *b);

void Drv_RGB_Proc(void)
{
  if((HAL_GetTick() - tickstart) >= 10)
  {
    //tickstart = HAL_GetTick();
    memset(buff_R[0], 0xff, CHIP_SIZE);
    memset(buff_G[0], 0xff, CHIP_SIZE);
    memset(buff_B[0], 0xff, CHIP_SIZE);
    Send_s203_oneString(buff_R[0], buff_G[0], buff_B[0]);
    //Send_e133_oneString(buff_R[0], buff_G[0], buff_B[0]);
  }
}


void delayns_100()
{
    __ASM("nop");
}

void delayns_300()
{
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
}

void delayns_600()
{
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
}

void delayns_900()
{
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
}

void Send_8bits1(uint8_t dat)
{
    uint8_t i;

    //RGB_IO_1_PIN_L;
    //delayns_100();
    for(i=0;i<8;i++)
    {
        if(dat & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_1_PIN_H;
            delayns_900();
            RGB_IO_1_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_1_PIN_H;
            delayns_300();
            RGB_IO_1_PIN_L;
            delayns_600();
        }
        dat=dat<<1;
    }
}

//IO 1
void Send_s203_oneString(uint8_t *r,uint8_t *g,uint8_t *b)
{
    uint8_t i=0;

    for(i=0;i<CHIP_SIZE;i++)
    {
        __disable_irq();
        Send_8bits1(r[i]);
        Send_8bits1(g[i]);
        Send_8bits1(b[i]);
        __enable_irq();
    }
}

/*/IO 2-7
void Send_e133_oneString(uint8_t *r,uint8_t *g,uint8_t *b)
{
    uint8_t i=0;

    for(i=0;i<CHIP_SIZE;i++)
    {
        __disable_irq();
        Send_8bits2(b[i]);
        Send_8bits2(g[i]);
        Send_8bits2(r[i]);
        __enable_irq();
    }
}
*/
