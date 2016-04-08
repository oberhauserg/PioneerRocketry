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
#endif // using XBee
	Serial1.print(msg);
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
#endif
	Serial1.begin(9600);
}


// -----------------------------------------------------------------------------------
// This method allows the user to modify the apogee via the XBee.
// the user has the option to modify this value via the XBee. It has a while loop that
// will loop through a maximum of 10 times. Each time, the user can modify the 
// apogee. Then they verify that it was correctly input. It returns the apogee 
// corrected by the user. If no apogee is entered, then it returns the initial value.
// -----------------------------------------------------------------------------------
int XBee::InitializeApogee(int apogee)
{
	// get apogee from XBee
	SendMessage("Current apogee is set at : " + String(apogee) +
				"\nDo you want to change this. \"Y\" -> change. "
				+ "\"N\" -> leave as is\n");
	String response = ReceiveMessage();
	response.toUpperCase();
	if (response.equals("Y") || response.equals("Y\n") || response.equals("Y\r")) 
	{
		bool badMessage = true;
		int numTimesInLoop = 0;
		int maxTimesInLoop = 10; // makes sure you don't have infinite loop without XBee
		while (badMessage && (numTimesInLoop++ != maxTimesInLoop))
		{
			SendMessage("\nPlease input your altitude as an integer. (No Periods, Spaces, Commas, or \\n).\n");
			response = ReceiveMessage();
			SendMessage("\nCurrent apogee is set at : " + response +      
				"\nIs this correct.  \"Y\" -> correct. \"N\" -> try again\n");
			String response2 = ReceiveMessage();
			response2.toUpperCase();
			if (response2.equals("Y") || response2.equals("Y\n") || response2.equals("Y\r"))
			{
				badMessage = false;
				apogee = response.toInt();
			}
			else
				badMessage = true;
		}
		if (numTimesInLoop == maxTimesInLoop)
		{
			SendMessage("\nExceeded max number of attemps!\n");
		}
	}
	SendMessage("Apogee set at :" + String(apogee) + "\n");
	return apogee;
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

// -----------------------------------------------------------------------------------
// This method sends data via the XBee.
// It sends a data sequence number. Plus the given time data, and displacement data.
// it sends "None" for the velocity data.
// -----------------------------------------------------------------------------------
void XBee::SendData(int combinedDis, long timeData)
{
	if (seqNum == MAX_SEQ_NUM)
		seqNum = 0;
	seqNum++;
	String num = String(seqNum);
	String msgTime = String(timeData);
	String vel = "None";
	String dis = String(combinedDis);
	String temp = num + ',' + msgTime + ',' + dis + "," + vel + '\n';
	SendMessage(temp);
}

// -----------------------------------------------------------------------------------
// This method sends data via the XBee.
// It sends a data sequence number. Plus the given time data, velocity data, 
// displacement data.
// -----------------------------------------------------------------------------------
void XBee::SendData(int combinedDis, float combinedVel,  long timeData)
{
	if (seqNum++ == MAX_SEQ_NUM)
		seqNum = 0;
	String num = String(seqNum);
	String msgTime = String(timeData);
	String vel = String(combinedVel, 3);
	String dis = String(combinedDis);
	String temp = num + ',' + msgTime + ',' + dis + "," + vel + "\n";
	SendMessage(temp);
}

void XBee::SendData(String msg)
{
	if (seqNum++ == MAX_SEQ_NUM)
		seqNum = 0;

	String num = String(seqNum);
	String msgTime = String(millis());
	String temp = num + "," + msgTime + "," + msg + "\n";
	SendMessage(temp);
}

// -----------------------------------------------------------------------------------
// This method sends data via the XBee.
// It sends a data sequence number. Plus the given time data, velocity data, 
// displacement data, and acceleration.
// -----------------------------------------------------------------------------------
void XBee::SendData(int dis, float vel, float acc, long timeData)
{
	if (seqNum++ == MAX_SEQ_NUM)
		seqNum = 0;
	String num = String(seqNum);
	String msgTime = String(timeData);
	String velocity = String(vel, 3);
	String displacment = String(dis);
	String acceleration = String(acc, 3);
	String temp = num + ',' + msgTime + ',' + displacment + "," 
				  + velocity + acceleration + "\n";
	SendMessage(temp);
}

