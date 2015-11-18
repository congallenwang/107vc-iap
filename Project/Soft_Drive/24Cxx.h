#ifndef _24Cxx_H
#define _24Cxx_H

#include "STM32f10x.h"

#define EEP_Firstpage          0x00

void I2C_24C02_Test(void);
void I2C_EE_Init(void);

void I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr);  //�����ֽ�д��
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite);//ҳд�����24C02�ܹ�֧��8���ֽ�ͬʱд���ܹ����д��ʱ�䡣
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite); //����д�룬���ݵ�ǰ��λ�ú�д��Ĵ�С���������ֽ�д����ҳд
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);//�������
void I2C_EE_WaitEepromStandbyState(void);

#endif
