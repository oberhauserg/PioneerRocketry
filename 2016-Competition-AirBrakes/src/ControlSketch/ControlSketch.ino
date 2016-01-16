// -----------------------------------------------------------------
// This is a early iteration of the Pioneer Rocketry electronics
// code. Hopefully it will be used as the basis for the code used 
// for the actual launch. It includes functions for pitot tube that
// are left blank until we actually have the pitot tube. However, 
// the strattologger is fully implemented. It also sends correct 
// data to the XBee.
// Author: Jacob Napp
// ----------------------------------------------------------------- 
#include <MatrixMath.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050_9Axis_MotionApps41.h>
#include <SdFat.h>
#include <SPI.h>

//Include our classes
#include "AirBrakes.h"
#include "Constants.h"
#include "Controller.h"


// Note: You can only have one software serial running 
// at one time
//SoftwareSerial Strato(10, 11); //RX, TX



long lastTimeRecorded;
bool mpuWorking = true;
bool airBreaking = false;

int apogee = 0;

void kalmanFilter(float *combinedVel, int *combinedDis, int deltaTime);

void initializeGyro();

void initializeStrato();

void initializeApogee();


Controller * cont;

AirBrakes * brakes; 

void setup() 
{
  Wire.begin(); // used for 9DOF 
  Serial.begin(9600);
  Serial2.begin(9600); 
  
  delay(2000); // gives serial time to open up

  // 9DOF initialization
  initializeGyro();

  // initialize Strattologger
  initializeStrato();

  brakes = new AirBrakes();

  cont = new Controller(*brakes);

  checkAirBreaks();
  if(airBreaking)
    sendMessage("Airbreaks are on.\n");
  else
    sendMessage("AirBreaks are off.\n");

  // initialize apogee
  initializeApogee(); 
    
  sendMessage("SequenceNumber,Time(milliSec),Altitude(feet),Velocity(fps)\n");
  lastTimeRecorded = micros();
} // end setup

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



void loop() 
{
  
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
    combineValues(&combinedVel, &combinedDis, pitoVel, pitoDis, stratoVel, stratoDis, deltaTime);

    sendData(combinedVel, combinedDis);

    controlAirBreaks();

    // check for state change
    checkForApogee(combinedVel,combinedDis);
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
    combineValues(&combinedVel, &combinedDis, pitoVel, pitoDis, stratoVel, stratoDis, deltaTime);
      
    sendData(combinedVel, combinedDis);
    if(!checkForBurnout(combinedVel,combinedDis))
      checkForApogee(combinedVel, combinedDis);
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

    havePitoData = false;
    combineValues(&combinedVel, &combinedDis, 0, 0, stratoVel, stratoDis, deltaTime);
    
    sendData(stratoVel, stratoDis);
    checkForLanding(combinedDis);
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
      combineValues(&combinedVel, &combinedDis, pitoVel, pitoDis, stratoVel, stratoDis, deltaTime);
    }
    sendData(combinedVel, combinedDis);
    checkForLiftoff(pitoVel, stratoVel);
  }
}

int sequenceNum = 0;
int maxInt = 1000;

void sendData(int combinedDis)
{
  if(sequenceNum == maxInt)
    sequenceNum = 0;
  sequenceNum++;
  String num = String(sequenceNum);
  String msgTime = String(lastTimeRecorded);
  String vel = "None";
  String dis = String(combinedDis);
  String temp = num + ',' + msgTime + ',' + dis + "," + vel + '\n';
  sendMessage(temp);
}

void sendData(float combinedVel, int combinedDis)
{
  if(sequenceNum == maxInt)
    sequenceNum = 0;
  sequenceNum++;
  String num = String(sequenceNum);
  String msgTime = String(lastTimeRecorded);
  String vel = String(combinedVel,3);
  String dis = String(combinedDis);
  String temp = num + ',' + msgTime + ',' + dis + "," + vel + "\n";
  sendMessage(temp);
}

void sendData(String msg)
{
  sequenceNum++;
  String num = String(sequenceNum);
  String msgTime = String(lastTimeRecorded);
  String temp = num + "," + msgTime + "," + msg + "\n";
  sendMessage(temp);
}


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
  localPitoDisplacement = localPitoDisplacement + deltaTime * pitoVel;
  return (int)localPitoDisplacement;
}

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

