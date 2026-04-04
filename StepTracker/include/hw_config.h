#ifndef HW_CONFIG_H
#define HW_CONFIG_H

#include <Arduino.h>

struct BoardConfig {
  uint8_t redLedPin;
  uint8_t yellowLedPin;
  uint8_t greenLedPin;
  uint8_t STPin;
  uint8_t XPin;
  uint8_t YPin;
  uint8_t ZPin;
  uint8_t pushButtonAPin;
  uint8_t pushButtonBPin;
  uint8_t lcdSDAPin;
  uint8_t lcdSCLPin;
};

#endif