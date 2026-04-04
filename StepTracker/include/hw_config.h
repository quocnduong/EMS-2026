#ifndef HW_CONFIG_H
#define HW_CONFIG_H

#include <Arduino.h>

struct BoardConfig {
  uint8_t redLedPin;
  uint8_t yellowLedPin;
  uint8_t greenLedPin;
  uint8_t stPin;
  uint8_t xAdcPin;
  uint8_t yAdcPin;
  uint8_t zAdcPin;
  uint8_t pushButtonAPin;
  uint8_t pushButtonBPin;
  uint8_t lcdSDAPin;
  uint8_t lcdSCLPin;
};

void initializeBoardPins(const BoardConfig &config);

#endif