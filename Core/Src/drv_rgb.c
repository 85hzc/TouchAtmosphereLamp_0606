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
#include "drv_serial.h"

/* Private variables ---------------------------------------------------------*/
static uint32_t tickstart;
static color_e up_type = white_time;
static color_e down_type = white_time;
static bool flash_flag = false;
bool switch_on_off = 0;
volatile uint8_t buff_G=9, buff_R=9, buff_B=9;
uint8_t lddj = 2;   //0-4
uint8_t mode = 2, scanType = 0;   //mode: 1 rgb; 2 ypg
volatile COLOR_RGB rgb_v;

COLOR_RGB white[5] = {
    {15, 15, 15},
    {12, 12, 12},
    {9, 9, 9},
    {7, 7, 7},
    {5, 5, 5},
};
COLOR_RGB yellow[5] = {
    {30, 30, 0},
    {25, 25, 0},
    {20, 20, 0},
    {15, 15, 0},
    {10, 10, 0},
};
COLOR_RGB pink[5] = {
    {0, 30, 30},
    {0, 25, 25},
    {0, 20, 20},
    {0, 15, 15},
    {0, 10, 10},
};

uint8_t rgb[5] = {
    {50},
    {40},
    {30},
    {20},
    {10},
};


uint8_t     cube_buff_G[CUBE_COL_SIZE];
uint8_t     cube_buff_R[CUBE_COL_SIZE];
uint8_t     cube_buff_B[CUBE_COL_SIZE];

bool        newReqFlag = true;
static uint32_t GRB48[CUBE_COL_SIZE]={
	#if 0
                       /*0xFFFF0F,0xEFFF18,0xDFFF1F,0xCFFF2F,0xBFFF3F,0xAFFF4F,0x9FFF5F,0x8FFF6F,
                       0x7FFF7F,0x6FFF8F,0x5FFF9F,0x4FFFAF,0x3FFFBF,0x2FFFCF,0x1FFFDF,0x18FFEF,
                       
                       0x0FEFFF,0x18DFFF,0x1FCFFF,0x2FBFFF,0x3FAFFF,0x4F9FFF,0x5F8FFF,0x6F7FFF,
                       0x7F6FFF,0x8F5FFF,0x9F4FFF,0xAF3FFF,0xBF2FFF,0xCF1FFF,0xDF18FF,0xEF0FFF,*/
	#else
	0x404000,
	0x3c4004,
	0x384008,
	0x34400c,
	0x304010,
	0x2c4014,
	0x284018,
	0x24401c,
  0x204020,
	0x1c4024,
	0x184028,
	0x14402c,
	0x104030,
	0x0c4034,
	0x084038,
	0x04403c,
  0x004040,
	0x043c40,
	0x083840,
	0x0c3440,
	0x103040,
	0x142c40,
	0x182840,
	0x1c2440,
  0x202040,
	0x241c40,
	0x281440,
	0x2c1040,
	0x300c40,
	0x340840,
	0x380440,
	0x3c0040,
	#endif
                       /*
                       0xFF18EF,0xFF1FDF,0xFF2FCF,0xFF3FBF,0xFF4FAF,0xFF5F9F,0xFF6F8F,0xFF7F7F,
                       0xFF8F6F,0xFF9F5F,0xFFAF4F,0xFFBF3F,0xFFCF2F,0xFFDF1F,0xFFEF18,0xFFFF0F*/
                       };


extern uint8_t key_cmd_flag;

/* Private function prototypes -----------------------------------------------*/
void Send_s203_line1(uint8_t r,uint8_t g,uint8_t b);
void Send_e133_line2(uint8_t r,uint8_t g,uint8_t b);
void Send_e133_line3(uint8_t r,uint8_t g,uint8_t b);
void Send_e133_line4(uint8_t r,uint8_t g,uint8_t b);
void Send_e133_line5(uint8_t r,uint8_t g,uint8_t b);
void Send_e133_line6(uint8_t r,uint8_t g,uint8_t b);
void Send_e133_line7(uint8_t r,uint8_t g,uint8_t b);

