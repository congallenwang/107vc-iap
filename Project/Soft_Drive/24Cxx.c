#include "24Cxx.h"
#include "I2C.h"
#include <stdio.h>

#define EEPROM_Block0_ADDRESS 0xA0
u16 EEPROM_ADDRESS;

void I2C_24C02_Test(void)
{
  u16 i;
  u8 I2C_Buf_Write[32];  //写缓冲区
  u8 I2C_Buf_Read[32];   //读缓冲区
  
  printf("AT24C02 Writing...\n\r");
  
  for(i=0;i<=31;i++)    //填充缓冲
  {   
    I2C_Buf_Write[i]=i;
    printf("0x%02X ",I2C_Buf_Write[i]);
    if(i%16 == 15)
    {
      printf("\n\r");
    }
  }
  
  I2C_EE_BufferWrite(I2C_Buf_Write,EEP_Firstpage,32);  //将I2c_Buf_Write中顺序递增的数据写入EERPOM中 
  printf("\n\rAT24C02 Reading...\n\r");
  I2C_EE_BufferRead(I2C_Buf_Read,EEP_Firstpage,32);    //将EEPROM读出数据顺序保持到I2c_Buf_Read中 
  
  for(i=0;i<32;i++)                                    //将I2c_Buf_Read中的数据通过串口打印
  {	
    if(I2C_Buf_Read[i]!=I2C_Buf_Write[i])
    {
      printf("0x%02X ", I2C_Buf_Read[i]);
      printf("Err:Data Read and Write are't Matching.\n\r");
      return;
    }
    printf("0x%02X ", I2C_Buf_Read[i]);
    if(i%16 == 15)
    {
      printf("\n\r");
    }
  }
  printf("I2C-AT24C02 Test Passed!\n\r");
}
/*******************************************************************************
* Function Name  : I2C_EE_Init
* Description    : Initializes peripherals used by the I2C EEPROM driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_Init()
{
    EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;
}

/*******************************************************************************
* Function Name  : I2C_EE_BufferWrite
* Description    : Writes buffer of data to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  Addr = WriteAddr % I2C_PageSize;
  count = I2C_PageSize - Addr;
  NumOfPage =  NumByteToWrite / I2C_PageSize;
  NumOfSingle = NumByteToWrite % I2C_PageSize;
 
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize); 
    	I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;
      }

      if(NumOfSingle!=0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;	
      
      if(count != 0)
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }
      if(NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); 
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }  
}

/*******************************************************************************
* Function Name  : I2C_EE_ByteWrite
* Description    : Writes one byte to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr)
{
  /* Send STRAT condition */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));  

  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
      
  /* Send the EEPROM's internal address to write to */
  I2C_SendData(I2C1, WriteAddr);
  
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* Send the byte to be written */
  I2C_SendData(I2C1, *pBuffer); 
   
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_PageWrite
* Description    : Writes more than one byte to the EEPROM with a single WRITE
*                  cycle. The number of byte can't exceed the EEPROM page size.
* Input          : - pBuffer : pointer to the buffer containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
  /* While the bus is busy */
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
  
  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); 
  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  

  /* Send the EEPROM's internal address to write to */    
  I2C_SendData(I2C1, WriteAddr);  

  /* Test on EV8 and clear it */
  while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* While there is data to be written */
  while(NumByteToWrite--)  
  {
    /* Send the current byte */
    I2C_SendData(I2C1, *pBuffer); 

    /* Point to the next byte to be written */
    pBuffer++; 
  
    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  }

  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_BufferRead
* Description    : Reads a block of data from the EEPROM.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the EEPROM.
*                  - ReadAddr : EEPROM's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{  
  /* While the bus is busy */
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
  
  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
   
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C1, ENABLE);

  /* Send the EEPROM's internal address to write to */
  I2C_SendData(I2C1, ReadAddr);  

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  
  /* Send EEPROM address for read */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Receiver);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  
  /* While there is data to be read */
  while(NumByteToRead)  
  {
    if(NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(I2C1, DISABLE);
      
      /* Send STOP Condition */
      I2C_GenerateSTOP(I2C1, ENABLE);
    }

    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))  
    {      
      /* Read a byte from the EEPROM */
      *pBuffer = I2C_ReceiveData(I2C1);

      /* Point to the next location where the byte read will be saved */
      pBuffer++; 
      
      /* Decrement the read bytes counter */
      NumByteToRead--;        
    }   
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C1, ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_WaitEepromStandbyState
* Description    : Wait for EEPROM Standby state
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_WaitEepromStandbyState(void)      
{
//  vu16 SR1_Tmp = 0;

//  do
//  {
//    /* Send START condition */
//    I2C_GenerateSTART(I2C1, ENABLE);
//    /* Read I2C1 SR1 register */
//    SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
//    /* Send EEPROM address for write */
//    I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);
//  }
//  while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));
//  
//  /* Clear AF flag */
//  I2C_ClearFlag(I2C1, I2C_FLAG_AF);
//	/* STOP condition */    
//    I2C_GenerateSTOP(I2C1, ENABLE); // Added by Najoua 27/08/2008
	
	while(1)
	{
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
		
		/* Send START condition */
		I2C_GenerateSTART(I2C1, ENABLE);
		/* Test on EV5 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
		
		/* Send EEPROM address for write */
		I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);
		
		/* Test on EV6 and clear it */
		while( (!I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR)) && (!I2C_GetFlagStatus(I2C1, I2C_FLAG_AF)) );
		if(I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR))
		{
			break;
		}
		if(I2C_GetFlagStatus(I2C1, I2C_FLAG_AF))
		{
			I2C_ClearFlag(I2C1,I2C_FLAG_AF);
			I2C_GenerateSTOP(I2C1, ENABLE);
			while(I2C1->CR1&0x0200);
		}
	}

	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_GenerateSTOP(I2C1, ENABLE);
	while(I2C1->CR1&0x0200);  
}
