// -----------------------------------------------------------------------------------
// Any functions directly operating the SD Card.
// -----------------------------------------------------------------------------------

void writeToSD(int deltaTime, int stratoDis, float pitoVel)
{
  
  mpu.getAcceleration  ( &a1, &a2, &a3  );
  ax = a1*16.0f/32768.0f;
  ay = a2 *16.0f/32768.0f;
  az = a3*16.0f/32768.0f;

  mpu.getRotation  ( &g1, &g2, &g3  );
  gx = g1*250.0f/32768.0f; // 250 deg/s full range for gyroscope
  gy = g2*250.0f/32768.0f;
  gz = g3*250.0f/32768.0f;

  mpu.getMag  ( &m1, &m2, &m3 );
  mx = m1*10.0f*1229.0f/4096.0f + 18.0f; // milliGauss (1229 microTesla per 2^12 bits, 10 mG per microTesla)
  my = m2*10.0f*1229.0f/4096.0f + 70.0f; // apply calibration offsets in mG that correspond to your environment and magnetometer
  mz = m3*10.0f*1229.0f/4096.0f + 270.0f;
  
  String info;
  String accel = String(ay,3);
  float ratioUp = getRatioVertical();
  String ratio = String(ratioUp);
  
  info = String(deltaTime) + "," + String(pitoVel) + "," + String(stratoDis) + "," + accel + "," + ratio + "\n";
  
  //data = SD.open("data.txt", FILE_WRITE);
  // if the file opened okay, write to it:
  if(data)
  {
    data.println(String(info));
    data.flush();
  } 
}

// -----------------------------------------------------------------------------------
// This methods write the given apogee to the SD card.
// If an old apogee file is recorded, the file is erased. A new apogee file is then 
// created containing the new apogee.
// -----------------------------------------------------------------------------------
File thisFile;

void writeApogeeToSD(int apogee)
{
  // delete old apogee
  SD.remove("apogee.txt");
  thisFile = SD.open("apogee.txt", FILE_WRITE);
  // if the file opened okay, write to it:
  if(thisFile)
  {
    thisFile.println(String(apogee));
    thisFile.close();
  }
}

// -----------------------------------------------------------------------------------
// This method reads the apogee from the SD card.
// The apogee must be stored as an integer in a file "APOGEE.TXT". The integer is 
// read as a string and then converted into an int. This integer is used to set the 
// apogee variable.
// -----------------------------------------------------------------------------------
File myFile;

void readApogeeFromSDCard()
{
  sendMessage("Attempting to open apogee file...\n");
  myFile = SD.open("apogee.txt");
  if(myFile)
  {
    String apogeeString;
    while(myFile.available())
    {
      char temp = myFile.read();
      apogeeString = apogeeString + String(temp);
    }
    myFile.close();
    apogee = apogeeString.toInt();
    sendMessage("Apogee was read as " + String(apogee) + "\n");
  }
  else
  {
    sendMessage("Error opening apogee file\n");
  }
}

// -----------------------------------------------------------------------------------
// This method is called to update initialize the SD card reader/writer. 
// It prints a message indicating whether it was successful or not.
// -----------------------------------------------------------------------------------
File dataFromLaunch;
int csPin = 4;

void initializeSDCard()
{
  sendMessage("Initializing SD card...\n");
  if (!SD.begin(csPin)) {
    sendMessage("initialization failed!\n");
    return;
  }
  sendMessage("initialization done.\n");
}

// -----------------------------------------------------------------------------------
// This function initializes and opens the data file.
// -----------------------------------------------------------------------------------
void initDataFile()
{
  data = SD.open("data.txt", FILE_WRITE);
  
  if(data)
  {
    data.println("Time(milliSec), PitoVelocity, StratoDisplacement, acceleration, RatioToVertical");
    data.flush();
  }
}
