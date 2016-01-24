// -----------------------------------------------------------------------------------
// This function sends a message to the XBee. 
// It allows the user to change the serial port the XBee is connected with a minimal
// ammount of modified code.
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
