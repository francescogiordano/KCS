#ifndef _FLASHMEMORY_H_
#define _FLASHMEMORY_H_

#include <stdbool.h>
#include <stdint.h>

#define FLASH_MEMORY_START_ADDRESS			0x00039C00
#define FLASH_MEMORY_START_PAGE				0x00E7
#define FLASH_MEMORY_MAX_PAGE_COUNT			0x0001
#define FLASH_MEMORY_PAGE_SIZE				0x0400

void SetFlashMemoryOpDoneFlag(uint8_t);
uint8_t GetFlashMemoryOpDoneFlag(void);

uint32_t GetFlashMemoryStartAddress(void);

uint32_t FlashMemoryPageErase(uint32_t pageOffset, uint32_t count);
uint32_t FlashMemoryPageWrite(uint32_t pageOffset, uint32_t* data, uint32_t count);

#endif
