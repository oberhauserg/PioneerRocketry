#include "Stratologger.h"

bool Stratologger::Initialize()
{
	Serial2.begin(9600);
	long endTime = millis() + DELAY_FOR_STRATO;
	while (Serial2.available() <= 0 && millis() < endTime); // loop until we receive data from stratologger
	prevTime = currTime = millis();
	if (millis() >= endTime)
		return false;
	else
	{
		int altAboveSea = Serial2.parseInt();
		return true;
	}
}

void Stratologger::Update()
{
	if (Serial2.available() > 0)
	{
		dis = Serial2.parseInt();
		vel = CalcVel();
		haveStratoData = true;
	}
	else
	{
		haveStratoData = false;
	}
}

float Stratologger::CalcVel()
{
	float temp = CalcDeltaT();
	if (temp == 0.0f)
		return (float)(dis - prevDis) / temp;
	else
		return 0;
}

float Stratologger::CalcDeltaT()
{
	prevTime = currTime;
	currTime = millis();
	return (float)(currTime - prevTime) / MILLISEC_IN_SEC;
}