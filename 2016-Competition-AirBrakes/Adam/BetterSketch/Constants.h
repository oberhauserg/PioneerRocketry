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
  static constexpr float GRAVITY_METERS = -9.81;
  static constexpr float GRAVITY_IMPERIAL_SCUM = -32.2;

  static constexpr float MILLIS_TO_BASE = 1000;

  /**
  * Constants that deal with airbreaking. 
  */
  static constexpr int MAXIMUM_BREAK_PERCENTAGE = 100;

  

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

  /**
  * Constants for calculating sensor measurements from raw data
  */
  static constexpr float PITOT_GAIN = 1;

  static constexpr float PITOT_ZERO = 0;

  static constexpr int MAX_12_INT = 1024;

  static constexpr int DELAY_FOR_STRATO = 120000;

};
