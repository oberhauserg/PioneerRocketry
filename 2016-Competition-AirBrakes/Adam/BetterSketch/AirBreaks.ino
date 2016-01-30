// -----------------------------------------------------------------------------------
// Any functions directly operating with the airbreaks.
//
// Functions
// void openAirBreaks()
// void controlAirBreaks()
// -----------------------------------------------------------------------------------

#include <Servo.h>
#include "AirBrakes.h"
#include "Controller.h"


// -----------------------------------------------------------------------------------
// This function initializes the Air Brakes. 
// -----------------------------------------------------------------------------------
int initialPercent = 0;
AirBrakes * brakes;

void initializeAirBreak()
{
  //Initialize AirBrakes module
  brakes = new AirBrakes();

  brakes->setDeploymentPercentage(initialPercent);
  brakes->update();

  //Initialize controller module
  //cont = new Controller(*brakes); // Kills program!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

void updateAirBrakes()
{
  brakes->update();
}

int finalPercent = 100;

void openAirBreaks()
{
  timeToTriggerAirbreak = 2147483647; // Max long prevents being triggered
                                      // more than once.
  brakes->setDeploymentPercentage(finalPercent);
  if(airBreaking)
    sendMessage("Opening Air Brakes\n");
  else
    sendMessage( String(millis()) + " Air Brake Point\n");
}

// -----------------------------------------------------------------------------------
// This function checks to see if airbreaks are on or off during this sketch.
// It checks the SD card for a file "BREAK". If this file exhists and the first
// character in the file is 'y', then the airbreaks are turned on.
// -----------------------------------------------------------------------------------
File sdFile;
void checkAirBreaks()
{
  sendMessage("Attempting to open checkAirBreak file...\n");
  sdFile = SD.open("break.txt");
  if(sdFile)
  {
    char temp = sdFile.read();
    if(temp == 'Y' || temp == 'y')
    {
      airBreaking = true;
    }
    else
    {
      airBreaking = false;
    }
  }
  else
  {
    sendMessage("Didn't find file.\n");
    airBreaking = false;
  }
}

void controlAirBreaks()
{
  //............................................................................................??????????????????
}
