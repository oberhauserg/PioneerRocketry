// includes for reading and writing to SD Card

//#include "Kalman.h"
//#include <SD.h>

#include "SDCard.h"
#include "XBee.h"
#include "SensorHub.h"
#include "AirBrake.h"

// classes
SDCard sd;
AirBrake ab;
XBee xbee;
SensorHub sh;

// variables
int apogeeGoal = 0;
int apogeeReached = 0;
bool airBrakessActive = false;
const static int MIN_INT = -2147483648;



void setup() 
{
  //Serial.begin(9600);
  
  xbee.InitializeXBee();
  sh.Initialize();
  
  ab.Initialize();
  
  sd.InitializeSDCard();
  apogeeGoal = sd.ReadApogeeFromSDCard();
  xbee.SendMessage("Opening Apogee file.\n");
  if(apogeeGoal ==  MIN_INT)
  {
    xbee.SendMessage("Error reading apogee from SD card.\n");
    apogeeGoal = 0;
  }

  apogeeGoal = xbee.InitializeApogee(apogeeGoal);
  airBrakessActive = sd.AirBrakesActive();
  ab.SetActive(sd.AirBrakesActive());
  xbee.SendMessage(sd.InitializeDataFile() + "\n");
  sh.Update();
  xbee.SendMessage("SequenceNumber,Time(milliSec),Altitude(feet),Velocity(fps)\n");

  //SensorTest st; 
  //xbee.SendMessage("Dis was " + String(st.GetDis() + "\n");
  //xbee.SendMessage("Dis2 was " + String(st.GetDis() + "\n");
  //delay(
}

void loop() 
{
  
  sh.Update();
  int dis = sh.GetDis();
  float vel = sh.GetVel();
  sd.WriteToSD(sh.CalcDeltaT(), sh.GetDisRaw(), sh.GetVelRaw(), sh.GetAccRaw());
  xbee.SendData(dis, vel, millis());
  ab.Update(dis, vel, sh.GetAcc()); 
  
}




