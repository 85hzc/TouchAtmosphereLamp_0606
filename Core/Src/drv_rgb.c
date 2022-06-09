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
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "drv_rgb.h"

/* Private variables ---------------------------------------------------------*/
static uint32_t tickstart;
#if 0
uint8_t         buff_G[IO_SIZE][CHIP_SIZE];
uint8_t         buff_R[IO_SIZE][CHIP_SIZE];
uint8_t         buff_B[IO_SIZE][CHIP_SIZE];
#else
volatile uint8_t         buff_G=0xff, buff_R=0xff, buff_B=0xff;
#endif
volatile COLOR_RGB       rgb_v;

extern uint8_t key_cmd_flag;

/* Private function prototypes -----------------------------------------------*/
void Send_s203_line1(uint8_t r,uint8_t g,uint8_t b);
void Send_e133_line2(uint8_t r,uint8_t g,uint8_t b);
void Send_e133_line3(uint8_t r,uint8_t g,uint8_t b);
void Send_e133_line4(uint8_t r,uint8_t g,uint8_t b);
void Send_e133_line5(uint8_t r,uint8_t g,uint8_t b);
void Send_e133_line6(uint8_t r,uint8_t g,uint8_t b);
void Send_e133_line7(uint8_t r,uint8_t g,uint8_t b);

/**------------------------------------------------------------------------------------------------
  * @brief  : This is return  a and b of minimum value
  * @param  : None
  * @retval : None
  *----------------------------------------------------------------------------------------------*/
static float minValue(float a,float b)  //calc minimum value
{ 
	float temp = b;
	if(a < temp)
		temp = a;
	return temp;
}


/**------------------------------------------------------------------------------------------------
  * @brief  : This is return  a and b of maximum value
  * @param  : None
  * @retval : None
  *----------------------------------------------------------------------------------------------*/
static float maxValue(float a,float b) //calc maximum value
{ 
	float temp = b;
	if(a > temp)
		temp = a;
	return temp;
}

/**------------------------------------------------------------------------------------------------
  * @brief  : This is RGB to HSV convert function
  * @param  : None
  * @retval : None
  *----------------------------------------------------------------------------------------------*/
static void RGB_TO_HSV(const COLOR_RGB* input,COLOR_HSV* output)  // convert RGB value to HSV value 
{
    float r,g,b,minRGB,maxRGB,deltaRGB;

    r = input->R/255.0f;
    g = input->G/255.0f;
    b = input->B/255.0f;
    minRGB = minValue(r,minValue(g,b));
    maxRGB = maxValue(r,maxValue(g,b));
    deltaRGB = maxRGB - minRGB;

    output->V = maxRGB;
    if(maxRGB != 0.0f)
        output->S = deltaRGB / maxRGB;
    else
        output->S = 0.0f;
    
    if (output->S <= 0.0f)
    {
        output->H = 0.0f;
    }
    else
    {
        if (r == maxRGB)
        {
            output->H = (g-b)/deltaRGB;
        }
        else
        {
            if (g == maxRGB)
            {
                output->H = 2.0f + (b-r)/deltaRGB;
            }
            else
            {
                if (b == maxRGB)
                {
                    output->H = 4.0f + (r-g)/deltaRGB;
                }
            }
        }
        output->H = output->H * 60.0f;
        if (output->H < 0.0f)
        {
          output->H += 360;
        }
        output->H /= 360;
    }
}


 /**------------------------------------------------------------------------------------------------
  * @brief  : This is HSV to RGB convert function
  * @param  : None
  * @retval : None
  *----------------------------------------------------------------------------------------------*/
static void HSV_TO_RGB(COLOR_HSV* input,COLOR_RGB* output)  //convert HSV value to RGB value
{
    float R,G,B;
    int k;
    float aa,bb,cc,f;

    if (input->S <= 0.0f)
        R = G = B = input->V;
    else
    {
        if (input->H == 1.0f)
            input->H = 0.0f;
        input->H *= 6.0f;
        k = (int)floor(input->H);
        f = input->H - k;
        aa = input->V * (1.0f - input->S);
        bb = input->V * (1.0f - input->S * f);
        cc = input->V * (1.0f -(input->S * (1.0f - f)));
        switch(k)
        {
            case 0:
                R = input->V; 
                G = cc; 
                B =aa;
                break;
            case 1:
                R = bb; 
                G = input->V;
                B = aa;
                break;
            case 2:
                R =aa;
                G = input->V;
                B = cc;
                break;
            case 3:
                R = aa;
                G = bb;
                B = input->V;
                break;
            case 4:
                R = cc;
                G = aa;
                B = input->V;
                break;
            case 5:
                R = input->V;
                G = aa;
                B = bb;
                break;
        }
    }
    output->R = (unsigned char)(R * 255);
    output->G = (unsigned char)(G * 255);
    output->B = (unsigned char)(B * 255);
}


  /**---------------------------------------------------------------------------
  * @brief  : This is brightness adjusting function
  * @param  : None
  * @retval : None
  *-----------------------------------------------------------------------------*/
