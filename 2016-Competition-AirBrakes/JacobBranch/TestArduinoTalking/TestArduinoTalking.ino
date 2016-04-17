//Update
#include <Wire.h>


void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  Wire.begin();
  delay(500);
  Serial.println("Reached end of Master setup.");
}
byte data = 0;

float currentValue;

void loop() {
<<<<<<< .mine  Serial.println("top of loop");
  if(millis() % 100 == 0)
  {
      
    char buf [10];
=======  Serial.println("top of loop");
  Wire.requestFrom(8, 10);
>>>>>>> .theirs  

    if (Wire.requestFrom (9, 10))
    {
      
      for (byte i = 0; i < 10; i++)
        buf [i] = Wire.read ();
     
      currentValue = (float)atof(buf);    
    }
    else
    {
      Serial.println("FAIL!");
      // The request failed. Just continue requesting.
    }
  
    Serial.print("Current Value: ");
    Serial.print(currentValue);
    Serial.print("\n");
  }


}
