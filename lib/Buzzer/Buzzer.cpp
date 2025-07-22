#include <PWM.h>
#include "Definitions.h"
#include <Arduino.h>
#include "Buzzer.h"

void buzzer_init(buzzer_t *buzzer, int pin, int frequency) {
    // Initialize the buzzer pin and set the frequency
    InitTimersSafe();
    /*
        * Set the PWM frequency to 2300 Hz.
        * This frequency was tested and works well with the buzzer in the remote.
    */
   
    buzzer->pin = BUZZER_PIN;
    buzzer->currentMillis = millis();
    buzzer->previousMillis = 0;
    buzzer->interval = BUZZER_INTERVAL;
    buzzer->pwmChannel = SetPinFrequencySafe(buzzer->pin, 2300);

    // Set the initial state of the buzzer to off
    pwmWrite(buzzer->pin, 0);
}

void buzzerToggle(buzzer_t *buzzer) {

    // Check if the interval has passed
    if (buzzer->currentMillis - buzzer->previousMillis >= buzzer->interval) {
        buzzer->previousMillis = buzzer->currentMillis; // Reload the previousMillis with the current time
 
        if (buzzer->state == true) {
            // Turn on the buzzer with 50% duty cycle
            pwmWrite(buzzer->pin, 128); // 128 is 50% duty cycle (0-255 range)
        } else {
            // Turn off the buzzer
            pwmWrite(buzzer->pin, 0);
        }
        
        // Toggle the buzzer state
        buzzer->state = !buzzer->state;
    }
}

void buzzerDisable(buzzer_t *buzzer) {
    // Turn off the buzzer
    pwmWrite(buzzer->pin, 0);
}
