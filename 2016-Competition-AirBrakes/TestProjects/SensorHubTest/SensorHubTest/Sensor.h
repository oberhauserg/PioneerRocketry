class Sensor
{

public:

   void virtual initialize() = 0;

   void virtual update() = 0;
   
   char* setId(char* ident){ id = ident; }

   char* getId(){ return id; }

protected:

   char* id;

};