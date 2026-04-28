#include "CPS3_drone.h"
/*
    * This file contains the bodies of the fuctions from file CPS_drone.h
    * It includes the initialization of the CPS3 drone structure, getting the battery state,
    * setting the slow mode, setting the motors direction, stopping the motors, and setting the motors speed.
*/
// Function to initialize the CPS3 drone structure
void cps3_init(cps3_t *cps3){
    // Initialize drone battery
    cps3->DroneBattery.Voltage1S = 0.0;
    cps3->DroneBattery.Voltage2S = 0.0;
    cps3->DroneBattery.VoltageTotal = 0.0;
    cps3->DroneBattery.Percentage = 0;
    cps3->DroneBattery.lowVoltageFlag = false;
    cps3->DroneBattery.firstMeasurementFlag = false;
    /*
        * Initialize motors
    */
    // Vertical motor (MotorA)
    cps3->MotorA.Speed = 0;
    cps3->MotorA.InvertedFlag = false;
    // Left motor (MotorL)
    cps3->MotorL.Speed = 0;
    cps3->MotorL.InvertedFlag = false;
    // Right motor (MotorR)
    cps3->MotorR.Speed = 0;
    cps3->MotorR.InvertedFlag = false;
}
// Function to get the battery state of the CPS3 drone
void get_cps3_battery_state(cps3_t *cps3) {
    // Wait for the drone to respond
    digitalWrite(RS_MASTER_ENABLE_PIN, LOW);
    // Read the data from the drone
    if (Serial.available() > 0) {
        // Read the data until newline character
        String data = Serial.readStringUntil('\n');
        // Looking for the indexes of the battery voltages data
        int bat1sIndex = data.indexOf("O");
        int bat2sIndex = data.indexOf("T");
        int masterModeIndex = data.indexOf("M");


        // If the data is present
        if (bat1sIndex != -1 && bat2sIndex != -1 && masterModeIndex != -1) {
            //Extract the values from the data
            String bat1sValue = data.substring(bat1sIndex + 1);
            String bat2sValue = data.substring(bat2sIndex + 1);  
            String masterModeValue = data.substring(masterModeIndex + 1);

            //Convert the values to floats
            cps3->DroneBattery.Voltage1S = bat1sValue.toFloat();
            cps3->DroneBattery.Voltage2S = bat2sValue.toFloat();
            cps3->master_mode = (bool)masterModeValue.toInt();
        }
        // Calculate the total battery voltage
        cps3->DroneBattery.VoltageTotal = cps3->DroneBattery.Voltage1S + cps3->DroneBattery.Voltage2S;

        // First drone battery measurement
        if(cps3->DroneBattery.VoltageTotal != 0.00 && cps3->DroneBattery.firstMeasurementFlag == false) {
            cps3->DroneBattery.firstMeasurementFlag = true; // Set the first measurement flag to true
        }

        // Calculate the battery percentage
        cps3->DroneBattery.Percentage = map((int)(cps3->DroneBattery.VoltageTotal * 100), 600, 840, 0, 100);
        digitalWrite(RS_MASTER_ENABLE_PIN, HIGH); // Enable RS485 master
    }
    else{
        return;
    }
}

// Fuction only for set_cps3_motors_speed
void map_speeds(cps3_t *cps3) {
    // Map the speeds of the motors to the range of 0 to 180
    cps3->MotorA.Speed = map(cps3->MotorA.Speed, -90, 90, MIN_SPEED, MAX_SPEED);
    cps3->MotorL.Speed = map(cps3->MotorL.Speed, -90, 90, MIN_SPEED, MAX_SPEED);
    cps3->MotorR.Speed = map(cps3->MotorR.Speed, -90, 90, MIN_SPEED, MAX_SPEED);
}

/*
    * This function limits the speeds of all three motors when the
    * low-speed mode is selected by the left switch.
    * Previously MotorA (vertical) was excluded, which made the vertical
    * thruster ignore the low-speed mode entirely.
*/
void set_cps3_speed_mode(cps3_t *cps3, remote_t *remote) {
    // Limit the speeds if speed mode is set to LOW
    if(remote->SwitchLeft_state == LOW) {
        cps3->MotorL.Speed /= 2;
        cps3->MotorR.Speed /= 2;
        cps3->MotorA.Speed /= 2;
    }
}
// Function to set the flight mode based on the right switch state
void set_cps3_flight_mode(cps3_t *cps3, remote_t *remote){
    if (remote->SwitchRight_state) {
        cps3->FlightMode = true; // Switch right is ON, set flight mode to ARMED
    } else {
        cps3->FlightMode = false; // Switch right is OFF, set flight mode to DISARMED
    }
}

