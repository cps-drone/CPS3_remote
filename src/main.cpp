//Display definitions:
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Logo.h"
#include "Motors_Speed.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

MotorsSpeed motors_speed;
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

//time counter variables for refreshing stuff at intervals:
unsigned long previousMillis = 0; // Stores the last time the LED was updated
const long interval = 500; // Interval at which to blink (milliseconds)

//Sensor value variable declarations:
int StickLeftH_value, StickLeftV_value, StickRightH_value, StickRightV_value;

//Switch state variable declarations:
bool SwitchRight_state = LOW; // This determines whether the drone is armed or disarmed
bool SwitchLeft_state = LOW;  // This determines if the speed mode is 50% or 100%

volatile bool flight_mode = DISARMED;
volatile bool sendDataFlag = false;

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
  receive_measurement_data();
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
  
//Function to receive measurement data
void receive_measurement_data() {
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');

        //Looking for the indexes of the battery voltage and temperature data
        int bat1sIndex = data.indexOf("BAT1S");
        int bat2sIndex = data.indexOf("BAT2S");
        int tempIndex = data.indexOf("TEMP");

        //Confirm that all the data is present
        if (bat1sIndex != -1 && bat2sIndex != -1 && tempIndex != -1) {
            //Extract the values from the data
            String bat1sValue = data.substring(bat1sIndex + 5, bat2sIndex);
            String bat2sValue = data.substring(bat2sIndex + 5, tempIndex);  
            String tempValue = data.substring(tempIndex + 4);             

            //Convert the values to floats
            DroneVoltage1 = bat1sValue.toFloat();
            DroneVoltage2 = bat2sValue.toFloat();
            DroneTemperature = tempValue.toFloat();
        }
    }
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

  unsigned long currentTime = millis(); // Get the current "time"

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
  if (currentTime - previousTime >= interval) {
    previousTime = currentTime;
    BatteryVoltage = map(analogRead(BatterySensor), 0, 1023, 0, 500);
    BatteryVoltage = BatteryVoltage/100;
  }

  //Set the speeds of the motors based on the joystick values
  motors_speed.setSpeedA(StickLeftV_value);
  motors_speed.setSpeedsLR(StickLeftH_value, StickRightV_value);
  speed_mode == LOW ? motors_speed.setSlowMode() : (void)0;
  motors_speed.mapSpeeds();

  //Send the joystick outputs to the drone if it's armed
  if (flight_mode == ARMED) { 
    digitalWrite(MasterEnable, HIGH);
    Serial.print(String("\nL") + motors_speed.getSpeedL() + "R" + motors_speed.getSpeedR() + "A" + motors_speed.getSpeedA());
    Serial.flush();

    // to make sure that after sending data, the data is equal to 0
    motors_speed.setZeroSpeed();
    }

  else {
    digitalWrite(MasterEnable, HIGH);
    Serial.print("\nd");
    Serial.flush();

    digitalWrite(MasterEnable, LOW);

    receive_measurement_data();
    update_display();
    delay(50);
    //Update the flight modes again, eventually a function for this is required
    // flight_mode = digitalRead(SwitchRight);
    // speed_mode = digitalRead(SwitchLeft);
  }
}
