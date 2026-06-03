#include "PaceClassifier.h"

PaceClassifier::PaceClassifier() {
    stepsInWindow_ = 0;
    windowStartMs_ = 0;
    cadence_ = 0.0;
    paceType_ = PaceType::UNKNOWN;
}

void PaceClassifier::reset(unsigned long nowMs) {
    stepsInWindow_ = 0;
    windowStartMs_ = nowMs;
    cadence_ = 0.0;
    paceType_ = PaceType::STATIONARY;
}

void PaceClassifier::onStep() {
    stepsInWindow_++;
}

void PaceClassifier::update(unsigned long nowMs) {
    unsigned long elapsed = nowMs - windowStartMs_;

    if (elapsed < windowMs_) {
        return;
    }

    cadence_ = stepsInWindow_ * 60000.0 / elapsed;

    if (cadence_ < stationaryCadenceThreshold_) {
        paceType_ = PaceType::STATIONARY;
    }
    else if (cadence_ < runningCadenceThreshold_) {
        paceType_ = PaceType::WALKING;
    }
    else {
        paceType_ = PaceType::RUNNING;
    }

    stepsInWindow_ = 0;
    windowStartMs_ = nowMs;
}

double PaceClassifier::getCadence() const {
    return cadence_;
}

int PaceClassifier::getStepsInWindow() const {
    return stepsInWindow_;
}

PaceType PaceClassifier::getPaceType() const {
    return paceType_;
}

const char* PaceClassifier::getPaceString() const {
    switch (paceType_) {
        case PaceType::STATIONARY:
            return "STATIONARY";

        case PaceType::WALKING:
            return "WALKING";

        case PaceType::RUNNING:
            return "RUNNING";

        default:
            return "UNKNOWN";
    }
}