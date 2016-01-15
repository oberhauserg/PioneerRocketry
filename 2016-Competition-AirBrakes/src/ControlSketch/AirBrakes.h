/**
* This class deals with the airbreaks, and setting 
* airbreak deployment amounts.
*/
class AirBrakes
{
	
	public:

		/**
		* Set the deployment percentage here. 
		* The PID controller relies on percentage of available force applied.
		* So just put the controller output into here, and you should be good.
		*/
		void setDeploymentPercentage(int percentage);

		/**
		* Sets whether or not the braking system should be active or not. 
		* If this is set to false, the breaks will be brought in all the 
		* way and any attempt to set the deployment percentage will be
		* ignored. (The update function still has to be called to achieve this).
		*/
		void setActive(bool a){active = a; linearActuatorGoal = 0;}

		/**
		* The linear actuator (or whatever else is being used), needs
		* to be checked up on every cycle to make sure it's at its target.
		* Make sure to call this function to make sure everything is in 
		* tip top shape with the actuators (or whatever). 
		*/
		void update();

	private:

		/**
		* The goal we are trying to hit with the actuators.
		*/
		int linearActuatorGoal = 0;

		bool active = false;


};
