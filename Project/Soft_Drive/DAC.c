#include "stm32f10x.h"
#include "DAC.h"

void DAC_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically 
     connected to the DAC converter. In order to avoid parasitic consumption, 
     the GPIO pin should be configured in analog */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void DAC_Configuration(void)
{
  DAC_InitTypeDef DAC_InitStructure;
  
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits8_0;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);
  /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is 
     automatically connected to the DAC converter. */
  DAC_Cmd(DAC_Channel_1, ENABLE);
  DAC_Cmd(DAC_Channel_2, ENABLE);

  /* Set DAC Channel1 DHR12L register */
//  DAC_SetChannel1Data(DAC_Align_12b_L, DAC_Out1_Code);     //12bits,Left
//  DAC_SetChannel2Data(DAC_Align_12b_L, DAC_Out2_Code);     //12bits,Left
  DAC_SetDualChannelData(DAC_Align_12b_L,DAC_Out2_Code,DAC_Out1_Code);
}


