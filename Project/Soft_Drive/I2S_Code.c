/******************** (C) COPYRIGHT 2009 www.armjishu.com********************
* File Name          : i2s_codec.c
* Author             : www.armjishu.com Team
* Version            : V2.0.0
* Date               : 04/27/2009
* Description        : This file includes the I2S Codec driver for AK4343 
*                      Audio Codec.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "I2S_Code.h"
#include "stm32_eval.h"
#include <stdio.h>

#define AudioFileAddressStart  (uint32_t) ArmjishuMusic
#define AudioFileAddressEnd    (uint32_t)(ArmjishuMusic + 0x30000)//( ArmjishuMusic + sizeof(ArmjishuMusic)/sizeof(uint32_t))


/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  LittleEndian,
  BigEndian
}Endianness;

/* Private define ------------------------------------------------------------*/

/* Audio file header size */
#define HEADER_SIZE          100

/* EvalBoard pins related to the Codec */
#define Codec_PDN_GPIO	     GPIOC
#define Codec_PDN_Pin	     GPIO_Pin_5

/* Uncomment this line to enable verifying data sent to codec after each write opeation */
//#define VERIFY_WRITTENDATA

/* The 7 bits Codec adress mask */
#define CODEC_ADDRESS         0x27  /* b00100111 */ 

/* Audio Parsing Constants */
#define  ChunkID             0x52494646  /* correspond to the letters 'RIFF' */
#define  FileFormat          0x57415645  /* correspond to the letters 'WAVE' */
#define  FormatID            0x666D7420  /* correspond to the letters 'fmt ' */
#define  DataID              0x64617461  /* correspond to the letters 'data' */
#define  FactID              0x66616374  /* correspond to the letters 'fact' */

#define  WAVE_FORMAT_PCM     0x01
#define  FormatChunkSize     0x10
#define  Channel_MONO        0x01
#define  Channel_STEREO      0x02

#define  SampleRate_8000     8000
#define  SampleRate_16000    16000
#define  SampleRate_22050    22050
#define  SampleRate_44100    44100
#define  SampleRate_48000    48000
#define  Bits_Per_Sample_8   8
#define  Bits_Per_Sample_16  16

#define DUMMY_DATA           0x1111

#define I2C_Config() 
#define CODEC_WriteRegister(RegisterAddr, RegisterValue);
#define CODEC_ReadRegister(RegisterAddr); 0x5

/* Extern variables ----------------------------------------------------------*/

extern u16 MusicBuffer[MUSIC_BUFFER_SIZE];
extern __IO int MusicBufferCurrentPos;
extern __IO int PlayMusicInSdCard;  
extern const char ArmjishuMusic[];

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Audio Frequency value */
uint16_t i2saudiofreq = I2S_AudioFreq_16k;

/* Header Table containing the audio file information */
uint8_t HeaderTabIndex = 0;
//uint8_t AudioFileHeader[HEADER_SIZE]; 
//uint8_t * AudioFileHeader = (uint8_t *)0x20002000; 
//uint8_t * AudioFileHeader = (uint8_t *)0x08008000; 
uint8_t * AudioFileHeader = (uint8_t *)ArmjishuMusic;

/* Audio Codec variables */
WAVE_FormatTypeDef  WAVE_Format;
__IO uint32_t AudioFileAddress = AUDIO_FILE_ADDRESS; 
uint32_t AudioDataLength = 0;
uint32_t DataStartAddr = 0x0;
__IO uint32_t AudioDataIndex = 0;
static __IO uint32_t AudioReplay = 0xFFFF;
static uint32_t AudioReplayCount = 0xFFFF;
static __IO uint32_t SendDummyData = 0;
static __IO uint32_t AudioPlayStatus = AudioPlayStatus_STOPPED;
static uint32_t CurrentOutputDevice = OutputDevice_HEADPHONE;
static uint8_t CurrentVolume = DEFAULT_VOL;
static uint32_t errorcode = 0xFF;
static __IO uint32_t monovar = 0, tmpvar = 0;

