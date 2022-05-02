#define clamp(value, min, max) (value < min ? min : value > max ? max : value)

#define ON 1
#define OFF 0

//Motor Drive
# define MAX_SPEED 220 // 70% of top speed ==> 256
# define MIN_SPEED 70 

// Drive radius special cases
#define RadStraight 32768
#define RadCCW 1
#define RadCW -1

int motorSpeed = 100;

//pin for button
int buttonPin = 7;

//device detect pin
int ddPin=5;

 /************************ MAP ***********************************/

//Orientation constants
#define NORTH 10
#define SOUTH 100
#define EAST 1000
#define WEST 10000

//#define X_SIZE 6
//#define Y_SIZE 6
//#define MAP_AREA 36
//#define FREE 0
//#define WALL 1
//#define GOAL 2

#define X_SIZE 3
#define Y_SIZE 3
#define MAP_AREA 9 
#define FREE 0
#define WALL 1
#define GOAL 2
