#include "Roomba_Defines_Prototype.h"
#include <SoftwareSerial.h>
//#include <SPI.h>
//#include <SD.h>

#define A A4 //A to segment pin A4
#define B A5
#define C 4
#define D 3
#define E 2
#define F A3
#define G 6

int rxPin=8; //rxPin: the pin on which to receive serial data

int txPin=9; //txPin: the pin on which to transmit serial data

//Create an instance of a SoftwareSerial object with the rx pin and tx pin as parameters.
SoftwareSerial Roomba(rxPin,txPin);

int currentMapNumber = 1; //Currently selected map number
const int maxMaps = 9; //Maximum number of maps
int buttonState = 0; //Button state
int lastButtonState = 1; //Previous button state (used to prevent multiple function calls per button press)

char bumpRight = '0'; //Used to check right bumper sensor
char bumpLeft = '0'; //Used to check left bumper sensor

void setup() {

  //Sets the speed (baud rate) for the serial communication.
  Roomba.begin(19200);

  //Sets the data rate in bits per second (baud) for serial data transmission.
  Serial.begin(19200);

  delay(2000); //delay to let Roomba initialize

  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  
  pinMode(ddPin, OUTPUT); //configures the devide detect pin into output mode. 

  wakeUp(); //Pulses the BRC pin in the Roomba to wake it up

  startSafe(); //Starts the Roomba and puts it in safe mode

  Serial.println("Robo Jumbo: Ready to Roll"); //Print to serial monitor

  turnOff(); //reset seven segment
  
  displayDigit(currentMapNumber); //display 1 on seven segment
  
}

void loop() {

  buttonState = digitalRead(buttonPin);  // read the state of the pushbutton value:

  //Check if the buttonState has changed
  if (buttonState != lastButtonState) 
  {
    //Check if the button is being pressed. If it is we know it went from off to on
    if (buttonState == HIGH) {

      //Change number being displayed by seven segment
      changeMap();

      //playSong(1);

      //have the Roomba drive until the bumper sensor is hit
      //driveUntilBump();
      
    } 
    else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }
    
    delay(10); // Delay a little bit to avoid bouncing
  }
  
  printSensorReadingBinary();
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;

}

void changeMap(){
  int currentMap = (currentMapNumber++ % maxMaps) + 1;
  turnOff(); //reset seven segment
  displayDigit(currentMap);
  Serial.print(currentMap); //Print to serial monitor
}

void wakeUp (void)
{
  //Pulses the BRC (Baud Rate Charge) pin low 3 times to wake up the Roomba
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
  Roomba.write(128);  //Starts the OI (must be sent before any other command)
  Roomba.write(131);  //Puts the OI into Safe Mode. Turns off all LEDs
  delay(1000);
}

void startFull()
{  
  Roomba.write(128);  //Starts the OI (must be sent before any other command)
  Roomba.write(132);  //Puts the OI into Safe Mode (Turns off all LEDs)
  delay(1000);
}

void drive(int velocity, int radius)
{
  clamp(velocity, -500, 500); //define max and min velocity in mm/s
  clamp(radius, -2000, 2000); //define max and min radius in mm
  
  Roomba.write(137); //Drive Op code
  Roomba.write(velocity >> 8); //Velocity MSB
  Roomba.write(velocity); //
  Roomba.write(radius >> 8);
  Roomba.write(radius);
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
  Roomba.write((byte)1); // sensor group packet ID 1, size 10
  // bytes, contains packets: 7-16
  delay(64);
  // wipe old sensor data
  char i = 0;
  while (i < 10) {
    sensorbytes[i++] = 0;
  }
  i = 0;
  while(Roomba.available()) 
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
  
  Serial.print(bumpRight);
  Serial.print(" ");
  Serial.println(bumpLeft);
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
  while (i < 10) {
    sensorbytes[i++] = 0;
  }
  i = 0;
  while(Roomba.available()) 
  {
    int c = Roomba.read();
    sensorbytes[i++] = c;
  }
  
  lightBumpRight = sensorbytes[5];
  // if right bumper is triggered sensorbytes[0] is: 00000001
  // bitwise AND with 0x01, i.e. 00000001 equals 1
  // see: http://arduino.cc/en/Reference/BitwiseAnd 
  //bumpLeft = sensorbytes[5] & 0x02; 
  // if left bumper is triggered sensorbytes[0] is: 00000010
  // bitwise AND with 0x02, i.e. 00000010 equals 2
  
  // So if the right bumper is triggered bumpRight is 1 
  // (if not triggered then 0)
  // if the left bumper is triggered bumpLeft is 2 
  // (if not triggered then 0)
  
  Serial.println(lightBumpRight);
}

void printSensorReadingBinary()
{
  //Packet 45: LtRightBumper
  char sensor = 0;
  char sensorbytes[28]; // variable to hold the returned 10 bytes
  // from iRobot Create
  
  Roomba.write((byte)142); // get sensor packets
  Roomba.write((byte)101); // sensor group packet ID 101, size 
  // bytes, contains packets: 43 - 58
  delay(64);
  // wipe old sensor data
  char i = 0;
  while (i < 28) {
    sensorbytes[i++] = 0;
  }
  i = 0;
  while(Roomba.available()) 
  {
    int c = Roomba.read();
    sensorbytes[i++] = c;
  }

  switch(currentMapNumber) {
   case 1  :
      sensor = sensorbytes[4] & 0x01;
      break;
   case 2  :
      sensor = sensorbytes[4] & 0x02;
      break;
   case 3  :
      sensor = sensorbytes[4] & 0x03;
      break;
   case 4  :
      sensor = sensorbytes[4] & 0x04;
      break;
   case 5  :
      sensor = sensorbytes[4] & 0x05;
      break;
   case 6  :
      sensor = sensorbytes[4] & 0x06;
      break;
  }
  
  //Get byte 4

  
  // if right bumper is triggered sensorbytes[0] is: 00000001
  // bitwise AND with 0x01, i.e. 00000001 equals 1
  // see: http://arduino.cc/en/Reference/BitwiseAnd 
  //bumpLeft = sensorbytes[5] & 0x02; 
  // if left bumper is triggered sensorbytes[0] is: 00000010
  // bitwise AND with 0x02, i.e. 00000010 equals 2
  
  // So if the right bumper is triggered bumpRight is 1 
  // (if not triggered then 0)
  // if the left bumper is triggered bumpLeft is 2 
  // (if not triggered then 0)
  
  Serial.println(sensor > 0);
}


