#ifndef REMOTE_H
#define REMOTE_H

#include <Arduino.h>
#include "Definitions.h"

/*
    * This file contains the structures and function prototypes for the remote control measurements.
    * It includes joystick, buttons module, and remote structures, as well as function prototypes for reading measurements.
    * Below are the structures and function prototypes for the remote control measurements.
*/

// Joystick structure
typedef struct joystick_s{
    int StickH_value;  // joystick horizontal value
    int StickV_value;  // joystick vertical value
    bool StickB_state;   // joystick button state
} joystick_t;

// Buttons module structure
typedef struct buttons_module_s {
    bool Button1_state; // Button 1
    bool Button2_state; // Button 2
    bool Button3_state; // Button 3
    bool Button4_state; // Button 4
    bool Button5_state; // Button 5
} buttons_module_t;


// Remote battery structure
typedef struct remote_battery_s {
    float rawVoltage; // Raw remote battery voltage in mV
    float voltage; // Remote battery voltage in mV
    int percents; // Remote battery percentage
    bool lowVoltageFlag; // Remote battery low state

    // Remote battery measuring timer
    unsigned long currentTime; // Current time in milliseconds
    unsigned long interval; // sending interval in milliseconds
    unsigned long previousTime; // Previous time in milliseconds for non-blocking timing
} remote_battery_t;

// Remote structure
typedef struct remote_s {
    joystick_t JoystickLeft; // Left joystick structure
    joystick_t JoystickRight; // Right joystick structure
    buttons_module_t Buttons;
    bool SwitchLeft_state;  // Left switch state
    bool SwitchRight_state; // Right switch state

    remote_battery_t Battery; // Remote battery structure

    bool MasterEnableState; // RS485 master enable state

    // Sending data timer
    // unsigned long currentSendTime; // Current time in milliseconds
    // unsigned long intervalSend; // sending interval in milliseconds
    // unsigned long previousSendTime; // Previous time in milliseconds for non-blocking timing
} remote_t;

// Function prototypes

/*
    * Function to initialize the remote structure.
    * It initialize the joysticks, buttons, switches and the battery with value 0.
    * It also sets the RS485 master enable pin as output and sets the remote as a receiver (slave)
*/
void remote_init(remote_t *remote);

/*
    * Function to read the joystick values form the joystick modules.
*/
void joystick_read(remote_t *remote);

/*
    * Function to read the buttons from buttons module.
    * It remember the state of the buttons and toggles them on press.
*/
void buttons_module_read(remote_t *remote);

/*
    * Function to read the switches values from the remote.
*/
void switches_read(remote_t *remote);

/*
    * Function to read the remote battery voltage and calculate the battery percentage.
*/
void remote_voltage_read(remote_t *remote);

/*
    * Fuction that reads states of the switches and the joysticks.
*/
void remote_read(remote_t *remote);

/*
    * Function to reset the buttons states to IDLE.
*/
void reset_buttons_states(remote_t *remote);

#endif // REMOTE_H

