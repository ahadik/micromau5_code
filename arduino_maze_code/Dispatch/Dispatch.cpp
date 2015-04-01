/*
Dispatch.cpp - Library for setting pin modes, creating pin array, querying IR sensors from set pins, and reading gyro input
Author: Alexander Hadik
Team: MicroMau5 - Tristan Paine, Kevin Jung, Alexander Hadik
*/

#include "Arduino.h"
#include "Dispatch.h"
#include "../L3G/L3G.h"
#include "Wire.h"

  int irValues[6];

  int gyroReading;

  int calibratedArray[6];

  int irArray[3][3];

  float gyroVal=0.0;

  //L3G gyro;

Dispatch::Dispatch(int ir_diag_out, int ir_side_out, int ir_front_out, int front_r_in, int front_l_in, int side_r_in, int side_l_in, int diag_r_in, int diag_l_in)
{

	pinMode( ir_diag_out, OUTPUT );
  pinMode( ir_side_out, OUTPUT );
  pinMode( ir_front_out, OUTPUT );

  pinMode( front_r_in, INPUT );
  pinMode( front_l_in, INPUT );
  pinMode( side_l_in, INPUT );
  pinMode( side_r_in, INPUT );
  pinMode( diag_r_in, INPUT );
  pinMode( diag_l_in, INPUT );

  irArray[0][0] = ir_front_out;
  irArray[0][1] = front_r_in;
  irArray[0][2] = front_l_in;
  irArray[1][0] = ir_diag_out;
  irArray[1][1] = diag_r_in;
  irArray[1][2] = diag_l_in;
  irArray[2][0] = ir_side_out;
  irArray[2][1] = side_r_in;
  irArray[2][2] = side_l_in;

}

/*
//Read from the gyro and update the gyroReading value
void Dispatch::updateGyro(){
  gyro.read();
  gyroReading = (int)gyro.g.x;
}

*/
  /*Mapping y,x values to irValues indices
    0,1 -> 0
    0,2 -> 1
    1,1 -> 2
    1,2 -> 3
    2,1 -> 4
    2,2 -> 5

    y=0, sum is one greater
    y=1, sum is equal
    y=2, sum is one less
  */

  int Dispatch::mapCoords(int y, int x){
    int returnVal=0;

    if(y==0){
      returnVal = x+y-1;
    }else if(y&1){
      returnVal = x+y;
    }else if(y&2){
      returnVal = x+y+1;
    }
    return returnVal;
  }

  /*
    INPUT: y and x values for pin number in irArray
    OUTPUT: void; updates irValues array based off of y,x values
  */
  void Dispatch::readSensor(int y, int x)
  {
    int returnVal = analogRead(irArray[y][x]);
    irValues[Dispatch::mapCoords(y,x)]=returnVal;
  }

  /*
    Power up and power down led on provided pin
  */
  void Dispatch::powerUp(int ledPin){
      digitalWrite(ledPin, HIGH);
  }

  void Dispatch::powerDown(int ledPin){
    digitalWrite(ledPin,LOW);
  }

  //Given an input of an LED pin and a receiver pin, get the average receiver value over 10 trials.
  int Dispatch::getAverage(int ledPin, int y, int x){
    int runningTally = 0;
    for (int a=0; a<10; a++){
      Dispatch::powerUp(ledPin);
      delayMicroseconds(80);
      Dispatch::readSensor(y,x);
      runningTally += irValues[Dispatch::mapCoords(y,x)];
      Dispatch::powerDown(ledPin);
      delayMicroseconds(80);
    }
    int avgTally = runningTally/10;
    return avgTally;
  }

  int Dispatch::getValue(int index){
    int value = irValues[index]-calibratedArray[index];
    if(value<0){value=0;}
    return value;
  }

  void Dispatch::setGyro(double gyroReading){
    gyroVal=gyroReading;
  }

  void Dispatch::calibrateSensors(){
    for (int i = 0; i<3; i++){
      for (int j=1; j<3; j++){
        calibratedArray[Dispatch::mapCoords(i,j)]=Dispatch::getAverage(irArray[i][0],i,j);
      }

    }
  }
