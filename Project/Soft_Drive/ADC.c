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
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;                  //ADC1�����ַ��ͨ�������ֲ�ɲ�0x4001244c
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;          //�ڴ��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                           //DMA���䷽����,����-->�ڴ�
  DMA_InitStructure.DMA_BufferSize = 1;                                        //DMA���������ȡ�
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;             //����DMA���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_PeripheralInc_Disable;                 //����DMA���ڴ����ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //���������ֳ�������16bits
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;          //�ڴ������ֳ�������16bits
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                              //ѭ��ģʽ������Bufferд�����Զ��ص���ʼ��ַ��ʼ����
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                          //����DMA�����ȼ���
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                 //����DMA��2��memory�еı����������
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
     
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                              //����ת��ģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;                                    //ɨ��ת��ģʽ����
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                              //����ת������
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;             //�ر��ⲿ����ģʽ
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                          //�����Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 1;                                         //ת�����г���
  ADC_Init(ADC1, &ADC_InitStructure);
  
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_55Cycles5);     //ADC1����1ͨ��
      
  ADC_DMACmd(ADC1, ENABLE);                                                        //ʹ��ADC��DMA����
  ADC_Cmd(ADC1, ENABLE);                                                           //ʹ��ADC1
  Delay_100us(100);

  ADC_ResetCalibration(ADC1);                 // ADC�Զ�У׼����������ִ��һ�Σ���֤���� 
  while(ADC_GetResetCalibrationStatus(ADC1));
  
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));      // ADC�Զ�У׼����---------------

  ADC_SoftwareStartConvCmd(ADC1, ENABLE);     //�������ADC1ת��

}

