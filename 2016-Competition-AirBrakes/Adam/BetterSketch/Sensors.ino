// -----------------------------------------------------------------------------------
// Any functions that directly interact with sensors.
//
// Functions
// float getVelocityPito()
// int getDisPito(int deltaTime, float pitoVel)
// int getDisStrato()
// float getVelStrato(int deltaTime, int deltaDis)
// void initializeStrato()
// void initializePito()
// -----------------------------------------------------------------------------------

// needs to be implemented.......................................................................
int MAX_12_INT = 1024;//4096;
int pitoAnalogPin = A5;

float getVelocityPito()
{
  //analogReadResolution(12);
  int sensorValue = analogRead(pitoAnalogPin);
  float pinRatio = (float) sensorValue / (float) MAX_12_INT;
  havePitoData = true;
  //float ratio = 1.0f;
  //if(mpuWorking)
  //   ratio = getRatioVertical();
  //havePitoData = false;
  return pinRatio;
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
    //sendMessage("Serial2.available() " + String(Serial2.available()) + "\n");
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

long DELAY_FOR_STRATO = 120000; // 2 minutes
void initializeStrato()
{
  sendMessage("Waiting for Strattologger to begin transmitting\nContinues without data if this exceeds 2 minutes\n");
  long endTime = millis() + DELAY_FOR_STRATO;
  long failSafeCount = 9000000;
  int count = 0;
  while(Serial2.available() <= 0 && millis() < endTime); // loop until we receive data from stratologger
  if(millis() >= endTime)
    sendMessage("Telemetry took too long to set up!\n");
  else
  {
    int temp = Serial2.parseInt();
    sendMessage("Initial Altitude above sea level is " + String(temp) + "\n");
  } 
}
// -----------------------------------------------------------------------------------
// This function initializes the BNO sensor. 
// Initialization result (failed/succeeded) are output to the sendMessage function.
// -----------------------------------------------------------------------------------
void initBno()
{
  /* Initialise the sensor */
  if(!bno.begin())
    sendMessage("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!\n");
  else
    sendMessage("BNO055 working!\n");

  delay(1000);

  bno.setExtCrystalUse(true);
}

// -----------------------------------------------------------------------------------
// This function returns the acceleration from the BNO055 acceleromter.
// -----------------------------------------------------------------------------------
void getAccel(float & aX, float & aY, float & aZ)
{
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  aX = accel.x();
  aY = accel.y();
  aZ = accel.z();
}
