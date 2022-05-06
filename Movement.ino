void drive(int velocity, int radius)
{
  clamp(velocity, -500, 500); // define max and min velocity in mm/s
  clamp(radius, -2000, 2000); // define max and min radius in mm

  Roomba.write(137);           // Drive Op code
  Roomba.write(velocity >> 8); // Velocity MSB
  Roomba.write(velocity);      //
  Roomba.write(radius >> 8);
  Roomba.write(radius);
}

void driveStraight(int distance_tmp, int velocity)
{
  drive(velocity, RadStraight);
  if(MovementTesting)
  {
    Serial.print("Going straight for");
  }
  Serial.println(distance_tmp);
  while (distance < distance_tmp)
  { // flaw is that if it goes above in the middle of function, it will drift
    distanceAngleSensors();
    checkBumperSensors();
    if (bumpRight == 1 || bumpLeft == 2)
    {
      checkBumperSensors();
      if(MovementTesting){
        Serial.println("Felt a bump OwO...");
      }
      driveStraight(distance, -velocity);
    }
  }
  // startTime = millis();
  // endTime = startTime;
  //  while((endTime - startTime) <=1000 || (bumpRight != 1 && bumpLeft != 2)) // do this loop for up to 3000mS
  //  {
  //    checkBumperSensors();
  //    Serial.println("Going straight for 1 seconds");
  //    endTime = millis();
  //  }
  if(MovementTesting){
    Serial.print("Current distance: ");
    Serial.println(distance);
    Serial.print("Current angle: ");
    Serial.println(angle); 
  }
  

  driveStop();
  distanceAngleSensors();

  distance = distance - distance_tmp; // resets angle to 0; accounts for an over/undershoot
}

void driveStop()
{
  drive(0, 0);
  distanceAngleSensors();
}

void driveUntilBump()
{
  drive(100, 32768);
  while (bumpRight != 2 && bumpLeft != 1)
  {
    checkBumperSensors();
  }
  drive(0, 0);
  playSong(2);
}

void driveWheels(int right, int left)
{
  clamp(right, -500, 500);
  clamp(left, -500, 500);

  Roomba.write(145);
  Roomba.write(right >> 8);
  Roomba.write(right);
  Roomba.write(left >> 8);
  Roomba.write(left);
}

void move1Square()
{
  driveStraight(cell_size, DEFAULT_SPEED);
}

void moveBack1Square()
{
  driveStraight(cell_size, -DEFAULT_SPEED);
}

void turn90CW()
{
  drive(DEFAULT_SPEED, RadCW);
  if(MovementTesting){
    Serial.println("Turning clockwise 90 degrees");
  }
  while ((-angle) < 90)
  {
    distanceAngleSensors(); // flaw is that if it goes above in the middle of function, it will drift
    //Serial.println(angle);
  }
  if(MovementTesting){
    Serial.println("STAHP");
    Serial.println(angle);
  }
  driveStop();
  distanceAngleSensors();

  angle = angle - 90; // resets angle to 0; accounts for an over/undershoot
  angle = 0;
}

void turn90CCW()
{
  drive(DEFAULT_SPEED, RadCCW);
  if(MovementTesting){
    Serial.println("Turning counter-clockwise 90 degrees");
  }
  while ((angle) < 90)
  {
    distanceAngleSensors(); // flaw is that if it goes above in the middle of function, it will drift
    //Serial.println(angle);
  }
  if(MovementTesting){
    Serial.println("STAHP");
    Serial.println(angle);
  }
  driveStop();
  distanceAngleSensors();

  angle = angle - 90; // resets angle to 0; accounts for an over/undershoot
  angle = 0;
}
