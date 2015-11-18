#include "stm32f10x.h"
#include "stdio.h"

void RTC_STOP_WakeUp(void)
{
    /* Enable PWR and BKP clocks */
    /* PWR时钟（电源控制）与BKP时钟（RTC后备寄存器）使能 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);
  
    /* RTC clock source configuration ----------------------------------------*/
    /* Reset Backup Domain */
    BKP_DeInit();
  
    /* Enable LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);
    /* Wait till LSE is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* RTC configuration -----------------------------------------------------*/
    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();

    /* Set the RTC time base to 1s */
    RTC_SetPrescaler(32767);  
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    
    /* Enable the RTC Alarm interrupt */
    RTC_ITConfig(RTC_IT_ALR, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    RTC_ClearFlag(RTC_FLAG_SEC);
    while(RTC_GetFlagStatus(RTC_FLAG_SEC) == RESET); 
	 
    /* Alarm in 5 second */
    RTC_SetAlarm(RTC_GetCounter()+ 5);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}

void RTC_STANDBY_WakeUp(void)
{
    /* Enable PWR and BKP clocks */
    /* PWR时钟（电源控制）与BKP时钟（RTC后备寄存器）使能 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);
  
    /* RTC clock source configuration ----------------------------------------*/
    /* Reset Backup Domain */
    BKP_DeInit();
  
    /* Enable LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);
    /* Wait till LSE is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* RTC configuration -----------------------------------------------------*/
    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();

    /* Set the RTC time base to 1s */
    RTC_SetPrescaler(32767);  
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    
    RTC_ClearFlag(RTC_FLAG_SEC);
    while(RTC_GetFlagStatus(RTC_FLAG_SEC) == RESET);
  
    /* Alarm in 5 second */
    RTC_SetAlarm(RTC_GetCounter()+ 5);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}

void Sys_SleepMode(void)
{
  PWR_EnterSLEEPMode(PWR_SLEEPONEXIT_Reset, PWR_SLEEPEntry_WFI);
}

void Sys_StopMode(void)
{
  /* Request to enter STOP mode with regulator in low power mode*/
  printf("\n\rThe System Goes into STOP Mode...\n\r");
  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}

void Sys_StandbyMode(void)
{
  NVIC_SystemLPConfig(NVIC_LP_SLEEPDEEP,ENABLE);                               //使能SLEEPDEEP位 (SYS->CTRL)
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);     //使能PWR和BKP外设时钟 
  printf("\n\rThe System Goes into StandBy Mode...\n\r");
  PWR_EnterSTANDBYMode();	                                                   //进入待命（STANDBY）模式 	
}

void SYSCLKConfig_STOP(void)
{
  ErrorStatus HSEStartUpStatus;
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {

#ifdef STM32F10X_CL
    /* Enable PLL2 */ 
    RCC_PLL2Cmd(ENABLE);

    /* Wait till PLL2 is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET)
    {
    }
#endif

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
} 
