// -----------------------------------------------------------------------------------
// Any functions directly operating with the airbreaks.
// -----------------------------------------------------------------------------------

void openAirBreaks()
{
  timeToTriggerAirbreak = 2147483647; // Max long prevents being triggered
                                      // more than once.
}

// -----------------------------------------------------------------------------------
// This function checks to see if airbreaks are on or off during this sketch.
// It checks the SD card for a file "BREAK". If this file exhists and the first
// character in the file is 'y', then the airbreaks are turned on.
// -----------------------------------------------------------------------------------
File sdFile;
void checkAirBreaks()
{
  sendMessage("Attempting to open checkAirBreak file...\n");
  sdFile = SD.open("break.txt");
  if(sdFile)
  {
    char temp = sdFile.read();
    if(temp == 'Y' || temp == 'y')
    {
      airBreaking = true;
    }
    else
    {
      airBreaking = false;
    }
  }
  else
  {
    sendMessage("Didn't find file.\n");
    airBreaking = false;
  }
}

void controlAirBreaks()
{
  //............................................................................................??????????????????
}
