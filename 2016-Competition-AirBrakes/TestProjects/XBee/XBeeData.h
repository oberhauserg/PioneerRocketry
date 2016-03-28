#ifndef _XBEEDATA_H
#define _XBEEDATA_H

class XBeeData: public XBee
{
public:
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
	void SendData(int combinedDis, float combinedVel, long timeData);

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
	const static int MAX_SEQ_NUM = 1000; // capped to decrease message length
	int seqNum = 0; // current sequence number
};

#endif