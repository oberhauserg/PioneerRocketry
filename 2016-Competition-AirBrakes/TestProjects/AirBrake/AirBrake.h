#ifndef _AIRBRAKE_H
#define _AIRBRAKE_H

#include <Servo.h>
// --------------------------------------------------------------------------------------
// This class contains all the methods related to the operation of the servo.
// --------------------------------------------------------------------------------------
class AirBrake
{
public:
	// -----------------------------------------------------------------------------------
	// This method is called to set the pin number of the servo. 
	// returns .isConnected() method of servo.
	// -----------------------------------------------------------------------------------
	bool Initialize();
	// -----------------------------------------------------------------------------------
	// This method will control the airBrakes with the input values below.
	// -----------------------------------------------------------------------------------
	void Update(int dis, float vel, float acc) {};
	// -----------------------------------------------------------------------------------
	// This method is called to update the vel and dis values of the class.
	// -----------------------------------------------------------------------------------
	void SetActive(bool _isActive) { isActive = _isActive; }
	// -----------------------------------------------------------------------------------
	// This method is called to open the Air Brakes to the BRAKE_OPEN amount. 
	// -----------------------------------------------------------------------------------
	void OpenBrakes() { myServo.writeMicroseconds(BRAKE_OPEN); }
	// -----------------------------------------------------------------------------------
	// This method is called to close the Air Brakes to the BRAKE_CLOSED amount. 
	// -----------------------------------------------------------------------------------
	void CloseBrakes() { myServo.writeMicroseconds(BRAKE_CLOSED); }
	// -----------------------------------------------------------------------------------
	// This method is called to open the Air Brakes a set amount. 
	// The num is an integer for the precentage the airbrakes need  to be opened. 
	// -----------------------------------------------------------------------------------
	void AirBrakePercent(int num); 
private:
	const static int BRAKE_OPEN = 2000; // for use with writeMicroseconds
	const static int BRAKE_CLOSED = 1000; // for use with writeMicroseconds
	const static int PIN_NUM = 13; // pin num for arduino
	const static int BASE_OF_PERCENT = 100;

	Servo myServo;
	bool isActive;
};

#endif
