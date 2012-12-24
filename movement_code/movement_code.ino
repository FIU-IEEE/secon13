//Import libraries
#include <Servo.h>
//Servo pin assignments
#define fl_pin 2
#define rl_pin 3
#define fr_pin 4
#define rr_pin 5
//Rotary Encoder pin assignemnts
#define fl_enc 18
#define rl_enc 19
#define fr_enc 20
#define rr_enc 21
//number of samples for encoder smoothing
#define samples 7
//Declaring servo objects
#define pings1 6 //pin for 1st side ping sensor
#define pings2 7 //pin for 2nd side ping sensor
#define pingb 8  //pin for back ping sensor

//decleration of servo objects
Servo fl, fr, rl, rr;
//time since last encoder interupt in uS per wheel
volatile unsigned long fl_time, fr_time, rl_time, rr_time;
//average period of rotation per wheel
unsigned long fl_period, fr_period, rl_period, rr_period;
//servo speed setting (currently in degrees but needs to be ms)
int fl_spd, fr_spd, rl_spd, rr_spd;
//count of encoder period samples for averaging purposes
volatile int fl_cnt, fr_cnt, rl_cnt, rr_cnt;
//array of period samples to be averaged
int fl_samples[samples], fr_samples[samples], rl_samples[samples], rr_samples[samples];
//ping sensor input
volatile long s1duration, s2duration, bduration;
volatile boolean offcourse;

void setup(){
  //initialize servos
  fl.attach(fl_pin);
  fr.attach(fr_pin);
  rl.attach(rl_pin);
  rr.attach(rr_pin);
  //initialize encoders
  pinMode(fl_enc, INPUT);
  pinMode(rl_enc, INPUT);
  pinMode(fr_enc, INPUT);
  pinMode(rr_enc, INPUT);
  //setup interupts for encoders
  attachInterrupt(5, encoder1, CHANGE);
  attachInterrupt(4, encoder2, CHANGE);
  attachInterrupt(3, encoder3, CHANGE);
  attachInterrupt(2, encoder4, CHANGE);
}

void loop(){
 moveStraight(20);
}
//encoder interupt service routines
//samples 7 period measurments and takes average 
void encoder1(){
  if(fl_cnt < samples+1){  //we dont have enough samples yet
    fl_samples[fl_cnt] = (micros()-fl_time);  //take another sample and save to the array
    fl_cnt++;  //increment count of samples
  }
  else{  //we do have enough samples
    fl_period = 0;  //reset count of samples
    while(fl_cnt > 0){  //add up all the values in the array while setting count back to zero
      fl_period += fl_samples[fl_cnt-1];
      fl_cnt--;
    }
    fl_period = fl_period/samples;  //divide sum by number of samples to get average
  }
  fl_time = micros();  //save time of interupt
}

void encoder2(){
  if(fr_cnt < samples+1){
    fr_samples[fr_cnt] = (micros()-fr_time);
    fr_cnt++;
  }
  else{
    fr_period = 0;
    while(fr_cnt > 0){
      fr_period += fr_samples[fr_cnt-1];
      fr_cnt--;
    }
    fr_period = fr_period/samples;
  }
  fr_time = micros();
}

void encoder3(){
  if(rl_cnt < samples+1){
    rl_samples[rl_cnt] = (micros()-rl_time);
    rl_cnt++;
  }
  else{
    rl_period = 0;
    while(rl_cnt > 0){
      rl_period += rl_samples[rl_cnt-1];
      rl_cnt--;
    }
    rl_period = rl_period/samples;
  }
  rl_time = micros();
}

void encoder4(){
  if(rr_cnt < samples+1){
    rr_samples[rr_cnt] = (micros()-rr_time);
    rr_cnt++;
  }
  else{
    rr_period = 0;
    while(rr_cnt > 0){
      rr_period += rr_samples[rr_cnt-1];
      rr_cnt--;
    }
    rr_period = rr_period/samples;
  }
  rr_time = micros();
}

