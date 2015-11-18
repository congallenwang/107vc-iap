#include "SD_MMC.h"
#include "SPI.h"
/*******************************************************************************
* Function Name  : SD_MMC_Low_Speed
* Description    : SD_MMC_Low_Speed
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SD_MMC_Low_Speed(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  /* SPI1 configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  
  SPI_Init(SPI1, &SPI_InitStructure);
  
  SPI_Cmd(SPI1, ENABLE);/* SPI1 enable */
}

/*******************************************************************************
* Function Name  : SD_MMC_High_Speed
* Description    : SD_MMC_High_Speed
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SD_MMC_High_Speed(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  /* SPI1 configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  
  SPI_Init(SPI1, &SPI_InitStructure);

  SPI_Cmd(SPI1, ENABLE);/* SPI1 enable */
}
/*******************************************************************************
* Function Name  : SD_MMC_Init
* Description    : SD_MMC_Init
* Input          : None
* Output         : None
* Return         : zero init success, non-zero init error
*******************************************************************************/
uint8_t SD_MMC_Init(void)
{
  uint8_t response;         /* reset card */
  uint16_t i;

  SD_MMC_Low_Speed();       /* initialize SPI with lowest frequency */
  
  for(i = 0; i < 10; ++ i)  /* card needs 74 cycles minimum to start up */
  {
    SPI_Send_Byte(0x00);    /* wait 8 clock cycles */
  }
  
  SPI_SD_CS_LOW();          /* address card */
  
  for(i = 0; ; ++ i)
  {
    response = SD_MMC_Send_Command(CMD_GO_IDLE_STATE , 0 );
    if( response == 0x01 )
      break;
    if(i == 0x1FF)
    {
      SPI_SD_CS_HIGH();
      return 1;
    }
  }
  
  for( i = 0; ; ++ i) /* wait for card to get ready */
  {
    response = SD_MMC_Send_Command(CMD_SEND_OP_COND, 0);
    if(!(response & (1 << R1_IDLE_STATE)))
      break;

    if(i == 0x7FFF)
    {
      SPI_SD_CS_HIGH();
      return 1;
    }
  }
  
  if(SD_MMC_Send_Command(CMD_SET_BLOCKLEN, 512)) /* set block size to 512 bytes */
  {
    SPI_SD_CS_HIGH();
    return 1;
  }
  
  SPI_SD_CS_HIGH();/* deaddress card */
  SD_MMC_High_Speed();/* switch to highest SPI frequency possible */
  
  return 0;
}

/*******************************************************************************
* Function Name  : SD_MMC_Send_Command
* Description    : SD_MMC_Send_Command
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t SD_MMC_Send_Command(uint8_t cmd, uint32_t arg)
{
  uint8_t Response;
  uint8_t Retry = 0;

  SPI_Send_Byte(0xff);       //高速写命令延时
  
  SPI_SD_CS_LOW();           //片选端置低，选中SD卡
  
  SPI_Send_Byte(cmd | 0x40); //分别写入命令
  SPI_Send_Byte(arg >> 24);
  SPI_Send_Byte(arg >> 16);
  SPI_Send_Byte(arg >> 8);
  SPI_Send_Byte(arg);
  SPI_Send_Byte(0x95);
  
  do
  {
    Response = SPI_Send_Byte(0xff);// 等待响应,响应的开始位为0 
    Retry++;
  }
  while(((Response & 0x80) != 0 ) && ( Retry < 200 ));
  
  SPI_SD_CS_HIGH();          //关闭片选
  
  SPI_Send_Byte(0xff);     //在总线上额外增加8个时钟，让SD卡完成剩下的工作
  
  return Response;  //返回状态值
}


/*******************************************************************************
* Function Name  : SD_MMC_Read_Single_Block
* Description    : SD_MMC_Read_Single_Block
* Input          : sector number and buffer data point
* Output         : None
* Return         : zero success, non-zero error
*******************************************************************************/
uint8_t SD_MMC_Read_Single_Block(u32 sector, uint8_t* buffer)
{
  uint8_t Response;
  uint16_t i;
  uint16_t Retry = 0;
  
  Response = SD_MMC_Send_Command(CMD_READ_SINGLE_BLOCK, sector<<9);//读命令 send read command
  
  if(Response != 0x00)
    return Response;
  
  SPI_SD_CS_LOW();
  
  while(SPI_Send_Byte(0xff) != 0xfe) // start byte 0xfe
  {
    if(++Retry > 0xfffe)
    {
      SPI_SD_CS_HIGH();
      return 1; //timeout
    }
  }

  for(i = 0; i < 512; ++i)
  {
    *buffer++ = SPI_Send_Byte(0xff); //读512个数据
  }

  SPI_Send_Byte(0xff);  //伪crc
  SPI_Send_Byte(0xff);  //伪crc

  SPI_SD_CS_HIGH();
  SPI_Send_Byte(0xff);  // extra 8 CLK

  return 0;
}


