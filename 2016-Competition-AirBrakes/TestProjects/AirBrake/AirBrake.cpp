#include "AirBrake.h"

bool AirBrake::Initialize()
{
	myServo.attach(PIN_NUM);
	myServo.writeMicroseconds(BRAKE_CLOSED);
	return myServo.attached();
}

void AirBrake::AirBrakePercent(int num)
{
	if (num >= 0 && num <= BASE_OF_PERCENT)
	{
		int range = BRAKE_OPEN - BRAKE_CLOSED;
		int  openAmount = num * range  / BASE_OF_PERCENT;
		myServo.writeMicroseconds(BRAKE_CLOSED + openAmount);
	}
}

