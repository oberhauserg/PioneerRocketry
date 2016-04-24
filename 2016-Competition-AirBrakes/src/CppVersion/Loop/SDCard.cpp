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
		data.println("Time(milliSec), PitoVelocity, StratoDisplacement, accel X, accel Y, accel Z, filtDis, filtVel, filtAx, airBrakePercent");
		data.flush();
   return fileName + String(" opened succesfully");
	}
	return String("Error opening SD card");
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
void SDCard::WriteToSD(int deltaTime, float dis, float vel, float ax, float ay, float az, float filteredDis, float filteredVel, float filteredAx, int airBrakePercent)
{
	String info = String(deltaTime) + "," + String(vel) + "," + String(dis) 
						 + "," + String(ax) + "," + String(ay) + "," + String(az) + "," 
						 + String(filteredDis) + "," + String(filteredVel) + "," 
             + String(filteredAx) + "," + String(airBrakePercent) +
						 "\n";
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

bool SDCard::AirBrakesActive()
{
  File sdFile = SD.open("break.txt");
  if(sdFile)
  {
    char temp = sdFile.read();
    if(temp == 'Y' || temp == 'y')
    {
      return true;
    }
  }
  return false;
}

// -----------------------------------------------------------------------------------
  // This method reads a message to the SD card.
  // -----------------------------------------------------------------------------------
  void SDCard::WriteToDataFile(String msg)
  {
    if (data)
    {
      data.print(msg);
      data.flush();
    }
  }
  

