#ifndef _SDCARD_H
#define _SDCARD_H

#include <Arduino.h>
#include <SD.h>

class SDCard
{
public:
	SDCard() {};
	bool initializeSDCard();
	bool initializeDataFile();
	bool writeApogeeToSD(int apogee);
	bool writeToSD(int deltaTime, float dis, float vel, float acc);
private:
	const static int CS_PIN = 48;
	File dataFromLaunch;
};



#endif
