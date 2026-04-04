#include "hw_config.h"

void configureBoardPins(const BoardConfig &config) {
    pinMode(config.redLedPin, OUTPUT);  
    pinMode(config.yellowLedPin, OUTPUT);
    pinMode(config.greenLedPin, OUTPUT);
    pinMode(config.STPin, OUTPUT);
  
    pinMode(config.XPin, INPUT);
    pinMode(config.YPin, INPUT);
    pinMode(config.ZPin, INPUT);

    pinMode(config.pushButtonAPin, INPUT); //has a hardware pulldown externally
    pinMode(config.pushButtonBPin, INPUT); //has a hardware pulldown externally

  // I2C pins are configured when Wire.begin(SDA, SCL) is called.

    digitalWrite(config.redLedPin, LOW);
    digitalWrite(config.yellowLedPin, LOW);
    digitalWrite(config.greenLedPin, LOW);
    digitalWrite(config.STPin, LOW);
}
