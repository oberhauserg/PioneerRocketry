//-------------------------------------------
//
// This sketch is used to test out concepts 
// and see if code complies.
//
//-------------------------------------------

#include "AirBrakes.h"
#include "Constants.h"
#include "Controller.h"
#include "SensorHub.h"

AirBrakes * brakes;

Controller * cont;

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
  
}
