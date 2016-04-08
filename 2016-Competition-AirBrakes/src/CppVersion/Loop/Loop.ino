// includes for reading and writing to SD Card

//#include "Kalman.h"
//#include <SD.h>

#include "SDCard.h"
#include "XBee.h"
#include "SensorHub.h"
#include "AirBrake.h"

// classes
SDCard sd;
//SensorHub sh;
AirBrake ab;
XBee xbee;
SensorHub sh;

// variables
int apogeeGoal = 0;
int apogeeReached = 0;

void setup() {
  //Serial.begin(9600);
  
  xbee.InitializeXBee();
  sh.Initialize();
  /*
  ab.Initialize();
  
  sd.InitializeSDCard();
  apogeeGoal = sd.ReadApogeeFromSDCard();
  ab.SetActive(sd.AirBrakesActive());
  sd.InitializeDataFile();
  
  
  sh.Update();
  */
}

void loop() {
  /*
  sh.Update();
  int dis = sh.GetDis();
  float vel = sh.GetVel();
  sd.WriteToSD(sh.CalcDeltaT(), sh.GetDisRaw(), sh.GetVelRaw(), sh.GetAccRaw());
  xbee.SendData(dis, vel, millis());
  ab.Update(dis, vel, sh.GetAcc()); */
}
