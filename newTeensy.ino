

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
const int baseSpeed = 150;

LedClass led;
MovementClass move(baseSpeed);
PIDClass pid(&move, &led, time);
MazeClass maze(&led, &move);
SoftwareSerial bluetooth(7, 8);

void testLed();
void testEncoder();
void testMotor();

void setup()
{
	delay(12000);
	bluetooth.begin(9600);
	Serial.begin(9600);
	led.init();
	move.turn_encoder(BACK);
	move.resetEncoder();
	led.leftMiddleThreshold = 4000;
	led.rightMiddleThreshold = 4000;
	led.measure(ledTime);
	control = 0;
	time = 0;
	
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



void testSolving()
{
	Serial.println(maze.printFloodFill());
	delay(10000);
	while (true)
	{
		maze.mapping();
		if (move.getDistanceTravel() > 18 * maze.counter - 1.2)
		{
			
			maze.counter++;
			Coordinate next = maze.pastPath.pop();
			Serial.print(maze.curLocation.x);
			Serial.print(" ");
			Serial.print(maze.curLocation.y);
			Serial.print("   ");
			Serial.print(next.x);
			Serial.print(" ");
			Serial.println(next.y);
			Dir temp = getDirToGo(maze.curLocation, next);
			if (temp == maze.curDirection) return;
			else
			{
			
				Serial.print(maze.getTurnDir(temp));
				maze.curDirection = temp;
				Serial.print(maze.curDirection);
			}
		}
		if (led.getLed(LEFT_REAR) + led.getLed(RIGHT_REAR) > led.frontThreshold*0.9)
		{
			digitalWrite(14, HIGH);
			move.resetEncoder();
			maze.counter = 1;
		}
		
	}

	//maze.command();
	delay(500);
}

void testOneWay(const PID_MODE &A)
{

	speed = 0;
	float extraSpace = 0;
	move.resetEncoder();
	while (true)
	{ 
		//maze.randomMapping();
		maze.mapping();
		int check = maze.command();
		if(check == -1) maze.command();
		else if (check == 1)
		{
			delay(1000);
			bluetooth.println(maze.printFloodFill());
			delay(1000);
		}
		
		
		//extraSpace = 2*speed;
		//if (speed > 4000) speed = 4000;
		led.measure(ledTime);
		if (led.getLed(LEFT_REAR) + led.getLed(RIGHT_REAR) > led.frontThreshold*0.65 - extraSpace)
		{
			move.stopForward();
			led.measure(ledTime);
			move.turn_encoder(RIGHT);
			move.resetEncoder();
			led.measure(ledTime);
			delay(1000);
			//move.goForward();
			//bluetooth.println(move.getDistanceTravel(), 3);
			//bluetooth.println(maze.printMap());
			//bluetooth.println(maze.printPath());
			//bluetooth.println(maze.printFloodFill());
			/*
			//maze.simpleTravel();
		if (false)//maze.command() && maze.curLocation == Coordinate(0,9))
		{
			move.stop();
			bluetooth.print(maze.curLocation.x);
			bluetooth.print(" ");
			bluetooth.println(maze.curLocation.y);
			bluetooth.print("  ");
			bluetooth.print(maze.curDirection);
			bluetooth.print("    ");
			bluetooth.print(maze.nextLocation.x);
			bluetooth.print(" ");
			bluetooth.println(maze.nextLocation.y);
			bluetooth.println(maze.printMap());
			bluetooth.println(maze.printFloodFill());
			bluetooth.println(maze.printPath());
			move.resetEncoder();
			bluetooth.println(move.getDistanceTravel(), 3);
			delay(5000);
			move.stop();
			pid.turnFlag = true;
			maze.counter = 1;
			led.measure(ledTime);
			pid.PID(A);
			
		}*/
			/*
			delay(2000);
			pid.turnFlag = true;
			maze.counter = 1;
			led.measure(ledTime);
			if (led.getLed(LEFT_DIAGONAL) < 0.5*led.leftThreshold)
			{
				move.turn_encoder(LEFT);
				maze.curDirection = leftDir(maze.curDirection);
			}
			else if (led.getLed(RIGHT_DIAGONAL) < 0.5*led.rightThreshold)
			{
				move.turn_encoder(RIGHT);
				maze.curDirection = rightDir(maze.curDirection);
			}
			else
			{
				move.turn_encoder(BACK);
				maze.curDirection = opposite(maze.curDirection);
			}
			delay(200);
			*/
			//led.measure(ledTime);
			//speed = 0;
			//pid.PID(A);
		}
		
		
		if (control > 1000)
		{
			PIDFlag = true;
			control = 0;
		}

		if (PIDFlag)
		{
			led.measure(ledTime);
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
		
		/*
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
		delay(100);
		
		//bluetooth.println("test");;
		*/
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
