/*************************************************************************
modified ILI9325D API for atmega328p(Arduino Nano/Uno) and WiringPi on ARM
@author Hank
@page https://github.com/hankso
*************************************************************************/

#if defined(__ARM__)
#include <WiringPi.h>
#elif defined(__ARDUINO__)
#include <Arduino.h>
#endif

#include "ILI9325D.h"

ILI9325D::ILI9325D(
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

void ILI9325D::write8(uint8_t data)
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

void ILI9325D::write16(uint16_t data)
{
    write8( *((uint8_t *)&data + 1) );
    write8( (uint8_t)data );
}

void ILI9325D::WriteCommandData(uint8_t cmd, uint16_t data)
{
    digitalWrite(RS, LOW); // command select
    write8(cmd);
    digitalWrite(RS, HIGH); // data select
    write16(data);
}

void ILI9325D::DisplayON(void)
{
    WriteCommandData(0x07, 0x0001); // set GON=0, DTE=0, D[1:0]=01
    delay(100); // wait for 100ms
    WriteCommandData(0x07, 0x0021); // set GON=1, DTE=0, D[1:0]=01
    delay(100); // wait for 100ms
    WriteCommandData(0x07, 0x0023); // set GON=1, DTE=0, D[1:0]=11
    delay(100); // wait for 100ms
    WriteCommandData(0x07, 0x0033); // set GON=1, DTE=1, D[1:0]=11
}

void ILI9325D::DisplayOFF(void)
{
    WriteCommandData(0x07, 0x0031); // set GON=1, DTE=1, D[1:0]=01
    delay(100); // wait for 100ms
    WriteCommandData(0x07, 0x0030); // set GON=1, DTE=1, D[1:0]=00
    delay(100); // wait for 100ms
    WriteCommandData(0x07, 0x0000); // set GON=0, DTE=0, D[1:0]=00
    WriteCommandData(0x10, 0x0000); // set SAP=0, AP[2:0]=000, PON=0
}


void ILI9325D::begin(void)
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

void ILI9325D::setXY(uint16_t x, uint16_t y)
{
    // GRAM address set
    WriteCommandData(0x20, x > m_width ? 0 : x);
    WriteCommandData(0x21, y > m_height ? 0 : y);
}

void ILI9325D::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
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

void ILI9325D::clearPixels(uint32_t pixels)
{
    digitalWrite(RS, LOW); // command select
    write8(0x22); // write GRAM 8 bit command
    digitalWrite(RS, HIGH); // data select
    while (pixels--)
    {
        write16(0x0000); // 16 bit per pixel
    }
}

void ILI9325D::clearWindow(uint16_t x, uint16_t y,
                                uint16_t width, uint16_t height)
{
    if (x == y == width == height == -1) {width = m_width; height = m_height;}
    uint32_t pixels = (uint32_t)width * height;
    setWindow(x, y, x + width , y + height);
    clearPixels(pixels);
}

void ILI9325D::clearLine(uint16_t line, bool vertical)
{
    if (vertical)
        clearWindow(line, 0, 1, m_height);
    else
        clearWindow(0, line, m_width, 1);
}

void ILI9325D::drawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    setXY(x, y);
    digitalWrite(RS, LOW); // command select
    write8(0x22); // write GRAM
    digitalWrite(RS, HIGH); // data select
    write16(color);
}

