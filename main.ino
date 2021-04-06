// SESSION_TIME (2 buttons - inc dec) [0-99 mins]

// extreme_pause_time (1 button - loops back to 0 after x counts)

// session = 0/1 (1 button - alternate options)

// 7-seg -> SESSION_TIME

// lcd -> extreme_pause_time

//hall_left, hall_right, hall_center -> interrupts - high to low - set direction

//Motor - speed(pd), direction (0/1) - higl-low-high-low switching on internal interrupt after speed secs
//stop_motor - pd = 0


// on_start - motor ghumega
// on_stop - motor will come to the center and stop

#include "SevSeg.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "TimerOne.h"

#define START_STOP_BUTT 15

#define SESS_TIME_INC_BUTT 12
#define SESS_TIME_DEC_BUTT 13

#define SET_EXTRM_TIME_BUTT 14

#define HALL_LEFT_PIN 2
#define HALL_RIGHT_PIN 3
#define HALL_CENTER_PIN 4

#define MOTOR_DIR_PIN 10
#define MOTOR_STEP_PIN 11

int session = 0;
long SESSION_START_TIME = 0;
int SESSION_REMAINING_TIME = 0;

int SESSION_TIME = 0;
int extreme_pause_time = 5;
int MAX_extreme_pause_time = 20;
int SKIPS_extreme_pause_time = 5;

int motor_direction = 0;
int motor_pulse_duration = 0;

int GO_TO_ZERO = 1;

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
	sei();
	initButtons();
	initMotor();
	initSensors();
	initLCD();
	
}

void loop(){
	if (session == 1){
		static long elapsed_time = millis() - SESSION_START_TIME;
		SESSION_REMAINING_TIME = SESSION_TIME - (elapsed_time / (100*60));
		if (elapsed_time >= SESSION_TIME){
			GO_TO_ZERO = 1;
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
	attachInterrupt(digitalPinToInterrupt(START_STOP_BUTT),toggleStartStop,FALLING);
}

void initMotor(){
	pinMode(MOTOR_DIR_PIN, OUTPUT);
	pinMode(MOTOR_STEP_PIN, OUTPUT);
	motor_pulse_duration = 0;

	Timer1.initialize(4000000);
	Timer1.attachInterrupt(toggleMotorStep);
}

void initSensors(){
	pinMode(HALL_LEFT_PIN, INPUT);
	pinMode(HALL_RIGHT_PIN, INPUT);
	pinMode(HALL_CENTER_PIN, INPUT);
	attachInterrupt(digitalPinToInterrupt(HALL_LEFT_PIN),leftExtremeReached,FALLING);
	attachInterrupt(digitalPinToInterrupt(HALL_RIGHT_PIN),rightExtremeReached,FALLING);
	attachInterrupt(digitalPinToInterrupt(HALL_CENTER_PIN),centerReached,FALLING);
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
	byte digitPins[] = {12, 13};
	byte segmentPins[] = {2, 3, 4, 5, 6, 7, 8, 9};
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
	motor_direction = 1
}

void rightExtremeReached(){
	extremePause();
	motor_direction = 0;
}

void centerReached(){
	if (GO_TO_ZERO == 1){
		Timer1.detachInterrupt();
		session = 0;
		GO_TO_ZERO = 0;
	}
}

void extremePause(){
	long pause_start = millis();
	while((millis() - pause_start) <= extreme_pause_time * 100);
}

void toggleStartStop(){
	static long last_start_stop_click = 0;
	long current_start_stop_click = millis();
	if (current_start_stop_click - last_start_stop_click > 200){
		session = (session == 1) ? 0 : 1;
		if (session == 1){
			SESSION_START_TIME = millis();
			Timer1.attachInterrupt(toggleMotorStep);
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
		displaySevSeg.setNumber(SESSION_TIME)
}

void setExtremePauseTime(){
	int inc_butt_val = digitalRead(SESS_TIME_INC_BUTT);
	int dec_butt_val = digitalRead(SESS_TIME_DEC_BUTT);
	int extreme_butt_val = digitalRead(SET_EXTRM_TIME_BUTT);
	
	if (extreme_butt_val == 1 && inc_butt_val == 0 && dec_butt_val == 1){
		static long last_extrm_time_butt_click = 0;
		long current_extrmtime_butt_click = millis();
		if (current_extrmtime_butt_click - last_extrm_time_butt_click >= 200){
			extreme_pause_time += SKIPS_extreme_pause_time;
			extreme_pause_time %= MAX_extreme_pause_time + 1;
		}
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
	}
}
