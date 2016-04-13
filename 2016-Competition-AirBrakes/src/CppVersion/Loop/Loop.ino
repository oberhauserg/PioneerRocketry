// includes for reading and writing to SD Card

//#include "Kalman.h"
//#include <SD.h>

#include "SDCard.h"
#include "XBee.h"
#include "SensorHub.h"
#include "AirBrake.h"
#include "Event.h"

// classes
SDCard sd;
AirBrake ab;
XBee xbee;
SensorHub sh;
Event event;

// variables
int apogeeGoal = 0;
int apogeeReached = 0;
bool airBrakessActive = false;
const static int MIN_INT = -2147483648;

bool landed = true;
bool engineBurning = false;
bool descending = false;



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
}

bool ap = false;
bool lft = false;
bool lnd = false;

void loop() 
{
  
  sh.Update();
  int dis = sh.GetDis();
  float vel = sh.GetVel();
  sd.WriteToSD(sh.CalcDeltaT(), sh.GetDisRaw(), sh.GetVelRaw(), sh.GetAccRaw());
  xbee.SendData(dis, vel, millis());
  ab.Update(dis, vel, sh.GetAcc()); 
  event.Update(dis,vel,sh.GetVelBackUp(),sh.GetAcc());
  if(landed)
  {
    if(event.HasBurnedOut())
    {
      landed = false;
      engineBurning = true;
      xbee.SendMessage("Burnout Occured.\n");
    }
  }
  else if(engineBurning)
  {
    if(event.HasReachedApogee())
    {
      descending = true;
      engineBurning = false;
      apogeeReached = event.GetApogee();
      xbee.SendMessage("Apogee read at " + String(apogeeReached) + ".\n");
      sd.WriteApogeeToSD(apogeeReached);
    }
  }
  else
  {
    if(event.HasLanded())
    {
      descending = false;
      landed = true;
      xbee.SendMessage("Rocket has Landed.\n");
    }
  }
  /*
//bool engineBurning = false;
//bool descending
  //xbee.SendMessage("vel is " + String(sh.GetVelBackUp()) + "\n");
  if(event.HasBurnedOut() )
  {
    if(!lft)
    {
      lnd = false;
      lft = true;
      xbee.SendMessage("Burned out\n");
    }
  }
  if(event.HasReachedApogee())
  {
    if(!ap)
    {
      lft = false;
      ap = true;
      xbee.SendMessage("Reached apogee " + String(event.GetApogee()) + "\n");
    }
    
  }
  if(event.HasLanded())
  {
    if(!lnd)
    {
      ap = false;
      lnd = true;
      xbee.SendMessage("Landed\n");
    }
  }
  */
  
}




