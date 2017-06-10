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
	distance = ((left_encoder.read() + right_encoder.read())/2.0) *1.005027318* WHEEL_CIRCUMFERENCE / 5760.0;
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
	int initialLeft = left_encoder.read(); 
	int initialRight = right_encoder.read();
	int tolerance = 30;
	elapsedMillis time = 0;
	while (time < 300)
	{
		int leftError = left_encoder.read()- initialLeft; // placeholder value for left rear led threshold
		int rightError = right_encoder.read() - initialRight; // placeholder for right rear led threshold

		if (leftError > tolerance) // too close from the wall
		{
			analogWrite(LEFT_BACKWARD, 150);
			analogWrite(LEFT_FORWARD, 0);
		}

		else if (leftError < -tolerance) // too far
		{
			analogWrite(LEFT_FORWARD, 120);
			analogWrite(LEFT_BACKWARD, 0);
		}

		else
		{
			analogWrite(LEFT_FORWARD, 0);
			analogWrite(LEFT_BACKWARD, 0);
		}

		if (rightError > tolerance) // too close from the wall
		{
			analogWrite(RIGHT_BACKWARD, 150);
			analogWrite(RIGHT_FORWARD, 0);
		}

		else if (rightError < -tolerance) // too far
		{
			analogWrite(RIGHT_FORWARD, 120);
			analogWrite(RIGHT_BACKWARD, 0);
		}

		else
		{
			analogWrite(RIGHT_FORWARD, 0);
			analogWrite(RIGHT_BACKWARD, 0);
		}
		delay(1);
	}
	goForward(0, 0);
	/*
	bt.println(printMap());
	bt.println(printFloodFill());
	bt.print(curLocation.x);
	bt.print(" ");
	bt.println(curLocation.y);
	*/
	delay(100);
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
	int errorL = 0;
	int errorR = 0;
	int tolerance = 20;
	int turnSpeedL = 230;
	int turnSpeedR = 230;
	elapsedMillis timeLimit = 0;
	unsigned int limit = 500;

	while (timeLimit < limit)
	{
		switch (dir)
		{
		case LEFT:
			errorL =  -(left_encoder.read() + (QUARTER_TURN_DISTANCE / 2.0));
			errorR = (QUARTER_TURN_DISTANCE / 2.0) - right_encoder.read();
			turnSpeedL = absolute(errorL) / (QUARTER_TURN_DISTANCE / 2.0) * 140 + 140;
			turnSpeedR = absolute(errorR) / (QUARTER_TURN_DISTANCE / 2.0) * 140 + 140;
			break;
		case DIAGONAL_LEFT:
			errorL = -(left_encoder.read() + (QUARTER_TURN_DISTANCE / 4.0));
			errorR = (QUARTER_TURN_DISTANCE / 4.0) - right_encoder.read();
			turnSpeedL = absolute(errorL) / (QUARTER_TURN_DISTANCE / 4.0) * 140 + 140;
			turnSpeedR = absolute(errorR) / (QUARTER_TURN_DISTANCE / 4.0) * 140 + 140;
			break;
		case RIGHT:
			errorL = ((QUARTER_TURN_DISTANCE / 2.0) - left_encoder.read());
			errorR = -((QUARTER_TURN_DISTANCE / 2.0) + right_encoder.read());
			turnSpeedL = absolute(errorL) / (QUARTER_TURN_DISTANCE / 2.0) * 140 + 140;
			turnSpeedR = absolute(errorR) / (QUARTER_TURN_DISTANCE / 2.0) * 140 + 140;
			break;
		case DIAGONAL_RIGHT:
			errorL = ((QUARTER_TURN_DISTANCE / 4.0) - left_encoder.read());
			errorR = -((QUARTER_TURN_DISTANCE / 4.0) + right_encoder.read());
			turnSpeedL = absolute(errorL) / (QUARTER_TURN_DISTANCE / 4.0) * 140 + 140;
			turnSpeedR = absolute(errorR) / (QUARTER_TURN_DISTANCE / 4.0) * 140 + 140;
			break;
		case BACK:
			limit = 1000;
			errorL = -(left_encoder.read() + (QUARTER_TURN_DISTANCE*1.0072));
			errorR = (QUARTER_TURN_DISTANCE*1.0072) - right_encoder.read();
			turnSpeedL = absolute(errorL) / (QUARTER_TURN_DISTANCE *1.0) * 100 + 150;
			turnSpeedR = absolute(errorR) / (QUARTER_TURN_DISTANCE*1.0) *  50 + 150;
			break;
		case NO_TURN:
			break;
		default:
			exit(1);
		}

		if (errorL > tolerance)                                   //positive turn right
		{
			analogWrite(LEFT_FORWARD, turnSpeedL);
			analogWrite(LEFT_BACKWARD, 0);
		}
		else if (errorL < -tolerance)							// negative turn left
		{
			analogWrite(LEFT_FORWARD, 0);
			analogWrite(LEFT_BACKWARD, turnSpeedL);
		}

		else                                                   // brake
		{
			analogWrite(LEFT_FORWARD, 0);
			analogWrite(LEFT_BACKWARD, 0);
		}

		if (errorR > tolerance)                                   //positive turn right
		{
			analogWrite(RIGHT_FORWARD, turnSpeedR);
			analogWrite(RIGHT_BACKWARD, 0);
		}
		else if (errorR < -tolerance)							// negative turn left
		{
			analogWrite(RIGHT_FORWARD, 0);
			analogWrite(RIGHT_BACKWARD, turnSpeedR);
		}

		else                                                   // brake
		{
			analogWrite(RIGHT_FORWARD, 0);
			analogWrite(RIGHT_BACKWARD, 0);
		}
	}
	goForward(0, 0);
	resetEncoder();
}



//MovementClass Movement;