int numMillisecondsInSecond = 1000;
// returns fps

float getVelStrato(int deltaTime, int deltaDis)
{
  return ((float)deltaDis)* numMillisecondsInSecond / ((float)deltaTime);
}

void sendMessage(String msg)
{
  Serial.print(msg);
}

long timeDelay = 7000; // time to wait for response in micro seconds

String receiveMessage()
{
  long endTime = millis();
  endTime += timeDelay;
  while(Serial.available() <= 0 && (millis() <= endTime) );
  if(Serial.available() > 0)
  {
    String message = Serial.readString();
    return message;
  }
  else
  {
    sendMessage("Timeout occured! Did not recieve you message.\n");
    return "TIME OUT";
  }
}

float RATIO_PITO_VEL = 0.7f;
float RATIO_PITO_DIS = 0.3f;
float RATIO_STRATO_VEL = 0.3f;
float RATIO_STRATO_DIS = 0.7f;

void combineValues(float *combinedVel, int *combinedDis, float pitoVel, int pitoDis, float stratoVel, int stratoDis, int deltaTime)
{
    if(haveStratoData && havePitoData)
    {
      *combinedVel = RATIO_PITO_VEL * pitoVel + RATIO_STRATO_VEL * stratoVel;
      *combinedDis = RATIO_PITO_DIS * pitoDis + RATIO_STRATO_DIS * stratoDis;
      kalmanFilter(combinedVel, combinedDis, deltaTime);
    }
    else if(haveStratoData)
    {
      *combinedVel = stratoVel;
      *combinedDis = stratoDis;
    }
    else
    {
      *combinedVel = pitoVel;
      *combinedDis = pitoDis;
    }
}

float processErrorsDis = 8.0f;
float processErrorsVel = 8.0f;
float E[][3] = {{0.0001f, 0, 0}, {0, 0.0001f, 0}, {0, 0, 0.0001f}};

float observationErrorDis = 5.0f;
float obvservationErrorVel = 8.0f;

float predX[3];
float prevX[] = {0,0,0};
float A[][3] = {
               {1, 0, 0},
               {1, 1, 0}, 
               {.5f, 1, 1}
               };
float prevP[][3] ={
                   {100,0,0},
                   {0,100,0},
                   {0,0,100}
                   };
float P[3][3];
float PBar[2][2];
float R[][2] = {
                {observationErrorDis * observationErrorDis, observationErrorDis * obvservationErrorVel},
                {observationErrorDis * obvservationErrorVel, obvservationErrorVel * obvservationErrorVel}
               };
float K[3][2];
float H[][3] = {{1,0},{0,1},{0, 0}};
float I[][3] = {
                {1,0,0},
                {0,1,0},
                {0,0,1}
                };

float Y[2]; // observed displacement and velocity

float X[2]; // real value for displacement and velocity

float gravity = -32.2f;



void setUpFilter(float combinedVel, int combinedDis)
{
  /*
}
  prevX[0] = combinedDis;
  prevX[1] = combinedVel;

  prevP[0][0] = processErrorsDis * processErrorsDis;
  prevP[0][1] = processErrorsDis * processErrorsVel;
  prevP[1][0] = processErrorsDis * processErrorsVel;
  prevP[1][1] = processErrorsVel * processErrorsVel;
  */
}

void kalmanFilter(float *combinedVel, int *combinedDis, int deltaTime)
{
  A[0][1] *= deltaTime;
  A[0][2] *= deltaTime * deltaTime;
  A[1][2] *= deltaTime;

  // predX = A * prevX + B * gravity + W; // ignore B * gravity and W
  updatePredX();
  
  
  // PBar = A * prevP * A^T + E // E-> scalar .0001 ????????????????????
  updatePBar();
  
  // K = (P * H^T) / (H * P * H^T + R); 
  updateK();

  // step 5: new observation
  Y[0] = *combinedDis;
  Y[1] = *combinedVel;

  // step 6: calculating the current state
  
  //X = predX + K(Y - H * predX);
  UpdateX();

  // step 7: updating the process covariance matrix
  // P = (I - K * H) PBar // note that H is identity in this instance, thus is disregarded
  UpdateP();
  
  // step 8: current values become previous values
  prevP[0][0] = P[0][0];
  prevP[0][1] = P[0][1];
  prevP[1][0] = P[1][0];
  prevP[1][1] = P[1][1];

  prevX[0] = X[0];
  prevX[1] = X[1];
  
  *combinedDis = prevX[0];
  *combinedVel = prevX[1];
}

 // predX = A * prevX + B * gravity + W; // ignore B * gravity and W
