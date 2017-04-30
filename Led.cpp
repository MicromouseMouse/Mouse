// 
// 
// 

#include "Led.h"

LedClass::LedClass()
{
	pinMode(LEFT_LED, OUTPUT);
	pinMode(RIGHT_LED, OUTPUT);
	pinMode(MIDDLE_LED, OUTPUT);
	pinMode(LEFT_REAR_RECEIVER, INPUT);
	pinMode(LEFT_DIAGONAL_RECEIVER, INPUT);
	pinMode(LEFT_MIDDLE_RECEIVER, INPUT);
	pinMode(RIGHT_MIDDLE_RECEIVER, INPUT);
	pinMode(RIGHT_DIAGONAL_RECEIVER, INPUT);
	pinMode(RIGHT_REAR_RECEIVER, INPUT);

	left_rear = 0;
	left_middle = 0;
	left_diagonal = 0;
	right_rear = 0;
	right_middle = 0;
	right_diagonal = 0;

	wallFront = false;
	wallLeft = true;
	wallRight = true;
}

void LedClass::init()
{
	delay(1000);
	measure(1000);
	leftThreshold = left_diagonal ;
	rightThreshold = right_diagonal ;
	frontThreshold = left_rear + right_rear + 650;
	offsetLed = left_diagonal - right_diagonal;
}

int LedClass::getLed(const LED &a)
{
	switch (a)
	{
	case LEFT_REAR:
		return left_rear;
	case LEFT_MIDDLE:
		return left_middle;
	case LEFT_DIAGONAL:
		return left_diagonal;
	case RIGHT_REAR:
		return right_rear;
	case RIGHT_MIDDLE:
		return right_middle;
	case RIGHT_DIAGONAL:
		return right_diagonal;
	}
	exit(1);
}

bool LedClass::checkWall(const Wall &a)
{
	switch (a)
	{
	case W_LEFT:
		return wallLeft;
	case W_RIGHT:
		return wallRight;
	case W_FRONT:
		return wallFront;
	}
	exit(1);
}

void LedClass::measure(const unsigned int &time)
{
	digitalWrite(LEFT_LED, HIGH);
	digitalWrite(RIGHT_LED, HIGH);
	delayMicroseconds(time);
	
	left_rear = analogRead(LEFT_REAR_RECEIVER);
	left_diagonal = analogRead(LEFT_DIAGONAL_RECEIVER);
	right_rear = analogRead(RIGHT_REAR_RECEIVER);
	right_diagonal = analogRead(RIGHT_DIAGONAL_RECEIVER);

	digitalWrite(LEFT_LED, LOW);
	digitalWrite(RIGHT_LED, LOW);
	digitalWrite(MIDDLE_LED, HIGH);
	delayMicroseconds(time);
	
	left_middle = analogRead(LEFT_MIDDLE_RECEIVER);
	right_middle = analogRead(RIGHT_MIDDLE_RECEIVER);

	digitalWrite(MIDDLE_LED, LOW);

	if (left_diagonal <= WALL_SIDE) wallLeft = false;
	else wallLeft = true;

	if (right_diagonal <= WALL_SIDE) wallRight = false;
	else wallRight = true;

	if ((left_rear + right_rear) <= WALL_FRONT) wallFront = false;
	else wallFront = true;
}