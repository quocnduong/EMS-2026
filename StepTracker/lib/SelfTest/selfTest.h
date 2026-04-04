#ifndef SELFTEST_H
#define SELFTEST_H

#include <Arduino.h>
#include "hw_config.h"

class selfTest {
private:
	BoardConfig config_;

public:
	explicit selfTest(const BoardConfig &config);
	void begin();
	void run();
};

#endif