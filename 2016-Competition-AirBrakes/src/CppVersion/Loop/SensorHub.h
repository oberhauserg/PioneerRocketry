#include "Sensor.h"
#include "Stratologger.h"
#include "Pitot.h"
#include "BNO.h"
#include "Kalman.h"
#include "SensorTest.h"

#ifndef _SENSOR_HUB
#define _SENSOR_HUB

#define DEBUG_SENSORS

/**
* This class organizes all of the sensors we have 
* on the rocket into one neat place. 
* Author: Grant Oberhauser
*/

class SensorHub
{
	
public:
	SensorHub();
	
	// Return a value to a pointer 
	
	//static int* getGyroMeasurements();

	//float*  getAccelMeasurements() { return &acceleration; }

	//int* getAltitude();

	//int* getPitotTubeVelocity();

	
	// Makes the sensor hub update all of the sensor readings again
	int GetDis() { return disPito; }
	int GetDisBackUp() { return disStrato; }
	int GetDisRaw() { return disRaw; }
	float GetVel() { return velStrato; }
	float GetVelBackUp() { return velPito; }
	float GetVelRaw() { return velRaw; }
	float GetAcc() { return ax; }
	float GetAccRaw() { return axRaw; }

	float CalcDeltaT(){return (float)(currT - prevT) / MILLISEC_IN_SEC;}

	
	void Update();

private: 
	void FilterData();
	const static int NUM_SENSORS = 3;
	const static int MILLISEC_IN_SEC = 1000;

	Sensor * sensors[NUM_SENSORS];
  Stratologger st;
  Pitot pt;
  BNO bno;

	float ax,ay,az, axRaw;
	float velPito, velStrato, velRaw;
	int disPito, disStrato, disRaw;
   
	float deltaT;
  long prevT, currT;
	Kalman kal;

#ifdef DEBUG_SENSORS
	SensorTest senseT;
	int prevDistStrato;

#endif



};

#endif
