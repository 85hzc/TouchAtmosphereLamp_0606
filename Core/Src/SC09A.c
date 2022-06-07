////////////////////////////////////////////////////////////////////////////////
// * Project         :
// * Development Tool:
// * Target MCU      : RL78G13
// * Crystal         : 4MHz
// * Date            : 2014-01-08
// * Author          : Jie
// * description     :
//
//
//
//
////////////////////////////////////////////////////////////////////////////////

//#include "defType.h"
//#include "platform.h"
#include "stm32f0xx_hal.h"
#include "SC09A.h"


//==============================================================================
// 函数功能:
// 描   述 :
// 全局变量:
// 局部变量:
// 入口参数: 无
// 出口参数: 无
// 状    态:
// 日    期: 2014
//==============================================================================
void SC09A_DelayuS(void)
{
  unsigned char temp;
  
  //for(temp=0;temp<4;temp++)  //延时  4MHz
   for(temp=0; temp < 32; temp++)  //延时
  {}
}

//==============================================================================
// 函数功能:
// 描   述 :
// 全局变量:
// 局部变量:
// 入口参数: 无
// 出口参数: 无
// 状    态:
// 日    期: 2014
//==============================================================================

#define CON_ADDR 0x81 // {A[6:0] + RWB} = 81H

//1. Start 和和 Stop 信号信号
//  Start 信号信号(S)
//    当 SC09A_SCL 是高电平时,SDA 由高到底变化，表示开始传输数据。
//  Stop 信号信号(P)
//    当 SC09A_SCL 是高电平时,SDA 由低到高变化，表示结束数据传输
//2. 数据有效
//  在 SC09A_SCL 为高电平期间,SDA 必须保持稳定的电平. SDA 线上的高低电平变化只能在 SC09A_SCL 为低电平期间。

//#define PIN_iINT             0x01 //PE0 Pin36 (HS) CLC_CCO       //I int   SC09A oNF 键中断输入(内上拉)
//#define PORT_iINT      GPIOE->IDR //PE0 Pin36 (HS) CLC_CCO       //I int   SC09A oNF 键中断输入(内上拉)

//#define PIN_oSCL             0x100 //PB8 Pin26 (HS)TM1_CH3       //O       SC09A oNF 时钟输出
//#define PORT_oSCL      GPIOB->ODR //PB8 Pin26 (HS)TM1_CH3       //O       SC09A oNF 时钟输出

//#define PIN_ioSDA            0x200 //PB9 Pin25 (HS)TM1_CH2       //OD      SC09A oNF SDA数据线(OD输出,外上拉)
//#define PORT_oSDA      GPIOB->ODR //PB9 Pin25 (HS)TM1_CH2       //OD      SC09A oNF SDA数据线(OD输出,外上拉)
//#define PORT_iSDA      GPIOB->IDR //PB9 Pin25 (HS)TM1_CH2       //OD      SC09A oNF SDA数据线(OD输出,外上拉)
//#define DDR_ioSDA      GPIOB->DDR

//start SC09A_SCL=H, SDA=fall edge,
//stop  SC09A_SCL=H, SDA=rise edge,

//SC09A_DIR = 1; //input
//SC09A_DIR = 0; //output

