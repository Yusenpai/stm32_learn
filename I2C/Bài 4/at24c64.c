#include "at24c64.h"

// Function to initialize AT24C64 handle
void AT24C64_Init(AT24C64 *eeprom, I2C_HandleTypeDef *hi2c, uint8_t deviceAddress)
{
	eeprom->hi2c = hi2c;
	eeprom->deviceAddress = deviceAddress << 1; // Shift left to make it a 7-bit address
}

// Function to write data to AT24C64
void AT24C64_Write(AT24C64 *eeprom, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
	// Prepare data buffer with memory address (2 bytes for AT24C64)
	uint8_t dataBuffer[AT24C64_PAGE_SIZE + 2];
	dataBuffer[0] = (MemAddress >> 8) & 0xFF; // MSB of memory address
	dataBuffer[1] = MemAddress & 0xFF;		  // LSB of memory address

	// Copy the data to write after the address
	memcpy(&dataBuffer[2], pData, Size);

	// Write data to EEPROM
	HAL_I2C_Master_Transmit(eeprom->hi2c, eeprom->deviceAddress, dataBuffer, Size + 2, HAL_MAX_DELAY);

	// EEPROM write cycle time (typically 5ms max for AT24C64)
	HAL_Delay(5);
}

// Function to read data from AT24C64
void AT24C64_Read(AT24C64 *eeprom, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
	// Send memory address (2 bytes for AT24C64)
	uint8_t addrBuffer[2];
	addrBuffer[0] = (MemAddress >> 8) & 0xFF; // MSB of memory address
	addrBuffer[1] = MemAddress & 0xFF;		  // LSB of memory address

	// Write memory address to EEPROM
	HAL_I2C_Master_Transmit(eeprom->hi2c, eeprom->deviceAddress, addrBuffer, 2, HAL_MAX_DELAY);

	// Read data from EEPROM
	HAL_I2C_Master_Receive(eeprom->hi2c, eeprom->deviceAddress, pData, Size, HAL_MAX_DELAY);
}

// Function to write a page of data (up to 32 bytes) to AT24C64
void AT24C64_WritePage(AT24C64 *eeprom, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
	if (Size > AT24C64_PAGE_SIZE)
	{
		Size = AT24C64_PAGE_SIZE; // Limit the size to page size if too large
	}
	AT24C64_Write(eeprom, MemAddress, pData, Size);
}

// Function to write multiple pages of data to AT24C64
void AT24C64_WriteMultiPage(AT24C64 *eeprom, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
	uint16_t remainingBytes = Size;
	uint16_t currentAddress = MemAddress;

	while (remainingBytes > 0)
	{
		uint16_t bytesToWrite = (remainingBytes > AT24C64_PAGE_SIZE) ? AT24C64_PAGE_SIZE : remainingBytes;

		AT24C64_WritePage(eeprom, currentAddress, pData, bytesToWrite);

		remainingBytes -= bytesToWrite;
		currentAddress += bytesToWrite;
		pData += bytesToWrite;
	}
}