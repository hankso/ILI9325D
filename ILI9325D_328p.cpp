/*************************************************************************
modified ILI9325D API for atmega328p(Arduino Nano/Uno)
@author Hank
@page https://github.com/hankso
*************************************************************************/

#include <Arduino.h>
#include "ILI9325D_328p.h"

ILI9325D_328p::ILI9325D_328p(
    uint8_t PIN_CS,
    uint8_t PIN_RS,
    uint8_t PIN_RST,
    uint8_t PIN_SCL,
    uint8_t PIN_SDA,
    uint16_t width,
    uint16_t height)
{
    this->CS = PIN_CS;
    this->RS = PIN_RS;
    this->RST = PIN_RST;
    this->SCL = PIN_SCL;
    this->SDA = PIN_SDA;
    pinMode(CS, OUTPUT);
    digitalWrite(CS, HIGH);
    pinMode(RS, OUTPUT);
    digitalWrite(RS, LOW);
    pinMode(RST, OUTPUT);
    digitalWrite(RST, HIGH);
    pinMode(SCL, OUTPUT);
    digitalWrite(SCL, LOW);
    pinMode(SDA, OUTPUT);
    digitalWrite(SDA, LOW);
    this->m_width = width;
    this->m_height = height;
}

void ILI9325D_328p::write8(uint8_t data)
{
    digitalWrite(CS, LOW);
    for (uint8_t i=0; i < 8; i++, data>>=1)
    {
        digitalWrite(SCL, LOW);
        digitalWrite(SDA, data & 1);
        digitalWrite(SCL, HIGH);
    }
    digitalWrite(CS, HIGH);
}

void ILI9325D_328p::write16(uint16_t data)
{
    write8( *((uint8_t *)&data + 1) );
    write8( (uint8_t)data );
}

void ILI9325D_328p::WriteCommandData(uint8_t cmd, uint16_t data)
{
    digitalWrite(RS, LOW); // command select
    write8(cmd);
    digitalWrite(RS, HIGH); // data select
    write16(data);
}

void ILI9325D_328p::DisplayON(void)
{
    WriteCommandData(0x07, 0x0001); // set GON=0, DTE=0, D[1:0]=01
    delay(100); // wait for 100ms
    WriteCommandData(0x07, 0x0021); // set GON=1, DTE=0, D[1:0]=01
    delay(100); // wait for 100ms
    WriteCommandData(0x07, 0x0023); // set GON=1, DTE=0, D[1:0]=11
    delay(100); // wait for 100ms
    WriteCommandData(0x07, 0x0033); // set GON=1, DTE=1, D[1:0]=11
}

void ILI9325D_328p::DisplayOFF(void)
{
    WriteCommandData(0x07, 0x0031); // set GON=1, DTE=1, D[1:0]=01
    delay(100); // wait for 100ms
    WriteCommandData(0x07, 0x0030); // set GON=1, DTE=1, D[1:0]=00
    delay(100); // wait for 100ms
    WriteCommandData(0x07, 0x0000); // set GON=0, DTE=0, D[1:0]=00
    WriteCommandData(0x10, 0x0000); // set SAP=0, AP[2:0]=000, PON=0
}


