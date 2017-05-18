#include "Maze.h"
#include "Movement.h"
#include <Bounce.h>

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
	minSpeed = 130;
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
	distance = ((left_encoder.read() + right_encoder.read())/2.0) * WHEEL_CIRCUMFERENCE / 5760.0 *30/29.0;
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
	analogWrite(RIGHT_FORWARD, baseSpeed+baseSpeed * 0.02);
	analogWrite(LEFT_BACKWARD, 0);
	analogWrite(RIGHT_BACKWARD, 0);
	currentSpeedLeft = baseSpeed;
	currentSpeedRight = baseSpeed + baseSpeed * 0.02;
}

void MovementClass::goBackward(const int & left, const int & right)
{
	analogWrite(LEFT_BACKWARD, left);
	analogWrite(RIGHT_BACKWARD, right);
	analogWrite(LEFT_FORWARD, 0);
	analogWrite(RIGHT_FORWARD, 0);
	currentSpeedLeft = left;
	currentSpeedRight = right;
}

void MovementClass::goForwardCell(const int &a) //go foward num cell
{
	travel = a * 18;
	distance = (left_encoder.read() + right_encoder.read()) *0.5 * WHEEL_CIRCUMFERENCE / 5760.0;
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

void MovementClass::stopForward()
{
	int initial = left_encoder.read() + right_encoder.read();
	int current = left_encoder.read() + right_encoder.read();
	int dt = 100;
	while(current + 1000  > initial )
	{
		current = left_encoder.read() + right_encoder.read();
		goBackward(750, 750);// kp*speed, kp*speed);
		delayMicroseconds(dt);
	}
	goForward(0, 0);
	delay(150);
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
	analogWrite(LEFT_FORWARD, 250);
	analogWrite(RIGHT_FORWARD, 250);
	analogWrite(LEFT_BACKWARD, 250);
	analogWrite(RIGHT_BACKWARD, 250);
	delay(80);
	baseSpeed = minSpeed;
	goForward(a, a);
}

void MovementClass::curveTurn(const Turn &dir)
{
	resetEncoder();
	float kp = 0.2;
	elapsedMillis timeLimit= 0;
	int left = left_encoder.read();
	int right = right_encoder.read();
	int turnDistance=0;
	int leftBaseSpeed=0;
	int rightBaseSpeed=0;
	int tolerance = 40;

	if (dir == U_LEFT || dir == U_RIGHT) turnDistance = 2 * QUARTER_CURVE_TURN;
	else turnDistance = QUARTER_CURVE_TURN;
	int error;
	int error2;
	while (left + right < 0.68*turnDistance)
	{
		switch (dir)
		{
		case LEFT:
		case U_LEFT:
			leftBaseSpeed = 90;
			rightBaseSpeed = 200;
			error = SPEED_RATIO *left - right;
			goForward(leftBaseSpeed - kp*error, rightBaseSpeed + kp*error);
			break;

		case RIGHT:
		case U_RIGHT:
			leftBaseSpeed = 200;
			rightBaseSpeed = 90;
			error = SPEED_RATIO *right - left;
			goForward(leftBaseSpeed + kp*error, rightBaseSpeed - kp*error);
			break;
		default:
			exit(1);
		}
		
		left = left_encoder.read();
		right = right_encoder.read();
		break;
		
	}

	timeLimit = 0;
	while (timeLimit < 500)
	{
		left = left_encoder.read();
		right = right_encoder.read();
		error = turnDistance - (right + left);
		switch (dir)
		{
		case LEFT:
		case U_LEFT:
			error2 = SPEED_RATIO * left - right;
			if (error > tolerance)
			{

				goForward(leftBaseSpeed - kp*error2, rightBaseSpeed + kp*error2);
			}
			else
			{
				goBackward(leftBaseSpeed + kp*error2, rightBaseSpeed - kp*error2);
			}
			break;
		case RIGHT:
		case U_RIGHT:
			error2 = SPEED_RATIO * right - left;
			if (error > tolerance)
			{
				goForward(leftBaseSpeed + kp*error2, rightBaseSpeed - kp*error2);
			}
			else
			{
				goBackward(leftBaseSpeed - kp*error2, rightBaseSpeed + kp*error2);
			}
			break;
		default:
			exit(1);
		}
	}
	stop();
	resetEncoder();
}

void MovementClass::turn_encoder(const Turn & dir)  // turn by encoder with LEFT,RIGHT, BACK
{
	
	resetEncoder();
	int error = 0;
	int tolerance = 30;
	int turnSpeed = 230;
	elapsedMillis timeLimit =0;
	unsigned int limit = 500;

	while (timeLimit < limit)
	{
		switch (dir)
		{
		case LEFT:
			error = -(QUARTER_TURN_DISTANCE - (right_encoder.read() - left_encoder.read()));
			turnSpeed = absolute(error) / (1.0*QUARTER_TURN_DISTANCE) * 100 + 130;
			break;
		case DIAGONAL_LEFT:
			error = -(QUARTER_TURN_DISTANCE/2 - (right_encoder.read() - left_encoder.read()));
			turnSpeed = absolute(error) / (0.5*QUARTER_TURN_DISTANCE) * 100 + 130;
			limit = 350;
			break;
		case RIGHT:
			error = (QUARTER_TURN_DISTANCE - (left_encoder.read() - right_encoder.read()));
			turnSpeed = absolute(error) / (1.0*QUARTER_TURN_DISTANCE) * 100 + 130;
			break;
		case DIAGONAL_RIGHT:
			error = (QUARTER_TURN_DISTANCE/2 - (left_encoder.read() - right_encoder.read()));
			turnSpeed = absolute(error) / (0.5*QUARTER_TURN_DISTANCE) * 100 + 130;
			limit = 350;
			break;
		case BACK:
			limit = 1000;
			error = (2 * QUARTER_TURN_DISTANCE - (left_encoder.read() - right_encoder.read()));
			turnSpeed = absolute(error) / (2.0*QUARTER_TURN_DISTANCE) * 100 + 130;
			break;
		default:
			exit(1);
		}


		if (error > tolerance)                                   //positive turn right
		{
			analogWrite(LEFT_BACKWARD, 0);
			analogWrite(RIGHT_FORWARD, 0);
			analogWrite(LEFT_FORWARD, turnSpeed);
			analogWrite(RIGHT_BACKWARD, turnSpeed);
		}

		else if (error < -tolerance)							// negative turn left
		{
			analogWrite(LEFT_FORWARD, 0);
			analogWrite(RIGHT_BACKWARD, 0);
			analogWrite(LEFT_BACKWARD, turnSpeed);
			analogWrite(RIGHT_FORWARD, turnSpeed);
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

//MovementClass Movement;

