#ifndef _SSD1306_H
#define _SSD1306_H

#include "main.h"
#include "fonts.h"

#define SSD1306_I2C_ADDR 0x78
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_COM_LR_REMAP 0
#define SSD1306_COM_ALTERNATIVE_PIN_CONFIG 1

typedef enum
{
    SSD1306_BLACK = 0x00, // Black color, no pixel
    SSD1306_WHITE = 0x01, // Pixel is set. Color depends on LCD
} SSD1306_COLOR;

typedef struct
{
    uint16_t currentX;
    uint16_t currentY;
    uint8_t inverted;
    uint8_t initialized;
    I2C_HandleTypeDef *hi2c;
    uint8_t buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
} SSD1306;

void SSD1306_Init(SSD1306 *dev, I2C_HandleTypeDef *hi2c);
void SSD1306_Fill(SSD1306 *dev, SSD1306_COLOR color);
void SSD1306_UpdateScreen(SSD1306 *dev);
void SSD1306_DrawPixel(SSD1306 *dev, uint8_t x, uint8_t y, SSD1306_COLOR color);
char SSD1306_WriteChar(SSD1306 *dev, char ch, FontDef Font, SSD1306_COLOR color);
char SSD1306_WriteString(SSD1306 *dev, const char *str, FontDef Font, SSD1306_COLOR color);
void SSD1306_InvertColors(SSD1306 *dev);
void SSD1306_SetCursor(SSD1306 *dev, uint8_t x, uint8_t y);

#endif // _SSD1306_H
