//-------------------------------------------
//
// This sketch is used to test out concepts 
// and see if code complies.
//
//-------------------------------------------

#include <Servo.h>

#include "AirBrakes.h"
#include "Constants.h"
#include "Controller.h"
#include "SensorHub.h"

AirBrakes * brakes;

Controller * cont;

boolean on = true;

int count = 0;

int next;

int max_count = 1000;

void setup() 
{

    Serial.begin(9600);
  Serial.println("1\n");
  //Initialize AirBrakes module
  brakes = new AirBrakes();

  delay(100);

  Serial.println("2\n");
  brakes->setDeploymentPercentage(100);

  delay(100);

  Serial.println("3\n");
  //Initialize controller module
  //cont = new Controller(*brakes);

  
}

void loop() { 

  //Update the controller
  //cont->update();
  

  //Make sure the brakes are being updated
  int test = brakes->update();

  Serial.print("Test: ");
  Serial.print(test);

  Serial.print(Serial.available() + "\n");
  int temp;
  if(Serial.available() > 0)
    next =  Serial.parseInt();

  brakes->setDeploymentPercentage(next);
  
  Serial.print(next);
  
}
