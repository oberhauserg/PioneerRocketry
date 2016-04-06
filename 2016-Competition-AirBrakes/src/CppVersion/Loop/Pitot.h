#include "Sensor.h"

#define MAX_12_INT 1024

/**
* 
*/
class Pitot : private Sensor
{

   bool initialize();

   void update();

   float getScalar();

private:

   float velocity;
   float rawVoltage;

};