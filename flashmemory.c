#include "flashmemory.h"
#include "nrf_error.h"
#include "nrf_soc.h"
#include "usart.h"

static volatile uint8_t flashMemoryOpDoneFlag = 0;

void SetFlashMemoryOpDoneFlag(uint8_t value) {
	flashMemoryOpDoneFlag = value;
}

uint8_t GetFlashMemoryOpDoneFlag(void) {
	return flashMemoryOpDoneFlag;
}
uint32_t GetFlashMemoryStartAddress(void) {
	return FLASH_MEMORY_START_ADDRESS;
}

uint32_t FlashMemoryPageErase(uint32_t pageOffset, uint32_t count) {
	uint32_t value = NRF_ERROR_INVALID_LENGTH;
	
	if (count <= FLASH_MEMORY_MAX_PAGE_COUNT) {
		for (int i = 0; i<count; i++) {
			flashMemoryOpDoneFlag = 0;
			value = sd_flash_page_erase(FLASH_MEMORY_START_PAGE + pageOffset + i);
			while (flashMemoryOpDoneFlag == 0);
		}
	}
	return value;
}
uint32_t FlashMemoryPageWrite(uint32_t pageOffset, uint32_t* data, uint32_t count) {
	uint32_t value;

	flashMemoryOpDoneFlag = 0;
	value = sd_flash_write((uint32_t*)FLASH_MEMORY_START_ADDRESS, data, count);
	while (flashMemoryOpDoneFlag == 0);

	return value;
}