// 
// 
// 
#include "PID.h"
PIDClass::PIDClass(MovementClass * a, LedClass* b, elapsedMicros & t)
{
	move = a;
	led = b;
	time = t;
	lastError = 0;
	integrator = 0;
	Ki_led = 0;
	Kp_led = 0.5;
	Kd_led = 120;

	Ki_encoder = 0;
	Kp_encoder = 0.1;
	Kd_encoder = 5;
}

void PIDClass::PID(const PID_MODE &mode)   // stay in middle by led
{
	int range = 5;
	double error =0;
	double pError;
	double dError;
	double iError=0;
	switch (mode)
	{
	case LED_MODE:


		if (led->getLed(LEFT_DIAGONAL) > WALL_LEFT && led->getLed(RIGHT_DIAGONAL) > WALL_RIGHT) // wall both side
		{
			error = led->getLed(LEFT_DIAGONAL) - led->getLed(RIGHT_DIAGONAL) - led->offsetLed;
		}
		else if (led->getLed(LEFT_DIAGONAL) > WALL_LEFT ) //wall left
		{
			error = led->getLed(LEFT_DIAGONAL) - led->leftThreshold;
		}
		else if (led->getLed(RIGHT_DIAGONAL) > WALL_RIGHT )  //wall right
		{
			error = led->rightThreshold - led->getLed(RIGHT_DIAGONAL);
		}
		else
		{
			PID(ENCODER_MODE);
			return;
		}					// no wall will switch to encoder
		
		pError = Kp_led* error;
		dError = Kd_led* (error - lastError) / (time/1000.0);
		//iError = Ki_led* integrator * time / 1000.0;
		break;

	case ENCODER_MODE:
		error = move->getEncoderReading(RIGHT) - move->getEncoderReading(LEFT) ;  // positive steer to the left
		pError = Kp_encoder* error;
		dError = Kd_encoder* (error - lastError) / (time / 1000.0);
		iError = Ki_encoder* integrator * time / 1000.0;
		break;
	}

	time = 0;
	lastError = error;
	integrator += error * (time / 1000.0);

	double fix = pError + dError +iError;
	move->currentSpeedLeft = move->baseSpeed + fix;
	move->currentSpeedRight = move->baseSpeed - fix;

	if (move->currentSpeedLeft < (move->baseSpeed *(10-range)/ 10 )) move->currentSpeedLeft = move->baseSpeed * (10-range) / 10;
	else if (move->currentSpeedLeft > (move->baseSpeed * (10+range) / 10) ) move->currentSpeedLeft = move->baseSpeed * (10 + range)/ 10;

	if (move->currentSpeedRight  < (move->baseSpeed *(10 - range) / 10)) move->currentSpeedRight = move->baseSpeed *(10 - range) / 10;
	else if (move->currentSpeedRight  > (move->baseSpeed * (10 + range) / 10)) move->currentSpeedRight = move->baseSpeed * (10 + range) / 10;
	
	move->goForward(move->currentSpeedLeft, move->currentSpeedRight);
}

