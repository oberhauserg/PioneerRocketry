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
	static int * getGyroMeasurements(){return gyroArray;}

	static int * getAccelMeasurements(){return accelArray;}

	int static getAltitude(){return 0;}

	int static getPitotTubeVelocity(){return 0;}

	/**
	* Makes the sensor hub update all of the sensor readings again
	*/
	void static update();

private: 

	/**
	* This is the array that contains the gyro data. 
	* This can be modified when the reference is passed to other 
	* functions. All I can say to that is, don't be a dick. 
	*
	* Mapped as: x, y, z
	*/
	static int gyroArray[3];

	/**
	* This is the array that contains acceleration data
	* Mapped as: x, y, z
	*/
	static int accelArray[3];



};
