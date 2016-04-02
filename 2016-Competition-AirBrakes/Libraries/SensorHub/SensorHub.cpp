#include "SensorHub.h"

void SensorHub::initialize()
{
   //Dynamically allocating sensors just in case we want to add the functionality of 
   //creating more sensors on the fly and registering them with the SensorHub. 
   //For now, we simply create them here and add them to the array
   sensorList = new  Sensor*[SENSORS_LENGTH];



}

void SensorHub::update()
{

   for (int i = 0; i < SENSORS_LENGTH; i++)
   {

      sensorList[i]->update();

   }
 

}