void updatePredX()
{
  // A * prevX
  Matrix2.Multiply((float*)A,(float*)prevX,3,3,1,(float*)predX);
}

// PBar = A * prevP * A^T + E
void updatePBar()
{
  // A^T = A 
  float transposeA[3][3];
  Matrix2.Transpose((float*)A, 3, 3, (float*)transposeA);

  // Atimesprev = A * prevP
  float AtimesprevP[3][3];
  Matrix2.Multiply((float*)A, (float*)prevP, 3, 3, 3, (float*)AtimesprevP);

  // tempP = (A * prevP) * A^T
  float tempP[3][3];
  Matrix2.Multiply((float*)AtimesprevP, (float*)transposeA, 3, 3, 3, (float*)tempP);

  // P = tempP + E;
  Matrix2.Add((float*)tempP,(float*)E, 3, 3, (float*)P);
}

// K = (P * H^T) / (H * P * H^T + R); // H is identity, so we remove it.
void updateK()
{
  // transpose = H^T
  float transpose[3][2];
  Matrix2.Transpose((float*)H,2,3,(float*)transpose);

  // P * H^T
  float temp[3][2];
  Matrix2.Multiply((float*)P,(float*)transpose,3,3,2,(float*)temp);

  // temp2 = H * P
  float temp2[2][3];
  Matrix2.Multiply((float*)H,(float*)P,2,3,3,(float*)temp2);

  // temp3 = ((H * P) * H^T
  float temp3[2][2];
  Matrix2.Multiply((float*)temp2,(float*)temp,2,3,2,(float*)temp3);

  //(H * P * H^T + R)
  float temp4[2][2];
  Matrix2.Add((float*)temp3,(float*)R,2,2,(float*)temp4);

  // invert(H * P * H^T + R)
  float temp5[2][2];
  Matrix2.Invert((float*)temp5,2);

  // (P * H^T) inverse(H * P * H^T + R)
  Matrix2.Multiply((float*)temp,(float*)temp5,3,2,2,(float*)K);
}

//X = predX + K(Y - H * predX); H is identity so we remove it
//X = predX + K(Y - predX);
void UpdateX()
{
  // temp0 = H * predX
  float temp0[2];
  Matrix2.Multiply((float*)H,(float*)predX,2,3,1,(float*)temp0);
  
  // temp[2] = Y[2] - temp0;
  float temp[2];
  Matrix2.Subtract((float*)Y, (float*)temp0, 2, 1, (float*)temp);

  // temp2[2] = K[2][2] * temp[2];
  float temp2[2];
  Matrix2.Multiply((float*)K, (float*)temp, 2, 2, 1, (float*)temp2);

  // X = predX[2] + temp2[2]
  Matrix2.Add((float*)predX, (float*)temp2, 2, 1, (float*)X);
}

// P = (I - K * H) P // note that H is identity in this instance, thus is disregarded
void UpdateP()
{
  // temp0 = K * H;
  float temp0[3][3];
  Matrix2.Multiply((float*)K,(float*)H,3,2,3,(float*)temp0);
  
  // temp[3][3] = I - temp0
  float temp[3][3];
  Matrix2.Subtract((float*)I, (float*)temp0, 3, 3, (float*)temp);

  // temp2[3][3] = temp * P;
  float temp2[3][3];
  Matrix2.Multiply((float*)temp,(float*)P, 3, 3, 3, (float*)temp2);

  // P[2][2] = temp2[3][3]
  Matrix2.Copy((float*)temp2, 3, 3, (float*)P);
}

void controlAirBreaks()
{
  //............................................................................................??????????????????
}

