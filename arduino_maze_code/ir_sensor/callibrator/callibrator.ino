#include <Event.h>
#include <Timer.h>

//Define integer variables for input and output pins.

int ir_diag_out = 7;
int ir_side_out = 4;
int ir_front_out = 8;

int front_r_in = A7;
int front_l_in = A6;
int side_l_in = A2;
int side_r_in = A3;
int diag_r_in = A1;
int diag_l_in = A0;

int counter = 0;

//This is an array of ir sensor pins. First element of each row is output(front, side, diag). Second two elements of each row is left and right for the output.
//Order is front, diagonal, side
int irArray[3][3]={
  {ir_front_out, front_r_in, front_l_in},
  {ir_diag_out,diag_r_in,diag_l_in},
  {ir_side_out, side_r_in, side_l_in}
};

//callibratedArray[i][0] => right sensor
//callibratedArray[i][0] => left sensor
int callibratedArray[3][2];

void setup()
{
  //Set intput and output modes for defined pins
    Serial.begin(9600);  //Begin serial communcation
    pinMode( ir_diag_out, OUTPUT );
    pinMode( ir_side_out, OUTPUT );
    pinMode( ir_front_out, OUTPUT );
    
    pinMode( front_r_in, INPUT );
    pinMode( front_l_in, INPUT );
    pinMode( side_r_in, INPUT );
    pinMode( side_l_in, INPUT );
    pinMode( diag_r_in, INPUT );
    pinMode( diag_l_in, INPUT );
    
  
    //Populate the table of sensor averages
    //callibrateSensors doesn't seem to work quite yet
    //callibrateSensors();
}

//Given an input of an LED pin and a receiver pin, get the average receiver value over 10 trials.
int getAverage(int ledPin, int recPin){
  int runningTally = 0;
  for (int x=0; x<=10; x++){
    runningTally += readSensor(ledPin, recPin);
  }
  int avgTally = runningTally/10;
  return avgTally;
}

void callibrateSensors(){
  for (int i = 0; i<3; i++){
    callibratedArray[3][0]=getAverage(irArray[i][0],irArray[i][1]);
    callibratedArray[3][1]=getAverage(irArray[i][0],irArray[i][2]);
  }
}

int readSensor(int ledPin, int recPin)
{
  digitalWrite(ledPin, HIGH);
  delayMicroseconds(80);
  int returnVal = analogRead(recPin);
  digitalWrite(ledPin,LOW);
  delayMicroseconds(80);
  
  return returnVal;
}

//INPUT: Six element array that holds the values for the ir sensors. Index moving front to back, left, right. The function updates the values by means of 
void querySensors(byte *sensorArray){

  int counter = 0;
  for (int i=0; i<3; i++){
      
    int sensorRight = readSensor(irArray[i][0],irArray[i][1]);
    int sensorLeft = readSensor(irArray[i][0],irArray[i][2]);
    
    sensorArray[counter]=sensorRight;
    counter++;
    sensorArray[counter]=sensorLeft;
    counter++;
  }
}

void loop(){
  
  //change this to get the sensor value
  //int sensorReading = readSensor(4, side_l_in);
  //Serial.println(sensorReading);
  
 pinMode(4, OUTPUT );
 digitalWrite(4, HIGH);
  
}
