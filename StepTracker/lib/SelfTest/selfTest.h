#ifndef SELFTEST_H
#define SELFTEST_H

#include <Arduino.h>
#include "hw_config.h"

class selfTest {
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

public:
	explicit selfTest(const BoardConfig &config);
	void run();
};

#endif