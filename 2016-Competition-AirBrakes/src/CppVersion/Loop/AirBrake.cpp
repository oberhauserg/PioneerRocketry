#include "AirBrake.h"

bool AirBrake::Initialize()
{
  //xb.InitializeXBee();
	myServo.attach(PIN_NUM);
	myServo.writeMicroseconds(BRAKE_CLOSED);
	return myServo.attached();
}

void AirBrake::AirBrakePercent(int num)
{
  //xb.SendMessage("per " + String(num) + "\n");
	if (num >= 0 && num <= BASE_OF_PERCENT)
	{
		int range = BRAKE_OPEN - BRAKE_CLOSED;
		int  openAmount = num * range  / BASE_OF_PERCENT;
		myServo.writeMicroseconds(BRAKE_CLOSED + openAmount);
	}
}

void AirBrake::Update(int dis, float vel, float deltaT, int goalApogee)
{
  prevEN = eN;
  //xb.SendMessage("pred " + String(PredictedRemaining(vel)) + "\n");
  eN = dis + PredictedRemaining(vel) - goalApogee;
  float x = eN * ( kP + kD / deltaT ) - prevEN * kD / deltaT;
  x = constrain(x,0,1);
  x *= 100;
  percent = (int)x;
  AirBrakePercent(percent);
}

int AirBrake::PredictedRemaining(float target)
{
  int lowerBound, upperBound;
  if (target < velAry[0])
    return 0;
  if (target > velAry[NUM_SAMPLES_IN_LOOKUP_TABLE - 1])
    return velAry[NUM_SAMPLES_IN_LOOKUP_TABLE - 1];
  FindIndeces(target, lowerBound, upperBound);
  int rise = (heightArray[upperBound] - heightArray[lowerBound]);
  float range = (velAry[upperBound] - velAry[lowerBound]);
  float subRange = (target - velAry[lowerBound]);
  int y =  rise /  range * subRange + heightArray[lowerBound];
  return y;
}

void AirBrake::FindIndeces(float target, int& lowerBound, int& upperBound)
{
  for (int i = 0; i < NUM_SAMPLES_IN_LOOKUP_TABLE; i++)
  {
    if (velAry[i] >= target)
    {
      if (i == 0)
      {
        lowerBound = i;
        upperBound = -1;
        return;
      }
      lowerBound = i - 1;
      upperBound = i;
      return;
    }
  }
}


