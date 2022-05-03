#include "Roomba_Defines_Prototype.h"
#include <SoftwareSerial.h>
#include <cppQueue.h>
//#include <SPI.h>
//#include <SD.h>

#define A A4 // A to segment pin A4
#define B A5
#define C 4
#define D 3
#define E 2
#define F A3
#define G 6

byte rxPin = 8; // rxPin: the pin on which to receive serial data

byte txPin = 9; // txPin: the pin on which to transmit serial data

// Create an instance of a SoftwareSerial object with the rx pin and tx pin as parameters.
SoftwareSerial Roomba(rxPin, txPin);

byte currentMapNumber = 1; // Currently selected map number
const byte maxMaps = 6;    // Maximum number of maps
int buttonState = 0;       // Button state
int lastButtonState = 1;   // Previous button state (used to prevent multiple function calls per button press)

byte bumpRight = '0'; // Used to check right bumper sensor
byte bumpLeft = '0';  // Used to check left bumper sensor

int startTime = 0; // used for timing
int endTime = 0;   // used for timing

int distance = 0;
int angle = 0;
int update_delay = 64;
// uint16_t timer_cnt = 0;
// uint8_t timer_on = 0;

int cell_size = 330; // the size of the robot
// int scan_one_cell=39;//lower number makes it see further

// map locations
byte x = 0;
byte y = 0;

// temp variables
byte counter = 0;

// when searching for a node with a lower value
byte minimum_node = 250;
byte min_node_location = 0;
byte reset_min = 250; // anything above this number is a special item, ie a wall or robot

struct Cell
{ // Declare Cell struct
  int x;
  int y;
};

// Variable for orientation of robot, default value is north
int orientation = NORTH;

// Variable to contain the directions to move to
int direction[MAP_AREA];
// Next best neighbor
Cell next;
// Neighbors of a cell
Cell northNeigh;
Cell southNeigh;
Cell eastNeigh;
Cell westNeigh;

// Values of neighbors
int northVal;
int southVal;
int eastVal;
int westVal;

Cell nextCell;

cppQueue q(sizeof(Cell), MAP_AREA, FIFO);

// //X is vertical, Y is horizontal
// int mapMatrix[X_SIZE][Y_SIZE]=
//         {{1,1,1,1,1,1},
//          {1,1,1,1,1,1},
//          {1,0,0,0,1,1},
//          {1,0,0,0,1,1},
//          {1,0,0,0,1,1},
//          {1,1,1,1,1,1}};
// int start_x = 1;
// int start_y = 4;
// int goal_x = 3;
// int goal_y = 2;
//int mapMatrixTest[20][20];

 int mapMatrix[X_SIZE][Y_SIZE]=
         {{WALL,WALL,START,0,WALL,WALL},
          {WALL,WALL,0,0,WALL,WALL},
          {WALL,WALL,0,0,WALL,WALL},
          {WALL,WALL,0,0,WALL,WALL},
          {WALL,WALL,0,0,WALL,WALL},
          {WALL,WALL,0,0,WALL,WALL}};
 int start_x = 2;
 int start_y = 0;
 int goal_x = 0;
 int goal_y = 0;

 bool boolMap[X_SIZE][Y_SIZE] =
         {{0,0,0,0,0,0},
          {0,0,0,0,0,0},
          {0,0,0,0,0,0},
          {0,0,0,0,0,0},
          {0,0,0,0,0,0},
          {0,0,0,0,0,0}};

//int mapMatrix[X_SIZE][Y_SIZE] =
//    {{-1, 0, 0},
//     {0, 0, 0},
//     {0, 0, 0}};
//int start_x = 0;
//int start_y = 0;
//int goal_x = 0;
//int goal_y = 0;
//
//bool boolMap[X_SIZE][Y_SIZE] =
//    {{0, 0, 0},
//     {0, 0, 0},
//     {0, 0, 0}};

void setup()
{

  // Sets the speed (baud rate) for the serial communication.
  Roomba.begin(19200);

  // Sets the data rate in bits per second (baud) for serial data transmission.
  Serial.begin(19200);

  delay(2000); // delay to let Roomba initialize

  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  pinMode(ddPin, OUTPUT); // configures the devide detect pin into output mode.

  wakeUp(); // Pulses the BRC pin in the Roomba to wake it up

  startSafe(); // Starts the Roomba and puts it in safe mode

  Serial.println("Robo Jumbo: Ready to Roll"); // Print to serial monitor

  turnOff(); // reset seven segment

  displayDigit(currentMapNumber); // display 1 on seven segment
}

