
/*
    Title: AD61600 Robots and Culture
    Author: Fabian Winkler
    Date: 2014
    Code Version: 1.0
    Availability:  http://web.ics.purdue.edu/~fwinkler/AD61600_S14/helloCliffSensor.zip (This has more than what we used)
*/
void checkBumperSensors()
{
  bumpRight = '0';
  bumpLeft = '0';
  char sensorbytes[10]; // variable to hold the returned 10 bytes from iRobot Create

  Roomba.write((byte)142); // get sensor packets
  Roomba.write((byte)1);   // sensor group packet ID 1, size 10 bytes, contains packets: 7-16
  delay(update_delay);
  
  // wipe old sensor data
  char i = 0;
  while (i < 10)
  {
    sensorbytes[i++] = 0;
  }
  i = 0;
  while (Roomba.available())
  {
    sensorbytes[i++] = Roomba.read();
  }

  bumpRight = sensorbytes[0] & 0x01;
  // if right bumper is triggered sensorbytes[0] is: 00000001 bitwise AND with 0x01, i.e. 00000001 equals 1 see: http://arduino.cc/en/Reference/BitwiseAnd
  bumpLeft = sensorbytes[0] & 0x02;
  // if left bumper is triggered sensorbytes[0] is: 00000010
  // bitwise AND with 0x02, i.e. 00000010 equals 2

  // So if the right bumper is triggered bumpRight is 1
  // (if not triggered then 0)
  // if the left bumper is triggered bumpLeft is 2
  // (if not triggered then 0)
  if(SensorTesting){
    Serial.print("Checking bumpers: ");
    Serial.print(bumpRight);
    Serial.print(" ");
    Serial.println(bumpLeft);
  }
}

//https://robotics.stackexchange.com/questions/7229/irobot-create-2-encoder-counts?rq=1
bool getEncoders(int left_encode, int right_encode) {
  byte msb = 0;
  byte lsb = 0;
  int sensorbytes[4];
  Roomba.write(byte(142));
  Roomba.write(byte(2));
  Roomba.write(byte(43));
  Roomba.write(byte(44));
  delay(100);
  int i = 0;
  while(Roomba.available()) {
    sensorbytes[i++] = Roomba.read();
  }
  right_encode = (int)(sensorbytes[2] << 8) | (int)(sensorbytes[3] & 0xFF);
  left_encode = (int)(sensorbytes[0] << 8) | (int)(sensorbytes[1] & 0xFF);
  Serial.print("Left encoder: ");
  Serial.println(left_encode);
  Serial.println("Right encoder: ");
  Serial.println(right_encode);
  return true;
}

//https://github.com/5ide6urnslab/iRobotCreate2
bool getPacketData(byte packetID, int buff) {
  byte msb = 0;
  byte lsb = 0;

  Roomba.write(byte(142));
  Roomba.write(byte(packetID));

  if(Roomba.available() > 0) {
    msb = Roomba.read();
    lsb = Roomba.read();
    buff = msb << 8 | lsb;
    Serial.print("Packet data = " );
    Serial.println(buff);
    return true;
  }
  return false;  
}

void printSensorReading()
{
  int lightBumpRight = 0;
  char sensorbytes[10]; // variable to hold the returned 10 bytes from iRobot Create

  Roomba.write((byte)142); // get sensor packets
  Roomba.write((byte)106); // sensor group packet ID 1, size 10 bytes, contains packets: 46-51
  delay(update_delay);

  // wipe old sensor data
  char i = 0;
  while (i < 10)
  {
    sensorbytes[i++] = 0;
  }
  i = 0;
  while (Roomba.available())
  {
    sensorbytes[i++] = Roomba.read();
  }

  lightBumpRight = sensorbytes[5];
  // Serial.println(lightBumpRight);
}

void printSensorReadingBinaryTest()
{
  // Packet 45: LtRightBumper
  byte sensor = 0;
  byte sensorbytes[28]; // variable to hold the returned 10 bytes
  // from iRobot Create

  Roomba.write((byte)142); // get sensor packets
  Roomba.write((byte)101); // sensor group packet ID 101, size
  // bytes, contains packets: 43 - 58
  delay(64);
  // wipe old sensor data
  char i = 0;
  while (i < 28)
  {
    sensorbytes[i++] = 0;
  }
  i = 0;
  while (Roomba.available())
  {
    sensorbytes[i++] = Roomba.read();
  }

  switch (currentMapNumber)
  {
  case 1:
    sensor = sensorbytes[4] & 0x01;
    break;
  case 2:
    sensor = sensorbytes[4] & 0x02;
    break;
  case 3:
    sensor = sensorbytes[4] & 0x04;
    break;
  case 4:
    sensor = sensorbytes[4] & 0x08;
    break;
  case 5:
    sensor = sensorbytes[4] & 0x16;
    break;
  case 6:
    sensor = sensorbytes[4] & 0x32;
    break;
  }

  Serial.println(sensor > 0);
}

boolean printSensorReadingBinary(int bit)
{
  byte sensor = 0;
  byte sensorbytes[28]; // variable to hold the returned 28 bytes
  // from iRobot Create

  Roomba.write((byte)142); // get sensor packets
  Roomba.write((byte)101); // sensor group packet ID 101, size
  // bytes, contains packets: 43 - 58
  delay(update_delay);
  // wipe old sensor data
  char i = 0;
  while (i < 28)
  {
    sensorbytes[i++] = 0;
  }
  i = 0;
  while (Roomba.available())
  {
    sensorbytes[i++] = Roomba.read();
  }
  // 1 - Light Bumper Left
  // 2 - Light Bumper Front Left
  // 3 - Light Bumper Center Left
  // 4 - Light Bumper Center Right
  // 5 - Light Bumper Front Right
  // 6 - Light Bumper Right
  switch (bit)
  {
  case 1:
    sensor = sensorbytes[4] & 0x01;
    break;
  case 2:
    sensor = sensorbytes[4] & 0x02;
    break;
  case 3:
    sensor = sensorbytes[4] & 0x04;
    break;
  case 4:
    sensor = sensorbytes[4] & 0x08;
    break;
  case 5:
    sensor = sensorbytes[4] & 0x10;
    break;
  case 6:
    sensor = sensorbytes[4] & 0x20;
    break;
  }

  return sensor > 0;
}

void distanceAngleSensors()
{
  byte sensor = 0;
  byte sensorbytes[6]; // variable to hold the returned 10 bytes
  // from iRobot Create

  Roomba.write((byte)142); // get sensor packets
  Roomba.write((byte)2);   // sensor group packet ID 101, size
  // bytes, contains packets: 43 - 58
  delay(update_delay);
  // wipe old sensor data
  char i = 0;
  while (i < 6)
  {
    sensorbytes[i++] = 0;
  }
  i = 0;
  while (Roomba.available())
  {
    sensorbytes[i++] = Roomba.read();
  }

  distance += (int)((sensorbytes[2] << 8) | sensorbytes[3]);
  angle += (int)((sensorbytes[4] << 8) | sensorbytes[5]);

//    Serial.print("distance ");
//    Serial.println(distance);
//  
//    Serial.print("angle ");
//    Serial.println(angle);
}
