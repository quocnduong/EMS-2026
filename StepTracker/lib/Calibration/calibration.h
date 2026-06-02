#ifndef CALIBRATION_H
#define CALIBRATION_H
#include <Arduino.h>
#include "hw_config.h"

// Per-axis calibration constants derived from the six-position method
//   offset      : ADC counts the axis reads at 0 g (the zero-g bias)
//   sensitivity : ADC counts per 1 g
// Convert any raw reading to g with: g = (raw - offset) / sensitivity
struct AxisCalibration {
    double offset = 0.0;
    double sensitivity = 0.0;
};

struct CalibrationResult {
    AxisCalibration x;
    AxisCalibration y;
    AxisCalibration z;
    bool valid = false; // true only when all six orientations were captured
};

// The board orientation being held while a sample is taken
// "Up" means the axis points opposite to gravity (reads +1 g),
// "Down" means it points along gravity (reads -1 g)
enum class Orientation {
    XUp, XDown,
    YUp, YDown,
    ZUp, ZDown
};

class Calibration {
private:
    BoardConfig config_;

    uint8_t  samples_       = 64; // readings averaged per capture (noise reduction)
    uint16_t sampleDelayMs_ = 5;  // delay between averaged samples

    // Captured per-axis ±1 g readings and their "have we got it yet" flags
    double xPos_ = 0.0, xNeg_ = 0.0;
    double yPos_ = 0.0, yNeg_ = 0.0;
    double zPos_ = 0.0, zNeg_ = 0.0;
    bool haveXPos_ = false, haveXNeg_ = false;
    bool haveYPos_ = false, haveYNeg_ = false;
    bool haveZPos_ = false, haveZNeg_ = false;

    // Plausibility band used by isPlausible(): how far the measured
    // sensitivity/offset may stray from the datasheet expectation.
    double plausibilityTolerance_ = 0.30;

    static constexpr double adcReferenceVoltage_ = 3.3;
    static constexpr double adcMaxCount_         = 4095.0; // 12-bit ADC

    double averageRead(uint8_t pin) const;
    void   promptCapture(Stream &io, Orientation o, const __FlashStringHelper *msg);

public:
    explicit Calibration(const BoardConfig &config);

    // Capture one orientation. Call all six (in any order), then compute().
    void capture(Orientation o);

    // Build the calibration from whatever has been captured so far.
    // result.valid is false if any orientation is missing or an axis
    // produced a non-positive sensitivity (e.g. up/down swapped)
    CalibrationResult compute() const;

    // Optional sanity check against the ADXL335 datasheet, scaled for the
    // actual supply voltage. Independent of compute(); use it to catch a
    // miswired axis or a bad rig
    bool isPlausible(const CalibrationResult &r, double supplyVoltage = 3.3) const;

    // Serial-driven flow: prompts for each of the six orientations, waiting
    // for the user to send a newline before sampling. Returns compute()
    CalibrationResult runInteractive(Stream &io = Serial);

    // Print the constants so they can be copied into firmware
    void printResult(const CalibrationResult &r, Stream &io = Serial) const;

    // Apply a calibration to a raw ADC reading -> acceleration in g
    static double toG(double raw, const AxisCalibration &c);

    // Optional knobs
    void setSamples(uint8_t n)            { samples_ = (n == 0) ? 1 : n; }
    void setSampleDelay(uint16_t ms)      { sampleDelayMs_ = ms; }
    void setPlausibilityTolerance(double t) { plausibilityTolerance_ = t; }
};
#endif
