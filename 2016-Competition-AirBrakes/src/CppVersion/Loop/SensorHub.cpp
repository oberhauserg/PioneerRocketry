#include "SensorHub.h"
/*
void SensorHub::update()
{

  //Calculate delta time
  deltaTime = millis() - lastLoopTime;

  lastLoopTime = millis();

  //Get pitot velocity
  int sensorValue = analogRead(pitoAnalogPin);
  float pinRatio = (float) sensorValue / (float) Constants::MAX_12_INT;
  havePitoData = true;

  lastPitoVel = pitoVel;
  pitoVel = pinRatio * Constants::PITOT_GAIN + Constants::PITOT_ZERO;

  //Get altitude from strattologger
  if(Serial2.available() > 0)
  {
  	lastStratoAlt = stratoAlt;
  	stratoAlt = Serial2.parseInt();
  }

  //Calculating redundant velocity from the altimeter.
  redundantVel = ((float)stratoAlt - lastStratoAlt) * (((float)deltaTime)/MILLIS_TO_BASE));

  

}

bool SensorHub::setup()
{
	long endTime = millis() + Constants::DELAY_FOR_STRATO;

  	while(Serial2.available() <= 0 && millis() < endTime); // loop until we receive data from stratologger
  	
  	//Check if we timed out
  	if(millis() < endTime)
  	{
  		stratoZero = Serial2.parseInt();
  		return true;
  	}
  	else
  	{ 
  		stratoZero = -1;
  		return false;
  	}
}*/