void ILI9325D_328p::begin(void)
{
    //----------------- Power On Reset -----------------//
    digitalWrite(RST, HIGH);
    delay(50);
    digitalWrite(RST, LOW);
    delay(50);
    digitalWrite(RST, HIGH);
    //--------------- Display OFF Setting ---------------//
    DisplayOFF();
    //------ Register Setting Before Power Supply ------//
    WriteCommandData(0x01, 0x0100); // set Driver Output Control
    WriteCommandData(0x02, 0x0200); // set 1 line inversion
    WriteCommandData(0x03, 0x1030); // set GRAM write direction and BGR=1
    WriteCommandData(0x04, 0x0000); // Resize register
    WriteCommandData(0x08, 0x0207); // set the back porch and front porch
    WriteCommandData(0x09, 0x0000); // set non-display area refresh cycle ISC[3:0]
    WriteCommandData(0x0A, 0x0000); // FMARK function
    WriteCommandData(0x0C, 0x0000); // RGB interface setting
    WriteCommandData(0x0D, 0x0000); // Frame marker Position
    WriteCommandData(0x0F, 0x0000); // RGB interface polarity
    //---------- Power Supply Initial Setting ----------//
    // set VC[2:0], VRH[3:0], VCM[5:0], VDV[4:0], BT[2:0]
    WriteCommandData(0x10, 0x0000); // BT[2:0]=000
    WriteCommandData(0x11, 0x0007); // VC[2:0]=111, DC0[2:0]=DC1[2:0]=000
    WriteCommandData(0x12, 0x0000); // VCIRE=0, VRH[3:0]=0000
    WriteCommandData(0x13, 0x0000); // VDV[4:0]=00000
    WriteCommandData(0x29, 0x000A); // VCM[5:0]=001010
    delay(200); // Dis-charge capacitor power voltage and for stabilizing
    //--------- Power Supply Operation Setting ---------//
    // set BT[2:0], DC1[2:0], DC0[2:0], AP[2:0], APE
    WriteCommandData(0x10, 0x1690); // SAP=1, BT[2:0]=110, APE=1, AP[2:0]=001
    WriteCommandData(0x11, 0x0227); // DC1[2:0]=DC0[2:0]=010, DC0[2:0]
    delay(50); // Delay 50ms
    WriteCommandData(0x12, 0x000D); // VCIRE=0, VRH[3:0]=1101
    delay(50); // Delay 50ms
    WriteCommandData(0x13, 0x1300); // VDV[4:0]=10011
    WriteCommandData(0x2B, 0x000D); // FRS[3:0]=1101, Set Frame Rate to 83
    //------------- Adjust the Gamma Curve -------------//
    WriteCommandData(0x30, 0x0000);
    WriteCommandData(0x31, 0x0404);
    WriteCommandData(0x32, 0x0003);
    WriteCommandData(0x35, 0x0405);
    WriteCommandData(0x36, 0x0808);
    WriteCommandData(0x37, 0x0407);
    WriteCommandData(0x38, 0x0303);
    WriteCommandData(0x39, 0x0707);
    WriteCommandData(0x3C, 0x0504);
    WriteCommandData(0x3D, 0x0808);
    //------------------ Set GRAM area ------------------//
    WriteCommandData(0x50, 0x0000); // Horizontal GRAM Start Address
    WriteCommandData(0x51, 0x00EF); // Horizontal GRAM End Address
    WriteCommandData(0x52, 0x0000); // Vertical GRAM Start Address
    WriteCommandData(0x53, 0x013F); // Vertical GRAM Start Address
    WriteCommandData(0x60, 0xA700); // Gate Scan Line
    WriteCommandData(0x61, 0x0001); // NDL,VLE, REV
    WriteCommandData(0x6A, 0x0000); // set scrolling line
    //------------- Partial Display Control -------------//
    WriteCommandData(0x80, 0x0000);
    WriteCommandData(0x81, 0x0000);
    WriteCommandData(0x82, 0x0000);
    WriteCommandData(0x83, 0x0000);
    WriteCommandData(0x84, 0x0000);
    WriteCommandData(0x85, 0x0000);
    //------------------ Panel Control ------------------//
    WriteCommandData(0x90, 0x0010);
    WriteCommandData(0x92, 0x0000);
    //--------------- Display ON Setting ---------------//
    DisplayON();
    WriteCommandData(0x07, 0x0133);
    //--------------- Write Data to Gram ---------------//
    WriteCommandData(0x20, 0x0000); // GRAM horizontal Address
    WriteCommandData(0x21, 0x0000); // GRAM Vertical Address
    digitalWrite(RS, LOW); // command select
    write8(0x22); // write data to R22h and AC will increase automatically
    digitalWrite(RS, HIGH); // data select
    write16(0xfefe); write16(0x0000); write16(0xfefe); write16(0x0000);
    write16(0xfefe); write16(0x0000); write16(0xfefe); write16(0x0000);
    write16(0xfefe); write16(0x0000); write16(0xfefe); write16(0x0000);
    write16(0xfefe); write16(0x0000); write16(0xfefe); write16(0x0000);
    write16(0xfefe);
    //---------------------- Done ----------------------//

    m_color[0] = Black; // black background
    m_color[1] = White; // white foreground
    clearWindow(0, 0, m_width, m_height);
}

