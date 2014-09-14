/*
IR.h - Library for IR sensors
Author: Alexander Hadik
Team: MicroMau5 - Tristan Paine, Kevin Jung, Alexander Hadik
*/

#ifndef IR_h
#define IR_h

#include "Arduino.h"

class IR
{
	public:
		IR(int ir_diag_out, int ir_side_out, int ir_front_out, int front_r_in, int front_l_in, int side_r_in, int side_l_in, int diag_r_in, int diag_l_in);
		int readSensor(int recPin);
		void powerUp(int ledPin);
		int irArray[][];
}

#endif