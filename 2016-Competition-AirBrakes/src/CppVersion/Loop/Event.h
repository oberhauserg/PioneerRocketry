#ifndef _EVENT_H
#define _EVENT_H

#include "Macros.h"
#include "XBee.h"

#ifdef TESTING_EVENT

#include <iostream>
using namespace std;

#endif

#include <cmath>

class Event
{
public:
	// initializes all events to 0
	Event();
	// reads values into next section of array
	void Update(int newDis, float newVel, float calcVel, float newAcc);
	bool HasBurnedOut() { return burnedOut; }
	bool HasReachedApogee() { return descending; }
	// returns MIN_INT if apogee not yet recorded
	int GetApogee() { return apogee; }
	bool HasLanded() { return landed; }
	

private:
	const static int MIN_INT = -2147483648;
	const static int NUM_SAMPLES = 8;
	const static int BURNOUT_HEIGHT = 220;
	const static int LANDED_DEVIATION = 1;
  const static int NUM_DIS_APOGEE_SAMPLES = 20;
  const static int DESC_VEL_INDICATOR = -13;

	float CalcAveDis();
	float CalcAveVel();
	float CalcAveAcc();
	float CalcAveCalcVel();

	void CalcApogee();
	void IncrementIndex();
	bool IsLanded();
	bool IsBurnedOut();
	bool IsDescending();
	float CalcStandardDeviation(int *array);

	float calcVel[NUM_SAMPLES];
	int dis[NUM_SAMPLES];
	float vel[NUM_SAMPLES];
	float acc[NUM_SAMPLES];
  int disForApogee[NUM_DIS_APOGEE_SAMPLES];
	float aveDis, aveVel, aveAcc, aveCalcVel;
	int index = 0;
  int indexDis = 0;
	int apogee = MIN_INT;
	// event bools
	bool landed, burnedOut, descending;

 #ifdef TESTING_EVENT_
 XBee xb;
 #endif
};

#endif
