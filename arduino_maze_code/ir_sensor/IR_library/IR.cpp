/*
IR.cpp - Library for setting pin modes, creating pin array, and querying IR sensors from set pins
Author: Alexander Hadik
Team: MicroMau5 - Tristan Paine, Kevin Jung, Alexander Hadik
*/

#include "Arduino.h"
#include "IR.h"

IR::IR(int ir_diag_out, int ir_side_out, int ir_front_out, int front_r_in, int front_l_in, int side_r_in, int side_l_in, int diag_r_in, int diag_l_in)
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

    int irArray[3][3]={
	  {ir_front_out, front_r_in, front_l_in},
	  {ir_diag_out,diag_r_in,diag_l_in},
	  {ir_side_out, side_r_in, side_l_in}
	};
}

int IR::readSensor(int recPin)
{
  int returnVal = analogRead(recPin);
  digitalWrite(ledPin,LOW);
  return returnVal;
}

void IR::powerUp(int ledPin){
    digitalWrite(ledPin, HIGH);
}