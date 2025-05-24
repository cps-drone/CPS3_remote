//Display definitions:
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <Arduino.h>
#include "Logo.h"
#include "Motors_Speed.h"
#include "Display_OLED.h"
#include "Definitions.h"
#include "Measurement.h"
#include "Buzzer.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //OLED display object

MotorsSpeed motors_speed; //Motors speed object

//time counter variables for refreshing stuff at intervals:
unsigned long previousTime = 0; //Timer variables
const long setupInterval = 100; //Time interval
const long loopInterval = 1000; //Time interval
unsigned long currentTime = 0; //Current time

unsigned long previousSendTime = 0; //Timer variables
const long sendingInterval = 100; //Time interval
unsigned long currentSendTime = 100; //Current time

bool connectionFlag = false; //Flag to check if the drone is connected
uint8_t connectionDisplayCtr = 0;
bool firstMeasurementFlag = false; //Flag to check if the first measurement is received

//Sensor value variable declarations:
int StickLeftH_value, StickLeftV_value, StickRightH_value, StickRightV_value;

//Battery voltage and temperature variable declarations:
float DroneVoltage1 = 0.00;
float DroneVoltage2 = 0.00;
float DroneVoltageTotal = 0.00;
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

bool motorDirectionA = false;
bool motorDirectionL = false;
bool motorDirectionR = false;

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
  else if (DroneBatteryPercent > 100) {
    DroneBatteryPercent = 100;
  }
  if (RemoteBatteryPercent < 0) {
    RemoteBatteryPercent = 0;
  }
  else if (RemoteBatteryPercent > 100) {
    RemoteBatteryPercent = 100;
  }
  
  
}
  
//Function to receive measurement data
void receive_measurement_data() {
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');

        //Looking for the indexes of the battery voltage and temperature data
        int bat1sIndex = data.indexOf("BAT1S");
        int bat2sIndex = data.indexOf("BAT2S");
        //Confirm that all the data is present
        if (bat1sIndex != -1 && bat2sIndex != -1) {
            //Extract the values from the data
            String bat1sValue = data.substring(bat1sIndex + 5, ',');
            String bat2sValue = data.substring(bat2sIndex + 5, ',');  

            //Convert the values to floats
            DroneVoltage1 = bat1sValue.toFloat();
            DroneVoltage2 = bat2sValue.toFloat();
        }
    }
}

void setup() {

  motorDirectionA = EEPROM.read(EEPROM_ADDR_MOTOR_A);
  motorDirectionL = EEPROM.read(EEPROM_ADDR_MOTOR_L);
  motorDirectionR = EEPROM.read(EEPROM_ADDR_MOTOR_R);

  //Set the pins as inputs or outputs
  pinMode(SwitchRight, INPUT);
  pinMode(SwitchLeft, INPUT);
  pinMode(MasterEnable, OUTPUT);
  pinMode(StickLeftB, INPUT);
  pinMode(StickRightB, INPUT);
  pinMode(Button1, INPUT);
  pinMode(Button2, INPUT);
  pinMode(Button3, INPUT);
  pinMode(Button4, INPUT);
  pinMode(Button5, INPUT);
  //Display initialization
  init_display();
  initBuzzer();
  Serial.begin(9600); //Set the baud rate for the serial communication

  while (connectionFlag == false) {
    currentTime = millis(); //Initialize the current time variable
    if (currentTime - previousTime >= setupInterval) {
      previousTime = currentTime;
      digitalWrite(MasterEnable, HIGH);
      Serial.print('Q');
      Serial.flush();
      digitalWrite(MasterEnable, LOW);
      SwitchRight_state = digitalRead(SwitchRight);
      flight_mode = SwitchRight_state;
      SwitchLeft_state = digitalRead(SwitchLeft);
      speed_mode = SwitchLeft_state;
      if(flight_mode == ARMED){
        BatteryVoltage = map(analogRead(BatterySensor), 0, 1023, 0, 500);
        BatteryVoltage = BatteryVoltage/100;
        calculate_batteries_percentage();
      }

      if(flight_mode == DISARMED){
        //Read the joystick values and map them to variables
        StickLeftH_value = map(analogRead(StickLeftH), 0, 1023, -90, 90);
        StickLeftV_value = map(analogRead(StickLeftV), 0, 1023, -90, 90);
        StickRightH_value = map(analogRead(StickRightH), 0, 1023, -90, 90);
        StickRightV_value = map(analogRead(StickRightV), 0, 1023, 90, -90);
      }

      update_display();

      if(digitalRead(Button1) == LOW && digitalRead(Button2) == LOW) {
        connectionFlag = true;
      }

      //Check if the drone is connected
      if(Serial.available() > 0) {
        char respond = Serial.read();
        if (respond == 'C') {
          connectionFlag = true;
          
          currentTime = 0;
          previousTime = 0;
        }
      }
    }
  }
}

