/*************************************************************************
modified ILI9325D API for atmega328p(Arduino Nano/Uno)
@author Hank
@page https://github.com/hankso
*************************************************************************/

#ifndef ILI9325D_328P_H
#define ILI9325D_328P_H

#define White          0xFFFF
#define Black          0x0000
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#define RGB16(r,g,b) (((uint16_t)(r >> 3) << 11) | ((uint16_t)(g >> 2) << 5) | (b >> 3))
#define RGB16_RED      0xF800
#define RGB16_GREEN    0x07E0
#define RGB16_BLUE     0x001F
#define RGB16_YELLOW   0xFFE0
#define RGB16_CYAN     0x7FF0
#define RGB16_PINK     0xF81F
#define RGB16_WHITE    0xFFFF
#define RGB16_BLACK    0x0000

class ILI9325D_328p
{
    public:
        ILI9325D_328p(uint8_t PIN_CS, uint8_t PIN_RS, uint8_t PIN_RST,
                      uint8_t PIN_SCL, uint8_t PIN_SDA,
                      uint16_t width, uint16_t height);

        void setCursor(uint16_t column, uint8_t line)
        {
            m_y = column;
            m_x = (uint16_t)line * 8;
        }
        void setColor(uint16_t color)
        {
            m_color[1] = color;
        }
        void setColor(uint8_t R, uint8_t G, uint8_t B)
        {
            m_color[1] = ((uint16_t)R << 11) | ((uint16_t)G << 5) | B;
        }
        void setBackColor(uint16_t color)
        {
            m_color[0] = color;
        }
        void setBackColor(uint8_t R, uint8_t G, uint8_t B)
        {
            m_color[0] = ((uint16_t)R << 11) | ((uint16_t)G << 5) | B;
        }

        void
            WriteCommandData(uint8_t, uint16_t),
            DisplayON(void),
            DisplayOFF(void),
            begin(void),
            setXY(uint16_t, uint16_t),
            setWindow(uint16_t, uint16_t, uint16_t, uint16_t),
            clearPixels(uint32_t),
            clearWindow(uint16_t x=-1, uint16_t y=-1,
                        uint16_t width=-1, uint16_t heigh=-1),
            clearLine(uint16_t line, bool vertical=true),
            drawPoint(uint16_t, uint16_t, uint16_t);
        void
            write8(uint8_t),
            write16(uint16_t);
    private:
        uint8_t
            CS,
            RS,
            RST,
            SCL,
            SDA;
        uint16_t
            m_x,
            m_y,
            m_width,
            m_height,
            m_color[2];
        byte lastData;
};

#endif
