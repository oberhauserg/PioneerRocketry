#include "Pitot.h"

void Pitot::Update()
{
	havePitotData = true;
	prevTime = currTime;
	currTime = millis();
	int sensorValue = analogRead(PITO_ANALOG_PIN);
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