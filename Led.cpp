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

}

void LedClass::init1()
{
	delay(1000);
	measure(10);
	leftThreshold = left_diagonal ;
	rightThreshold = right_diagonal ;
	frontThresholdL = left_rear;
	frontThresholdR = right_rear;
	frontThreshold = left_rear + right_rear;
	offsetLed = left_diagonal - right_diagonal;
}

void LedClass::init2()
{
	delay(1000);
	measure(10);
	leftMiddleThreshold = left_middle;
	rightMiddleThreshold = right_middle;
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
}