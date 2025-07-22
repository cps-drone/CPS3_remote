#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#include "PWM.h"

typedef struct buzzer_s{
    int pwmChannel; // PWM channel for the buzzer
    bool state; // State of the buzzer, false = off, true = on
    unsigned long previousMillis; // Variable to store the previous time the buzzer was toggled, used for non-blocking timing
    unsigned long interval; // Interval for toggling the buzzer state in milliseconds
    unsigned long currentMillis; // Current time in milliseconds
    int frequency; // Frequency of the buzzer
    int pin; // Pin number for the buzzer
} buzzer_t;

/*
    * Function that initializes buzzer struct object
    * It sets the pin, frequency, initial state, and interval for toggling.
    * It also gets the current time for the buzzer timer.
*/
void buzzer_init(buzzer_t *buzzer, int pin, int frequency);

/*
    * Function that toggles the buzzer state.
*/
void buzzerToggle(buzzer_t *buzzer);

/*
    * Fuction that disables the buzzer.
*/
void buzzerDisable(buzzer_t *buzzer);

#endif