#include "Pitot.h"
#include "Constants.h"

void Pitot::update()
{

   //update the velocity variable with the new velocity
   int sensorValue = analogRead(pin);
   float pinRatio = (float)sensorValue / (float)MAX_12_INT;

   //This probably needs changing
   velocity = pinRatio;

}