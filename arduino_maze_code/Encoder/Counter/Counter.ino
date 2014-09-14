
#include "Encoder_Polling.h"

const int encoderPin_A = 2;
const int encoderPin_B = 6;

const int motorA = 3;
const int motorB = 11;
const int motor2A = 9;
const int motor2B = 10;

int counter = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);
  digitalWrite(motorA, LOW);
  digitalWrite(motorB, LOW);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, LOW);
  
  encoder_begin(encoderPin_A, encoderPin_B); // Start the decoder
}

void loop()
{
  int dir = encoder_data(); // Check for rotation
  
  if(dir == 1)       // If its forward...
  {
    counter++;       // Increment the counter
    if (counter >= 512)
    {
      counter = 0; 
    }
    Serial.println(counter);
  }
  else if(dir == -1) // If its backward...
  {
    counter--;       // Decrement the counter
    if (counter <= -512)
    {
      counter = 0; 
    }
    Serial.println(counter);
  }
}
