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
float currentVelocity;
float unsquare;
float P0 = 101;

void loop() {
      
    char buf [10];

    if (Wire.requestFrom (9, 10))
    {
      for (byte i = 0; i < 10; i++)
        buf [i] = Wire.read ();
     
      currentValue = (float)atof(buf);  
      currentValue += 0.4;
        
    }
    else
    {
      Serial.println("FAIL!");
      // The request failed. Just continue requesting.
    }

     

    unsquare = 293 * (pow(((currentValue)/P0 + 1), 0.286) - 1);

    if(unsquare >= 0)
      currentVelocity = 44.83 * 2.23694 * sqrt(unsquare);
    else
      currentVelocity = 0;
  
    Serial.print("Current Velocity: ");
    Serial.print(currentVelocity);
    Serial.print("\n");



}
