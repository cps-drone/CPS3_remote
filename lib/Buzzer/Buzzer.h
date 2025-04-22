#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#include "PWM.h"

void toggleBuzzerNonBlocking();
void initBuzzer();
void disableBuzzer();

#endif