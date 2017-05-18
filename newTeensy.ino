

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
elapsedMillis speed;
bool ledFlag = true;
bool PIDFlag = true;
bool slowFlag = false;
const int MIN_SPEED = 85;

volatile bool modeLeft = false;
volatile bool modeRight = false;

const int ledTime = 10; //microsecond
const int baseSpeed = 125;

LedClass led;
MovementClass move(baseSpeed);
PIDClass pid(&move, &led, time);
MazeClass maze(&led, &move);
SoftwareSerial bluetooth(7, 8);

const int frontWallThreshold = 17500;
void testLed();
void testEncoder();
void testMotor();
void testStraight(const PID_MODE& A, char m);

void setup()
{
	delay(7000);
	bluetooth.begin(9600);
	Serial.begin(9600);
	led.init();
	move.turn_encoder(BACK);
	move.resetEncoder();
	led.leftMiddleThreshold = 4000;
	led.rightMiddleThreshold = 4000;
	control = 0;
	time = 0;
	led.measure(ledTime);
	//move.goForward();
}

void loop()
{
	//testLed();
	//testEncoder();
	//testMotor();
	//testStraight(ENCODER_MODE,'a');
	//testSolving();
	testOneWay(LED_MODE);
	//testRealMaze(LED_MODE);
}

void testRealMaze(const PID_MODE &A)
{
	speed = 0;
	float extraSpace = 0;
	while (true)
	{ 
		maze.mapping();
		extraSpace = speed * 4.5;
		if (led.getLed(LEFT_REAR) + led.getLed(RIGHT_REAR) > frontWallThreshold - extraSpace)
		{
			move.stopForward();
			maze.counter = 1;
			led.measure(ledTime);
			//maze.updateMap();
			//maze.floodFill(Coordinate(8, 8));
			//maze.command();
			
			move.baseSpeed = baseSpeed;
			delay(500);
			speed = 0;
			continue;
		}


		if (control > 500)
		{
			ledFlag = true;
			PIDFlag = true;
			control = 0;
		}
		else if (control > 330)
		{
			ledFlag = true;
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


void testSolving()
{
	maze.floodFill(Coordinate(8,8));
	//maze.command();
	delay(10000);
}

void testOneWay(const PID_MODE &A)
{
	speed = 0;
	float extraSpace = 0;
	while (true)
	{ 
		maze.mapping();
		extraSpace = 0;
		if (speed > 2000) speed = 2000;
		if (led.getLed(LEFT_REAR) + led.getLed(RIGHT_REAR) > led.frontThreshold*0.75 - extraSpace)
		{
			move.stopForward();
			//bluetooth.println(maze.printMap());
			//bluetooth.println(maze.printPath());
			//bluetooth.println(maze.printFloodFill());
			delay(2000);
			pid.turnFlag = true;
			maze.counter = 1;
			led.measure(ledTime);
			if (led.getLed(LEFT_DIAGONAL) < 0.5*led.leftThreshold)
			{
				move.turn_encoder(LEFT);
				maze.curDirection = leftDir(maze.curDirection);
				led.measure(ledTime);
			}
			else if (led.getLed(RIGHT_DIAGONAL) < 0.5*led.rightThreshold)
			{
				move.turn_encoder(RIGHT);
				maze.curDirection = rightDir(maze.curDirection);
				led.measure(ledTime);
			}
			else
			{
				move.turn_encoder(BACK);
				maze.curDirection = opposite(maze.curDirection);
				led.measure(ledTime);
			}
			move.baseSpeed = baseSpeed;
			delay(500);
			speed = 0;
			continue;
		}
		
		
		if (control > 3000)
		{
			ledFlag = true;
			PIDFlag = true;
			control = 0;
		}
		else if (control > 330)
		{
			ledFlag = true;
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
		bluetooth.print(led.getLed(LEFT_REAR));//, DEC);
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

		//delay(2000);
		//move.turn_encoder(RIGHT);

		//delay(2000);
		//move.turn_encoder(BACK);

		//delay(2000);
		//move.turn_encoder(DIAGONAL_RIGHT);

		delay(2000);
		move.curveTurn(RIGHT);
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
			bluetooth.print(led.getLed(LEFT_DIAGONAL));
			bluetooth.print(" ");
			bluetooth.println(led.getLed(RIGHT_DIAGONAL));
			//bluetooth.print("Left = ");
			//bluetooth.print(move.left_encoder.read());
			//bluetooth.print(", Right = ");
			//bluetooth.println(move.right_encoder.read());
			//bluetooth.print(move.currentSpeedLeft);
			//bluetooth.print(" ");
			//bluetooth.println(move.currentSpeedRight);
			break;
		case 's':
			Serial.print("Left = ");
			Serial.print(move.left_encoder.read());
			Serial.print(", Right = ");
			Serial.println(move.right_encoder.read());
			break;
		default:
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