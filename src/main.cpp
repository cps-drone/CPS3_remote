//Display definitions:
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PWM.h>
#include "Logo.h"
#include "Motors_Speed.h"
#include "Display_OLED.h"
#include "Definitions.h"
#include "Measurement.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //OLED display object

MotorsSpeed motors_speed; //Motors speed object

//time counter variables for refreshing stuff at intervals:
unsigned long previousTime = 0; //Timer variables
const long interval = 1000; //Time interval
unsigned long currentTime = 1000; //Current time

//Sensor value variable declarations:
int StickLeftH_value, StickLeftV_value, StickRightH_value, StickRightV_value;

//Battery voltage and temperature variable declarations:
float DroneVoltage1 = 0.00;
float DroneVoltage2 = 0.00;
float DroneVoltageTotal = 0.00;
float DroneTemperature  = 0.00;
int DroneBatteryPercent = 0;
int RemoteBatteryPercent = 0;

//Switch state variable declarations:
bool SwitchRight_state = LOW; // This determines whether the drone is armed or disarmed
bool SwitchLeft_state = LOW;  // This determines if the speed mode is 50% or 100%

bool speed_mode = LOW;
volatile bool flight_mode = DISARMED;

//Battery voltage variable declaration:
float BatteryVoltage = 0.00;
float DroneVoltage = 0.00;

//output motor value declaration:
int SpeedL = 0;
int SpeedR = 0;
int SpeedA = 0;

//Function prototypes:
void update_display();
void receive_measurement_data();
void calculate_batteries_percentage();

//FUNCTIONS BODIES:

//Function to calculate the battery percentage:
void calculate_batteries_percentage() {
  //map method need int arguments
  DroneBatteryPercent = map(int(DroneVoltageTotal * 100), 580, 840, 0, 100);
  RemoteBatteryPercent = map(int(BatteryVoltage * 100), 300, 420, 0, 100);
  if (DroneBatteryPercent < 0) {
    DroneBatteryPercent = 0;
  }
  if (RemoteBatteryPercent < 0) {
    RemoteBatteryPercent = 0;
  }
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
  //WRONG PWM PIN
  // InitTimersSafe();
  // SetPinFrequencySafe(Buzzer, BuzzerPWMFrequency);

  //Set the pins as inputs or outputs
  pinMode(SwitchRight, INPUT);
  pinMode(SwitchLeft, INPUT);
  pinMode(MasterEnable, OUTPUT);
  //pinMode(Buzzer, OUTPUT); //WRONG PWM PIN
  pinMode(StickLeftB, INPUT);
  pinMode(StickRightB, INPUT);
  pinMode(Button1, INPUT);
  pinMode(Button2, INPUT);
  pinMode(Button3, INPUT);
  pinMode(Button4, INPUT);
  pinMode(Button5, INPUT);

  //Display initialization
  init_display();
  Serial.begin(115200); //Set the baud rate for the serial communication
}

void loop() {

  //Read the battery voltage and save it to proper variable every interval
  if (currentTime - previousTime >= interval) {
    previousTime = currentTime;
    BatteryVoltage = map(analogRead(BatterySensor), 0, 1023, 0, 500);
    BatteryVoltage = BatteryVoltage/100;
    calculate_batteries_percentage();
  }
  currentTime = millis(); //Initialize the current time variable

  update_display();  //Display data on the OLED screen

  //Read the joystick values and map them to variables
  StickLeftH_value = map(analogRead(StickLeftH), 0, 1023, -90, 90);
  StickLeftV_value = map(analogRead(StickLeftV), 0, 1023, -90, 90);
  StickRightH_value = map(analogRead(StickRightH), 0, 1023, -90, 90);
  StickRightV_value = map(analogRead(StickRightV), 0, 1023, 90, -90);

  //Read the switches and map them to variables
  SwitchRight_state = digitalRead(SwitchRight);
  flight_mode = SwitchRight_state;
  SwitchLeft_state = digitalRead(SwitchLeft);
  speed_mode = SwitchLeft_state;

  //Set the speeds of the motors based on the joystick values
  motors_speed.setSpeedA(StickLeftV_value);
  motors_speed.setSpeedsLR(StickLeftH_value, StickRightV_value);
  speed_mode == LOW ? motors_speed.setSlowMode() : (void)0;
  motors_speed.mapSpeeds();

  //Send calculated motors speeds to the drone if it's armed
  if (flight_mode == ARMED) { 
    digitalWrite(MasterEnable, HIGH);
    Serial.print(String("\nL") + motors_speed.getSpeedL() + "R" + motors_speed.getSpeedR() + "A" + motors_speed.getSpeedA());
    Serial.flush();

    // to make sure that after sending data, the data is equal to 0
    motors_speed.setZeroSpeed();
    }

  //If drone is disarmed, get the measurement data from the drone
  else {
    digitalWrite(MasterEnable, HIGH);
    Serial.print("\nd");
    Serial.flush();

    digitalWrite(MasterEnable, LOW);
    receive_measurement_data();
    DroneVoltageTotal = DroneVoltage1 + DroneVoltage2;

    //WRONG PWM PIN
    // if (DroneVoltageTotal < 6.0 || DroneVoltageTotal > 8.4 || BatteryVoltage < 3.0 || DroneTemperature > 50) {
    //   pwmWrite(Buzzer, 128);
    // }
    // else {
    //   pwmWrite(Buzzer, 0);
    // }
    //update_display();
    //delay(50);
  }
}
