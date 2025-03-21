//Display definitions:
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "logo.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Joystick sticks:
#define StickLeftH A2
#define StickLeftV A1
#define StickRightH A3
#define StickRightV A6
//Joystick buttons:
#define StickLeftB 11
#define StickRightB 12

//Remote battery voltage measurement:
#define BatterySensor A0

//Toggle switches:
#define SwitchRight 9
#define SwitchLeft 2

//RS485 enable pins:
#define MasterEnable 3

//Modes definitions
#define ARMED HIGH
#define DISARMED LOW

//Buttons module definitions
#define Button1 10
#define Button2 8
#define Button3 7
#define Button4 6
#define Button5 5

//Buzzer
#define Buzzer 13

//Motor speed limits
#define MAX_SPEED 140
#define MIN_SPEED 40

//time counter variables for refreshing stuff at intervals:
unsigned long previousMillis = 0; // Stores the last time the LED was updated
const long interval = 250;

//Sensor value variable declarations:
int StickLeftH_value, StickLeftV_value, StickRightH_value, StickRightV_value;

//Switch state variable declarations:
bool SwitchRight_state = LOW; // This determines whether the drone is armed or disarmed
bool SwitchLeft_state = LOW;  // This determines if the speed mode is 50% or 100%

bool flight_mode = DISARMED;
bool speed_mode = LOW;

//Drone and remote voltages and temperature variables:
float BatteryVoltage, DroneVoltage1, DroneVoltage2, DroneTemperature;

//Output motor value variables:
int SpeedL, SpeedR, SpeedA;

//Function for updating the display:
void update_display() {
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner

  //Display mode:
  display.setTextSize(1); 
  (flight_mode) == ARMED ? display.println(F("      mode:ARMED")) : display.println(F("    mode:DISARMED"));

  //Display speed:
  (speed_mode == LOW) ? display.println(F("     Speed: LOW")) : display.println(F("     Speed: HIGH"));

  //Display the drone voltages:
  display.print(String("Vd1:") + DroneVoltage1 + "V   ");
  display.println(String("Vd2:") + DroneVoltage2 + "V");

  //Display temperature and battery voltage of the remote: 
  display.print(String("Vr:") + BatteryVoltage + "V   ");
  display.println(String("Temp:") + DroneTemperature + "C");

  display.display();
}
  
