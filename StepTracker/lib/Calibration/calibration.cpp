#include "Calibration.h"

Calibration::Calibration(const BoardConfig &config) : config_(config) {}

// Average several ADC reads to knock down sensor/quantisation noise.
double Calibration::averageRead(uint8_t pin) const {
    double sum = 0.0;
    for (uint8_t i = 0; i < samples_; ++i) {
        sum += analogRead(pin);
        delay(sampleDelayMs_);
    }
    return sum / samples_;
}

void Calibration::capture(Orientation o) {
    // Make sure self-test isn't biasing the reading while we calibrate.
    digitalWrite(config_.stPin, LOW);

    switch (o) {
        case Orientation::XUp:   xPos_ = averageRead(config_.xAdcPin); haveXPos_ = true; break;
        case Orientation::XDown: xNeg_ = averageRead(config_.xAdcPin); haveXNeg_ = true; break;
        case Orientation::YUp:   yPos_ = averageRead(config_.yAdcPin); haveYPos_ = true; break;
        case Orientation::YDown: yNeg_ = averageRead(config_.yAdcPin); haveYNeg_ = true; break;
        case Orientation::ZUp:   zPos_ = averageRead(config_.zAdcPin); haveZPos_ = true; break;
        case Orientation::ZDown: zNeg_ = averageRead(config_.zAdcPin); haveZNeg_ = true; break;
    }
}

CalibrationResult Calibration::compute() const {
    CalibrationResult r;

    const bool haveAll =
        haveXPos_ && haveXNeg_ &&
        haveYPos_ && haveYNeg_ &&
        haveZPos_ && haveZNeg_;
    if (!haveAll) {
        r.valid = false;
        return r;
    }

    // Six-position method: the +1 g and -1 g readings straddle the zero-g
    // bias, so their midpoint is the offset and their half-span is the
    // counts-per-g sensitivity.
    r.x.offset      = (xPos_ + xNeg_) / 2.0;
    r.x.sensitivity = (xPos_ - xNeg_) / 2.0;
    r.y.offset      = (yPos_ + yNeg_) / 2.0;
    r.y.sensitivity = (yPos_ - yNeg_) / 2.0;
    r.z.offset      = (zPos_ + zNeg_) / 2.0;
    r.z.sensitivity = (zPos_ - zNeg_) / 2.0;

    // A non-positive sensitivity means an axis' up/down captures were
    // swapped (or the wiring is wrong) -- don't hand back garbage.
    r.valid =
        r.x.sensitivity > 0.0 &&
        r.y.sensitivity > 0.0 &&
        r.z.sensitivity > 0.0;

    return r;
}

bool Calibration::isPlausible(const CalibrationResult &r, double supplyVoltage) const {
    if (!r.valid) return false;

    // ADXL335 output is ratiometric to VS: zero-g sits at VS/2 and
    // sensitivity is ~300 mV/g at VS = 3.0 V, scaling linearly with VS.
    const double supplyScale = supplyVoltage / 3.0;
    const double expectedSensMv = 300.0 * supplyScale;                  // mV per g
    const double expectedZeroMv = (supplyVoltage * 1000.0) / 2.0;       // mV at 0 g

    const double expectedSens =
        (expectedSensMv / (adcReferenceVoltage_ * 1000.0)) * adcMaxCount_;
    const double expectedZero =
        (expectedZeroMv / (adcReferenceVoltage_ * 1000.0)) * adcMaxCount_;

    const double sLo = expectedSens * (1.0 - plausibilityTolerance_);
    const double sHi = expectedSens * (1.0 + plausibilityTolerance_);
    const double oLo = expectedZero * (1.0 - plausibilityTolerance_);
    const double oHi = expectedZero * (1.0 + plausibilityTolerance_);

    auto inBand = [](double v, double lo, double hi) { return v >= lo && v <= hi; };

    return
        inBand(r.x.sensitivity, sLo, sHi) && inBand(r.x.offset, oLo, oHi) &&
        inBand(r.y.sensitivity, sLo, sHi) && inBand(r.y.offset, oLo, oHi) &&
        inBand(r.z.sensitivity, sLo, sHi) && inBand(r.z.offset, oLo, oHi);
}

void Calibration::promptCapture(Stream &io, Orientation o, const __FlashStringHelper *msg) {
    io.println(msg);
    io.println(F("  ...then send any character (Enter) to sample."));

    while (io.available()) io.read();      // flush stale input
    while (!io.available()) delay(10);     // wait for the user
    while (io.available()) io.read();      // consume what they sent

    capture(o);
    io.println(F("  captured."));
}

CalibrationResult Calibration::runInteractive(Stream &io) {
    io.println(F("=== ADXL335 6-position calibration ==="));
    io.println(F("Hold the board still in each requested orientation."));

    promptCapture(io, Orientation::XUp,   F("[1/6] X axis pointing UP (+1 g on X)"));
    promptCapture(io, Orientation::XDown, F("[2/6] X axis pointing DOWN (-1 g on X)"));
    promptCapture(io, Orientation::YUp,   F("[3/6] Y axis pointing UP (+1 g on Y)"));
    promptCapture(io, Orientation::YDown, F("[4/6] Y axis pointing DOWN (-1 g on Y)"));
    promptCapture(io, Orientation::ZUp,   F("[5/6] Z axis pointing UP (+1 g on Z)"));
    promptCapture(io, Orientation::ZDown, F("[6/6] Z axis pointing DOWN (-1 g on Z)"));

    CalibrationResult r = compute();
    printResult(r, io);
    return r;
}

void Calibration::printResult(const CalibrationResult &r, Stream &io) const {
    if (!r.valid) {
        io.println(F("Calibration INVALID (missing positions or bad sensitivity)."));
        return;
    }
    io.println(F("--- Calibration result (ADC counts) ---"));
    io.print(F("X  offset=")); io.print(r.x.offset, 2);
    io.print(F("  sens="));    io.println(r.x.sensitivity, 2);
    io.print(F("Y  offset=")); io.print(r.y.offset, 2);
    io.print(F("  sens="));    io.println(r.y.sensitivity, 2);
    io.print(F("Z  offset=")); io.print(r.z.offset, 2);
    io.print(F("  sens="));    io.println(r.z.sensitivity, 2);
}

double Calibration::toG(double raw, const AxisCalibration &c) {
    if (c.sensitivity == 0.0) return 0.0; // guard against uninitialised calibration
    return (raw - c.offset) / c.sensitivity;
}
