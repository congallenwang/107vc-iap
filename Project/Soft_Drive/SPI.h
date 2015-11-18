#ifndef SPI_H
#define SPI_H
#include "stm32f10x.h"

//#define  SPI_MEM1_PIN          GPIO_Pin_3
#define  SPI_MEM1_PIN          GPIO_Pin_15
//#define  SPI_MEM1_PORT         GPIOE
#define  SPI_MEM1_PORT         GPIOA

#define  SPI_FLASH_CS_LOW()    GPIO_ResetBits(SPI_MEM1_PORT, SPI_MEM1_PIN)  /* Select SPI MEM1: ChipSelect pin low  */
#define  SPI_FLASH_CS_HIGH()   GPIO_SetBits(SPI_MEM1_PORT, SPI_MEM1_PIN)    /* Deselect SPI MEM1: ChipSelect pin high */

#define  SPI_MEM2_PIN          GPIO_Pin_12
#define  SPI_MEM2_PORT         GPIOC
#define  SPI_SD_CS_LOW()       GPIO_ResetBits(SPI_MEM2_PORT, SPI_MEM2_PIN)  /* Select SPI MEM2: ChipSelect pin low  */
#define  SPI_SD_CS_HIGH()      GPIO_SetBits(SPI_MEM2_PORT, SPI_MEM2_PIN)    /* Deselect SPI MEM2: ChipSelect pin high */

#define  SPI_Touch_PIN         GPIO_Pin_4
#define  SPI_Touch_PORT        GPIOE
#define  SPI_Touch_CS_LOW()    GPIO_ResetBits(SPI_Touch_PORT, SPI_Touch_PIN)  /* Select SPI MEM2: ChipSelect pin low  */
#define  SPI_Touch_CS_HIGH()   GPIO_SetBits(SPI_Touch_PORT, SPI_Touch_PIN)    /* Deselect SPI MEM2: ChipSelect pin high */
       
void SPI1_Flash_Configuration(void);
void SPI1_Flash_GPIO_Configuration(void);
void SPI1_MSD_Configuration(void);
void SPI1_GPIO_Configuration(void);
void SPI1_MSD_GPIO_Configuration(void);
void SPI1_Touch_GPIO_Configuration(void);
void SPI1_Touch_Configuration(void);
#endif
