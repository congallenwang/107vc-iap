/******************** (C) COPYRIGHT 2009  www.armjishu.com ********************
* File Name          : waveplayer.h
* Author             : www.armjishu.com Team
* Version            : V2.0.0
* Date               : 04/27/2009
* Description        : This file contains all the functions prototypes for the
*                      wave player driver.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WAVEPLAYER_H
#define __WAVEPLAYER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "i2s_code.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Audio file location */
//#define AUDIO_FILE_ADDRESS  0x08008000
#define AUDIO_FILE_ADDRESS  (uint32_t) ArmjishuMusic

/* LCD display update parameters */
#define STOP        0x10000000
#define PLAY        0x01000000
#define PAUSE       0x00100000 
#define VOL         0x00010000
#define PROGRESS    0x00001000
#define FRWD        0x00000100
#define ALL         0x11110100

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void WavePlayer_StartSpeaker (void);
void LCD_Update(uint32_t Status);
void LCD_DisplayError(uint32_t err);
void I2S_CODEC_LCDConfig(void);

#endif /* __WAVEPLAYER_H */

/******************* (C) COPYRIGHT 2009 www.armjishu.com *****END OF FILE****/
