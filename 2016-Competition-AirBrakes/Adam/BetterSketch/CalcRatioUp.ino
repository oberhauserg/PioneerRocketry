// -----------------------------------------------------------------------------------
// Any functions calculating upward displacement. 
// This is a subset of the Sensor tab because it directly uses the 9 degree of 
// freedom sensor.
// -----------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// Beginning of MagwickQuaternionUpdate function
//-------------------------------------------------------------------------------------
// Implementation of Sebastian Madgwick's "...efficient orientation filter for... inertial/magnetic sensor arrays"
// (see http://www.x-io.co.uk/category/open-source/ for examples and more details)
// which fuses acceleration, rotation rate, and magnetic moments to produce a quaternion-based estimate of absolute
// device orientation -- which can be converted to yaw, pitch, and roll. Useful for stabilizing quadcopters, etc.
// The performance of the orientation filter is at least as good as conventional Kalman-based filtering algorithms
// but is much less computationally intensive---it can be performed on a 3.3 V Pro Mini operating at 8 MHz!
//9DOF sensor variables

#define GyroMeasError PI * (40.0f / 180.0f)       // gyroscope measurement error in rads/s (shown as 3 deg/s)
#define GyroMeasDrift PI * (0.0f / 180.0f)      // gyroscope measurement drift in rad/s/s (shown as 0.0 deg/s/s)
// There is a tradeoff in the beta parameter between accuracy and response speed.
// In the original Madgwick study, beta of 0.041 (corresponding to GyroMeasError of 2.7 degrees/s) was found to give optimal accuracy.
// However, with this value, the LSM9SD0 response time is about 10 seconds to a stable initial quaternion.
// Subsequent changes also require a longish lag time to a stable output, not fast enough for a quadcopter or robot car!
// By increasing beta (GyroMeasError) by about a factor of fifteen, the response time constant is reduced to ~2 sec
// I haven't noticed any reduction in solution accuracy. This is essentially the I coefficient in a PID control sense; 
// the bigger the feedback coefficient, the faster the solution converges, usually at the expense of accuracy. 
// In any case, this is the free parameter in the Madgwick filtering and fusion scheme.
#define beta sqrt(3.0f / 4.0f) * GyroMeasError   // compute beta
#define zeta sqrt(3.0f / 4.0f) * GyroMeasDrift   // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value
#define Kp 2.0f * 5.0f // these are the free parameters in the Mahony filter and fusion scheme, Kp for proportional feedback, Ki for integral
#define Ki 0.0f



uint8_t MagRate;     // read rate for magnetometer data

float pitch, yaw, roll;
float deltat = 0.0f;        // integration interval for both filter schemes
uint16_t lastUpdate = 0; // used to calculate integration interval
uint16_t now = 0;        // used to calculate integration interval


