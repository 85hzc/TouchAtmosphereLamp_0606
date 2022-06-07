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
// ��������:
// ��   �� :
// ȫ�ֱ���:
// �ֲ�����:
// ��ڲ���: ��
// ���ڲ���: ��
// ״    ̬:
// ��    ��: 2014
//==============================================================================
void SC09A_DelayuS(void)
{
  unsigned char temp;
  
  //for(temp=0;temp<4;temp++)  //��ʱ  4MHz
   for(temp=0; temp < 32; temp++)  //��ʱ
  {}
}

//==============================================================================
// ��������:
// ��   �� :
// ȫ�ֱ���:
// �ֲ�����:
// ��ڲ���: ��
// ���ڲ���: ��
// ״    ̬:
// ��    ��: 2014
//==============================================================================

#define CON_ADDR 0x81 // {A[6:0] + RWB} = 81H

//1. Start �ͺ� Stop �ź��ź�
//  Start �ź��ź�(S)
//    �� SC09A_SCL �Ǹߵ�ƽʱ,SDA �ɸߵ��ױ仯����ʾ��ʼ�������ݡ�
//  Stop �ź��ź�(P)
//    �� SC09A_SCL �Ǹߵ�ƽʱ,SDA �ɵ͵��߱仯����ʾ�������ݴ���
//2. ������Ч
//  �� SC09A_SCL Ϊ�ߵ�ƽ�ڼ�,SDA ���뱣���ȶ��ĵ�ƽ. SDA ���ϵĸߵ͵�ƽ�仯ֻ���� SC09A_SCL Ϊ�͵�ƽ�ڼ䡣

//#define PIN_iINT             0x01 //PE0 Pin36 (HS) CLC_CCO       //I int   SC09A oNF ���ж�����(������)
//#define PORT_iINT      GPIOE->IDR //PE0 Pin36 (HS) CLC_CCO       //I int   SC09A oNF ���ж�����(������)

//#define PIN_oSCL             0x100 //PB8 Pin26 (HS)TM1_CH3       //O       SC09A oNF ʱ�����
//#define PORT_oSCL      GPIOB->ODR //PB8 Pin26 (HS)TM1_CH3       //O       SC09A oNF ʱ�����

//#define PIN_ioSDA            0x200 //PB9 Pin25 (HS)TM1_CH2       //OD      SC09A oNF SDA������(OD���,������)
//#define PORT_oSDA      GPIOB->ODR //PB9 Pin25 (HS)TM1_CH2       //OD      SC09A oNF SDA������(OD���,������)
//#define PORT_iSDA      GPIOB->IDR //PB9 Pin25 (HS)TM1_CH2       //OD      SC09A oNF SDA������(OD���,������)
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
  SC09A_DelayuS(); //��ʱ
  
  // ���� SDA �˿��ͳ� START �ź�
  //SC09A_SDA = 0;
  //PORT_oSDA &=~PIN_ioSDA; //0
  IIC_SDA_0();
  SC09A_DelayuS(); //��ʱ
  
  //���� 8 λ��ַ�ֽ� (A[6:0]+RWB)
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
    SC09A_DelayuS(); //��ʱ
    
    //SC09A_SCL = 1;
    //PORT_oSCL |= PIN_oSCL; //1
    IIC_SCL_1();
    SC09A_DelayuS(); //��ʱ
  }
  
  //input //�ͷ� SDA �˿� ,�� SDA ����Ϊ����˿�
  //SC09A_SDA = 1;
  //PORT_oSDA |= PIN_ioSDA; //H
  IIC_SDA_1();
  //SC09A_DIR = 1;
  //DDR_ioSDA &=~PIN_ioSDA;  //input
  SDA_IN();
    
  //SC09A_SCL = 0;
  //PORT_oSCL &=~ PIN_oSCL; //0 
  IIC_SCL_0();
  SC09A_DelayuS(); //��ʱ
  
  //SC09A_SCL = 1;
  //PORT_oSCL |= PIN_oSCL; //1
  IIC_SCL_1();
  SC09A_DelayuS(); //��ʱ
  
  //bit_temp0 = PORT_iSDA;
  //if(bit_temp0 & PIN_ioSDA) //�� ack ��Ӧ
  if (SDA_READ())
  {
    //ack �ź�û�ж�����ָʾͨ������
    return(0);
  }
  
  SC09A_DelayuS(); //��ʱ
  
  //�� 16 λ���������ֽ�(D[15:0])
  for(bitnum0=0; bitnum0 < 16; bitnum0++)
  {
    //SC09A_SCL = 0;
    //PORT_oSCL &=~ PIN_oSCL; //0   
    IIC_SCL_0(); 
    SC09A_DelayuS(); //��ʱ
    
    //SC09A_SCL = 1;
    //PORT_oSCL |= PIN_oSCL; //1
    IIC_SCL_1();
    SC09A_DelayuS(); //��ʱ
    
    //bit_temp0 = PORT_iSDA;
    //if(bit_temp0 & PIN_ioSDA) //�� ack ��Ӧ
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
  SC09A_DelayuS(); //��ʱ
  
  //SC09A_SCL = 1;
  //PORT_oSCL |= PIN_oSCL; //1
  IIC_SCL_1();
  SC09A_DelayuS(); //��ʱ
  
  //SC09A_SCL = 0;
  //PORT_oSCL &=~ PIN_oSCL; //0 
  IIC_SCL_0();
  
  //SC09A_DIR = 0; //output
  //SC09A_SDA = 0; //���� NACK �ź�
  //DDR_ioSDA |= PIN_ioSDA;  //output
  SDA_OUT();
  
  //PORT_oSDA &=~ PIN_ioSDA; //0 //���� NACK �ź�
  IIC_SDA_0();
  SC09A_DelayuS(); //��ʱ
  
  //SC09A_SCL = 1;
  //PORT_oSCL |= PIN_oSCL; //1
  IIC_SCL_1();
  SC09A_DelayuS(); //��ʱ
  
  //SC09A_SDA = 1; //�ͷ� SDA �˿� ,�� SDA ����Ϊ����˿�
  //PORT_oSDA |= PIN_ioSDA; //H
  IIC_SDA_1();
  
  SC09A_DelayuS(); //��ʱ
  //key2byte0 = key2byte0^0xffff;
  
  return(key2byte0);
  //����λΪ 1 ��˵����Ӧ���������������磬����ֵΪ 0x0500 ˵����
  //�� CIN2 �� CIN4 ��������
}


