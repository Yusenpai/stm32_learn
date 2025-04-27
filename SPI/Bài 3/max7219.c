#include "max7219.h"

// Helper function to send data to the MAX7219
static void MAX7219_SendData(MAX7219 *dev, uint8_t reg, uint8_t data)
{
	HAL_GPIO_WritePin(dev->csPinPort, dev->csPin, GPIO_PIN_RESET); // Select the MAX7219
	HAL_SPI_Transmit(dev->hspi, &reg, 1, HAL_MAX_DELAY);		   // Send the register address
	HAL_SPI_Transmit(dev->hspi, &data, 1, HAL_MAX_DELAY);		   // Send the data
	HAL_GPIO_WritePin(dev->csPinPort, dev->csPin, GPIO_PIN_SET);   // Deselect the MAX7219
}

static void MAX7219_SendData32(MAX7219 *dev, uint8_t reg, uint32_t data)
{
	HAL_GPIO_WritePin(dev->csPinPort, dev->csPin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(dev->hspi, &reg, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(dev->hspi, (uint8_t *)&data + 3, 1, HAL_MAX_DELAY);

	HAL_SPI_Transmit(dev->hspi, &reg, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(dev->hspi, (uint8_t *)&data + 2, 1, HAL_MAX_DELAY);

	HAL_SPI_Transmit(dev->hspi, &reg, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(dev->hspi, (uint8_t *)&data + 1, 1, HAL_MAX_DELAY);

	HAL_SPI_Transmit(dev->hspi, &reg, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(dev->hspi, (uint8_t *)&data, 1, HAL_MAX_DELAY);

	HAL_GPIO_WritePin(dev->csPinPort, dev->csPin, GPIO_PIN_SET);
}

// Initialize the MAX7219 device
void MAX7219_Init(MAX7219 *dev, SPI_HandleTypeDef *hspi, GPIO_TypeDef *csPinPort, uint16_t csPin)
{
	dev->hspi = hspi;
	dev->csPinPort = csPinPort;
	dev->csPin = csPin;

	// Turn on the display (exit shutdown mode)
	MAX7219_Turn(dev, 1);

	// Set intensity to maximum (0x0F = maximum brightness)
	MAX7219_ChangeIntensity(dev, 0x0F);

	// Enable BCD decoding mode for all digits
	MAX7219_Decode(dev, 1);

	// Blank all digits (send 0x0F to all digit registers to display nothing)
	MAX7219_PrintDigit(dev, DIGIT_1, BLANK);
	MAX7219_PrintDigit(dev, DIGIT_2, BLANK);
	MAX7219_PrintDigit(dev, DIGIT_3, BLANK);
	MAX7219_PrintDigit(dev, DIGIT_4, BLANK);
	MAX7219_PrintDigit(dev, DIGIT_5, BLANK);
	MAX7219_PrintDigit(dev, DIGIT_6, BLANK);
	MAX7219_PrintDigit(dev, DIGIT_7, BLANK);
	MAX7219_PrintDigit(dev, DIGIT_8, BLANK);

	// Set scan limit to display all digits (0x07 means all 7 digits will be scanned)
	MAX7219_SendData(dev, REG_SCAN_LIMIT, 0x07);
}

// Print a number on a specific digit
void MAX7219_PrintDigit(MAX7219 *dev, MAX7219_Digits position, MAX7219_Numeric number)
{
	MAX7219_SendData(dev, position, number);
}

void MAX7219_PrintNumber(MAX7219 *dev, uint64_t number)
{
	uint8_t digits[8];
	uint8_t i = 0;
	uint8_t leading_zero = 1; // Flag to skip leading zeros

	// Extract each digit from the number (from most significant to least significant)
	digits[0] = (number / 10000000) % 10; // digit 8
	digits[1] = (number / 1000000) % 10;  // digit 7
	digits[2] = (number / 100000) % 10;	  // digit 6
	digits[3] = (number / 10000) % 10;	  // digit 5
	digits[4] = (number / 1000) % 10;	  // digit 4
	digits[5] = (number / 100) % 10;	  // digit 3
	digits[6] = (number / 10) % 10;		  // digit 2
	digits[7] = number % 10;			  // digit 1

	// Loop through digits and print, skipping leading zeros
	for (i = 0; i < 8; i++)
	{
		if (digits[i] != 0 || !leading_zero || i == 7)
		{
			// Start printing once a non-zero digit is found, or always print the last digit
			leading_zero = 0;
			MAX7219_PrintDigit(dev, DIGIT_8 - i, digits[i]); // Print from DIGIT_8 to DIGIT_1
		}
		else
		{
			// Clear leading zero positions by setting them to BLANK
			MAX7219_PrintDigit(dev, DIGIT_8 - i, BLANK);
		}
	}
}

void MAX7219_PrintFloat(MAX7219 *dev, float number, uint8_t decimalPlace)
{
	// Ensure decimalPlace does not exceed display limits
	if (decimalPlace > 7)
	{
		decimalPlace = 7; // Cap the decimal places to fit into 8 digits
	}

	// Scale the number to an integer
	uint64_t scaled_number = (uint64_t)round(number * pow(10, decimalPlace));

	// Calculate the number of digits
	uint8_t total_digits = 8;

	// Extract integer and fractional parts
	uint64_t integer_part = scaled_number / (uint64_t)pow(10, decimalPlace);
	uint64_t fraction_part = scaled_number % (uint64_t)pow(10, decimalPlace);

	// Prepare the digits array, initialize with blanks
	uint8_t digits[8] = {BLANK}; // Use BLANK or appropriate empty value

	// Fill the digits array from the fraction part
	for (int i = 0; i < decimalPlace; i++)
	{
		digits[total_digits - 1 - i] = fraction_part % 10;
		fraction_part /= 10;
	}

	// Fill the digits array from the integer part
	for (int i = decimalPlace; i < total_digits; i++)
	{
		digits[total_digits - 1 - i + decimalPlace] = integer_part % 10;
		integer_part /= 10;
	}

	// Print all digits, setting the decimal point position
	for (int i = 0; i < total_digits; i++)
	{
		if (i == total_digits - decimalPlace - 1)
		{
			// Set the decimal point
			MAX7219_PrintDot(dev, DIGIT_8 - i, 1);
		}
		// Print the current digit
		MAX7219_PrintDigit(dev, DIGIT_8 - i, digits[i]);
	}
}

// Print a number on a specific digit with a dot
void MAX7219_PrintDot(MAX7219 *dev, MAX7219_Digits position, uint8_t dot)
{
	uint8_t value = dot ? (0x80) : 0x00; // Dot is set using the MSB (0x80)
	MAX7219_SendData(dev, position, value);
}

// Turn the MAX7219 display on or off
void MAX7219_Turn(MAX7219 *dev, uint8_t onOff)
{
	if (onOff)
	{
		MAX7219_SendData(dev, REG_SHUTDOWN, 0x01); // Normal operation mode
	}
	else
	{
		MAX7219_SendData(dev, REG_SHUTDOWN, 0x00); // Shutdown mode
	}
}

// Change the intensity (brightness) of the display
void MAX7219_ChangeIntensity(MAX7219 *dev, uint8_t intensity)
{
	if (intensity > 0x0F)
		intensity = 0x0F; // Clamp intensity to valid range (0-15)
	MAX7219_SendData(dev, REG_INTENSITY, intensity);
}

// Set decode mode for the display (on: decoded mode, off: raw mode)
void MAX7219_Decode(MAX7219 *dev, uint8_t onOff)
{
	if (onOff)
	{
		MAX7219_SendData(dev, REG_DECODE_MODE, 0xFF); // Use BCD decoding for all digits
	}
	else
	{
		MAX7219_SendData(dev, REG_DECODE_MODE, 0x00); // No decoding, raw mode
	}
}

void MAX7219_Clear(MAX7219 *dev)
{
	// Blank all digits (send BLANK to each digit)
	MAX7219_PrintDigit(dev, DIGIT_8, BLANK);
	MAX7219_PrintDigit(dev, DIGIT_7, BLANK);
	MAX7219_PrintDigit(dev, DIGIT_6, BLANK);
	MAX7219_PrintDigit(dev, DIGIT_5, BLANK);
	MAX7219_PrintDigit(dev, DIGIT_4, BLANK);
	MAX7219_PrintDigit(dev, DIGIT_3, BLANK);
	MAX7219_PrintDigit(dev, DIGIT_2, BLANK);
	MAX7219_PrintDigit(dev, DIGIT_1, BLANK);
}

void MAX7219_InitMatrix(MAX7219 *dev, SPI_HandleTypeDef *hspi, GPIO_TypeDef *csPinPort, uint16_t csPin)
{
	// Assign the SPI handle and chip select pin to the device structure
	dev->hspi = hspi;
	dev->csPinPort = csPinPort;
	dev->csPin = csPin;

	// Set the CS pin to high (inactive state)
	HAL_GPIO_WritePin(dev->csPinPort, dev->csPin, GPIO_PIN_SET);

	// Disable decode mode (used for matrix mode)
	MAX7219_Decode(dev, 0);

	// Set scan limit to all 8 digits (columns) active
	MAX7219_SendData(dev, REG_SCAN_LIMIT, 0x07);

	// Exit shutdown mode (turn on display)
	MAX7219_Turn(dev, 1);

	// Clear the display
	for (int i = 1; i <= 8; i++)
	{
		MAX7219_SendData(dev, i, 0x00); // Clear each row (digit)
	}
}

void MAX7219_PrintMatrix(MAX7219 *dev, uint8_t *buffer)
{
	// Send the 8 bytes from the buffer to the MAX7219, each byte corresponds to one row
	for (int i = 0; i < 8; i++)
	{
		MAX7219_SendData(dev, DIGIT_1 + i, buffer[i]);
	}
}

void MAX7219_PrintMatrix32(MAX7219 *dev, uint32_t *buffer)
{
	for (int i = 0; i < 8; i++)
	{
		MAX7219_SendData32(dev, DIGIT_1 + i, buffer[i]);
	}
}
