#ifndef __MAX7219_H__
#define __MAX7219_H__

#include "main.h"
#include <math.h>

typedef enum
{
	DIGIT_1 = 1,
	DIGIT_2,
	DIGIT_3,
	DIGIT_4,
	DIGIT_5,
	DIGIT_6,
	DIGIT_7,
	DIGIT_8
} MAX7219_Digits;

typedef enum
{
	REG_NO_OP = 0x00,
	REG_DIGIT_0 = 0x01,
	REG_DIGIT_1 = 0x02,
	REG_DIGIT_2 = 0x03,
	REG_DIGIT_3 = 0x04,
	REG_DIGIT_4 = 0x05,
	REG_DIGIT_5 = 0x06,
	REG_DIGIT_6 = 0x07,
	REG_DIGIT_7 = 0x08,
	REG_DECODE_MODE = 0x09,
	REG_INTENSITY = 0x0A,
	REG_SCAN_LIMIT = 0x0B,
	REG_SHUTDOWN = 0x0C,
	REG_DISPLAY_TEST = 0x0F,
} MAX7219_REGISTERS;

typedef enum
{
	NUM_0 = 0x00,
	NUM_1 = 0x01,
	NUM_2 = 0x02,
	NUM_3 = 0x03,
	NUM_4 = 0x04,
	NUM_5 = 0x05,
	NUM_6 = 0x06,
	NUM_7 = 0x07,
	NUM_8 = 0x08,
	NUM_9 = 0x09,
	MINUS = 0x0A,
	LETTER_E = 0x0B,
	LETTER_H = 0x0C,
	LETTER_L = 0x0D,
	LETTER_P = 0x0E,
	BLANK = 0x0F
} MAX7219_Numeric;

typedef struct
{
	GPIO_TypeDef *csPinPort;
	uint16_t csPin;
	SPI_HandleTypeDef *hspi;
} MAX7219;

void MAX7219_Init(MAX7219 *dev, SPI_HandleTypeDef *hspi, GPIO_TypeDef *csPinPort, uint16_t csPin);
void MAX7219_PrintDigit(MAX7219 *dev, MAX7219_Digits position, MAX7219_Numeric number);
void MAX7219_PrintNumber(MAX7219 *dev, uint64_t number);
// void MAX7219_PrintFloat(MAX7219 *dev, float number, uint8_t decimalPlace);
void MAX7219_PrintDot(MAX7219 *dev, MAX7219_Digits position, uint8_t dot);
void MAX7219_Turn(MAX7219 *dev, uint8_t onOff);
void MAX7219_ChangeIntensity(MAX7219 *dev, uint8_t intensity);
void MAX7219_Decode(MAX7219 *dev, uint8_t onOff);
void MAX7219_Clear(MAX7219 *dev);

void MAX7219_InitMatrix(MAX7219 *dev, SPI_HandleTypeDef *hspi, GPIO_TypeDef *csPinPort, uint16_t csPin);
void MAX7219_PrintMatrix(MAX7219 *dev, uint8_t *buffer);
void MAX7219_PrintMatrix32(MAX7219 *dev, uint32_t *buffer);

#endif // __MAX7219_H__
