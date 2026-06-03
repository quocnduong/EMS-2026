#include "StepCounter.h"
#include <math.h>

StepCounter::StepCounter() {
    reset();
}

void StepCounter::reset() {
    stepCount_ = 0;
    filteredMovement_ = 0.0;
    aboveThreshold_ = false;
    lastStepTime_ = 0;
}

double StepCounter::calculateMovementIntensity(const AccelData &accel) {
    double magnitude = sqrt(
        accel.xG * accel.xG +
        accel.yG * accel.yG +
        accel.zG * accel.zG
    );

    double movement = fabs(magnitude - 1.0);

    return movement;
}

bool StepCounter::update(const AccelData &accel, unsigned long nowMs) {
    double movement = calculateMovementIntensity(accel);

    filteredMovement_ =
        alpha_ * movement +
        (1.0 - alpha_) * filteredMovement_;

    bool stepDetected = false;

    if (!aboveThreshold_ &&
        filteredMovement_ > stepThreshold_ &&
        nowMs - lastStepTime_ > minStepIntervalMs_) {

        stepCount_++;
        lastStepTime_ = nowMs;
        aboveThreshold_ = true;
        stepDetected = true;
    }

    if (filteredMovement_ < resetThreshold_) {
        aboveThreshold_ = false;
    }

    return stepDetected;
}

int StepCounter::getStepCount() const {
    return stepCount_;
}

double StepCounter::getMovementIntensity() const {
    return filteredMovement_;
}

double StepCounter::getFilteredMovement() const {
    return filteredMovement_;
}