void loop()
{

  buttonState = digitalRead(buttonPin); // read the state of the pushbutton value:

  // Check if the buttonState has changed
  if (buttonState != lastButtonState)
  {
    // Check if the button is being pressed. If it is we know it went from off to on
    if (buttonState == HIGH)
    {

      // Change number being displayed by seven segment
      changeMap();

      // map();
      generateCells();
      delay(2000);
      computePath(X_SIZE, Y_SIZE, start_x, start_y);
    }
    else
    {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }

    delay(10); // Delay a little bit to avoid bouncing
  }

  // printSensorReadingBinaryTest();
  // checkBumperSensors();

  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;
}

void changeMap()
{
  currentMapNumber = (currentMapNumber++ % maxMaps) + 1;
  turnOff(); // reset seven segment
  displayDigit(currentMapNumber);
  Serial.print(currentMapNumber); // Print to serial monitor
}

void wakeUp(void)
{
  // Pulses the BRC (Baud Rate Charge) pin low 3 times to wake up the Roomba
  digitalWrite(ddPin, HIGH);
  delay(100);
  digitalWrite(ddPin, LOW);
  delay(500);
  digitalWrite(ddPin, HIGH);
  delay(100);
  digitalWrite(ddPin, LOW);
  delay(500);
  digitalWrite(ddPin, HIGH);
  delay(100);
  digitalWrite(ddPin, LOW);
  delay(500);
  digitalWrite(ddPin, HIGH);
  delay(2000);
}

void startSafe()
{
  Roomba.write(128); // Starts the OI (must be sent before any other command)
  Roomba.write(131); // Puts the OI into Safe Mode. Turns off all LEDs
  delay(1000);
}

void startFull()
{
  Roomba.write(128); // Starts the OI (must be sent before any other command)
  Roomba.write(132); // Puts the OI into Safe Mode (Turns off all LEDs)
  delay(1000);
}

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
  char sensorbytes[10]; // variable to hold the returned 10 bytes
  // from iRobot Create

  Roomba.write((byte)142); // get sensor packets
  Roomba.write((byte)1);   // sensor group packet ID 1, size 10
  // bytes, contains packets: 7-16
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
    int c = Roomba.read();
    sensorbytes[i++] = c;
  }

  bumpRight = sensorbytes[0] & 0x01;
  // if right bumper is triggered sensorbytes[0] is: 00000001
  // bitwise AND with 0x01, i.e. 00000001 equals 1
  // see: http://arduino.cc/en/Reference/BitwiseAnd
  bumpLeft = sensorbytes[0] & 0x02;
  // if left bumper is triggered sensorbytes[0] is: 00000010
  // bitwise AND with 0x02, i.e. 00000010 equals 2

  // So if the right bumper is triggered bumpRight is 1
  // (if not triggered then 0)
  // if the left bumper is triggered bumpLeft is 2
  // (if not triggered then 0)
  // Serial.print("Checking bumpers: ");
  // Serial.print(bumpRight);
  // Serial.print(" ");
  // Serial.println(bumpLeft);
}

void printSensorReading()
{
  int lightBumpRight = 0;
  char sensorbytes[10]; // variable to hold the returned 10 bytes
  // from iRobot Create

  Roomba.write((byte)142); // get sensor packets
  Roomba.write((byte)106); // sensor group packet ID 1, size 10
  // bytes, contains packets: 46-51
  delay(64);
  // wipe old sensor data
  char i = 0;
  while (i < 10)
  {
    sensorbytes[i++] = 0;
  }
  i = 0;
  while (Roomba.available())
  {
    int c = Roomba.read();
    sensorbytes[i++] = c;
  }

  lightBumpRight = sensorbytes[5];
  // if right bumper is triggered sensorbytes[0] is: 00000001
  // bitwise AND with 0x01, i.e. 00000001 equals 1
  // see: http://arduino.cc/en/Reference/BitwiseAnd
  // bumpLeft = sensorbytes[5] & 0x02;
  // if left bumper is triggered sensorbytes[0] is: 00000010
  // bitwise AND with 0x02, i.e. 00000010 equals 2

  // So if the right bumper is triggered bumpRight is 1
  // (if not triggered then 0)
  // if the left bumper is triggered bumpLeft is 2
  // (if not triggered then 0)

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
    int c = Roomba.read();
    sensorbytes[i++] = c;
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
    int c = Roomba.read();
    sensorbytes[i++] = c;
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

  // Get byte 4

  // if right bumper is triggered sensorbytes[0] is: 00000001
  // bitwise AND with 0x01, i.e. 00000001 equals 1
  // see: http://arduino.cc/en/Reference/BitwiseAnd
  // bumpLeft = sensorbytes[5] & 0x02;
  // if left bumper is triggered sensorbytes[0] is: 00000010
  // bitwise AND with 0x02, i.e. 00000010 equals 2

  // So if the right bumper is triggered bumpRight is 1
  // (if not triggered then 0)
  // if the left bumper is triggered bumpLeft is 2
  // (if not triggered then 0)

  // Serial.println(sensor > 0);
  return sensor > 0;
}

