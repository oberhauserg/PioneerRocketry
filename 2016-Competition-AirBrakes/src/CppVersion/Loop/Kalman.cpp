#include "Kalman.h"
//#include "boost_1_60_0/boost/tokenizer.hpp"

//#define _CRT_SECURE_NO_WARNINGS
// test 1?

Kalman::Kalman()
{
	predX(0) = predX(1) = predX(2) = 0;
	prevX(0) = prevX(1) = prevX(2) = 0;
	X(0) = X(1) = X(2) = 0;
	
	// initialize A
	A << 1,1,0.5,
		 0,1,1,
		 0,0,1;
	
	// initialize prevP
	prevP << 100,0,0,
			 0,100,0,
			 0,0,100;
	
	// initialize PBar
	PBar << 0,0,0,
		    0,0,0,
			0,0,0;
	
	// initialize K
	K << 0,0,0,
		 0,0,0,
		 0,0,0;
	
	// initialize P
	P << 100,0,0,
		 0,100,0,
		 0,0,100;
	
	// initialize H
	H << 1,0,0,
		 0,1,0,
		 0,0,1;
	
	// initialize R
	R << processError * processError, 0, 0,
		 0 , processError * processError, 0,
		 0, 0, processError * processError;
		 
	// initialize I identity matrix
	I << 1,0,0,
		 0,1,1,
		 0,0,1;
}

void Kalman::update(int * displacement, float * velocity, float * acceleration, int deltaTime)
{
	A(0,1) *= deltaTime;
	A(0,2) *= deltaTime * deltaTime;
	A(1,2) *= deltaTime;
	
	predX = A * prevX;
	
	PBar = A * prevP * A.transpose();
	
	K = (P * H.transpose() ) * (H * P * H.transpose() +  R).inverse();
	
	Y << (float)*displacement, *velocity, *acceleration;
	
	X = predX + K * (Y - H * predX);
	
	P = (I - K * H) * PBar;
	
	prevX = X;
	
	*displacement = (int)X(0);
	*velocity = X(1);
	*acceleration = X(2);
}
#ifdef DEBUG_KALMAN
int main()
{
	Kalman kal;
	Tokenizer tok;
	int displacement;
	float velocity, acceleration, deltaT;
	const char del = ',';

	std::string line;
	std::ifstream file("data.csv");
	std::ofstream ofile("result.csv");
	if (ofile.is_open())
	{
		ofile << "Sensor Altitude,Sensor Velocity,Sensor Acceleration,"
			  << "Delta Time,Corrected Altitude,Corrected Velocity,"
			  << "Corrected Acceleration,Corrected Delta Time\n";
	}
	if (file.is_open())
	{
		while (getline(file, line))
		{
			tok.Insert(line, ",");

			// displacment
			std::string dis = tok.NextToken();
			displacement = stoi(dis, 0, 10);

			// velocity
			std::string vel = tok.NextToken();
			velocity = stof(vel, 0);

			// acceleration
			std::string acl = tok.NextToken();
			acceleration = stof(acl, 0);

			// deltaT
			std::string time = tok.NextToken();
			deltaT = stof(time, 0);
			
			std::cout << "Read " << displacement << " " << velocity << " " << acceleration << " " << deltaT << endl;
			if (ofile.is_open())
			{
				ofile << to_string(displacement) << "," << to_string(velocity) << "," << to_string(acceleration)
					  << "," << to_string(deltaT) << ",";
			}

			kal.update(&displacement, &velocity, &acceleration, deltaT);

			std::cout << "Corrected " << displacement << " " << velocity << " " << acceleration << " " << deltaT << "\n";
			if (ofile.is_open())
			{
				ofile << to_string(displacement) << "," << to_string(velocity) << "," << to_string(acceleration)
					  << "," << to_string(deltaT) << "\n";
			}
		}
	}
	ofile.close();
	//Kalman kal;
	std::cin >> displacement;
	return 0;
	
}

#endif