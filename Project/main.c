/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    04/16/2010
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "string.h"
#include "stm32f10x.h"
#include "SysTick.h"
#include "USART.h"
#include "SPI.h"
#include "GD25Qxx.h"
#include "stmflash.h"


//#define IDC_FW
#if 0
#ifdef IDC_FW
	#include "idc_fw.c"
	int g_fw=0;
#else
	#include "dsl_fw.c"
	int g_fw=1;
#endif
#endif
//#include "uip_web.bin.c"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define TxBufferSize   (countof(TxBuffer) - 1)
#define RxBufferSize   0xFF
/* Private macro -------------------------------------------------------------*/
#define  Version_ID             "1.0.0000"    //software version
#define  countof(a)             (sizeof(a) / sizeof(*(a)))
/* Private variables ---------------------------------------------------------*/
uint32_t IntDeviceSerial[3];                  //store the Device unique code
uint8_t  count;

/* Extern variables ----------------------------------------------------------*/
     //#define  sFLASH_ID              0xC84015            //GigaDevice SPI_Flash  GD25Q16B
     #define  sFLASH_ID              0xEF4015            //GigaDevice SPI_Flash  GD25Q16B
     #define  FLASH_WriteAddress     0x000000
     #define  FLASH_ReadAddress      FLASH_WriteAddress
     uint8_t  Tx_Buffer[256];
     uint8_t  Rx_Buffer[256];
     uint32_t Manufact_ID = 0;
     uint32_t DeviceID = 0;
	 uint16_t i = 0;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void Turn_On_LED(uint8_t LED_NUM);

/* Private functions ---------------------------------------------------------*/
void Get_ChipSerialNum(void)
{
  IntDeviceSerial[0] = *(__IO uint32_t*)(0x1FFFF7E8);
  IntDeviceSerial[1] = *(__IO uint32_t*)(0x1FFFF7EC);
  IntDeviceSerial[2] = *(__IO uint32_t*)(0x1FFFF7F0);
}


void LED_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_ResetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_2);
  GPIO_ResetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1);/*关闭所有的LED指示灯*/
}

void RCC_Configuration(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);          
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO  , ENABLE);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);                       // Enable USART1 clocks 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);                         // Enable SPI1 clocks
}
/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures NVIC and Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
  /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);  
#endif
}
/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern unsigned int fw_len;
extern const unsigned char image1[];


#define FW_OFFSET 0x100000


#define FLASH_APP1_ADDR		0x08005000  	//第一个应用程序起始地址(存放在FLASH)
typedef  void (*iapfun)(void);				//定义一个函数类型的参数.   
iapfun jump2app; 
u16 iapbuf[512];   
//appxaddr:应用程序的起始地址
//appbuf:应用程序CODE.
//appsize:应用程序大小(字节).
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize)//2556
{
	u16 t;
	u16 i=0,j;
	u16 temp;
	u32 fwaddr=appxaddr;//当前写入的地址
	u8 *dfu=appbuf;

	for(t=0;t<appsize;t+=2)
	{						    
		temp=(u16)dfu[1]<<8;
		temp+=(u16)dfu[0];	  
		dfu+=2;//偏移2个字节
		iapbuf[i++]=temp;	    
		if(i==512)
		{
			i=0;
			STMFLASH_Write(fwaddr,iapbuf,512);	
			fwaddr+=1024;//偏移1024 16=2*8.所以要乘以2.
		}
	}

#if 0
	printf("\r\ndump buf,fwaddr=0x%4X:\r\n",fwaddr);
	for(j=0;j<i;j++)
	{
		printf("%04X ",iapbuf[j]);
		if(j%16==15)
			printf("\r\n");
	}
#endif	
	if(i)
		STMFLASH_Write(fwaddr,iapbuf,i);//将最后的一些内容字节写进去.  
}

__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0             //set Main Stack value
    BX r14
}


void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}
}	


#define APP_OFFSET 0x200000
u16 Buffer[512];
const unsigned char* p_app;
int app_len;

