#ifndef __DS1307_H__
#define __DS1307_H__

#include "main.h"

#define DS1307_I2C_ADDR (0x68 << 1)
#define DS1307_REG_SECOND 0x00
#define DS1307_REG_MINUTE 0x01
#define DS1307_REG_HOUR 0x02
#define DS1307_REG_DOW 0x03
#define DS1307_REG_DATE 0x04
#define DS1307_REG_MONTH 0x05
#define DS1307_REG_YEAR 0x06
#define DS1307_REG_CONTROL 0x07

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
	I2C_HandleTypeDef *i2cHandle;
} DS1307;

void DS1307_Init(DS1307 *dev, I2C_HandleTypeDef *hi2c);
void DS1307_ReadTime(DS1307 *dev);
void DS1307_SetTime(DS1307 *dev);
void DS1307_SqwEnable(DS1307 *dev, uint8_t enable);

#endif // __DS1307_H__
