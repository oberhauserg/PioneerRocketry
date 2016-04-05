#ifndef _BNO_H
#define _BNO_H

#include "..\Sensor\Sensor.h"
#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>

class BNO : public Sensor
{
public:
	BNO() { ax = ay = az = 0; }
	bool Initialize();
	void Update();
	float GetAx() { return ax; }
	float GetAy() { return ay; }
	float GetAz() { return az; }
private:
	float ax, ay, az;
	Adafruit_BNO055 bno;
};

#endif