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

  Wire.requestFrom(8, 10);
  
  char buf [10];

  if (Wire.requestFrom (8, 10))
  {
    for (byte i = 0; i < 10; i++)\
      buf [i] = Wire.read ();
    
     
    currentValue = (float)atof(buf);    
  }
  else
  {
    // The request failed. Just continue requesting.
  }

  Serial.print("Current Value: ");
  Serial.print(currentValue);
  Serial.print("\n");
  /*Wire.beginTransmission(8);
  //Serial.println(data);
  String floatValue = "";
  while(Wire.available() > 0)
  {
    char c = Wire.read();
    floatValue = String(floatValue + c);
  }
  Wire.write(data);
  Serial.println(floatValue);
  float value = floatValue.toFloat();
  Serial.println(String(value, 3));
  Wire.endTransmission();
  delay(2000);*/
}
