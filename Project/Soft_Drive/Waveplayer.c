/******************** (C) COPYRIGHT 2009 www.armjishu.com ********************
* File Name          : waveplayer.c
* Author             : www.armjishu.com Team
* Version            : V2.0.0
* Date               : 04/27/2009
* Description        : Wave Player driver source file.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "waveplayer.h"

extern const char ArmjishuMusic[];

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define REPLAY  3


/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#define Line0          0
#define Line1          24
#define Line2          48
#define Line3          72
#define Line4          96
#define Line5          120
#define Line6          144
#define Line7          168
#define Line8          192
#define Line9          216

#define Horizontal     0x00
#define Vertical       0x01

#define LCD_Clear(p)
#define LCD_SetTextColor(p) 
#define LCD_ClearLine(p) 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t DemoTitle[20] = "STM32 I2S Codec Demo"; 

uint8_t CmdTitle0[20] = "  Control Buttons:  "; 

uint8_t CmdTitle1Playing[20] = "KEY>Pause  UP  >Vol+";

uint8_t CmdTitle2Playing[20] = "SEL>Stop   DOWN>Vol-";

uint8_t CmdTitle1Paused[20] =  "KEY>Play   UP  >Spkr";

uint8_t CmdTitle2Paused[20] =  "SEL>Stop   DOWN>Head";

uint8_t CmdTitle1Stopped[20] = "    UP > Speaker    "; 

uint8_t CmdTitle2Stopped[20] = "  DOWN > Headphone  "; 

uint8_t StatusTitleStopped[20] = "      Stopped       ";

uint8_t StatusTitlePlaying[20] = "      Playing       ";

uint8_t StatusTitlePaused[20] = "       Paused       ";

uint8_t i2cerr[20] =  "ERROR:I2C com. ->RST";
uint8_t memerr[20] =  "ERROR: Memory  ->RST";
uint8_t fileerr[20] = "ERROR: No Wave File ";

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : WavePlayer_StartSpeaker
* Description    : Starts the wave player application.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WavePlayer_StartSpeaker(void)
{
  /* Choose number of repetitions: 0 => infinite repetitions */
  I2S_CODEC_ReplayConfig(10);
  I2S_CODEC_Init(OutputDevice_SPEAKER, AUDIO_FILE_ADDRESS);
  I2S_CODEC_Play(1);
}
/******************* (C) COPYRIGHT 2009 www.armjishu.com *****END OF FILE****/
