//Update
#include <Wire.h>

const float offset = 0.0;

const float sensitivity = 1.0;

const int pinSensor = A0;

//Density of the air at around 1000 ft above sea level. (Platteville)
const float airDensity = 1.225; 

const float PCAL1 = 55.555;
const float PCAL2 = -2.222;

const float PITOTCONSTANT_A = 749.4;
const float PITOTCONSTANT_B = 101325;
const float PITOTCONSTANT_C = 0.2868;

uint32_t floatRep;

int value;


char floatString[10];

void setup() {
  Wire.begin(9);
  Wire.onRequest(requestEvent);
  //Wire.onReceive(receiveEvent);
}

void loop() {
  value = analogRead(A0);
  //float voltage = (((float) value)/1023.0f) * 5.0f;
  float calcedSpeed = calculateSpeed(value);
  
  
}

//5V is 1023 for value. 
float calculateSpeed(int rawADC)
{
  float voltage = (((float) rawADC)/1023.0f) * 5.0f;
  float deltaP = voltage * PCAL1 + PCAL2;
  float velocity = PITOTCONSTANT_A * sqrt(pow(deltaP/(PITOTCONSTANT_B+1),PITOTCONSTANT_C) - 1);

  dtostrf(pow(deltaP/(PITOTCONSTANT_B+1),PITOTCONSTANT_C) - 1, 10, 1, floatString);
    
}

void requestEvent()
{
  
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