void loop() {

  if (connectionDisplayCtr <= 50){
    connectionDisplayCtr++;
    receive_measurement_data();
    DroneVoltageTotal = DroneVoltage1 + DroneVoltage2;
  }

  //Read the battery voltage and save it to proper variable every interval
  if (currentTime - previousTime >= loopInterval) {
    previousTime = currentTime;
    BatteryVoltage = map(analogRead(BatterySensor), 0, 1023, 0, 500);
    BatteryVoltage = BatteryVoltage/100;
    calculate_batteries_percentage();
  }
  currentTime = millis(); //Initialize the current time variable
  currentSendTime = millis(); //Initialize the current time variable

  update_display();  //Display data on the OLED screen

  //Read the joystick values and map them to variables
  StickLeftH_value = map(analogRead(StickLeftH), 0, 1023, -90, 90);
  StickLeftV_value = map(analogRead(StickLeftV), 0, 1023, -90, 90);
  StickRightH_value = map(analogRead(StickRightH), 0, 1023, -90, 90);
  StickRightV_value = map(analogRead(StickRightV), 0, 1023, 90, -90);
  

  //Read the switches and map them to variables
  SwitchRight_state = digitalRead(SwitchRight);
  (firstMeasurementFlag == false) ? flight_mode = DISARMED : flight_mode = SwitchRight_state;
  SwitchLeft_state = digitalRead(SwitchLeft);
  speed_mode = SwitchLeft_state;

  //Send calculated motors speeds to the drone if it's armed
  if (flight_mode == ARMED) { 

    if (currentSendTime - previousSendTime >= sendingInterval) {

      motors_speed.setSpeedA(StickLeftV_value);
      motors_speed.setSpeedsLR(StickLeftH_value, StickRightV_value);

      if(speed_mode == LOW) {
        motors_speed.setSlowMode();
      }

      motors_speed.mapSpeeds();

      // //Set the speeds of the motors based on the joystick values
      if(motorDirectionA == true){
        motors_speed.InvertMotorAdirection();
      }
      if(motorDirectionL == true){
        motors_speed.InvertMotorLdirection();
      }
      if(motorDirectionR == true){
        motors_speed.InvertMotorRdirection();
      }

      previousSendTime = currentSendTime;
      digitalWrite(MasterEnable, HIGH);
      Serial.print(String("\nL") + motors_speed.getSpeedL() + "R" + motors_speed.getSpeedR() + "A" + motors_speed.getSpeedA());
      Serial.flush();
      // to make sure that after sending data, the data is equal to 0
      motors_speed.setZeroSpeed();
    }
  }

  //If drone is disarmed, get the measurement data from the drone
  else {
    digitalWrite(MasterEnable, HIGH);
    Serial.print("d\n");
    Serial.flush();

    digitalWrite(MasterEnable, LOW);
    receive_measurement_data();
    DroneVoltageTotal = DroneVoltage1 + DroneVoltage2;
    if(DroneVoltageTotal != 0.00){
      firstMeasurementFlag = true;
    }
    update_display();
    delay(50);
  }

  if((DroneVoltage1 < 3.1 || DroneVoltage2 < 3.1 || RemoteBatteryPercent < 20) && connectionFlag == true && connectionDisplayCtr > 50 && firstMeasurementFlag == true) {
    toggleBuzzerNonBlocking();
  } else {
    disableBuzzer();
  }
}
