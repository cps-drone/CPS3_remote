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

void update_display(){
    receive_measurement_data();
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0,0);             // Start at top-left corner
    
    //Display mode:
    display.setTextSize(2); 
    (flight_mode) == ARMED ? display.println(F("   ARMED")) : display.println(F(" DISARMED"));
    
    display.setTextSize(1); 
    //Display speed:
    if(speed_mode == LOW && flight_mode == ARMED){
        display.println(F("      Speed:LOW"));
    }
    else if(speed_mode == HIGH && flight_mode == ARMED){
        display.println(F("     Speed: HIGH"));
    }
    else{}
    //Display remote battery state in %
    if(flight_mode == ARMED){
        display.println(String("Remote battery: ") + RemoteBatteryPercent + "%");
    }
    //Display drone battery cells voltages
    if(flight_mode == DISARMED && digitalRead(Button1) == LOW) {
        display.println(String("  1S voltage: ") + DroneVoltage1 + "V");
        display.print(String("  2S voltage: ") + DroneVoltage2 + "V ");
    }
    //Display drone temperature in Celsius
    if(flight_mode == DISARMED && digitalRead(Button2) == LOW) {
        display.println(" Drone temperature");
        display.println(String("       ") + DroneTemperature + "C");
    }
    //Display drone total battery voltage and state in %
    if (flight_mode == DISARMED && digitalRead(Button2) == HIGH && digitalRead(Button1) == HIGH){
        display.println(String("Drone voltage: ") + DroneVoltageTotal + "V");
        display.println(String("  Drone state: ") + DroneBatteryPercent + "%");
    }
    
    display.display(); //Push the data to the RAM of the display
}