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

	int static getPitotTubeVelocity(){return pitoVel;}

	int static getVelocityRedundant(){return redundantVel;};

	/**
	* Makes the sensor hub update all of the sensor readings again
	*/
	void static update();

	/**
	* Sets up anything that needs to be setup. 
	* Call this right at the start
	*/
	bool static setup();

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

	static float redundantVel;

	static float pitoVel; 

	static float lastPitoVel;

	static int stratoAlt; 

	static int lastStratoAlt;

	static int stratoZero;

	//Time since last loop;
 
	int deltaTime = 0;

	int lastLoopTime = 0;




};
