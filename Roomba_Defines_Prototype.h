#ifndef ROOMBA_DEFINES_PROTOTYPE_H
#define ROOMBA_DEFINES_PROTOTYPE_H

#define clamp(value, min, max) (value < min ? min : value > max ? max : value)

#define ON 1
#define OFF 0

//Motor Drive
# define MAX_SPEED 220 // 70% of top speed ==> 256
# define MIN_SPEED 70 

int motorSpeed = 100;

//pin for button
int buttonPin = 7;

//device detect pin
int ddPin=5;

#endif
