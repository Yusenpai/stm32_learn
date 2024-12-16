#include "dht11.h"

#define WAIT_FOR_PIN_STATE(level)                          \
	{                                                      \
		uint32_t timeout = HAL_GetTick() + 2;              \
		while (HAL_GPIO_ReadPin(dhtPort, dhtPin) != level) \
		{                                                  \
			if (HAL_GetTick() > timeout)                   \
			{                                              \
				return 1; /* Timeout error */              \
			}                                              \
		}                                                  \
	}

#define CHECK_PIN_STATE(level)                          \
	{                                                   \
		if (HAL_GPIO_ReadPin(dhtPort, dhtPin) != level) \
			return 1;                                   \
	}

uint8_t readDHT11(DHT11 *device)
{
	uint8_t dhtPin = device->pin;
	uint8_t dhtPort = device->port;

	// Data frame: | MCU Start | DHT Response | H.int | H.dec | T.int | T.dec | checksum |

	/* 1. MCU send START signal. LOW for >=18ms then HIGH for 20us - 40us */
	HAL_GPIO_WritePin(dhtPort, dhtPin, 0);
	HAL_Delay(20);
	HAL_GPIO_WritePin(dhtPort, dhtPin, 1);
	delayUs(40);

	/* 2. DHT Response. LOW for 80us and HIGH for 80us */
	delayUs(20);
	CHECK_PIN_STATE(0);
	delayUs(80);
	CHECK_PIN_STATE(1);
	WAIT_FOR_PIN_STATE(0);

	/* 3. Read DHT data */
	uint8_t dhtData[5];
	for (uint8_t j = 0; j < 5; j++)
		for (uint8_t i = 0; i < 8; i++)
		{
			WAIT_FOR_PIN_STATE(1);
			delayUs(40);
			uint8_t sample = HAL_GPIO_ReadPin(dhtPort, dhtPin);
			dhtData[j] = (dhtData[j] << 1) | sample;
			if (sample == 1)
				WAIT_FOR_PIN_STATE(0);
		}

	/* 4. Checksum */
	uint8_t checksum = 0;
	for (uint8_t i = 0; i < 4; i++)
	{
		checksum ^= dhtData[i];
		if (checksum != dhtData[4])
			return 1;
	}

	/* 5. Write data out */
	device->humidity = dhtData[0];
	device->temperature = dhtData[2];

	return 0;
}
void initDHT11(DHT11 *device, GPIO_TypeDef *port, uint32_t pin)
{
	device->port = port;
	device->pin = pin;
	device->temperature = 0;
	device->humidity = 0;
}
