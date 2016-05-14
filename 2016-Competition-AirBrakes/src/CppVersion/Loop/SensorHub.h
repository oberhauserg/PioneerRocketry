#include "XBee.h"
#include "Sensor.h"
#include "Stratologger.h"
#include "Macros.h"
#include "BNO.h"
#include "Pitot.h"
#include "Kalman.h"
#include "SensorTest.h"


#ifndef _SENSOR_HUB
#define _SENSOR_HUB



/**
* This class organizes all of the sensors we have 
* on the rocket into one neat place. 
* Author: Grant Oberhauser
*/

class SensorHub
{
	
public:
	void Initialize();
	float CalcDeltaT();//{return (float)(currT - prevT) / MILLISEC_IN_SEC;}
	void Update();

  int GetDis() { return disStrato; }
  int GetDisBackUp() { return disStrato; }
  int GetDisRaw() { return disRaw; }
  float GetVel() { return velPito; }
  float GetVelBackUp() { return velStrato; }
  float GetVelRaw() { return velRaw; }
  float GetAcc() { return az; }
  float GetAy() { return ay; }
  float GetAx() { return ax; }
  float GetAccRaw() { return axRaw; }

private: 
	void FilterData();
	const static int NUM_SENSORS = 3;
	const static int MILLISEC_IN_SEC = 1000;
  const static int MIN_INT = -2147483648;
  const float P0 = 101.325;
  const float STATIC_P_CONST = 0.00038897965879265;
  

  bool haveStratoData = false;
  bool havePitotData = false;

  XBee xb;
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
