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
const unsigned long period = 200;
const unsigned long period1 = 250;// in millisecond
unsigned long startMillis;
unsigned long currentMillis;
unsigned long startMillis1;
unsigned long currentMillis1;
void setup()
{
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  pinMode(but, INPUT);
  pinMode(but1, INPUT);
  byte numDigits = 2;
  byte digitPins[] = {10, 11};
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
}
void loop()
{

  butstate =  digitalRead(but);
  butstate1 =  digitalRead(but1);
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
    //    lcd.setCursor(0, 1);
    //    lcd.print("Timer =");
    //    lcd.setCursor(7, 1);
    //    lcd.print(number);

  }
  for (int i = number; i >= 0; i--) {
    currentMillis = millis();
    if (currentMillis - startMillis >= 5000)
    {
      number = number - 1;
      startMillis = currentMillis;

    }
    Display.setNumber(number);
    Display.refreshDisplay();
    //    lcd.setCursor(0, 1);
    //    lcd.print("Timer =");
    //    lcd.setCursor(7, 1);
    //    lcd.print(number);
    Serial.println(number);
  }
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

    //    lcd.setCursor(12, 0);
    //    lcd.print(number1);
  }



}