/* Wave details names table */
//WAVE_FormatTypeDef  WAVE_Format;
__IO ErrorCode  WaveFileStatus = Unvalid_RIFF_ID;   
 
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void I2S_GPIO_Config(void);
static void I2S_GPIO_Config(void);
static void I2S_Config(uint16_t Standard, uint16_t MCLKOutput, uint16_t AudioFreq);
static uint32_t CODEC_Config(uint16_t AudioOutput, uint16_t I2S_Standard, uint16_t I2S_MCLKOutput, uint8_t Volume);
uint32_t AudioFile_Init(void);
/*******************************************************************************
* Function Name  : ReadUnit
* Description    : Reads a number of bytes from the SPI Flash and reorder them
*                  in Big or little endian.
* Input          : - NbrOfBytes : number of bytes to read.
*                    This parameter must be a number between 1 and 4.
*                  - ReadAddr : external memory address to read from.
*                  - Endians : specifies the bytes endianness.
*                    This parameter can be one of the following values:
*                          - LittleEndian
*                          - BigEndian
* Output         : None
* Return         : Bytes read from the SPI Flash.
*******************************************************************************/
static uint32_t ReadUnit(uint8_t NbrOfBytes, Endianness BytesFormat)
{
  uint32_t index = 0;
  uint32_t Temp = 0;

  if(BytesFormat == LittleEndian)
  {
    for(index = 0; index < NbrOfBytes; index++)
    {
      Temp |= AudioFileHeader[HeaderTabIndex++] << (index * 8);
    }
  }
  else
  {
    for(index = NbrOfBytes; index != 0; index--)
    {
      Temp |= AudioFileHeader[HeaderTabIndex++] << ((index-1) * 8);
    }
  }

  return Temp;
}

/*******************************************************************************
* Function Name  : I2S_CODEC_WaveParsing
* Description    : Checks the format of the .WAV file and gets information about
*                  the audio format. This is done by reading the value of a
*                  number of parameters stored in the file header and comparing
*                  these to the values expected authenticates the format of a
*                  standard .WAV  file (44 bytes will be read). If  it is a valid
*                  .WAV file format, it continues reading the header to determine
*                  the  audio format such as the sample rate and the sampled data
*                  size. If the audio format is supported by this application,
*                  it retrieves the audio format in WAVE_Format structure and
*                  returns a zero value. Otherwise the function fails and the
*                  return value is nonzero.In this case, the return value specifies
*                  the cause of  the function fails. The error codes that can be
*                  returned by this function are declared in the header file.
* Input          : None
* Output         : None
* Return         : Zero value if the function succeed, otherwise it return
*                  a nonzero value which specifies the error code.
*******************************************************************************/
ErrorCode I2S_CODEC_WaveParsing(uint8_t* HeaderTab)
{
  uint32_t Temp = 0x00;
  uint32_t ExtraFormatBytes = 0; 
  
  /* Initialize the HeaderTabIndex variable */
  HeaderTabIndex = 0;
  printf("\n\r HeaderTab 0x%X!",HeaderTab); 
  /* Read chunkID, must be 'RIFF'	----------------------------------------------*/
  Temp = ReadUnit(4, BigEndian);
  if(Temp != ChunkID)
  {
    printf("\n\rUnvalid_RIFF_ID!\n\r "); 
    return(Unvalid_RIFF_ID);
  }
  /* Read the file length ----------------------------------------------------*/
  WAVE_Format.RIFFchunksize = ReadUnit(4, LittleEndian);
	
  /* Read the file format, must be 'WAVE' ------------------------------------*/
  Temp = ReadUnit(4, BigEndian);
  if(Temp != FileFormat)
  {
    printf("\n\r Unvalid_WAVE_Format!\n\r "); 
    return(Unvalid_WAVE_Format);
  }
  /* Read the format chunk, must be 'fmt ' -----------------------------------*/
  Temp = ReadUnit(4, BigEndian);
  if(Temp != FormatID)
  {
    printf("\n\r Unvalid_FormatChunk_ID!\n\r "); 
    return(Unvalid_FormatChunk_ID);
  }
  /* Read the length of the 'fmt' data, must be 0x10 -------------------------*/
  Temp = ReadUnit(4, LittleEndian);
  if(Temp != 0x10)
  {
    ExtraFormatBytes = 1;
  }
  /* Read the audio format, must be 0x01 (PCM) -------------------------------*/
  WAVE_Format.FormatTag = ReadUnit(2, LittleEndian);
  if(WAVE_Format.FormatTag != WAVE_FORMAT_PCM)
  {
    printf("\n\r Unsupporetd_FormatTag!\n\r ");
    return(Unsupporetd_FormatTag);	
  }	
  /* Read the number of channels: 0x02->Stereo 0x01->Mono --------------------*/
  WAVE_Format.NumChannels = ReadUnit(2, LittleEndian);

  /* Read the Sample Rate ----------------------------------------------------*/
  WAVE_Format.SampleRate = ReadUnit(4, LittleEndian);

  /* Update the I2S_AudioFreq value according to the .WAV file Sample Rate */
  switch(WAVE_Format.SampleRate)
  {
    case SampleRate_8000 : i2saudiofreq = I2S_AudioFreq_8k;  break; 
    case SampleRate_16000: i2saudiofreq = I2S_AudioFreq_16k; break; 
    case SampleRate_22050: i2saudiofreq = I2S_AudioFreq_22k; break; 	
    case SampleRate_44100: i2saudiofreq = I2S_AudioFreq_44k; break;  		
    case SampleRate_48000: i2saudiofreq = I2S_AudioFreq_48k; break;  		
    default: return(Unsupporetd_Sample_Rate);
  }	
  /* Read the Byte Rate ------------------------------------------------------*/
  WAVE_Format.ByteRate = ReadUnit(4, LittleEndian);
	
  /* Read the block alignment ------------------------------------------------*/
  WAVE_Format.BlockAlign = ReadUnit(2, LittleEndian);
	
  /* Read the number of bits per sample --------------------------------------*/
  WAVE_Format.BitsPerSample = ReadUnit(2, LittleEndian);
  if(WAVE_Format.BitsPerSample != Bits_Per_Sample_16)
  {
    printf("\n\r Unsupporetd_Bits_Per_Sample!\n\r ");
    return(Unsupporetd_Bits_Per_Sample);
  }
  /* If there are Extra format bytes, these bytes will be defined in "Fact Chunk" */
  if(ExtraFormatBytes == 1)
  {
    /* Read th Extra format bytes, must be 0x00 ------------------------------*/
    Temp = ReadUnit(2, LittleEndian);
    if(Temp != 0x00)
    {
      printf("\n\r Unsupporetd_ExtraFormatBytes!\n\r ");
      return(Unsupporetd_ExtraFormatBytes);
    }
    /* Read the Fact chunk, must be 'fact' -----------------------------------*/
    Temp = ReadUnit(4, BigEndian);
    if(Temp != FactID)
    {
      printf("\n\r Unvalid_FactChunk_ID!\n\r ");
      return(Unvalid_FactChunk_ID);
    }
    /* Read Fact chunk data Size ---------------------------------------------*/
    Temp = ReadUnit(4, LittleEndian);

    /* Set the index to start reading just after the header end */
    HeaderTabIndex += Temp;
  }
  /* Read the Data chunk, must be 'data' -------------------------------------*/
  Temp = ReadUnit(4, BigEndian);
  if(Temp != DataID)
  {
    printf("\n\r Unvalid_DataChunk_ID!\n\r ");
    return(Unvalid_DataChunk_ID);
  }
  /* Read the number of sample data ------------------------------------------*/
  WAVE_Format.DataSize = ReadUnit(4, LittleEndian);

  /* Set the data pointer at the beginning of the effective audio data */
	DataStartAddr += HeaderTabIndex;
  
  return(Valid_WAVE_File);	
}

