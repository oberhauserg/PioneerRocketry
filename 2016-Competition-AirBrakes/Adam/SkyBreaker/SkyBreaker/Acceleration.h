#ifndef _ACCELERATION_H
#define _ACCELERATION_H
#define ARDUINO 9

#include "Sensor.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

class acceleration: public Sensor
{
public:
private:
   
   Adafruit_BNO055 bno = Adafruit_BNO055();

   float xAccel;
   float yAccel;
   float zAccel;
}

#endif