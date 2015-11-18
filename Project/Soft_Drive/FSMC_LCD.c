#include "stm32f10x.h"
#include "FSMC_LCD.h"

void LCD_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Set PD.00(D2), PD.01(D3), PD.08(D13), PD.09(D14), PD.10(D15), PD.14(D0), PD.15(D1),
     PE.14(D11), PE.15(D12) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;                             
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure); 

  /* PD.07(NE1)configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* PE.02(RS)-A23 configuration,1:R/W data, 0:R/W Command*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
}

void FSMC_LCD_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;

  FSMC_NORSRAMTimingInitTypeDef  p;

/*-- FSMC Configuration ------------------------------------------------------*/
/*----------------------- SRAM Bank 1 ----------------------------------------*/
  /* FSMC_Bank1_NORSRAM1 configuration */
  p.FSMC_AddressSetupTime = 2;
  p.FSMC_AddressHoldTime = 0;
  p.FSMC_DataSetupTime = 5;
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;

  /* Color LCD configuration ------------------------------------
     LCD configured as follow:
        - Data/Address MUX = Disable									 
        - Memory Type = SRAM
        - Data Width = 16bit
        - Write Operation = Enable												
        - Extended Mode = Enable													
        - Asynchronous Wait = Disable */
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  

  /* BANK 1 (of NOR/SRAM Bank 1~4) is enabled */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}



