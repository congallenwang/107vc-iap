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
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                        //�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);
*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 |GPIO_Pin_5; //Pb3:SCK,Pb5:MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                        //�����������
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

  SPI_FLASH_CS_HIGH();       //CS1-SPI_Flash��1
  SPI_SD_CS_HIGH();          //CS2-Micro SD ��1
  SPI_Touch_CS_HIGH();		 //CS3-SPI_Touch��1
}


void SPI3_Flash_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 |GPIO_Pin_7; //PA5:SCK,PA7:MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                        //�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Confugure PA6:MISO pin as Input Floating  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;                              //PE3:CS(SPI-FLASH)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  SPI_FLASH_CS_HIGH();       //CS1-SPI_Flash��1
  SPI_SD_CS_HIGH();          //CS2-Micro SD ��1
  SPI_Touch_CS_HIGH();		 //CS3-SPI_Touch��1
}


void SPI1_MSD_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7; //PA5:SCK, PA7:MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                        //�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Confugure PA6:MISO pin as Input Floating  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                              //PC12:CS(SPI-Micro SD)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  SPI_FLASH_CS_HIGH();       //CS1-SPI_Flash��1
  SPI_SD_CS_HIGH();          //CS2-Micro SD ��1
  SPI_Touch_CS_HIGH();		 //CS3-SPI_Touch��1
}

//don't use the SPI1 alternate function��just use as GPIO
void SPI1_Touch_GPIO_Configuration(void)		  
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7; //PA5:SCK, PA7:MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                        //�������
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Confugure PA6:MISO pin as Input Floating  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                              //PE4:CS(SPI-Touch)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  SPI_FLASH_CS_HIGH();       //CS1-SPI_Flash��1
  SPI_SD_CS_HIGH();          //CS2-Micro SD ��1
  SPI_Touch_CS_HIGH();       //CS3-SPI_Touch��1
}
						     
void SPI1_Flash_Configuration(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  /* SPI1 configuration */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                     //����SPI����ģʽ:����Ϊ��SPI
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                 //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                        //ѡ���˴���ʱ�ӵ���̬:ʱ�����յ�
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                      //���ݲ����ڵ�һ��ʱ����
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                         //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ4
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
  SPI_InitStructure.SPI_CRCPolynomial = 7;                          //CRCֵ����Ķ���ʽ           
  
  SPI_Init(SPI1, &SPI_InitStructure);
  SPI_Cmd(SPI1, ENABLE);                                            //ʹ��SPI1����
}

void SPI1_MSD_Configuration(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  /* SPI1 configuration */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                     //����SPI����ģʽ:����Ϊ��SPI
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                 //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                       //ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                      //���ݲ����ڵڶ���ʱ����
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                         //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ4
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
  SPI_InitStructure.SPI_CRCPolynomial = 7;                          //CRCֵ����Ķ���ʽ           
  
  SPI_Init(SPI1, &SPI_InitStructure);
  SPI_Cmd(SPI1, ENABLE);                                            //ʹ��SPI1����
}

