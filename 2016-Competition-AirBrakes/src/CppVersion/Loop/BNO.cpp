#include "BNO.h"

bool BNO::Initialize()
{
	bool bnoWorking;
	bnoWorking = bno.begin();
	delay(1000);
	bno.setExtCrystalUse(true);
	return bnoWorking;
}

void BNO::Update()
{
	//VECTOR_LINEARACCEL == acceleration minus gravity
	imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

	ax = MPS_TO_FTPS * accel.x();
	ay = MPS_TO_FTPS * accel.y();
	az = MPS_TO_FTPS * accel.z();
}
