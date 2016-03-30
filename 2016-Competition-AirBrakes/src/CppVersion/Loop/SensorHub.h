#include "Sensor.h"

/**
* This class organizes all of the sensors we have 
* on the rocket into one neat place. 
* Author: Grant Oberhauser
*/
class SensorHub
{
	
public:

	/**
	* Return a value to a pointer 
	*/
	int* static getGyroMeasurements();

	int* static getAccelMeasurements();

	int static getAltitude();

	int static getPitotTubeVelocity();

	/**
	* Makes the sensor hub update all of the sensor readings again
	*/
	void static update();

private: 

   Sensor ** sensorList[3]; 

   //Filtered acceleration on the local y axis
   float acceleration;

   //Filtered velocity on the local y axis
   float velocity;

   //Filtered displacement on the global y axis
   int displacement;





};
