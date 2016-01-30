#include "AirBrakes.h"
#include <Servo.h>

void AirBrakes::setDeploymentPercentage(int percentage)
{
	//lets just pretend.
	
	//Some thoughts: 
	//This is all going to be math, and setting the linearActuatorGoal to 
	//some value in the end. It will all most likely deal with the 
	//total surface area available at full flaps deployed, compared 
	//to the surface are available at every angle of the flaps. 
	//
	//          x
	//       ------
	//       \    |
	//        \   |
	//       h \  | y
	//          \ |
	//           \|
	//
	//       Imagine the --- line is the flaps fully deployed. 
	//       At fully deployed, the angle between the rocket and the flap
	//       is 90 degrees. The x above that line is the total length of 
	//       the flap at that deployment amount. That is what we are 
	//       measuring when we say "deployment percentage". What 
	//       percentage of x's full length are we getting? We can relate 
	//       the percentage to the angle between the x and y. However, we
	//       should probably take measurments of Jake's spinny 
	//       contraption before diving in too far. 
	//

  //Ignore above

  linearActuatorGoal = MAX_ACTUATE - MIN_ACTUATE + ((1 - (percentage * 0.01)) * (MAX_ACTUATE - MIN_ACTUATE));

  
}

void AirBrakes::setLinearActuatorAmount(int amount)
{
 
  linearActuatorGoal = amount;
  
}

int AirBrakes::update()
{
	//This will be fun.
  
  int tempGoal = linearActuatorGoal;
  
  if(tempGoal > MAX_ACTUATE)
    tempGoal = MAX_ACTUATE;
  else if(tempGoal < MIN_ACTUATE)
    tempGoal = MIN_ACTUATE;

  
  
  if(tempGoal != currentSet && isActive)
  {
    linearActuator->writeMicroseconds(tempGoal);
    currentSet = tempGoal; 
  }
  else(!isActive)
  {
  	linearActuator->writeMicroseconds(MAX_ACTUATE);
    currentSet = MAX_ACTUATE; 
  }

  
  newPos = false;

  return tempGoal;

}