float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};    // vector to hold quaternion
float eInt[3] = {0.0f, 0.0f, 0.0f};       // vector to hold integral error for Mahony method

        void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
        {
            float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
            float norm;
            float hx, hy, _2bx, _2bz;
            float s1, s2, s3, s4;
            float qDot1, qDot2, qDot3, qDot4;

            // Auxiliary variables to avoid repeated arithmetic
            float _2q1mx;
            float _2q1my;
            float _2q1mz;
            float _2q2mx;
            float _4bx;
            float _4bz;
            float _2q1 = 2.0f * q1;
            float _2q2 = 2.0f * q2;
            float _2q3 = 2.0f * q3;
            float _2q4 = 2.0f * q4;
            float _2q1q3 = 2.0f * q1 * q3;
            float _2q3q4 = 2.0f * q3 * q4;
            float q1q1 = q1 * q1;
            float q1q2 = q1 * q2;
            float q1q3 = q1 * q3;
            float q1q4 = q1 * q4;
            float q2q2 = q2 * q2;
            float q2q3 = q2 * q3;
            float q2q4 = q2 * q4;
            float q3q3 = q3 * q3;
            float q3q4 = q3 * q4;
            float q4q4 = q4 * q4;

            // Normalise accelerometer measurement
            norm = sqrt(ax * ax + ay * ay + az * az);
      #ifdef debug
      Serial1.print("Norm for accelerometer = ");
      Serial1.println(norm);
      delay(1000);
      #endif
            if (norm == 0.0f) return; // handle NaN
            norm = 1.0f/norm;
            ax *= norm;
            ay *= norm;
            az *= norm;

            // Normalise magnetometer measurement
            norm = sqrt(mx * mx + my * my + mz * mz);
     #ifdef debug
     Serial1.print("norm for Mag = ");
     Serial1.println(norm);
     delay(1000);
     #endif
            if (norm == 0.0f) return; // handle NaN
            norm = 1.0f/norm;
            mx *= norm;
            my *= norm;
            mz *= norm;

            // Reference direction of Earth's magnetic field
            _2q1mx = 2.0f * q1 * mx;
            _2q1my = 2.0f * q1 * my;
            _2q1mz = 2.0f * q1 * mz;
            _2q2mx = 2.0f * q2 * mx;
            hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
            hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
            _2bx = sqrt(hx * hx + hy * hy);
            _2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
            _4bx = 2.0f * _2bx;
            _4bz = 2.0f * _2bz;

            // Gradient decent algorithm corrective step
            s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
            s2 = _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
            s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
            s4 = _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
            norm = sqrt(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);    // normalise step magnitude
            norm = 1.0f/norm;
            s1 *= norm;
            s2 *= norm;
            s3 *= norm;
            s4 *= norm;

            // Compute rate of change of quaternion
            qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
            qDot2 = 0.5f * (q1 * gx + q3 * gz - q4 * gy) - beta * s2;
            qDot3 = 0.5f * (q1 * gy - q2 * gz + q4 * gx) - beta * s3;
            qDot4 = 0.5f * (q1 * gz + q2 * gy - q3 * gx) - beta * s4;

            // Integrate to yield quaternion
            q1 += qDot1 * deltat;
            q2 += qDot2 * deltat;
            q3 += qDot3 * deltat;
            q4 += qDot4 * deltat;
            norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
            norm = 1.0f/norm;
            q[0] = q1 * norm;
            q[1] = q2 * norm;
            q[2] = q3 * norm;
            q[3] = q4 * norm;

        }
