/**
* This class organizes all of the sensors we have 
* on the rocket into one neat place. 
* Author: Grant Oberhauser
*/
class SensorHub
{
	
public:

	/**
	* Return a value to a pointer 
	*/
	int* getGyroMeasurements();

	int* getAccelMeasurements();

	int getAltitude();

	int getPitotTubeVelocity();

private: 

	/**
	* This is the array that contains the gyro data. 
	* This can be modified when the reference is passed to other 
	* functions. All I can say to that is, don't be a dick. 
	*
	* Mapped as: x, y, z
	*/
	int * gyroArray = {0,0,0};

	/**
	* This is the array that contains acceleration data
	* Mapped as: x, y, z
	*/
	int * accelArray = {0,0,0};



};
