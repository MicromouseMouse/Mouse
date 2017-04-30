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
	float distance = (left_encoder.read() + right_encoder.read()) / 2.0 * WHEEL_CIRCUMFERENCE / 5760.0;
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
	analogWrite(RIGHT_FORWARD, baseSpeed);
	analogWrite(LEFT_BACKWARD, 0);
	analogWrite(RIGHT_BACKWARD, 0);
	currentSpeedLeft = baseSpeed;
	currentSpeedRight = baseSpeed;
}

void MovementClass::stop()
{
	analogWrite(LEFT_FORWARD, baseSpeed);
	analogWrite(RIGHT_FORWARD, baseSpeed);
	analogWrite(LEFT_BACKWARD, baseSpeed);
	analogWrite(RIGHT_BACKWARD, baseSpeed);
	delay(300);
	goForward(0, 0);
}

void MovementClass::slow(const int & a)
{
	analogWrite(LEFT_FORWARD, baseSpeed);
	analogWrite(RIGHT_FORWARD, baseSpeed);
	analogWrite(LEFT_BACKWARD, baseSpeed);
	analogWrite(RIGHT_BACKWARD, baseSpeed);
	delay(100);
	goForward(a, a);
}

void MovementClass::turn_encoder(const Turn & dir)  // turn by encoder with LEFT,RIGHT, BACK
{
	
	resetEncoder();
	int error = 0;
	int tolerance = 60;
	int minSpeed = 200;
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
			minSpeed = 200;
			limit = 1000;
			error = (2*QUARTER_TURN_DISTANCE - (left_encoder.read() - right_encoder.read()));
		}
		
		if (error > tolerance)                                   //positive turn right
		{
			analogWrite(LEFT_BACKWARD, 0);
			analogWrite(RIGHT_FORWARD, 0);
			analogWrite(LEFT_FORWARD,  minSpeed);
			analogWrite(RIGHT_BACKWARD, minSpeed + 60);
		}

		else if (error < -tolerance)							// negative turn left
		{
			analogWrite(LEFT_FORWARD, 0);
			analogWrite(RIGHT_BACKWARD, 0);
			analogWrite(LEFT_BACKWARD, minSpeed);
			analogWrite(RIGHT_FORWARD, minSpeed + 60);
		}

		else                                                   // brake
		{
			analogWrite(LEFT_FORWARD, 250);
			analogWrite(RIGHT_BACKWARD, 250);
			analogWrite(LEFT_BACKWARD, 250);
			analogWrite(RIGHT_FORWARD, 250);
		}
	}
	goForward(0, 0);
	resetEncoder();
}

void MovementClass::turn_gyro(const Turn & dir) // turn by gyro with LEFT,RIGHT, BACK
{
	
	int count = left_encoder.read();
	short dt = 100;
	double curAngle = 0;
	double turn = 90;
	int turnSpeed = 250;
	switch (dir)
	{
	case LEFT:
		

		while (curAngle < turn)
		{
			curAngle += getGyroError() * 3.3 / 65535 / (0.57/1000)  * dt / 1000000;
			turnSpeed = baseSpeed - (baseSpeed) * (curAngle / turn);
			if (curAngle > 30) turnSpeed = 100;

			analogWrite(LEFT_BACKWARD, turnSpeed);
			analogWrite(LEFT_FORWARD, 0);
			analogWrite(RIGHT_FORWARD, turnSpeed);
			analogWrite(RIGHT_BACKWARD, 0);
			delayMicroseconds(dt);
		}

		break;

	case RIGHT:
		

		while (curAngle < turn*2/3)
		{
			curAngle += -getGyroError() * 3.3 / 65535 / (0.57 / 1000)  * dt / 1000000;
			turnSpeed = baseSpeed - (baseSpeed ) * (curAngle / (turn * 2 / 3));
			if (curAngle > 30) turnSpeed = 100;

			analogWrite(LEFT_FORWARD, turnSpeed);
			analogWrite(LEFT_BACKWARD, 0);
			analogWrite(RIGHT_BACKWARD, turnSpeed);
			analogWrite(RIGHT_FORWARD, 0);
			delayMicroseconds(dt);
		}

		break;

	case BACK:

		

		while (curAngle < 2*turn)
		{
			curAngle += getGyroError() * 3.3 / 65535 / (0.57 / 1000)  * dt / 1000000;
			turnSpeed = baseSpeed - (baseSpeed) * (curAngle / (2*turn));
			if (curAngle > 120) turnSpeed = 100;
			
			analogWrite(LEFT_BACKWARD, turnSpeed);
			analogWrite(LEFT_FORWARD, 0);
			analogWrite(RIGHT_FORWARD, turnSpeed);
			analogWrite(RIGHT_BACKWARD, 0);
			delayMicroseconds(dt);
		}

		break;
	}
	left_encoder.write(count);
	right_encoder.write(count);
	goForward(0,0);
}

//MovementClass Movement;

