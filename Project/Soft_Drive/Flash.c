#include "stm32f10x.h"
#include "Flash.h"
#include "stdio.h"

#define StartAddr  ((uint32_t)0x0801F000)
#define EndAddr    ((uint32_t)0x0801F400)

#define DataStartAddr   ((uint32_t)0x0801F000)
#define DataEndAddr     ((uint32_t)0x0801F00E)

#if defined (STM32F10X_HD) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

uint32_t EraseCounter = 0x00, Address = 0x00;
uint16_t Flash_Data,Flash_Data1,Flash_Data3,Flash_Data4,Flash_Data5,Flash_Data6;
uint32_t Flash_Data2;
uint8_t  ReadDataBuf[14];
uint32_t  ReadDataAddr = 0x00;
__IO uint32_t NbrOfPage = 0x00;
__IO FLASH_Status FLASHStatus;
__IO TestStatus MemoryProgramStatus;

void Flash_Program_Test(void)
{
  FLASHStatus = FLASH_COMPLETE;
  MemoryProgramStatus = PASSED;

  Flash_Data1 = 0x0001;
  Flash_Data2 = 0x12345678;
  Flash_Data3 = 0xabcd;
  Flash_Data4 = 0x5678;
  Flash_Data5 = 0x1234;
  Flash_Data6 = 0xab12;
 
  
  /* Unlock the Flash Program Erase controller */
  FLASH_Unlock();
  
  /* Define the number of page to be erased */
  NbrOfPage = (EndAddr - StartAddr) / FLASH_PAGE_SIZE;

  /* Clear All pending flags */
  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

  ReadDataAddr = DataStartAddr;

  while( ReadDataAddr < DataEndAddr)
  {
    ReadDataBuf[Address] = *(__IO uint32_t*)(ReadDataAddr);
 	ReadDataAddr +=1;
    Address += 1;
	printf("\n\rThe Flash Data is 0x%02X\n\r",ReadDataBuf[Address-1]);	

  }
  
  /* Erase the FLASH pages */
  for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  {
    FLASHStatus = FLASH_ErasePage(StartAddr + (FLASH_PAGE_SIZE * EraseCounter));
  }
  
  /*  FLASH Word program of Flash_Data1 at define addresses*/
  Address = DataStartAddr;
  while((Address < DataStartAddr+2) && (FLASHStatus == FLASH_COMPLETE))
  {
    FLASHStatus = FLASH_ProgramHalfWord(Address, Flash_Data1);
    Address = Address + 2;
  }
    
  /*  FLASH Word program of Flash_Data2 at define addresses*/
  while((Address < DataStartAddr+6) && (FLASHStatus == FLASH_COMPLETE))
  {
    FLASHStatus = FLASH_ProgramWord(Address, Flash_Data2);
    Address = Address + 4;
  }
  
  /*  FLASH Word program of Flash_Data3 at define addresses*/
  while((Address < DataStartAddr+8) && (FLASHStatus == FLASH_COMPLETE))
  {
    FLASHStatus = FLASH_ProgramHalfWord(Address, Flash_Data3);
    Address = Address + 2;
  }

  /*  FLASH Word program of Flash_Data4 at define addresses*/
  while((Address < DataStartAddr+10) && (FLASHStatus == FLASH_COMPLETE))
  {
    FLASHStatus = FLASH_ProgramHalfWord(Address, Flash_Data4);
    Address = Address + 2;
  }

  /*  FLASH Word program of Flash_Data5 at define addresses*/
  while((Address < DataStartAddr+12) && (FLASHStatus == FLASH_COMPLETE))
  {
    FLASHStatus = FLASH_ProgramHalfWord(Address, Flash_Data5);
    Address = Address + 2;
  }

  /*  FLASH Word program of Flash_Data6 at define addresses*/
  while((Address < DataStartAddr+14) && (FLASHStatus == FLASH_COMPLETE))
  {
    FLASHStatus = FLASH_ProgramHalfWord(Address, Flash_Data6);
    Address = Address + 2;
  }
}
