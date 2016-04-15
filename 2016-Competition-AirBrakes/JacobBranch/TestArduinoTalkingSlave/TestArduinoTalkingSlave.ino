//Update
#include <Wire.h>

uint32_t floatRep;

void setup() {
  Serial.begin(9600);
  Wire.begin(8);
  //Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent); // register event
  Serial.println("Reached end of setup");

}

void loop() {
  
  Serial.println(value);
}

//5V is 1023 for value. 
float calculateSpeed(int value)
{

  return 0.00f;
  
}

void requestEvent()
{
  char floatString[10];

  int value = analogRead(A0);

  float calcedSpeed = calculateSpeed(value);

  dtostrf(value, 10, 1, floatString);

  Serial.println(value);
  Serial.println(floatString);
  
  Wire.write(floatString, 10);
  
  /*floatRep = value;

  char data[sizeof(float)];

  memcpy(data, &value, sizeof value);

  for(int i = 0; i < sizeof(float); i++)
    Wire.write(data[i]);
  
  /*Wire.write((floatRep >> 24) & 0xFF );
  Wire.write((floatRep >> 16) & 0xFF );
  Wire.write((floatRep >> 8) & 0xFF );
  Wire.write((floatRep) & 0xFF );*/
  //Serial.println("I printed!");
  
  //Wire.write("hello "); // respond with message of 6 bytes
  // as expected by master
}

void receiveEvent(int howMany)
{
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
  
  Wire.write(1);       

  /*String temp = String(value,3);
  Serial.println(value);
  Serial.println(String(value,3));
  Serial.println(String(value,3).c_str());
  //delay(1000);
  Wire.write(temp.c_str());*/
  
}

