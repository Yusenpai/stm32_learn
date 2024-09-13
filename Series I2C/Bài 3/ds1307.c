#include "ds1307.h"

void DS1307_Init(DS1307 *dev, I2C_HandleTypeDef *hi2c)
{
	dev->i2cHandle = hi2c;
	dev->seconds = 0;
	dev->minutes = 0;
	dev->hours = 0;
	dev->day = 0;
	dev->date = 0;
	dev->month = 0;
	dev->year = 0;
}

static uint8_t BCDToDec(uint8_t val)
{
	return (val >> 4) * 10 + (val & 0x0F);
}

static uint8_t DecToBCD(uint8_t val)
{
	return ((val / 10) << 4) | (val % 10);
}

void DS1307_ReadTime(DS1307 *dev)
{
	uint8_t buffer[7];
	HAL_I2C_Mem_Read(dev->i2cHandle, DS1307_I2C_ADDR, DS1307_REG_SECOND, 1, buffer, 7, HAL_MAX_DELAY);

	dev->seconds = BCDToDec(buffer[0] & 0x7F); // Mask for the CH (clock halt) bit
	dev->minutes = BCDToDec(buffer[1]);
	dev->hours = BCDToDec(buffer[2] & 0x3F); // 24-hour mode
	dev->day = BCDToDec(buffer[3]);
	dev->date = BCDToDec(buffer[4]);
	dev->month = BCDToDec(buffer[5]);
	dev->year = BCDToDec(buffer[6]);
}

void DS1307_SetTime(DS1307 *dev)
{
	uint8_t buffer[7];

	buffer[0] = DecToBCD(dev->seconds);
	buffer[1] = DecToBCD(dev->minutes);
	buffer[2] = DecToBCD(dev->hours); // Assuming 24-hour mode
	buffer[3] = DecToBCD(dev->day);
	buffer[4] = DecToBCD(dev->date);
	buffer[5] = DecToBCD(dev->month);
	buffer[6] = DecToBCD(dev->year);

	HAL_I2C_Mem_Write(dev->i2cHandle, DS1307_I2C_ADDR, DS1307_REG_SECOND, 1, buffer, 7, HAL_MAX_DELAY);
}

void DS1307_SqwEnable(DS1307 *dev, uint8_t enable)
{
	uint8_t control;
	HAL_I2C_Mem_Read(dev->i2cHandle, DS1307_I2C_ADDR, DS1307_REG_CONTROL, 1, &control, 1, HAL_MAX_DELAY);

	if (enable)
		control |= 0x10; // Set square wave enable bit
	else
		control &= ~0x10; // Clear square wave enable bit

	HAL_I2C_Mem_Write(dev->i2cHandle, DS1307_I2C_ADDR, DS1307_REG_CONTROL, 1, &control, 1, HAL_MAX_DELAY);
}