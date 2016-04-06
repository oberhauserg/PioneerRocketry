#include "Pitot.h"

void Pitot::update()
{

   //analogReadResolution(12);
   int sensorValue = analogRead(pitoAnalogPin);
   float pinRatio = (float)sensorValue / (float)MAX_12_INT;
   havePitoData = true;
   return pinRatio;

}

float Pitot::getScalar()
{
   

   
}