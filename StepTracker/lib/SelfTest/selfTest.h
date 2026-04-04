#ifndef SELFTEST_H
#define SELFTEST_H

#include <Arduino.h>
#include "hw_config.h"

class selfTest {
private:
	BoardConfig config_;

    double oldX;
    double oldY;
    double oldZ;

    double newX;
    double newY;
    double newX;

public:
	explicit selfTest(const BoardConfig &config);
	void run();
};

#endif