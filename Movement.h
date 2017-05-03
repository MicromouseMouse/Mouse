// Movement.h

#ifndef _MOVEMENT_h
#define _MOVEMENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include "Maze.h"

const uint8_t LEFT_ENCODER_1 = 0;
const uint8_t LEFT_ENCODER_2 = 1;
const uint8_t RIGHT_ENCODER_1 = 12;
const uint8_t RIGHT_ENCODER_2 = 11;

const uint8_t GYRO = 23;

const uint8_t LEFT_FORWARD = 3;
const uint8_t LEFT_BACKWARD = 4;
const uint8_t RIGHT_FORWARD = 10;
const uint8_t RIGHT_BACKWARD = 9;

const int QUARTER_TURN_DISTANCE = 5250;
const double WHEEL_CIRCUMFERENCE = 3.7 * PI;
const int GYRO_OFFSET = 29976;


enum Turn{LEFT,RIGHT,BACK};

class MovementClass
{
public:
	int baseSpeed;
	Encoder left_encoder;
	Encoder right_encoder;
	int currentSpeedLeft;
	int currentSpeedRight;
	int offsetEncoder;
	double distance;
	double travel;
	int minSpeed;
	bool slowFlag;


	int getGyroError()
	{
		int threshold = 100;
		int error = (analogRead(GYRO) - GYRO_OFFSET);
		if ((error < threshold) && (error > (-threshold))) return 0;
		else if (error > threshold) return error - threshold;
		else return error + threshold;
	}

	// initilize encoder, gyro, base speed
	MovementClass(const int & base);

	void stop();

	void slow(const int & a);
	
	void resetEncoder()
	{
		left_encoder.write(0);
		right_encoder.write(0);
	}

	long getEncoderReading(const Turn& side);  //get encoder reading of LEFT or RIGHT

	int getGyroReading() { return analogRead(GYRO); }

	int getCurrentSpeed(const Turn &dir); //get current speed LEFT or RIGHT

	float getDistanceTravel();  // get total distance travel
	
	void goForward(const int & left, const int & right);  // go forward at specified speed

	void goForward(); // go forward at base speed

	void turn_encoder(const Turn & dir);  // turn by encoder with LEFT,RIGHT, BACK

	void goForwardCell(const int &a);
};

//extern MovementClass Movement(0);

#endif

