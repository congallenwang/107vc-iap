#include <stm32f10x.h>
#include"delay.h"

static u8  fac_us=0;//us延时倍乘数
static u16 fac_ms=0;//ms延时倍乘数
/*********************************************************
@  Fuction:     delay_init
@  Description:    延时初始化
@  Input:  SYSCLK:总线上晶振频率，PLL后
@  Output :NULL
@  Return : NULL
@  Other:   此处用的是内核外部时钟为总线时钟HCLK/8
**********************************************************/
void delay_init(u8 SYSCLK)
{
	SysTick->CTRL&=0xfffffffb;//bit2清空,选择外部时钟  HCLK/8
	fac_us=SYSCLK/8;	//72M，1us 计数为72/8=9	    
	fac_ms=(u16)fac_us*1000;//72M，1ms 计数为72/8*1000=9000
	//倒数9为1us，倒数9000为1ms
}	

/*********************************************************
@  Fuction:     delay_ms
@  Description:    延时ms
@  Input:  nms:
@  Output :NULL
@  Return : NULL
@  Other:   SysTick->LOAD为24位倒计时计数器
                  对于72M，2的24次/8 =1864ms,最大延时
**********************************************************/
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL=0x01 ;          //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	  	    
}  

/*********************************************************
@  Fuction:     delay_us
@  Description:    延时us
@  Input:  nus us输入
@  Output :NULL
@  Return : NULL
@  Other:   NULL
**********************************************************/
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00 ;    //清空计数器	 
}