// Function to set the direction of the motors based on their inverted flags
void set_cps3_motors_direction(cps3_t *cps3) {
    /*
        * Read the motors direction from EEPROM
    */
    cps3->MotorA.InvertedFlag = EEPROM.read(EEPROM_ADDR_MOTOR_A); // Read the inverted flag for the vertical motor from EEPROM
    cps3->MotorL.InvertedFlag = EEPROM.read(EEPROM_ADDR_MOTOR_L); // Read the inverted flag for the left motor from EEPROM
    cps3->MotorR.InvertedFlag = EEPROM.read(EEPROM_ADDR_MOTOR_R); // Read the inverted flag for the right motor from EEPROM

    if(cps3->MotorA.InvertedFlag == true) {
        // Invert the direction of the vertical motor (MotorA)
        cps3->MotorA.Speed = 180 - cps3->MotorA.Speed;
    }
    if(cps3->MotorL.InvertedFlag == true) {
        // Invert the direction of the left motor (MotorL)
        cps3->MotorL.Speed = 180 - cps3->MotorL.Speed;
    }
    if(cps3->MotorR.InvertedFlag == true) {
        // Invert the direction of the right motor (MotorR)
        cps3->MotorR.Speed = 180 - cps3->MotorR.Speed;
    }
}

/*
    * Function to set the speed of the motors based on the remote control inputs.
    * It also uses the set_cps3_speed_mode and set_cps3_motors_direction functions,
    * to limit the speeds and set the direction of the motors.
    * The vertical motor (MotorA) is controlled by the vertical value of the left joystick,
    * while the left and right motors (MotorL and MotorR) are controlled by the vertical value
    * of the right joystick and the horizontal value of the left joystick.
*/ 
void set_cps3_motors_speed(cps3_t *cps3, remote_t *remote) {
    // Set the speed of the vertical motor (MotorA)
    cps3->MotorA.Speed = remote->JoystickLeft.StickV_value;

    // Set the speed of the Left and Right motors (MotorL and MotorR)
    if (remote->JoystickRight.StickV_value >= 5 && remote->JoystickRight.StickV_value <= 45) { // Slowly moving forward
        if (remote->JoystickLeft.StickH_value <= -5) { // Turning left
            remote->JoystickLeft.StickH_value = -remote->JoystickLeft.StickH_value;
            cps3->MotorL.Speed = (remote->JoystickRight.StickV_value - 2 * remote->JoystickLeft.StickH_value) / 3;
            cps3->MotorR.Speed = (remote->JoystickRight.StickV_value + 2 * remote->JoystickLeft.StickH_value) / 3;
        } else if (remote->JoystickLeft.StickH_value >= 5) { // Turning right
            cps3->MotorL.Speed = (remote->JoystickRight.StickV_value + 2 * remote->JoystickLeft.StickH_value) / 3;
            cps3->MotorR.Speed = (remote->JoystickRight.StickV_value - 2 * remote->JoystickLeft.StickH_value) / 3;
        } else { // No turning (dead zone)
            cps3->MotorL.Speed = remote->JoystickRight.StickV_value;
            cps3->MotorR.Speed = remote->JoystickRight.StickV_value;
        }
    } else if (remote->JoystickRight.StickV_value > 45) { // Fast moving forward
        if (remote->JoystickLeft.StickH_value <= -5) { // Turning left
            remote->JoystickLeft.StickH_value = -remote->JoystickLeft.StickH_value;
            // cps3->MotorL.Speed = (2 * remote->JoystickRight.StickV_value - remote->JoystickLeft.StickH_value) / 3;
            cps3->MotorL.Speed = 0;
            cps3->MotorR.Speed = (2 * remote->JoystickRight.StickV_value + remote->JoystickLeft.StickH_value) / 3;
        } else if (remote->JoystickLeft.StickH_value >= 5) { // Turning right
            cps3->MotorL.Speed = (2 * remote->JoystickRight.StickV_value + remote->JoystickLeft.StickH_value) / 3;
            //cps3->MotorR.Speed = (2 * remote->JoystickRight.StickV_value - remote->JoystickLeft.StickH_value) / 3;
            cps3->MotorR.Speed = 0;
        } else { // No turning (dead zone)
            cps3->MotorL.Speed = remote->JoystickRight.StickV_value;
            cps3->MotorR.Speed = remote->JoystickRight.StickV_value;
        }
    } else if (remote->JoystickRight.StickV_value <= -5 && remote->JoystickRight.StickV_value >= -45) { // Slowly moving backward
        if (remote->JoystickLeft.StickH_value <= -5) { // Turning left
            cps3->MotorL.Speed = (remote->JoystickRight.StickV_value + 2 * remote->JoystickLeft.StickH_value) / 3;
            //cps3->MotorR.Speed = (remote->JoystickRight.StickV_value - 2 * remote->JoystickLeft.StickH_value) / 3;
            cps3->MotorR.Speed = 0;
        } else if (remote->JoystickLeft.StickH_value >= 5) { // Turning right
            remote->JoystickLeft.StickH_value = -remote->JoystickLeft.StickH_value;
            //cps3->MotorL.Speed = (remote->JoystickRight.StickV_value - 2 * remote->JoystickLeft.StickH_value) / 3;
            cps3->MotorL.Speed = 0;
            cps3->MotorR.Speed = (remote->JoystickRight.StickV_value + 2 * remote->JoystickLeft.StickH_value) / 3;
        } else { // No turning (dead zone)
            cps3->MotorL.Speed = remote->JoystickRight.StickV_value;
            cps3->MotorR.Speed = remote->JoystickRight.StickV_value;
        }
    } else if (remote->JoystickRight.StickV_value < -45) { // Fast moving backward
        if (remote->JoystickLeft.StickH_value <= -5) { // Turning left
            cps3->MotorL.Speed = (2 * remote->JoystickRight.StickV_value + remote->JoystickLeft.StickH_value) / 3;
            //cps3->MotorR.Speed = (2 * remote->JoystickRight.StickV_value - remote->JoystickLeft.StickH_value) / 3;
            cps3->MotorR.Speed = 0;
        } else if (remote->JoystickLeft.StickH_value >= 5) { // Turning right
            remote->JoystickLeft.StickH_value = -remote->JoystickLeft.StickH_value;
            //cps3->MotorL.Speed = (2 * remote->JoystickRight.StickV_value - remote->JoystickLeft.StickH_value) / 3;
            cps3->MotorL.Speed = 0;
            cps3->MotorR.Speed = (2 * remote->JoystickRight.StickV_value + remote->JoystickLeft.StickH_value) / 3;
        } else { // No turning (dead zone)
            cps3->MotorL.Speed = remote->JoystickRight.StickV_value;
            cps3->MotorR.Speed = remote->JoystickRight.StickV_value;
        }
    } 
    else { // No forward or backward movement
        if (remote->JoystickLeft.StickH_value <= -5) { // Turning left
            remote->JoystickLeft.StickH_value = -remote->JoystickLeft.StickH_value;
            cps3->MotorL.Speed = -remote->JoystickLeft.StickH_value;
            cps3->MotorR.Speed = remote->JoystickLeft.StickH_value;
        } else if (remote->JoystickLeft.StickH_value >= 5) { // Turning right
            cps3->MotorL.Speed = remote->JoystickLeft.StickH_value;
            cps3->MotorR.Speed = -remote->JoystickLeft.StickH_value;
        }
        else{
            // No turning (dead zone)
            cps3->MotorL.Speed = 0;
            cps3->MotorR.Speed = 0;
        }
    }
    set_cps3_speed_mode(cps3, remote); // Set the speed mode
    map_speeds(cps3); // Map the speeds of the motors to the range of 0 to 180
    set_cps3_motors_direction(cps3); // Set the motors direction based on inverted flags

}

// Function to set all motors speed to 0
void set_cps3_motors_stopped(cps3_t *cps3) {
    cps3->MotorA.Speed = 0;
    cps3->MotorL.Speed = 0;
    cps3->MotorR.Speed = 0;
}

void set_cps3_as_master(cps3_t *cps3){
    cps3->master_mode = RS485_MASTER_MODE; // Set CPS3 drone to master mode
}

void send_cps3_motors_speed(remote_t *remote, cps3_t *cps3) {
    digitalWrite(RS_MASTER_ENABLE_PIN, HIGH); // Enable RS485 master
    set_cps3_as_master(cps3); // Set CPS3 drone as master
    if(cps3->FlightMode == ARMED){
        Serial.println(String("L") + cps3->MotorL.Speed + "R" + cps3->MotorR.Speed + "A" + cps3->MotorA.Speed + "M" + cps3->master_mode);
    }
    else{
        Serial.println(String("L") + DISABLED_SPEED + "R" + DISABLED_SPEED + "A" + DISABLED_SPEED + "M" + cps3->master_mode);
    }
    Serial.flush();
    digitalWrite(RS_MASTER_ENABLE_PIN, LOW);
}