//foward @ 90
void moveStraight(int vel){
  getRange(0);
  getRange(1);
  fl.write(90-vel);  //set speed to master servo, others will try to match this one
  //fr.write(vel+90);
  //rl.write(90-vel);
  //rr.write(vel+90);
  
  //speed match fr servo
  if(vel > 0){  //if we should be going in dir1
    if(fr_spd > 90){  //and we are
      if(fr_period < fl_period){  //and were spinning to fast
        fr_spd--;  //slow down
      }
      else{
        fr_spd++;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      fr_spd++;  //try to change direction
    }
  }
  else{  //if we should be going in dir 2
    if(fr_spd < 90){  //and we are
      if(fr_period < fl_period){  //and were spinning to fast
        fr_spd++;  //slow down
      }
      else{
        fr_spd--;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      fr_spd--;  //try to change direction
    }
  }
  fr.write(fr_spd);
  //speed match rl servo
  if(vel > 0){  //if we should be going in dir1
    if(rl_spd < 90){  //and we are
      if(rl_period < fl_period){  //and were spinning to fast
        rl_spd++;  //slow down
      }
      else{
        rl_spd--;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rl_spd--;  //try to change direction
    }
  }  
  else{  //if we should be going in dir 2
    if(rl_spd > 90){  //and we are
      if(rl_period < fl_period){  //and were spinning to fast
        rl_spd--;  //slow down
      }
      else{
        rl_spd++;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rl_spd++;  //try to change direction
    }
  }
  rl.write(rl_spd);
  //speed match rr servo
  if(vel > 0){  //if we should be going in dir1
    if(rr_spd > 90){  //and we are
      if(rr_period < fl_period){  //and were spinning to fast
        rr_spd--;  //slow down
      }
      else{
        rr_spd++;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rr_spd++;  //try to change direction
    }
  }  
  else{  //if we should be going in dir 2
    if(rr_spd < 90){  //and we are
      if(rr_period < fl_period){  //and were spinning to fast
        rr_spd++;  //slow down
      }
      else{
        rr_spd--;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rr_spd--;  //try to change direction
    }
  }
  rr.write(rr_spd);
}
//moves right @ 90
void moveSide(int vel){
  fl.write(vel+90);
  //fr.write(vel+90);
  //rl.write(90-vel);
  //rr.write(90-vel);
  
  //speed match fr servo
  if(vel > 0){  //if we should be going in dir1
    if(fr_spd > 90){  //and we are
      if(fr_period < fl_period){  //and were spinning to fast
        fr_spd--;  //slow down
      }
      else{
        fr_spd++;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      fr_spd++;  //try to change direction
    }
  }  
  else{  //if we should be going in dir 2
    if(fr_spd < 90){  //and we are
      if(fr_period < fl_period){  //and were spinning to fast
        fr_spd++;  //slow down
      }
      else{
        fr_spd--;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      fr_spd--;  //try to change direction
    }
  }
  //speed match rl servo
  if(vel > 0){  //if we should be going in dir1
    if(rl_spd < 90){  //and we are
      if(rl_period < fl_period){  //and were spinning to fast
        rl_spd++;  //slow down
      }
      else{
        rl_spd--;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rl_spd--;  //try to change direction
    }
  }  
  else{  //if we should be going in dir 2
    if(rl_spd > 90){  //and we are
      if(rl_period < fl_period){  //and were spinning to fast
        rl_spd--;  //slow down
      }
      else{
        rl_spd++;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rl_spd++;  //try to change direction
    }
  }
  rl.write(rl_spd);
  //speed match rr servo
  if(vel > 0){  //if we should be going in dir1
    if(rr_spd > 90){  //and we are
      if(rr_period < fl_period){  //and were spinning to fast
        rr_spd--;  //slow down
      }
      else{
        rr_spd++;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rr_spd++;  //try to change direction
    }
  }  
  else{  //if we should be going in dir 2
    if(rr_spd < 90){  //and we are
      if(rr_period < fl_period){  //and were spinning to fast
        rr_spd++;  //slow down
      }
      else{
        rr_spd--;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rr_spd--;  //try to change direction
    }
  }
  rr.write(rr_spd);
}
//rotates left @ 90
void rotate(int vel){
  fl.write(vel+90);
  //fr.write(vel+90);
  //rl.write(vel+90);
  //rr.write(vel+90);
  
  //speed match fr servo
  if(vel > 0){  //if we should be going in dir1
    if(fr_spd > 90){  //and we are
      if(fr_period < fl_period){  //and were spinning to fast
        fr_spd--;  //slow down
      }
      else{
        fr_spd++;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      fr_spd++;  //try to change direction
    }
  }  
  else{  //if we should be going in dir 2
    if(fr_spd < 90){  //and we are
      if(fr_period < fl_period){  //and were spinning to fast
        fr_spd++;  //slow down
      }
      else{
        fr_spd--;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      fr_spd--;  //try to change direction
    }
  }
  fr.write(fr_spd);
  
  //speed match rl servo
  if(vel > 0){  //if we should be going in dir1
    if(rl_spd > 90){  //and we are
      if(rl_period < fl_period){  //and were spinning to fast
        rl_spd--;  //slow down
      }
      else{
        rl_spd++;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rl_spd++;  //try to change direction
    }
  }  
  else{  //if we should be going in dir 2
    if(rl_spd < 90){  //and we are
      if(rl_period < fl_period){  //and were spinning to fast
        rl_spd++;  //slow down
      }
      else{
        rl_spd--;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rl_spd--;  //try to change direction
    }
  }
  rl.write(rl_spd);

  //speed match rr servo
  if(vel > 0){  //if we should be going in dir1
    if(rr_spd > 90){  //and we are
      if(rr_period < fl_period){  //and were spinning to fast
        rr_spd--;  //slow down
      }
      else{
        rr_spd++;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rr_spd++;  //try to change direction
    }
  }  
  else{  //if we should be going in dir 2
    if(rr_spd < 90){  //and we are
      if(rr_period < fl_period){  //and were spinning to fast
        rr_spd++;  //slow down
      }
      else{
        rr_spd--;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rr_spd--;  //try to change direction
    }
  }
  rr.write(rr_spd);
}

//FR @ 90, RL @ -90
void diagonalR(int vel){
  fl.write(90-vel);
  //rr.write(vel+90);
  //speed match rr servo
  if(vel > 0){  //if we should be going in dir1
    if(rr_spd > 90){  //and we are
      if(rr_period < fl_period){  //and were spinning to fast
        rr_spd--;  //slow down
      }
      else{
        rr_spd++;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rr_spd++;  //try to change direction
    }
  }  
  else{  //if we should be going in dir 2
    if(rr_spd < 90){  //and we are
      if(rr_period < fl_period){  //and were spinning to fast
        rr_spd++;  //slow down
      }
      else{
        rr_spd--;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rr_spd--;  //try to change direction
    }
  }
  rr.write(rr_spd);
}
//FL @ 90, RR @ -90
void diagonalL(int vel){
  fr.write(vel+90);
  //rl.write(90-vel);
  //speed match rl servo
  if(vel > 0){  //if we should be going in dir1
    if(rl_spd < 90){  //and we are
      if(rl_period < fl_period){  //and were spinning to fast
        rl_spd++;  //slow down
      }
      else{
        rl_spd--;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rl_spd--;  //try to change direction
    }
  }  
  else{  //if we should be going in dir 2
    if(rl_spd > 90){  //and we are
      if(rl_period < fl_period){  //and were spinning to fast
        rl_spd--;  //slow down
      }
      else{
        rl_spd++;  //speed up
      }
    }
    else{  //or we're not going in the right direcion
      rl_spd++;  //try to change direction
    }
  }
  rl.write(rl_spd);
}
void getRange(int sensor){
  switch(sensor){
    case 0: 
      pinMode(pings1, OUTPUT);
      digitalWrite(pings1, LOW);
      delayMicroseconds(2);
      digitalWrite(pings1, HIGH);
      delayMicroseconds(5);
      digitalWrite(pings1, LOW);
      pinMode(pings1, INPUT);
      s1duration = pulseIn(pings1, HIGH);
      break;
    case 1:
      pinMode(pings2, OUTPUT);
      digitalWrite(pings2, LOW);
      delayMicroseconds(2);
      digitalWrite(pings2, HIGH);
      delayMicroseconds(5);
      digitalWrite(pings2, LOW);
      pinMode(pings2, INPUT);
      s2duration = pulseIn(pings2, HIGH);
      break;
    case 2:
      pinMode(pingb, OUTPUT);
      digitalWrite(pingb, LOW);
      delayMicroseconds(2);
      digitalWrite(pingb, HIGH);
      delayMicroseconds(5);
      digitalWrite(pingb, LOW);
      pinMode(pingb, INPUT);
      bduration = pulseIn(pingb, HIGH);
      break;
  }
}

