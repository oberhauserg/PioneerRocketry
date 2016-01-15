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
		Controller(AirBreaks &b){breaks = b};

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
		* Set the P parameter for the 
		*
		*/
		void setPParameter(float param);

		void setIParameter(float param);

		void setDParameter(float param);



	private:

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
		* The default constructor is private 
		* to force breaks to be passed
		*/
		Controller();

		/**
		* The breaks being used for control
		*/
		AirBreaks * breaks;

}