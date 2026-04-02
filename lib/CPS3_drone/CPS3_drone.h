#ifndef CPS3_DRONE_H
#define CPS3_DRONE_H

#include <Arduino.h>
#include "Definitions.h"
#include "Remote.h"
#include "EEPROM.h"

/*    
    * Drone battery structure
    * It contains the voltage of the battery in 1S and 2S, total voltage, battery percentage,
    * first measurement flag and low voltage warning flag.
    * Voltages are represented as a float values.
    * Battery percentage is represented as an integer value.
    * firstMeasurementFlag is a boolean value that indicates if the first measurement has been taken.
    * lowVoltageFlag is a boolean value that indicates if the battery voltage is low.
*/
typedef struct drone_battery_s {
    float Voltage; // Battery voltage 2S in V
    float VoltageTotal; // Total battery voltage in V
    int Percentage; // Battery percentage
    bool firstMeasurementFlag; // Flag to indicate if the first measurement has been taken
    bool lowVoltageFlag; // Low voltage warning flag
} drone_battery_t;

/*    
    * Motor structure
    * It contains the speed of the motor, speed mode (LOW or HIGH) and inverted flag
    * The inverted flag indicates if the motor direction is inverted
    * (Inverted Flag is stored in EEPROM nonvolatile memory)
    * Speed is represented as an integer value
    * SpeedMode is a boolean value (LOW or HIGH)
*/
typedef struct motor_s {
    int Speed; // Speed of the motor
    bool SpeedMode; // Speed mode of the motor (LOW or HIGH)
    bool InvertedFlag; // Direction of the motor
} motors_t;

typedef enum command_e {
    CLOSE = 0,
    STOP = 1,
    OPEN = 2
}command_t;


typedef struct gripper_s{
    bool enabled;
    command_t command;
}gripper_t;


/* 
    * CPS3 drone structure
    * It contains the drone battery, motors and flight mode
    * The motors are represented by the motors_t structure
    * The flight mode is represented by a boolean value (ARMED or DISARMED)
*/ 
typedef struct cps3_s {
    drone_battery_t DroneBattery;
    gripper_t gripper; // Gripper structure
    motors_t MotorA; // Vertical motor
    motors_t MotorL; // Left motor
    motors_t MotorR; // Right motor
    bool FlightMode; // Flight mode of the drone (ARMED or DISARMED)
    bool master_mode; // RS485 master mode
    bool LEDs_flag; // Flag to toggle the LEDs on the drone
} cps3_t;

/*
    *Functions prototypes for CPS3 drone operations
*/

void gripper_steering(cps3_t *cps3, remote_t *remote);


// Function that initialize the CPS3 drone structure
void cps3_init(cps3_t *cps3);
// Function to get the battery state of the CPS3 drone
void get_cps3_battery_state(cps3_t *cps3);
/*
    * Function that limits the speeds of the motors if the speed mode is set to LOW
    * It only affects the Left and Right motors (MotorL and MotorR)
*/
void set_cps3_speed_mode(cps3_t *cps3, remote_t *remote);

// Function that set the flight mode of the CPS3 drone based on the remote's left switch state
void set_cps3_flight_mode(cps3_t *cps3, remote_t *remote);

// Function that set the direction of the motors based on their inverted flags
void set_cps3_motors_direction(cps3_t *cps3);

// Function that set all motors speed to 0
void set_cps3_motors_stopped(cps3_t *cps3);

/*
    * Function that set the speed of the motors based on the remote control inputs.
    * It also uses the set_cps3_speed_mode and set_cps3_motors_direction functions,
    * to limit the speeds and set the direction of the motors.
    * The vertical motor (MotorA) is controlled by the vertical value of the left joystick,
    * while the left and right motors (MotorL and MotorR) are controlled by the vertical value
    * of the right joystick and the horizontal value of the left joystick.
*/ 
void set_cps3_motors_speed(cps3_t *cps3, remote_t *remote);

// Function that sends the motors speed to the CPS3 drone
void send_to_cps3(remote_t *remote, cps3_t *cps3);

// Fuction that set CPS3 drone flag into master mode
void set_cps3_as_master(cps3_t *cps3);

#endif// CPS3_DRONE_H