void distanceAngleSensors()
{
  // Packet 45: LtRightBumper
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
    int c = Roomba.read();
    sensorbytes[i++] = c;
  }

  distance += (int)((sensorbytes[2] << 8) | sensorbytes[3]);
  angle += (int)((sensorbytes[4] << 8) | sensorbytes[5]);

    Serial.print("distance ");
    Serial.println(distance);
  
    Serial.print("angle ");
    Serial.println(angle);
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

void displayDigit(int digit)
{
  // Conditions for displaying segment A
  if (digit != 1 && digit != 4)
    digitalWrite(A, HIGH);

  // Conditions for displaying segment B
  if (digit != 5 && digit != 6)
    digitalWrite(B, HIGH);

  // Conditions for displaying segment C
  if (digit != 2)
    digitalWrite(C, HIGH);

  // Conditions for displaying segment D
  if (digit != 1 && digit != 4 && digit != 7)
    digitalWrite(D, HIGH);

  // Conditions for displaying segment E
  if (digit == 2 || digit == 6 || digit == 8 || digit == 0)
    digitalWrite(E, HIGH);

  // Conditions for displaying segment F
  if (digit != 1 && digit != 2 && digit != 3 && digit != 7)
    digitalWrite(F, HIGH);
  if (digit != 0 && digit != 1 && digit != 7)
    digitalWrite(G, HIGH);
}

void turnOff()
{
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, LOW);
}

