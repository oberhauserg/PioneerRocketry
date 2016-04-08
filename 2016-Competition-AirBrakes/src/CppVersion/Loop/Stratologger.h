#ifndef _STRATOLOGGER_H
#define _STRATOLOGGER_H
#include "Sensor.h"

#define _DONT_WAIT_FOR_STRATO

// --------------------------------------------------------------------------------------
// This class holds all the values and methods for the Stratologger.
// --------------------------------------------------------------------------------------
class Stratologger : public Sensor
{
public:
	// -----------------------------------------------------------------------------------
	// This method is called to initialize the stratologger.
	// It reads the first value and sets that as the altitude above sea level. This 
	// prevents a sudden jump in altitude causing the liftoff event to be triggered.
	// -----------------------------------------------------------------------------------
	bool Initialize();
	// -----------------------------------------------------------------------------------
	// This method is called to update the vel and dis values of the class.
	// -----------------------------------------------------------------------------------
	void Update();
	// -----------------------------------------------------------------------------------
	// This method is returns the stored alitude above sea level.
	// If this has not been stored, the MIN_INT for the DUE is returned -2147483648.
	// -----------------------------------------------------------------------------------
	int GetAltAboveSeaLevel() { return altAboveSea; }
	// -----------------------------------------------------------------------------------
	// This method returns the stored velocity.
	// -----------------------------------------------------------------------------------
	float GetVel() { return vel; }
	// -----------------------------------------------------------------------------------
	// This method returns the stored displacement.
	// -----------------------------------------------------------------------------------
	int GetDis() { return dis; }
	// -----------------------------------------------------------------------------------
	// This method returns whether the stratologger recieved data the during the last
	// update method called.
	// -----------------------------------------------------------------------------------
	bool HaveData() { return haveStratoData; }
private:
	// -----------------------------------------------------------------------------------
	// This method returns the velocity calculated from the displacement and delta time.
	// This data is very noise.
	// -----------------------------------------------------------------------------------
	float CalcVel();
	// -----------------------------------------------------------------------------------
	// This method calculates the time since the last update.
	// -----------------------------------------------------------------------------------
	float CalcDeltaT();

	const static long DELAY_FOR_STRATO = 120000; // time for strattologger to initialize
	const static int MIN_INT = -2147483648;
	const static int MILLISEC_IN_SEC = 1000;

	int altAboveSea = MIN_INT;
	int prevDis = 0;
	int dis = 0;
	float vel = 0;
	long prevTime, currTime = 0;
	bool haveStratoData = false;

};

#endif
