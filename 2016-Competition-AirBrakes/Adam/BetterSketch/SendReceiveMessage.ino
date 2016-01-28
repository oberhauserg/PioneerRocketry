// -----------------------------------------------------------------------------------
// Any functions directly send or receive data from the XBee. Does not include send
// data functions that include a sequence number.
//
// Functions
// void sendMessage(String msg)
// String receiveMessage()
// void initializeApogee()
// -----------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------
// This function sends a message to the XBee. 
// It allows the user to change the serial port the XBee is connected with a minimal
// ammount of modified code. Does not include newline, so user will have to hardcode 
// those in.
// -----------------------------------------------------------------------------------
void sendMessage(String msg)
{
  Serial1.print(msg);
}

// -----------------------------------------------------------------------------------
// This method is called to receive a message via the XBee.
// It waits timeDelay seconds for the message from the user. If a message is not 
// recieved in that time, an error message is sent to the XBee and the String
// "TIME OUT" is returned. Otherwise, the String is read from the XBee returned.
// -----------------------------------------------------------------------------------
long timeDelay = 10000; // time to wait for response in micro seconds

String receiveMessage()
{
  long endTime = millis();
  endTime += timeDelay;
  while(Serial1.available() <= 0 && (millis() <= endTime) );
  if(Serial1.available() > 0)
  {
    String message = Serial1.readString();
    return message;
  }
  else
  {
    sendMessage("\nTimeout occured! Did not recieve you message.\n");
    return "TIME OUT";
  }
}

// -----------------------------------------------------------------------------------
// This function allows the user to modify the apogee via the XBee.
// It first reads the apogee from the SD card. After this has been done, the user 
// then has the option to modify this value via the XBee. It has a while loop that
// will loop through a maximum of 10 times. Each time, the user can modify the 
// apogee. Then they verify that it was correctly input.
// -----------------------------------------------------------------------------------
void initializeApogee()
{
  readApogeeFromSDCard();

  // get apogee from XBee
  sendMessage("Current apogee is set at : " + String(apogee) + 
              "\nDo you want to change this. \"Y\" -> change. \"N\" -> leave as is\n");
  String response = receiveMessage(); 
  response.toUpperCase();
  if(response.equals("Y") || response.equals("Y\n") || response.equals("Y\r")) // good to here
  {
    bool badMessage = true;
    int numTimesInLoop = 0;
    int maxTimesInLoop = 10; // makes sure you don't have infinite loop if XBee communcation cut.
    while(badMessage && (numTimesInLoop++ != maxTimesInLoop))
    {
      sendMessage("\nPlease input your altitude as an integer. (No Periods, Spaces, Commas, or \\n).\n");
      sendMessage("Don't mess this up!!!\n");
      response = receiveMessage();
      sendMessage("\nCurrent apogee is set at : " + response +      // BAD MESSAGE
                "\nIs this correct.  \"Y\" -> correct. \"N\" -> try again\n");
      String response2 = receiveMessage();
      response2.toUpperCase();
      if(response2.equals("Y") || response2.equals("Y\n")|| response2.equals("Y\r"))
      {
        badMessage = false;
        apogee = response.toInt();
      }
      else
        badMessage = true;
    }  
    if(numTimesInLoop == maxTimesInLoop)
    {
      sendMessage("\nExceeded max number of attemps!\n");       
    }
  }
  sendMessage("Apogee set at :" + String(apogee) + "\n");
  writeApogeeToSD(apogee); // update record of apogee in case of power reset
}