/*******************************************************************************
* Function Name  : I2S_CODEC_ReplayConfig
* Description    : Set AudioReplay variable value .
* Input          : Repetions: Number of repetitions
* Output         : None
* Return         : None
*******************************************************************************/
void I2S_CODEC_ReplayConfig(uint32_t Repetions)
{ 
  /* Audio Replay number set by user */
  AudioReplay = Repetions;
  
  /* Audio Replays number remaining (if AudioReplay != 0) */
  AudioReplayCount = Repetions;
}

/*******************************************************************************
* Function Name  : I2S_CODEC_Init
* Description    : Initializes the I2S audio codec according parameters configured 
*                    by I2S_CODEC_Config function.
* Input          : - OutputDevice: Could be OutoutDevice_SPEAKER or 
*                      OutoutDevice_HEADPHONE.
*                  - Address: Specifies the location of the audio file in the memory.   
* Output         : None
* Return         : - 0: if all initializations are OK.
*                  - 1: if memory initialization failed (LD2 is turned on).
*                  - 2: if audio file initialization failed (LD2 is turned on).
*                  - 3: if Codec initialization failed (LD1 is turned on).
*******************************************************************************/
uint32_t I2S_CODEC_Init(uint32_t OutputDevice, uint32_t Address)
{
  uint32_t count = 0;

  /* Set the audio file address */
  AudioFileAddress = (uint32_t) Address;

  /* Configure the I2S2, I2C1 and GPIOF pins */
  I2S_GPIO_Config(); 

  /* Read the Audio file to extract the audio data length and frequency */
  errorcode = AudioFile_Init();

  if (errorcode < 3)
  {
    /* Turn on LD2 connected to PF.07 */
    return errorcode;
  }

  /* Configure the SPI2 peripheral in I2S mode */
  I2S_Config(I2S_STANDARD, I2S_MCLKOUTPUT, i2saudiofreq); 

  printf("\n\r i2saudiofreq:  0x%dHz ", i2saudiofreq);

  /* Set the current output device */
  CurrentOutputDevice = OutputDevice;

  /* Codec Configuration via I2C interface */
  count = CODEC_Config(OutputDevice, I2S_Standard_MSB, I2S_MCLKOUTPUT, DEFAULT_VOL);

  if (count != 0)
  {
    /* Turn on LD1 connected to PF.06 */
    return 3;
  }

  /* Turn on LD4 connected to PF.09 */
  return 0; /* Configuration is OK */
}

