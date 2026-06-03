#ifndef STEP_COUNTER_H
#define STEP_COUNTER_H

#include <Arduino.h>

struct AccelData {
    double xG;
    double yG;
    double zG;
};

class StepCounter {
public:
    StepCounter();

    void reset();

    bool update(const AccelData &accel, unsigned long nowMs);

    int getStepCount() const;
    double getMovementIntensity() const;
    double getFilteredMovement() const;

private:
    double calculateMovementIntensity(const AccelData &accel);

    int stepCount_;

    double filteredMovement_;
    bool aboveThreshold_;

    unsigned long lastStepTime_;

    const double alpha_ = 0.40;

    const double stepThreshold_ = 0.20;
    const double resetThreshold_ = 0.10;

    const unsigned long minStepIntervalMs_ = 300;
};

#endif