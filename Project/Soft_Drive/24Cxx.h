#ifndef _24Cxx_H
#define _24Cxx_H

#include "STM32f10x.h"

#define EEP_Firstpage          0x00

void I2C_24C02_Test(void);
void I2C_EE_Init(void);

void I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr);  //单个字节写入
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite);//页写，这款24C02能够支持8个字节同时写，能够提高写入时间。
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite); //缓存写入，根据当前的位置和写入的大小，调用是字节写还是页写
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);//缓存读出
void I2C_EE_WaitEepromStandbyState(void);

#endif
