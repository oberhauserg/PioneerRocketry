#ifndef _AIRBRAKE_H
#define _AIRBRAKE_H

#include <Servo.h>
#include "XBee.h"
//#include <Arduino.h>
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
	void Update(int dis, float vel, float deltaT, int goalApogee);
	// -----------------------------------------------------------------------------------
	// This method is called to update the vel and dis values of the class.
	// -----------------------------------------------------------------------------------
	void SetActive(bool _isActive) { isActive = _isActive; }
	// -----------------------------------------------------------------------------------
	// This method is called to open the Air Brakes to the BRAKE_OPEN amount. 
	// -----------------------------------------------------------------------------------
	void OpenBrakes() { myServo.writeMicroseconds(BRAKE_OPEN); percent = 100; }
	// -----------------------------------------------------------------------------------
	// This method is called to close the Air Brakes to the BRAKE_CLOSED amount. 
	// -----------------------------------------------------------------------------------
	void CloseBrakes() { myServo.writeMicroseconds(BRAKE_CLOSED); percent = 0; }
	// -----------------------------------------------------------------------------------
	// This method is called to open the Air Brakes a set amount. 
	// The num is an integer for the precentage the airbrakes need  to be opened. 
	// -----------------------------------------------------------------------------------
	void AirBrakePercent(int num); 
  // -----------------------------------------------------------------------------------
  // This method returns whether the airbrakes were set active or not. 
  // -----------------------------------------------------------------------------------
  bool IsActive() { return isActive; }
  // -----------------------------------------------------------------------------------
  // This method returns the current percent the airbrakes are set at.
  // -----------------------------------------------------------------------------------
  int GetAirBrakePercent() { return percent; }
private:
  //------------------------------------------------------------------------------------
  // This method calculates the height the rocket still is going to travel.
  // This is rocket specific based off the mass of the specific rocket being used.
  //------------------------------------------------------------------------------------
  int PredictedRemaining(float vel);

  //------------------------------------------------------------------------------------
  // This method goes through the velAry and finds the two surrounding indexes of target.
  // If the velocity equals one of the values, then it returns the first value, and -1 
  // second. The user must check to ensure that the velocity is not larger then the max
  // velocity in velAry and not smaller than the min value int he velAry before calling 
  // this.
  //------------------------------------------------------------------------------------
  void FindIndeces(float target, int& lowerBound, int& upperBound);

	const static int BRAKE_OPEN = 1860; // for use with writeMicroseconds
	const static int BRAKE_CLOSED = 965; // for use with writeMicroseconds
	const static int PIN_NUM = 13; // pin num for arduino
	const static int BASE_OF_PERCENT = 100;
  const static int NUM_SAMPLES_IN_LOOKUP_TABLE = 20;
  const float kP = 0.05; // used in Update, proportional gain
  const float kD = 0.01; // used in Update, derivative gain
  float eN, prevEN = 0; // how far is off sample

  int percent = 0;

  XBee xb;

  float velAry[NUM_SAMPLES_IN_LOOKUP_TABLE] {0,43.16894737,86.33789474,129.5068421,172.6757895,215.8447368,259.0136842,302.1826316,345.3515789,388.5205263,431.6894737,474.8584211,518.0273684,561.1963158,604.3652632,647.5342105,690.7031579,733.8721053,777.0410526,820.21};

  int heightArray[NUM_SAMPLES_IN_LOOKUP_TABLE] {0,29,114,256,448,688,969,1285,1630,1999,2383,2779,3180,3583,3983,4377,4789,5231,5672,6114};
  

	Servo myServo;
	bool isActive;
};

#endif
