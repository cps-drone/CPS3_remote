#include "Remote.h"

void remote_init(remote_t *remote) {
    // Initialize left joystick
    remote->JoystickLeft.StickH_value = 0;
    remote->JoystickLeft.StickV_value = 0;
    remote->JoystickLeft.StickB_state = false;

    // Initialize right joystick
    remote->JoystickRight.StickH_value = 0;
    remote->JoystickRight.StickV_value = 0;
    remote->JoystickRight.StickB_state = false;

    // Initialize buttons module
    remote->Buttons.Button1_state = false;
    remote->Buttons.Button2_state = false;
    remote->Buttons.Button3_state = false;
    remote->Buttons.Button4_state = false;
    remote->Buttons.Button5_state = false;

    // Initialize switches
    remote->SwitchLeft_state = false;
    remote->SwitchRight_state = false;

    // Initialize battery voltage and percentage
    remote->Battery.voltage = 0.0f;
    remote->Battery.percents = 0;

    // Initialize RS485 master enable state
    remote->MasterEnableState = false;

    // Initialize timing variables
    remote->currentSendTime = 0;
    remote->intervalSend = 100; // Default interval for reading remote data
    remote->previousSendTime = 0;

    //Set the pins as inputs or outputs
    pinMode(SWITCH_RIGHT, INPUT);
    pinMode(SWITCH_LEFT, INPUT);
    pinMode(RS_MASTER_ENABLE_PIN, OUTPUT);
    pinMode(BUTTON1, INPUT);
    pinMode(BUTTON2, INPUT);
    pinMode(BUTTON3, INPUT);
    pinMode(BUTTON4, INPUT);
    pinMode(BUTTON5, INPUT);
    pinMode(STICK_LEFT_H_PIN, INPUT);
    pinMode(STICK_LEFT_V_PIN, INPUT);
    pinMode(STICK_RIGHT_H_PIN, INPUT);
    pinMode(STICK_RIGHT_V_PIN, INPUT);
    pinMode(STICK_LEFT_B_PIN, INPUT);
    pinMode(STICK_RIGHT_B_PIN, INPUT);
    pinMode(REMOTE_BATTERY_SENSOR_PIN, INPUT);

    // Set the baud rate for the RS485 communication
    Serial.begin(9600); //Set the baud rate for the serial communication  
}

void joystick_read(remote_t *remote){
    remote->JoystickLeft.StickH_value = map(analogRead(STICK_LEFT_H_PIN), 0, 1023, -90, 90);
    remote->JoystickLeft.StickV_value = map(analogRead(STICK_LEFT_V_PIN), 0, 1023, -90, 90);
    remote->JoystickLeft.StickB_state = digitalRead(STICK_LEFT_B_PIN);

    remote->JoystickRight.StickH_value = map(analogRead(STICK_RIGHT_H_PIN), 0, 1023, -90, 90);
    remote->JoystickRight.StickV_value = map(analogRead(STICK_RIGHT_V_PIN), 0, 1023, 90, -90);
    remote->JoystickRight.StickB_state = digitalRead(STICK_RIGHT_B_PIN);
}

void buttons_module_read(remote_t *remote){
    if(digitalRead(BUTTON1) == PRESSED){
        delay(DEBOUNCE); // Debounce delay
        remote->Buttons.Button1_state = !remote->Buttons.Button1_state;
    }
    else if(digitalRead(BUTTON2) == PRESSED){
        delay(DEBOUNCE); // Debounce delay
        remote->Buttons.Button2_state = !remote->Buttons.Button2_state;
    }
    else if(digitalRead(BUTTON3) == PRESSED){
        delay(DEBOUNCE); // Debounce delay
        remote->Buttons.Button3_state = !remote->Buttons.Button3_state;
    }
    else if(digitalRead(BUTTON4) == PRESSED){
        delay(DEBOUNCE); // Debounce delay
        remote->Buttons.Button4_state = !remote->Buttons.Button4_state;
    }
    else if(digitalRead(BUTTON5) == PRESSED){
        delay(DEBOUNCE); // Debounce delay
        remote->Buttons.Button5_state = !remote->Buttons.Button5_state;
    }
}

void reset_buttons_states(remote_t *remote){
    remote->Buttons.Button1_state = IDLE;
    remote->Buttons.Button2_state = IDLE;
    remote->Buttons.Button3_state = IDLE;
    remote->Buttons.Button4_state = IDLE;
    remote->Buttons.Button5_state = IDLE;
}

void switches_read(remote_t *remote){
    remote->SwitchLeft_state = digitalRead(SWITCH_LEFT);
    remote->SwitchRight_state = digitalRead(SWITCH_RIGHT);
}

void remote_voltage_read(remote_t *remote) {
    remote->Battery.rawVoltage = analogRead(REMOTE_BATTERY_SENSOR_PIN);
    remote->Battery.voltage = map(remote->Battery.rawVoltage, 0, 1023, 0, 500);
    remote->Battery.voltage /= 100; // Convert to volts
    remote->Battery.percents = map((int)(remote->Battery.voltage*100), 300, 420, 0, 100); // Assuming 3.0V is 0% and 4.2V is 100%
    if (remote->Battery.percents < 0) remote->Battery.percents = 0;
    if (remote->Battery.percents > 100) remote->Battery.percents = 100;
}

void remote_read(remote_t *remote) {
    joystick_read(remote);
    switches_read(remote);
}




