
#include "Maze.h"
#include "Map.h"
#include <Bounce\Bounce.h>
#include "PID.h"
#include <Encoder\Encoder.h>
#include "Movement.h"
#include "Led.h"
#include <stddef.h>
#include <SoftwareSerial.h>
elapsedMicros time;  // timer for pid to calculate dt
elapsedMicros control;  // timer to reset flag

bool ledFlag = true;
bool PIDFlag = true;
bool slowFlag = false;

volatile bool modeLeft = false;
volatile bool modeRight = false;

const int ledTime = 30; //microsecond
const int baseSpeed = 200;

LedClass led;
MovementClass move(baseSpeed);
PIDClass pid(&move, &led, time);
SoftwareSerial bluetooth(7, 8);

const int frontWallThreshold = 23000;
void testLed();
void testEncoder();
void testMotor();

void setup()
{
	delay(2000);
	bluetooth.begin(9600);
	
	led.init();
	//move.goForward();
	move.resetEncoder();
	control = 0;
	time = 0;
}

void loop()
{
	//testLed();
	testMotor();
	char command;
	int a = analogRead(14);
	if (a > 50000) modeLeft = true;
	else modeLeft = false;
	
	/* mode adjust
	int b = digitalRead(13);   // one use only no pull down resistor
	if (n == HIGH) modeRight = true;
	else modeRight = false;
	*/
	double distance = move.left_encoder.read() + move.right_encoder.read();
	distance = distance * 0.5 * WHEEL_CIRCUMFERENCE / 5760.0;

	if (bluetooth.available())
	{
		command = (char)bluetooth.read();
	}
	switch (command)
	{
	case 'm':
		/*
		int kp = (int)bluetooth.read();
		int kd = (int)bluetooth.read();
		pid.Kp_led = kp;
		pid.Kd_led = kd;
		break;
		*/
	case 'o':
		move.left_encoder.write(0);
		move.right_encoder.write(0);
		move.goForward();

		while (distance < 17.9)
		{
			pid.PID(ENCODER_MODE);
			distance = move.left_encoder.read() + move.right_encoder.read();
			distance = distance * 0.5 * WHEEL_CIRCUMFERENCE / 5760.0;
		}
		move.stop();
		break;
	case 'g':
		move.goForward();
		break;
	case 'r':
		move.turn_encoder(RIGHT);
		break;
	case 'l':
		move.turn_encoder(LEFT);
		break;
	case 's':
		move.stop();
		break;
	case 't':
		bluetooth.print(pid.Kp_led,3);
		bluetooth.print(" ");
		bluetooth.print(pid.Kd_led, 3);
		bluetooth.println(" ");
		bluetooth.println(distance, 5);
		bluetooth.print(move.left_encoder.read(), DEC);
		bluetooth.print(" ");
		bluetooth.println(move.right_encoder.read(), DEC);
	default:
		break;
	}

}
void store()
{
	//else
	{
		double distance = move.left_encoder.read() + move.right_encoder.read();
		distance = distance * 0.5 * WHEEL_CIRCUMFERENCE / 5760.0;
		bluetooth.println(distance, 5);
		bluetooth.print(move.left_encoder.read(), DEC);
		bluetooth.print(" ");
		bluetooth.println(move.right_encoder.read(), DEC);
		//testLed();

		//testGyro();
		//testMotor();
		//testEncoder();

		//else
		{
			unsigned int count = 0;
			if (count < control % 100)
			{
				count = control % 100;
				ledFlag = true;
			}
			if (control > 1000)           // reset flag
			{
				//Serial.println("in control");
				//ledFlag = true;
				PIDFlag = true;
				control = 0;
				count = 0;
			}
			/*
			// Minh's version
			double factor =  (20000 - led.getLed(LEFT_REAR) - led.getLed(RIGHT_REAR) )*1.0  / (1.0*(20000 - led.frontThreshold));
			move.baseSpeed = (baseSpeed - 85)*factor + 85;
			*/

			volatile int front = led.getLed(LEFT_REAR) + led.getLed(RIGHT_REAR);

			if (ledFlag)
			{
				ledFlag = false;
				led.measure(ledTime); 
				// time in microsecond
				/*
				Serial.print(led.left_diagonal, DEC);
				Serial.print(" ");
				Serial.print(led.right_diagonal, DEC);
				Serial.print(" ");
				Serial.print(led.leftThreshold, DEC);
				Serial.print(" ");
				Serial.print(led.rightThreshold, DEC);
				Serial.println(" ");
				*/
			}
			if (PIDFlag)
			{
				PIDFlag = false;
				//pid.PID(LED_MODE);
				pid.PID(ENCODER_MODE);
				/*Serial.println("");
				Serial.print(move.currentSpeedLeft, DEC);
				Serial.print(" ");
				Serial.print(move.currentSpeedRight, DEC);
				Serial.println("");
				*/
			}

			if (front > led.frontThreshold && !slowFlag)
			{
				slowFlag = true;
				move.slow(100);
				move.baseSpeed = 100;
			}

			if (front > frontWallThreshold)
			{
				move.stop();
			}
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
		*/
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

			delay(2000);
			move.turn_encoder(LEFT);

			delay(2000);
			move.turn_encoder(RIGHT);

			delay(2000);
			move.turn_encoder(BACK);


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