void Send_s203_line1_laying(uint8_t *r,uint8_t *g,uint8_t *b);
void Send_e133_line2_laying(uint8_t *r,uint8_t *g,uint8_t *b);
void Send_e133_line3_laying(uint8_t *r,uint8_t *g,uint8_t *b);
void Send_e133_line4_laying(uint8_t *r,uint8_t *g,uint8_t *b);
void Send_e133_line5_laying(uint8_t *r,uint8_t *g,uint8_t *b);
void Send_e133_line6_laying(uint8_t *r,uint8_t *g,uint8_t *b);
void Send_e133_line7_laying(uint8_t *r,uint8_t *g,uint8_t *b);
											 
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

static unsigned char Red0 = 0, Green0 = 0, Blue0 = 0;  // 起始三原色
static float f_red0 = 0.0, f_green0 = 0.0, f_blue0 = 0.0;

void initBaseColor()
{
    Red0 = 0;
    Green0 = 0;
    Blue0 = 0;

    f_red0 = 0;
    f_blue0 = 0;
    f_green0 = 0;
}
void switchONOFF(uint8_t Red, uint8_t Green, uint8_t Blue)
{
    Red0 = Red;
    Green0 = Green;
    Blue0 = Blue;
    Drv_SERIAL_Log("switchONOFF");
    Send_s203_line1(Red, Green, Blue);
    Send_e133_line2(Red, Green, Blue);
    Send_e133_line3(Red, Green, Blue);
    Send_e133_line4(Red, Green, Blue);
    Send_e133_line5(Red, Green, Blue);
    Send_e133_line6(Red, Green, Blue);
    Send_e133_line7(Red, Green, Blue);
}

void ColorGradualDimmingS203(uint8_t Red1, uint8_t Green1, uint8_t Blue1)
{
    Drv_SERIAL_Log("S203 %d %d %d", Red1, Green1, Blue1);

    Send_s203_line1(Red1, Green1, Blue1);
}

