
#include "main.h"
#include "stm32f0xx_hal.h"


GPIO_InitTypeDef        GPIO_InitStructure;  
   
#define BYTE uint8_t

void delay_us(void)
{ 
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
    __ASM("nop");
}

void delayus(unsigned long time)
{
    while(time--)
      delay_us();
}


/**
  * @brief  Set SDA Pin as Output Mode
  * @retval None
  */
void SDA_OUT()  
{  
  memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitTypeDef));

  GPIO_InitStructure.Pin = sda_Pin;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(sda_GPIO_Port, &GPIO_InitStructure);
}  

/**
  * @brief  Set SDA Pin as Input Mode
  * @retval None
  */
void SDA_IN()  
{

    memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitTypeDef));

    GPIO_InitStructure.Pin = sda_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(scl_GPIO_Port, &GPIO_InitStructure);
} 

/**
  * @brief  read input voltage from SDA pin
  * @retval None
  */
GPIO_PinState SDA_READ()
{
  return HAL_GPIO_ReadPin(sda_GPIO_Port, sda_Pin);
}

/**
  * @brief  output high form SDA pin
  * @retval None
  */
void IIC_SDA_1()
{
  HAL_GPIO_WritePin(sda_GPIO_Port, sda_Pin, 1);
}

/**
  * @brief  output low form SDA pin
  * @retval None
  */
void IIC_SDA_0()
{
  //GPIO_ResetBits(GPIOF, GPIO_Pin_7);
  
    HAL_GPIO_WritePin(sda_GPIO_Port, sda_Pin, 0);
}

/**
  * @brief  output high form SCL pin
  * @retval None
  */
void IIC_SCL_1()
{
//GPIO_SetBits(GPIOF, GPIO_Pin_6);

    HAL_GPIO_WritePin(scl_GPIO_Port, scl_Pin, 1);
}

/**
  * @brief  output LOW form SCL pin
  * @retval None
  */
void IIC_SCL_0()
{
    HAL_GPIO_WritePin(scl_GPIO_Port, scl_Pin, 0);
}


/**
* @brief  Simulate IIC conmunication :Create Start signal
  * @retval None
  */
void IIC_Start(void)
{
	SDA_OUT();     //sda output
	IIC_SDA_1();
	IIC_SCL_1();
	delayus(4);
 	IIC_SDA_0();   //START:when CLK is high,DATA change form high to low 
	delayus(4);
	IIC_SCL_0();   //hold scl line, prepare to transmit data
}

/**
  * @brief  Simulate IIC conmunication : Create Stop signal
  * @retval None
  */
void IIC_Stop(void)
{
	SDA_OUT();    //sda output mode 
	IIC_SCL_0();
	IIC_SDA_0();  //STOP:when CLK is high DATA change form low to high
 	delayus(4);
	IIC_SCL_1(); 
	IIC_SDA_1();  //indicate transmit over
	delayus(4);
}

/**
* @brief  Simulate IIC conmunication : wait for target device's ACK
* @retval ACK (0) : receive success
* @retval NACK(1) : receive unsuccess
  */
BYTE IIC_Wait_Ack(void)
{
	BYTE ucErrTime = 0;
	SDA_IN();      //set as input mode
	IIC_SDA_1();
	delayus(1);
	IIC_SCL_1();
	delayus(1);
	while(SDA_READ())
	{
		ucErrTime++;
		if(ucErrTime > 250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_0(); //release scl line
	return 0;  
} 

/**
  * @brief  Simulate IIC conmunication : make an ACK
  * @retval None
  */
void IIC_Ack(void)
{
	IIC_SCL_0();
	SDA_OUT();
	IIC_SDA_0();
	delayus(2);
	IIC_SCL_1();
	delayus(2);
	IIC_SCL_0();
}

/**
  * @brief  Simulate IIC conmunication : don't make an ACK
  * @retval None
  */
void IIC_NAck(void)
{
	IIC_SCL_0();
	SDA_OUT();
	IIC_SDA_1();
	delayus(2);
	IIC_SCL_1();
	delayus(2);
	IIC_SCL_0();
}


/**
  * @brief  Simulate IIC conmunication : Transmit one byte Data
  * @param  txd: data to be transmit
  * @retval None
  */
void IIC_Send_Byte(BYTE txd)
{                        
  BYTE i;   
  SDA_OUT(); 	    
  IIC_SCL_0();//push down scl  to start transmit data
  //delayus(2);
  for(i = 0; i < 8; ++i)
  {
    if(txd & 0x80)
    {
      IIC_SDA_1();
    }
    else
    {
      IIC_SDA_0();
    }
    txd <<= 1;
    delayus(2);
    IIC_SCL_1();
    delayus(6);
    IIC_SCL_0();
    delayus(2);
  }

  IIC_Wait_Ack();//hzc +
}

//????1????????????????ack=1????????????????ACK????ack=0????????????nACK   
/**
  * @brief  Simulate IIC conmunication : Receive one byte Data
  * @param  ack: Whether transmit ACK
  * @retval the data have been receive
  */
BYTE IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, res = 0;
	SDA_IN();               //SDA input mode
  for(i = 0; i < 8; ++i )
	{
    IIC_SCL_0(); 
    delayus(2);
    IIC_SCL_1();
    res <<= 1;
    if(SDA_READ())
    {
      res++; 
    }      
		delayus(1); 
  }
  if (!ack)
  {
    IIC_NAck();//make NACK
  }
  else
  {
    IIC_Ack(); //make ACK
  }
  return res;
}

/*
I2C?????????
addr?????????????????????
buf???????????????
len????????????????????????
*/
void i2c_read(unsigned char addr, unsigned char* buf, int len)
{
	int i;
	unsigned char t;
	IIC_Start();                        //?????????????????????????????????
	//?????????????????????????????????
	t = (addr << 1) | 1;                //?????????1??????????????????
	IIC_Send_Byte(t);
	//????????????
	for (i=0; i<len; i++)
		buf[i] = IIC_Read_Byte(0);        //send ack
	IIC_Stop();                         //?????????????????????????????????
}

/*
I2C?????????
addr?????????????????????
buf???????????????
len????????????????????????
*/
void i2c_write(unsigned char addr, unsigned char* buf, int len)
{
	int i;
	unsigned char t;
	IIC_Start();                        //?????????????????????????????????
	//?????????????????????????????????
	t = (addr << 1) | 0;                    //?????????0??????????????????
	IIC_Send_Byte(t);
	//????????????
	for (i=0; i<len; i++)
	    IIC_Send_Byte(buf[i]);
	IIC_Stop();                     //?????????????????????????????????
}

