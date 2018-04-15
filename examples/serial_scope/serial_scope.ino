#include <ILI9325D_328p.h>

#define RS 6
#define CS 7
#define RST 8
#define SDA 9
#define SCL 10

#define X_CONST (220 - 1)
#define Y_CONST (176 - 1)

ILI9325D_328p lcd = ILI9325D_328p(CS, RS, RST, SCL, SDA, X_CONST, Y_CONST);

void setup() {
    pinMode(13, OUTPUT);
    Serial.begin(115200);
    lcd.begin();
    lcd.setXY(0, 0);
    for (uint16_t i=0; i < 50; i++) { lcd.write16(Red); }
    lcd.drawPoint(10, 1, Red);
    lcd.drawPoint(10, 2, Red);
    lcd.drawPoint(10, 3, Red);
    lcd.drawPoint(10, 4, Red);
    lcd.drawPoint(10, 5, Red);
}

void loop() {
}
