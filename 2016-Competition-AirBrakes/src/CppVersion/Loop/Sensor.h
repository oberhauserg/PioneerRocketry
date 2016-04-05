#ifndef _SENSOR_H
#define _SENSOR_H
#include <arduino.h>

class Sensor
{
public:
	bool Initialize();
	void Update();
	String GetID() { return id; }
	void SetID(String _id) {id = _id;}
private:
	String id;
};

#endif _SENSOR_H
