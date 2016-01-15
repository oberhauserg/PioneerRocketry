#include "AirBrakes.h"

#ifndef CONROLLER_H
#define CONROLLER_H


/**
* This class deals with the controller and controller algorithms.
*/
class Controller
{
	
	public:

		/**
		* The constructor takes an AirBreaks object. 
		* This way, other parts of the system can 
		* deactivate the breaks if necessary. 
		*/
		Controller(const AirBrakes &b);

		/**
		* Sets whether the controller should be active. 
		* Competiton guidelines say that you should not start deploying
		* brakes before burnout. So this should probably be set to false
		* right away. 
		*/
		void setActive(bool a);

		/**
		* Tells the controller to update its self. 
		* When this function is called, it
		* performs all the necessary calculations 
		* and sets the airbreaks to the correct deployment
		* percentage.
		*/
		void update();

		/**
		* Set the P parameter for the controller
		*
		*/
		void setPParameter(float param);

		/**
		* Set the I parameter for the controller
		*/
		void setIParameter(float param);

		/**
		* Set the D parameter for the conroller.
		*/
		void setDParameter(float param);

		/**
		* Set the target altitude
		*/
		void setTargetAltitude(int target);


	private:

		/**
		* The breaks being used for control
		*/
		AirBrakes * brakes;

		/**
		* Is the controller currently active?
		*/
		bool active;

		/**
		* The error in the last cycle
		*/
		double error; 

		/**
		* The varaible used for getting the previous error.
		*/
		double prevError;

		/**
		* The integral of errors so far.
		*/
		double integral = 0;

		/**
		* The derivative of the errors in the last cycle
		*/
		double derivative = 0;  

		/**
		* The P parameter for the controller
		*/
		float paramP;

		/**
		* The I parameter for the controller
		*/
		float paramI;

		/**
		* The D parameter for the controller
		*/
		float paramD;

		/**
		* The altitude we want our apogee at
		*/
		int targetAltitude;

		/** 
		* The last time that the controller was ran
		*/
		unsigned long lastControlTime;


		/**
		* The default constructor is private 
		* to force breaks to be passed
		*/
		Controller();

		/**
		* The controller uses seconds to figure out the change
		* that states went through. However, arduino has libraries
		* for microseconds and milliseconds. So we use those here
		* in order to convert to seconds. 
		*/
		double getElapsedSeconds();

		/**
		* Makes sure all of the end values are set and the functions
		* the controller uses are all setup to run again. 
		*/
		void cleanupController();

};

#endif
