int rightPin1 = 3; 
int rightPin2 = 11;
int leftPin1 = 9;
int leftPin2 = 10;

//int perSpeed = 50;
int actSpeed = 0;



int time;

void setup()
{
  pinMode(rightPin1, OUTPUT);
  pinMode(rightPin2, OUTPUT);
  pinMode(leftPin1, OUTPUT);
  pinMode(leftPin2, OUTPUT);
  
  // TCCR0B is pins 5 and 6
  // 0x01 = 62500
  // 0x02 = 7812.5
  TCCR0B = TCCR0B & 0b11111000 | 0x01;
  
  // TCCR1B is pins 9 and 10
  // TCCR2B is pins 3 and 11
  // 0x01 = 31372.55
  // 0x02 = 3921.16
  TCCR1B = TCCR1B & 0b11111000 | 0x01;
  TCCR2B = TCCR2B & 0b11111000 | 0x01;
  
  Serial.begin(9600);
}


// Changing pin1 and pin2 decides left motors, right motors,
// and forward and backwards
void motorForward(float perSpeed, int pin1, int pin2) {
   actSpeed = (perSpeed/100)*255;
   analogWrite(pin1, actSpeed);
   analogWrite(pin2, 0);
}

void loop()
{
  motorForward(50.0, 3,11);
  motorForward(50.0, 9,10);
  delay(4000);
  motorForward(0,3,11);
  motorForward(0,9,10);
  delay(1000);
}