void ColorGradualDimming(uint8_t Red1, uint8_t Green1, uint8_t Blue1)
{
    //Drv_SERIAL_Log("Dimming %d %d %d", Red1, Green1, Blue1);

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

    Drv_SERIAL_Log("step:%f,%f,%f, [%d]", RedStep, GreenStep, BlueStep, NStep);
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

void ColorLayingS203(uint8_t *Red0, uint8_t *Green0, uint8_t *Blue10)
{
    Send_s203_line1_laying(Red0, Green0, Blue0);
}

void ColorLayingE133(uint8_t *Red0, uint8_t *Green0, uint8_t *Blue10)
{
    Send_e133_line2_laying(Red0, Green0, Blue0);
    Send_e133_line3_laying(Red0, Green0, Blue0);
    Send_e133_line4_laying(Red0, Green0, Blue0);
    Send_e133_line5_laying(Red0, Green0, Blue0);
    Send_e133_line6_laying(Red0, Green0, Blue0);
    Send_e133_line7_laying(Red0, Green0, Blue0);
}

void Drv_RGB_Init(void)
{
    rgb_v.R = buff_R;
    rgb_v.G = buff_G;
    rgb_v.B = buff_B;
	  rgb_v.l = 50;   //启动初始亮度50, rgb:9
}


void ScenceLayering()
{
    uint8_t     row=0;
    uint8_t     g,r,b;

    if(newReqFlag)
    {
        newReqFlag = 0;
        for( row=0; row<CUBE_COL_SIZE; row++ )
        {
            //初始化颜色组
            cube_buff_G[row] = ((GRB48[row]>>16)&0xff);
            cube_buff_R[row] = ((GRB48[row]>>8) &0xff);
            cube_buff_B[row] = ((GRB48[row])    &0xff);
        }
    }
    else
    {
        g = cube_buff_G[0];
        r = cube_buff_R[0];
        b = cube_buff_B[0];

        //颜色上升
        for( row=0; row<CUBE_COL_SIZE-1; row++ )
        {
            cube_buff_G[row] = cube_buff_G[row+1];
            cube_buff_R[row] = cube_buff_R[row+1];
            cube_buff_B[row] = cube_buff_B[row+1];
        }

        //新导入的一行颜色
        cube_buff_G[CUBE_COL_SIZE-1] = g;
        cube_buff_R[CUBE_COL_SIZE-1] = r;
        cube_buff_B[CUBE_COL_SIZE-1] = b;
    }
    
#if 0
    Drv_SERIAL_Log("---------------------------\r\n");
    for( row=0; row<CUBE_COL_SIZE; row++ )
    {
        Drv_SERIAL_Log("[%d]:%x %x %x",row,
                cube_buff_G[row],cube_buff_R[row],cube_buff_B[row]);
    }
    Drv_SERIAL_Log("\r\n");
#endif
}

void ColorCalc()
{
    if (mode==1)    //rgb or laying or white
    {
        #if 1
        if (up_type==red_time)
        {
            buff_G = 0;
            buff_R = rgb[lddj];
            buff_B = 0;
        }
        else if (up_type==green_time)
        {
            buff_G = rgb[lddj];
            buff_R = 0;
            buff_B = 0;
        }
        else if (up_type==blue_time)
        {
            buff_G = 0;
            buff_R = 0;
            buff_B = rgb[lddj];
        }
        else if (up_type==white_time)
        {
            buff_G = white[lddj].G;
            buff_R = white[lddj].R;
            buff_B = white[lddj].B;
        }
        #endif
    }
    else if (mode==2)    //white
    {
        if (down_type==yellow_time)
        {
            buff_G = yellow[lddj].G;
            buff_R = yellow[lddj].R;
            buff_B = yellow[lddj].B; 
        }
        else if (down_type==pink_time)
        {
            buff_G = pink[lddj].G;
            buff_R = pink[lddj].R;
            buff_B = pink[lddj].B; 
        }
        else if (down_type==green_time)
        {
            buff_G = rgb[lddj];
            buff_R = 0;
            buff_B = 0;
        }
        else if (down_type==white_time)
        {
            buff_G = white[lddj].G;
            buff_R = white[lddj].R;
            buff_B = white[lddj].B;
        }
    }
}

void Drv_RGB_Proc(void)
{
    static bool mode0_immedia = false;
    //Drv_SERIAL_Log("key_cmd_flag %d", key_cmd_flag);
    if(key_cmd_flag)
    {
        flash_flag = true;

        switch(key_cmd_flag)
        {
            case 9: //down
            {
                switch_on_off = true;
                mode = 2;
                Drv_SERIAL_Log("switch YPG");
                switch (down_type)
                {
                    case yellow_time:
                        down_type = pink_time;
                        break;

                    case pink_time:
                        down_type = green_time;
                        break;

                    case green_time:
                        down_type = white_time;
                        break;
                    
                    case white_time:
                    default:
                        down_type = yellow_time;
                        break;
                }
                ColorCalc();
                /*
                rgb_v.G = buff_G;
                rgb_v.R = buff_R;
                rgb_v.B = buff_B;
                */
                break;
            }
            case 12:  //up
            {
                mode0_immedia = true;
                switch_on_off = true;
                mode = 1;
                newReqFlag = true;
                scanType++;
                Drv_SERIAL_Log("switch %s", scanType%2?"RGB":"Laying");
                break;
            }
            case 8: //right
            {
                switch_on_off = true;
                Drv_SERIAL_Log("turn up");
                if (lddj>1)
                    lddj--;
                ColorCalc();
                //adjustBrightness(10);
            /*
                if (rgb_v.R>0xa0 && rgb_v.G>0xa0 && rgb_v.B>0xa0)
                {
                    Drv_SERIAL_Log("over current protection");
                    adjustBrightness(-10);
                }*/
                /*buff_G = rgb_v.G;
                buff_R = rgb_v.R;
                buff_B = rgb_v.B;*/
                break;
            }
            case 11:  //left
            {
                switch_on_off = true;
                Drv_SERIAL_Log("turn down");
                if (lddj<4)
                    lddj++;
                ColorCalc();
                //adjustBrightness(-10);
                /*
                if ((rgb_v.R + rgb_v.G == 0 && rgb_v.B < 200) ||
                    (rgb_v.G + rgb_v.B == 0 && rgb_v.R < 200) ||
                    (rgb_v.B + rgb_v.R == 0 && rgb_v.G < 200) ||

                    ((rgb_v.R + rgb_v.G) < 300) && (rgb_v.B==0) ||
                    ((rgb_v.R != 0 && rgb_v.G != 0 && rgb_v.B != 0) && ((rgb_v.R + rgb_v.G + rgb_v.B) < 300)))
                {
                    Drv_SERIAL_Log("low current protection");
                    adjustBrightness(10);
                }*/
                /*
                buff_G = rgb_v.G;
                buff_R = rgb_v.R;
                buff_B = rgb_v.B;
                */
                break;
            }
            case 10:  //on/off
            {
                switch_on_off = !switch_on_off;
                Drv_SERIAL_Log("%s", switch_on_off==true ? "on":"off");
                if (switch_on_off)// ON
                {
                    /*buff_G = rgb_v.G;
                    buff_R = rgb_v.R;
                    buff_B = rgb_v.B;*/
                    ColorCalc();
                }
                else            //OFF
                {
                    buff_G = 0;
                    buff_R = 0;
                    buff_B = 0;
                }
                break;
            }

            default:
                break;
        }
        key_cmd_flag = 0;
    }

    //Drv_SERIAL_Log("flash flag mode %d, flag %d", mode, flash_flag);
    if (flash_flag)
    {
        if (mode==1)    //rgb or laying
        {
            //Drv_SERIAL_Log("mode==1 %d %d", HAL_GetTick(), tickstart);
            
            if (scanType%2 == 0)
            {
                if((HAL_GetTick()-tickstart > 25 || mode0_immedia) && switch_on_off)
                {
                    mode0_immedia = false;
                    //Drv_SERIAL_Log("e133 RGB");
                    ScenceLayering();
                    ColorLayingS203((uint8_t*)cube_buff_R, (uint8_t*)cube_buff_G, (uint8_t*)cube_buff_B);
                    ColorLayingE133((uint8_t*)cube_buff_R, (uint8_t*)cube_buff_G, (uint8_t*)cube_buff_B);
                    tickstart = HAL_GetTick();
                }
                else if (!switch_on_off)
                {
                    switchONOFF(0,0,0);
                    initBaseColor();
                    flash_flag = false;
                }
            }
            else
            {
                if((HAL_GetTick()-tickstart > 1000 || mode0_immedia) && switch_on_off)
                {
                    mode0_immedia = false;
                    switch (up_type)
                    {
                        case red_time:
                            up_type = green_time;
                            break;

                        case green_time:
                            up_type = blue_time;
                            break;

                        case blue_time:
                            up_type = white_time;
                            break;

                        case white_time:
                        default:
                            up_type = red_time;
                            break;
                    }
                    ColorCalc();
                    //ColorGradualDimmingS203(buff_R, buff_G, buff_B);
                    ColorGradualDimming(buff_R, buff_G, buff_B);
                    tickstart = HAL_GetTick();
                }
                else if (!switch_on_off)
                {
                    ColorGradualDimming(buff_R, buff_G, buff_B);
                    initBaseColor();
                    flash_flag = false;
                }
            }
        }
        else
        {
            flash_flag = false;
            //if (!switch_on_off)
            {
            ///    switchONOFF(0,0,0);
            }
            //else
            {
                //ColorGradualDimmingS203(buff_R, buff_G, buff_B);
                ColorGradualDimming(buff_R, buff_G, buff_B);
            }
        }
    }
}

void Send_8bits1(uint8_t dat1, uint8_t dat2, uint8_t dat3)
{
    uint8_t i;//, dat1, dat2, dat3;

    //dat1 = dat11;
    for(i=0;i<8;i++)
    {
        if(dat1 & 0x80)//1
        {
            RGB_IO_1_PIN_H;
            delay900;
            RGB_IO_1_PIN_L;
            delay100;
        }
        else    //0
        {
            RGB_IO_1_PIN_H;
            delay300;
            RGB_IO_1_PIN_L;
            delay600;
        }
        dat1=dat1<<1;
    }

    //dat2 = dat22;
    for(i=0;i<8;i++)
    {
        if(dat2 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_1_PIN_H;
            delay900;
            RGB_IO_1_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_1_PIN_H;
            delay300;
            RGB_IO_1_PIN_L;
            delay600;
        }
        dat2=dat2<<1;
    }

    //dat3 = dat33;
    for(i=0;i<8;i++)
    {
        if(dat3 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_1_PIN_H;
            delay900;
            RGB_IO_1_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_1_PIN_H;
            delay300;
            RGB_IO_1_PIN_L;
            delay600;
        }
        dat3=dat3<<1;
    }
}


void Send_8bits2(uint8_t dat1, uint8_t dat2, uint8_t dat3)
{
    uint8_t i;

    //RGB_IO_2_PIN_L;
    //delay100;
    for(i=0;i<8;i++)
    {
        if(dat1 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_2_PIN_H;
            delay900;
            RGB_IO_2_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_2_PIN_H;
            delay300;
            RGB_IO_2_PIN_L;
            delay600;
        }
        dat1=dat1<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat2 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_2_PIN_H;
            delay900;
            RGB_IO_2_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_2_PIN_H;
            delay300;
            RGB_IO_2_PIN_L;
            delay600;
        }
        dat2=dat2<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat3 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_2_PIN_H;
            delay900;
            RGB_IO_2_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_2_PIN_H;
            delay300;
            RGB_IO_2_PIN_L;
            delay600;
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
            delay900;
            RGB_IO_3_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_3_PIN_H;
            delay300;
            RGB_IO_3_PIN_L;
            delay600;
        }
        dat1=dat1<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat2 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_3_PIN_H;
            delay900;
            RGB_IO_3_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_3_PIN_H;
            delay300;
            RGB_IO_3_PIN_L;
            delay600;
        }
        dat2=dat2<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat3 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_3_PIN_H;
            delay900;
            RGB_IO_3_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_3_PIN_H;
            delay300;
            RGB_IO_3_PIN_L;
            delay600;
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
            delay900;
            RGB_IO_4_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_4_PIN_H;
            delay300;
            RGB_IO_4_PIN_L;
            delay600;
        }
        dat1=dat1<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat2 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_4_PIN_H;
            delay900;
            RGB_IO_4_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_4_PIN_H;
            delay300;
            RGB_IO_4_PIN_L;
            delay600;
        }
        dat2=dat2<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat3 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_4_PIN_H;
            delay900;
            RGB_IO_4_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_4_PIN_H;
            delay300;
            RGB_IO_4_PIN_L;
            delay600;
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
            delay900;
            RGB_IO_5_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_5_PIN_H;
            delay300;
            RGB_IO_5_PIN_L;
            delay600;
        }
        dat1=dat1<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat2 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_5_PIN_H;
            delay900;
            RGB_IO_5_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_5_PIN_H;
            delay300;
            RGB_IO_5_PIN_L;
            delay600;
        }
        dat2=dat2<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat3 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_5_PIN_H;
            delay900;
            RGB_IO_5_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_5_PIN_H;
            delay300;
            RGB_IO_5_PIN_L;
            delay600;
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
            delay900;
            RGB_IO_6_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_6_PIN_H;
            delay300;
            RGB_IO_6_PIN_L;
            delay600;
        }
        dat1=dat1<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat2 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_6_PIN_H;
            delay900;
            RGB_IO_6_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_6_PIN_H;
            delay300;
            RGB_IO_6_PIN_L;
            delay600;
        }
        dat2=dat2<<1;
    }
    for(i=0;i<8;i++)
    {
        if(dat3 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_6_PIN_H;
            delay900;
            RGB_IO_6_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_6_PIN_H;
            delay300;
            RGB_IO_6_PIN_L;
            delay600;
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
            delay900;
            RGB_IO_7_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_7_PIN_H;
            delay300;
            RGB_IO_7_PIN_L;
            delay600;
        }
        dat1=dat1<<1;
    }

    for(i=0;i<8;i++)
    {
        if(dat2 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_7_PIN_H;
            delay900;
            RGB_IO_7_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_7_PIN_H;
            delay300;
            RGB_IO_7_PIN_L;
            delay600;
        }
        dat2=dat2<<1;
    }

    for(i=0;i<8;i++)
    {
        if(dat3 & 0x80)//1,for "1",H:0.8us,L:0.45us;
        {
            RGB_IO_7_PIN_H;
            delay900;
            RGB_IO_7_PIN_L;
            delay100;
        }
        else    //0 ,for "0",H:0.4us,L: 0.85us
        {
            RGB_IO_7_PIN_H;
            delay300;
            RGB_IO_7_PIN_L;
            delay600;
        }
        dat3=dat3<<1;
    }
}
//IO 1
void Send_s203_line1(uint8_t r,uint8_t g,uint8_t b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<LINE_1_SIZE;i++)
    {
        Send_8bits1(b, g, r);
    }
    __enable_irq();
}
void Send_s203_line1_laying(uint8_t *r,uint8_t *g,uint8_t *b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<LINE_1_SIZE;i++)
    {
        Send_8bits1(r[i],g[i],b[i]);
    }
    __enable_irq();
}
void Send_e133_line2(uint8_t r,uint8_t g,uint8_t b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<LINE_2_SIZE;i++)
    {
        Send_8bits2(r,g,b);
    }
    __enable_irq();
}


