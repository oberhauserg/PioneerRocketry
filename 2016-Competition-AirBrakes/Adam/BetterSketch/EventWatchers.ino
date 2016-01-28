// -----------------------------------------------------------------------------------
// Any functions that check for events triggering state changes.
// 
// Functions
// void checkForLiftoff(float vel1, float vel2)
// bool checkForBurnout(float vel, int deltaT)
// void checkForApogee(float vel, int dis)
// void checkForLanding(int dis)
// -----------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------
// This function watches for liftoff and perfoms necessary state change procedures.
// It averages the last LIFTOFF_NUM_AVE velocities. If the average velocity is less 
// greater than 20.0 fps, it triggers the liftoff event state change. You input at 
// most two velocity measurement. The larger measurement is recorded. Thus, if only 
// one form of velocity is measured, the remaining velocity can be set to 0.
// -----------------------------------------------------------------------------------
int LIFTOFF_THRESHHOLD = 20;
int LIFTOFF_NUM_AVE = 3;
float velocity[] = {0.0f,0.0f,0.0f};
int liftoffPnt = 0;

void checkForLiftoff(float vel1, float vel2)
{ 
  if(vel1 > vel2)
    velocity[liftoffPnt++] = vel1;
  else
    velocity[liftoffPnt++] = vel2;
  if(liftoffPnt == LIFTOFF_NUM_AVE)
    liftoffPnt = 0;
  float sum;
  for(int i = 0; i < LIFTOFF_NUM_AVE; i++)
  {
    sum += velocity[i];
  }
  float average = ((int)sum)/LIFTOFF_NUM_AVE; // convert to int for faster math
  if(average >= LIFTOFF_THRESHHOLD)
  {
    sendMessage("Lift Off\n");
    preLaunch = false;
    engineBurning = true;
    
    // timed airbreak deployment used for first flight only
    timeToTriggerAirbreak = millis() + timeToBurnOut;
    
    
  }
}

// -----------------------------------------------------------------------------------
// This function checks for burnout and triggers appropriate state change behavior.
// It records the last 4 velocity measurements and deltaTimes. From this, it 
// calculates the last 3 accelerations. If the average acceleration for these three
// values is negative, it triggers the burnout condition.
// -----------------------------------------------------------------------------------
int NUM_BURNOUT_SAMPLES = 4;
float burnoutVel[] = {0.0f,0.0f,0.0f,0.0f};
float burnoutTime[] = {0.00001f,0.00001f,0.00001f}; // prevents division by zero
int burnoutPnt = 0;

bool checkForBurnout(float vel, int deltaT)
{ 
  
  burnoutVel[burnoutPnt] = vel;
  burnoutTime[burnoutPnt++] = ((float)deltaT)/numMillisecondsInSecond;
  
  float sumAccelerations;
  if(burnoutPnt == NUM_BURNOUT_SAMPLES)
    burnoutPnt = 0;
  // acceleration = (inital velocity - current velocity ) / time between them
  for(int i = 1; i < NUM_BURNOUT_SAMPLES; i++)
  {
    if(burnoutTime[i] != 0.0f) // prevents division by zero
      sumAccelerations += (burnoutVel[i - 1] - burnoutVel[i]) / burnoutTime[i];
  }
  float aveAccelerations = sumAccelerations / (NUM_BURNOUT_SAMPLES - 1); 
  if(aveAccelerations <= 0 )
  {
    sendMessage("Burnout.\n");
    engineBurning = false;
    midLaunch = true;
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------------
// This function tests for apogee and triggers appropriate state changes.
// It records the past 3 Stratologger velocities and combined displacements. 
// The stratologger uses displacment to calculate velocity, so it will read negative
// velocity once the rocket passes apogee. 
// If the past 3 stratologger velocities are negative this event triggers the state
// change to descending.
// Also, the displacements are recorded, so that once apogee is reached, the largest
// value for the apogee is recorded as apogee.
// -----------------------------------------------------------------------------------
int NUM_APOGEE_AVE = 3;
float apogeeVel[] = {0.0f,0.0f,0.0f};
int apogeeDis[] = {0,0,0};
int apogeePnt = 0;

void checkForApogee(float vel, int dis)
{
  apogeeVel[apogeePnt] = vel;
  apogeeDis[apogeePnt++] = dis;
  if(apogeePnt == NUM_APOGEE_AVE)
    apogeePnt = 0;
  float sum;
  for(int i = 0; i < NUM_APOGEE_AVE; i++)
  {
    sum += apogeeVel[i];
  }
  int ave = sum / NUM_APOGEE_AVE;
  if(ave < 0)
  {
    midLaunch = false;
    descending = true;
    havePitoData = false;
    int maxApogee = 0;
    // find largest apogee in array
    for(int i = 0; i < NUM_APOGEE_AVE; i++)
    {
      if(apogeeDis[i] > maxApogee)
        maxApogee = apogeeDis[i];
    }
    apogee = maxApogee;
    String apogeeString = String(maxApogee);
    sendMessage("Apogee is : " + apogeeString + "\n");
    writeApogeeToSD(apogee); 
  }
}

// -----------------------------------------------------------------------------------
// This function watches for the rocket landing and triggers the landing event.
// It uses 15 values for the average so we have much less of a chance of getting a 
// false positive. Also, the landing is is not highly time sensative, so we are not
// concerned with using a large sample.
// It calculates the standard deviation. If it is less then 4, then it triggers the
// rocket landed event. 
// -----------------------------------------------------------------------------------
int NUM_LAND_AVE = 15;
int landingDisplacements[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int landingPnt = 0;

void checkForLanding(int dis)
{
  landingDisplacements[landingPnt++] = dis;
  if(landingPnt == NUM_LAND_AVE)
    landingPnt = 0;
  int sum = 0;
  for(int i = 0; i < NUM_LAND_AVE; i++)
  {
    sum += landingDisplacements[i];
  }
  float ave = (float)sum / (float)NUM_LAND_AVE;

  // calculate standard deviation
  float deviation[NUM_LAND_AVE];
  for(int i = 0; i < NUM_LAND_AVE; i++)
  {
    deviation[i] = (float)landingDisplacements[i] - ave;
  }
  // square deviation
  for(int i = 0; i < NUM_LAND_AVE; i++)
  {
    deviation[i] *= deviation[i];
  }
  float sumDeviation;
  for(int i = 0; i < NUM_LAND_AVE; i++)
  {
    sumDeviation += deviation[i];
  }
  float aveDeviation = sumDeviation / (float)NUM_LAND_AVE;
  float stdDev = sqrt(aveDeviation);
  
  if(stdDev <= 3)
  {
    sendMessage("Has Landed.\n");
    descending = false;
    landed = true;
    data.close(); // needs to be implemented without using global variable .......................................
  }
}

