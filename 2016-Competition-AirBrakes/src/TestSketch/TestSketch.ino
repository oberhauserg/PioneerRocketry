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

int max_count = 1000;

void setup() {

  //Initialize AirBrakes module
  brakes = new AirBrakes();

  //Initialize controller module
  cont = new Controller(*brakes);
  
}

void loop() { 

  //Update the controller
  cont->update();

  //Make sure the brakes are being updated
  brakes->update();
  
  if(on)
    digitalWrite(13, HIGH); 
  else
    digitalWrite(13, LOW);
   
  if(count >= max_count)
  {
    on = !on;
    count = 0;
  }
  count++;
}