int main(void)
{
  uint32_t write_addr;
  uint8_t flag=0;
  int i=0,j;
  int size,total;
  
  RCC_Configuration();
  NVIC_Configuration();
  SysTick_Configuration();

  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
  LED_GPIO_Configuration();

  SPI1_Flash_GPIO_Configuration();
  SPI1_Flash_Configuration();

  USART1_GPIO_Configuration();
  USART1_Configuration();

  *(__IO uint32_t*)(AFIO_BASE+0x04) |= 0x1;
  
/* GD32103C-EVAL Start up */
  printf("\n\r###############################################################################\n\r");
  printf("\n\rGD32107C-EVAL Allen System is Starting up...\n\r");
  printf("\n\rGD32107C-EVAL Program Version number:GD%s\n\r",Version_ID);
  printf("\n\rGD32107C-EVAL Program Compile time:("__DATE__" - "__TIME__")");
  printf("\r\n");
  printf("\n\rGD32107C-EVAL STM32F10x_StdPeriph_Version:%d.%d.%d\n\r",
			__STM32F10X_STDPERIPH_VERSION_MAIN,
			__STM32F10X_STDPERIPH_VERSION_SUB1,
			__STM32F10X_STDPERIPH_VERSION_SUB2);
  printf("\n\rGD32107C-EVAL SystemCoreClock:%dHz\n\r",SystemCoreClock);
  printf("\n\rGD32107C-EVAL Flash:%dK字节\n\r",*(__IO uint16_t*)(0x1FFFF7E0));

  printf("\n AFIO_BASE = 0x%x\n",AFIO_BASE);
  
  Get_ChipSerialNum();  //获取Unique Device ID 
  
  printf("\n\rGD32107C-EVAL The CPU Unique Device ID:[%X-%X-%X]\n\r",IntDeviceSerial[2],IntDeviceSerial[1],IntDeviceSerial[0]);                       

  printf("\n\rGD32107C-EVAL SPI Flash:GD25Q16 configured...\n\r");

  printf("\n\r read AFIO_PCFR = 0x%x\n\r",*(__IO uint32_t*)(AFIO_BASE+0x04));

  //printf("stop here******************\n");
  //while(1);
  
  Manufact_ID = SPI_FLASH_ReadID();
  printf("\n\rThe Flash_ID:0x%X\n\r", Manufact_ID);


  	
  printf("\n\r read AFIO_BASE = 0x%x\n\r",*(__IO uint32_t*)(AFIO_BASE));  
  printf("\n\r read AFIO_PCFR = 0x%x\n\r",*(__IO uint32_t*)(AFIO_BASE+0x04));
	
  if (Manufact_ID == sFLASH_ID)   
  {
  	//p_app =(const unsigned char*)rom_data;
	//app_len= rom_size * 4;
	app_len= 153600;

#if 0
       i = app_len/4096 + 1;
	printf("erase %d sector\r\n",i);
	while(i)
	{
		SPI_FLASH_SectorErase(0x200000 + (i-1)*4096);
		i--;
	}
		
	SPI_FLASH_BufferWrite((uint8_t*)p_app,0x200000,app_len);//从FLASH_WriteAddress处开始,写入SIZE长度的数据
	Delay_100us(10);
#endif

	Delay_100us(10);
	printf("\n\>>>>>>>>>>>>>>>>>>>>rRead from app_Buffer:\n\r");

/*
	SPI_FLASH_BufferRead(Rx_Buffer,0x200000, 256);  //从FLASH_ReadAddress处开始,读入BufferSize长度的数据

	for(i=0; i<=255; i++)    //将Rx_Buffer中的数据通过串口打印
	{	
		printf("0x%02X ", Rx_Buffer[i]);
		if(i%16 == 15)
		{
			 printf("\n\r");
		}
	}

	SPI_FLASH_BufferRead(Rx_Buffer,0x200000 + app_len - app_len%256 , 256);  //从FLASH_ReadAddress处开始,读入BufferSize长度的数据
       printf(">>>>>>>>>>>>>>addr:%x\r\n",0x200000 + app_len - app_len%256);
	for(i=0; i<=255; i++)    //将Rx_Buffer中的数据通过串口打印
	{	
		printf("0x%02X ", Rx_Buffer[i]);
		if(i%16 == 15)
		{
			 printf("\n\r");
		}
	}
*/
#if 1
	//copy from spi flash to stmflash
	i=0; total=0;
	while(1)
	{
		size = 256;
		if(total+256>app_len)
			size = app_len -total;
		SPI_FLASH_BufferRead(Rx_Buffer,0x100000 + i*256, 256);  //从FLASH_ReadAddress处开始,读入BufferSize长度的数据

#if 0
		if(0 )//|| size<256)
		{
			printf("dump, i=%d\r\n",i);
			
			for(j=0; j<=255; j++)    //将Rx_Buffer中的数据通过串口打印
			{	
				printf("0x%02X ", Rx_Buffer[j]);
				if(j%16 == 15)
				{
					 printf("\n\r");
				}
			}
		}
#endif
		if(1)
		{			
			//copy to stm flash
			iap_write_appbin(STM32_FLASH_BASE+0x5000+i*256,Rx_Buffer,size);
		}

		i++;
		total += size;
		
		if(total >= app_len)
		{
			printf("size=%d,page=%d\r\n",size,i);
			break;
		}
	}
#endif

#if 0  
	//read internal flash
	printf(">>>>>>>>>>>>>>>>>>>>>stm flash from 0:\r\n");
	STMFLASH_Read(STM32_FLASH_BASE,Buffer,512);   	
	for(i=0;i<512;i++)
	{
		printf("%04X ",Buffer[i]);
		if(i%16==15)
			printf("\r\n");
	}
#endif

#if 0
for(j=0;j<5;j++)
{
	printf(">>>>>>>>>>>>>>>>>>>stm flash from 0x%x:\r\n",STM32_FLASH_BASE+0x5000+j*sizeof(Buffer));
	STMFLASH_Read(STM32_FLASH_BASE+0x5000+j*sizeof(Buffer),Buffer,512);   	
	for(i=0;i<512;i++)
	{
		printf("%04X ",Buffer[i]);
		if(i%16==15)
			printf("\r\n");
	}
}
#endif

#if 1
	printf(">>>>>>>>>>>>>>>>>>>stm flash from 0x%x:\r\n",STM32_FLASH_BASE+0x5000);
	STMFLASH_Read(STM32_FLASH_BASE+0x5000,Buffer,512);   	
	for(i=0;i<512;i++)
	{
		printf("%04X ",Buffer[i]);
		if(i%16==15)
			printf("\r\n");
	}
#endif
	printf("\n\r>>>>>>>>>>>>>>>>>>>>>>>jump to app<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\r");

	iap_load_app(FLASH_APP1_ADDR);

/*   
	for(i=0; i<=255; i++)    //填充Tx_Buffer缓冲区
     {
       Tx_Buffer[i] = fw_sdfe4_data[i];
       printf("0x%02X ",Tx_Buffer[i]);
       if(i%16 == 15)
       {
         printf("\n\r");
       }
     }
*/         
    //SPI_FLASH_SectorErase(FLASH_WriteAddress);


    //when flag=0, burn idc fw from 0
#if 0    
    if(g_fw==0)
    {
		printf("flash idc firmware\r\n");
		SPI_FLASH_BulkErase();
		write_addr = FLASH_WriteAddress;
    }
	//flag=1, bur dsl fw from 0x100000
	else
	{
	    printf("flash dsl firmware\r\n");
    	write_addr = FLASH_WriteAddress+FW_OFFSET;
	}
	
    SPI_FLASH_BufferWrite((uint8_t*)image1,write_addr,fw_len);//从FLASH_WriteAddress处开始,写入SIZE长度的数据
    Delay_100us(10);


    Delay_100us(10);
    printf("\n\rRead from Rx_Buffer:%d,%d\n\r",fw_len/256,fw_len%256);
	SPI_FLASH_BufferRead(Rx_Buffer,write_addr+(fw_len/256-1)*256, 256);  //从FLASH_ReadAddress处开始,读入BufferSize长度的数据
    for(i=0; i<=255; i++)    //将Rx_Buffer中的数据通过串口打印
     {	
       printf("0x%02X ", Rx_Buffer[i]);
       if(i%16 == 15)
       {
         printf("\n\r");
       }
     }

    printf("\n\rRead from Rx_Buffer:%d,%d\n\r",fw_len/256,fw_len%256);
	Delay_100us(10);
   	SPI_FLASH_BufferRead(Rx_Buffer,write_addr+(fw_len/256)*256, 256);  //从FLASH_ReadAddress处开始,读入BufferSize长度的数据
    for(i=0; i<=255; i++)    //将Rx_Buffer中的数据通过串口打印
     {	
       printf("0x%02X ", Rx_Buffer[i]);
       if(i%16 == 15)
       {
         printf("\n\r");
       }
     }


	//when flag=0, burn idc fw from 0
    if(g_fw==0)
    {
		printf("flash idc firmware finished\r\n");
    }
	//flag=1, bur dsl fw from 0x100000
	else
	{
	    printf("flash dsl firmware finished\r\n");
	}
#endif	
  }
  else
  {
    printf("\n\rSPI Flash:GD25Q16 Initialize Fail!\n\r");
  }

  while(1)	
  {
	GPIO_ResetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_2);
	GPIO_ResetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1);/*关闭所有的LED指示灯*/
	Turn_On_LED(count%4);	//点亮一个LED灯		
	count++;
	Delay_100us(5000);		//500ms
  }
}

void Turn_On_LED(uint8_t LED_NUM)	           /*点亮对应灯*/
{
	switch(LED_NUM)
	{
        case 0:
          GPIO_SetBits(GPIOC,GPIO_Pin_0);  /*点亮DS1灯*/
          break;
        case 1:
          GPIO_SetBits(GPIOC,GPIO_Pin_2);  /*点亮DS2灯*/
          break;
        case 2:
          GPIO_SetBits(GPIOE,GPIO_Pin_0);  /*点亮DS3灯*/
          break;
        case 3:
          GPIO_SetBits(GPIOE,GPIO_Pin_1);  /*点亮DS4灯*/
          break;          
        default:
		  GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_2);
	      GPIO_SetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1); /*点亮所有的灯*/
          break;
	}
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  USART_SendData(USART1, (uint8_t) ch);
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  {
  }
  return ch;
}


#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
