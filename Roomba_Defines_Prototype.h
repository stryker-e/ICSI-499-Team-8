#include <cppQueue.h>

#define ON 1
#define OFF 0

//-------------------------------------------Testing Flags-------------------------------------------
#define MovementTesting 1
#define SensorTesting 1
#define WavefrontTesting 1

//-------------------------------------------Pin Definitions-------------------------------------------

#define A A4 // A to segment pin A4
#define B A5
#define C 4
#define D 3
#define E 2
#define F A3
#define G 6

//pin for button
int buttonPin = 7;

//device detect pin
int ddPin=5;

//-------------------------------------------Button Variables-------------------------------------------

int buttonState = 0;       // Button state
int lastButtonState = 1;   // Previous button state (used to prevent multiple function calls per button press)

//-------------------------------------------Movement-------------------------------------------

#define clamp(value, min, max) (value < min ? min : value > max ? max : value)
#define DEFAULT_SPEED 100
//int motorSpeed = 100; //PROBABLY NOT NEEDED. DELETE AFTER TESTING

//Motor Drive
# define MAX_SPEED 220 // 70% of top speed ==> 256
# define MIN_SPEED 70 

//Drive radius special cases
#define RadStraight 32768
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
int cell_size = 330; // the size of the robot

//Orientation constants
#define NORTH 10
#define SOUTH 100
#define EAST 1000
#define WEST 10000

//map locations
byte x = 0;
byte y = 0;

//#define X_SIZE 6
//#define Y_SIZE 6
//#define MAP_AREA 36
//#define FREE 0
//#define WALL 1
//#define GOAL 2

#define X_SIZE 6
#define Y_SIZE 6
#define MAP_AREA X_SIZE * Y_SIZE
#define FREE 0
#define WALL -2
#define START -1
#define MAX_MAP_SIZE 15

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