int LIFTOFF_THRESHHOLD = 20;
int LIFTOFF_NUM_AVE = 3;
float velocity[] = {0.0f,0.0f,0.0f};
int currentPnt = 0;
void checkForLiftoff(float vel1, float vel2)
{ 
  if(vel1 > vel2)
    velocity[currentPnt++] = vel1;
  else
    velocity[currentPnt++] - vel2;
  if(currentPnt == LIFTOFF_NUM_AVE)
    currentPnt = 0;
  float sum;
  for(int i = 0; i < LIFTOFF_NUM_AVE; i++)
  {
    sum += velocity[i];
  }
  float average = sum/LIFTOFF_NUM_AVE;
  if(average > LIFTOFF_THRESHHOLD)
  {
    sendMessage("Liftoff\n");
    preLaunch = false;
    engineBurning = true;
  }
}

// returns true if has burned out
// prevents checking for apogee at same time
int NUM_BURNOUT_SAMPLES = 4;
float burnoutVel[] = {0.0f,0.0f,0.0f,0.0f};
int burnoutTime[] = {0,0,0,0};
int burnoutPnt = 0;

bool checkForBurnout(float vel, int dis)
{ 
  burnoutVel[burnoutPnt] = vel;
  burnoutTime[burnoutPnt++] = dis;
  float sumAccelerations;
  if(burnoutPnt == NUM_BURNOUT_SAMPLES)
    burnoutPnt = 0;
  // acceleration = (inital velocity - current velocity ) / time between them
  for(int i = 1; i < NUM_BURNOUT_SAMPLES; i++)
  {
    sumAccelerations += (burnoutVel[i - 1] - burnoutVel[i]) / burnoutTime[i];
  }
  float aveAccelerations = sumAccelerations / (NUM_BURNOUT_SAMPLES - 1);
  if(aveAccelerations <= 0 )
  {
    sendMessage("Reached Burnout.\n");
    engineBurning = false;
    midLaunch = true;
    setUpFilter(vel, dis);
    return true;
  }
  return false;
}

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
    sum += vel;
  }
  int ave = sum / NUM_APOGEE_AVE;
  if(ave < 0)
  {
    midLaunch = false;
    descending = true;
    int maxApogee = 0;
    for(int i = 0; i < NUM_APOGEE_AVE; i++)
    {
      if(apogeeDis[i] > maxApogee)
        maxApogee = apogeeDis[i];
    }
    apogee = maxApogee;
    String apogeeString = String(maxApogee);
    sendMessage("Apogee is : " + apogeeString + "\n");
    writeApogeeToArduino(apogee);
  }
}

