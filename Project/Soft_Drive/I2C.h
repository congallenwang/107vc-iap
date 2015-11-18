#ifndef I2C_H
#define I2C_H

#include "stm32f10x.h"

#define I2C1_Speed              400000
#define I2C1_SLAVE_ADDRESS7     0xA0
#define I2C_PageSize            8


void I2C1_GPIO_Configuration(void);
void I2C1_Configuration(void);

#endif