void ILI9325D_328p::setXY(uint16_t x, uint16_t y)
{
    // GRAM address set
    WriteCommandData(0x20, x > m_width ? 0 : x);
    WriteCommandData(0x21, y > m_height ? 0 : y);
}

void ILI9325D_328p::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    x1 = x1 > 0x00EF ? 0x00EF : x1;
    y1 = y1 > 0x013F ? 0x013F : y1;
    x0 = x0 < x1     ? x0     : 0;
    y0 = y0 < y1     ? y0     : 0;

    // window address set
    WriteCommandData(0x50, x0);
    WriteCommandData(0x51, x1);
    WriteCommandData(0x52, y0);
    WriteCommandData(0x53, y1);

    // set curser point
    setXY(x0, y0);
}

void ILI9325D_328p::clearPixels(uint32_t pixels)
{
    digitalWrite(RS, LOW); // command select
    write8(0x22); // write GRAM 8 bit command
    digitalWrite(RS, HIGH); // data select
    while (pixels--)
    {
        write16(0x0000); // 16 bit per pixel
    }
}

void ILI9325D_328p::clearWindow(uint16_t x, uint16_t y,
                                uint16_t width, uint16_t height)
{
    if (x == y == width == height == -1) {width = m_width; height = m_height;}
    uint32_t pixels = (uint32_t)width * height;
    setWindow(x, y, x + width , y + height);
    clearPixels(pixels);
}

void ILI9325D_328p::clearLine(uint16_t line, bool vertical)
{
    if (vertical)
        clearWindow(line, 0, 1, m_height);
    else
        clearWindow(0, line, m_width, 1);
}

void ILI9325D_328p::drawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    setXY(x, y);
    digitalWrite(RS, LOW); // command select
    write8(0x22); // write GRAM
    digitalWrite(RS, HIGH); // data select
    write16(color);
}