int NUM_LAND_AVE = 15;
int landingDisplacements[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int landingPnt = 0;

void checkForLanding(int dis)
{
  landingDisplacements[landingPnt++] = dis;
  if(landingPnt == NUM_LAND_AVE)
    landingPnt = 0;
  int sum;
  for(int i = 0; i < NUM_LAND_AVE; i++)
  {
    sum += landingDisplacements[i];
  }
  int ave = sum / NUM_LAND_AVE;
  if(ave == 0)
  {
    sendMessage("Has Landed.\n");
    descending = false;
    landed = true;
  }
  
  // last 15 measurements average difference < 1;
}

void writeApogeeToArduino(int apogee)
{
  //????????????????????????????????????????????????????????????????????????????????????????????????????
}

void initializeApogee()
{
  readApogeeFromSDCard();

  // get apogee from XBee
  sendMessage("Current apogee is set at : " + String(apogee) + 
              "\nDo you want to change this. \"Y\" -> change. \"N\" -> leave as is\n");
  String response = receiveMessage(); 
  response.toUpperCase();
  if(response.equals("Y") || response.equals("Y\n")) // good to here
  {
    bool badMessage = true;
    int numTimesInLoop = 0;
    int maxTimesInLoop = 10;
    while(badMessage && numTimesInLoop++ != maxTimesInLoop)
    {
      sendMessage("Please input your altitude as an integer. (No Periods, Spaces, Commas, or \\n).\n");
      sendMessage("Don't mess this up!!!\n");
      response = receiveMessage();
      sendMessage("Current apogee is set at : " + response +      // BAD MESSAGE
                "\nIs this correct.  \"Y\" -> correct. \"N\" -> re do");
      String response2 = receiveMessage();
      response2.toUpperCase();
      if(response2.equals("Y") || response2.equals("Y\n"))
      {
        badMessage = false;
        apogee = response.toInt();
      }
      else
        badMessage = true;
    }  
    if(numTimesInLoop == maxTimesInLoop)
    {
      sendMessage("Exceeded max number of attemps!\n");       
    }
  }

  sendMessage("Apogee set at :" + String(apogee) + "\n");
}


void readApogeeFromSDCard()
{
  
}

//-------------------------------------------------------------------------------------
// Beginning of MagwickQuaternionUpdate function
//-------------------------------------------------------------------------------------
// Implementation of Sebastian Madgwick's "...efficient orientation filter for... inertial/magnetic sensor arrays"
// (see http://www.x-io.co.uk/category/open-source/ for examples and more details)
// which fuses acceleration, rotation rate, and magnetic moments to produce a quaternion-based estimate of absolute
// device orientation -- which can be converted to yaw, pitch, and roll. Useful for stabilizing quadcopters, etc.
// The performance of the orientation filter is at least as good as conventional Kalman-based filtering algorithms
// but is much less computationally intensive---it can be performed on a 3.3 V Pro Mini operating at 8 MHz!
//9DOF sensor variables
MPU6050 mpu;
#define GyroMeasError PI * (40.0f / 180.0f)       // gyroscope measurement error in rads/s (shown as 3 deg/s)
#define GyroMeasDrift PI * (0.0f / 180.0f)      // gyroscope measurement drift in rad/s/s (shown as 0.0 deg/s/s)
// There is a tradeoff in the beta parameter between accuracy and response speed.
// In the original Madgwick study, beta of 0.041 (corresponding to GyroMeasError of 2.7 degrees/s) was found to give optimal accuracy.
// However, with this value, the LSM9SD0 response time is about 10 seconds to a stable initial quaternion.
// Subsequent changes also require a longish lag time to a stable output, not fast enough for a quadcopter or robot car!
// By increasing beta (GyroMeasError) by about a factor of fifteen, the response time constant is reduced to ~2 sec
// I haven't noticed any reduction in solution accuracy. This is essentially the I coefficient in a PID control sense; 
// the bigger the feedback coefficient, the faster the solution converges, usually at the expense of accuracy. 
// In any case, this is the free parameter in the Madgwick filtering and fusion scheme.
#define beta sqrt(3.0f / 4.0f) * GyroMeasError   // compute beta
#define zeta sqrt(3.0f / 4.0f) * GyroMeasDrift   // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value
#define Kp 2.0f * 5.0f // these are the free parameters in the Mahony filter and fusion scheme, Kp for proportional feedback, Ki for integral
#define Ki 0.0f

int16_t a1, a2, a3, g1, g2, g3, m1, m2, m3;     // raw data arrays reading

uint8_t MagRate;     // read rate for magnetometer data

float pitch, yaw, roll;
float deltat = 0.0f;        // integration interval for both filter schemes
uint16_t lastUpdate = 0; // used to calculate integration interval
uint16_t now = 0;        // used to calculate integration interval

float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 
float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};    // vector to hold quaternion
float eInt[3] = {0.0f, 0.0f, 0.0f};       // vector to hold integral error for Mahony method

        void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
        {
            float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
            float norm;
            float hx, hy, _2bx, _2bz;
            float s1, s2, s3, s4;
            float qDot1, qDot2, qDot3, qDot4;

            // Auxiliary variables to avoid repeated arithmetic
            float _2q1mx;
            float _2q1my;
            float _2q1mz;
            float _2q2mx;
            float _4bx;
            float _4bz;
            float _2q1 = 2.0f * q1;
            float _2q2 = 2.0f * q2;
            float _2q3 = 2.0f * q3;
            float _2q4 = 2.0f * q4;
            float _2q1q3 = 2.0f * q1 * q3;
            float _2q3q4 = 2.0f * q3 * q4;
            float q1q1 = q1 * q1;
            float q1q2 = q1 * q2;
            float q1q3 = q1 * q3;
            float q1q4 = q1 * q4;
            float q2q2 = q2 * q2;
            float q2q3 = q2 * q3;
            float q2q4 = q2 * q4;
            float q3q3 = q3 * q3;
            float q3q4 = q3 * q4;
            float q4q4 = q4 * q4;

            // Normalise accelerometer measurement
            norm = sqrt(ax * ax + ay * ay + az * az);
      #ifdef debug
      Serial.print("Norm for accelerometer = ");
      Serial.println(norm);
      delay(1000);
      #endif
            if (norm == 0.0f) return; // handle NaN
            norm = 1.0f/norm;
            ax *= norm;
            ay *= norm;
            az *= norm;

            // Normalise magnetometer measurement
            norm = sqrt(mx * mx + my * my + mz * mz);
     #ifdef debug
     Serial.print("norm for Mag = ");
     Serial.println(norm);
     delay(1000);
     #endif
            if (norm == 0.0f) return; // handle NaN
            norm = 1.0f/norm;
            mx *= norm;
            my *= norm;
            mz *= norm;

            // Reference direction of Earth's magnetic field
            _2q1mx = 2.0f * q1 * mx;
            _2q1my = 2.0f * q1 * my;
            _2q1mz = 2.0f * q1 * mz;
            _2q2mx = 2.0f * q2 * mx;
            hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
            hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
            _2bx = sqrt(hx * hx + hy * hy);
            _2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
            _4bx = 2.0f * _2bx;
            _4bz = 2.0f * _2bz;

            // Gradient decent algorithm corrective step
            s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
            s2 = _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
            s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
            s4 = _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
            norm = sqrt(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);    // normalise step magnitude
            norm = 1.0f/norm;
            s1 *= norm;
            s2 *= norm;
            s3 *= norm;
            s4 *= norm;

            // Compute rate of change of quaternion
            qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
            qDot2 = 0.5f * (q1 * gx + q3 * gz - q4 * gy) - beta * s2;
            qDot3 = 0.5f * (q1 * gy - q2 * gz + q4 * gx) - beta * s3;
            qDot4 = 0.5f * (q1 * gz + q2 * gy - q3 * gx) - beta * s4;

            // Integrate to yield quaternion
            q1 += qDot1 * deltat;
            q2 += qDot2 * deltat;
            q3 += qDot3 * deltat;
            q4 += qDot4 * deltat;
            norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
            norm = 1.0f/norm;
            q[0] = q1 * norm;
            q[1] = q2 * norm;
            q[2] = q3 * norm;
            q[3] = q4 * norm;

        }
