#include "Display_OLED.h"

void display_init(Adafruit_SSD1306 *display){
    if(!display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        for(;;); // Don't proceed, loop forever
    }
    display->clearDisplay();
    //The most beautiful CPS logo
    display->drawBitmap(0, 0, cps3_logo, 128, 32, SSD1306_WHITE);

    display->display();
    delay(3000);
    display->clearDisplay();  //Clear the display after showing the logo
    display->display(); //Ensure the display is updated
}

void display_motor_direction_menu(Adafruit_SSD1306 *display, cps3_t *cps3, remote_t *remote) {
    static char selectedMotor = 'A'; // Default motor selection
    // Handle motor selection using Button4
    if (digitalRead(BUTTON4) == LOW) {
        delay(100); // Button Debounce
        if (selectedMotor == 'A') {
            selectedMotor = 'L';
        } else if (selectedMotor == 'L') {
            selectedMotor = 'R';
        } else if (selectedMotor == 'R') {
            selectedMotor = 'A';
        }
    }
    // Handle direction change using Button3
    if (digitalRead(BUTTON3) == LOW) {
        delay(100); // Button Debounce
        if (selectedMotor == 'A') {
            cps3->MotorA.InvertedFlag = !cps3->MotorA.InvertedFlag;
            EEPROM.update(EEPROM_ADDR_MOTOR_A, cps3->MotorA.InvertedFlag); // Write to EEPROM
        } else if (selectedMotor == 'L') {
            cps3->MotorL.InvertedFlag = !cps3->MotorL.InvertedFlag;
            EEPROM.update(EEPROM_ADDR_MOTOR_L, cps3->MotorL.InvertedFlag); // Write to EEPROM
        } else if (selectedMotor == 'R') {
            cps3->MotorR.InvertedFlag = !cps3->MotorR.InvertedFlag;
            EEPROM.update(EEPROM_ADDR_MOTOR_R, cps3->MotorR.InvertedFlag); // Write to EEPROM
        }
    }
    // Display the motor direction menu
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(0, 0);
    display->println(F("Motor Direction Menu"));
    display->print(F(" Selected motor: "));
    display->println(selectedMotor);
    display->println("-------------------");
    display->print(F("  A: "));
    display->print(cps3->MotorA.InvertedFlag ? F("1") : F("0"));
    display->print(F(", L: "));
    display->print(cps3->MotorL.InvertedFlag ? F("1") : F("0"));
    display->print(F(", R: "));
    display->println(cps3->MotorR.InvertedFlag ? F("1") : F("0"));
    display->display();
}

void display_test_mode(Adafruit_SSD1306 *display, remote_t *remote){

    remote_read(remote); // Read remote data
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(0, 0);
    display->println(F("   Test Mode"));

    display->print(F("Button pressed: "));
    if (digitalRead(BUTTON1) == PRESSED)
        display->println(F("1"));
    else if (digitalRead(BUTTON3) == PRESSED)
        display->println(F("3"));
    else if (digitalRead(BUTTON4) == PRESSED)
        display->println(F("4"));
    else if (digitalRead(BUTTON5) == PRESSED)
        display->println(F("5"));
    else{
        display->println(F(""));
    }
    
    display->print(F("Switch L: "));
    display->print(remote->SwitchLeft_state ? F("1") : F("0"));
    display->print(F(" R: "));
    display->println(remote->SwitchRight_state ? F("1") : F("0"));

    if(remote->JoystickLeft.StickB_state == PRESSED && remote->JoystickRight.StickB_state == IDLE){
        display->print(F("Stick L: H="));
        display->print(remote->JoystickLeft.StickH_value);
        display->print(F(", V="));
        display->println(remote->JoystickLeft.StickV_value);
    }
    if(remote->JoystickRight.StickB_state == PRESSED && remote->JoystickLeft.StickB_state == IDLE){
        display->print(F("Stick R: H="));
        display->print(remote->JoystickRight.StickH_value);
        display->print(F(", V="));
        display->println(remote->JoystickRight.StickV_value);
    }
    display->display();
}

void display_switch_voltage_menu() {
    if (digitalRead(BUTTON1) == LOW) {
        delay(DEBOUNCE); 
        uint8_t currentMenuValue = EEPROM.read(EEPROM_ADDR_VOLTAGE_MENU);
        if (currentMenuValue >= 0 && currentMenuValue < 3) {
            currentMenuValue++;     // increment menu variable
        } else {
            currentMenuValue = 0;   // if menu variable is > 3 set it to 0
        }
        EEPROM.update(EEPROM_ADDR_VOLTAGE_MENU, currentMenuValue);
    }
}

void toggle_LEDs(cps3_t *cps3) {
    if (digitalRead(BUTTON4) == LOW && cps3->gripper.enabled == false) {
        //delay(50); 
        cps3->LEDs_flag = !cps3->LEDs_flag; // Toggle the LEDs flag
    }
}

void update_display(Adafruit_SSD1306 *display, remote_t *remote, cps3_t *cps3) {
    display->clearDisplay();
    display->setTextSize(2);             // Normal 1:1 pixel scale
    display->setTextColor(SSD1306_WHITE);        // Draw white text
    display->setCursor(0, 0);            // Start at top-left corner

    // Normal display logic (e.g., flight mode, speed, etc.)
    if(cps3->DroneBattery.firstMeasurementFlag == false) {
        display->setTextSize(2);
        display->println(F("  DISARMED"));

        display->setTextSize(1);
        display->println(F("  Waiting for drone\n voltage measurement"));
    } 
    else {
        if(cps3->FlightMode == DISARMED) {
            display->setTextSize(2);
            if (cps3->gripper.enabled == false){
                display->println(" DISARMED");
            }
            else{
                display->println("DISARMED G");
            }
            
        }
        else {
            display->setTextSize(2);
            if (cps3->gripper.enabled == false){
                display->println(F("   ARMED"));
            }
            else{
                display->println(F(" ARMED  G"));
            }
            display->setTextSize(1);
            if(remote->SwitchLeft_state == HIGH){
                display->println(F("     Speed: HIGH"));
            }
            else{
                display->println(F("     Speed: LOW"));
            }
        }
        display->setTextSize(1);
        switch (EEPROM.read(EEPROM_ADDR_VOLTAGE_MENU)) {
            case 0: // Display percentage voltage of remote and drone
            display->print(F("   Drone Battery:"));
                display->print(cps3->DroneBattery.Percentage);
                display->println(F("%"));
                break;

            case 1:
                display->print(F("  Remote battery: "));
                display->print(remote->Battery.percents);
                display->println(F("%"));
                break;

            case 2: // Display total voltage of drone and remote voltage
                display->print(F("Remote Voltage: "));
                display->print(remote->Battery.voltage, 2);
                display->println(F("V"));
                break;
            case 3:
                display->print(F(" Drone Voltage: "));
                display->print(cps3->DroneBattery.VoltageTotal, 2);
                display->println(F("V"));
                break;

            default:
                break;
        }
    }
    
    display->display();
}