#if 0

//#define PIN_iINT1A          0x20U //PB5 Pin16 AIN5              //I int //SC09A Func ���ж�����
//#define PORT_iINT1A    GPIOB->ODR //PB5 Pin16 AIN5              //I int //SC09A Func ���ж�����

//#define PIN_oSCL1A          0x40U //PB6 Pin15 AIN6              //O     //SC09A Func ʱ�����
//#define PORT_oSCL1A    GPIOB->ODR //PB6 Pin15 AIN6              //O     //SC09A Func ʱ�����

//#define PIN_ioSDA1A         0x80U //PB7 Pin14 AIN7              //IO    //SC09A Func ������
//#define PORT_oSDA1A    GPIOB->ODR //PB7 Pin14 AIN7              //IO    //SC09A Func ������
//#define PORT_iSDA1A    GPIOB->IDR //PB7 Pin14 AIN7              //IO    //SC09A Func ������
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
  SC09A_DelayuS(); //��ʱ
  
  // ���� SDA �˿��ͳ� START �ź�
  //SC09A_SDA = 0;
  PORT_oSDA1A &=~PIN_ioSDA1A; //0  
  SC09A_DelayuS(); //��ʱ
  
  //���� 8 λ��ַ�ֽ� (A[6:0]+RWB)
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
    SC09A_DelayuS(); //��ʱ
    
    //SC09A_SCL = 1;
    PORT_oSCL1A |= PIN_oSCL1A; //1
    SC09A_DelayuS(); //��ʱ
  }
  
  //input //�ͷ� SDA �˿� ,�� SDA ����Ϊ����˿�
  //SC09A_SDA = 1;
  //SC09A_DIR = 1;
  PORT_oSDA1A |= PIN_ioSDA1A; //H
  //GPIOB->DDR  &=~PIN_ioSDA1A;  //input
  DDR_ioSDA1A &=~PIN_ioSDA1A;  //input
    
  //SC09A_SCL = 0;
  PORT_oSCL1A &=~ PIN_oSCL1A; //0 
  SC09A_DelayuS(); //��ʱ
  
  //SC09A_SCL = 1;
  PORT_oSCL1A |= PIN_oSCL1A; //0 
  SC09A_DelayuS(); //��ʱ
  
  //bit_temp = PORT_ioSDA;
  bit_temp = PORT_iSDA1A;
  if(bit_temp & PIN_ioSDA1A) //�� ack ��Ӧ
  {
    //ack �ź�û�ж�����ָʾͨ������
    return(0);
  }
  
  SC09A_DelayuS(); //��ʱ
  
  //�� 16 λ���������ֽ�(D[15:0])
  for(bitnum=0; bitnum < 16; bitnum++)
  {
    //SC09A_SCL = 0;
    PORT_oSCL1A &=~ PIN_oSCL1A; //0    
    SC09A_DelayuS(); //��ʱ
    
    //SC09A_SCL = 1;
    PORT_oSCL1A |= PIN_oSCL1A; //1
    SC09A_DelayuS(); //��ʱ
    
    bit_temp = PORT_iSDA1A;
    if(bit_temp & PIN_ioSDA1A) //�� ack ��Ӧ
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
  SC09A_DelayuS(); //��ʱ
  
  //SC09A_SCL = 1;
  PORT_oSCL1A |= PIN_oSCL1A; //1
  SC09A_DelayuS(); //��ʱ
  
  //SC09A_SCL = 0;
  PORT_oSCL1A &=~ PIN_oSCL1A; //0 
  
  //SC09A_DIR = 0; //output
  //SC09A_SDA = 0; //���� NACK �ź�
  //GPIOB->DDR  |= PIN_ioSDA1A;  //output
  DDR_ioSDA1A |= PIN_ioSDA1A;  //output
  
  PORT_oSDA1A &=~ PIN_ioSDA1A; //0 //���� NACK �ź�
  SC09A_DelayuS(); //��ʱ
  
  //SC09A_SCL = 1;
  PORT_oSCL1A |= PIN_oSCL1A; //1
  SC09A_DelayuS(); //��ʱ
  
  //SC09A_SDA = 1; //�ͷ� SDA �˿� ,�� SDA ����Ϊ����˿�
  PORT_oSDA1A |= PIN_ioSDA1A; //H
  
  SC09A_DelayuS(); //��ʱ
  //key2byte = key2byte^0xffff;
  
  return(key2byte);
  //����λΪ 1 ��˵����Ӧ���������������磬����ֵΪ 0x0500 ˵����
  //�� CIN2 �� CIN4 ��������
  
}
#endif

////////////////////////////////////////////////////////////////////////////////
//                            THE       END                                   //
////////////////////////////////////////////////////////////////////////////////
