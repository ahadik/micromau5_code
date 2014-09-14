    //Notes: https://engineering.purdue.edu/ME588/LabManual/lab6.pdf   \




int rightPin1 = 10; //10 is a PWM pin
int rightPin2 = 11; //11 is a PWM pin
int perSpeed = 50;
int actSpeed = 0;


int time;

void setup()
{
  pinMode(rightPin1, OUTPUT);
  pinMode(rightPin2, OUTPUT);
}



void rightMotorForward(float perSpeed)
{
  actSpeed = (perSpeed/100)*255; //convert % max speed to value [0,255]
  analogWrite(rightPin1, actSpeed); //analogWrite(pin,value) with value range [0, 255]
  analogWrite(rightPin2, 0);
}

void rightMotorReverse(float perSpeed)
{
  actSpeed = (perSpeed/100)*255;
  analogWrite(rightPin2, actSpeed);
  analogWrite(rightPin1, 0);
}

void loop()
{
  rightMotorReverse(50);
  delay(3000);
  rightMotorReverse(0);
  delay(1000);
  rightMotorForward(50);
  delay(3000);
  rightMotorForward(0);
  delay(1000);
}
