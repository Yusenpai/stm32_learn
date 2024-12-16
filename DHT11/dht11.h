#ifndef __DHT11_H__
#define __DHT11_H__

#include "main.h"
#include "stdint.h"

typedef struct
{
	uint8_t humidity;
	uint8_t temperature;
	GPIO_TypeDef *port;
	uint32_t pin;
} DHT11;

void initDHT11(DHT11 *device, GPIO_TypeDef *port, uint32_t pin);
uint8_t readDHT11(DHT11 *device);

#endif // __DHT11_H__