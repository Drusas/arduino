#include "LcdTask.h"
#include <LiquidCrystal_I2C.h>

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  
bool useFirstRow = false;

LcdTask::LcdTask() {}

void LcdTask::configure(int interval, queue<string> *q) {
    setEnabled(false);
    updateInterval = interval;
    statusQueue = q;
    useFirstRow = true;
}

void LcdTask::start() {
    lcd.init();
    lcd.backlight();
}

void LcdTask::performUpdate() {
    if (!statusQueue->empty()) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(statusQueue->front().c_str());
        statusQueue->pop();
    }

}

void LcdTask::helloWorld() {
    uint8_t row = useFirstRow ? 0 : 1;
    useFirstRow = !useFirstRow;
    lcd.clear();
    lcd.setCursor(0, row);
    lcd.print("Hello, World!");
}