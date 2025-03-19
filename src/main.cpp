//Display definitions:
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//Joystick pins:
#define StickLeftY A3
#define StickLeftX A2
#define StickRightY A6
#define StickRightX A1

//battery pin
#define BatterySensor A0

//Switch pins:
#define SwitchRight 8
#define SwitchLeft 9

//RS485 enable pins:
#define MasterEnable 3

// Mode definitions
#define ARMED HIGH
#define DISARMED LOW

//time counter variables for refreshing stuff at intervals:
unsigned long previousMillis = 0; // Stores the last time the LED was updated
const long interval = 500; // Interval at which to blink (milliseconds)

//Sensor value variable declarations:
int StickLeftX_value = 0;
int StickLeftY_value = 0;
int StickRightY_value = 0;
int StickRightX_value = 0;

//Switch state variable declarations:
bool SwitchRight_state = LOW; // This determines whether the drone is armed or disarmed
bool SwitchLeft_state = LOW;  // This determines if the speed mode is 50% or 100%

bool flight_mode = DISARMED;
bool speed_mode = LOW;

//Battery voltage variable declaration:
float BatteryVoltage = 0.00;
float DroneVoltage = 0.00;


//output motor value declaration:
int SpeedL = 0;
int SpeedR = 0;
int SpeedA = 0;

//Function for updating the display:
void update_display() {
  display.clearDisplay();

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner

  //Display mode:
  if (flight_mode == ARMED){
    display.println(F("   ARMED"));
  }

  else {
    display.setCursor(6,0);
    display.println(F(" DISARMED"));
  }
  
  //Display battery voltage of the remote:
  display.setTextSize(1);  
  display.print(F("Vr: "));
  display.print(BatteryVoltage);

  //Display the drone voltage:
  display.print(F("    Vd: "));
  display.println(DroneVoltage);
  
  //Display speed:

  if (speed_mode == HIGH){
    display.println(F("     Speed: HIGH"));
  }

  else {
    display.println(F("     Speed:  LOW"));
  }

  display.display();
}

void setup() {

  pinMode(SwitchRight, INPUT);
  pinMode(SwitchLeft, INPUT);
  pinMode(MasterEnable, OUTPUT);

  // Display startup sequence
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();

    //Beautiful CPS 3 logo
  display.setTextSize(4);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(6,2);
  display.println(F("CPS 3"));
  display.display();
  delay(3000);
    
  Serial.begin(115200);
  
}

void loop() {

  unsigned long currentMillis = millis(); // Get the current "time" (actually the number of milliseconds since the program started)
  
  //Display code:
  update_display();
  
  // read the joysticks and map them to variables
  StickLeftX_value = map(analogRead(StickLeftX), 0, 1023, 140, 40);
  StickLeftY_value = map(analogRead(StickLeftY), 0, 1023, 140, 40);
  StickRightY_value = map(analogRead(StickRightY), 0, 1023, 140, 40);
  StickRightX_value = map(analogRead(StickRightX), 0, 1023, 140, 40);

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
  
  SpeedA = (180-StickLeftY_value + 1);
  
  if (StickRightY_value > 90) {
    SpeedL = (StickRightY_value+(StickLeftX_value-90));
    SpeedR = (StickRightY_value-(StickLeftX_value-90));
  }
  else {
    SpeedL = (StickRightY_value-(StickLeftX_value-90));
    SpeedR = (StickRightY_value+(StickLeftX_value-90));
  }


  // limit the output values, which are above the limit for the motor
  if (SpeedL >= 140) {
    SpeedL = 140;
  }

  if (SpeedL <= 40) {
    SpeedL = 40;
  }
  
  if (SpeedR >= 140) {
    SpeedR = 140;
  }

  if (SpeedR <= 40) {
    SpeedR = 40;
  }


  // limit the speeds if speed mode is set to LOW
  if (speed_mode == LOW) {
    SpeedA = (SpeedA-90) / 2 + 90;
    SpeedL = (SpeedL-90) / 2 + 90;
    SpeedR = (SpeedR-90) / 2 + 90;
  }
  


  //Send the joystick outputs to the drone if it's armed
  if (flight_mode == ARMED) { 
    
    digitalWrite(MasterEnable, HIGH);
    
    Serial.print("\nL");
    Serial.print(SpeedL);
  
    Serial.print("R");
    Serial.print(SpeedR);
    
    Serial.print("A");
    Serial.print(SpeedA);
  
    Serial.flush();
    
     }

  else {

    digitalWrite(MasterEnable, HIGH);
    
    Serial.print("\nd");
    Serial.flush();

    while (flight_mode == DISARMED) {
    
      // While the drone is disarmed we are only reading the battery voltage from the drone and displaying it

      update_display();
      digitalWrite(MasterEnable, LOW);

      //Update the flight modes again, eventually a function for this is required
      SwitchRight_state = digitalRead(SwitchRight);
      flight_mode = SwitchRight_state;

      SwitchLeft_state = digitalRead(SwitchLeft);
      speed_mode = SwitchLeft_state;
      
      if (Serial.available() > 0) {
          String data;
          data = Serial.readStringUntil('\n');

          // Now, we need to get this data displayed on the screen, for now I'll write it to a terminal
          DroneVoltage = data.toFloat();

          Serial.print("\n");
          Serial.print(DroneVoltage);
          delay(100); //delay to make sure the battery voltage doesn't get read at random intervals
          
          }
     }
  }
}