#include <cppQueue.h>

#define ON 1
#define OFF 0

//-------------------------------------------Testing Flags-------------------------------------------
#define MovementTesting 0
#define SensorTesting 0
#define WavefrontTesting 1

//-------------------------------------------Pin Definitions-------------------------------------------

#define A A4 // A to segment pin A4
#define B A5
#define C 4
#define D 3
#define E A2
#define F A3
#define G 6

//pin for button
byte buttonPin = 7;
byte button2Pin = 2;

//device detect pin
byte ddPin=5;

//-------------------------------------------Button Variables-------------------------------------------

boolean buttonState = 0;       // Button state
boolean button2State = 0;
boolean lastButtonState = 1;   // Previous button state (used to prevent multiple function calls per button press)
boolean lastButton2State = 1; 

//-------------------------------------------Movement-------------------------------------------

#define clamp(value, min, max) (value < min ? min : value > max ? max : value)
#define DEFAULT_SPEED 100
//int motorSpeed = 100; //PROBABLY NOT NEEDED. DELETE AFTER TESTING

//Motor Drive
# define MAX_SPEED 220 // 70% of top speed ==> 256
# define MIN_SPEED 70 

//Drive radius special cases
#define RadStraight 32767
#define RadCCW 1
#define RadCW -1

//-------------------------------------------Sensor Variables-------------------------------------------
byte bumpRight = '0'; // Used to check right bumper sensor
byte bumpLeft = '0';  // Used to check left bumper sensor

int startTime = 0; // used for timing
int endTime = 0;   // used for timing

int distance = 0;
int angle = 0;
int update_delay = 64;
// uint16_t timer_cnt = 0;
// uint8_t timer_on = 0;

//-------------------------------------------Maps-------------------------------------------

byte currentMapNumber = 1; // Currently selected map number
const byte maxMaps = 6;    // Maximum number of maps
int cell_size = 270; // the size of the robot

//Orientation constants
#define NORTH 10
#define SOUTH 100
#define EAST 1000
#define WEST 10000

//map locations
byte x = 0;
byte y = 0;

#define X_SIZE 9
#define Y_SIZE 9
#define MAP_AREA X_SIZE * Y_SIZE
#define FREE 0
#define WALL -2
#define START -1
#define MAX_MAP_SIZE 15

struct Cell
{ // Declare Cell struct
  int8_t x;
  int8_t y;
};

// Variable for orientation of robot, default value is north
int orientation = SOUTH;

// Variable to contain the directions to move to
int8_t direction[MAP_AREA];
// Next best neighbor
Cell nextCell;
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

//Queue used for generating map
cppQueue q(sizeof(Cell), MAP_AREA, FIFO);

//test maps
// //X is vertical, Y is horizontal
//int8_t mapMatrix[X_SIZE][Y_SIZE] =
//    {{-2, -2, -2, -2, -2, -2, -2, -2, -2},
//     {-2, -2, 0, 0, 0, 0, -2, -2, -2},
//     {-2, -2, 0, -2, -2, 0, -2, -2, -2}
//     {-2, -2, 0, -2, -2, 0, -2, -2, -2}
//     {-2, -2, 0, -2, -2, 0, -2, -2, -2}
//     {-2, -2, -1, 0, 0, 0, -2, -2, -2}
//     {-2, -2, -2, -2, -2, -2, -2, -2, -2}
//     {-2, -2, -2, -2, -2, -2, -2, -2, -2}
//     {-2, -2, -2, -2, -2, -2, -2, -2, -2}};
// int start_x = 2;
// int start_y = 5;
// int goal_x = 0;
// int goal_y = 0;
// bool boolMap[X_SIZE][Y_SIZE] =
//         {{0,0,0,0,0,0},
//          {0,0,0,0,0,0},
//          {0,0,0,0,0,0},
//          {0,0,0,0,0,0},
//          {0,0,0,0,0,0},
//          {0,0,0,0,0,0}};
//
//int8_t mapMatrix[X_SIZE][Y_SIZE] =
//    {{-2, -2, -1, 0, -2, -2, -2, -2, -2},
//     {-2, -2, 0, 0, -2, -2, -2, -2, -2},
//     {-2, -2, 0, 0, -2, -2, -2, -2, -2}
//     {-2, -2, 0, 0, -2, -2, -2, -2, -2}
//     {-2, -2, 0, 0, -2, -2, -2, -2, -2}
//     {-2, -2, 0, 0, -2, -2, -2, -2, -2}
//     {-2, -2, -2, -2, -2, -2, -2, -2, -2}
//     {-2, -2, -2, -2, -2, -2, -2, -2, -2}
//     {-2, -2, -2, -2, -2, -2, -2, -2, -2}};

// int mapMatrix[X_SIZE][Y_SIZE]=
//         {{WALL,WALL,START,0,WALL,WALL},
//          {WALL,WALL,0,0,WALL,WALL},
//          {WALL,WALL,0,0,WALL,WALL},
//          {WALL,WALL,0,0,WALL,WALL},
//          {WALL,WALL,0,0,WALL,WALL},
//          {WALL,WALL,0,0,WALL,WALL}};
// int start_x = 2;
// int start_y = 0;
// int goal_x = 0;
// int goal_y = 0;
//
// bool boolMap[X_SIZE][Y_SIZE] =
//         {{0,0,0,0,0,0},
//          {0,0,0,0,0,0},
//          {0,0,0,0,0,0},
//          {0,0,0,0,0,0},
//          {0,0,0,0,0,0},
//          {0,0,0,0,0,0}};

//3 by 3
//int8_t mapMatrix[X_SIZE][Y_SIZE] =
//    {{-1, 0, 0, -2, -2, -2, -2, -2, -2},
//     {0, 0, 0, -2, -2, -2, -2, -2, -2},
//     {0, 0, 0, -2, -2, -2, -2, -2, -2},
//     {-2, -2, -2, -2, -2, -2, -2, -2, -2},
//     {-2, -2, -2, -2, -2, -2, -2, -2, -2},
//     {-2, -2, -2, -2, -2, -2, -2, -2, -2},
//     {-2, -2, -2, -2, -2, -2, -2, -2, -2},
//     {-2, -2, -2, -2, -2, -2, -2, -2, -2},
//     {-2, -2, -2, -2, -2, -2, -2, -2, -2}};
//     
//int8_t start_x = 0;
//int8_t start_y = 0;
//int8_t goal_x = 0;
//int8_t goal_y = 0;
//
//bool boolMap[X_SIZE][Y_SIZE] =
//    {{0, 0, 0},
//     {0, 0, 0},
//     {0, 0, 0}};

//default map
int8_t mapMatrix[X_SIZE][Y_SIZE] =
    {{-1, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0}};
     
int8_t start_x = 0;
int8_t start_y = 0;
int8_t goal_x = 0;
int8_t goal_y = 0;

bool boolMap[X_SIZE][Y_SIZE] =
    {{0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0}};
