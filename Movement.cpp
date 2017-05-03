// 
// 
// 

#include "Movement.h"
#include <Bounce.h>
#include <SoftwareSerial.h>
double absolute(const double &a)
{
	if (a > 0) return a;
	else return -a;
}

MovementClass::MovementClass(const int& base) :left_encoder(LEFT_ENCODER_1, LEFT_ENCODER_2), right_encoder(RIGHT_ENCODER_1, RIGHT_ENCODER_2)
{
	analogReadRes(16);
	analogWriteRes(10);

	currentSpeedLeft = base;
	currentSpeedRight = base;
	baseSpeed = base;
	distance = 0;
	travel = 0;
	minSpeed = 120;
	slowFlag = false;
	pinMode(GYRO, INPUT);
}

int MovementClass::getCurrentSpeed(const Turn &dir)
{
	if (dir == LEFT) return currentSpeedLeft;
	else return currentSpeedRight;
}

long MovementClass::getEncoderReading(const Turn& side)
{
	if (side == LEFT) return left_encoder.read();
	else return right_encoder.read();
}

float MovementClass::getDistanceTravel()   // get total distance travel
{
	distance = (left_encoder.read() + right_encoder.read()) / 2.0 * WHEEL_CIRCUMFERENCE / 5760.0 * 31/29.0;
	return distance;
}

void MovementClass::goForward(const int & left, const int & right)  // go forward at specified speed
{
	analogWrite(LEFT_FORWARD, left);
	analogWrite(RIGHT_FORWARD, right);
	analogWrite(LEFT_BACKWARD, 0);
	analogWrite(RIGHT_BACKWARD, 0);
	currentSpeedLeft = left;
	currentSpeedRight = right;
}

void MovementClass::goForward() // go forward at base speed
{
	analogWrite(LEFT_FORWARD, baseSpeed);
	analogWrite(RIGHT_FORWARD, baseSpeed+baseSpeed/50);
	analogWrite(LEFT_BACKWARD, 0);
	analogWrite(RIGHT_BACKWARD, 0);
	currentSpeedLeft = baseSpeed;
	currentSpeedRight = baseSpeed + baseSpeed / 50;
}

void MovementClass::goForwardCell(const int &a) //go foward num cell
{
	travel = a * 18;
	distance = (left_encoder.read() + right_encoder.read()) / 2.0 * WHEEL_CIRCUMFERENCE / 5760.0;
	if (distance > 0.99*travel)
		stop();
	else if ((travel - distance) < 10)
	{
		if (!slowFlag)
		{
			slowFlag = true;
			slow(minSpeed);
		}
		goForward(minSpeed, minSpeed);
	}
}

void MovementClass::stop()
{
	slowFlag = false;
	analogWrite(LEFT_FORWARD, baseSpeed);
	analogWrite(RIGHT_FORWARD, baseSpeed);
	analogWrite(LEFT_BACKWARD, baseSpeed);
	analogWrite(RIGHT_BACKWARD, baseSpeed);
	delay(300);
	goForward(0, 0);
}

void MovementClass::slow(const int & a)
{
	analogWrite(LEFT_FORWARD, 250);
	analogWrite(RIGHT_FORWARD, 250);
	analogWrite(LEFT_BACKWARD, 250);
	analogWrite(RIGHT_BACKWARD, 250);
	delay(300);
	baseSpeed = minSpeed;
	goForward(a, a);
}

void MovementClass::turn_encoder(const Turn & dir)  // turn by encoder with LEFT,RIGHT, BACK
{
	
	resetEncoder();
	int error = 0;
	int tolerance = 30;
	int turnSpeed = 180;
	elapsedMillis timeLimit =0;
	int limit = 500;
	while (timeLimit < limit)
	{
		switch (dir)
		{
		case LEFT:
			error = -(QUARTER_TURN_DISTANCE - (right_encoder.read() - left_encoder.read()));
			break;
		case RIGHT:
			error = (QUARTER_TURN_DISTANCE - (left_encoder.read() - right_encoder.read()));
			break;
		case BACK:
			limit = 2000;
			error = (2*QUARTER_TURN_DISTANCE - (left_encoder.read() - right_encoder.read()));
		}
		
		if (error > tolerance)                                   //positive turn right
		{
			analogWrite(LEFT_BACKWARD, 0);
			analogWrite(RIGHT_FORWARD, 0);
			analogWrite(LEFT_FORWARD, turnSpeed);
			analogWrite(RIGHT_BACKWARD, turnSpeed+5);
		}

		else if (error < -tolerance)							// negative turn left
		{
			analogWrite(LEFT_FORWARD, 0);
			analogWrite(RIGHT_BACKWARD, 0);
			analogWrite(LEFT_BACKWARD, turnSpeed);
			analogWrite(RIGHT_FORWARD, turnSpeed+5);
		}

		else                                                   // brake
		{
			analogWrite(LEFT_FORWARD, 50);
			analogWrite(RIGHT_BACKWARD, 50);
			analogWrite(LEFT_BACKWARD, 50);
			analogWrite(RIGHT_FORWARD, 50);
		}
	}
	goForward(0, 0);
	resetEncoder();
}

//MovementClass Movement;

