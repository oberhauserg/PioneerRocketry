#include "Controller.h"
#include "Constants.h"
#include "SensorHub.h"
#include <math.h> 
//For the millis() function
#include <Arduino.h>

void Controller::update()
{
	//Get the velocity as measured by the pitot tube
	int vertVelocity =  SensorHub::getPitotTubeVelocity();

	//Get the z acceleration from the SensorHub
	int acceleration = SensorHub::getAccelMeasurements()[2];

	//Is the PID controller supposed to be controlling now?
	if(active)
	{

		//The ridiculous final altitude calculations.
		//We might want to break this into its own function
		//This is basically what's going on here:
		//(m / (2 * k)) * Log((m * -g + k * v^2)/(m * -g)) + y
	    // Where:
	    // m = mass in grams
	    // k = K constant - This is calculated beforehand and is a constant in the program. 
	    // g = acceleration due to gravity in meters (-9.81). 
	    // v = vertical velocity
	    // y = current altitude 
		float finalAltitude = 
		(Constants::BURNOUT_MASS_IN_GRAMS /(2 * Constants::NOMINAL_K_VALUE)) 
		* log((Constants::BURNOUT_MASS_IN_GRAMS * -Constants::GRAVITY_METERS 
		+ Constants::NOMINAL_K_VALUE * vertVelocity * vertVelocity) 
		/ (Constants::BURNOUT_MASS_IN_GRAMS * -Constants::GRAVITY_METERS))
		+ SensorHub::getAltitude();

		double elapsedSecs = getElapsedSeconds();

		float error = finalAltitude - targetAltitude;

		integral = integral + error * elapsedSecs;

		derivative = (error - prevError)/elapsedSecs;

		int output = paramP * error + paramI * integral + paramD * derivative;

		brakes->setDeploymentPercentage(output);

	}

        cleanupController();


}

double Controller::getElapsedSeconds()
{

  return (millis() - lastControlTime) * Constants::MILLIS_TO_BASE;
  
}


void Controller::cleanupController()
{
  
  lastControlTime = millis();
  
}
