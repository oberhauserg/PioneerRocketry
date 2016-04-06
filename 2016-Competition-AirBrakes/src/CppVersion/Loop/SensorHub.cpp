#include "SensorHub.h"

SensorHub::SensorHub()
{
	deltaT = 0;
	prevT = currT = millis();
	sensors[0] = dynamic_cast<Sensor*>(&Stratologger());
	sensors[1] = dynamic_cast<Sensor*>(&Pitot());
	sensors[2] = dynamic_cast<Sensor*>(&BNO());
	for (int i = 0; i < NUM_SENSORS; i++)
	{
		sensors[i]->Initialize();
	}
	kal = Kalman();
}
void SensorHub::Update()
{
	for (int i = 0; i < NUM_SENSORS; i++)
	{
		sensors[i]->Update();
	}
	Stratologger * strato = (Stratologger*)(sensors[0]);
	disStrato = strato->GetDis();
	velStrato = strato->GetVel();

	Pitot * pito = (Pitot*)(sensors[1]);
	velPito = pito->GetVel();
	disPito = pito->GetDis();

	BNO * bno = (BNO*)(sensors[2]);
	ax = bno->GetAx();
	ay = bno->GetAy();
	az = bno->GetAz();

	deltaT = (float)(currT - prevT) / MILLISEC_IN_SEC;
	FilterData();
}

void SensorHub::FilterData()
{
	kal.update(&disStrato, &velPito, &ax, deltaT);
}
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
