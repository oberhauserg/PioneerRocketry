/*
 Name:		AirBrakesTest.ino
 Created:	3/5/2016 3:37:47 PM
 Author:	Grant Oberhauser
*/

#include <Servo.h>
#include "AirBrakes.h"

AirBrakes * b;

//Change this pin to whatever pin the airbrake is actually using
int airbrakePin = 13;
int test = 0;

void setup() {

   b = new AirBrakes(airbrakePin);

}

void loop() {

   //The tests take one minute to complete a cycle
   long looptime = millis() % 60000; 

   if (looptime < 5000)
      b->setDeploymentPercentage(25);
   else if (looptime < 10000)
      b->setDeploymentPercentage(50);
   else if (looptime < 15000)
      b->setDeploymentPercentage(75);
   else if (looptime < 20000)
      b->setDeploymentPercentage(100);

   else if (looptime < 30000)
      b->setActive(false);
   else if (looptime < 40000)
      b->setActive(true);

   //At 40 seconds, set the airbrakes to deactivated. 
   //Turn of the arduino if necessary at this point.
   else
      b->setActive(false);


   b->update();

}
