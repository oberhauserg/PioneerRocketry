#include "SensorTest.h"
#include <string>


void SensorTest::InitSD()
{
	file = SD.open("debug.csv");
}

void SensorTest::Update()
{
	
	char * str = readLine();
	char * p = str;
	char * tok;
	
	dis = stof(strtok(p,","));
	vel = stof(strtok(NULL, ",");
	ax = stof(strtok(NULL, ",");

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