//-------------------------------------------------------------------------------------
// end of MagwickQuaternionUpdate function
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// Begin of getRatioVertical function
//-------------------------------------------------------------------------------------
float getRatioVertical()
{
  if(mpu.getIntDataReadyStatus() == 1) // wait for data ready status register to update all data registers
  mpu.getMotion9(&a1, &a2, &a3, &g1, &g2, &g3, &m1, &m2, &m3);
  now = micros();
  deltat = ((now - lastUpdate)/1000000.0f); // set integration time by time elapsed since last filter update
  lastUpdate = now;

  // Switching x and z original below
   //MadgwickQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f,  mx,  my, mz);
  MadgwickQuaternionUpdate(az, ay, ax, gz*PI/180.0f, gy*PI/180.0f, gx*PI/180.0f,  mz,  my, mx);
  yaw   = atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]);   
  pitch = -asin(2.0f * (q[1] * q[3] - q[0] * q[2]));
  roll  = atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
  pitch *= 180.0f / PI;
  yaw   *= 180.0f / PI + 1.63; // Declination at Platteville WI is 1 degrees 38 minutes on 2015-12-29
  roll  *= 180.0f / PI;
  // modified so that roll and pitch are at 0 when y-axis is pointing up
  roll = roll - 90.0f;
    
  /*
  Serial1.print("Yaw, Pitch, Roll: ");
  Serial1.print(yaw, 2);
  Serial1.print(", ");
  Serial1.print(pitch, 2);
  Serial1.print(", ");
  Serial1.print(roll, 2);
  */
  float ratio = CaclulateUpwardDisplacmentRatio(pitch, roll);
  return ratio;
}
//-------------------------------------------------------------------------------------
// End of getRatioVertical function
//-------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
// Beginning of CaclulateUpwardDisplacmentRatio function
//-------------------------------------------------------------------------------------
float CaclulateUpwardDisplacmentRatio(float angle1, float angle2)
{
  // u = cos(AngleFromXAxis) * i + cos(AngleFromYAxis) * j + cos(AngleFromZAxis) * k
  // sqrt( 1 - cos^2(AngleFromXAxis) -cos^2(AngleFromYAxis)) = cos(AngleFromZAxis)
  // sqrt( 1 - cos^2(90 - AngleFromXAxis) -cos^2(90 - AngleFromYAxis)) = cos(AngleFromZAxis)
  //                 What Gyro gives us          What gyro gives us

  //#define debugThis

  if(angle1 > 90 || angle2 > 90)
    return 0.0f;
  

  // convert to radians
  angle1 = angle1 * PI / 180.0f;
        #ifdef debugThis
        Serial1.print("angle1 is ");
        Serial1.println(angle1);
        #endif
  angle2 = angle2 * PI / 180.0f;
        #ifdef debugThis
        Serial1.print("angle2 is ");
        Serial1.println(angle2);
        #endif
  float firstValue = cos(PI/2.0f - angle1);
        #ifdef debugThis
        Serial1.print("firstValue is ");
        Serial1.println(firstValue);
        #endif
  float secondValue = cos(PI/2.0f - angle2);
        #ifdef debugThis
        Serial1.print("secondValue is ");
        Serial1.println(secondValue);
        #endif
  float answer = 1 - firstValue*firstValue - secondValue*secondValue;
        #ifdef debugThis
        Serial1.print("answer is ");
        Serial1.println(answer);
        #endif
  answer = sqrt(answer);
        #ifdef debugThis
        Serial1.print("answer is ");
        Serial1.println(answer);
        #endif
  return answer;
}
//-------------------------------------------------------------------------------------
// End of CaclulateUpwardDisplacmentRatio function
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// This method initializes the 9 degrees of freedom board.
// The accelerometer was adjusted to +/-16G. Otherwise, the settings where taken from 
// the example code for this board. (mpu9150)
//-------------------------------------------------------------------------------------
void initializeGyro()
{
  mpu.initialize();
  mpuWorking = mpu.testConnection();
  if(mpuWorking)
    sendMessage("GYRO working!\n");
  else
    sendMessage("GYRO malfunctioning!\n");
  
  // Set up the accelerometer, gyro, and magnetometer for data output

   mpu.setRate(7); // set gyro rate to 8 kHz/(1 * rate) shows 1 kHz, accelerometer ODR is fixed at 1 KHz

   MagRate = 10; // set magnetometer read rate in Hz; 10 to 100 (max) Hz are reasonable values
// Digital low pass filter configuration. 
// It also determines the internal sampling rate used by the device as shown in the table below.
// The accelerometer output rate is fixed at 1kHz. This means that for a Sample
// Rate greater than 1kHz, the same accelerometer sample may be output to the
// FIFO, DMP, and sensor registers more than once.
/*
 *          |   ACCELEROMETER    |           GYROSCOPE
 * DLPF_CFG | Bandwidth | Delay  | Bandwidth | Delay  | Sample Rate
 * ---------+-----------+--------+-----------+--------+-------------
 * 0        | 260Hz     | 0ms    | 256Hz     | 0.98ms | 8kHz
 * 1        | 184Hz     | 2.0ms  | 188Hz     | 1.9ms  | 1kHz
 * 2        | 94Hz      | 3.0ms  | 98Hz      | 2.8ms  | 1kHz
 * 3        | 44Hz      | 4.9ms  | 42Hz      | 4.8ms  | 1kHz
 * 4        | 21Hz      | 8.5ms  | 20Hz      | 8.3ms  | 1kHz
 * 5        | 10Hz      | 13.8ms | 10Hz      | 13.4ms | 1kHz
 * 6        | 5Hz       | 19.0ms | 5Hz       | 18.6ms | 1kHz
 */
  mpu.setDLPFMode(4); // set bandwidth of both gyro and accelerometer to ~20 Hz

  // Full-scale range of the gyro sensors:
  // 0 = +/- 250 degrees/sec, 1 = +/- 500 degrees/sec, 2 = +/- 1000 degrees/sec, 3 = +/- 2000 degrees/sec
  mpu.setFullScaleGyroRange(0); // set gyro range to 250 degrees/sec

  // Full-scale accelerometer range.
  // The full-scale range of the accelerometer: 0 = +/- 2g, 1 = +/- 4g, 2 = +/- 8g, 3 = +/- 16g
  mpu.setFullScaleAccelRange(3); // set accelerometer to 2 g range

  mpu.setIntDataReadyEnabled(true); // enable data ready interrupt
}

