#include "stm32f10x_i2c.h"
#include "I2C.h"
#include <stdio.h>

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configure the used I/O ports pin
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
   
  /* Configure I2C1 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : I2C_Configuration
* Description    : I2C Configuration
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_Configuration(void)
{
  I2C_InitTypeDef  I2C_InitStructure; 
  /* I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                   //设置 I2C为 I2C模式
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                           //I2C快速模式 Tlow / Thigh = 2
  I2C_InitStructure.I2C_OwnAddress1 = I2C1_SLAVE_ADDRESS7;                     //设置第一个设备地址
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                                  //使能应答
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;    //应答 7位地址
  I2C_InitStructure.I2C_ClockSpeed = I2C1_Speed;                               //设置时钟频率
  
  I2C_Cmd(I2C1, ENABLE);/* I2C Peripheral Enable */
  I2C_Init(I2C1, &I2C_InitStructure); /* Apply I2C configuration after enabling it */
  I2C_AcknowledgeConfig(I2C1, ENABLE);/*允许1字节1应答模式*/
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