void adjustBrightness(int16_t step)             //adjust RGB brightness
{
    COLOR_HSV hsv_v;
    
    RGB_TO_HSV(&rgb_v,&hsv_v);

    rgb_v.l += step;
    if(rgb_v.l <= 0)
    {
      rgb_v.l = 1;
    }
    else if(rgb_v.l >= 100)
    {
      rgb_v.l = 100;
    }
    hsv_v.V = rgb_v.l /100.0;
    HSV_TO_RGB(&hsv_v,&rgb_v);
}


void ColorGradualDimming(uint8_t Red1, uint8_t Green1, uint8_t Blue1)
{
    static unsigned char Red0 = 0, Green0 = 0, Blue0 = 0;  // 起始三原色
    static float f_red0 = 0.0, f_green0 = 0.0, f_blue0 = 0.0;
    int  RedMinus, GreenMinus, BlueMinus; // 颜色差（color1 - color0）
    unsigned char NStep;              // 需要几步
    float    RedStep, GreenStep, BlueStep;    // 各色步进值
    uint8_t stepgo;

    // 计算需要多少步（取差值的最大值）
    RedMinus   = Red1 - Red0;
    GreenMinus = Green1 - Green0;
    BlueMinus  = Blue1 - Blue0;

    NStep = ( abs(RedMinus) > abs(GreenMinus) ) ? abs(RedMinus):abs(GreenMinus);
    NStep = ( NStep > abs(BlueMinus) ) ? NStep:abs(BlueMinus);

    // 计算出各色步进值
    RedStep   = (float)RedMinus   / NStep;
    GreenStep = (float)GreenMinus / NStep;
    BlueStep  = (float)BlueMinus  / NStep;

    Drv_SERIAL_Log("rgb step:%f,%f,%f", RedStep, GreenStep, BlueStep);
    // 渐变开始
    for ( stepgo = 0; stepgo < NStep; stepgo++ )
    {
        /*
        Red0    += (int)(RedStep);
        Green0  += (int)(GreenStep);
        Blue0   += (int)(BlueStep);
        */
        f_red0      += RedStep;
        f_green0    += GreenStep;
        f_blue0     += BlueStep;

        Red0        = (int)(f_red0);
        Green0      = (int)(f_green0);
        Blue0       = (int)(f_blue0);

        //Drv_SERIAL_Log("step:%d [%d,%d,%d]", stepgo, Red0, Green0, Blue0);
        Send_s203_line1(Red0, Green0, Blue0);
        Send_e133_line2(Red0, Green0, Blue0);
        Send_e133_line3(Red0, Green0, Blue0);
        Send_e133_line4(Red0, Green0, Blue0);
        Send_e133_line5(Red0, Green0, Blue0);
        Send_e133_line6(Red0, Green0, Blue0);
        Send_e133_line7(Red0, Green0, Blue0);
        HAL_Delay(1);
    }

    // 记录当前颜色
    Red0   = Red1;
    Green0 = Green1;
    Blue0  = Blue1;
}

void Drv_RGB_Init(void)
{
    rgb_v.R = buff_R;
    rgb_v.G = buff_G;
    rgb_v.B = buff_B;
    rgb_v.l = 100;
}

typedef enum
{
    red_time,
    green_time,
    blue_time,

    yellow_time,
    pink_time,

} color_e;

