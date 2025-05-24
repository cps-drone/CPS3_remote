#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "Logo.h"
#include "Definitions.h"
#include "Motors_Speed.h"
#include <EEPROM.h>
#include "Buzzer.h"

extern Adafruit_SSD1306 display;

//Display variables
extern volatile bool flight_mode;
extern bool speed_mode;
extern float DroneVoltage1;
extern float DroneVoltage2;
extern float DroneVoltageTotal;
extern int RemoteBatteryPercent;
extern int DroneBatteryPercent;
extern int StickRightH_value;
extern int StickRightV_value;
extern int StickLeftH_value;
extern int StickLeftV_value;
extern bool motorDirectionA;
extern bool motorDirectionL;
extern bool motorDirectionR;
extern bool connectionFlag;
extern uint8_t connectionDisplayCtr;

extern void receive_measurement_data();

void init_display();
void update_display();

#endif