/*******************************************************************************
* Function Name  : I2S_CODEC_DataTransfer
* Description    : Sends the audio data using the SPI2 peripheral and checks the 
*                :   audio playing status (if a command (Pause/Stop) is pending 
*                :   the playing status is updated). If the TXE flag interrupt 
*                :   is used to synchronize data sending, this function should be 
*                :   called in the SPI2 ISR.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void I2S_CODEC_DataTransfer(void)
{
  /* Audio codec configuration section -------------------------------------*/
  if (GetVar_SendDummyData() == 1) 
  {
    /* Send a dummy data just to generate the I2S clock */
    SPI_I2S_SendData(SPI2, DUMMY_DATA);
    //printf("\n\r DUMMY_DATA!\n\r ");
  }
  /* Audio codec communication section -------------------------------------*/
  else
  {
    if(PlayMusicInSdCard == 1) /* 播放SD卡里的音乐 */
    {
      if(MusicBufferCurrentPos >= MUSIC_BUFFER_SIZE)
      {
        MusicBufferCurrentPos = 0;
      }
      SPI_I2S_SendData(SPI2, (MusicBuffer[MusicBufferCurrentPos]));
      MusicBufferCurrentPos++;
    }
    else                       /* 播放示例音乐 */
    {
      /* Send the data read from the memory */
      SPI_I2S_SendData(SPI2, (Media_ReadHalfWord(AudioDataIndex)));
  
      /* Increment the index */
      IncrementVar_AudioDataIndex(WAVE_Format.NumChannels); 
  
      /* Check and update the stream playing status */
      //I2S_CODEC_UpdateStatus();
    }
  } 
}

/*******************************************************************************
* Function Name  : I2S_CODEC_Play
* Description    : Plays the audio file.
* Input          : - AudioStartPosition: Adress from which the wave data begin
* Output         : None
* Return         : AudioDataIndex value.
*******************************************************************************/
uint32_t I2S_CODEC_Play(uint32_t AudioStartPosition)
{   
  /* Set Playing status to inform other modules about the codec status */
  SetVar_AudioPlayStatus(AudioPlayStatus_PLAYING);

  /* Enable the I2S2 TXE Interrupt  => Generate the clocks*/ 
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);

  return AudioDataIndex;
}

/*******************************************************************************
* Function Name  : delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length
* Output         : None
* Return         : The length of the wave file read from the SPI_Flash
*******************************************************************************/
void delay(__IO uint32_t nCount)
{
  for (; nCount != 0; nCount--);
}

/*******************************************************************************
* Function Name  : I2S_GPIO_Config
* Description    : Initializes the GPIO pins used by the codec application.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void I2S_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* I2S2 SD, CK and WS pins configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* I2S2 MCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* 连接到PCM1770的SPI接口片选 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* PCM1770的SPI接口片选无效 */
  GPIO_SetBits(GPIOA, GPIO_Pin_4);

  printf("\n\r\n\r I2S_GPIO_Config finishd");
}

