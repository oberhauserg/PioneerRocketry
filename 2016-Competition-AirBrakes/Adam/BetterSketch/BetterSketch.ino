// -----------------------------------------------------------------
// This is a early iteration of the Pioneer Rocketry electronics
// code. Hopefully it will be used as the basis for the code used 
// for the actual launch. It includes functions for pitot tube that
// are left blank until we actually have the pitot tube. However, 
// the strattologger is fully implemented. It also sends correct 
// data to the XBee.
// Author: Jacob Napp
// ----------------------------------------------------------------- 

#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_9Axis_MotionApps41.h"
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

  // initialize Strattologger
  initializeStrato();
  

  checkAirBreaks();
  if(airBreaking)
    sendMessage("Airbreaks are on.\n\n");
  else
    sendMessage("AirBreaks are off.\n\n");

  // initialize apogee
  initializeApogee(); 
    
  sendMessage("SequenceNumber,Time(milliSec),Altitude(feet),Velocity(fps)\n");
  lastTimeRecorded = micros();
  
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
  // time controlled airbreak is used in first test only
  if(millis() >= timeToTriggerAirbreak)
    openAirBreaks();
  // end timed airbreak control
  haveStratoData = havePitoData = false;
  if( airBreaking &&  midLaunch)
  {
    // resetting time variables
    int deltaTime = (int)(micros() - lastTimeRecorded);
    lastTimeRecorded = micros();
     
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
  else if((!airBreaking && midLaunch) || engineBurning)
  {
    // resetting time variables
    int deltaTime = (int)(micros() - lastTimeRecorded);
    lastTimeRecorded = micros();
     
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
    combineValues(&combinedVel, &combinedDis, pitoVel, pitoDis, stratoVel, stratoDis, (float)deltaTime / numMillisecondsInSecond);
      
    sendData(combinedVel, combinedDis, lastTimeRecorded);
    if(!checkForBurnout(combinedVel,deltaTime))
      checkForApogee(stratoVel, combinedDis); // pitot tube will stop working after apogee
      
    writeToSD(deltaTime, stratoDis, pitoVel);
  }
  else if(descending) // assume pitot tube no longer functioning
  {
     // resetting time variables
    int deltaTime = (int)(micros() - lastTimeRecorded);
    lastTimeRecorded = micros();
    
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
    int deltaTime = (int)(micros() - lastTimeRecorded);
    lastTimeRecorded = micros();
     
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
      checkForLiftoff(pitoVel, stratoVel);
      writeToSD(deltaTime, stratoDis, pitoVel);
    }
    sendData(combinedVel, combinedDis, lastTimeRecorded);
    checkForLiftoff(pitoVel, stratoVel);
    
    
    
    //data.close();
  }
}

