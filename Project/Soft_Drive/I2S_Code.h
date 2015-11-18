/******************** (C) COPYRIGHT 2009  www.armjishu.com ********************
* File Name          : i2s_codec.h
* Author             : www.armjishu.com Team
* Version            : V2.0.0
* Date               : 04/27/2009
* Description        : This file contains all the functions prototypes for the
*                      I2S codec firmware driver.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2S_CODEC_H
#define __I2S_CODEC_H

/* .WAV file format :

 Endian      Offset      Length      Contents
  big         0           4 bytes     'RIFF'             // 0x52494646
  little      4           4 bytes     <file length - 8>
  big         8           4 bytes     'WAVE'             // 0x57415645

Next, the fmt chunk describes the sample format:

  big         12          4 bytes     'fmt '          // 0x666D7420
  little      16          4 bytes     0x00000010      // Length of the fmt data (16 bytes)
  little      20          2 bytes     0x0001          // Format tag: 1 = PCM
  little      22          2 bytes     <channels>      // Channels: 1 = mono, 2 = stereo
  little      24          4 bytes     <sample rate>   // Samples per second: e.g., 22050
  little      28          4 bytes     <bytes/second>  // sample rate * block align
  little      32          2 bytes     <block align>   // channels * bits/sample / 8
  little      34          2 bytes     <bits/sample>   // 8 or 16

Finally, the data chunk contains the sample data:

  big         36          4 bytes     'data'        // 0x64617461
  little      40          4 bytes     <length of the data block>
  little      44          *           <sample data>

*/
//#define     __I     volatile const            /*!< defines 'read only' permissions      */
//#define     __O     volatile                  /*!< defines 'write only' permissions     */
//#define     __IO    volatile                  /*!< defines 'read / write' permissions   */


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "waveplayer.h"
#include "stm32f10x_spi.h"

/* Exported types ------------------------------------------------------------*/

/* Audio file information structure */
typedef struct
{
  uint32_t  RIFFchunksize;
  uint16_t  FormatTag;
  uint16_t  NumChannels;
  uint32_t  SampleRate;
  uint32_t  ByteRate;
  uint16_t  BlockAlign;
  uint16_t  BitsPerSample;
  uint32_t  DataSize;
} WAVE_FormatTypeDef;

/* Error Identification structure */
typedef enum
{
  Valid_WAVE_File = 0,
  Unvalid_RIFF_ID,
  Unvalid_WAVE_Format,
  Unvalid_FormatChunk_ID,
  Unsupporetd_FormatTag,
  Unsupporetd_Number_Of_Channel,
  Unsupporetd_Sample_Rate,
  Unsupporetd_Bits_Per_Sample,
  Unvalid_DataChunk_ID,
  Unsupporetd_ExtraFormatBytes,
  Unvalid_FactChunk_ID
} ErrorCode;
 
/* Exported constants --------------------------------------------------------*/

/* Codec output DEVICE */
#define OutputDevice_SPEAKER          1
#define OutputDevice_HEADPHONE        2  
#define OutputDevice_BOTH             3

/* VOLUME control constants */
#define DEFAULT_VOL                   MAX_VOL
#define MAX_VOL                       0x3F
#define VolumeDirection_HIGH          0xF
#define VolumeDirection_LOW           0xA
#define VolumeDirection_LEVEL         0x0
#define VOLStep                       4 

/* Forward and Rewind constants */
#define STEP_FORWARD                  2 /* 2% of wave file data length */
#define STEP_BACK                     6 /* 6% of wave file data length */

/* Codec POWER DOWN modes */
#define CodecPowerDown_HW             1
#define CodecPowerDown_SW 	          2

/* Audio Play STATUS */
#define AudioPlayStatus_STOPPED       0
#define AudioPlayStatus_PLAYING	      1
#define AudioPlayStatus_PAUSED        2	 

/* MUTE commands */ 
#define MUTE_ON                       1
#define MUTE_OFF                      0

/* I2S configuration parameters */
#define I2S_STANDARD                  I2S_Standard_MSB//I2S_Standard_Phillips
#define I2S_MCLKOUTPUT                I2S_MCLKOutput_Enable

#define MUSIC_BUFFER_SIZE 512

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
ErrorCode I2S_CODEC_WaveParsing(uint8_t* HeaderTab);
void I2S_CODEC_ReplayConfig(uint32_t Repetions);
uint32_t I2S_CODEC_Init(uint32_t OutputDevice, uint32_t Address);
void I2S_CODEC_DataTransfer(void);
uint32_t I2S_CODEC_Play(uint32_t AudioStartPosition);
uint32_t GetVar_SendDummyData(void);
void IncrementVar_AudioDataIndex(uint32_t IncrementNumber);
void Decrement_AudioReplay(void);
uint32_t SetVar_AudioPlayStatus(uint32_t Status);
uint16_t Media_ReadHalfWord(uint32_t Offset);
uint32_t CODEC_Config(uint16_t OutputDevice, uint16_t I2S_Standard, uint16_t I2S_MCLKOutput, uint8_t Volume);
uint32_t SetVar_SendDummyData(void);
uint32_t ResetVar_SendDummyData(void);
void I2S_CODEC_Reset(void);

#endif /* __I2S_CODEC_H */

/******************* (C) COPYRIGHT 2009  www.armjishu.com *****END OF FILE****/
