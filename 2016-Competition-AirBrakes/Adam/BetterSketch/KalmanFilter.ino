// -----------------------------------------------------------------------------------
// Any functions used to implement the Kalman filter.
// -----------------------------------------------------------------------------------

#include <MatrixMath.h>

// Kalman Filter. Still needs work...................................................................

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

void kalmanFilter(float *combinedVel, int *combinedDis, float deltaTime)
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
  Matrix2.Copy((float*)P,3,3,(float*)prevP);

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

//X = predX + K(Y - H * predX)
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

// end Kalman Filter....................................................................