void driveUntilBump()
{
  drive(100, 32768);
  while(bumpRight != 1 && bumpLeft != 1){
    checkBumperSensors();
  }
  drive(0, 0);
  playSong(2);
}



void displayDigit(int digit)
{
 //Conditions for displaying segment A
 if(digit!=1 && digit != 4)
 digitalWrite(A,HIGH);
 
 //Conditions for displaying segment B
 if(digit != 5 && digit != 6)
 digitalWrite(B,HIGH);
 
 //Conditions for displaying segment C
 if(digit !=2)
 digitalWrite(C,HIGH);
 
 //Conditions for displaying segment D
 if(digit != 1 && digit !=4 && digit !=7)
 digitalWrite(D,HIGH);
 
 //Conditions for displaying segment E
 if(digit == 2 || digit ==6 || digit == 8 || digit==0)
 digitalWrite(E,HIGH);
 
 //Conditions for displaying segment F
 if(digit != 1 && digit !=2 && digit!=3 && digit !=7)
 digitalWrite(F,HIGH);
 if (digit!=0 && digit!=1 && digit !=7)
 digitalWrite(G,HIGH);
}

void turnOff()
{
  digitalWrite(A,LOW);
  digitalWrite(B,LOW);
  digitalWrite(C,LOW);
  digitalWrite(D,LOW);
  digitalWrite(E,LOW);
  digitalWrite(F,LOW);
  digitalWrite(G,LOW);
}

void playSong(int songNumber)
{
  switch (songNumber)
  { 
    case 1: 
      //New song: Serial sequence: [140] [Song Number] [Song Length] [Note Number 1] [Note Duration 1] 
      //[Note Number 2] [Note Duration 2], etc. 
      //Song opcode: [140] Song number: 1 [01] Song length: 12 [0c] (110 BPM) 
      //Note 1: C 60 [3c] Note 1 Duration: 1/4 36 [09]
      //Note 2: D# 63 [3f] Note 2 Duration: 1/4 36 [09] 
      //Note 3: F 65 [41] Note 3 Duration: Dotted 1/8 24 [06]
      //Note 4: D# 63 [3f] Note 4 Duration: 1/16 + 1/8 32 [08] 
      //Note 5: F 65 [41] Note 5 Duration: 1/8 16 [04]
      //Note 6: F 65 [41] Note 6 Duration: 1/8 16 [04] 
      //Note 7: F 65 [41] Note 7 Duration: 1/8 16 [04]
      //Note 8: A# 70 [46] Note 8 Duration: 1/8 16 [04] 
      //Note 9: Ab 68 [44] Note 9 Duration: 1/8 16 [04]
      //Note 10: G 67 [43] Note 10 Duration: 1/16 8 [02] 
      //Note 11: F 65 [41] Note 11 Duration: 1/8 16 [04]
      //Note 12: G 67 [43] Note 12 Duration: 1/16 + 1/4 44 [0b]
      Roomba.write("\x8c\x01\x0c\x3c\x24\x3f\x20\x41\x18\x3f\x20\x41\x10\x41\x10\x41\x10\x46\x10\x44\x10\x43\x08\x41\x10\x43\x2c"); 
      Roomba.write("\x8d\x01");

      break;

   case 2: 
      //New song: Serial sequence: [140] [Song Number] [Song Length] [Note Number 1] [Note Duration 1] 
      //[Note Number 2] [Note Duration 2], etc. 
      //Song opcode: [140] Song number: 1 [01] Song length: 11 [0b] (110 BPM) 
      //Note 1: G 67 [43] Note 1 Duration: 1/4 36 [24]
      //Note 2: A# 70 [46] Note 2 Duration: 1/4 36 [24] 
      //Note 3: C 72 [48] Note 3 Duration: Dotted 1/8 24 [18]
      //Note 4: F 65 [41] Note 4 Duration: 1/16 + 1/8 32 [20] 
      //Note 5: D# 63 [3f] Note 5 Duration: 1/8 16 [10]
      //Note 6: A# 70 [46] Note 6 Duration: 1/8 16 [10] 
      //Note 7: A# 70 [46] Note 7 Duration: 1/8 16 [10]
      //Note 8: G 67 [43] Note 8 Duration: 1/8 16 [10] 
      //Note 9: A# 70 [46] Note 9 Duration: 1/8 16 [10]
      //Note 10: A# 70 [46] Note 10 Duration: Dotted 1/8 24 [18] 
      //Note 11: C 72 [48] Note 11 Duration: 1/16 + 1/4 42 [2A] 
      Roomba.write("\x8c\x02\x0b\x43\x24\x46\x24\x48\x18\x41\x20\x3f\x10\x46\x10\x46\x10\x43\x10\x46\x10\x46\x18\x48\x2A"); 
      Roomba.write("\x8d\x02");

      break;
  
  }
}
