#include "XBeeData.h"

// -----------------------------------------------------------------------------------
// This method allows the user to modify the apogee via the XBee.
// the user has the option to modify this value via the XBee. It has a while loop that
// will loop through a maximum of 10 times. Each time, the user can modify the 
// apogee. Then they verify that it was correctly input. It returns the apogee 
// corrected by the user. If no apogee is entered, then it returns the initial value.
// -----------------------------------------------------------------------------------
int XBeeData::InitializeApogee(int apogee)
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
// This method sends data via the XBee.
// It sends a data sequence number. Plus the given time data, and displacement data.
// it sends "None" for the velocity data.
// -----------------------------------------------------------------------------------
void XBeeData::SendData(int combinedDis, long timeData)
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
void XBeeData::SendData(int combinedDis, float combinedVel, long timeData)
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

void XBeeData::SendData(String msg)
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
void XBeeData::SendData(int Dis, float vel, float acc, long timeData)
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