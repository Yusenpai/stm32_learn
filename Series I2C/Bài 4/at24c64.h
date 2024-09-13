#ifndef __AT24C64_H__
#define __AT24C64_H__

#include "main.h"
#include "string.h"

// AT24C64 memory-related constants
#define AT24C64_PAGE_SIZE 32
#define AT24C64_MEMORY_SIZE 8192

// Struct to hold the I2C handle and device-specific data
typedef struct
{
	I2C_HandleTypeDef *hi2c; // Pointer to the I2C handle
	uint16_t deviceAddress;	 // I2C address of the AT24C64 (7-bit left-shifted)
} AT24C64;

void AT24C64_Init(AT24C64 *eeprom, I2C_HandleTypeDef *hi2c, uint8_t deviceAddress);
void AT24C64_Write(AT24C64 *eeprom, uint16_t MemAddress, uint8_t *pData, uint16_t Size);
void AT24C64_Read(AT24C64 *eeprom, uint16_t MemAddress, uint8_t *pData, uint16_t Size);
void AT24C64_WritePage(AT24C64 *eeprom, uint16_t MemAddress, uint8_t *pData, uint16_t Size);
void AT24C64_WriteMultiPage(AT24C64 *eeprom, uint16_t MemAddress, uint8_t *pData, uint16_t Size);

#endif // __AT24C64_H__