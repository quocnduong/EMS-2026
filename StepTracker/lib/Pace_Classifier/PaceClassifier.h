#ifndef PACE_CLASSIFIER_H
#define PACE_CLASSIFIER_H

#include <Arduino.h>

enum class PaceType {
    UNKNOWN,
    STATIONARY,
    WALKING,
    RUNNING
};

class PaceClassifier {
public:
    PaceClassifier();

    void reset(unsigned long nowMs);

    void onStep();
    void update(unsigned long nowMs);

    double getCadence() const;
    int getStepsInWindow() const;
    PaceType getPaceType() const;

    const char* getPaceString() const;

private:
    int stepsInWindow_;
    unsigned long windowStartMs_;
    

    double cadence_;
    PaceType paceType_;

    const unsigned long windowMs_ = 1000;

    const double stationaryCadenceThreshold_ = 10.0;
    const double runningCadenceThreshold_ = 120.0;
};

#endif