/*******************************************************************************
* Function Name  : I2S_Config
* Description    : Configure the I2S Peripheral.
* Input          :  - Standard: I2S_Standard_Phillips, I2S_Standard_MSB or I2S_Standard_LSB
*                   - MCLKOutput: I2S_MCLKOutput_Enable or I2S_MCLKOutput_Disable
*                   - AudioFreq: I2S_AudioFreq_8K, I2S_AudioFreq_16K, I2S_AudioFreq_22K,
*                                I2S_AudioFreq_44K or I2S_AudioFreq_48K
* Output         : None
* Return         : None
*******************************************************************************/
static void I2S_Config(uint16_t Standard, uint16_t MCLKOutput, uint16_t AudioFreq)
{
  I2S_InitTypeDef I2S_InitStructure; 

  /* Enable I2S2 APB1 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

  /* Deinitialize SPI2 peripheral */
  SPI_I2S_DeInit(SPI2); 

  /* I2S2 peripheral configuration */
  I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
  I2S_InitStructure.I2S_Standard = Standard;
  I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
  I2S_InitStructure.I2S_MCLKOutput = MCLKOutput;
  I2S_InitStructure.I2S_AudioFreq = AudioFreq;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_High;  
  I2S_Init(SPI2, &I2S_InitStructure);

  /* Disable the I2S2 TXE Interrupt */ 
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
 
  /* Enable the SPI2/I2S2 peripheral */
  I2S_Cmd(SPI2, ENABLE);
  printf("\n\r I2S peripheral Config finished");
}

/*******************************************************************************
* Function Name  : GetVar_SendDummyData
* Description    : returns SendDummyData variable value (used by stm32f10x_it.c file).
* Input          : None
* Output         : None
* Return         : SendDummyData
*******************************************************************************/
uint32_t GetVar_SendDummyData(void)
{ 
  return SendDummyData;
}

/*******************************************************************************
* Function Name  : IncrementVar_AudioDataIndex
* Description    : Increment the AudioDataIndex variable.
* Input          : - IncrementNumber: number of incrementations.
* Output         : None
* Return         : None
*******************************************************************************/
void IncrementVar_AudioDataIndex(uint32_t IncrementNumber)
{ 
  AudioDataIndex += (uint32_t)IncrementNumber;
  
  if (AudioDataIndex >= AudioDataLength)
  {
    Decrement_AudioReplay();
  }
}

/*******************************************************************************
* Function Name  : SetVar_AudioReplay
* Description    : Decrement the AudioReplayCount variable if AudioReplay is different
*                :  from zero (infinite replaying).
* Input          : None.
* Output         : None
* Return         : AudioPlayStatus value.
*******************************************************************************/
void Decrement_AudioReplay(void)
{ 
  if (AudioReplay != 0)
  {
    AudioReplayCount--;

		if (AudioReplayCount == 0)
	  {
	    /* Command the Stop of the audio playing */ 
	    SetVar_AudioPlayStatus(AudioPlayStatus_STOPPED);
	    
	    /* Reset the counter */
	    AudioReplayCount = AudioReplay;
	  }
  }
}

/*******************************************************************************
* Function Name  : SetVar_AudioPlayStatus
* Description    : Set the AudioDataIndex variable to Status.
* Input          : Status: could be AudioPlayStatus_STOPPED, AudioPlayStatus_PLAYING
*                :        or AudioPlayStatus_PAUSED.
* Output         : None
* Return         : AudioPlayStatus value.
*******************************************************************************/
uint32_t SetVar_AudioPlayStatus(uint32_t Status)
{ 
  AudioPlayStatus = (uint32_t)Status;
 
  return AudioPlayStatus;
}

/*******************************************************************************
* Function Name  : Media_ReadHalfWord
* Description    : Read one half word from the media (SPI_Flash/NOR/NAND memories..)
* Input          : - Offset: the adress offset for read operation
* Output         : None.
* Return         : Data read from the media memory.
*******************************************************************************/
uint16_t Media_ReadHalfWord(uint32_t Offset)
{ 
  static uint32_t ReplayTimes = 0;  
  /* Test if the left channel is to be sent */
  if(monovar == 0)
  {
      if(AudioFileAddress + Offset > AudioFileAddressEnd)
      {
        AudioDataIndex = 0;
        /* Toggle selected LED */  
        STM_EVAL_LEDToggle(LED4);
        ReplayTimes++;
        printf("\r 重复播放 %d ... ", ReplayTimes); 
      }
    
      tmpvar =  (*(__IO uint16_t *) (AudioFileAddress + Offset));
      
      /* Increment the mono variable only if the file is in mono format */   
      if(WAVE_Format.NumChannels == Channel_MONO)
      {
        /* Increment the monovar variable */
        monovar++;
      }
   
      /* Return the read value */
      return tmpvar;
  }
  /* Right channel to be sent in mono format */
  else 
  {
    /* Reset the monovar variable */
    monovar = 0;
   
   /* Return the previous read data in mono format */
    return tmpvar;
  }	

}

