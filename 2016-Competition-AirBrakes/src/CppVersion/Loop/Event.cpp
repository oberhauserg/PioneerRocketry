#include "Event.h"

Event::Event()
{
  #ifdef TESTING_EVENT_
  xb.InitializeXBee();
  #endif
  
	descending = burnedOut = false;
	landed = true;

	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		dis[i] = 0; 
		vel[i] = 0;
		acc[i] = 0;
		if ( i != NUM_SAMPLES )
			calcVel[i] = 0;
	}
 for(int i = 0; i < NUM_DIS_APOGEE_SAMPLES; i++)
 {
  disForApogee[i] = 0;
 }
}

void Event::Update(int newDis, float newVel, float newCalcVel, float newAcc)
{
  //xb.SendMessage("input dis is " + String(newDis) + " index is " + String(index) + "\n");
  
  disForApogee[indexDis] = newDis;
  
	dis[index] = newDis;
  //xb.SendMessage("dis[0] 42 is " + String(dis[0]) + "\n");
	vel[index] = newVel;
  //xb.SendMessage("dis[0] 44 is " + String(dis[0]) + "\n");
	acc[index] = newAcc;

  //xb.SendMessage("dis[0] 43 is " + String(dis[0]) + "\n");
	calcVel[index] = newCalcVel;

  //xb.SendMessage("dis[0] is " + String(dis[0]) + "\n");

	aveDis = CalcAveDis();
	aveVel = CalcAveVel();
	aveAcc = CalcAveAcc();
	aveCalcVel = CalcAveCalcVel();
  
  //xb.SendMessage("dis[0] is 2 " + String(dis[0]) + "\n");
  
	if(landed)
		burnedOut = IsBurnedOut();
	else if (burnedOut)
		descending = IsDescending();
	else if (descending)
		landed = IsLanded();

  //xb.SendMessage("dis[0] is 3 " + String(dis[0]) + "\n");
    
	IncrementIndex();

 //xb.SendMessage("dis[0] is 4 " + String(dis[0]) + "\n");

}

float Event::CalcStandardDeviation(int *array1)
{
	// Calc ave
	int sum = 0;
	for (int i = 0; i < NUM_SAMPLES; i++)
		sum += array1[i];
	float ave = (float)sum / NUM_SAMPLES;

	// calc sum of deviations
	float deviation[NUM_SAMPLES];

	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		deviation[i] = (float)array1[i] - ave;
	}
	double temp = 0.0;
	//temp = sqrt();
	for (int i = 0; i < NUM_SAMPLES; i++)
		deviation[i] *= deviation[i];
	float sumSquares = 0;
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		sumSquares += deviation[i];
	}
	long double aveDeviations = sumSquares / NUM_SAMPLES;

	long double temp1 = std::sqrt(aveDeviations);
	float stdDev = static_cast<float>(temp1);

	return stdDev;
}

float Event::CalcAveDis()
{
	int sum = 0;
	for (int i = 0; i < NUM_SAMPLES; i++)
 {
    //xb.SendMessage("dis " + String(i) + " " + String(dis[i]) + "\n");
		sum += dis[i];
 }
	return (float)sum / NUM_SAMPLES;
}

float Event::CalcAveVel()
{
	float sum = 0;
	for (int i = 0; i < NUM_SAMPLES; i++)
		sum += vel[i];
	return sum / NUM_SAMPLES;
}

float Event::CalcAveAcc()
{
	float sum = 0;
	for (int i = 0; i < NUM_SAMPLES; i++)
		sum += acc[i];
	return sum / NUM_SAMPLES;
}


void Event::IncrementIndex()
{
	index++;
	if (index == NUM_SAMPLES)
		index = 0;
  indexDis++;
  if(indexDis == NUM_DIS_APOGEE_SAMPLES)
    indexDis = 0;
}

void Event::CalcApogee()
{
	int maxApogee = 0;
	for (int i = 0; i < NUM_DIS_APOGEE_SAMPLES; i++)
	{
		if (disForApogee[i] > maxApogee)
			maxApogee = disForApogee[i];
	}
	apogee = maxApogee;
}

float Event::CalcAveCalcVel()
{
	float sum = 0;
	for (int i = 0; i < NUM_SAMPLES; i++)
		sum += calcVel[i];
	return sum / NUM_SAMPLES;
}

bool Event::IsDescending()
{
  //XBee xb;
  //xb.InitializeXBee();
  //xb.SendMessage("vel " + String(aveCalcVel) + "\n");
	if (aveCalcVel < DESC_VEL_INDICATOR)
	{
		CalcApogee();
		burnedOut = false;
		descending = true;
		return true;
	}
	return false;
}


bool Event::IsBurnedOut()
{
	if (aveDis > BURNOUT_HEIGHT)
	{
		landed = false;
		burnedOut = true;
		return true;
	}
	return false;
}

bool Event::IsLanded()
{
  if(aveCalcVel > - DESC_VEL_INDICATOR)
  {
     descending = false;
     burnedOut = true;
     return false;
  }
	float stdDev = CalcStandardDeviation(dis);
	if (stdDev < LANDED_DEVIATION)
	{
		landed = true;
		descending = false;
		return true;
	}
	return false;
}

#ifdef TESTING_EVENT
int main()
{
	Event e;
	int dis = 0;
	float vel, calcVel, acc;
	while (dis != -1000)
	{
		cout << "Read in dis, vel, acc, dis == -1000 to end loop" << endl;
		cin >> dis >> vel; // >> acc;
		calcVel = vel;
		acc = 10;
		e.Update(dis, vel, vel, acc);
		if (e.HasBurnedOut())
			cout << "Has burned out" << endl;
		if (e.HasLanded())
			cout << "Has Landed" << endl;
		if (e.HasReachedApogee())
			cout << "Reached apogee " << endl;
	}
	return 0;
	
}
#endif

