#include "Display_OLED.h"

void init_display(){
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        for(;;); // Don't proceed, loop forever
      }
      display.clearDisplay();
      //The most beautiful CPS logo
      display.drawBitmap(0, 0, cps3_logo, 128, 32, SSD1306_WHITE);
    
      display.display();
      delay(5000);
      display.clearDisplay();  //Clear the display after showing the logo
      display.display(); //Ensure the display is updated

}

void update_display() {
    receive_measurement_data();
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, 0);            // Start at top-left corner

    // Check if Button5 is pressed to enter the motor direction menu
    if (digitalRead(Button5) == LOW && flight_mode == ARMED) {
        static char selectedMotor = 'A'; // Default motor selection

        // Handle motor selection using Button4
        if (digitalRead(Button4) == LOW) {
            delay(200); // Debounce
            if (selectedMotor == 'A') {
                selectedMotor = 'L';
            } else if (selectedMotor == 'L') {
                selectedMotor = 'R';
            } else if (selectedMotor == 'R') {
                selectedMotor = 'A';
            }
        }

        // Handle direction change using Button3
        if (digitalRead(Button3) == LOW) {
            delay(200); // Debounce
            if (selectedMotor == 'A') {
                motorDirectionA = !motorDirectionA;
                EEPROM.update(EEPROM_ADDR_MOTOR_A, motorDirectionA); // Zapisz do EEPROM
            } else if (selectedMotor == 'L') {
                motorDirectionL = !motorDirectionL;
                EEPROM.update(EEPROM_ADDR_MOTOR_L, motorDirectionL); // Zapisz do EEPROM
            } else if (selectedMotor == 'R') {
                motorDirectionR = !motorDirectionR;
                EEPROM.update(EEPROM_ADDR_MOTOR_R, motorDirectionR); // Zapisz do EEPROM
            }
        }

        // Display the motor direction menu
        display.clearDisplay();
        display.setTextSize(1);
        display.println(F("Motor Direction Menu"));
        display.print(F(" Selected motor: "));
        display.println(selectedMotor);
        display.println("-------------------");
        display.print(F("  A: "));
        display.print(motorDirectionA ? F("1") : F("0"));
        display.print(F(", L: "));
        display.print(motorDirectionL ? F("1") : F("0"));
        display.print(F(", R: "));
        display.println(motorDirectionR ? F("1") : F("0"));
        
        display.display();
        return; // Exit the function to avoid other display updates
    }

    // Normal display logic (e.g., flight mode, speed, etc.)
    display.setTextSize(2);
    (flight_mode == ARMED) ? display.println(F("   ARMED")) : display.println(F(" DISARMED"));

    display.setTextSize(1);

    if(firstMeasurementFlag == false){
        display.println(F("  Waiting for drone\n voltage measurement"));
        display.display();
    }

    if (speed_mode == LOW && flight_mode == ARMED) {
        display.println(F("      Speed:LOW"));
    } else if (speed_mode == HIGH && flight_mode == ARMED) {
        display.println(F("     Speed: HIGH"));
    }
   
    if (flight_mode == ARMED) {
        display.println(String("Remote battery: ") + RemoteBatteryPercent + "%");
    }

    if(flight_mode == DISARMED){
        if (digitalRead(Button1) == LOW) {
            display.println(String("  1S voltage: ") + DroneVoltage1 + "V");
            display.print(String("  2S voltage: ") + DroneVoltage2 + "V  ");  
        }

        // Display StickRightH_value and StickRightV_value if StickRightB is LOW
        if (digitalRead(StickRightB) == LOW) {
            display.setTextSize(1);
            display.print(F("Right: "));
            display.print(F("H= "));
            display.print(StickRightH_value);
            display.print(F(", V= "));
            display.println(StickRightV_value);
            display.println(F(""));
        }
        // Display StickLeftH_value and StickLeftV_value if StickLeftB is LOW
        if (digitalRead(StickLeftB) == LOW) {
            display.setTextSize(1);
            display.print(F("Left: "));
            display.print(F("H= "));
            display.print(StickLeftH_value);
            display.print(F(", V= "));
            display.println(StickLeftV_value);
            display.println(F(""));
        }

        else {
            // if(connectionFlag == true){
                display.println(String("Drone voltage: ") + DroneVoltageTotal + "V");
                display.println(String("  Drone state: ") + DroneBatteryPercent + "%");
            // }
        }
    }

    display.display(); // Push the data to the RAM of the display
}
