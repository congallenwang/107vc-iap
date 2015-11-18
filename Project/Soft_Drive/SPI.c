#include "SPI.h"
#include "STM32f10x.h"

#define  SPI_Send16Address(Address) {      \
SPI_Send_Byte( (uint8_t)(Address >> 8) );  \
SPI_Send_Byte( (uint8_t)(Address & 0xff) );\
}

void SPI1_Flash_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

/*
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 |GPIO_Pin_7; //PA5:SCK,PA7:MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                        //复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);
*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 |GPIO_Pin_5; //Pb3:SCK,Pb5:MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                        //复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* Confugure PA6:MISO pin as Input Floating  */
/*
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

/*
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;                              //PE3:CS(SPI-FLASH)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;                              //PE3:CS(SPI-FLASH)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  SPI_FLASH_CS_HIGH();       //CS1-SPI_Flash置1
  SPI_SD_CS_HIGH();          //CS2-Micro SD 置1
  SPI_Touch_CS_HIGH();		 //CS3-SPI_Touch置1
}


void SPI3_Flash_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 |GPIO_Pin_7; //PA5:SCK,PA7:MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                        //复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Confugure PA6:MISO pin as Input Floating  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;                              //PE3:CS(SPI-FLASH)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  SPI_FLASH_CS_HIGH();       //CS1-SPI_Flash置1
  SPI_SD_CS_HIGH();          //CS2-Micro SD 置1
  SPI_Touch_CS_HIGH();		 //CS3-SPI_Touch置1
}


void SPI1_MSD_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7; //PA5:SCK, PA7:MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                        //复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Confugure PA6:MISO pin as Input Floating  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                              //PC12:CS(SPI-Micro SD)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  SPI_FLASH_CS_HIGH();       //CS1-SPI_Flash置1
  SPI_SD_CS_HIGH();          //CS2-Micro SD 置1
  SPI_Touch_CS_HIGH();		 //CS3-SPI_Touch置1
}

//don't use the SPI1 alternate function，just use as GPIO
void SPI1_Touch_GPIO_Configuration(void)		  
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7; //PA5:SCK, PA7:MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                        //推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Confugure PA6:MISO pin as Input Floating  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                              //PE4:CS(SPI-Touch)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  SPI_FLASH_CS_HIGH();       //CS1-SPI_Flash置1
  SPI_SD_CS_HIGH();          //CS2-Micro SD 置1
  SPI_Touch_CS_HIGH();       //CS3-SPI_Touch置1
}
						     
void SPI1_Flash_Configuration(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  /* SPI1 configuration */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                     //设置SPI工作模式:设置为主SPI
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                 //设置SPI的数据大小:SPI发送接收8位帧结构
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                        //选择了串行时钟的稳态:时钟悬空低
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                      //数据捕获于第一个时钟沿
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                         //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//定义波特率预分频的值:波特率预分频值为4
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
  SPI_InitStructure.SPI_CRCPolynomial = 7;                          //CRC值计算的多项式           
  
  SPI_Init(SPI1, &SPI_InitStructure);
  SPI_Cmd(SPI1, ENABLE);                                            //使能SPI1外设
}

void SPI1_MSD_Configuration(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  /* SPI1 configuration */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                     //设置SPI工作模式:设置为主SPI
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                 //设置SPI的数据大小:SPI发送接收8位帧结构
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                       //选择了串行时钟的稳态:时钟悬空高
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                      //数据捕获于第二个时钟沿
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                         //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//定义波特率预分频的值:波特率预分频值为4
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
  SPI_InitStructure.SPI_CRCPolynomial = 7;                          //CRC值计算的多项式           
  
  SPI_Init(SPI1, &SPI_InitStructure);
  SPI_Cmd(SPI1, ENABLE);                                            //使能SPI1外设
}