/*******************************************************************************
* Function Name  : SD_MMC_Write_Single_Block
* Description    : SD_MMC_Write_Single_Block
* Input          : sector number and buffer data point
* Output         : None
* Return         : zero success, non-zero error.
*******************************************************************************/
uint8_t SD_MMC_Write_Single_Block(u32 sector, uint8_t* buffer)
{
  uint8_t Response;
  uint16_t i;
  uint16_t retry=0;
  
  Response = SD_MMC_Send_Command(CMD_WRITE_SINGLE_BLOCK, sector<<9);//写命令  send write command
  if(Response != 0x00)
    return Response;

  SPI_SD_CS_LOW();
	
  SPI_Send_Byte(0xff);
  SPI_Send_Byte(0xff);
  SPI_Send_Byte(0xff);
  
  SPI_Send_Byte(0xfe); //发开始符	start byte 0xfe
  
  for(i=0; i<512; i++) //送512字节数据	send 512 bytes data
  {
    SPI_Send_Byte(*buffer++);
  }
	
  SPI_Send_Byte(0xff); //dummy crc
  SPI_Send_Byte(0xff); //dummy crc
	
  Response = SPI_Send_Byte(0xff);
  
  if( (Response&0x1f) != 0x05)//等待是否成功	judge if it successful
  {
    SPI_SD_CS_HIGH();
    return Response;
  }
  
  while(SPI_Send_Byte(0xff) != 0x00)//等待操作完  wait no busy
  {
    if(retry++ > 0xfffe)
    {
      SPI_SD_CS_HIGH();
      return 1;
    }
  }
  
  SPI_SD_CS_HIGH();
  SPI_Send_Byte(0xff);             // extra 8 CLK
  
  return 0;
}
/*********************************************************************************
*从SD卡中读回指定长度的数据，放置在给定位置
*输入: uint8_t *data(存放读回数据的内存>len)
*      uint16_t len(数据长度）	 
*返回值:0：NO_ERR
*  	other：错误信息	
********************************************************************************/
uint8_t SD_MMC_ReceiveData(uint8_t *data, uint16_t len)
{
  uint16_t i;
  uint16_t Retry = 0;
  
  SPI_SD_CS_LOW();	             // 启动一次传输	
  
  while(SPI_Send_Byte(0xff) != 0xfe) //等待SD卡发回数据起始令牌0xFE
  {
    if(++Retry > 0xfffe)
    {
      SPI_SD_CS_HIGH();
      return 1; //timeout
    }
  }
  
  for(i = 0; i < len; ++ i)
  {
    *data ++ = SPI_Send_Byte(0xff); //读512个数据
  }
	
  SPI_Send_Byte(0xFF);   //2个伪CRC（dummy CRC）
  SPI_Send_Byte(0xFF);

  SPI_SD_CS_HIGH();      //结束本次传输
  SPI_Send_Byte(0xff);  // extra 8 CLK
												  					    
  return 0;
}
/******************************************************************************* 
*获取SD卡的CID信息，包括制造商信息
*输入: uint8_t *cid_data(存放CID的内存，至少16Byte）	  
*返回值:0：NO_ERR
*	1：TIME_OUT
*   other：错误信息	
*******************************************************************************/
uint8_t SD_MMC_GetCID(uint8_t *cid_data)
{
  uint8_t r1;	
  r1 = SD_MMC_Send_Command(CMD_SEND_CID,0);               //发CID命令，读CID
  if(r1 != 0x00) return r1;                               //没返回正确应答，则退出，报错
  SD_MMC_ReceiveData(cid_data,16);                        //接收16个字节的数据	 
  return 0;
}		
/******************************************************************************																			  
*获取SD卡的CSD信息，包括容量和速度信息
*输入:uint8_t *cid_data(存放CID的内存，至少16Byte）	    
*返回值:0：NO_ERR
*       1：TIME_OUT
*       other：错误信息		
*******************************************************************************/
uint8_t SD_MMC_GetCSD(uint8_t *csd_data)
{
  uint8_t r1;	
  r1=SD_MMC_Send_Command(CMD_SEND_CSD,0);                 //发CSD命令，读CSD
  if(r1) return r1;                                       //没返回正确应答，则退出，报错
  SD_MMC_ReceiveData(csd_data, 16);                       //接收16个字节的数据 
  return 0;
}  
/*******************************************************************************	
*获取SD卡的容量（字节）   
*返回值:0： 取容量出错 
*      其他:SD卡的容量(字节)	
********************************************************************************/
u32 SD_MMC_GetCapacity(void)
{
  uint8_t csd[16];
  u32 Capacity;
  uint8_t r1;
  uint16_t i;
  uint16_t temp;  	
  
  if(SD_MMC_GetCSD(csd)!=0) return 0;	   //取CSD信息，如果期间出错，返回0 
  if((csd[0]&0xC0)==0x40)                  //如果为SDHC卡，按照下面方式计算
  {			
    Capacity=((u32)csd[8])<<8;
    Capacity+=(u32)csd[9]+1;	
    Capacity = (Capacity)*1024;//得到扇区数
    Capacity*=512;//得到字节数	
  }
  else
  {		
    i = csd[6]&0x03;
    i<<=8;
    i += csd[7];
    i<<=2;
    i += ((csd[8]&0xc0)>>6);
    r1 = csd[9]&0x03;//C_SIZE_MULT
    r1<<=1;
    r1 += ((csd[10]&0x80)>>7);	
    r1+=2;//BLOCKNR
    temp = 1;
    while(r1)
    {
      temp*=2;
      r1--;
    }
    Capacity = ((u32)(i+1))*((u32)temp);
    // READ_BL_LEN
    i = csd[5]&0x0f;
    //BLOCK_LEN
    temp = 1;
    while(i)
    {
      temp*=2;
      i--;
    }
    //The final result
    Capacity *= (u32)temp;//字节为单位 
  }
  return (u32)Capacity;
}
/*******************************************************************************
* Function Name  : SPI_SendByte
* Description    : Sends a byte through the SPI interface and return the byte 
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
uint8_t SPI_Send_Byte(uint8_t byte)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, byte);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);                                              //返回通过SPIx最近接收的数据
}
