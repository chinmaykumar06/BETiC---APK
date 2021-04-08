#include "SevSeg.h"
SevSeg Display;
uint8_t number = 0;
const unsigned long period = 200;
unsigned long startMillis;
unsigned long currentMillis;
int Timi = 4; ///// Timer Increment
int Timd = 5; ///// Timer Decrement
int buzz = 19;
int Timi_state = 0;
int Timd_state = 0;
////////////////////Hall sensors//////////////////
#define halla 2
#define hallb 3
////////////////Stepper motor configuration////////////////////////////////////////////
#define dir 6
#define Step 7
//////////////////Speed--- Constant////////////////////////////////////
int pd = 4000;
//////////////Direction Change////////////////////////
boolean setdir = LOW;
void limit_a() {
  setdir = !setdir;
}
void limit_b() {
  setdir = !setdir;
}

void setup()
{
  Serial.begin(9600);
  ///////pin change Interruots///////////////////// 
  PCMSK2 = B00110000; 
  PCIFR = B00000000; 
  PCICR = B00000100;
  int pd = 4000;
  pinMode(dir, OUTPUT);
  pinMode(Step, OUTPUT);
  pinMode(halla, INPUT);
  pinMode(hallb, INPUT);
  pinMode(buzz, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(halla), limit_a, FALLING);
  attachInterrupt(digitalPinToInterrupt(hallb), limit_b, FALLING);
  pinMode(Timi, INPUT);
  pinMode(Timd, INPUT);
  byte numDigits = 2;
  byte digitPins[] = {16, 15};
  byte segmentPins[] = {8, 9, 10, 11, 12, 13, 14};
  bool resistorsOnSegments = true;
  bool updateWithDelaysIn = true;
  byte hardwareConfig = COMMON_ANODE;
  Display.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  Display.setBrightness(100);
  Display.setNumber(0000);
  Display.refreshDisplay();
  digitalWrite(buzz, LOW);
}
void loop()
{
  for (int i = number; i >= 0; i--)
  {
    currentMillis = millis();
    if (currentMillis - startMillis >= 60000) /////////////Timer set////////////////////
    {
      number = number - 1;     //Decrementing the Timer value
      startMillis = currentMillis;


    }
    digitalWrite(dir, setdir);
    digitalWrite(Step, HIGH);
    delayMicroseconds(pd);
    digitalWrite(Step, LOW);
    delayMicroseconds(pd);
    Serial.println(number);
    Display.setNumber(number);
    Display.refreshDisplay();
    if(number==0) /////////////After number =0, motor will stop and buzzer will be on indicating the session is over and Main swicth needs to be switched off//////////
    {
      digitalWrite(buzz, HIGH);
      delay(2000);
      digitalWrite(buzz, LOW); 
      digitalWrite(Step, LOW);
      delay(5000);
      break;
    }
  

  }


}

/////////////////Interrupt service routine///////////////////////////////
ISR(PCINT2_vect)
{
  Timi_state = digitalRead(Timi);
  Timd_state = digitalRead(Timd);
  if (Timi_state == HIGH && Timd_state == LOW)
  {
    number = number + 5;     //Incrementing the Timer value
    if (number >= 99)
      number  = 0;
  }

  if (Timi_state == LOW && Timd_state == HIGH)
  {
    number = number - 1;     //Decrementing the Timer value
    if (number == 0 )
      number  = 0;
  }

}