//-------------------------------------------------------------------------------------
// end of MagwickQuaternionUpdate function
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// Begin of getRatioVertical function
//-------------------------------------------------------------------------------------
float getRatioVertical()
{
  if(mpu.getIntDataReadyStatus() == 1) // wait for data ready status register to update all data registers
  mpu.getMotion9(&a1, &a2, &a3, &g1, &g2, &g3, &m1, &m2, &m3);
  now = micros();
  deltat = ((now - lastUpdate)/1000000.0f); // set integration time by time elapsed since last filter update
  lastUpdate = now;

  // Switching x and z original below
   //MadgwickQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f,  mx,  my, mz);
  MadgwickQuaternionUpdate(az, ay, ax, gz*PI/180.0f, gy*PI/180.0f, gx*PI/180.0f,  mz,  my, mx);
  yaw   = atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]);   
  pitch = -asin(2.0f * (q[1] * q[3] - q[0] * q[2]));
  roll  = atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
  pitch *= 180.0f / PI;
  yaw   *= 180.0f / PI + 1.63; // Declination at Platteville WI is 1 degrees 38 minutes on 2015-12-29
  roll  *= 180.0f / PI;
  // modified so that roll and pitch are at 0 when y-axis is pointing up
  roll = roll - 90.0f;
    
  /*
  Serial1.print("Yaw, Pitch, Roll: ");
  Serial1.print(yaw, 2);
  Serial1.print(", ");
  Serial1.print(pitch, 2);
  Serial1.print(", ");
  Serial1.print(roll, 2);
  */
  float ratio = CaclulateUpwardDisplacmentRatio(pitch, roll);
  return ratio;
}
//-------------------------------------------------------------------------------------
// End of getRatioVertical function
//-------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
// Beginning of CaclulateUpwardDisplacmentRatio function
//-------------------------------------------------------------------------------------
float CaclulateUpwardDisplacmentRatio(float angle1, float angle2)
{
  // u = cos(AngleFromXAxis) * i + cos(AngleFromYAxis) * j + cos(AngleFromZAxis) * k
  // sqrt( 1 - cos^2(AngleFromXAxis) -cos^2(AngleFromYAxis)) = cos(AngleFromZAxis)
  // sqrt( 1 - cos^2(90 - AngleFromXAxis) -cos^2(90 - AngleFromYAxis)) = cos(AngleFromZAxis)
  //                 What Gyro gives us          What gyro gives us

  //#define debugThis

  if(angle1 > 90 || angle2 > 90)
    return 0.0f;
  

  // convert to radians
  angle1 = angle1 * PI / 180.0f;
        #ifdef debugThis
        Serial.print("angle1 is ");
        Serial.println(angle1);
        #endif
  angle2 = angle2 * PI / 180.0f;
        #ifdef debugThis
        Serial.print("angle2 is ");
        Serial.println(angle2);
        #endif
  float firstValue = cos(PI/2.0f - angle1);
        #ifdef debugThis
        Serial.print("firstValue is ");
        Serial.println(firstValue);
        #endif
  float secondValue = cos(PI/2.0f - angle2);
        #ifdef debugThis
        Serial.print("secondValue is ");
        Serial.println(secondValue);
        #endif
  float answer = 1 - firstValue*firstValue - secondValue*secondValue;
        #ifdef debugThis
        Serial.print("answer is ");
        Serial.println(answer);
        #endif
  answer = sqrt(answer);
        #ifdef debugThis
        Serial.print("answer is ");
        Serial.println(answer);
        #endif
  return answer;
}
//-------------------------------------------------------------------------------------
// End of CaclulateUpwardDisplacmentRatio function
//-------------------------------------------------------------------------------------

