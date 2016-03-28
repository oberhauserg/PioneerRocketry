#include "XBee.h"
// -----------------------------------------------------------------------------------
// This method sends a message to the XBee. 
// It allows the user to change the serial port the XBee is connected with a minimal
// ammount of modified code. Does not include newline, so user will have to hardcode 
// those in.
// -----------------------------------------------------------------------------------
void XBee::SendMessage(String msg)
{
#ifdef USING_SERIAL_MONITOR
	Serial.print(msg);
#else // using XBee
	Serial1.print(msg);
#endif
}
// -----------------------------------------------------------------------------------
// This method opens the serial ports being used to communicate on.
// If the USING_SERIAL_MONITOR is uncommnted, then the method initializes Serial.
// at a 9600 baud rate. Otherwise, the method initializes Serial1 at a 9600 baud rate.
// -----------------------------------------------------------------------------------
void XBee::InitializeXBee()
{
#ifdef USING_SERIAL_MONITOR
	Serial.begin(9600);
#else
	Serial1.begin(9600);
#endif
}


// -----------------------------------------------------------------------------------
// This method is called to receive a message via the XBee.
// It waits TIME_DELAY seconds for the message from the user. If a message is not 
// recieved in that time, an error message is sent to the XBee and the String
// "TIME OUT" is returned. Otherwise, the String is read from the XBee returned.
// -----------------------------------------------------------------------------------
String XBee::ReceiveMessage()
{
	long endTime = millis() + TIME_DELAY;
	// used for if you don't want to use XBee
#ifdef USING_SERIAL_MONITOR
	while (Serial.available() <= 0 && (millis() <= endTime));
	if (Serial.available() > 0)
	{
		String message = Serial.readString();
		return message;
	}
	else
	{
		SendMessage("\nTimeout occured! Did not recieve you message.\n");
		return "TIME OUT";
	}
#else // using XBee
	while (Serial1.available() <= 0 && (millis() <= endTime));
	if (Serial1.available() > 0)
	{
		String message = Serial1.readString();
		return message;
	}
	else
	{
		SendMessage("\nTimeout occured! Did not recieve you message.\n");
		return "TIME OUT";
	}
#endif
}

