

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
int baseSpeed = 175;

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
	bluetooth.begin(9600);
	Serial.begin(9600);
	/*
	delay(2000);
	delay(6000);
	//delay(6000);
	
	led.init1();
	move.turn_encoder(LEFT);
	maze.stopAlign();
	move.turn_encoder(LEFT);
	move.resetEncoder();
	maze.floodFill(Coordinate(0,0));
	led.init2();
	delay(500);
	control = 0;
	time = 0;
	move.stop();
	//move.goForward();
	*/
}

void loop()
{
	//testMotor();
	//testLed();
	//testEncoder();
	//testMove();
	//testStraight(ENCODER_MODE,'a');
	//testSolving();
	//testOneWay(HYBRID_MODE);
	//testRealMaze(LED_MODE);
	killerMouse();
}

void testMove()
{
	move.resetEncoder();
	move.goForward();
	delayMicroseconds(300);
	int counter = 1;
	while (true)
	{
		
		if (move.getDistanceTravel() > 18.0* 15 - 0.15*15)
		//if(led.left_rear + led.right_rear > 15500)//led.frontThreshold * 0.52)
		{
			int a = move.left_encoder.read();
			int b = move.right_encoder.read();
		    move.stopForward();
			//maze.stopAlign();
			bluetooth.println(maze.printMap());
			//maze.stopAlign();
			bluetooth.print(a);
			bluetooth.print(" ");
			bluetooth.println(b);
			bluetooth.println(move.getDistanceTravel());
			delay(10000);
		}
		else if (move.getDistanceTravel() > 18.0* counter - 6)
		{
			analogWrite(13, 1023);
			++counter;
			maze.updateMap();
			analogWrite(13, 0);
		}
		led.measure(10);
		pid.PID(HYBRID_MODE);
		delayMicroseconds(3000);
	}
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
	move.resetEncoder();
	while (true)
	{ 
		if (control > 3000)
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
		
		maze.mapping();
		int check = maze.command(0);
		if(check == -1) check=maze.command(true);
		if (check == 7)
		{
			
			delay(300);
			bluetooth.print(maze.curLocation.x);
			bluetooth.print(" ");
			bluetooth.println(maze.curLocation.y);
			
			bluetooth.println(maze.printFloodFill());
			bluetooth.println(maze.printMap());
			delay(300);
			check = maze.command(true);
		}
	
		
		led.measure(ledTime);
		if (led.getLed(LEFT_REAR) + led.getLed(RIGHT_REAR) > led.frontThreshold*0.75 )
		{
			move.stopForward();
			maze.stopAlign();
			led.measure(ledTime);
			/*
			bluetooth.println("in this loop\n");
			bluetooth.print(maze.curLocation.x);
			bluetooth.print(" ");
			bluetooth.println(maze.curLocation.y);
			bluetooth.println(maze.printFloodFill());
			bluetooth.println(maze.printMap());
			*/
			if (check == 0) check = maze.command(true);
			led.measure(ledTime);

				
			//delay(1000);
			//move.goForward();
			//bluetooth.println(move.getDistanceTravel(), 3);
			//bluetooth.println(maze.printMap());
			//bluetooth.println(maze.printPath());
			//bluetooth.println(maze.printFloodFill());
			pid.PID(A);
		}
		analogWrite(13, 0);
	}
	}
	

void testLed()
{
	move.goForward();
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
		delay(100);
		

		//bluetooth.println("test");;
		
	}
}

void testMotor()
{
	move.resetEncoder();
	while (true)
	{
		//move.goForward();
		//printf("%d   %d\n", move.getCurrentSpeed(LEFT), move.getCurrentSpeed(RIGHT));

		delay(2000);
		move.turn_encoder(LEFT);

		delay(2000);
		move.turn_encoder(RIGHT);

		delay(2000);
		move.turn_encoder(BACK);

		delay(2000);
		move.turn_encoder(DIAGONAL_RIGHT);

		delay(2000);
		move.turn_encoder(DIAGONAL_LEFT);

		//delay(2000);
		//move.curveTurn(RIGHT);
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

void killerMouse()
{
	char c;
	if (bluetooth.available())
	{
		c = bluetooth.read();
		switch (c)
		{
		case 'u':
			move.baseSpeed = 350;
			break;
		case 'd':
			move.baseSpeed = 450;
			break;
		case 'f':
			control = 0;
			move.goForward();
			while (control< 1000000)
			{
				pid.PID(ENCODER_MODE);
				delay(1);
			}
			break;
		case 'q':
			move.goBackward(150,150);
			break;
		case 's':
			move.stopForward();
			break;
		case 'k':
			move.baseSpeed = 900;
			control = 0;
			while (control< 1000000)
			{
				pid.PID(ENCODER_MODE);
				delay(1);
			}
			move.baseSpeed = 175;
			move.stopForward();
			break;
		case 'j':
			for (int i = 0; i < 3; i++)
			{
				analogWrite(LEFT_FORWARD, 350);
				analogWrite(LEFT_BACKWARD, 0);
				analogWrite(RIGHT_FORWARD, 0);
				analogWrite(RIGHT_BACKWARD, 350);
				delay(200);
				analogWrite(LEFT_FORWARD, 0);
				analogWrite(LEFT_BACKWARD, 350);
				analogWrite(RIGHT_FORWARD, 350);
				analogWrite(RIGHT_BACKWARD, 0);
				delay(200);
			}
			break;
		case 'l':
			analogWrite(LEFT_FORWARD, 0);
			analogWrite(LEFT_BACKWARD, 140);
			analogWrite(RIGHT_FORWARD, 140);
			analogWrite(RIGHT_BACKWARD, 0);
			break;
		case 'r':
			analogWrite(LEFT_FORWARD, 140);
			analogWrite(LEFT_BACKWARD, 0);
			analogWrite(RIGHT_FORWARD, 0);
			analogWrite(RIGHT_BACKWARD, 140);
			break;
		case 'b':
			move.turn_encoder(BACK);
			break;

		}
	}
}