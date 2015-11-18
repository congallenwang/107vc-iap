#include <stm32f10x.h>
#include"delay.h"

static u8  fac_us=0;//us��ʱ������
static u16 fac_ms=0;//ms��ʱ������
/*********************************************************
@  Fuction:     delay_init
@  Description:    ��ʱ��ʼ��
@  Input:  SYSCLK:�����Ͼ���Ƶ�ʣ�PLL��
@  Output :NULL
@  Return : NULL
@  Other:   �˴��õ����ں��ⲿʱ��Ϊ����ʱ��HCLK/8
**********************************************************/
void delay_init(u8 SYSCLK)
{
	SysTick->CTRL&=0xfffffffb;//bit2���,ѡ���ⲿʱ��  HCLK/8
	fac_us=SYSCLK/8;	//72M��1us ����Ϊ72/8=9	    
	fac_ms=(u16)fac_us*1000;//72M��1ms ����Ϊ72/8*1000=9000
	//����9Ϊ1us������9000Ϊ1ms
}	

/*********************************************************
@  Fuction:     delay_ms
@  Description:    ��ʱms
@  Input:  nms:
@  Output :NULL
@  Return : NULL
@  Other:   SysTick->LOADΪ24λ����ʱ������
                  ����72M��2��24��/8 =1864ms,�����ʱ
**********************************************************/
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL=0x01 ;          //��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	  	    
}  

/*********************************************************
@  Fuction:     delay_us
@  Description:    ��ʱus
@  Input:  nus us����
@  Output :NULL
@  Return : NULL
@  Other:   NULL
**********************************************************/
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00 ;    //��ռ�����	 
}


