// Led.h

#ifndef _LED_h
#define _LED_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

const uint8_t LEFT_LED = 2;
const uint8_t RIGHT_LED = 6;
const uint8_t MIDDLE_LED = 5;

const uint8_t LEFT_REAR_RECEIVER = 22;
const uint8_t LEFT_DIAGONAL_RECEIVER = 21;
const uint8_t LEFT_MIDDLE_RECEIVER = 20;
const uint8_t RIGHT_MIDDLE_RECEIVER = 19;
const uint8_t RIGHT_DIAGONAL_RECEIVER = 18;
const uint8_t RIGHT_REAR_RECEIVER = 17;

const int WALL_LEFT = 11000;
const int WALL_RIGHT = 6000;
const int WALL_FRONT = 15000;

enum Wall{W_LEFT,W_RIGHT,W_FRONT};
enum LED{LEFT_REAR, LEFT_MIDDLE, LEFT_DIAGONAL, RIGHT_REAR, RIGHT_MIDDLE, RIGHT_DIAGONAL};
class LedClass
{
public:
	unsigned short left_rear;
	unsigned short left_middle;
	unsigned short left_diagonal;
	unsigned short right_rear;
	unsigned short right_middle;
	unsigned short right_diagonal;

	int offsetLed;
	
	int leftThreshold;
	int rightThreshold;
	int frontThreshold;
	int leftMiddleThreshold;
	int rightMiddleThreshold;
	int frontThresholdL;
	int frontThresholdR;

	LedClass();
	// initialize pinMode, led reading and wall

	void init1();
	void init2();
	int getLed(const LED &a);
		// return LED reading from correspoding position


	void measure(const unsigned int &time);
	// turn on LED, delay for time (in microsecond) then get reading then turn off 
	// do left, right first then middle

};



#endif

