#include "Motors_Speed.h"

void MotorsSpeed::setSpeedA(int StickLeftV_value) {
    SpeedA = StickLeftV_value;
}

void MotorsSpeed::setSpeedsLR(int StickLeftH_value, int StickRightV_value) {
    if (StickRightV_value > 5 && StickRightV_value <= 45) { // Slowly moving forward
        if (StickLeftH_value < -5) { // Turning left
            StickLeftH_value = -StickLeftH_value;
            SpeedL = (StickRightV_value - 2 * StickLeftH_value) / 3;
            SpeedR = (StickRightV_value + 2 * StickLeftH_value) / 3;
        } else if (StickLeftH_value > 5) { // Turning right
            SpeedL = (StickRightV_value + 2 * StickLeftH_value) / 3;
            SpeedR = (StickRightV_value - 2 * StickLeftH_value) / 3;
        } else { // No turning (dead zone)
            SpeedL = StickRightV_value;
            SpeedR = StickRightV_value;
        }
    } else if (StickRightV_value > 45) { // Fast moving forward
        if (StickLeftH_value < -5) { // Turning left
            StickLeftH_value = -StickLeftH_value;
            SpeedL = (2 * StickRightV_value - StickLeftH_value) / 3;
            SpeedR = (2 * StickRightV_value + StickLeftH_value) / 3;
        } else if (StickLeftH_value > 5) { // Turning right
            SpeedL = (2 * StickRightV_value + StickLeftH_value) / 3;
            SpeedR = (2 * StickRightV_value - StickLeftH_value) / 3;
        } else { // No turning (dead zone)
            SpeedL = StickRightV_value;
            SpeedR = StickRightV_value;
        }
    } else if (StickRightV_value < -5 && StickRightV_value >= -45) { // Slowly moving backward
        if (StickLeftH_value < -5) { // Turning left
            SpeedL = (StickRightV_value - 2 * StickLeftH_value) / 3;
            SpeedR = (StickRightV_value + 2 * StickLeftH_value) / 3;
        } else if (StickLeftH_value > 5) { // Turning right
            StickLeftH_value = -StickLeftH_value;
            SpeedL = (StickRightV_value + 2 * StickLeftH_value) / 3;
            SpeedR = (StickRightV_value - 2 * StickLeftH_value) / 3;
        } else { // No turning (dead zone)
            SpeedL = StickRightV_value;
            SpeedR = StickRightV_value;
        }
    } else if (StickRightV_value < -45) { // Fast moving backward
        if (StickLeftH_value < -5) { // Turning left
            SpeedL = (2 * StickRightV_value - StickLeftH_value) / 3;
            SpeedR = (2 * StickRightV_value + StickLeftH_value) / 3;
        } else if (StickLeftH_value > 5) { // Turning right
            StickLeftH_value = -StickLeftH_value;
            SpeedL = (2 * StickRightV_value + StickLeftH_value) / 3;
            SpeedR = (2 * StickRightV_value - StickLeftH_value) / 3;
        } else { // No turning (dead zone)
            SpeedL = StickRightV_value;
            SpeedR = StickRightV_value;
        }
    } else if (StickRightV_value >= -5 && StickRightV_value <= 5) { // No forward or backward movement
        if (StickLeftH_value < -5) { // Turning left
            StickLeftH_value = -StickLeftH_value;
            SpeedL = -StickLeftH_value;
            SpeedR = StickLeftH_value;
        } else if (StickLeftH_value > 5) { // Turning right
            SpeedL = StickLeftH_value;
            SpeedR = -StickLeftH_value;
        }
    }
}

void MotorsSpeed::setSlowMode() {
    // Limit the speeds if speed mode is set to LOW
    SpeedA /= 4;
    SpeedA *= 3;
    SpeedL /= 2;
    SpeedR /= 2;
}

void MotorsSpeed::mapSpeeds() {
    //Map the speeds of motors to the range of given in limits
    SpeedL = map(SpeedL, -90, 90, MIN_SPEED, MAX_SPEED);
    SpeedR = map(SpeedR, -90, 90, MIN_SPEED, MAX_SPEED);
    SpeedA = map(SpeedA, -90, 90, MIN_SPEED, MAX_SPEED);
}

void MotorsSpeed::setZeroSpeed(){
    SpeedA = 0;
    SpeedL = 0;
    SpeedR = 0;
}

void MotorsSpeed::InvertMotorAdirection(){
    SpeedA = 180 - SpeedA; // Invert the speed
}

void MotorsSpeed::InvertMotorLdirection(){
    SpeedL = 180 - SpeedL; // Invert the speed
}

void MotorsSpeed::InvertMotorRdirection(){
    SpeedR = 180 - SpeedR; // Invert the speed
}
