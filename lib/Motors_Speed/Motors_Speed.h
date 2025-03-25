#ifndef MOTORS_SPEED_H
#define MOTORS_SPEED_H

#include <Arduino.h>

extern bool speed_mode;

//Motor speed limits
#define MAX_SPEED 140
#define MIN_SPEED 40

class MotorsSpeed {
private:
    int SpeedA; // Speed for vertical movement
    int SpeedL; // Speed for the left motor
    int SpeedR; // Speed for the right motor

public:
    // Constructor
    MotorsSpeed() : SpeedA(0), SpeedL(0), SpeedR(0) {}

    // Destructor
    ~MotorsSpeed() {}

    // Method to set SpeedA based on StickLeftV_value
    void setSpeedA(int StickLeftV_value);

    //Method to set SpeedL and SpeedR based on StickLeftH_value and StickRightV_value
    void setSpeedsLR(int StickLeftH_value, int StickRightV_value);

    //Method to set 0 speed to all motors
    void setZeroSpeed();

    //Method to set slow mode
    void setSlowMode();

    //Method to map speeds to motor range
    void mapSpeeds();

    //For future  development if needed
    int getSpeedA() const { return SpeedA; }
    int getSpeedL() const { return SpeedL; }
    int getSpeedR() const { return SpeedR; }
};

#endif