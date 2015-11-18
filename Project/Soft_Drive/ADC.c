#include "stm32f10x.h"
#include "ADC.h"
#include "SysTick.h"
#define ADC1_DR_Address  (uint32_t)0x4001244C
__IO uint16_t ADCConvertedValue;

/*******************************************************************************
* Function Name  : ADC_GPIO_Configuration
* Description    : ADC_GPIO_Configuration
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		    
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}																								  ;


/*******************************************************************************
* Function Name  : ADC_Configuration
* Description    : ADC_Configuration
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_Configuration(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
  /* DMA1 channel1 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel1);                                                   //ADC1--channel1
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;                  //ADC1外设地址，通过数据手册可查0x4001244c
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;          //内存地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                           //DMA传输方向单向,外设-->内存
  DMA_InitStructure.DMA_BufferSize = 1;                                        //DMA缓冲区长度。
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;             //设置DMA的外设递增模式
  DMA_InitStructure.DMA_MemoryInc = DMA_PeripheralInc_Disable;                 //设置DMA的内存递增模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //外设数据字长，半字16bits
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;          //内存数据字长，半字16bits
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                              //循环模式开启，Buffer写满后，自动回到初始地址开始传输
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                          //设置DMA的优先级别
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                 //设置DMA的2个memory中的变量互相访问
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
     
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                              //独立转换模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;                                    //扫描转换模式开启
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                              //连续转换开启
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;             //关闭外部触发模式
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                          //数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 1;                                         //转换序列长度
  ADC_Init(ADC1, &ADC_InitStructure);
  
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_55Cycles5);     //ADC1，第1通道
      
  ADC_DMACmd(ADC1, ENABLE);                                                        //使能ADC的DMA功能
  ADC_Cmd(ADC1, ENABLE);                                                           //使能ADC1
  Delay_100us(100);

  ADC_ResetCalibration(ADC1);                 // ADC自动校准，开机后需执行一次，保证精度 
  while(ADC_GetResetCalibrationStatus(ADC1));
  
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));      // ADC自动校准结束---------------

  ADC_SoftwareStartConvCmd(ADC1, ENABLE);     //软件启动ADC1转换

}

