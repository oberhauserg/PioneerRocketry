#ifndef _SENSORTEST_H
#define _SENSORTEST_H

#include<Arduino.h>
#include <SD.h>

class SensorTest
{
public:
	//---------------------------------
	// Parses the next line of depug data
	//---------------------------------
	void Update();

	//---------------------------------
	// Gets the current displacement
	//---------------------------------
	float GetDis();

	//---------------------------------
	// Gets the current velocity
	//---------------------------------
	float GetVel();

	//---------------------------------
	// Gets the curren X acceleration
	//---------------------------------
	float GetAX();

  //---------------------------------
  // Initializes the SD card
  //---------------------------------
  void InitSD();

  //--------------------------------------------------------------------
  // Get velocity of pito. 
  // This take the velocity read in. If it is negative, it inverts it.
  //--------------------------------------------------------------------
  float GetPitoVel();

private:

	char* ReadLine();

	File file;
	float dis;
	float vel;
	float ax;


};

#endif
