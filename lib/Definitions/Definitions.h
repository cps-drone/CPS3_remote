#ifndef DEFINITIONS_H
#define DEFINITIONS_H

//Display definitions:
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

//Joystick sticks:
#define StickLeftH A2
#define StickLeftV A1
//StickRightH is not used, A3 and A6 are inverted due to position of the right joystick
#define StickRightH A3 
#define StickRightV A6

//Joystick buttons:
#define StickLeftB 11
#define StickRightB 12

//Remote battery voltage measurement:
#define BatterySensor A0

//Toggle switches:
#define SwitchRight 9
#define SwitchLeft 2 //NEED TO BE CHANGED TO 13 FOR THE FINAL VERSION

//RS485 enable pins:
#define MasterEnable 3 //NEED TO BE CHANGED TO 2 FOR THE FINAL VERSION

//Modes definitions
#define ARMED HIGH
#define DISARMED LOW

//Buttons module definitions
#define Button1 5
#define Button2 6
#define Button3 7
#define Button4 8
#define Button5 10

//Buzzer
//NEED TO BE CHANGED TO 3 FOR THE FINAL VERSION
// #define Buzzer 13 // WRONG PWM PIN 
// #define BuzzerPWMFrequency 2900

#endif