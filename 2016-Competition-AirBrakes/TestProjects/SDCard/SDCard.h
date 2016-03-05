#ifndef _SDCARD_H
#define _SDCARD_H

#include <string>
#include "SD/src/utility"
#include "SD/src/SD.h"

using namespace std;

class SDCard
{
public:
   SDCard();
   void writeToSD(int deltaTime, int dis, float vel, float accel);
   void writeApogeeToSD(int apogee);
   void readApogeeFromSDCard();
   void initializeSDCard();
   void sendMessage(string msg);
private:
   string apogeeFileName, dataFile;
   int csPin;
};


#endif