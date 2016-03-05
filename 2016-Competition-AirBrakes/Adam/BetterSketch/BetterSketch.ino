// -----------------------------------------------------------------
// This is a early iteration of the Pioneer Rocketry electronics
// code. Hopefully it will be used as the basis for the code used 
// for the actual launch. It includes functions for pitot tube that
// are left blank until we actually have the pitot tube. However, 
// the strattologger is fully implemented. It also sends correct 
// data to the XBee.
// Author: Jacob Napp
// 
// Functions
// void setup() 
// void loop() 
// void combineValues(float *combinedVel, int *combinedDis, float pitoVel, int pitoDis, float stratoVel, int stratoDis, float deltaTime)
// ----------------------------------------------------------------- 

#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_9Axis_MotionApps41.h"
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <SD.h>
// Note: You can only have one software serial running 
// at one time
//SoftwareSerial Strato(10, 11); //RX, TX

MPU6050 mpu;
int16_t a1, a2, a3, g1, g2, g3, m1, m2, m3;     // raw data arrays reading
float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 

long lastTimeRecorded;
bool mpuWorking = true;
bool airBreaking = false;

File data;

int apogee = 0;

// first test flight data
long timeToBurnOut = 1500; // 1.5 seconds
long timeToTriggerAirbreak = 2147483647; // max long
// end first test flight data

void setup() 
{
  Wire.begin(); // used for 9DOF 
  Serial1.begin(9600);
  Serial2.begin(9600); 
  
  delay(1000); // gives serial time to open up

  // initialize SD Card Reader/Writer
  initializeSDCard();

  // 9DOF initialization
  initializeGyro();

  // initialize air Brakes
  initializeAirBreak();

  // initialize Strattologger
  initializeStrato();
  

  checkAirBreaks();
  if(airBreaking)
    sendMessage("Air Brakes are on.\n\n");
  else
    sendMessage("AirBreaks are off.\n\n");

  // initialize apogee
  initializeApogee(); 
    
  sendMessage("SequenceNumber,Time(milliSec),Altitude(feet),Velocity(fps)\n");
  lastTimeRecorded = millis();
  
  initDataFile();
} // end setup

// -----------------------------------------------------------------------------------
// This is the primary control loop.
// -----------------------------------------------------------------------------------

float pitoVel;
int pitoDis;
float stratoVel;
int stratoDis;
int prevStratoDis = 0; // initialized to zero at launch pad

float combinedVel;
int prevCombinedVel = 0; // initialized to zero at launch pad
int combinedDis;

bool preLaunch = true;
bool engineBurning = false;
bool midLaunch = false;
bool descending = false;
bool landed = false;

bool haveStratoData = false;
bool havePitoData = false;

int numMillisecondsInSecond = 1000;

void loop() 
{
  updateAirBrakes();
  // time controlled airbrake is used in first test only
  //if(millis() >= timeToTriggerAirbreak)
  //  openAirBreaks();
  // end timed airbrake control
  haveStratoData = havePitoData = false;
  if( airBreaking &&  midLaunch)
  {
    // resetting time variables
    int deltaTime = (int)(millis() - lastTimeRecorded);
    lastTimeRecorded = millis();
     
    // get pitot tube values
    pitoVel = getVelocityPito();
    pitoDis = getDisPito(deltaTime, pitoVel);

    // get stratologger values
    stratoDis = getDisStrato();
    int deltaDis = stratoDis - prevStratoDis;
    prevStratoDis = stratoDis;
    stratoVel = getVelStrato(deltaTime, deltaDis);

    // combine values
    if(!haveStratoData && !havePitoData)
      sendData("No Data");
    else
    {
      combineValues(&combinedVel, &combinedDis, pitoVel, pitoDis, stratoVel, stratoDis, (float) deltaTime / numMillisecondsInSecond);

      sendData(combinedVel, combinedDis, lastTimeRecorded);

      controlAirBreaks();

      // check for state change
      checkForApogee(combinedVel,combinedDis);
    
      writeToSD(deltaTime, stratoDis, pitoVel);
    }
  }
  else if((!airBreaking && midLaunch) || engineBurning)
  {
    // resetting time variables
    int deltaTime = (int)(millis() - lastTimeRecorded);
    lastTimeRecorded = millis();
     
    // get pitot tube values
    pitoVel = getVelocityPito();
    if(havePitoData)
      pitoDis = getDisPito(deltaTime, pitoVel);

    // get stratologger values
    stratoDis = getDisStrato();
    int deltaDis = stratoDis - prevStratoDis;
    prevStratoDis = stratoDis;
    stratoVel = getVelStrato(deltaTime, deltaDis);

    // combine values
    if(!haveStratoData && !havePitoData)
      sendData("No Data");
    else
    {
      combineValues(&combinedVel, &combinedDis, pitoVel, pitoDis, stratoVel, stratoDis, (float)deltaTime / numMillisecondsInSecond);
      
      sendData(combinedVel, combinedDis, lastTimeRecorded);
      if(!checkForBurnout(combinedVel,deltaTime))
        checkForApogee(stratoVel, combinedDis); // pitot tube will stop working after apogee
      
      writeToSD(deltaTime, stratoDis, pitoVel);
    }
  }
  else if(descending) // assume pitot tube no longer functioning
  {
    havePitoData = false;
     // resetting time variables
    int deltaTime = (int)(millis() - lastTimeRecorded);
    lastTimeRecorded = millis();
    
    stratoDis = getDisStrato();
    int deltaDis = stratoDis - prevStratoDis;
    prevStratoDis = stratoDis;
    stratoVel = getVelStrato(deltaTime, deltaDis);

    if(haveStratoData)
    {
      combineValues(&combinedVel, &combinedDis, 0, 0, stratoVel, stratoDis, (float) deltaTime / numMillisecondsInSecond);
      sendData(stratoVel, stratoDis, lastTimeRecorded );
      checkForLanding(combinedDis);
      writeToSD(deltaTime, stratoDis, pitoVel);
    }
    else
    {
      sendData("No Data");
    }
  }
  else // preLaunch or grounded
  {
    // resetting time variables
    int deltaTime = (int)(millis() - lastTimeRecorded);
    lastTimeRecorded = millis();
     
    // get pitot tube values
    pitoVel = getVelocityPito();
    if(havePitoData)
      pitoDis = getDisPito(deltaTime, pitoVel);

    // get stratologger values
    stratoDis = getDisStrato();
    int deltaDis = stratoDis - prevStratoDis;
    prevStratoDis = stratoDis;
    stratoVel = getVelStrato(deltaTime, deltaDis);
    
    if(!haveStratoData && !havePitoData)
      sendData("No Data");
    else
    {
      combineValues(&combinedVel, &combinedDis, pitoVel, pitoDis, stratoVel, stratoDis, (float) deltaTime / numMillisecondsInSecond);
      sendData(combinedVel, combinedDis, lastTimeRecorded);
      checkForLiftoff(combinedVel, combinedDis);
      writeToSD(deltaTime, stratoDis, pitoVel);
    }
    
    
    
    //data.close();
  }
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
      *combinedVel = pitoVel; //RATIO_PITO_VEL * pitoVel + RATIO_STRATO_VEL * stratoVel;
      *combinedDis = stratoDis; //RATIO_PITO_DIS * pitoDis + RATIO_STRATO_DIS * stratoDis;
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



