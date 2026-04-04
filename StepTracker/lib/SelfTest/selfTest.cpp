#include "selfTest.h"

selfTest::selfTest(const BoardConfig &config) : config_(config) {}

void selfTest::run() {
    isStationary_ = false;
	//CHECK IF STATIONARY
    while (!isStationary_) {
        //READ AND STORE ACC VALUES
        oldX_ = analogRead(config_.XPin);
        oldY_ = analogRead(config_.YPin);
        oldZ_ = analogRead(config_.ZPin);
        //WAIT
        delay(100);
        //READ AND STORE NEW ACC VALUES
        newX_ = analogRead(config_.XPin);
        newY_ = analogRead(config_.YPin);
        newZ_ = analogRead(config_.ZPin);
        //COMPARE
        if (
            newX_ < (oldX_ * (1+tolerance_)) && newX_ > (oldX_ * (1-tolerance_)) &&
            newY_ < (oldY_ * (1+tolerance_)) && newY_ > (oldY_ * (1-tolerance_)) &&
            newZ_ < (oldZ_ * (1+tolerance_)) && newZ_ > (oldZ_ * (1-tolerance_))
        ) {isStationary_ = true;}
    }
        
    //READ AND STORE ACC VALUES

    //SET ST TO HIGH

    //READ AND STORE NEW ACC VALUES

    //COMPARE OLD AND NEW ACC VALUES

}