bool switch_on_off = 0;
void Drv_RGB_Proc(void)
{
    static color_e up_type = red_time;
    static color_e down_type = yellow_time;

  if(((HAL_GetTick() - tickstart) >= 50) && key_cmd_flag)
  {
    tickstart = HAL_GetTick();

    switch(key_cmd_flag)
    {
      case 12:  //up
      {
        switch_on_off = true;
        Drv_SERIAL_Log("switch RGB");
        switch (up_type)
        {
            case red_time:
                up_type = green_time;
                buff_G = 0;
                buff_R = 0xff;
                buff_B = 0;
                break;

            case green_time:
                up_type = blue_time;
                buff_G = 0xff;
                buff_R = 0;
                buff_B = 0;
                break;

            case blue_time:
            default:
                up_type = red_time;
                buff_G = 0;
                buff_R = 0;
                buff_B = 0xff;
                break;
        }
        rgb_v.G = buff_G;
        rgb_v.R = buff_R;
        rgb_v.B = buff_B;
        break;
      }
      case 8: //right
      {
        switch_on_off = true;
        Drv_SERIAL_Log("turn up");
        adjustBrightness(20);
        buff_G = rgb_v.G;
        buff_R = rgb_v.R;
        buff_B = rgb_v.B;
        break;
      }
      case 11:  //left
      {
        switch_on_off = true;
        Drv_SERIAL_Log("turn down");
        adjustBrightness(-20);
        buff_G = rgb_v.G;
        buff_R = rgb_v.R;
        buff_B = rgb_v.B;
        break;
      }
      case 10:  //on/off
      {
        switch_on_off = !switch_on_off;
        Drv_SERIAL_Log("%s", switch_on_off==true ? "on":"off");
        if (switch_on_off)
        {
            buff_G = rgb_v.G;
            buff_R = rgb_v.R;
            buff_B = rgb_v.B;
        }
        else
        {
            buff_G = 0;
            buff_R = 0;
            buff_B = 0;
        }
        break;
      }
      case 9: //down
      {
        switch_on_off = true;
        Drv_SERIAL_Log("switch YPG");
        switch (down_type)
        {
            case yellow_time:
                down_type = pink_time;
                buff_G = 0xff;
                buff_R = 0xff;
                buff_B = 0;
                break;

            case pink_time:
                down_type = green_time;
                buff_G = 0x14;
                buff_R = 0xff;
                buff_B = 0x93;
                break;

            case green_time:
            default:
                down_type = yellow_time;
                buff_G = 0xff;
                buff_R = 0;
                buff_B = 0;
                break;
        }
        rgb_v.G = buff_G;
        rgb_v.R = buff_R;
        rgb_v.B = buff_B;
        break;
      }

      default:
        break;
    }
    key_cmd_flag = 0;
    Drv_SERIAL_Log("RGB 0x%x 0x%x 0x%x", buff_R, buff_G, buff_B);
    ColorGradualDimming(buff_R, buff_G, buff_B);
  }
}

#pragma GCC push_options
#pragma GCC optimize ("O0")

void delayns_100()
{
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
}

void delayns_300()
{
    __ASM("nop");
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
}

void Send_8bits1(uint8_t dat1, uint8_t dat2, uint8_t dat3)
{
    uint8_t i;

    //RGB_IO_1_PIN_L;
    //delayns_100();
    for(i=0;i<8;i++)
    {
        if(dat1 & 0x80)//1,for "1",H:0.8us,L:0.45us;
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
        dat1=dat1<<1;
    }
#if 1
    for(i=0;i<8;i++)
    {
        if(dat2 & 0x80)//1,for "1",H:0.8us,L:0.45us;
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
        dat2=dat2<<1;
    }

    for(i=0;i<8;i++)
    {
        if(dat3 & 0x80)//1,for "1",H:0.8us,L:0.45us;
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
        dat3=dat3<<1;
    }
#endif
}


void Send_8bits2(uint8_t dat1, uint8_t dat2, uint8_t dat3)
{
    uint8_t i;

    //RGB_IO_2_PIN_L;
    //delayns_100();
    for(i=0;i<8;i++)
    {
        if(dat1 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_2_PIN_H;
            delayns_900();
            RGB_IO_2_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_2_PIN_H;
            delayns_300();
            RGB_IO_2_PIN_L;
            delayns_600();
        }
        dat1=dat1<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat2 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_2_PIN_H;
            delayns_900();
            RGB_IO_2_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_2_PIN_H;
            delayns_300();
            RGB_IO_2_PIN_L;
            delayns_600();
        }
        dat2=dat2<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat3 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_2_PIN_H;
            delayns_900();
            RGB_IO_2_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_2_PIN_H;
            delayns_300();
            RGB_IO_2_PIN_L;
            delayns_600();
        }
        dat3=dat3<<1;
    }
}

void Send_8bits3(uint8_t dat1, uint8_t dat2, uint8_t dat3)
{
    uint8_t i;

    for(i=0;i<8;i++)
    {
        if(dat1 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_3_PIN_H;
            delayns_900();
            RGB_IO_3_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_3_PIN_H;
            delayns_300();
            RGB_IO_3_PIN_L;
            delayns_600();
        }
        dat1=dat1<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat2 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_3_PIN_H;
            delayns_900();
            RGB_IO_3_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_3_PIN_H;
            delayns_300();
            RGB_IO_3_PIN_L;
            delayns_600();
        }
        dat2=dat2<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat3 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_3_PIN_H;
            delayns_900();
            RGB_IO_3_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_3_PIN_H;
            delayns_300();
            RGB_IO_3_PIN_L;
            delayns_600();
        }
        dat3=dat3<<1;
    }
}