void Send_e133_line2_laying(uint8_t *r,uint8_t *g,uint8_t *b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<LINE_2_SIZE;i++)
    {
        Send_8bits2(r[i],g[i],b[i]);
    }
    __enable_irq();
}

void Send_e133_line3(uint8_t r,uint8_t g,uint8_t b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<LINE_3_SIZE;i++)
    {
        Send_8bits3(r,g,b);
    }
    __enable_irq();
}

void Send_e133_line3_laying(uint8_t *r,uint8_t *g,uint8_t *b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<LINE_3_SIZE;i++)
    {
        Send_8bits3(r[i],g[i],b[i]);
    }
    __enable_irq();
}

void Send_e133_line4(uint8_t r,uint8_t g,uint8_t b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<LINE_4_SIZE;i++)
    {
        Send_8bits4(r,g,b);
    }
    __enable_irq();
}
void Send_e133_line4_laying(uint8_t *r,uint8_t *g,uint8_t *b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<LINE_4_SIZE;i++)
    {
        Send_8bits4(r[i],g[i],b[i]);
    }
    __enable_irq();
}
void Send_e133_line5(uint8_t r,uint8_t g,uint8_t b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<LINE_5_SIZE;i++)
    {
        Send_8bits5(r,g,b);
    }
    __enable_irq();
}
void Send_e133_line5_laying(uint8_t *r,uint8_t *g,uint8_t *b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<LINE_5_SIZE;i++)
    {
        Send_8bits5(r[i],g[i],b[i]);
    }
    __enable_irq();
}
void Send_e133_line6(uint8_t r,uint8_t g,uint8_t b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<LINE_6_SIZE;i++)
    {
        Send_8bits6(r,g,b);
    }
    __enable_irq();
}
void Send_e133_line6_laying(uint8_t *r,uint8_t *g,uint8_t *b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<LINE_6_SIZE;i++)
    {
        Send_8bits6(r[i],g[i],b[i]);
    }
    __enable_irq();
}
void Send_e133_line7(uint8_t r,uint8_t g,uint8_t b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<LINE_7_SIZE;i++)
    {
        Send_8bits7(r,g,b);
    }
    __enable_irq();
}
void Send_e133_line7_laying(uint8_t *r,uint8_t *g,uint8_t *b)
{
    uint8_t i=0;

    __disable_irq();
    for(i=0;i<LINE_7_SIZE;i++)
    {
        Send_8bits7(r[i],g[i],b[i]);
    }
    __enable_irq();
}
#pragma GCC pop_options
