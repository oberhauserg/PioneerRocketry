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
const static int MIN_INT = -2147483648;
const static int ALT_PERCENT = 75;
const static int DENOMINATOR_OF_PERCENT = 100;
const static int MIN_AIRBRAKE_VELOCITY = 20; // fps


void setup() 
{
  
  xbee.InitializeXBee();
  sh.Initialize();
  ab.Initialize();
  sd.InitializeSDCard();

  // sets apogee goal
  xbee.SendMessage("Opening Apogee file.\n");
  apogeeGoal = (sd.ReadApogeeFromSDCard() * ALT_PERCENT) / DENOMINATOR_OF_PERCENT;
  if(apogeeGoal ==  MIN_INT) // no apogee read
  {
    xbee.SendMessage("Error reading apogee from SD card.\n");
    apogeeGoal = 0;
  }
  apogeeGoal = xbee.InitializeApogee(apogeeGoal); // input apogee via xbee
  
  // sets air brakes active or inactive
  ab.SetActive(sd.AirBrakesActive()); // checks sd card for apogee file
  if(ab.IsActive())
    xbee.SendMessage("AirBrakes are ACTIVE\n");
  else
    xbee.SendMessage("AirBrakes are NOT active\n");
    
  xbee.SendMessage(sd.InitializeDataFile() + "\n"); // let user know what data file name is
  sh.Update();
  xbee.SendMessage("SequenceNumber,Time(milliSec),Altitude(feet),Velocity(fps)\n");
}

// event memory
bool landed = true;
bool burnedOut = false;
bool descending = false;

void loop() 
{
  //Sensor Hub
  sh.Update();
  int dis = sh.GetDis();
  float vel = sh.GetVel();
  float acc = sh.GetAcc();

  // update events
  event.Update(dis,vel,sh.GetVelBackUp(),sh.GetAcc());
  if(event.HasBurnedOut() && !burnedOut)
  {
     xbee.SendMessage("Burnout\n");
     burnedOut = true;
     landed = false;
     if(ab.IsActive())
     {
      ab.OpenBrakes(); // initially set to max
      // update loop will take over later but we want to open quickly
      xbee.SendMessage("Opening AirBrakes\n");
     }
     else
       xbee.SendMessage("AirBrake Point\n");
     
  }
  else if(event.HasReachedApogee() && !descending)
  {
    apogeeReached = event.GetApogee();
    sd.WriteApogeeToSD(apogeeReached);
    xbee.SendMessage("Apogee " + String(apogeeReached) + "\n");
    descending = true;
    burnedOut = false;
  }
  else if(event.HasLanded() && !landed)
  {
    xbee.SendMessage("Landed\n");
    landed = true;
    descending = false;
    
  }

  if(burnedOut && ab.IsActive())
  {
    if( vel < MIN_AIRBRAKE_VELOCITY)
    {
      ab.CloseBrakes();
      xbee.SendMessage("Close AirBrakes\n");
    }
    else
    {
      ab.Update(dis, vel, sh.CalcDeltaT(), apogeeGoal);
    }
  }
  else // Landed || Descending || burnedOut && AirBrakes inactive
    ab.CloseBrakes();

  // record data
  sd.WriteToSD(sh.CalcDeltaT(), sh.GetDisRaw(), sh.GetVelRaw(), sh.GetAccRaw());
  xbee.SendData(dis, vel, millis());
}




