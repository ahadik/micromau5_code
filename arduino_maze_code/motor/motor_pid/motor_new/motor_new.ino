 int val; 
const int R_encoder_A = 2;
const int R_encoder_B = 11;
const int L_encoder_A = 3;
const int L_encoder_B  = 12;

const int R_motor_forward = 5;
const int R_motor_backward = 6;
const int L_motor_forward = 9;
const int L_motor_backward = 10;

 int encoder0Pos = 0;
 int encoder0PinALast = LOW;
 int n = LOW;
 
 int oneTurn = 512;
 int actSpeed = 0;

 void setup() { 
   pinMode (R_encoder_A,INPUT);
   pinMode(R_motor_forward, OUTPUT);
   digitalWrite(R_motor_forward, LOW);
   pinMode(R_motor_backward, OUTPUT);
   digitalWrite(R_motor_backward, LOW);
   pinMode (R_encoder_B,INPUT);
   Serial.begin (9600);
 } 

void motorMove(float perSpeed, int pin1, int pin2) {
   actSpeed = (perSpeed/100)*255;
   analogWrite(pin1, actSpeed);
   analogWrite(pin2, 0); 
}


 void loop() { 
   n = digitalRead(R_encoder_A);

//  Serial.println(encoder0Pos);
//  motorMove(5.0, R_motor_forward, R_motor_backward);
   
  if (encoder0Pos >= -10*oneTurn) {
    motorMove(5.0, R_motor_forward,R_motor_backward);
    }
  else {
    motorMove(0,R_motor_forward, R_motor_backward);
  }

   
   if ((encoder0PinALast == LOW) && (n == HIGH)) {
     if (digitalRead(R_encoder_B) == LOW) {
       encoder0Pos--;
     } else {
       encoder0Pos++;
     }
     Serial.print (encoder0Pos);
     Serial.print ("/");
   } 
   encoder0PinALast = n;
 } 
