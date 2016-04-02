#include "Sensor.h"

#define SENSORS_LENGTH 3

/**
* This class organizes all of the sensors we have 
* on the rocket into one neat place. 
* Author: Grant Oberhauser
*/
class SensorHub
{
	
public:


   void static initialize();

	/**
	* Makes the sensor hub update all of the sensor readings again
	*/
	void static update();

   float getAccel(){ return acceleration; }

   float getVelocity(){ return velocity; }

   int getDisp(){ return displacement; }

private: 

   /**
   * Pass in the current raw acceleration, velocity, and displacement
   *
   * returns: the filtered acceleration, velocity, and displacement
   */
   void static filter(float &a , float &v, int &d);

   Sensor static ** sensorList; 

   //Filtered acceleration on the local y axis
   float acceleration;

   //Filtered velocity on the local y axis
   float velocity;

   //Filtered displacement on the global y axis
   int displacement;


};
