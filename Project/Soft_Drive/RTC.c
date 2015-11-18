#include "stm32f10x.h"
#include <stdio.h>
#include "RTC.h"

__IO uint32_t TimeDisplay = 0;

void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  /* PWRʱ�ӣ���Դ���ƣ���BKPʱ�ӣ�RTC�󱸼Ĵ�����ʹ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  /*ʹ��RTC�ͺ󱸼Ĵ������� */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  /* ������BKP��ȫ���Ĵ�������Ϊȱʡֵ */
  BKP_DeInit();

  /* Enable LSE */
  /* ʹ��LSE���ⲿ32.768KHz���پ���*/
  RCC_LSEConfig(RCC_LSE_ON);
  
  /* Wait till LSE is ready */
  /* �ȴ��ⲿ�������ȶ���� */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select LSE as RTC Clock Source */
  /*ʹ���ⲿ32.768KHz������ΪRTCʱ�� */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  /* ʹ�� RTC ��ʱ�ӹ��� */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  /*�ȴ�RTC�Ĵ���ͬ�� */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  /* �ȴ���һ�ζ�RTC�Ĵ�����д������� */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  /* ʹ��RTC�����ж� */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  /* �ȴ���һ�ζ�RTC�Ĵ�����д������� */
  RTC_WaitForLastTask();
  
  /* Set RTC prescaler: set RTC period to 1sec */
  /* 32.768KHz����Ԥ��Ƶֵ��32767,����Ծ���Ҫ��ܸ߿����޸Ĵ˷�Ƶֵ��У׼���� */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  /* �ȴ���һ�ζ�RTC�Ĵ�����д������� */
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
  * ��ʱ��ת��ΪRTC����ֵд��RTC�Ĵ���
  */
void Time_Adjust(void)
{
  /* Wait until last write operation on RTC registers has finished */
  /* �ȴ���һ�ζ�RTC�Ĵ�����д������� */
  RTC_WaitForLastTask();
  
  /* Change the current time */
  /* ��ʱ��ת��ΪRTC����ֵд��RTC�Ĵ��� */
  RTC_SetCounter(Time_Regulate());
  
  /* Wait until last write operation on RTC registers has finished */
  /* �ȴ���һ�ζ�RTC�Ĵ�����д������� */
  RTC_WaitForLastTask();
}

/**
  * @brief  Displays the current time.
  * @param  TimeVar: RTC counter value.
  * @retval None
  * ��RTC�Ĵ�������ֵת��Ϊʱ�䲢ͨ�����ڴ�ӡ
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

  /* ͨ�����ڴ�ӡʱ�䣬ע��˴���'\r'��֤�ڴ���ͬһ�и������ */
  printf("Time: %0.2d:%0.2d:%0.2d\r", THH, TMM, TSS);
}

/**
  * @brief  Shows the current time (HH:MM:SS) on the Hyperterminal.
  * @param  None
  * @retval None
  * �ȴ���ÿ�밴��(ʱ:��:��)�ĸ�ʽ���ʱ��һ��
  */   
void Time_Show(void)
{
  printf("\n\r");

  /* Infinite loop */
  while (1)
  {
    /* If 1s has paased */
    /* �ж�ȫ�ֱ���TimeDisplay�Ƿ�Ϊ1�����Ϊ1��ʱ�䲢����TimeDisplay��0
       TimeDisplay������"stm32f10x_it.c"�ļ���"void RTC_IRQHandler(void)"
       �����е��������ж�ʱΪ��Ϊ1��������ʵ���˾�׼��ÿ�����ʱ��һ�� */
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
