#include <Arduino.h>
#include "hw_config.h"
#include "selfTest.h"

BoardConfig boardConfig{
  35, // redLedPin
  32, // yellowLedPin
  33, // greenLedPin
  5,  // STPin
  25, // XPin
  26, // YPin
  27, // ZPin
  18, // pushButtonAPin
  19, // pushButtonBPin
  21, // lcdSDAPin
  22  // lcdSCLPin
};

void setup() {
  configureBoardPins(boardConfig);
}

void loop() {

}