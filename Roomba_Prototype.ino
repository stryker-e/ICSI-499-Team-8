#include "Roomba_Defines_Prototype.h"
#include <SoftwareSerial.h>

#define A 3
#define B 2
#define C 9
#define D 7
#define E 6
#define F 4
#define G 8
#define DP 12 // decimal

//rxPin: the pin on which to receive serial data
int rxPin=10;
//txPin: the pin on which to transmit serial data
int txPin=11;

//Create an instance of a SoftwareSerial object with the rx pin and tx pin as parameters.
SoftwareSerial Roomba(rxPin,txPin);

//Currently selected map number
int currentMapNumber = 1;
//Maximum number of maps
const int maxMaps = 9;
//Button state
int buttonState = 0;  
//Previous button state (used to prevent multiple function calls per button press)
int lastButtonState = 1;

int seg[] {A,B,C,D,E,F,G,DP}; // segment pins
byte Chars[10][9] { 
           {'0',1,1,1,1,1,1,0,0},//0
           {'1',0,1,1,0,0,0,0,0},//1
           {'2',1,1,0,1,1,0,1,0},//2
           {'3',1,1,1,1,0,0,1,0},//3
           {'4',0,1,1,0,0,1,1,0},//4
           {'5',1,0,1,1,0,1,1,0},//5
           {'6',1,0,1,1,1,1,1,0},//6
           {'7',1,1,1,0,0,0,0,0},//7
           {'8',1,1,1,1,1,1,1,0},//8
           {'9',1,1,1,1,0,1,1,0},//9
           };

char bumpRight = '0';
char bumpLeft = '0';

void setup() {

  //Sets the speed (baud rate) for the serial communication.
  Roomba.begin(19200);

  //Sets the data rate in bits per second (baud) for serial data transmission.
  Serial.begin(19200);

  delay(2000);

  //configures the devide detect pin into output mode. 
  pinMode(ddPin, OUTPUT);

  //Pulses the BRC pin in the Roomba to wake it up
  wakeUp();

  //Starts the Roomba and puts it in safe mode
  startSafe();

  Serial.println("Robo Jumbo: Ready to Roll");

  //reset seven segment
  turnOff();
  //display 1 on seven segment
  displayDigit(currentMapNumber);
  
  delay(2000);
  
  
}

void loop() {

  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  //Check if the buttonState has changed
  if (buttonState != lastButtonState) {
    //Check if the button is being pressed
    //if the current state is HIGH then the button went from off to on:
    if (buttonState == HIGH) {
      changeMap();
      //testDrive();
      playSong(1);
      driveUntilBump();
      //Serial.println("Button Works");
      
      //The Roomba Spins in a half circle
      //turnCW(100, 180);
      
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(10);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;

}


void changeMap(){
  int currentMap = (currentMapNumber++ % maxMaps) + 1;
  turnOff();
  displayDigit(currentMap);
  Serial.print(currentMap);
}

void wakeUp (void)
{
  //Pulses the BRC (Baud Rate Charge) pin to wake up the Roomba
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
  clamp(velocity, -500, 500); //def max and min velocity in mm/s
  clamp(radius, -2000, 2000); //def max and min radius in mm
  
  Roomba.write(137);
  Roomba.write(velocity >> 8);
  Roomba.write(velocity);
  Roomba.write(radius >> 8);
  Roomba.write(radius);
}

//Control the forward and backward motion of Roomba’s drive wheels 
//independently. It takes four data bytes, which are interpreted as two 16-bit signed values using two’s 
//complement. The first two bytes specify the velocity of the right wheel in millimeters per second (mm/s), 
//with the high byte sent first. The next two bytes specify the velocity of the left wheel, in the same 
//format. A positive velocity makes that wheel drive forward, while a negative velocity makes it drive backward. 
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

//Controls the raw forward and backward motion of Roomba’s drive wheels 
//independently. It takes four data bytes, which are interpreted as two 16-bit signed values using two’s 
//complement. The first two bytes specify the PWM of the right wheel, with the high byte sent first. The 
//next two bytes specify the PWM of the left wheel, in the same format. A positive PWM makes that wheel 
//drive forward, while a negative PWM makes it drive backward. 
void driveWheelsPWM(int rightPWM, int leftPWM)
{
  clamp(rightPWM, -255, 255);
  clamp(leftPWM, -255, 255);
  
  Roomba.write(146);
  Roomba.write(rightPWM >> 8);
  Roomba.write(rightPWM);
  Roomba.write(leftPWM >> 8);
  Roomba.write(leftPWM);
}

//Turns clockwise by driving with a radius of -1
void turnCW(unsigned short velocity, unsigned short degrees)
{
  drive(velocity, -1);
  clamp(velocity, 0, 500);
  delay(6600);
  drive(0,0);
}

//Turns counter-clockwise by driving with a radius of 1
void turnCCW(unsigned short velocity, unsigned short degrees)
{
  drive(velocity, 1); 
  clamp(velocity, 0, 500);
  delay(6600);
  drive(0,0);
}

void driveStop(void)
{
  drive(0,0);
}

void driveLeft(int left)
{
  driveWheels(left, 0);
}

void driveRight(int right)
{
  driveWheels(0, right);
}

//Resets the Roomba as if the battery was removed. 
void reset(void)
{
  Roomba.write(7);
}


/*This command stops the OI. All streams will stop and the robot will no longer respond to commands. Use this command when you are finished working with the robot. */
void stop(void)
{
  Roomba.write(173);
}

/*This command powers down Roomba.*/
void powerOff(void)
{
  Roomba.write(133);
}

/*This command sends Roomba to seek for its dock.*/
void seekDock(void)
{
  Roomba.write(143);
}

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
  // Song opcode: [140] Song number: 1 [01] Song length: 12 [0c] (110 BPM) 
  //Note 1: C 60 [3c] Note 1 Duration: 1/4 9 [09]
  //Note 2: D# 63 [3f] Note 2 Duration: 1/4 9 [09] 
  //Note 3: F 65 [41] Note 3 Duration: Dotted 1/8 6 [06]
  //Note 4: D# 63 [3f] Note 4 Duration: 1/16 + 1/8 8 [08] 
  //Note 5: F 65 [41] Note 5 Duration: 1/8 4 [04]
  //Note 6: F 65 [41] Note 6 Duration: 1/8 4 [04] 
  //Note 7: F 65 [41] Note 7 Duration: 1/8 4 [04]
  //Note 8: A# 70 [46] Note 8 Duration: 1/8 4 [04] 
  //Note 9: Ab 68 [44] Note 9 Duration: 1/8 4 [04]
  //Note 10: G 67 [43] Note 10 Duration: 1/16 2 [02] 
  //Note 11: F 65 [41] Note 11 Duration: 1/8 4 [04]
  //Note 12: G 67 [43] Note 12 Duration: 1/16 + 1/4 11 [0b]
  Roomba.write("\x8c\x01\x0c\x3c\x24\x3f\x20\x41\x18\x3f\x20\x41\x10\x41\x10\x41\x10\x46\x10\x44\x10\x43\x08\x41\x10\x43\x2c"); 
  Roomba.write("\x8d\x01");

      break;

   case 2: 
     //New song: Serial sequence: [140] [Song Number] [Song Length] [Note Number 1] [Note Duration 1] 
  //[Note Number 2] [Note Duration 2], etc. 
  // Song opcode: [140] Song number: 1 [01] Song length: 11 [0b] (110 BPM) 
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

void testDrive()
{
  drive(100, 32768);
  delay(1000);
  drive(0,0);
}
