#include "SensorTest.h"
#include <string>

#include "XBee.h"


void SensorTest::InitSD()
{
  SD.begin(48);
  XBee xb;
  xb.InitializeXBee();
  if(SD.exists("debug.csv"))
    xb.SendMessage("Test file opened\n");
  else
    xb.SendMessage("Test file did NOT open\n");
	file = SD.open("debug.csv"); 
}

void SensorTest::Update()
{
	//XBee xb;
  //xb.InitializeXBee();
	char * str = ReadLine();
	char * p = str;
	char * tok;
  //String temp = String(strtok_r(p, ",", &i));
	dis = String(strtok(p,",")).toFloat();
  //Serial.print(" dis: " + String(dis));
  //xb.SendMessage("dis is " + String(dis) + "\n");
	vel = String(strtok(NULL, ",")).toFloat();
  //Serial.print(" vel: " + String(vel));
  //xb.SendMessage("vel is " + String(vel) + "\n");
	ax = String(strtok(NULL, ",")).toFloat();
 //Serial.println(" ax: " + String(ax));
  //xb.SendMessage("ax is " + String(ax) + "\n");
  //delay(2000);
}


float SensorTest::GetDis()
{
	return dis;
}


float SensorTest::GetVel()
{
	return vel;
}


float SensorTest::GetAX()
{
	return ax;
}

char* SensorTest::ReadLine()
{
	char newString[1000];
	char readChar = file.read();
	int i = 0; 

	for(; i < 1000 && (readChar  != '\n') ; i++)
	{
		newString[i] = readChar;
    readChar = file.read();
	}

	newString[i] = '\0';
	return newString;

}

//--------------------------------------------------------------------
  // Get velocity of pito. 
  // This take the velocity read in. If it is negative, it inverts it.
  //--------------------------------------------------------------------
  float SensorTest::GetPitoVel()
  {
    if(vel < 0)
      return - vel;
    return vel;
  }

/*void main()
{
	String str;



}*/