void Send_8bits4(uint8_t dat1, uint8_t dat2, uint8_t dat3)
{
    uint8_t i;

    for(i=0;i<8;i++)
    {
        if(dat1 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_4_PIN_H;
            delayns_900();
            RGB_IO_4_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_4_PIN_H;
            delayns_300();
            RGB_IO_4_PIN_L;
            delayns_600();
        }
        dat1=dat1<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat2 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_4_PIN_H;
            delayns_900();
            RGB_IO_4_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_4_PIN_H;
            delayns_300();
            RGB_IO_4_PIN_L;
            delayns_600();
        }
        dat2=dat2<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat3 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_4_PIN_H;
            delayns_900();
            RGB_IO_4_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_4_PIN_H;
            delayns_300();
            RGB_IO_4_PIN_L;
            delayns_600();
        }
        dat3=dat3<<1;
    }
}

void Send_8bits5(uint8_t dat1, uint8_t dat2, uint8_t dat3)
{
    uint8_t i;

    for(i=0;i<8;i++)
    {
        if(dat1 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_5_PIN_H;
            delayns_900();
            RGB_IO_5_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_5_PIN_H;
            delayns_300();
            RGB_IO_5_PIN_L;
            delayns_600();
        }
        dat1=dat1<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat2 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_5_PIN_H;
            delayns_900();
            RGB_IO_5_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_5_PIN_H;
            delayns_300();
            RGB_IO_5_PIN_L;
            delayns_600();
        }
        dat2=dat2<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat3 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_5_PIN_H;
            delayns_900();
            RGB_IO_5_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_5_PIN_H;
            delayns_300();
            RGB_IO_5_PIN_L;
            delayns_600();
        }
        dat3=dat3<<1;
    }
}

void Send_8bits6(uint8_t dat1, uint8_t dat2, uint8_t dat3)
{
    uint8_t i;

    for(i=0;i<8;i++)
    {
        if(dat1 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_6_PIN_H;
            delayns_900();
            RGB_IO_6_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_6_PIN_H;
            delayns_300();
            RGB_IO_6_PIN_L;
            delayns_600();
        }
        dat1=dat1<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat2 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_6_PIN_H;
            delayns_900();
            RGB_IO_6_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_6_PIN_H;
            delayns_300();
            RGB_IO_6_PIN_L;
            delayns_600();
        }
        dat2=dat2<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat3 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_6_PIN_H;
            delayns_900();
            RGB_IO_6_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_6_PIN_H;
            delayns_300();
            RGB_IO_6_PIN_L;
            delayns_600();
        }
        dat3=dat3<<1;
    }
}

void Send_8bits7(uint8_t dat1, uint8_t dat2, uint8_t dat3)
{
    uint8_t i;

    for(i=0;i<8;i++)
    {
        if(dat1 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_7_PIN_H;
            delayns_900();
            RGB_IO_7_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_7_PIN_H;
            delayns_300();
            RGB_IO_7_PIN_L;
            delayns_600();
        }
        dat1=dat1<<1;
    }

    for(i=0;i<8;i++)
    {
        if(dat2 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_7_PIN_H;
            delayns_900();
            RGB_IO_7_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_7_PIN_H;
            delayns_300();
            RGB_IO_7_PIN_L;
            delayns_600();
        }
        dat2=dat2<<1;
    }

    for(i=0;i<8;i++)
    {
        if(dat3 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_7_PIN_H;
            delayns_900();
            RGB_IO_7_PIN_L;
            delayns_100();
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_7_PIN_H;
            delayns_300();
            RGB_IO_7_PIN_L;
            delayns_600();
        }
        dat3=dat3<<1;
    }
}
//IO 1
void Send_s203_line1(uint8_t r,uint8_t g,uint8_t b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<CHIP_SIZE;i++)
    {
        Send_8bits1(b, g, r);
    }
    __enable_irq();
}

void Send_e133_line2(uint8_t r,uint8_t g,uint8_t b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<CHIP_SIZE;i++)
    {
        Send_8bits2(r,g,b);
    }
    __enable_irq();
}
void Send_e133_line3(uint8_t r,uint8_t g,uint8_t b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<CHIP_SIZE;i++)
    {
        Send_8bits3(r,g,b);
    }
    __enable_irq();
}
void Send_e133_line4(uint8_t r,uint8_t g,uint8_t b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<CHIP_SIZE;i++)
    {
        Send_8bits4(r,g,b);
    }
    __enable_irq();
}
void Send_e133_line5(uint8_t r,uint8_t g,uint8_t b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<CHIP_SIZE;i++)
    {
        Send_8bits5(r,g,b);
    }
    __enable_irq();
}
void Send_e133_line6(uint8_t r,uint8_t g,uint8_t b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<CHIP_SIZE;i++)
    {
        Send_8bits6(r,g,b);
    }
    __enable_irq();
}
void Send_e133_line7(uint8_t r,uint8_t g,uint8_t b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<CHIP_SIZE;i++)
    {
        Send_8bits7(r,g,b);
    }
    __enable_irq();
}
#pragma GCC pop_options
