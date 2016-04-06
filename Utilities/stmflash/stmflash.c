#include <stm32f10x.h>

#include "stmflash.h"
#include "delay.h"
//#include "usart.h"

//解锁STM32的FLASH
void STMFLASH_Unlock(void)
{
  FLASH->KEYR=FLASH_KEY1;//写入解锁序列.
  FLASH->KEYR=FLASH_KEY2;
}
//flash上锁
void STMFLASH_Lock(void)
{
  FLASH->CR|=1<<7;//上锁
}
//得到FLASH状态
u8 STMFLASH_GetStatus(void)
{	
	u32 res;		
	res=FLASH->SR; 
	if(res&(1<<0))return 1;		    //忙
	else if(res&(1<<2))return 2;	//编程错误
	else if(res&(1<<4))return 3;	//写保护错误
	return 0;						//操作完成
}
//等待操作完成
//time:要延时的长短
//返回值:状态.
u8 STMFLASH_WaitDone(u16 time)
{
	u8 res;
	do
	{
		res=STMFLASH_GetStatus();
		if(res!=1)break;//非忙,无需等待了,直接退出.
		delay_us(1);
		time--;
	 }while(time);
	 if(time==0)res=0xff;//TIMEOUT
	 return res;
}
//擦除页
//paddr:页地址
//返回值:执行情况
u8 STMFLASH_ErasePage(u32 paddr)
{
	u8 res=0;
	res=STMFLASH_WaitDone(0X5FFF);//等待上次操作结束,>20ms    
	if(res==0)
	{ 
		FLASH->CR|=1<<1;//页擦除
		FLASH->AR=paddr;//设置页地址 
		FLASH->CR|=1<<6;//开始擦除		  
		res=STMFLASH_WaitDone(0X5FFF);//等待操作结束,>20ms  
		if(res!=1)//非忙
		{
			FLASH->CR&=~(1<<1);//清除页擦除标志.
		}
	}
	return res;
}
//在FLASH指定地址写入半字
//faddr:指定地址(此地址必须为2的倍数!!)
//dat:要写入的数据
//返回值:写入的情况
u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat)
{
	u8 res;	   	    
	res=STMFLASH_WaitDone(0XFF);	 
	if(res==0)//OK
	{
		FLASH->CR|=1<<0;//编程使能
		*(vu16*)faddr=dat;//写入数据
		//printf("%x  ",*(vu16*)faddr);
		res=STMFLASH_WaitDone(0XFF);//等待操作完成
		if(res!=1)//操作成功
		{
			FLASH->CR&=~(1<<0);//清除PG位.
		}
	} 
	return res;
} 
//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}
#if STM32_FLASH_WREN	//如果使能了写   
//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	//printf("WriteAddr[0x%x],NumToWrite[%d]\r\n",WriteAddr,NumToWrite);
	for(i=0;i<NumToWrite;i++)
	{
		STMFLASH_WriteHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//地址增加2.
	}  
} 
//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
//#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 
//#else 
//#define STM_SECTOR_SIZE	2048
//#endif		 
u16 STMFLASH_BUF[STM_SECTOR_SIZE];//最多是2K字节
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)	   
 	u16 i;    
	u32 offaddr;   //去掉0X08000000后的地址
	int rem,done;

       if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*256)))
            return;//非法地址

	STMFLASH_Unlock();	//解锁
	offaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址offaddr=.5000 
	secpos=offaddr/sizeof(STMFLASH_BUF);			        //扇区号
	secoff=offaddr - secpos*sizeof(STMFLASH_BUF);		        //在扇区内的偏移(2个字节为基本单位.)
	secremain=sizeof(STMFLASH_BUF)-secoff;		//扇区剩余空间大小   

	//printf("offaddr[%4X],secpos[%d],secoff[%d],secremain[%d]\r\n",offaddr,secpos,secoff,secremain);
	
	//if(NumToWrite<=secremain)
	//	secremain=NumToWrite;//不大于该扇区范围

	if(NumToWrite>STM_SECTOR_SIZE)
		done = STM_SECTOR_SIZE;
	else
		done = NumToWrite;	
	
	while(1) 
	{	
#if 1
		if(secoff==0)
		{
			//printf("erase 0x%x",secpos*sizeof(STMFLASH_BUF)+STM32_FLASH_BASE);
			STMFLASH_ErasePage(secpos*sizeof(STMFLASH_BUF)+STM32_FLASH_BASE);//擦除这个扇区
		}

		//write flash directly
		STMFLASH_Write_NoCheck(WriteAddr,pBuffer,done);//写已经擦除了的,直接写入扇区剩余区间. 	

		rem = NumToWrite - done;
		if(rem>0)
		{
			WriteAddr += sizeof(STMFLASH_BUF);
			pBuffer += STM_SECTOR_SIZE;
			done = rem>STM_SECTOR_SIZE?STM_SECTOR_SIZE:rem;
		}
		else
			break;
#else
		STMFLASH_Read(secpos*sizeof(STMFLASH_BUF)+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE);//读出整个扇区的内容

		for(i=0;i<secremain;i++)//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		
		if(i<secremain)//需要擦除
		{
		       printf("1,i=%d,secremain=%d,erase 0x%x\r\n",i,secremain,secpos*sizeof(STMFLASH_BUF)+STM32_FLASH_BASE);
			
			STMFLASH_ErasePage(secpos*sizeof(STMFLASH_BUF)+STM32_FLASH_BASE);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE);//写入整个扇区  
		}
		else
		{
			printf("2,i=%d,secremain=%d\r\n",i,secremain);
			STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		}

		if(NumToWrite==secremain)
			break;//写入结束了
		else//写入未结束
		{
		       printf(" sec plus 1\r\n");
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain;	//写地址偏移	   
		   	NumToWrite-=secremain;	//字节(16位)数递减

			if(NumToWrite>STM_SECTOR_SIZE)
               		secremain=STM_SECTOR_SIZE;//下一个扇区还是写不完
			else 
				secremain=NumToWrite;//下一个扇区可以写完了
		}	
#endif		
	};	
	STMFLASH_Lock();//上锁
}
#endif

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:起始地址
//WriteData:要写入的数据
void Test_Write(u32 WriteAddr,u16 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//写入一个字 
}
