void initializeGyro()
{
  mpu.initialize();
  mpuWorking = mpu.testConnection();
  if(mpuWorking)
    sendMessage("GYRO working!\n");
  else
    sendMessage("GYRO malfunctioning!\n");
  
  // Set up the accelerometer, gyro, and magnetometer for data output

   mpu.setRate(7); // set gyro rate to 8 kHz/(1 * rate) shows 1 kHz, accelerometer ODR is fixed at 1 KHz

   MagRate = 10; // set magnetometer read rate in Hz; 10 to 100 (max) Hz are reasonable values
// Digital low pass filter configuration. 
// It also determines the internal sampling rate used by the device as shown in the table below.
// The accelerometer output rate is fixed at 1kHz. This means that for a Sample
// Rate greater than 1kHz, the same accelerometer sample may be output to the
// FIFO, DMP, and sensor registers more than once.
/*
 *          |   ACCELEROMETER    |           GYROSCOPE
 * DLPF_CFG | Bandwidth | Delay  | Bandwidth | Delay  | Sample Rate
 * ---------+-----------+--------+-----------+--------+-------------
 * 0        | 260Hz     | 0ms    | 256Hz     | 0.98ms | 8kHz
 * 1        | 184Hz     | 2.0ms  | 188Hz     | 1.9ms  | 1kHz
 * 2        | 94Hz      | 3.0ms  | 98Hz      | 2.8ms  | 1kHz
 * 3        | 44Hz      | 4.9ms  | 42Hz      | 4.8ms  | 1kHz
 * 4        | 21Hz      | 8.5ms  | 20Hz      | 8.3ms  | 1kHz
 * 5        | 10Hz      | 13.8ms | 10Hz      | 13.4ms | 1kHz
 * 6        | 5Hz       | 19.0ms | 5Hz       | 18.6ms | 1kHz
 */
  mpu.setDLPFMode(4); // set bandwidth of both gyro and accelerometer to ~20 Hz

  // Full-scale range of the gyro sensors:
  // 0 = +/- 250 degrees/sec, 1 = +/- 500 degrees/sec, 2 = +/- 1000 degrees/sec, 3 = +/- 2000 degrees/sec
  mpu.setFullScaleGyroRange(0); // set gyro range to 250 degrees/sec

  // Full-scale accelerometer range.
  // The full-scale range of the accelerometer: 0 = +/- 2g, 1 = +/- 4g, 2 = +/- 8g, 3 = +/- 16g
  mpu.setFullScaleAccelRange(0); // set accelerometer to 2 g range

  mpu.setIntDataReadyEnabled(true); // enable data ready interrupt
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

void checkAirBreaks() // needs to be implemented..............................................................................................................
{
}


