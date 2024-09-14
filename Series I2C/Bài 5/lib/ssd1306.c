#include "SSD1306.h"

//
//  Send a byte to the command register
//
static uint8_t SSD1306_WriteCommand(I2C_HandleTypeDef *hi2c, uint8_t command)
{
    return HAL_I2C_Mem_Write(hi2c, SSD1306_I2C_ADDR, 0x00, 1, &command, 1, 10);
}

//
//  Initialize the oled screen
//
void SSD1306_Init(SSD1306 *dev, I2C_HandleTypeDef *hi2c)
{
    // Set default values for screen object
    dev->currentX = 0;
    dev->currentY = 0;
    dev->initialized = 1;
    dev->hi2c = hi2c;

    // Wait for the screen to boot
    HAL_Delay(10);

    // Init LCD
    SSD1306_WriteCommand(hi2c, 0xAE); // Display off
    SSD1306_WriteCommand(hi2c, 0x20); // Set Memory Addressing Mode
    SSD1306_WriteCommand(hi2c, 0x10); // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    SSD1306_WriteCommand(hi2c, 0xB0); // Set Page Start Address for Page Addressing Mode,0-7
    SSD1306_WriteCommand(hi2c, 0xC8); // Set COM Output Scan Direction
    SSD1306_WriteCommand(hi2c, 0x00); // Set low column address
    SSD1306_WriteCommand(hi2c, 0x10); // Set high column address
    SSD1306_WriteCommand(hi2c, 0x40); // Set start line address
    SSD1306_WriteCommand(hi2c, 0x81); // set contrast control register
    SSD1306_WriteCommand(hi2c, 0xFF);
    SSD1306_WriteCommand(hi2c, 0xA1); // Set segment re-map 0 to 127
    SSD1306_WriteCommand(hi2c, 0xA6); // Set normal display
    SSD1306_WriteCommand(hi2c, 0xA8); // Set multiplex ratio(1 to 64)
    SSD1306_WriteCommand(hi2c, SSD1306_HEIGHT - 1);
    SSD1306_WriteCommand(hi2c, 0xA4); // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    SSD1306_WriteCommand(hi2c, 0xD3); // Set display offset
    SSD1306_WriteCommand(hi2c, 0x00); // No offset
    SSD1306_WriteCommand(hi2c, 0xD5); // Set display clock divide ratio/oscillator frequency
    SSD1306_WriteCommand(hi2c, 0xF0); // Set divide ratio
    SSD1306_WriteCommand(hi2c, 0xD9); // Set pre-charge period
    SSD1306_WriteCommand(hi2c, 0x22);
    SSD1306_WriteCommand(hi2c, 0xDA); // Set com pins hardware configuration
    SSD1306_WriteCommand(hi2c, SSD1306_COM_LR_REMAP << 5 | SSD1306_COM_ALTERNATIVE_PIN_CONFIG << 4 | 0x02);
    SSD1306_WriteCommand(hi2c, 0xDB); // Set vcomh
    SSD1306_WriteCommand(hi2c, 0x20); // 0x20,0.77xVcc
    SSD1306_WriteCommand(hi2c, 0x8D); // Set DC-DC enable
    SSD1306_WriteCommand(hi2c, 0x14); //
    SSD1306_WriteCommand(hi2c, 0xAF); // Turn on SSD1306 panel

    // Clear screen
    SSD1306_Fill(dev, SSD1306_BLACK);

    // Flush buffer to screen
    SSD1306_UpdateScreen(dev);
}

//
//  Fill the whole screen with the given color
//
void SSD1306_Fill(SSD1306 *dev, SSD1306_COLOR color)
{
    // Fill screenbuffer with a constant value (color)
    uint32_t i;

    for (i = 0; i < sizeof(dev->buffer); i++)
    {
        dev->buffer[i] = (color == SSD1306_BLACK) ? 0x00 : 0xFF;
    }
}

//
//  Write the screenbuffer with changed to the screen
//
void SSD1306_UpdateScreen(SSD1306 *dev)
{
    uint8_t i;
    I2C_HandleTypeDef *hi2c = dev->hi2c;
    for (i = 0; i < 8; i++)
    {
        SSD1306_WriteCommand(hi2c, 0xB0 + i);
        SSD1306_WriteCommand(hi2c, 0x00);
        SSD1306_WriteCommand(hi2c, 0x10);

        HAL_I2C_Mem_Write(hi2c, SSD1306_I2C_ADDR, 0x40, 1, &(dev->buffer[SSD1306_WIDTH * i]), SSD1306_WIDTH, 100);
    }
}

//
//  Draw one pixel in the screenbuffer
//  X => X Coordinate
//  Y => Y Coordinate
//  color => Pixel color
//
void SSD1306_DrawPixel(SSD1306 *dev, uint8_t x, uint8_t y, SSD1306_COLOR color)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
    {
        // Don't write outside the buffer
        return;
    }

    // Check if pixel should be inverted
    if (dev->inverted)
    {
        color = (SSD1306_COLOR)!color;
    }

    // Draw in the correct color
    if (color == SSD1306_WHITE)
    {
        dev->buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    }
    else
    {
        dev->buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

//
//  Draw 1 char to the screen buffer
//  ch      => Character to write
//  Font    => Font to use
//  color   => Black or White
//
char SSD1306_WriteChar(SSD1306 *dev, char ch, FontDef Font, SSD1306_COLOR color)
{
    uint32_t i, b, j;

    // Check remaining space on current line
    if (SSD1306_WIDTH <= (dev->currentX + Font.FontWidth) ||
        SSD1306_HEIGHT <= (dev->currentY + Font.FontHeight))
    {
        // Not enough space on current line
        return 0;
    }

    // Translate font to screenbuffer
    for (i = 0; i < Font.FontHeight; i++)
    {
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for (j = 0; j < Font.FontWidth; j++)
        {
            if ((b << j) & 0x8000)
            {
                SSD1306_DrawPixel(dev, dev->currentX + j, (dev->currentY + i), (SSD1306_COLOR)color);
            }
            else
            {
                SSD1306_DrawPixel(dev, dev->currentX + j, (dev->currentY + i), (SSD1306_COLOR)!color);
            }
        }
    }

    // The current space is now taken
    dev->currentX += Font.FontWidth;

    // Return written char for validation
    return ch;
}

//
//  Write full string to screenbuffer
//
char SSD1306_WriteString(SSD1306 *dev, const char *str, FontDef Font, SSD1306_COLOR color)
{
    // Write until null-byte
    while (*str)
    {
        if (SSD1306_WriteChar(dev, *str, Font, color) != *str)
        {
            // Char could not be written
            return *str;
        }

        // Next char
        str++;
    }

    // Everything ok
    return *str;
}

//
//  Invert background/foreground colors
//
void SSD1306_InvertColors(SSD1306 *dev)
{
    dev->inverted = !dev->inverted;
}

//
//  Set cursor position
//
void SSD1306_SetCursor(SSD1306 *dev, uint8_t x, uint8_t y)
{
    dev->currentX = x;
    dev->currentY = y;
}
