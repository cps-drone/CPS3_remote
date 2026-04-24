#ifndef DEFINITIONS_H
#define DEFINITIONS_H

//Display definitions:
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

//EEPROM adresses definitions:
#define EEPROM_ADDR_MOTOR_A 0
#define EEPROM_ADDR_MOTOR_L 1
#define EEPROM_ADDR_MOTOR_R 2
#define EEPROM_ADDR_VOLTAGE_MENU 3

//Joystick sticks:
#define STICK_LEFT_H_PIN A2
#define STICK_LEFT_V_PIN A1
//StickRightH is not used, A3 and A6 are inverted due to position of the right joystick
#define STICK_RIGHT_H_PIN A3 
#define STICK_RIGHT_V_PIN A6

//Joystick buttons:
#define STICK_LEFT_B_PIN 12
#define STICK_RIGHT_B_PIN 11

//Remote battery voltage measurement:
#define REMOTE_BATTERY_SENSOR_PIN A0

//Toggle switches
#define SWITCH_RIGHT 9
#define SWITCH_LEFT 13

//RS485 enable pins
/*
    * RS485 master enable pin, if it is HIGH in remote it sets the remote as transmitter (master),
    * if it is LOW it sets the remote as a receiver (slave).
*/
#define RS_MASTER_ENABLE_PIN 2  

// RS485 other constants
#define RS485_SWITCH_DELAY 10   // MAX485 switching delay (w ms)
#define MESSAGE_LENGTH 23       // Fixed length of the message to be send


//RS485 MODES
#define RS485_MASTER_MODE 1 // Mode for transmitting the data to the CPS3 drone.(Remote is a master)
#define RS485_SLAVE_MODE 0  // Mode for receiving the data from the CPS3 drone. (Remote is a slave)

//Modes definitions
#define ARMED HIGH          // State enabling convertion joysticks values to motor speeds
#define DISARMED LOW        // State disabling convertion joysticks values to motor speeds

//Buttons states
#define PRESSED 0           // State for pressed buttons
#define IDLE 1              // State for idle buttons

//Buttons module definitions
#define BUTTON1 5
#define BUTTON2 6
#define BUTTON3 7
#define BUTTON4 8
#define BUTTON5 10

//Buzzer
#define BUZZER_PIN 3            // Buzzer pin
#define BUZZER_FREQUENCY 2300   // Buzzer frequency in Hz

//Motor speed limits
#define MAX_SPEED 140       // Maximum speed of the motor (in HIGH speed mode)
#define DISABLED_SPEED 90   // Speed of the motor in which it is not spinning (in HIGH speed mode)
#define MIN_SPEED 40        // Minimum speed of the motor (in HIGH speed mode)

//Intervals, time constants
#define DEBOUNCE 100 // Button debounce time in milliseconds
#define BUZZER_INTERVAL 500 // Buzzer toggling interval in miliseconds

//Low voltage warning values
#define REMOTE_LOW_VOLTAGE_WARNING 3.0 // Remote low voltage warning, buzzer will start beeping if the voltage is below that value
#define CPS3_LOW_VOLTAGE_WARNING 6.2 // CPS3 drone low voltage warning, buzzer will start beeping if voltage is below that value

#endif