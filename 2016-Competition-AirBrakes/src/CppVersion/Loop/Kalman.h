#ifndef _KALMAN_H
#define _KALMAN_H

//#define DEBUG_KALMAN
#ifdef DEBUG_KALMAN
#include <iostream>
#include <fstream>
#include <string>
#include "Tokenizer.cpp"
#endif
//#inclue "Eigen/Dense"


//#undef printf
//#include <stdio.h>
//#include <C:\Program Files (x86)\Arduino\libraries\stlport\stlport.h>

//#include <C:\Users\Jacob\Documents\Arduino\libraries\StandardCplusplus\StandardCplusplus.h>
//#include <stlport.h>
//#include <StandardCplusplus.h>
//#include <fstream>
#undef printf
#include <stdio.h>
#include <C:\Program Files (x86)\Arduino\libraries\stlport\stlport.h>
//#include <C:\Users\Jacob\Documents\Arduino\libraries\StandardCplusplus\StandardCplusplus.h>
#include <Eigen30.h>
#include <Eigen/Dense>

//#include <Eigen313.h>
//#include <C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\Eigen313\Core>

using namespace Eigen;

class Kalman
{
public:
	Kalman();
	// predX = A * prevX + B * gravity + W; // ignore B * gravity and W
	// PBar = A * prevP * A^T + E // E-> scalar .0001 ????????????????????
	// K = (P * H^T) / (H * P * H^T + R); 
	  // step 5: new observation
	void Update(int * displacement, float * velocity, float * acceleration, int deltaTime);
	
private:
	Vector3f predX = Vector3f(3);
	Matrix3f A = Matrix3f(3,3);
	Matrix3f PBar = Matrix3f(3, 3);
	Matrix3f prevP = Matrix3f(3, 3);
	Matrix3f K = Matrix3f(3, 3);
	Matrix3f P = Matrix3f(3, 3);
	Matrix3f H = Matrix3f(3, 3);
	Matrix3f R = Matrix3f(3, 3);
	Matrix3f I = Matrix3f(3, 3);
	Vector3f input = Vector3f(3);
	Vector3f prevX = Vector3f(3);
	Vector3f X = Vector3f(3);
	Vector3f Y = Vector3f(3);
	
	float processError = 8.0f;
};

#endif

