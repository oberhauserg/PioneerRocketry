#include <Wire.h>
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  Wire.begin();
  delay(500);
    Wire.write(data);
  Serial.println("Reached end of Master setup.");
}
byte data = 0;

void loop() {
  Wire.beginTransmission(8);
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
  delay(2000);
}
