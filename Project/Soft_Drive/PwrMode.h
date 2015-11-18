#ifndef PWR_MODE_H
#define PWR_MODE_H

void Sys_SleepMode(void);
void Sys_StopMode(void);
void RTC_STOP_WakeUp(void);
void RTC_STANDBY_WakeUp(void);
void Sys_StandbyMode(void);
void SYSCLKConfig_STOP(void);

#endif
