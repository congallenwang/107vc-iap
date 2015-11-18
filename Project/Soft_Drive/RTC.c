#include "stm32f10x.h"
#include <stdio.h>
#include "RTC.h"

__IO uint32_t TimeDisplay = 0;

void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  /* PWR时钟（电源控制）与BKP时钟（RTC后备寄存器）使能 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  /*使能RTC和后备寄存器访问 */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  /* 将外设BKP的全部寄存器重设为缺省值 */
  BKP_DeInit();

  /* Enable LSE */
  /* 使能LSE（外部32.768KHz低速晶振）*/
  RCC_LSEConfig(RCC_LSE_ON);
  
  /* Wait till LSE is ready */
  /* 等待外部晶振震荡稳定输出 */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select LSE as RTC Clock Source */
  /*使用外部32.768KHz晶振作为RTC时钟 */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  /* 使能 RTC 的时钟供给 */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  /*等待RTC寄存器同步 */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  /* 等待上一次对RTC寄存器的写操作完成 */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  /* 使能RTC的秒中断 */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  /* 等待上一次对RTC寄存器的写操作完成 */
  RTC_WaitForLastTask();
  
  /* Set RTC prescaler: set RTC period to 1sec */
  /* 32.768KHz晶振预分频值是32767,如果对精度要求很高可以修改此分频值来校准晶振 */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  /* 等待上一次对RTC寄存器的写操作完成 */
  RTC_WaitForLastTask();
}


/**
  * @brief  Returns the time entered by user, using Hyperterminal.
  * @param  None
  * @retval Current time RTC counter value
  */
uint32_t Time_Regulate(void)
{
  uint32_t Tmp_HH = 0xFF, Tmp_MM = 0xFF, Tmp_SS = 0xFF;

  printf("\r\n==============Time Settings=====================================");
  printf("\r\n  Please Set Hours");

  while (Tmp_HH == 0xFF)
  {
    Tmp_HH = USART_Scanf(23);
  }
  printf(":  %d", Tmp_HH);
  printf("\r\n  Please Set Minutes");
  while (Tmp_MM == 0xFF)
  {
    Tmp_MM = USART_Scanf(59);
  }
  printf(":  %d", Tmp_MM);
  printf("\r\n  Please Set Seconds");
  while (Tmp_SS == 0xFF)
  {
    Tmp_SS = USART_Scanf(59);
  }
  printf(":  %d", Tmp_SS);

  /* Return the value to store in RTC counter register */
  return((Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS));
}

/**
  * @brief  Adjusts time.
  * @param  None
  * @retval None
  * 把时间转化为RTC计数值写入RTC寄存器
  */
void Time_Adjust(void)
{
  /* Wait until last write operation on RTC registers has finished */
  /* 等待上一次对RTC寄存器的写操作完成 */
  RTC_WaitForLastTask();
  
  /* Change the current time */
  /* 把时间转化为RTC计数值写入RTC寄存器 */
  RTC_SetCounter(Time_Regulate());
  
  /* Wait until last write operation on RTC registers has finished */
  /* 等待上一次对RTC寄存器的写操作完成 */
  RTC_WaitForLastTask();
}

/**
  * @brief  Displays the current time.
  * @param  TimeVar: RTC counter value.
  * @retval None
  * 把RTC寄存器的数值转化为时间并通过串口打印
  */
void Time_Display(uint32_t TimeVar)
{
  uint32_t THH = 0, TMM = 0, TSS = 0;

  /* Compute  hours */
  THH = (TimeVar / 3600)%24;
  /* Compute minutes */
  TMM = (TimeVar % 3600) / 60;
  /* Compute seconds */
  TSS = (TimeVar % 3600) % 60;

  /* 通过串口打印时间，注意此处的'\r'保证在串口同一行覆盖输出 */
  printf("Time: %0.2d:%0.2d:%0.2d\r", THH, TMM, TSS);
}

/**
  * @brief  Shows the current time (HH:MM:SS) on the Hyperterminal.
  * @param  None
  * @retval None
  * 等待并每秒按照(时:分:秒)的格式输出时间一次
  */   
void Time_Show(void)
{
  printf("\n\r");

  /* Infinite loop */
  while (1)
  {
    /* If 1s has paased */
    /* 判断全局变量TimeDisplay是否为1，如果为1则时间并将请TimeDisplay清0
       TimeDisplay变量在"stm32f10x_it.c"文件的"void RTC_IRQHandler(void)"
       函数中当发生秒中断时为置为1，这样便实现了精准的每秒输出时间一次 */
    if (TimeDisplay == 1)
    {
      /* Display current time */
      Time_Display(RTC_GetCounter());
      TimeDisplay = 0;
    }
  }
}
/**
  * @brief  Gets numeric values from the hyperterminal.
  * @param  None
  * @retval None
  */
uint8_t USART_Scanf(uint32_t value)
{
  uint32_t index = 0;
  uint32_t tmp[2] = {0, 0};

  while (index < 2)
  {
    /* Loop until RXNE = 1 */
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
    {}
    tmp[index++] = (USART_ReceiveData(USART1));
    if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
    {
      printf("\n\rPlease enter valid number between 0 and 9");
      index--;
    }
  }
  /* Calculate the Corresponding value */
  index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
  /* Checks */
  if (index > value)
  {
    printf("\n\rPlease enter valid number between 0 and %d", value);
    return 0xFF;
  }
  return index;
}
