#ifndef FSMC_LCD_H
#define FSMD_LCD_H

#define Bank1_LCD_D    ((uint32_t)0x61000000)    //LCD Data ADDR	通过RS引脚计算
#define Bank1_LCD_C    ((uint32_t)0x60000000)	 //LCD Reg  ADDR

void LCD_GPIO_Configuration(void);
void FSMC_LCD_Init(void);

#endif
