#include "SensorHub.h"



void SensorHub::Initialize()
{
	xb.InitializeXBee();
  delay(1000);
	deltaT = 0;
	prevT = currT = millis();
	sensors[0] = dynamic_cast<Sensor*>(&st);
	sensors[1] = dynamic_cast<Sensor*>(&pt);
	sensors[2] = dynamic_cast<Sensor*>(&bno);
	sensors[0]->SetID("Stratologger");
	sensors[1]->SetID("Pitot Sensor");
	sensors[2]->SetID("BNO055");
	for (int i = 0; i < NUM_SENSORS; i++)
	{
		xb.SendMessage("Attempting to intialize " + sensors[i]->GetID() + "\n");
		if(sensors[i]->Initialize())
			xb.SendMessage(sensors[i]->GetID() + " initialized\n");
		else
			xb.SendMessage(sensors[i]->GetID() + " failed to initialize\n");
	}
#ifdef DEBUG_SENSORS

   senseT.InitSD();

#endif
	
}
void SensorHub::Update()
{
  prevT = currT;
  currT = millis();
  deltaT = (float)(currT - prevT) / MILLISEC_IN_SEC;

#ifndef DEBUG_SENSORS
  
	for (int i = 0; i < NUM_SENSORS; i++)
	{
		sensors[i]->Update();
	}
	Stratologger * strato = (Stratologger*)(sensors[0]);
	disStrato = strato->GetDis();
	velStrato = strato->GetVel();

  //Pitot returns pressure. 
  //Velocity requires displacement.
	Pitot * pito = (Pitot*)(sensors[1]);
  float pressure = pito->GetPressure();
	//velPito = pito->GetVel();
	//disPito = pito->GetDis();

  float staticPressure = P0 * (1 - STATIC_P_CONST * disStrato); 
  
  float unsquared = 293 * (pow(((pressure)/P0 + 1), 0.286) - 1);
 
  velPito = 44.83 * 3.28084 * sqrt(unsquared);

  disPito += velPito * CalcDeltaT();
 
#ifdef NO_PITOT
  velPito = velStrato;
  disPito = disStrato;
#endif

	BNO * bno = (BNO*)(sensors[2]);
	ax = bno->GetAx();
	ay = bno->GetAy();
	az = bno->GetAz();
#endif

#ifdef DEBUG_SENSORS

  senseT.Update();
  //Get disStrato here
  disRaw = disStrato = senseT.GetDis();
  //xb.SendMessage("readValue is " + String(disStrato) + "\n");
  
  velStrato = velRaw = velPito = senseT.GetVel();
  velPito = senseT.GetPitoVel();
  axRaw = az = senseT.GetAX();
  ay = ax = 0;
  deltaT = 0.05;

/*
  if (deltaT == 0.0f)
    velStrato = (float)(disStrato - prevDisStrato) / deltaT;
  else
    velStrato = 0;
*/
#endif

  disRaw = disStrato;
  velRaw = velPito;
  axRaw = az;
	FilterData();
}

void SensorHub::FilterData()
{
	kal.Update(&disStrato, &velPito, &az, deltaT);
}

float SensorHub::CalcDeltaT()
{
#ifndef DEBUG_SENSORS
  return deltaT; //(float)(currT - prevT) / MILLISEC_IN_SEC;
#else
  return deltaT;
#endif
}
