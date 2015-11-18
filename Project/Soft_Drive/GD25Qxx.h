#ifndef GD25Qxx_H
#define GD25Qxx_H

#define SPI_FLASH_PageSize    0x100     //256

/*----- High layer function -----*/
void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
uint32_t SPI_FLASH_ReadID(void);
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr);

/*----- Low layer function -----*/
uint8_t SPI_FLASH_ReadByte(void);
uint8_t SPI_FLASH_SendByte(uint8_t byte);
uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

#endif
