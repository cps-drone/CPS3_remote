#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "Logo.h"
#include "Definitions.h"
#include <EEPROM.h>
#include "Remote.h"
#include "CPS3_drone.h"

/*
    * Function that initializes the OLED diplay,
    * it also displays the CPS3 logo for 3 seconds.
*/ 
void display_init(Adafruit_SSD1306 *display);

/*
    * Function that switches the volatage menu,
    * Current (favourite) menu is stored in EEPROM non-volatile memory.
    * MENUs:
    * 0 - Drone battery percentage
    * 1 - Remote battery percentage
    * 2 - Remote battery volatage
    * 3 - Drone battery total voltage
    * 4 - Drone battery 1S voltage
    * 5 - Drone battery 2S voltage
    * To switch the menu, press and hold the Button1.
*/
void display_switch_voltage_menu();

/*
    * Function that updates the display with measurements,
    * flight mode, and speed mode.
    * 
*/
void update_display(Adafruit_SSD1306 *display, remote_t *remote, cps3_t *cps3);

/*
    * Function that displays the motor direction inversion menu,
    * it allows to select the motor for which the direction will be inverted,
    * and to invert the direction of the selected motor.
    * The selected motor, and all invertion flags are stored in EEPROM non-volatile memory.
    * The motors are selected by pressing the Button4,
    * and the direction is inverted by pressing the Button3.
*/
void display_motor_direction_menu(Adafruit_SSD1306 *display, cps3_t *cps3, remote_t *remote);

/*
    * Function that displays the test mode,
    * it shows the pressed button, switches state,
    * and the joystick values (if the joystick button is pressed).
*/
void display_test_mode(Adafruit_SSD1306 *display, remote_t *remote);

#endif