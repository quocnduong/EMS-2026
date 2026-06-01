#include "SelfTest.h"

SelfTest::SelfTest(const BoardConfig &config) : config_(config) {}

bool SelfTest::run(double supplyVoltage) {
    isStationary_ = false;
	//CHECK IF STATIONARY
    while (!isStationary_) {
        //READ AND STORE ACC VALUES
        oldX_ = analogRead(config_.xAdcPin);
        oldY_ = analogRead(config_.yAdcPin);
        oldZ_ = analogRead(config_.zAdcPin);
        //WAIT
        delay(100);
        //READ AND STORE NEW ACC VALUES
        newX_ = analogRead(config_.xAdcPin);
        newY_ = analogRead(config_.yAdcPin);
        newZ_ = analogRead(config_.zAdcPin);
        //COMPARE
        if (
            newX_ < (oldX_ * (1+tolerance_)) && newX_ > (oldX_ * (1-tolerance_)) &&
            newY_ < (oldY_ * (1+tolerance_)) && newY_ > (oldY_ * (1-tolerance_)) &&
            newZ_ < (oldZ_ * (1+tolerance_)) && newZ_ > (oldZ_ * (1-tolerance_))
        ) {isStationary_ = true;}
    }
    //READ AND STORE ACC VALUES
    oldX_ = analogRead(config_.xAdcPin);
    oldY_ = analogRead(config_.yAdcPin);
    oldZ_ = analogRead(config_.zAdcPin);
    //SET ST TO HIGH
    digitalWrite(config_.stPin, HIGH);
    delay(10); //not sure if necessary
    //READ AND STORE NEW ACC VALUES
    newX_ = analogRead(config_.xAdcPin);
    newY_ = analogRead(config_.yAdcPin);
    newZ_ = analogRead(config_.zAdcPin);
    //COMPARE OLD AND NEW ACC VALUES
    // ADXL335 self-test response is specified at 3.0 V and scales roughly with VS^3.
    const double supplyScale = supplyVoltage / 3.0;
    const double selfTestScale = supplyScale * supplyScale * supplyScale;
    const double expectedXDeltaMv = 325.0 * selfTestScale;
    const double expectedYDeltaMv = 325.0 * selfTestScale;
    const double expectedZDeltaMv = 550.0 * selfTestScale;
    const double expectedXDelta = (expectedXDeltaMv / adcReferenceVoltage_) * 4095.0;
    const double expectedYDelta = (expectedYDeltaMv / adcReferenceVoltage_) * 4095.0;
    const double expectedZDelta = (expectedZDeltaMv / adcReferenceVoltage_) * 4095.0;
    if (
        (oldX_ - newX_) > (expectedXDelta * (1-tolerance_)) &&
        (newY_ - oldY_) > (expectedYDelta * (1-tolerance_)) &&
        (newZ_ - oldZ_) > (expectedZDelta * (1-tolerance_))
    ) {return(true);}
    else {return(false);}
}

