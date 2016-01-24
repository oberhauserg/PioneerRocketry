// -----------------------------------------------------------------------------------
// Any functions that directly interact with sensors.
// -----------------------------------------------------------------------------------

// needs to be implemented.......................................................................
float getVelocityPito()
{
  float ratio = 1.0f;
  if(mpuWorking)
    ratio = getRatioVertical();
    
  havePitoData = false;
  return 0.0f;
}

float localPitoDisplacement = 0.0f;

int getDisPito(int deltaTime, float pitoVel)
{
  localPitoDisplacement = localPitoDisplacement + deltaTime * pitoVel / numMillisecondsInSecond;
  return (int)localPitoDisplacement;
}

// -----------------------------------------------------------------------------------
// This function reads altitude data from the Strattologger. 
// If no data is available, then 0 is returned and haveStratoData is set to false;
// -----------------------------------------------------------------------------------
int getDisStrato()
{
  if(Serial2.available() > 0)
  {
    int temp = Serial2.parseInt();
    haveStratoData = true;
    return temp;
  }
  else
  {
    haveStratoData = false;
    return 0;
  }
}

// -----------------------------------------------------------------------------------
// This function calculates velocity from change in time and change in displacement.
// The deltaTime is given in milliseconds, so it will need to be converted to seconds
// before feet per second can be calculated. 
// -----------------------------------------------------------------------------------
float getVelStrato(int deltaTime, int deltaDis)
{
  return ((float)deltaDis)* numMillisecondsInSecond / ((float)deltaTime);
}

// -----------------------------------------------------------------------------------
// This function takes the values for velocity and displacement and combineds them.
// If both Stratologger and Pitot tube data are available, then the calculated values 
// have a much smaller impact on the combined values then the directly measured 
// values. If only one data source is available, then combined values are just the 
// one measured value and the one calculated value. Once the combined values are
// calculated, they are put through a Kalman filter to smooth the data.
// -----------------------------------------------------------------------------------
float RATIO_PITO_VEL = 0.7f;
float RATIO_PITO_DIS = 0.3f;
float RATIO_STRATO_VEL = 0.3f;
float RATIO_STRATO_DIS = 0.7f;

void combineValues(float *combinedVel, int *combinedDis, float pitoVel, int pitoDis, float stratoVel, int stratoDis, float deltaTime)
{
    if(haveStratoData && havePitoData)
    {
      *combinedVel = RATIO_PITO_VEL * pitoVel + RATIO_STRATO_VEL * stratoVel;
      *combinedDis = RATIO_PITO_DIS * pitoDis + RATIO_STRATO_DIS * stratoDis;
      //kalmanFilter(&combinedVel, &combinedDis, deltaTime);
      // we will not be using kalman filter during the first test flight
    }
    else if(haveStratoData)
    {
      *combinedVel = stratoVel;
      *combinedDis = stratoDis;
      //kalmanFilter(&combinedVel, &combinedDis, deltaTime);
      // we will not be using kalman filter during the first test flight
    }
    else
    {
      *combinedVel = pitoVel;
      *combinedDis = pitoDis;
      //kalmanFilter(&combinedVel, &combinedDis, deltaTime);
      // we will not be using kalman filter during the first test flight
    }
}


long DELAY_FOR_STRATO = 12000; // 2 minutes
void initializeStrato()
{
  sendMessage("Waiting for Strattologger to begin transmitting\n");
  long endTime = millis() + DELAY_FOR_STRATO;
  long failSafeCount = 9000000;
  int count = 0;
  while(Serial2.available() <= 0 && millis() > endTime); // loop until we receive data from stratologger
  if(millis() >= endTime)
    sendMessage("Telemetry took too long to set up!\n");
  else
  {
    int temp = Serial2.parseInt();
    sendMessage("Initial Altitude above sea level is " + String(temp) + "\n");
  } 
}
