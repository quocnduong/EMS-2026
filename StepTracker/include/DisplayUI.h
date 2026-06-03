#ifndef DISPLAY_UI_H
#define DISPLAY_UI_H

#include <Arduino.h>
#include "PaceClassifier.h"

class DisplayUI {
public:
    DisplayUI(uint8_t sdaPin, uint8_t sclPin, uint8_t lcdAddress = 0x27);

    void begin();
    void showStartup();
    void showSelfTest(bool passed);
    void showCalibration(bool passed);

    void update(
        int totalSteps,
        double cadence,
        double stepsPerSecond,
        PaceType paceType,
        bool stepDetected
    );

private:
    uint8_t sdaPin_;
    uint8_t sclPin_;
    uint8_t lcdAddress_;
    const char* paceToShortString(PaceType paceType) const;

    unsigned long lastUpdateMs_;
    const unsigned long lcdUpdateIntervalMs_ = 500;

    const char* shortPaceString(PaceType paceType) const;
};

#endif