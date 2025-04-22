#include <PWM.h>
#include "Definitions.h"
#include <Arduino.h>
#include "Buzzer.h"

// Pin buzzera
#define Buzzer 3

// Zmienna do przechowywania identyfikatora kanału PWM
int pwmChannel;

// Zmienna do sterowania stanem buzzera
bool buzzerState = false;

// Czas ostatniej zmiany stanu buzzera
unsigned long previousMillis = 0;

// Interwał włączenia/wyłączenia buzzera (1 sekunda)
const unsigned long interval = 1000;

void initBuzzer() {
    // Inicjalizacja biblioteki PWM
    InitTimersSafe();
    // Ustaw częstotliwość PWM na 2,3 kHz
    pwmChannel = SetPinFrequencySafe(Buzzer, 2300);

    // Wyłącz buzzer na start
    pwmWrite(Buzzer, 0);
}


void toggleBuzzerNonBlocking() {
    unsigned long currentMillis = millis();

    // Sprawdź, czy nadszedł czas na zmianę stanu buzzera
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis; // Zaktualizuj czas ostatniej zmiany

        // Zmień stan buzzera
        buzzerState = !buzzerState;

        if (buzzerState) {
            // Włącz buzzer z wypełnieniem 50%
            pwmWrite(Buzzer, 128); // 128 to 50% wypełnienia (zakres 0-255)
        } else {
            // Wyłącz buzzer
            pwmWrite(Buzzer, 0);
        }
    }
}

void disableBuzzer() {
    // Wyłącz buzzer
    pwmWrite(Buzzer, 0);
}