unsigned int SC09A_ReadKeyValue(void)
{
  unsigned int  key2byte0;
  unsigned char bitnum0, temp0, addr0;
  unsigned char bit_temp0;
  
  addr0 = CON_ADDR;
  key2byte0 = 0xFFFF;
  
  //SC09A_SCL = 1; //H
  //PORT_oSCL |= PIN_oSCL; //H
  IIC_SCL_1();
  
  //SC09A_DIR = 0; //output
  //DDR_ioSDA |= PIN_ioSDA;  //output
  SDA_OUT();
  
  //S C09A_SDA = 1; //H
  //PORT_oSDA |= PIN_ioSDA; //H
  IIC_SDA_1();
  SC09A_DelayuS(); //延时
  
  // 拉低 SDA 端口送出 START 信号
  //SC09A_SDA = 0;
  //PORT_oSDA &=~PIN_ioSDA; //0
  IIC_SDA_0();
  SC09A_DelayuS(); //延时
  
  //发送 8 位地址字节 (A[6:0]+RWB)
  for(bitnum0=0; bitnum0<8; bitnum0++)
  {
    //SC09A_SCL = 0;
    //PORT_oSCL &=~ PIN_oSCL; //0
    IIC_SCL_0();
    
    temp0 = addr0 & 0x80;
    if(temp0 == 0x80)
    { 
      //SC09A_SDA = 1; 
      //PORT_oSDA |= PIN_ioSDA; //H
      IIC_SDA_1();
    }
    else
    {
      //SC09A_SDA = 0; 
      //PORT_oSDA &=~ PIN_ioSDA; //H
      IIC_SDA_0();
    }
    
    addr0 = addr0<<1;
    SC09A_DelayuS(); //延时
    
    //SC09A_SCL = 1;
    //PORT_oSCL |= PIN_oSCL; //1
    IIC_SCL_1();
    SC09A_DelayuS(); //延时
  }
  
  //input //释放 SDA 端口 ,将 SDA 设置为输入端口
  //SC09A_SDA = 1;
  //PORT_oSDA |= PIN_ioSDA; //H
  IIC_SDA_1();
  //SC09A_DIR = 1;
  //DDR_ioSDA &=~PIN_ioSDA;  //input
  SDA_IN();
    
  //SC09A_SCL = 0;
  //PORT_oSCL &=~ PIN_oSCL; //0 
  IIC_SCL_0();
  SC09A_DelayuS(); //延时
  
  //SC09A_SCL = 1;
  //PORT_oSCL |= PIN_oSCL; //1
  IIC_SCL_1();
  SC09A_DelayuS(); //延时
  
  //bit_temp0 = PORT_iSDA;
  //if(bit_temp0 & PIN_ioSDA) //读 ack 回应
  if (SDA_READ())
  {
    //ack 信号没有读到，指示通信有误
    return(0);
  }
  
  SC09A_DelayuS(); //延时
  
  //读 16 位按键数据字节(D[15:0])
  for(bitnum0=0; bitnum0 < 16; bitnum0++)
  {
    //SC09A_SCL = 0;
    //PORT_oSCL &=~ PIN_oSCL; //0   
    IIC_SCL_0(); 
    SC09A_DelayuS(); //延时
    
    //SC09A_SCL = 1;
    //PORT_oSCL |= PIN_oSCL; //1
    IIC_SCL_1();
    SC09A_DelayuS(); //延时
    
    //bit_temp0 = PORT_iSDA;
    //if(bit_temp0 & PIN_ioSDA) //读 ack 回应
    if (SDA_READ())
    {
      key2byte0 = key2byte0 <<1;
      key2byte0 = key2byte0 | 0x01;
    }
    else
    {
      key2byte0 = key2byte0 <<1;
    }
  }
  
  //SC09A_SCL = 0;
  //PORT_oSCL &=~ PIN_oSCL; //0
  IIC_SCL_0(); 
  SC09A_DelayuS(); //延时
  
  //SC09A_SCL = 1;
  //PORT_oSCL |= PIN_oSCL; //1
  IIC_SCL_1();
  SC09A_DelayuS(); //延时
  
  //SC09A_SCL = 0;
  //PORT_oSCL &=~ PIN_oSCL; //0 
  IIC_SCL_0();
  
  //SC09A_DIR = 0; //output
  //SC09A_SDA = 0; //发送 NACK 信号
  //DDR_ioSDA |= PIN_ioSDA;  //output
  SDA_OUT();
  
  //PORT_oSDA &=~ PIN_ioSDA; //0 //发送 NACK 信号
  IIC_SDA_0();
  SC09A_DelayuS(); //延时
  
  //SC09A_SCL = 1;
  //PORT_oSCL |= PIN_oSCL; //1
  IIC_SCL_1();
  SC09A_DelayuS(); //延时
  
  //SC09A_SDA = 1; //释放 SDA 端口 ,将 SDA 设置为输入端口
  //PORT_oSDA |= PIN_ioSDA; //H
  IIC_SDA_1();
  
  SC09A_DelayuS(); //延时
  //key2byte0 = key2byte0^0xffff;
  
  return(key2byte0);
  //数据位为 1 ，说明相应按键被触摸。例如，返回值为 0x0500 说明按
  //键 CIN2 和 CIN4 被触摸。
}


#if 0

//#define PIN_iINT1A          0x20U //PB5 Pin16 AIN5              //I int //SC09A Func 键中断输入
//#define PORT_iINT1A    GPIOB->ODR //PB5 Pin16 AIN5              //I int //SC09A Func 键中断输入

//#define PIN_oSCL1A          0x40U //PB6 Pin15 AIN6              //O     //SC09A Func 时钟输出
//#define PORT_oSCL1A    GPIOB->ODR //PB6 Pin15 AIN6              //O     //SC09A Func 时钟输出

//#define PIN_ioSDA1A         0x80U //PB7 Pin14 AIN7              //IO    //SC09A Func 数据线
//#define PORT_oSDA1A    GPIOB->ODR //PB7 Pin14 AIN7              //IO    //SC09A Func 数据线
//#define PORT_iSDA1A    GPIOB->IDR //PB7 Pin14 AIN7              //IO    //SC09A Func 数据线
//#define DDR_ioSDA1A    GPIOB->DDR

