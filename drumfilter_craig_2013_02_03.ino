/* 
 Stepper Motor Control - speed control
 
 This program drives a unipolar or bipolar stepper motor. 
 The motor is attached to digital pins 8 - 11 of the Arduino.
 A potentiometer is connected to analog input 0.
 
 The motor will rotate in a clockwise direction. The higher the potentiometer value,
 the faster the motor speed. Because setSpeed() sets the delay between steps, 
 you may notice the motor is less responsive to changes in the sensor value at
 low speeds.
 
 Created 30 Nov. 2009
 Modified 28 Oct 2010
 by Tom Igoe
 
 */

#include <Stepper.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Time.h>
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor


// initialize the stepper library on pins 8 through 11:
//Stepper myStepper(stepsPerRevolution, 9,10,11,12);            
LiquidCrystal lcd(2, 3, 7, 6, 5, 4);
int stepCount = 0;  // number of steps the motor has taken

boolean bMotorOn = false;
boolean bPumpOn = false;
boolean bLowLevel = false;
boolean bStartMotor = false;
boolean bStartPump = false;
boolean bManualRun = false;
byte iTimeout = 15; //minutes until start of motor
boolean bSensorOn = true;
static int timeAddr = 0;
static int sensorOnAddr = 1;
int pumpPin = 11;
int motorPin = 13;
int sensorPin = 10;
int minutes;
boolean bLowLevelOld = false;
boolean bLowLevelNew = false;

void setup() {
  // nothing to do inside the setup
  Serial.begin(9600);
  lcd.begin(16,2);
  iTimeout = EEPROM.read(timeAddr);    
  if(iTimeout == 255) iTimeout = 5;
  bSensorOn = EEPROM.read(sensorOnAddr) == 1 ? true : false; 
  pinMode(pumpPin,OUTPUT);
  pinMode(motorPin,OUTPUT);
  pinMode(sensorPin,INPUT);
  pinMode(12,INPUT);
  minutes = minute();
  lcd.clear();
  lcd.setCursor(1,1);
  lcd.cursor();
  bLowLevel = digitalRead(sensorPin);
  bLowLevelNew = bLowLevel;
  bLowLevelOld = bLowLevelNew;
}

void updateDisplay(){
  lcd.setCursor(0,0);
  lcd.print("M:");
  lcd.print(bMotorOn ? 1 : 0);
  lcd.print("  P:");
  lcd.print(bPumpOn ? 1 : 0);
  lcd.print("  L:  ");
  lcd.print(bLowLevel ? 1 : 0);
  lcd.setCursor(0,1);
  if(!bSensorOn){
    lcd.print("Time:");
    lcd.print(iTimeout);
    if(iTimeout <10){
      lcd.print("  ");
    }
    else if(iTimeout < 100){
      lcd.print(" ");
    }
  }
  else{
    lcd.print("        ");
  }
  lcd.setCursor(8,1);
  lcd.print("  Lon:");
  lcd.print(bSensorOn);
}

long previousMillis = 0;        // will store last time LED was updated

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 100;           // interval at which to update LCD
unsigned long millisStart;
int i = 0;
int spe = 0;
int buttonState = 0;
int buttonStateOld = 0;
boolean bDisplayError = false;


void loop() {
  //bMotorOn = digitalRead(motorPin);
  //bPumpOn = digitalRead(pumpPin);
  /*if(!digitalRead(sensorPin)){
    bLowLevel = false;
    oneCount = 0;
  }
  else{
    oneCount++;  //On filter  
  }
  if(oneCount > 10000){
    oneCount = 26; //Just limit the variable
  }
  if(oneCount > 25){
    bLowLevel = true; //If on for more than 25 samples, it is truly on.
  }
  */
  /*
  if(!bMotorOn){
    bLowLevelOld = bLowLevelNew;
    bLowLevelNew = digitalRead(sensorPin);
    
    if(!bLowLevelNew){
      bLowLevel = false;
    }
    else if(bLowLevelNew != bLowLevelOld && bLowLevelNew){
      bLowLevel = bLowLevelNew;
    }
  }
  */
  // read the sensor value:
  bLowLevel = digitalRead(sensorPin);
  int sensorReading = analogRead(A0);
  // map it to a range from 0 to 100:
/*  int motorSpeed = map(sensorReading, 0, 1023, 100, 0);
  // set the motor speed:
  if (motorSpeed > 10) {    
    myStepper.setSpeed(motorSpeed);
    // step 1/100 of a revolution:
    myStepper.step(1);
  }
  else{
    myStepper.setSpeed(0);
    myStepper.step(0);
  }
  */
  //
  
  
    if(sensorReading > 1000 && sensorReading < 1040){ //Nothing pressed
      //spe = 0;
      buttonState = 0;
    }
    else if(sensorReading > 720 && sensorReading < 724){ //Select
      buttonState = 1;
    }
    else if(sensorReading > 476 && sensorReading < 484){ //Left
      buttonState = 2;
    }
    else if(sensorReading > 126 && sensorReading < 134){ //Up
      buttonState = 3;
    }
    else if(sensorReading > 300 && sensorReading < 310){ //Down
      buttonState = 4;
    }
    else if(sensorReading > 0 && sensorReading < 40){ //Right
      buttonState = 5;
    }
    else{
      bDisplayError = true;
    }
/*
    if((((lastTurn-minute()) > iTimeout) == 0) && !bSensorOn){
       bMotorOn = true;
       lastTurn = minute();
       millisStart = millis();
    }    
    else */

    
    if(bSensorOn){
       if(bLowLevel && !bMotorOn){         
         bMotorOn = true;
         bPumpOn = true;
         millisStart = millis();
       }
    }        
    if(bMotorOn && (millis()-millisStart) > 20000){
      millisStart = millis();
      bPumpOn = false;
      bMotorOn = false;
    }
    
  if(buttonState != buttonStateOld && buttonState != 0){
    switch(buttonState){
      case 1:
        bMotorOn = !bMotorOn;
        break;
      case 2:        
        Serial.println("Button 2");
        bPumpOn = !bPumpOn;
        break;
      case 3:
        if(!bSensorOn && iTimeout < 255){
          iTimeout++;  
          EEPROM.write(timeAddr, iTimeout);              
        }
        break;
      case 4:
        if(!bSensorOn && iTimeout > 0){
          iTimeout--;
          EEPROM.write(timeAddr, iTimeout);    
        }
        break;
      case 5:
        bSensorOn = !bSensorOn;
        EEPROM.write(sensorOnAddr, bSensorOn ? 1 : 0); 
        minutes = minute();
        break;
      case 6:
        break;
    }
  }
  
  digitalWrite(motorPin, bMotorOn);
  digitalWrite(pumpPin, bPumpOn);
  
  buttonStateOld = buttonState;
  
  unsigned long currentMillis = millis();
  /*if(currentMillis % 5000 == 0){
     lcd.clear();
  }*/
   
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   
    updateDisplay();    
    //lcd.clear();
    //lcd.setCursor(0,0);
    //lcd.print(sensorReading);

    
    
  }
  
  
//  delay(250);
  //lcd.clear();
  /*i++;
  if(i>300){
    i=0;
  }*/
//  int val = millis()%1000;
  //if(val >= 0 && val <= 1){
  //  
  //}
  //myStepper.setSpeed(0);
  //myStepper.step(200);  
  //i++;
  //delay(20);
    //lcd.clear();
    //lcd.print("Hello World!");
    // Turn off the display:
  
}