void setup() {
  pinMode(SwitchRight, INPUT);
  pinMode(SwitchLeft, INPUT);
  pinMode(MasterEnable, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(StickLeftB, INPUT);
  pinMode(StickRightB, INPUT);
  pinMode(Button1, INPUT);
  pinMode(Button2, INPUT);
  pinMode(Button3, INPUT);
  pinMode(Button4, INPUT);
  pinMode(Button5, INPUT);
  
  // Display startup sequence
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();

  // More Beautiful CPS 3 logo
  display.drawBitmap(0, 0, cps3_logo, 128, 32, SSD1306_WHITE);

  display.display();
  delay(3000);

  // Clear the display after showing the logo
  display.clearDisplay();
  display.display(); // Ensure the display is updated

  Serial.begin(115200);
}

void loop() {

  unsigned long currentMillis = millis(); // Get the current "time" (actually the number of milliseconds since the program started)
  
  //Display code:
  update_display();

  //Read the joystick values and map them to variables
  StickLeftH_value = map(analogRead(StickLeftH), 0, 1023, -90, 90);
  StickLeftV_value = map(analogRead(StickLeftV), 0, 1023, -90, 90);
  StickRightH_value = map(analogRead(StickRightH), 0, 1023, -90, 90);
  StickRightV_value = map(analogRead(StickRightV), 0, 1023, 90, -90);

  // read the switches and map them to variables
  SwitchRight_state = digitalRead(SwitchRight);
  flight_mode = SwitchRight_state;
  
  SwitchLeft_state = digitalRead(SwitchLeft);
  speed_mode = SwitchLeft_state;

  //read the battery voltage and save it to proper variable, but only two times per second
  if (currentMillis - previousMillis >= interval) {
    // Save the last time you blinked the LED
    previousMillis = currentMillis;
    BatteryVoltage = map(analogRead(BatterySensor), 0, 1023, 0, 500);
    BatteryVoltage = BatteryVoltage/100;
  }

  //Figure out the proper motor speeds for the drone
  SpeedA = StickLeftV_value;
  
  // Calculate motor speeds based on joystick inputs
  if(StickRightV_value > 5 && StickRightV_value <= 45) { //Slowly moving forward
    if(StickLeftH_value < -5){ //Turning left
      StickLeftH_value = -StickLeftH_value;
      SpeedL = (StickRightV_value - 2 * StickLeftH_value) / 3;
      SpeedR = (StickRightV_value + 2 * StickLeftH_value) / 3;
    }
    else if (StickLeftH_value > 5) { //Turning right
      SpeedL = (StickRightV_value + 2 * StickLeftH_value) / 3;
      SpeedR = (StickRightV_value - 2 * StickLeftH_value) / 3;
    }
    else{ //No turning (dead zone)
      SpeedL = StickRightV_value;
      SpeedR = StickRightV_value;
    }
  } else if(StickRightV_value > 45) { //Fast moving forward
    if(StickLeftH_value < -5){ //Turning left
      StickLeftH_value = -StickLeftH_value;
      SpeedL = (2 * StickRightV_value - StickLeftH_value) / 3;
      SpeedR = (2 * StickRightV_value + StickLeftH_value) / 3;
    }
    else if (StickLeftH_value > 5) { //Turning right
      SpeedL = (2 * StickRightV_value + StickLeftH_value) / 3;
      SpeedR = (2 * StickRightV_value - StickLeftH_value) / 3;
    }
    else{ //No turning (dead zone)
      SpeedL = StickRightV_value;
      SpeedR = StickRightV_value;
    }
  } 
  else if (StickRightV_value < -5 && StickRightV_value >= -45) { //Slowly moving backwards
    if(StickLeftH_value < -5){ //Turning left
      SpeedL = (StickRightV_value - 2 * StickLeftH_value) / 3;
      SpeedR = (StickRightV_value + 2 * StickLeftH_value) / 3;
    }
    else if (StickLeftH_value > 5) { //Turning right
      StickLeftH_value = -StickLeftH_value;
      SpeedL = (StickRightV_value + 2 * StickLeftH_value) / 3;
      SpeedR = (StickRightV_value - 2 * StickLeftH_value) / 3;
    }
    else{ //No turning (dead zone)
      SpeedL = StickRightV_value;
      SpeedR = StickRightV_value;
    }
  } 
  else if (StickRightV_value < -45) { //Fast moving backwards
    if(StickLeftH_value < -5){ //Turning left
      SpeedL = (2 * StickRightV_value - StickLeftH_value) / 3;
      SpeedR = (2 * StickRightV_value + StickLeftH_value) / 3;
    }
    else if (StickLeftH_value > 5) { //Turning right
      StickLeftH_value = -StickLeftH_value;
      SpeedL = (2 * StickRightV_value + StickLeftH_value) / 3;
      SpeedR = (2 * StickRightV_value - StickLeftH_value) / 3;
    }
    else{ //No turning (dead zone)
      SpeedL = StickRightV_value;
      SpeedR = StickRightV_value;
    }
  } 
  else if(StickRightV_value >= -5 && StickRightV_value <= 5) { //No forward or backward movement
    if(StickLeftH_value < -5){ //Turning left
      StickLeftH_value = -StickLeftH_value;
      SpeedL = -StickLeftH_value;
      SpeedR = StickLeftH_value;
    }
    else if (StickLeftH_value > 5) { //Turning right
      SpeedL = StickLeftH_value;
      SpeedR = -StickLeftH_value;
    }
  }

  // Limit the speeds if speed mode is set to LOW
  if (speed_mode == LOW) {
    SpeedA/=2;
    SpeedL/=2;
    SpeedR/=2;
  }

  SpeedL = map(SpeedL, -90, 90, MIN_SPEED, MAX_SPEED);
  SpeedR = map(SpeedR, -90, 90, MIN_SPEED, MAX_SPEED);
  SpeedA = map(SpeedA, -90, 90, MIN_SPEED, MAX_SPEED);

  //Send the joystick outputs to the drone if it's armed
  if (flight_mode == ARMED) { 
    digitalWrite(MasterEnable, HIGH);
    Serial.print(String("\nL") + SpeedL + "R" + SpeedR + "A" + SpeedA);
    
    Serial.flush();

    // to make sure that after sending data, the data is equal to 0
    SpeedL = 0;
    SpeedR = 0;
    SpeedA = 0;
    }
  else {
    digitalWrite(MasterEnable, HIGH);
    Serial.print("\nd");
    Serial.flush();

    while (flight_mode == DISARMED) {      // While the drone is disarmed we are only reading the battery voltage from the drone and displaying it
    
      update_display();
      digitalWrite(MasterEnable, LOW);

      //Update the flight modes again, eventually a function for this is required
      flight_mode = digitalRead(SwitchRight);
      speed_mode = digitalRead(SwitchLeft);
      
      if (Serial.available() > 0) {
          String data;
          data = Serial.readStringUntil('\n');

          // Now, we need to get this data displayed on the screen, for now I'll write it to a terminal
          int firstComma = data.indexOf(',');
          int secondComma = data.indexOf(',', firstComma + 1);

          DroneVoltage1 = data.substring(0, firstComma).toFloat();
          DroneVoltage2 = data.substring(firstComma + 1, secondComma).toFloat();
          DroneTemperature = data.substring(secondComma + 1).toFloat();        
          }
      }
  }
}
