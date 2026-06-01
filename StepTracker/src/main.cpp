#include <Arduino.h>
#include "hw_config.h"
#include "SelfTest.h"

BoardConfig boardConfig{
  32, // redLedPin
  33, // yellowLedPin
  25, // greenLedPin
  5,  // stPin
  26, // xAdcPin
  27, // yAdcPin
  14, // zAdcPin
  18, // pushButtonAPin
  19, // pushButtonBPin
  21, // lcdSDAPin
  22  // lcdSCLPin
};

SelfTest selfTest(boardConfig);

void setup() {
  initializeBoardPins(boardConfig);
  selfTest.run(); //add voltage
}

void loop() {
  
}