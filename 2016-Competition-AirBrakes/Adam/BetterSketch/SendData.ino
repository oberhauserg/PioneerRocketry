// -----------------------------------------------------------------------------------
// Any functions directly send Data to the XBee. 
// Note this is different from SendfREceiveMesssage because each time data is sent,
// it contains a sequence number.
// -----------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------
// This method sends data via the XBee.
// It sends a data sequence number. Plus the given time data, and displacement data.
// it sends "None" for the velocity data.
// -----------------------------------------------------------------------------------
int SEQ_NUM = 0;
int maxInt = 1000;

void sendData(int combinedDis, long timeData)
{
  if(SEQ_NUM == maxInt)
    SEQ_NUM = 0;
  SEQ_NUM++;
  String num = String(SEQ_NUM);
  String msgTime = String(timeData);
  String vel = "None";
  String dis = String(combinedDis);
  String temp = num + ',' + msgTime + ',' + dis + "," + vel + '\n';
  sendMessage(temp);
}

// -----------------------------------------------------------------------------------
// This method sends data via the XBee.
// It sends a data sequence number. Plus the given time data, velocity data, 
// displacement data.
// -----------------------------------------------------------------------------------
int MAX_SEQ_NUM = 100;

void sendData(float combinedVel, int combinedDis, long timeData)
{
  if(SEQ_NUM++ == MAX_SEQ_NUM)
    SEQ_NUM = 0;
  String num = String(SEQ_NUM);
  String msgTime = String(timeData);
  String vel = String(combinedVel,3);
  String dis = String(combinedDis);
  String temp = num + ',' + msgTime + ',' + dis + "," + vel + "\n";
  sendMessage(temp);
}

// -----------------------------------------------------------------------------------
// This method sends data via the XBee.
// It sends a data sequence number plus the input string value.
// -----------------------------------------------------------------------------------
void sendData(String msg)
{
  if(SEQ_NUM++ == MAX_SEQ_NUM)
    SEQ_NUM = 0;
  
  String num = String(SEQ_NUM);
  String msgTime = String(lastTimeRecorded);
  String temp = num + "," + msgTime + "," + msg + "\n";
  sendMessage(temp);
}


