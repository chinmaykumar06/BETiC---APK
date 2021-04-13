#include "SevSeg.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "TimerOne.h"

#define SESS_TIME_INC_BUTT 4
#define SESS_TIME_DEC_BUTT 5

#define SET_EXTRM_TIME_BUTT A1
#define START_STOP_BUTT A0

#define HALL_LEFT_PIN 2
#define HALL_RIGHT_PIN 3

#define MOTOR_DIR_PIN 6
#define MOTOR_STEP_PIN 7

int session = 0;
long SESSION_START_TIME = 0;
int SESSION_REMAINING_TIME = 0;

int SESSION_TIME = 0;
int extreme_pause_time = 5;
int MAX_extreme_pause_time = 20;
int SKIPS_extreme_pause_time = 5;

int motor_direction = 0;
int motor_pulse_duration = 0;


LiquidCrystal_I2C lcd(0x27, 16, 2);
SevSeg displaySevSeg;

void initButtons();
void initMotor();
void initSensors();
void initLCD();
void initSevSeg();

void toggleStartStop();
void toggleMotorStep();

void leftExtremeReached();
void rightExtremeReached();
void centerReached();
void extremePause();

void display();
void setExtremePauseTIme();
void increaseSessionTime();
void decreaseSessionTime();

void setup(){
  //initializations
  PCICR |= 0b00000010; 
  PCMSK1 |= 0b00000001;
  sei();
  initButtons();
  initSensors();
  initMotor();
  initLCD();
  initSevSeg();
}

void loop(){
  if (session == 1){
    static long elapsed_time = millis() - SESSION_START_TIME;
    SESSION_REMAINING_TIME = SESSION_TIME - (elapsed_time / (1000*60));
    if (elapsed_time >= SESSION_TIME){
      Timer1.detachInterrupt();
      session = 0;
    }
  }
  else{
    // take inputs
    setExtremePauseTime();
    increaseSessionTime();
    decreaseSessionTime();
  }
  display();
}

//inits
void initButtons(){
  pinMode(START_STOP_BUTT, INPUT);
  pinMode(SESS_TIME_DEC_BUTT, INPUT);
  pinMode(SESS_TIME_INC_BUTT, INPUT);
  pinMode(SET_EXTRM_TIME_BUTT, INPUT);
}

void initSensors(){
  pinMode(HALL_LEFT_PIN, INPUT);
  pinMode(HALL_RIGHT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(HALL_LEFT_PIN),leftExtremeReached,FALLING);
  attachInterrupt(digitalPinToInterrupt(HALL_RIGHT_PIN),rightExtremeReached,FALLING);
}

void initMotor(){
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  pinMode(MOTOR_STEP_PIN, OUTPUT);

  digitalWrite(MOTOR_DIR_PIN, 1);
  motor_pulse_duration = 0;

  Timer1.initialize(4000);
}

void initLCD(){
  lcd.begin();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Pause time =");
  lcd.setCursor(0, 1);
  lcd.print("BETIC KJSCE");
}

void initSevSeg(){
  byte numDigits = 2;
  byte digitPins[] = {16, 15};
  byte segmentPins[] = {8, 9, 10, 11, 12, 13, 14};
  bool resistorsOnSegments = true;
  bool updateWithDelaysIn = true;
  byte hardwareConfig = COMMON_ANODE;

  displaySevSeg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  displaySevSeg.setBrightness(100);
  displaySevSeg.setNumber(00);
  displaySevSeg.refreshDisplay();
}

//interrupts
void leftExtremeReached(){
  extremePause();
  motor_direction = 1;
  digitalWrite(MOTOR_DIR_PIN, motor_direction);
}

void rightExtremeReached(){
  extremePause();
  motor_direction = 0;
  digitalWrite(MOTOR_DIR_PIN, motor_direction);
}

void extremePause(){
  long pause_start = millis();
  while((millis() - pause_start) <= extreme_pause_time * 1000);
}

ISR(PCINT8_vect)
{
  static long last_start_stop_click = 0;
  long current_start_stop_click = millis();
  if (current_start_stop_click - last_start_stop_click > 200){
    session = (session == 1) ? 0 : 1;
    if (session == 1){
      SESSION_START_TIME = millis();
      Timer1.attachInterrupt(toggleMotorStep);
    }
    else {
      Timer1.detachInterrupt();
    }
  }
  last_start_stop_click = current_start_stop_click;
}

void toggleMotorStep(){
  int current = digitalRead(MOTOR_STEP_PIN);
  if (session == 1){
    digitalWrite(MOTOR_STEP_PIN, current ^ 1);
  }
}

//important functions
void display(){
  lcd.setCursor(12, 0);
  lcd.print(extreme_pause_time);

  if session == 1:
    displaySevSeg.setNumber(SESSION_REMAINING_TIME);
  else:
    displaySevSeg.setNumber(SESSION_TIME);
}

void setExtremePauseTime(){
  int inc_butt_val = digitalRead(SESS_TIME_INC_BUTT);
  int dec_butt_val = digitalRead(SESS_TIME_DEC_BUTT);
  int extreme_butt_val = digitalRead(SET_EXTRM_TIME_BUTT);
  
  if (extreme_butt_val == 1 && inc_butt_val == 0 && dec_butt_val == 0){
    static long last_extrm_time_butt_click = 0;
    long current_extrm_time_butt_click = millis();
    if (current_extrm_time_butt_click - last_extrm_time_butt_click >= 200){
      extreme_pause_time += SKIPS_extreme_pause_time;
      extreme_pause_time %= MAX_extreme_pause_time + 1;
    }
    last_extrm_time_butt_click = current_extrm_time_butt_click;
  }
}

void increaseSessionTime(){
  int inc_butt_val = digitalRead(SESS_TIME_INC_BUTT);
  int dec_butt_val = digitalRead(SESS_TIME_DEC_BUTT);
  int extreme_butt_val = digitalRead(SET_EXTRM_TIME_BUTT);

  if (inc_butt_val == 1 && dec_butt_val == 0 && extreme_butt_val == 0){
    static long last_sess_time_inc_butt_click = 0;
    long current_sess_time_inc_butt_click = millis();
    if (current_sess_time_inc_butt_click - last_sess_time_inc_butt_click >= 200){
      SESSION_TIME += 1;
    }
    last_sess_time_inc_butt_click = current_sess_time_inc_butt_click;
  }
}

void decreaseSessionTime(){
  int inc_butt_val = digitalRead(SESS_TIME_INC_BUTT);
  int dec_butt_val = digitalRead(SESS_TIME_DEC_BUTT);
  int extreme_butt_val = digitalRead(SET_EXTRM_TIME_BUTT);

  if (inc_butt_val == 0 && dec_butt_val == 1 && extreme_butt_val == 0){
    static long last_sess_time_dec_butt_click = 0;
    long current_sess_time_dec_butt_click = millis();
    if (current_sess_time_dec_butt_click - last_sess_time_dec_butt_click >= 200){
      SESSION_TIME -= 1;
    }
    last_sess_time_dec_butt_click = current_sess_time_dec_butt_click;
  }
}
