

#include "Maze.h"
#include "Map.h"
#include <Bounce\Bounce.h>
#include "PID.h"
#include <Encoder\Encoder.h>
#include "Movement.h"
#include "Led.h"
#include <stddef.h>
#include <SoftwareSerial.h>

using namespace std;
elapsedMicros time;  // timer for pid to calculate dt
elapsedMicros control;  // timer to reset flag
elapsedMicros mapLoop;
bool ledFlag = true;
bool PIDFlag = true;
bool slowFlag = false;
const int MIN_SPEED = 85;

volatile bool modeLeft = false;
volatile bool modeRight = false;

const int ledTime = 30; //microsecond
const int baseSpeed = 200;

LedClass led;
MovementClass move(baseSpeed);
PIDClass pid(&move, &led, time);
MazeClass maze(&led, &move);
SoftwareSerial bluetooth(7, 8);

const int frontWallThreshold = 25000;
void testLed();
void testEncoder();
void testMotor();
void testStraight(const PID_MODE& A, char m);

void setup()
{
	delay(2000);
	bluetooth.begin(9600);
	
	led.init();
	
	move.resetEncoder();
	control = 0;
	time = 0;
	//move.goForward();
}

void loop()
{
	//testLed();
	//testEncoder();
	testMotor();
	//testStraight(ENCODER_MODE,'a');
	

}

void store()
{
	int count = 0;
	while (true)
	{
		//bluetooth.println(move.getDistanceTravel(),3);
		//bluetooth.println(maze.curDistance, 3);
		if (led.getLed(LEFT_REAR) + led.getLed(RIGHT_REAR) > frontWallThreshold)
		{
			move.stop();
			maze.updateMap();
			move.baseSpeed = baseSpeed;
			slowFlag = false;
			//bluetooth.print(maze.printMap());
			//delay(2000);
			if (led.getLed(LEFT_DIAGONAL) < 7000)
			{
				move.turn_encoder(LEFT);
				led.measure(ledTime);
				move.goForward();
				delay(100);
			}
			else if (led.getLed(RIGHT_DIAGONAL) < 6000)
			{
				move.turn_encoder(RIGHT);
				led.measure(ledTime);
				move.goForward();
				delay(100);
			}
			else;
			break;
		}

		else if (led.getLed(LEFT_REAR) + led.getLed(RIGHT_REAR) > led.frontThreshold && !slowFlag)
		{
			slowFlag = true;
			move.slow(move.minSpeed);
		}

		maze.mapping();

		if (control > 1000)
		{
			ledFlag = true;
			PIDFlag = true;
			control = 0;
		}

		if (ledFlag)
		{
			led.measure(ledTime);
			ledFlag = false;
		}

		if (PIDFlag)
		{
			pid.PID(LED_MODE);
			PIDFlag = false;
		}

	}
}

void testLed()
{
	while (true)

	{
		led.measure(ledTime);
		/*
		Serial.print("");
		Serial.print(led.getLed(LEFT_REAR), DEC);
		Serial.print("  ");
		Serial.print(led.getLed(LEFT_DIAGONAL), DEC);
		Serial.print("  ");
		Serial.print(led.getLed(LEFT_MIDDLE), DEC);
		Serial.print("  ");
		Serial.print(led.getLed(RIGHT_MIDDLE), DEC);
		Serial.print("  ");
		Serial.print(led.getLed(RIGHT_DIAGONAL), DEC);
		Serial.print("  ");
		Serial.print(led.getLed(RIGHT_REAR), DEC);
		Serial.println("\n");
		*/

		bluetooth.println("test");
		bluetooth.print("");
		bluetooth.println(led.getLed(LEFT_REAR));//, DEC);
		bluetooth.print("  ");
		bluetooth.print(led.getLed(LEFT_DIAGONAL));//, DEC);
		bluetooth.print("  ");
		bluetooth.print(led.getLed(LEFT_MIDDLE));//, DEC);
		bluetooth.print("  ");
		bluetooth.print(led.getLed(RIGHT_MIDDLE));//, DEC);
		bluetooth.print("  ");
		bluetooth.print(led.getLed(RIGHT_DIAGONAL));//), DEC);
		bluetooth.print("  ");
		bluetooth.print(led.getLed(RIGHT_REAR));//, DEC);
		bluetooth.println("\n");
		delay(500);

		//bluetooth.println("test");
		delay(100);

	}
}

void testMotor()
{
	while (true)
	{
		//move.goForward();
		//printf("%d   %d\n", move.getCurrentSpeed(LEFT), move.getCurrentSpeed(RIGHT));

		//delay(2000);
		//move.turn_encoder(LEFT);

		delay(2000);
		move.turn_encoder(RIGHT);

		//delay(2000);
		//move.turn_encoder(BACK);


	}
}

void testEncoder()
{
	long positionLeft = -999;
	long positionRight = -999;

	while (true)
	{
		bluetooth.println("test");
		long newLeft, newRight;
		newLeft = move.getEncoderReading(LEFT);
		newRight = move.getEncoderReading(RIGHT);
		if (newLeft != positionLeft || newRight != positionRight) {
			Serial.print("Left = ");
			Serial.print(newLeft);
			Serial.print(", Right = ");
			Serial.print(newRight);
			Serial.println();
			positionLeft = newLeft;
			positionRight = newRight;
		}

	}
}

void testStraight(const PID_MODE& A, char m)
{
	while (true)
	{
		switch (m)
		{
		case 'b':
			bluetooth.print("Left = ");
			bluetooth.print(move.left_encoder.read());
			bluetooth.print(", Right = ");
			bluetooth.println(move.right_encoder.read());
			break;
		default:
			Serial.print("Left = ");
			Serial.print(move.left_encoder.read());
			Serial.print(", Right = ");
			Serial.println(move.right_encoder.read());
			break;
		}
		
		if (led.getLed(LEFT_REAR) + led.getLed(RIGHT_REAR) > frontWallThreshold)
		{
		move.stop();
		break;
		}
		if (control > 1000)
		{
			ledFlag = true;
			PIDFlag = true;
			control = 0;
		}

		if (ledFlag)
		{
			led.measure(ledTime);
			ledFlag = false;
		}

		if (PIDFlag)
		{
			pid.PID(A);
			PIDFlag = false;
		}
	}
}