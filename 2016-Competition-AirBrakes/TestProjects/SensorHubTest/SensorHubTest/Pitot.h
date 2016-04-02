#define PITOT_DEFAULT_PIN A5

class Pitot : public Sensor
{

public:

   void initialize(){ initialize(PITOT_DEFAULT_PIN); }

   void initialize(int p){ pin = p; }

   void update();

   float getVelocity();

private:

   float velocity;
   int pin;

};