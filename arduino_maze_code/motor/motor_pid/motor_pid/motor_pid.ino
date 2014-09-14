//#include "Encoder_Polling.h"
#include <Encoder.h>

//#include <Wire.h>
//#include <L3G.h>

const int R_encoder_A = 2;
const int R_encoder_B = 11;
const int L_encoder_A = 3;
const int L_encoder_B  = 12;

const int R_motor_forward = 5;
const int R_motor_backward = 6;
const int L_motor_forward = 9;
const int L_motor_backward = 10;

/*
const int R_encoder_A = 2;
const int R_encoder_B = 6;
const int L_encoder_A = 5;
const int L_encoder_B = 12;

const int R_motor_forward = 3;
const int R_motor_backward = 11;
const int L_motor_forward = 9;
const int L_motor_backward = 10;
*/
int counter = 0;
int motorSpeed = 0;
int actSpeed = 0;

//L3G gyro;

// PID controls
int damping = 0.1;

Encoder rightEnc(R_encoder_A, R_encoder_B);
//Encoder leftEnc(L_encoder_A, L_encoder_B);

long positionRight = -999;
//long positionLeft = -999;

int oneTurn = 2048;

void setup()
{
  pinMode(R_encoder_A, INPUT);
  digitalWrite(R_encoder_A, LOW);
  pinMode(R_encoder_B, INPUT);
  digitalWrite(R_encoder_B, LOW);
  pinMode(L_encoder_A, INPUT);
  digitalWrite(L_encoder_A, LOW);
  pinMode(L_encoder_B, INPUT);
  digitalWrite(L_encoder_B, LOW);
  pinMode(R_motor_forward, OUTPUT);
  digitalWrite(R_motor_forward, LOW);
  pinMode(R_motor_backward, OUTPUT);
  digitalWrite(R_motor_backward, LOW);
  pinMode(L_motor_forward, OUTPUT);
  digitalWrite(L_motor_forward, LOW);
  pinMode(L_motor_backward, OUTPUT);
  digitalWrite(L_motor_backward, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(12, LOW);
  
  
  // TCCR0B is pins 5 and 6
  // 0x01 = 62500
  // 0x02 = 7812.5
  //TCCR0B = TCCR0B & 0b11111000 | 0x01;
  
  // TCCR1B is pins 9 and 10
  // TCCR2B is pins 3 and 11
  //TCCR1B = TCCR1B & 0b11111000 | 0x01;
  //TCCR2B = TCCR2B & 0b11111000 | 0x01;
  
  Serial.begin(115200);
  Serial.println("Starting");

 // Wire.begin();
  
  /*if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
    while(1); 
  }*/
  
  //gyro.enableDefault();
}


void motorMove(float perSpeed, int pin1, int pin2) {
   actSpeed = (perSpeed/100)*255;
   analogWrite(pin1, actSpeed);
   analogWrite(pin2, 0); 
}

  
void loop()
{
  //gyro.read();
  //Serial.println((int)gyro.g.z);
  
  
  long newRight; //, newLeft;
  newRight = rightEnc.read();
  if (newRight != positionRight) {
    positionRight = newRight;
    Serial.println(newRight);
  }
  
  if (newRight >= -10*oneTurn) {
  //  motorMove(10.0, R_motor_forward,R_motor_backward);
  }
  else {
    newRight = 0;
    motorMove(0,R_motor_forward, R_motor_backward);
  }

}
  
  
