#include "SensorTest.h"
#include <string>


void SensorTest::InitSD()
{
	file = SD.open("debug.csv");
}

void SensorTest::Update()
{
	
	char * str = ReadLine();
	char * p = str;
	char * tok;
	
	dis = String(strtok(p,",")).toFloat();
	vel = String(strtok(NULL, ",")).toFloat();
	ax = String(strtok(NULL, ",")).toFloat();
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
	char readChar;
	int i = 0;



	for(; i < 1000 && ((readChar = file.read()) != '\n') ; i++)
	{
		newString[i] = readChar;
	}

	newString[i] = '\0';

	return newString;

}

/*void main()
{
	String str;



}*/
