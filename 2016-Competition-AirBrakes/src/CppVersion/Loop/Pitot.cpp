#include "Pitot.h"

void Pitot::Update()
{
	havePitotData = false;
	prevTime = currTime;
	currTime = millis();

	//Ensure a delay for at least 40 millis to allow the slave to make calculations
	if(millis() % 100 < 60)
	{
	  
		char buf [10];
		 
		if (Wire.requestFrom (8, 10))
		{
			for (byte i = 0; i < 10; i++)\
		    	buf [i] = Wire.read ();
		     
		      
		    vel = (float)atof(buf);    
		}
		else
		{
		    // The request failed. Just continue requesting.
		}

	}

	//int sensorValue = analogRead(PITO_ANALOG_PIN);
	vel = (float)sensorValue / (float)MAX_12_INT;
	CalcDis();
	
}

void Pitot::CalcDis()
{
	if (havePitotData)
	{
		localPitoDisplacement = localPitoDisplacement + GetDeltaT() * vel;
		dis = (int)localPitoDisplacement;
	}
}

float Pitot::GetDeltaT()
{
	return (float)(currTime - prevTime) / MILLISEC_IN_SEC;
}