unsigned int SC09A_1A_ReadKeyValue(void)
{
  unsigned int  key2byte;
  unsigned char bitnum, temp, addr;
  unsigned char bit_temp;
  
  addr = CON_ADDR;
  key2byte = 0xFFFF;
  
  //SC09A_SCL = 1; //H
  PORT_oSCL1A |= PIN_oSCL1A; //H    
  
  //SC09A_DIR = 0; //output
  //GPIOB->DDR  |= PIN_ioSDA1A;  //output
  DDR_ioSDA1A |= PIN_ioSDA1A;  //output
  
  //S C09A_SDA = 1; //H
  PORT_oSDA1A |= PIN_ioSDA1A; //H  
  SC09A_DelayuS(); //延时
  
  // 拉低 SDA 端口送出 START 信号
  //SC09A_SDA = 0;
  PORT_oSDA1A &=~PIN_ioSDA1A; //0  
  SC09A_DelayuS(); //延时
  
  //发送 8 位地址字节 (A[6:0]+RWB)
  for(bitnum=0; bitnum<8; bitnum++)
  {
    //SC09A_SCL = 0;
    PORT_oSCL1A &=~ PIN_oSCL1A; //0    
    
    temp = addr & 0x80;
    if(temp == 0x80)
    { 
      //SC09A_SDA = 1; 
      PORT_oSDA1A |= PIN_ioSDA1A; //H
    }
    else
    {
      //SC09A_SDA = 0; 
      PORT_oSDA1A &=~ PIN_ioSDA1A; //H
    }
    
    addr = addr<<1;
    SC09A_DelayuS(); //延时
    
    //SC09A_SCL = 1;
    PORT_oSCL1A |= PIN_oSCL1A; //1
    SC09A_DelayuS(); //延时
  }
  
  //input //释放 SDA 端口 ,将 SDA 设置为输入端口
  //SC09A_SDA = 1;
  //SC09A_DIR = 1;
  PORT_oSDA1A |= PIN_ioSDA1A; //H
  //GPIOB->DDR  &=~PIN_ioSDA1A;  //input
  DDR_ioSDA1A &=~PIN_ioSDA1A;  //input
    
  //SC09A_SCL = 0;
  PORT_oSCL1A &=~ PIN_oSCL1A; //0 
  SC09A_DelayuS(); //延时
  
  //SC09A_SCL = 1;
  PORT_oSCL1A |= PIN_oSCL1A; //0 
  SC09A_DelayuS(); //延时
  
  //bit_temp = PORT_ioSDA;
  bit_temp = PORT_iSDA1A;
  if(bit_temp & PIN_ioSDA1A) //读 ack 回应
  {
    //ack 信号没有读到，指示通信有误
    return(0);
  }
  
  SC09A_DelayuS(); //延时
  
  //读 16 位按键数据字节(D[15:0])
  for(bitnum=0; bitnum < 16; bitnum++)
  {
    //SC09A_SCL = 0;
    PORT_oSCL1A &=~ PIN_oSCL1A; //0    
    SC09A_DelayuS(); //延时
    
    //SC09A_SCL = 1;
    PORT_oSCL1A |= PIN_oSCL1A; //1
    SC09A_DelayuS(); //延时
    
    bit_temp = PORT_iSDA1A;
    if(bit_temp & PIN_ioSDA1A) //读 ack 回应
    {
      key2byte = key2byte <<1;
      key2byte = key2byte | 0x01;
    }
    else
    {
      key2byte = key2byte <<1;
    }
  }
  
  //SC09A_SCL = 0;
  PORT_oSCL1A &=~ PIN_oSCL1A; //0    
  SC09A_DelayuS(); //延时
  
  //SC09A_SCL = 1;
  PORT_oSCL1A |= PIN_oSCL1A; //1
  SC09A_DelayuS(); //延时
  
  //SC09A_SCL = 0;
  PORT_oSCL1A &=~ PIN_oSCL1A; //0 
  
  //SC09A_DIR = 0; //output
  //SC09A_SDA = 0; //发送 NACK 信号
  //GPIOB->DDR  |= PIN_ioSDA1A;  //output
  DDR_ioSDA1A |= PIN_ioSDA1A;  //output
  
  PORT_oSDA1A &=~ PIN_ioSDA1A; //0 //发送 NACK 信号
  SC09A_DelayuS(); //延时
  
  //SC09A_SCL = 1;
  PORT_oSCL1A |= PIN_oSCL1A; //1
  SC09A_DelayuS(); //延时
  
  //SC09A_SDA = 1; //释放 SDA 端口 ,将 SDA 设置为输入端口
  PORT_oSDA1A |= PIN_ioSDA1A; //H
  
  SC09A_DelayuS(); //延时
  //key2byte = key2byte^0xffff;
  
  return(key2byte);
  //数据位为 1 ，说明相应按键被触摸。例如，返回值为 0x0500 说明按
  //键 CIN2 和 CIN4 被触摸。
  
}
#endif

////////////////////////////////////////////////////////////////////////////////
//                            THE       END                                   //
////////////////////////////////////////////////////////////////////////////////
