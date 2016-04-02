#include <String>

class Sensor
{

public:

   void virtual update() = 0;

   string getId(){ return id; }

private:

   string id;

};