/*
size_t LCD_ILI9325D::write(uint8_t c)
{
    if (c == '\n') {
        m_x += (m_font + 1) << 3;
        return 0;
    } else if (c == '\r') {
        setXY(m_x, m_x + 7, m_y, 319);
        clearPixels((320 - m_y) * 8);
        m_y = 0;
        return 0;
    }

    if (m_font == FONT_SIZE_SMALL) {
        setXY(m_x, m_x + 7, m_y, m_y + 4);
        m_y += 6;
        if (m_y >= 320) {
            m_x += (m_font + 1) << 3;
            m_y = 0;
            if (m_x >= 240) {
                m_x = 0;
            }
        }
        if (c > 0x20 && c < 0x7f) {
            byte pgm_buffer[5];
            memcpy_P(pgm_buffer, &font5x8[c - 0x21], 5);
            byte i = 4;
            do {
                unsigned char d = pgm_buffer[i];
                for (byte j = 0; j < 8; j++, d >>= 1) {
                    write(m_color[d & 1]);
                }
            } while (i--);
        } else {
            clearPixels(5 * 8);
        }
    } else {
        setXY(m_x, m_x + 15, m_y, m_y + 7);
        m_y += 9;
        if (m_y >= 320) {
            m_x += (m_font + 1) << 3;
            m_y = 0;
            if (m_x >= 240) {
                m_x = 0;
            }
        }
        if (c > 0x20 && c < 0x7f) {
            byte pgm_buffer[16];
            memcpy_P(pgm_buffer, &font8x16_terminal[c - 0x21], 16);
            for (byte i = 0; i < 16; i += 2) {
                unsigned char d = pgm_buffer[14 - i];
                for (byte j = 0; j < 8; j++, d >>= 1) {
                    write(m_color[d & 1]);
                }
                d = pgm_buffer[15 - i];
                for (byte j = 0; j < 8; j++, d >>= 1) {
                    write(m_color[d & 1]);
                }
            }
        } else {
            clearPixels(8 * 16);
        }
    }
}

void LCD_ILI9325D::writeDigit(byte n)
{
    if (m_font == FONT_SIZE_LARGE) {
        setXY(m_x, m_x + 15, m_y, m_y + 15);
        m_y += 16;
        if (n <= 9) {
            byte pgm_buffer[32];
            memcpy_P(pgm_buffer, &digits16x16[n], sizeof(pgm_buffer));
            for (byte i = 0; i < 16; i++) {
                unsigned char d = pgm_buffer[15 - i];
                for (byte j = 0; j < 8; j++, d >>= 1) {
                    write(m_color[d & 1]);
                }
                d = pgm_buffer[31 - i];
                for (byte j = 0; j < 8; j++, d >>= 1) {
                    write(m_color[d & 1]);
                }
            }
        } else {
            clearPixels(16 * 16);
        }
    } else if (m_font == FONT_SIZE_XLARGE) {
        setXY(m_x, m_x + 23, m_y, m_y + 15);
        m_y += 18;
        if (n <= 9) {
            byte pgm_buffer[48];
            memcpy_P(pgm_buffer, &digits16x24[n], sizeof(pgm_buffer));
            for (int i = 0; i < 48; i += 3) {
                unsigned char d = pgm_buffer[45 - i];
                for (int j = 0; j < 8; j++, d >>= 1) {
                    write(m_color[d & 1]);
                }
                d = pgm_buffer[46 - i];
                for (int j = 0; j < 8; j++, d >>= 1) {
                    write(m_color[d & 1]);
                }
                d = pgm_buffer[47 - i];
                for (int j = 0; j < 8; j++, d >>= 1) {
                    write(m_color[d & 1]);
                }
            }
        } else {
            clearPixels(16 * 24);
        }
    } else {
        write(n <= 9 ? ('0' + n) : ' ');
    }
}

void LCD_ILI9325D::draw(const PROGMEM byte* buffer, uint16_t width, uint16_t height)
{
    byte rows = height >> 3;
    setXY(m_x, m_x + height - 1, m_y, m_y + width - 1);
    uint16_t i = width - 1;
    do {
        for (uint8_t h = 0; h < rows; h++) {
            byte d = pgm_read_byte_far(buffer + i + width * h);
            for (byte j = 0; j < 8; j++, d >>= 1) {
                write(m_color[d & 1]);
            }
        }
    } while (i--);
    m_y += width;
}

void LCD_ILI9325D::draw(const PROGMEM byte* buffer, uint16_t width, uint16_t height, byte scaleX, byte scaleY)
{
    byte rows = height >> 3;
    if (scaleY == 0) scaleY = scaleX;
    setXY(m_x, m_x + height - 1, m_y, m_y + width - 1);
    uint16_t i = width - 1;
    do {
        for (byte n = 0; n < scaleX; n++) {
            for (uint8_t h = 0; h < rows; h++) {
                byte d = pgm_read_byte_far(buffer + i + width * h);
                for (byte j = 0; j < 8; j++, d >>= 1) {
                    for (byte m = 0; m < scaleY; m++) {
                        write(m_color[d & 1]);
                    }
                }
            }
        }
    } while (i--);
    m_y += width * scaleX;
}

void LCD_ILI9325D::draw4bpp(const PROGMEM byte* buffer, uint16_t width, uint16_t height)
{
    char buf[240];
    setXY(m_x, m_x + height * 2 - 1, m_y, m_y + width * 2- 1);
    uint16_t i = width - 1;
    do {
        memcpy_P(buf, buffer + i * height * 2, height * 2);
        for (byte j = 0; j < height * 2; j += 2) {
            write(buf[j], buf[j + 1]);
            write(buf[j], buf[j + 1]);
        }
        for (byte j = 0; j < height * 2; j += 2) {
            write(buf[j], buf[j + 1]);
            write(buf[j], buf[j + 1]);
        }
    } while (i--);
    m_y += width * 2;
}

*/
