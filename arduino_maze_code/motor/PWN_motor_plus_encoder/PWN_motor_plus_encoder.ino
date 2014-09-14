#include <Encoder.h>


Encoder knobRight(2, 11);

//int perSpeed = 50;
int actSpeed = 0;



int time;

void setup()
{
  pinMode(rightPin1, OUTPUT);
  pinMode(rightPin2, OUTPUT);
  pinMode(leftPin1, OUTPUT);
  pinMode(leftPin2, OUTPUT);
  Serial.begin(9600);
  
//  TCCR1B = TCCR1B & b11111000 | 0x01;

}

void motorForward(float perSpeed, int pin1, int pin2) {
   actSpeed = (perSpeed/100)*255;
   analogWrite(pin1, actSpeed);
   analogWrite(pin2, 0);
}

long positionRight = -999;

void loop()
{
  motorForward(50.0, 5,6);
  long newRight;
  newRight = knobRight.read();
  if (newRight != positionRight) {
    Serial.print(", Right = ");
    Serial.print(newRight);
    Serial.println();
    positionRight = newRight;
  }
  // if a character is sent from the serial monitor,
  // reset both back to zero.
  if (Serial.available()) {
    Serial.read();
    Serial.println("Reset both knobs to zero");
    knobRight.write(0);
  }
  delay(4000);
  motorForward(10.0, 6,5);
  delay(2000);
  motorForward(0,5,6);
  delay(1000);
}


void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
