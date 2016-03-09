#ifndef _SDCARD_H
#define _SDCARD_H

#include <Arduino.h>
#include <SD.h>

class SDCard
{
public:
	SDCard() {};
	// -----------------------------------------------------------------------------------
	// This method is called to update initialize the SD card reader/writer. 
	// It prints a message indicating whether it was successful or not.
	// -----------------------------------------------------------------------------------
	bool initializeSDCard();
	// -----------------------------------------------------------------------------------
	// This function initializes and opens the file that outputs telemetry data.
	// -----------------------------------------------------------------------------------
	String initializeDataFile();
	// -----------------------------------------------------------------------------------
	// This methods write the given apogee to the SD card.
	// If an old apogee file is recorded, the file is erased. A new apogee file is then 
	// created containing the new apogee.
	// -----------------------------------------------------------------------------------
	bool writeApogeeToSD(int apogee);
	// -----------------------------------------------------------------------------------
	// This methods writes data to the SD card data file.
	// -----------------------------------------------------------------------------------
	void writeToSD(int deltaTime, float dis, float vel, float acc);
	// -----------------------------------------------------------------------------------
	// This methods reads the apogee value from the SD card. 
	// It looks for a file name apogee.txt on the SD card. If it finds one, it reads
	// the entire file into a String. Then it returns the String to an integer using the
	// String.ToInt() method.
	// -----------------------------------------------------------------------------------
	int readApogeeFromSDCard();
private:
	const static int CS_PIN = 48; // pin on arduino connected to SD card
	const static int MIN_INT = -2147483648; // returns this if no apogee read from Card
	File data, apogeeFile;

};
#endif
