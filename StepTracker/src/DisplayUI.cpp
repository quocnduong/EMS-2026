#include "DisplayUI.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>


static LiquidCrystal_I2C lcd(0x27, 8, 2);

DisplayUI::DisplayUI(uint8_t sdaPin, uint8_t sclPin, uint8_t lcdAddress)
    : sdaPin_(sdaPin),
      sclPin_(sclPin),
      lcdAddress_(lcdAddress),
      lastUpdateMs_(0) {
}

void DisplayUI::begin() {
    Wire.begin(sdaPin_, sclPin_);

    lcd.init(); 
    lcd.backlight();

    lcd.clear();
}

void DisplayUI::showStartup() {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("StpTrkr");

    lcd.setCursor(0, 1);
    lcd.print("Start");
}

void DisplayUI::showSelfTest(bool passed) {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Self Test");

    lcd.setCursor(0, 1);
    if (passed) {
        lcd.print("PASS");
    } else {
        lcd.print("FAIL");
    }
}

void DisplayUI::showCalibration(bool passed) {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Calib");

    lcd.setCursor(0, 1);
    if (passed) {
        lcd.print("PASS");
    } else {
        lcd.print("FAIL");
    }
}

const char* DisplayUI::paceToShortString(PaceType paceType) const {
    switch (paceType) {
        case PaceType::STATIONARY:
            return "Still";
        case PaceType::WALKING:
            return "Walk";
        case PaceType::RUNNING:
            return "Run";
        default:
            return "Unknown";
    }
}

void DisplayUI::update(
    int totalSteps,
    double stepsPerMinute,
    double stepsPerSecond,
    PaceType paceType,
    bool stepDetected
) {
    unsigned long now = millis();

    if (now - lastUpdateMs_ < lcdUpdateIntervalMs_) {
        return;
    }

    lastUpdateMs_ = now;

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(paceToShortString(paceType));

    if (stepDetected) {
        lcd.print(" *");
    }

    lcd.setCursor(0, 1);
    lcd.print("S:");
    lcd.print(totalSteps);

    lcd.print(" M:");
    lcd.print(stepsPerMinute, 0);
}