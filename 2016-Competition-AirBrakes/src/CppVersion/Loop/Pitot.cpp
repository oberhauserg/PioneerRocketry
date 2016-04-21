#include "Pitot.h"

bool Pitot::Initialize()
{
  Wire.begin();
  digitalWrite(RESET_PIN, HIGH);
  return true;
}
void Pitot::Update()
{
  digitalWrite(RESET_PIN, HIGH);
  havePitotData = false;
  prevTime = currTime;
  currTime = millis();

  char buf [10];

  if (Wire.requestFrom (9, 10))
  {
    curFails = 0;
    for (byte i = 0; i < 10; i++)
      buf [i] = Wire.read ();

    pressure = (float)atof(buf);

    pressure += DISPLACED;

    havePitotData = true;
    
  }
  else
  {
    curFails++;
    // The request failed. Just continue requesting.
  }

  if(curFails >= 3)
    digitalWrite(RESET_PIN, LOW);
    
  //int sensorValue = analogRead(PITO_ANALOG_PIN);
  //vel = (float)sensorValue / (float)MAX_12_INT;
  //CalcDis();

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

float Pitot::GetVel()
{
  return pressure;
  /*
  Wire.requestFrom(8, 10);

  char buf [10];

  if (Wire.requestFrom (8, 10))
  {
    for (byte i = 0; i < 10; i++)\
      buf [i] = Wire.read ();


    return (float)atof(buf);
  }
  else
  {
    return 0;
  }*/

}


