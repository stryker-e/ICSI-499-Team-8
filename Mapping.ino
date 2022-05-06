//void mapRoom(int mapNumber){
//    if (getMap(mapNumber) != null)//this method doesnt exist yet, basically check if the current map if empty
//    {
//        int mapMatrix[MAX_MAP_SIZE][MAX_MAP_SIZE]=
//        {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
//
//    }
//}

void printMaptoSerial()
{
  for (int i = 0; i < Y_SIZE; i++)
  {
    for (int j = 0; j < X_SIZE; j++)
    {
      Serial.print(mapMatrix[i][j]);
      Serial.print(" ");
    }
    Serial.print("\t");
    for (int j = 0; j < X_SIZE; j++)
    {
      Serial.print(boolMap[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void wallFollow()
{
  // drive straight
  driveWheels(100, 100);
  // check for collision
  while (bumpRight != 1 && bumpLeft != 2)
  {
    checkBumperSensors();
    Serial.println("Waiting for bump...");
  }
  driveStop();
  boolean done = false;
  int loopCount = 0;
  // while(!done)
  while (++loopCount < 10)
  {
    Serial.println("3 second delay...");
    delay(3000);
    drive(20, -1);
    while (printSensorReadingBinary(1) || printSensorReadingBinary(2) || printSensorReadingBinary(3) || printSensorReadingBinary(4) || printSensorReadingBinary(5))
    {
      Serial.println("Turning Away from wall...");
    }
    delay(1000);
    driveStop();
    Serial.println("3 second delay...");
    delay(3000);
    driveWheels(100, 100);
    startTime = millis();
    endTime = startTime;
    while ((endTime - startTime) <= 1000 || (bumpRight != 1 && bumpLeft != 2)) // do this loop for up to 3000mS
    {
      checkBumperSensors();
      Serial.println("Going straight for 1 seconds");
      endTime = millis();
    }
    driveStop();
    Serial.println("3 second delay...");
    delay(3000);
    // drive left?
    driveWheels(-50, 100);
    // turn left until collision
    while (bumpRight != 1 && bumpLeft != 2)
    {
      checkBumperSensors();
      Serial.println("Turning Left until bump...");
    }
    driveStop();
    changeMap();
  }
}
