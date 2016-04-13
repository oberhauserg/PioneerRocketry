#include <Wire.h>
void setup() {
  Serial.begin(9600);
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Serial.println("Reached end of setup");
  
}

void loop() {
  delay(1000);
}
float value = 4.55f;
void receiveEvent(int howMany)
{
  String temp = String(value,3);
  Serial.println(value);
  Serial.println(String(value,3));
  Serial.println(String(value,3).c_str());
  Wire.write(temp.c_str());

}

