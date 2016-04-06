#include <stm32f10x.h>

#include "stmflash.h"
#include "delay.h"
//#include "usart.h"

//����STM32��FLASH
void STMFLASH_Unlock(void)
{
  FLASH->KEYR=FLASH_KEY1;//д���������.
  FLASH->KEYR=FLASH_KEY2;
}
//flash����
void STMFLASH_Lock(void)
{
  FLASH->CR|=1<<7;//����
}
//�õ�FLASH״̬
u8 STMFLASH_GetStatus(void)
{	
	u32 res;		
	res=FLASH->SR; 
	if(res&(1<<0))return 1;		    //æ
	else if(res&(1<<2))return 2;	//��̴���
	else if(res&(1<<4))return 3;	//д��������
	return 0;						//�������
}
//�ȴ��������
//time:Ҫ��ʱ�ĳ���
//����ֵ:״̬.
u8 STMFLASH_WaitDone(u16 time)
{
	u8 res;
	do
	{
		res=STMFLASH_GetStatus();
		if(res!=1)break;//��æ,����ȴ���,ֱ���˳�.
		delay_us(1);
		time--;
	 }while(time);
	 if(time==0)res=0xff;//TIMEOUT
	 return res;
}
//����ҳ
//paddr:ҳ��ַ
//����ֵ:ִ�����
u8 STMFLASH_ErasePage(u32 paddr)
{
	u8 res=0;
	res=STMFLASH_WaitDone(0X5FFF);//�ȴ��ϴβ�������,>20ms    
	if(res==0)
	{ 
		FLASH->CR|=1<<1;//ҳ����
		FLASH->AR=paddr;//����ҳ��ַ 
		FLASH->CR|=1<<6;//��ʼ����		  
		res=STMFLASH_WaitDone(0X5FFF);//�ȴ���������,>20ms  
		if(res!=1)//��æ
		{
			FLASH->CR&=~(1<<1);//���ҳ������־.
		}
	}
	return res;
}
//��FLASHָ����ַд�����
//faddr:ָ����ַ(�˵�ַ����Ϊ2�ı���!!)
//dat:Ҫд�������
//����ֵ:д������
u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat)
{
	u8 res;	   	    
	res=STMFLASH_WaitDone(0XFF);	 
	if(res==0)//OK
	{
		FLASH->CR|=1<<0;//���ʹ��
		*(vu16*)faddr=dat;//д������
		//printf("%x  ",*(vu16*)faddr);
		res=STMFLASH_WaitDone(0XFF);//�ȴ��������
		if(res!=1)//�����ɹ�
		{
			FLASH->CR&=~(1<<0);//���PGλ.
		}
	} 
	return res;
} 
//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
//����ֵ:��Ӧ����.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}
#if STM32_FLASH_WREN	//���ʹ����д   
//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	//printf("WriteAddr[0x%x],NumToWrite[%d]\r\n",WriteAddr,NumToWrite);
	for(i=0;i<NumToWrite;i++)
	{
		STMFLASH_WriteHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//��ַ����2.
	}  
} 
//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
//pBuffer:����ָ��
//NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
//#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 
//#else 
//#define STM_SECTOR_SIZE	2048
//#endif		 
u16 STMFLASH_BUF[STM_SECTOR_SIZE];//�����2K�ֽ�
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //������ַ
	u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	u16 secremain; //������ʣ���ַ(16λ�ּ���)	   
 	u16 i;    
	u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
	int rem,done;

       if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*256)))
            return;//�Ƿ���ַ

	STMFLASH_Unlock();	//����
	offaddr=WriteAddr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַoffaddr=.5000 
	secpos=offaddr/sizeof(STMFLASH_BUF);			        //������
	secoff=offaddr - secpos*sizeof(STMFLASH_BUF);		        //�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain=sizeof(STMFLASH_BUF)-secoff;		//����ʣ��ռ��С   

	//printf("offaddr[%4X],secpos[%d],secoff[%d],secremain[%d]\r\n",offaddr,secpos,secoff,secremain);
	
	//if(NumToWrite<=secremain)
	//	secremain=NumToWrite;//�����ڸ�������Χ

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
			STMFLASH_ErasePage(secpos*sizeof(STMFLASH_BUF)+STM32_FLASH_BASE);//�����������
		}

		//write flash directly
		STMFLASH_Write_NoCheck(WriteAddr,pBuffer,done);//д�Ѿ������˵�,ֱ��д������ʣ������. 	

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
		STMFLASH_Read(secpos*sizeof(STMFLASH_BUF)+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE);//������������������

		for(i=0;i<secremain;i++)//У������
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//��Ҫ����  	  
		}
		
		if(i<secremain)//��Ҫ����
		{
		       printf("1,i=%d,secremain=%d,erase 0x%x\r\n",i,secremain,secpos*sizeof(STMFLASH_BUF)+STM32_FLASH_BASE);
			
			STMFLASH_ErasePage(secpos*sizeof(STMFLASH_BUF)+STM32_FLASH_BASE);//�����������
			for(i=0;i<secremain;i++)//����
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE);//д����������  
		}
		else
		{
			printf("2,i=%d,secremain=%d\r\n",i,secremain);
			STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		}

		if(NumToWrite==secremain)
			break;//д�������
		else//д��δ����
		{
		       printf(" sec plus 1\r\n");
			secpos++;				//������ַ��1
			secoff=0;				//ƫ��λ��Ϊ0 	 
		   	pBuffer+=secremain;  	//ָ��ƫ��
			WriteAddr+=secremain;	//д��ַƫ��	   
		   	NumToWrite-=secremain;	//�ֽ�(16λ)���ݼ�

			if(NumToWrite>STM_SECTOR_SIZE)
               		secremain=STM_SECTOR_SIZE;//��һ����������д����
			else 
				secremain=NumToWrite;//��һ����������д����
		}	
#endif		
	};	
	STMFLASH_Lock();//����
}
#endif

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:��ʼ��ַ
//WriteData:Ҫд�������
void Test_Write(u32 WriteAddr,u16 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//д��һ���� 
}
