/*******************************************************************************
* Function Name  : AudioFile_Init
* Description    : Initializes the SPI_Flsh and returns the Wavadatalength variable.
* Input          : None
* Output         : None
* Return         : - The length of the wave file read from the SPI_Flash
*                  - 1 if an error occured when initializing the memory.
*                  - 2 if an error occured on the audio file intialization.
*******************************************************************************/
uint32_t AudioFile_Init(void)
{
  uint32_t err = 0;
  
  /* Read and check the audio file Header */
  WaveFileStatus = I2S_CODEC_WaveParsing((uint8_t*)(0x8040000));
    
  /* Check if the selected file is a correct wave file */
  if(WaveFileStatus == Valid_WAVE_File)
  {
    /* Read and set the audio data length (/!\ data are counted as BYTES /!\) */ 
    AudioDataLength = WAVE_Format.DataSize ;  
    
    /* Read and set the audio frequency */
    i2saudiofreq = (uint16_t)WAVE_Format.SampleRate;

    /* Return the audio file length */
    return AudioDataLength;
  }
  else /* Wrong wave file */
  {
    printf("\n\rWaveFileStatus ErrorCode is : 0x%X\n\r",WaveFileStatus); 
    return 2;
  }
}


/*******************************************************************************
* Function Name  : CODEC_Config
* Description    : Configure the Codec in Headphone mode.
* Input          :  - OutputDevice: OutputDeviceHEADPHONE or OutputDeviceSPEAKER
*                :  - I2S_Standard: I2S communication standard could be I2S_Standard_Phillips
*                :       I2S_Standard_MSB or I2S_Standard_LSB.
*                :  -	I2S_MCLKOutput: could be I2S_MCLKOutput_
*                :  -	Volume: 
* Output         : None
* Return         : 0-> correct communication, else wrong communication
*******************************************************************************/
uint32_t CODEC_Config(uint16_t OutputDevice, uint16_t I2S_Standard, uint16_t I2S_MCLKOutput, uint8_t Volume)
{
  uint32_t Standard =0, counter = 0, PLLMode = 0;

  /* Command the sending of dummy data */
  ResetVar_SendDummyData();

  /* Reset the Codec Registers */
  I2S_CODEC_Reset(); 

  //SPI1_Init_For_PCM1770();

  /* Determine the I2S standard used */
  switch (I2S_Standard)
  {
    case I2S_Standard_Phillips: Standard = 0x03; break;
    case I2S_Standard_MSB:     Standard = 0x02; break;
    default :                   Standard = 0x01; break;
  }

  /* MCKI input frequency = 256.Fs */
  //counter += PCM1770_Write_reg(0x03, 0x80);
  		
  /* Command the sending of dummy data */
  SetVar_SendDummyData();
		
  /* Enable the I2S2 TXE Interrupt  => Generate the clocks*/ 
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);

  /* Disable the I2S2 TXE Interrupt */ 
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
  
  /* Disable the sending of Dummy data */
  ResetVar_SendDummyData();

  //SPI1_Init_For_LCD();
  
  /* Return the counter value */
  return counter;
}

/*******************************************************************************
* Function Name  : SetVar_SendDummyData
* Description    : Set the SendDummyData variable to 1.
* Input          : None
* Output         : None
* Return         : SendDummyData
*******************************************************************************/
uint32_t SetVar_SendDummyData(void)
{ 
  SendDummyData = (uint32_t)0x1;
 
  return SendDummyData;
}

/*******************************************************************************
* Function Name  : ResetVar_SendDummyData
* Description    : Reset the SendDummyData variable to 0.
* Input          : None
* Output         : None
* Return         : SendDummyData
*******************************************************************************/
uint32_t ResetVar_SendDummyData(void)
{ 
  SendDummyData = (uint32_t)0;
 
  return SendDummyData;
}

/*******************************************************************************
* Function Name  : I2S_CODEC_Reset
* Description    : Reset the Audio Codec.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void I2S_CODEC_Reset(void)
{
  /* Power Down the Codec */
	GPIO_ResetBits(Codec_PDN_GPIO, Codec_PDN_Pin); 

  /* wait for a delay to allow registers erasing */
  delay(0xFF);

  /* Power On the Codec after the power off => all registers are reinitialized*/
  GPIO_SetBits(Codec_PDN_GPIO, Codec_PDN_Pin);   
	 	
}