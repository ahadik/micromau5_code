/*
Dispatch.h - Library for sensors on the MicroMau5
Author: Alexander Hadik
Team: MicroMau5 - Tristan Paine, Kevin Jung, Alexander Hadik
*/

#ifndef Dispatch_h
#define Dispatch_h

#include "L3G.h"
#include "Arduino.h"

class Dispatch
{
	public:
		Dispatch(int ir_diag_out, int ir_side_out, int ir_front_out, int front_r_in, int front_l_in, int side_r_in, int side_l_in, int diag_r_in, int diag_l_in);
		//void updateGyro();
		void readSensor(int y, int x);
		void powerUp(int ledPin);
		void powerDown(int ledPin);
		void calibrateSensors();
		int mapCoords(int y, int x);
		int getValue(int index);
		L3G gyro;
		int irArray[3][3];
		int gyroReading;
		int calibratedArray[6];
		int irValues[6];
		float gyroVal;
		void setGyro(double gyroReading);
		int getAverage(int ledPin, int y, int x);
};

#endif
