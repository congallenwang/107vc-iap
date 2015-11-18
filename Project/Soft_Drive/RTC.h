#ifndef RTC_H
#define RTC_H
void RTC_Configuration(void);
uint32_t Time_Regulate(void);
void Time_Adjust(void);
void Time_Display(uint32_t TimeVar);
void Time_Show(void);
uint8_t USART_Scanf(uint32_t value);
#endif
