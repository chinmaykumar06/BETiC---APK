#define halla 2
#define hallb 3
#define dir 10
#define Step 11
int pd =4000;
boolean setdir =LOW;

void limit_a(){
  setdir = !setdir;
}
void limit_b(){
  setdir = !setdir;
}


void setup() {
  int pd =4000;
  pinMode(dir,OUTPUT);
  pinMode(Step,OUTPUT);
  pinMode(halla,INPUT);
  pinMode(hallb,INPUT);
  attachInterrupt(digitalPinToInterrupt(halla),limit_a,FALLING);
  attachInterrupt(digitalPinToInterrupt(hallb),limit_b,FALLING);
}

void loop() {
  digitalWrite(dir,setdir);
  digitalWrite(Step,HIGH);
  delayMicroseconds(pd);
  digitalWrite(Step,LOW);
  delayMicroseconds(pd);
  
  // lcd 
  // seven_segment
}
