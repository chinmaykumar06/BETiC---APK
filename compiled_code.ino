#define halla 2
#define hallb 3
#define dir 10
#define Step 11
#include "SevSeg.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
SevSeg Display;
int but = 12;
int but1 = 13;
int butstate = LOW;
int butstate1 = LOW;
unsigned int number = 0;
unsigned int number1 = 0;
unsigned int time_now = 0;
const unsigned long period = 200;
const unsigned long period1 = 250;// in millisecond
unsigned long startMillis;
unsigned long currentMillis;
unsigned long startMillis1;
unsigned long currentMillis1;
int pd =4000;
boolean setdir =LOW;
void limit_a(){
  pause_delay();
  setdir = !setdir;
}
void limit_b(){
  pause_delay();
  setdir = !setdir;
}
void pause_delay()
{
    time_now = millis();
    while(millis() < time_now + number1){}
}


void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  pinMode(but, INPUT);
  pinMode(but1, INPUT);
  byte numDigits = 2;
  byte digitPins[] = {12, 13};
  byte segmentPins[] = {2, 3, 4, 5, 6, 7, 8, 9};
  bool resistorsOnSegments = true;
  bool updateWithDelaysIn = true;
  byte hardwareConfig = COMMON_ANODE;
  Display.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  Display.setBrightness(100);
  Display.setNumber(00);
  Display.refreshDisplay();
  lcd.setCursor(0, 0);
  lcd.print("Pause time =");
  lcd.setCursor(0, 1);
  lcd.print("BETIC KJSCE");
  int pd =4000;
  pinMode(dir,OUTPUT);
  pinMode(Step,OUTPUT);
  pinMode(halla,INPUT);
  pinMode(hallb,INPUT);
  attachInterrupt(digitalPinToInterrupt(halla),limit_a,FALLING);
  attachInterrupt(digitalPinToInterrupt(hallb),limit_b,FALLING);
}

void loop() {
  
  //read current buttons
  butstate =  digitalRead(but);
  butstate1 =  digitalRead(but1);

  // set session_time
  if (butstate == HIGH && butstate1 == LOW) {
    
    currentMillis = millis();
    if (currentMillis - startMillis >= period)
    {
      number = number + 1;
      startMillis = currentMillis;
    }
    if (number >= 99)
    {
      number  = 0;
    }
    Display.setNumber(number);
    Display.refreshDisplay();
  }

  //set pause_time
  if (butstate1 == HIGH && butstate == LOW) {
    
    currentMillis1 = millis();
    if (currentMillis1 - startMillis1 >= period1)
    {
      number1 = number1 + 5;
      startMillis1 = currentMillis1;
    }
    if (number1 >= 21)
    {

      number1  = 0;
    }
    if (number1 == 0) {
      lcd.setCursor(12, 0);
      lcd.print("00");
    }
    if (number1 == 5) {
      lcd.setCursor(12, 0);
      lcd.print("05");
    }
    if (number1 == 10) {
      lcd.setCursor(12, 0);
      lcd.print("10");
    }
    if (number1 == 15) {
      lcd.setCursor(12, 0);
      lcd.print("15");
    }
    if (number1 == 20) {
      lcd.setCursor(12, 0);
      lcd.print("20");
    }

  }
  for (int i = number; i >= 0; i--) {

    digitalWrite(dir,setdir);
    digitalWrite(Step,HIGH);
    delayMicroseconds(pd);
    digitalWrite(Step,LOW);
    delayMicroseconds(pd);
    currentMillis = millis();
    if (currentMillis - startMillis >= 5000)
    {
      number = number - 1;
      startMillis = currentMillis;

    }
    Display.setNumber(number);
    Display.refreshDisplay();
    Serial.println(number);
  }
  
}
