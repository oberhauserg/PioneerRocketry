#include "SDCard.h"
// -----------------------------------------------------------------------------------
// This method is called to update initialize the SD card reader/writer. 
// It prints a message indicating whether it was successful or not.
// -----------------------------------------------------------------------------------
bool SDCard::InitializeSDCard()
{
	return SD.begin(CS_PIN);
}

// -----------------------------------------------------------------------------------
// This method initializes the data file for the telemetry.
// The file name is of the format dataX.csv where X is an integer. This prevents 
// deleting old files or having mulitple concatination onto the same data file.
// It inserts a header with all the varaible types on the top  of the file.
// -----------------------------------------------------------------------------------
String SDCard::InitializeDataFile()
{
	String fileName;
	int count = 1;
	do
	{
		fileName = "data";
		fileName.concat(count++);
		fileName.concat(".csv");
	} while (SD.exists(fileName));

	data = SD.open(fileName, FILE_WRITE);
	if (data)
	{
		data.println("Time(milliSec), PitoVelocity, StratoDisplacement, accel X");
		data.flush();
	}
	return fileName;
}

// -----------------------------------------------------------------------------------
// This methods write the given apogee to the SD card.
// If an old apogee file is recorded, the file is erased. A new apogee file is then 
// created containing the new apogee.
// -----------------------------------------------------------------------------------
bool SDCard::WriteApogeeToSD(int height)
{
	// delete old apogee
	SD.remove("apogee.txt");
	apogeeFile = SD.open("apogee.txt", FILE_WRITE);
	// if the file opened okay, write to it:
	if (apogeeFile)
	{
		apogeeFile.println(String(height));
		apogeeFile.close();
	}
}

// -----------------------------------------------------------------------------------
// This methods writes data to the SD card data file.
// -----------------------------------------------------------------------------------
void SDCard::WriteToSD(int deltaTime, float dis, float vel, float acc)
{
	String info = String(deltaTime) + "," + String(vel) + ","
						 + String(dis) + "," + String(acc) + "\n";
	// if the file opened okay, write to it:
	if (data)
	{
		data.println(String(info));
		data.flush();
	}
}

// -----------------------------------------------------------------------------------
// This methods reads the apogee value from the SD card. 
// It looks for a file name apogee.txt on the SD card. If it finds one, it reads
// the entire file into a String. Then it returns the String to an integer using the
// String.ToInt() method.
// -----------------------------------------------------------------------------------
int SDCard::ReadApogeeFromSDCard()
{
	apogeeFile = SD.open("apogee.txt");
	if (apogeeFile)
	{
		String apogeeString;
		while (apogeeFile.available())
		{
			char temp = apogeeFile.read();
			apogeeString = apogeeString + String(temp);
		}
		apogeeFile.close();
		return apogeeString.toInt();
	}
	else
	{
		return MIN_INT;
	}
}