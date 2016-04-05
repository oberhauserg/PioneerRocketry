Created by Jacob Napp, Adam Nielson, Grant Oberhauser for 
the University of Wisconsin Platteville Pioneer Rocketry
Midwest Regional Rocketry Competition.

NOTE: This will not compile unless you do follow these steps.

1. Go into Kalman.h file and change the director 

#include <C:\Program Files (x86)\Arduino\libraries\stlport\stlport.h>

to the folder where you have copied your stlport.h to. For some reason
the Arduino IDE didn't want to find it in its library folder when I put
it there.

2. You must also move the Eigen30 folder from the libraries folder (It 
is a couple levels up in this same folder) and put it into the one of 
your Arduino libraries folders. These can be found in the x86 Program
Files. Otherwise, you can find the other libraries option in your 
Arduino file in your documents. You can only put it in one of these two
locations, otherwise it will not compile.

