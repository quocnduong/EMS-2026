#include "SelfTest.h"

SelfTest::SelfTest(const BoardConfig &config) : config_(config) {}

bool SelfTest::run() {
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
    if (
        newX_ < (oldX_ * (1+tolerance_)) && newX_ > (oldX_ * (1-tolerance_)) &&
        newY_ < (oldY_ * (1+tolerance_)) && newY_ > (oldY_ * (1-tolerance_)) &&
        newZ_ < (oldZ_ * (1+tolerance_)) && newZ_ > (oldZ_ * (1-tolerance_))
    ) {return(true);}
    else {return(false);}
}

