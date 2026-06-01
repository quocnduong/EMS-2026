#ifndef SELFTEST_H
#define SELFTEST_H

#include <Arduino.h>
#include "hw_config.h"

class SelfTest {
private:
	BoardConfig config_;

    double oldX_;
    double oldY_;
    double oldZ_;

    double newX_;
    double newY_;
    double newZ_;

    bool isStationary_;

    double tolerance_ = 0.1;
    static constexpr double adcReferenceVoltage_ = 3.3;

public:
    explicit SelfTest(const BoardConfig &config);
	bool run(double supplyVoltage = 3.3);
};

#endif