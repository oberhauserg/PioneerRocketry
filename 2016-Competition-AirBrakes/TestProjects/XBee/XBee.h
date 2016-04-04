#ifndef _XBEE_H
#define _XBEE_H

#include <Arduino.h>
#include <Wire.h>

// -----------------------------------------------------------------------------------
// This Class provides features of providing feedback from the Arduino.
// It can send data either to the XBee or to the Serial port to be read by the 
// serial monitor. The sendData methods send altimeter data with a sequence number.
// The sendMessage method directly sends the String.
// -----------------------------------------------------------------------------------

// switches from sending to XBee to sending to Serial Monitor
//#define USING_SERIAL_MONITOR
class XBee
{
public:
	// -----------------------------------------------------------------------------------
	// This method opens the serial ports being used to communicate on.
	// If the USING_SERIAL_MONITOR is uncommnted, then the method initializes Serial.
	// at a 9600 baud rate. Otherwise, the method initializes Serial1 at a 9600 baud rate.
	// -----------------------------------------------------------------------------------
	void InitializeXBee();
	// -----------------------------------------------------------------------------------
	// This method sends a message to the XBee. 
	// It allows the user to change the serial port the XBee is connected with a minimal
	// ammount of modified code. Does not include newline, so user will have to hardcode 
	// those in.
	// -----------------------------------------------------------------------------------
	void SendMessage(String msg);

	// -----------------------------------------------------------------------------------
	// This method is called to receive a message via the XBee.
	// It waits timeDelay seconds for the message from the user. If a message is not 
	// recieved in that time, an error message is sent to the XBee and the String
	// "TIME OUT" is returned. Otherwise, the String is read from the XBee returned.
	// -----------------------------------------------------------------------------------
	String ReceiveMessage();

	// -----------------------------------------------------------------------------------
	// This method allows the user to modify the apogee via the XBee.
	// the user has the option to modify this value via the XBee. It has a while loop that
	// will loop through a maximum of 10 times. Each time, the user can modify the 
	// apogee. Then they verify that it was correctly input. It returns the apogee 
	// corrected by the user. If no apogee is entered, then it returns the initial value.
	// -----------------------------------------------------------------------------------
	int InitializeApogee(int apogee);

	// -----------------------------------------------------------------------------------
	// This method sends data via the XBee.
	// It sends a data sequence number. Plus the given time data, velocity data, 
	// displacement data, and acceleration.
	// -----------------------------------------------------------------------------------
	void SendData(int Dis, float vel, float acc, long timeData);

	// -----------------------------------------------------------------------------------
	// This method sends data via the XBee.
	// It sends a data sequence number. Plus the given time data, velocity data, 
	// displacement data.
	// -----------------------------------------------------------------------------------
	void SendData(int combinedDis, float combinedVel,  long timeData);

	// -----------------------------------------------------------------------------------
	// This method sends data via the XBee.
	// It sends a data sequence number. Plus the given time data, and displacement data.
	// it sends "None" for the velocity data.
	// -----------------------------------------------------------------------------------
	void SendData(int combinedDis, long timeData);
	
	// -----------------------------------------------------------------------------------
	// This method sends data via the XBee.
	// It sends a data sequence number plus the input string value.
	// -----------------------------------------------------------------------------------
	void SendData(String msg);
	
private:
	const static long TIME_DELAY = 10000; // time to wait for response in milliseconds
	const static int MAX_SEQ_NUM = 1000; // capped to decrease message length
	int seqNum = 0; // current sequence number
};

#endif
