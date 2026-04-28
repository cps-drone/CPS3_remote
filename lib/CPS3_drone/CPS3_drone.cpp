#include "CPS3_drone.h"

/*
    * This file contains the bodies of the fuctions from file CPS_drone.h
    * It includes the initialization of the CPS3 drone structure, getting the battery state,
    * setting the slow mode, setting the motors direction, stopping the motors, and setting the motors speed.
*/
// Function to initialize the CPS3 drone structure
void cps3_init(cps3_t *cps3){
    // Initialize drone battery
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
    // Set RS485 to receive mode
    digitalWrite(RS_MASTER_ENABLE_PIN, LOW);
    delay(RS485_SWITCH_DELAY); // Delay to allow the MAX485 to switch to receive mode
    char buffer[6]; // Buffer to store the incoming message (5 characters + null terminator)
    int index = 0;  // Index to track the position in the buffer
    unsigned long startTime = millis(); // Start the timer

    // Wait for data with a timeout of 20 ms
    while (millis() - startTime < 20) {
        if (Serial.available() > 0) {
            char incomingByte = Serial.read(); // Read one byte from the serial buffer
            // Store the byte in the buffer if there's space
            if (index < 5) {
                buffer[index++] = incomingByte;
            }
            // Stop reading if the message ends with 'E'
            if (incomingByte == 'E') {
                break;
            }
        }
    }
    buffer[index] = '\0';     // Null-terminate the buffer to make it a valid string
    Serial.print("Received battery message: ");
    Serial.println(buffer); // Print the received message for debugging
    if (buffer[0] == 'V') {         // Check if the message starts with 'V'
        cps3->DroneBattery.VoltageTotal = atof(&buffer[1]);        // Convert the substring after 'V' to a float
        // If this is the first valid voltage measurement, set the flag
        if (cps3->DroneBattery.VoltageTotal != 0.00 && !cps3->DroneBattery.firstMeasurementFlag) {
            cps3->DroneBattery.firstMeasurementFlag = true;
        }
        cps3->DroneBattery.Percentage = map(            // Calculate the battery percentage based on the voltage range
            (int)(cps3->DroneBattery.VoltageTotal * 100), // Convert voltage to an integer for mapping
            600, 840,                                    // Map range: 6.00V to 8.40V
            0, 100                                       // Output range: 0% to 100%
        );
    }
    
    digitalWrite(RS_MASTER_ENABLE_PIN, HIGH);
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
    !cps3->FlightMode ? set_cps3_motors_stopped(cps3) : void(); // If the flight mode is DISARMED, stop the motors
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


// Function to control the gripper based on the remote control inputs
void gripper_steering(cps3_t *cps3, remote_t *remote){
    if (remote->Buttons.Button3_state == PRESSED){
        cps3->gripper.enabled = !cps3->gripper.enabled; // Toggle the gripper enabled state
    }
    if (remote->Buttons.Button2_state == PRESSED){
        if(cps3->gripper.enabled) {
            cps3->gripper.command = CLOSE; // Set the gripper command to OPEN
        }
    }
    else if (remote->Buttons.Button4_state == PRESSED){
        if(cps3->gripper.enabled) {
            cps3->gripper.command = OPEN; // Set the gripper command to CLOSE
        }
    }
    if(remote->Buttons.Button4_state == IDLE && remote->Buttons.Button2_state == IDLE){
        cps3->gripper.command = STOP; // Set the gripper command to STOP
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


void send_to_cps3(remote_t *remote, cps3_t *cps3) {

    digitalWrite(RS_MASTER_ENABLE_PIN, HIGH); // Set the RS485 transmission mode to master
    delay(RS485_SWITCH_DELAY); // Wait to ensure the mode is switched before sending the message

    // 
    char send_message[MESSAGE_LENGTH + 1]; // +1 for character "null-terminator"
    snprintf(send_message, MESSAGE_LENGTH + 1, "L%03dR%03dA%03dD%01dG%01dC%05dE", 
             cps3->MotorL.Speed,    // 3 characters for MotorL.Speed
             cps3->MotorR.Speed,    // 3 characters for MotorR.Speed
             cps3->MotorA.Speed,    // 3 characters for MotorA.Speed
             cps3->LEDs_flag,       // 2 characters for LEDs_flag
             cps3->gripper.command, // 2 characters for gripper.command
             00000);              // 5 characters to be customised
                        

    Serial.print(send_message); // Send the message to the drone
    Serial.flush(); // Force to clear the serial buffer and wait for the transmission to complete
    delay(RS485_SWITCH_DELAY); // Wait to ensure the message is fully sent before switching back to receive mode

    digitalWrite(RS_MASTER_ENABLE_PIN, LOW); // Set RS485 to the receive mode
}