void playSong(int songNumber)
{
  switch (songNumber)
  {
  case 1:
    // New song: Serial sequence: [140] [Song Number] [Song Length] [Note Number 1] [Note Duration 1]
    //[Note Number 2] [Note Duration 2], etc.
    // Song opcode: [140] Song number: 1 [01] Song length: 12 [0c] (110 BPM)
    // Note 1: C 60 [3c] Note 1 Duration: 1/4 36 [09]
    // Note 2: D# 63 [3f] Note 2 Duration: 1/4 36 [09]
    // Note 3: F 65 [41] Note 3 Duration: Dotted 1/8 24 [06]
    // Note 4: D# 63 [3f] Note 4 Duration: 1/16 + 1/8 32 [08]
    // Note 5: F 65 [41] Note 5 Duration: 1/8 16 [04]
    // Note 6: F 65 [41] Note 6 Duration: 1/8 16 [04]
    // Note 7: F 65 [41] Note 7 Duration: 1/8 16 [04]
    // Note 8: A# 70 [46] Note 8 Duration: 1/8 16 [04]
    // Note 9: Ab 68 [44] Note 9 Duration: 1/8 16 [04]
    // Note 10: G 67 [43] Note 10 Duration: 1/16 8 [02]
    // Note 11: F 65 [41] Note 11 Duration: 1/8 16 [04]
    // Note 12: G 67 [43] Note 12 Duration: 1/16 + 1/4 44 [0b]
    Roomba.write("\x8c\x01\x0c\x3c\x24\x3f\x20\x41\x18\x3f\x20\x41\x10\x41\x10\x41\x10\x46\x10\x44\x10\x43\x08\x41\x10\x43\x2c");
    Roomba.write("\x8d\x01");

    break;

  case 2:
    // New song: Serial sequence: [140] [Song Number] [Song Length] [Note Number 1] [Note Duration 1]
    //[Note Number 2] [Note Duration 2], etc.
    // Song opcode: [140] Song number: 1 [01] Song length: 11 [0b] (110 BPM)
    // Note 1: G 67 [43] Note 1 Duration: 1/4 36 [24]
    // Note 2: A# 70 [46] Note 2 Duration: 1/4 36 [24]
    // Note 3: C 72 [48] Note 3 Duration: Dotted 1/8 24 [18]
    // Note 4: F 65 [41] Note 4 Duration: 1/16 + 1/8 32 [20]
    // Note 5: D# 63 [3f] Note 5 Duration: 1/8 16 [10]
    // Note 6: A# 70 [46] Note 6 Duration: 1/8 16 [10]
    // Note 7: A# 70 [46] Note 7 Duration: 1/8 16 [10]
    // Note 8: G 67 [43] Note 8 Duration: 1/8 16 [10]
    // Note 9: A# 70 [46] Note 9 Duration: 1/8 16 [10]
    // Note 10: A# 70 [46] Note 10 Duration: Dotted 1/8 24 [18]
    // Note 11: C 72 [48] Note 11 Duration: 1/16 + 1/4 42 [2A]
    Roomba.write("\x8c\x02\x0b\x43\x24\x46\x24\x48\x18\x41\x20\x3f\x10\x46\x10\x46\x10\x43\x10\x46\x10\x46\x18\x48\x2A");
    Roomba.write("\x8d\x02");

    break;
  }
}

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
  Serial.print("Going straight for");
  Serial.println(distance_tmp);
  while (distance < distance_tmp)
  { // flaw is that if it goes above in the middle of function, it will drift
    distanceAngleSensors();
    checkBumperSensors();
    if (bumpRight == 1 || bumpLeft == 2)
    {
      checkBumperSensors();
      Serial.println("Felt a buwmp OwO...");
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

  Serial.print("distance ");
  Serial.println(distance);

  Serial.print("angle ");
  Serial.println(angle);

  driveStop();
  distanceAngleSensors();

  distance = distance - distance_tmp; // resets angle to 0; accounts for an over/undershoot
}

void driveStop()
{
  drive(0, 0);
  distanceAngleSensors();
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

void driveLeft(int left)
{
  driveWheels(left, 0);
}

void driveRight(int right)
{
  driveWheels(0, right);
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
  Serial.println("Turning clockwise 90 degrees");
  while ((-angle) < 90)
  {
    distanceAngleSensors(); // flaw is that if it goes above in the middle of function, it will drift
    //Serial.println(angle);
  }
  Serial.println("STAHP");
  Serial.println(angle);
  driveStop();
  distanceAngleSensors();

  angle = angle - 90; // resets angle to 0; accounts for an over/undershoot
  angle = 0;
}

void turn90CCW()
{
  drive(DEFAULT_SPEED, RadCCW);
  Serial.println("Turning counter-clockwise 90 degrees");
  while ((angle) < 90)
  {
    distanceAngleSensors(); // flaw is that if it goes above in the middle of function, it will drift
    //Serial.println(angle);
  }
  Serial.println("STAHP");
  Serial.println(angle);
  driveStop();
  distanceAngleSensors();

  angle = angle - 90; // resets angle to 0; accounts for an over/undershoot
  angle = 0;
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

/************************ COMPUTE NEIGHBOR *************************/
void computeNeighbor(Cell &currCell)
{
  northNeigh.x = currCell.x;
  northNeigh.y = currCell.y - 1;
  // If coordinates of north neighbor are valid, northVal is the value of north neighbor, else it's invalid
  checkCoordinate(northNeigh.x, northNeigh.y) == true ? northVal = mapMatrix[northNeigh.y][northNeigh.x] : northVal = -1;
  // Serial.println("north neighbor is: %d,%d |Value is: %d",northNeigh.x,northNeigh.y,northVal);

  southNeigh.x = currCell.x;
  southNeigh.y = currCell.y + 1;
  checkCoordinate(southNeigh.x, southNeigh.y) == true ? southVal = mapMatrix[southNeigh.y][southNeigh.x] : southVal = -1;
  // Serial.println("south neighbor is: %d,%d |Value is: %d",southNeigh.x,southNeigh.y,southVal);

  eastNeigh.x = currCell.x + 1;
  eastNeigh.y = currCell.y;
  checkCoordinate(eastNeigh.x, eastNeigh.y) == true ? eastVal = mapMatrix[eastNeigh.y][eastNeigh.x] : eastVal = -1;
  // Serial.println("east neighbor is: %d,%d |Value is: %d",eastNeigh.x,eastNeigh.y,eastVal);

  westNeigh.x = currCell.x - 1;
  westNeigh.y = currCell.y;
  checkCoordinate(westNeigh.x, westNeigh.y) == true ? westVal = mapMatrix[westNeigh.y][westNeigh.x] : westVal = -1;
  // Serial.println("west neighbor is: %d,%d |Value is: %d",westNeigh.x,westNeigh.y,westVal);
}
/************************ END COMPUTE NEIGHBOR ***********************************/

void setGoal()
{
  counter = 0;
  for (int i = 0; i < Y_SIZE; i++)
  {
    for (int j = 0; j < X_SIZE; j++)
    {
      if (counter < mapMatrix[i][j])
      {
        counter = mapMatrix[i][j];
        goal_x = j;
        goal_y = i;
      }
    }
  }

  Serial.print("The goal is set to ");
  Serial.print(goal_x);
  Serial.print(" ");
  Serial.println(goal_y);
}

/******************** COMPUTE PATH ************************************/
void computePath(int numCols, int numRows, int startX, int startY)
{
  // Initialize start positions
  Cell currCell;
  currCell.x = startX;
  currCell.y = startY;

  int smallestCell = 0;

  bool pathFound;

  // compare all neighbors to find the smallest which is not OBST

  int i = 0;
  while (smallestCell != mapMatrix[goal_y][goal_x])
  {
    pathFound = false;
    Serial.print(i);
    Serial.print(" ");
    Serial.println(sizeof(direction));
    if (i > sizeof(direction))
    {
      // Create new array with bigger size
      int newDirections[sizeof(direction) * 2];
      for (int i = 0; i < sizeof(direction); i++)
      {
        newDirections[i] = direction[i];
      }
      memcpy(direction, newDirections, sizeof(direction));
    }
    computeNeighbor(currCell); // Compute and set all possible neighbors
    // Check to make sure the neighbors are valid
    // Assign arbitrary smallest cell for comparison
    //smallestCell = mapMatrix[currCell.y][currCell.x];
    Serial.print("temp smallestCell value: ");
    Serial.println(smallestCell);

    while (!pathFound)
    {

      if (northVal < smallestCell && (northVal != WALL && northVal != -1) && !boolMap[currCell.x][currCell.y - 1])
      {
        smallestCell = northVal;
        Serial.println(northVal);
        // found best cell...set current cell to that cexl
        currCell.x = northNeigh.x;
        currCell.y = northNeigh.y;
        direction[i] = 1;
        pathFound = true;
      }
      if (southVal < smallestCell && (southVal != WALL && southVal != -1) && !boolMap[currCell.x][currCell.y + 1])
        {
          smallestCell = southVal;
          Serial.println(southVal);
          // found best cell...set current cell to that cell
          currCell.x = southNeigh.x;
          currCell.y = southNeigh.y;
          direction[i] = 2;
          pathFound = true;
        }
      if (eastVal < smallestCell && (eastVal != WALL && eastVal != -1) && !boolMap[currCell.x + 1][currCell.y])
        {
          smallestCell = eastVal;
          Serial.println(eastVal);
          // found best cell...set current cell to that cell
          currCell.x = eastNeigh.x;
          currCell.y = eastNeigh.y;
          direction[i] = 3;
          pathFound = true;
        }
      if (westVal < smallestCell && (westVal != WALL && westVal != -1) && !boolMap[currCell.x - 1][currCell.y])
          {
            smallestCell = westVal;
            Serial.println(westVal);
            // found best cell...set current cell to thay cell
            currCell.x = westNeigh.x;
            currCell.y = westNeigh.y;
            direction[i] = 4;
            pathFound = true;
          }
       if (!pathFound){
         smallestCell++;
         Serial.println(smallestCell);
       }
    }
    // Print
    // Serial.println("Moving to cell: %d,%d | Value: %d",currCell.x,currCell.y, mapMatrix[currCell.y][currCell.x]);
    Serial.print("Moving to cell: ");
    Serial.print(currCell.x);
    Serial.print(" ");
    Serial.println(currCell.y);
    Serial.print("The current smallest cell is: ");
    Serial.print(currCell.x);
    Serial.print(" ");
    Serial.println(currCell.y);
    // mark current cell as visited in boolean matrix map
    boolMap[currCell.x][currCell.y] = 1;
    followPlan(i);
    i++;
  }
  verifyCoverage();
  for (int j = 0; j < sizeof(direction); j++)
  {
    Serial.print(direction[j]);
    Serial.print(" ");
  }
}
/******************************** END COMPUTE PATH *********************************/

/******************** CHECK COORDINATES *************************************/
bool checkCoordinate(int x, int y)
{
  bool returnValue;
  int dummy = 0;
  int dummy2 = 0;

  x == -1 ? returnValue = false : dummy += 1; // If x is invalid, return value is false and increment dummy by 1
  y == -1 ? returnValue = false : dummy += 2; // If y is invalid return value is false and increment dummy by `

  x > (X_SIZE - 1) ? returnValue = false : dummy2 += 3; // If x exceeds boundary return value is false, increment dummy2 by 3
  y > (Y_SIZE - 1) ? returnValue = false : dummy2 += 4; // If y exceeds boundary return value is false, increment dummy2 by 4

  // If dummy is 3 and dumm2 is 7, then both tests have passed, coordinates are valid
  if (dummy == 3 && dummy2 == 7)
  {
    //    Serial.print(x);
    //    Serial.print(" ");
    //    Serial.println(y);
    //    Serial.println("True");
    return true;
  }
  else // Otherwise tests failed, return false
  {
    //    Serial.print(x);
    //    Serial.print(" ");
    //    Serial.println(y);
    //    Serial.println("False");
    return returnValue;
  }
}
/*********************** END CHECK COORDINATES ******************************/

/*********************** FOLLOW PLAN *************************************************/
void followPlan(int i)
{
  //********************   Direction to move to: NORTH    ***************//
  // Orientation: NORTH
  if (direction[i] == 1 && orientation == NORTH)
  {
    move1Square();
    Serial.println("Orientation is still NORTH");
  }
  // Orientation: SOUTH
  else if (direction[i] == 1 && orientation == SOUTH)
  {
    turn90CW();
    turn90CW();
    move1Square();
    orientation = NORTH; // Reset Orientation
    Serial.println("Orientation is NORTH");
  }
  // Orientation: EAST
  else if (direction[i] == 1 && orientation == EAST)
  {
    turn90CCW();
    move1Square();
    orientation = NORTH; // Reset Orientation
    Serial.println("Orientation is NORTH");
  }
  // Orientation: WEST
  else if (direction[i] == 1 && orientation == WEST)
  {
    turn90CW();
    move1Square();
    orientation = NORTH; // Reset orientation
    Serial.println("Orientation is NORTH");
  }

  //********************  Direction to move to: SOUTH    ***************//
  // Orientation: SOUTH
  if (direction[i] == 2 && orientation == SOUTH)
  {
    move1Square();
    Serial.println("Orientation is still SOUTH");
  }
  // Orientation: NORTH
  else if (direction[i] == 2 && orientation == NORTH)
  {
    turn90CW();
    turn90CW();
    move1Square();
    orientation = SOUTH; // Reset Orientation
    Serial.println("Orientation is SOUTH");
  }
  // Orientation: EAST
  else if (direction[i] == 2 && orientation == EAST)
  {
    turn90CW();
    move1Square();
    orientation = SOUTH; // Reset orientation
    Serial.println("Orientation is SOUTH");
  }
  // Orientation: WEST
  else if (direction[i] == 2 && orientation == WEST)
  {
    turn90CCW();
    move1Square();
    orientation = SOUTH; // Reset orientation
    Serial.println("Orientation is SOUTH");
  }

  //******************** Direction to move to: EAST     ***************//
  // Orientation: EAST
  if (direction[i] == 3 && orientation == EAST)
  {
    move1Square();
    Serial.println("Orientation is still EAST");
  }
  // Orientation: NORTH
  else if (direction[i] == 3 && orientation == NORTH)
  {
    turn90CW();
    move1Square();
    orientation = EAST; // Reset Orientation
    Serial.println("Orientation is EAST");
  }
  // ORIENTATION: SOUTH
  else if (direction[i] == 3 && orientation == SOUTH)
  {
    turn90CCW();
    move1Square();
    orientation = EAST; // Reset orientation
    Serial.println("Orientation is EAST");
  }
  // ORIENTATION: WEST
  else if (direction[i] == 3 && orientation == WEST)
  {
    turn90CW();
    turn90CW();
    move1Square();
    orientation = EAST; // Reset Orientation
    Serial.println("Orientation is EAST");
  }

  //********************  Direction to move to: WEST     ***************//
  // Orientation: WEST
  if (direction[i] == 4 && orientation == WEST)
  {
    move1Square();
    Serial.println("Orientation is still WEST");
  }
  // Orientation: NORTH
  else if (direction[i] == 4 && orientation == NORTH)
  {
    turn90CCW();
    move1Square();
    orientation = WEST; // Reset Orientation
    Serial.println("Orientation is WEST");
  }
  // Orientation: WEST
  else if (direction[i] == 4 && orientation == SOUTH)
  {
    turn90CW();
    move1Square();
    orientation = WEST; // Reset Orientation
    Serial.println("Orientation is WEST");
  }
  // Orientation: EAST
  else if (direction[i] == 4 && orientation == EAST)
  {
    turn90CCW();
    turn90CCW();
    move1Square();
    orientation = WEST; // Reset Orientation
    Serial.println("Orientation is WEST");
  }
}

//If the roomba missed some cells while on the way to the goal
//Find the cell with the lowest # and set it as the new goal
//Then call computePath with the previous goal as the start point
void verifyCoverage(){
   int smallestValue = mapMatrix[goal_y][goal_x];
   Cell temp;
   Cell prevGoal;
   for (int i = 0; i < Y_SIZE; i++)
    {
    for (int j = 0; j < X_SIZE; j++)
    {
      if(mapMatrix[i][j] > 0 && boolMap[i][j] != 0){
        if(mapMatrix[i][j] < smallestValue){
          smallestValue = mapMatrix[i][j];
          temp.x = j;
          temp.y = j;
        }
      }
    }
  }
   if(smallestValue != mapMatrix[goal_y][goal_x])
    {
      prevGoal.x = goal_x;
      prevGoal.y = goal_y;
      goal_x = temp.x;
      goal_y = temp.y;
    }
    computePath(X_SIZE, Y_SIZE, prevGoal.x, prevGoal.y);
}

void generateCells()
{
  Cell cell = {start_x, start_y};
  q.push(&cell);

  while (!q.isEmpty())
  {
    q.peek(&nextCell);

    Cell temp;
    temp.x = nextCell.x;
    temp.y = nextCell.y;

    computeNeighbor(temp);
    // display();
    // End
    q.peek(&temp);
    int y = temp.y;
    int x = temp.x;

    if (northVal == 0)
    {
      mapMatrix[northNeigh.y][northNeigh.x] = mapMatrix[y][x] + 1;
      temp.x = northNeigh.x;
      temp.y = northNeigh.y;
      //      Serial.print(temp.x);
      //      Serial.print(" ");
      //      Serial.print(temp.y);
      //      Serial.print(" ");
      //      Serial.println(counter++);
      q.push(&temp);
      // display();
    }
    if (southVal == 0)
    {
      mapMatrix[southNeigh.y][southNeigh.x] = mapMatrix[y][x] + 1;
      temp.x = southNeigh.x;
      temp.y = southNeigh.y;
      //      Serial.print(temp.x);
      //      Serial.print(" ");
      //      Serial.print(temp.y);
      //      Serial.print(" ");
      //      Serial.println(counter++);
      q.push(&temp);
      // display();
    }
    if (eastVal == 0)
    {
      mapMatrix[eastNeigh.y][eastNeigh.x] = mapMatrix[y][x] + 1;
      temp.x = eastNeigh.x;
      temp.y = eastNeigh.y;
      //      Serial.print(temp.x);
      //      Serial.print(" ");
      //      Serial.print(temp.y);
      //      Serial.print(" ");
      //      Serial.println(counter++);
      q.push(&temp);
      // display();
    }
    if (westVal == 0)
    {
      mapMatrix[westNeigh.y][westNeigh.x] = mapMatrix[y][x] + 1;
      temp.x = westNeigh.x;
      temp.y = westNeigh.y;
      //      Serial.print(temp.x);
      //      Serial.print(" ");
      //      Serial.print(temp.y);
      //      Serial.print(" ");
      //      Serial.println(counter++);
      q.push(&temp);
      // display();
    }
    q.drop();
  }
  setGoal();
  printMaptoSerial();
}
