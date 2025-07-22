//Display definitions:
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include "Logo.h"
#include "Display_OLED.h"
#include "Definitions.h"
#include "Remote.h"
#include "CPS3_drone.h"
#include "Buzzer.h"

/*
  * Display, remote, CPS3 drone and buzzer struct objects
*/
Adafruit_SSD1306 Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // OLED display object
remote_t Remote;  // Remote struct object
cps3_t CPS3;      // CPS3 drone struct object
buzzer_t Buzzer;  // Buzzer struct object

void setup() {
  /*
    * Initialize OLED display, CPS3 drone, remote and buzzer struct objects
  */
  cps3_init(&CPS3);     // Initialize the CPS3 drone struct object
  remote_init(&Remote); // Initialize the remote struct object
  buzzer_init(&Buzzer, BUZZER_PIN, BUZZER_FREQUENCY); // Initialize the buzzer struct object
  display_init(&Display);       // Initialize the OLED display
  CPS3.master_mode = RS485_MASTER_MODE; // Set the CPS3 drone to RS485 slave mode
}

/*
  * Main loop function
*/
void loop() {
  //Remote.currentSendTime = millis(); // Update the current time variable
  Remote.Battery.currentTime = millis(); // Update the current time variable for the remote battery
  Buzzer.currentMillis = millis();

  //Read the battery voltage and save it to proper variable every interval
  if (Remote.Battery.currentTime - Remote.Battery.previousTime >= Remote.Battery.interval) {
    Remote.Battery.previousTime = Remote.Battery.currentTime; //Update the previous time variable
    remote_voltage_read(&Remote); //Read the remote battery voltage
  }

  reset_buttons_states(&Remote); //Reset the buttons states
  buttons_module_read(&Remote); //Read the remote buttons only

  while (Remote.Buttons.Button2_state == PRESSED){
    display_test_mode(&Display, &Remote); //Display the test mode on the OLED screen
    Display.display(); // Ensure the display is updated

    if (digitalRead(BUTTON2) == PRESSED) {
      delay(DEBOUNCE); // Button Debounce
      reset_buttons_states(&Remote); // Reset the buttons states
      Display.clearDisplay(); // Clear the display when exiting test mode
      Display.display(); // Ensure the display is updated
    }
  }
  
  buttons_module_read(&Remote); //Read the remote buttons only

  while (Remote.Buttons.Button5_state == PRESSED){
    display_motor_direction_menu(&Display, &CPS3, &Remote); // Call the motor direction menu function
    Display.display(); // Ensure the display is updated

    if (digitalRead(BUTTON5) == PRESSED) {
      delay(DEBOUNCE); // Button Debounce
      reset_buttons_states(&Remote); // Reset the buttons states
      Display.clearDisplay(); // Clear the display when exiting test mode
      Display.display(); // Ensure the display is updated
    }
  }

  display_switch_voltage_menu();
  update_display(&Display, &Remote, &CPS3);  //Display data on the OLED screen
  set_cps3_flight_mode(&CPS3, &Remote); //Set the flight mode based on the left switch state

      // Read the joystick, switches and buttons values from the remote
  remote_read(&Remote);
  set_cps3_motors_speed(&CPS3, &Remote); //Set the speed mode based on the right switch state

  //Remote.previousSendTime = Remote.currentSendTime; //Update the previous time variable
  if(CPS3.master_mode == RS485_SLAVE_MODE) {
    send_cps3_motors_speed(&Remote, &CPS3); //Send the data to the drone
  } 
  else if(CPS3.master_mode == RS485_MASTER_MODE) {
    get_cps3_battery_state(&CPS3); //Get the battery state of the CPS3 drone
  }

  // Checking the drone and the remote battery voltage, if the voltage is below the warning level, the buzzer will toggle
  if((CPS3.DroneBattery.VoltageTotal < CPS3_LOW_VOLTAGE_WARNING || Remote.Battery.voltage < REMOTE_LOW_VOLTAGE_WARNING) && CPS3.DroneBattery.firstMeasurementFlag == true){
    buzzerToggle(&Buzzer);
  }
  else{
    Buzzer.state = false;
    buzzerDisable(&Buzzer);
  }
  
}