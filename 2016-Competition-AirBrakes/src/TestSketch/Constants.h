//-------------------------------------------------
//
// This class is used to store constants dealing
// with everything for the flight system
//
// Author(s): Grant Oberhauser
//-------------------------------------------------

class Constants
{
  public:
  
  /**
  * Constants that deal with real-world physical constants. 
  */
  static const float GRAVITY_METERS = -9.81;
  static const float GRAVITY_IMPERIAL_SCUM = -32.2;

  static const float MILLIS_TO_BASE = 1000;

  /**
  * Constants that deal with airbreaking. 
  */
  static const int MAXIMUM_BREAK_PERCENTAGE = 100;

  

  /**
  * This isn't actually a constant because we might want to set it
  * through serial or something while the controller is running
  */
  static int BURNOUT_MASS_IN_GRAMS;

  /**
  * This is also not a constant for the same reason. 
  * This is all of the constants in the drag calculation
  * lumped into one number and placed right here.
  */
  static float NOMINAL_K_VALUE;

};
