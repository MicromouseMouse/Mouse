// 
// 
// 
#include "PID.h"
#include <SoftwareSerial.h>

SoftwareSerial BT(7, 8);

PIDClass::PIDClass(MovementClass * a, LedClass* b, elapsedMicros & t)
{
	move = a;
	led = b;
	time = t;
	lastError = 0;
	integrator = 0;
	Ki_led = 0;
	Kp_led = 2;
	Kd_led = 60;

	Ki_encoder = 0;
	Kp_encoder = 1;
	Kd_encoder = 0;

	encoderFlag = false;
	encoderOffset = 0;
	turnFlag = false;
}

void PIDClass::PID(const PID_MODE &mode)   // stay in middle by led
{
	int range = 3;
	double error =0;
	double pError;
	double dError;
	double iError=0;
	int leftF = led->left_middle;
	int rightF = led->right_middle;
	int leftD = led->left_diagonal;
	int rightD = led->right_diagonal;
	
	switch (mode)
	{
	case LED_MODE:
		
		if ((leftF > led->leftMiddleThreshold*0.75 && leftD > led->leftThreshold*0.75)
			&& (rightF > led->rightMiddleThreshold*0.75 && rightD > led->rightThreshold*0.75)
			&& turnFlag == false) // wall both side
		{
		error = leftD - rightD - led->offsetLed;
		}
		else if (leftF > led->leftMiddleThreshold*0.75 && leftD > led->leftThreshold*0.75) //wall left
		{
			error = leftD - led->leftThreshold +5000;//-200;
		}
		else if (rightF > led->rightMiddleThreshold*0.75 && rightD > led->rightThreshold*0.75)  //wall right
		{
			error = led->rightThreshold - rightD -5000;//+1000;
		}
		else
		{
			if (!encoderFlag)
			{
				encoderFlag = true;
				encoderOffset = move->right_encoder.read() -  move->left_encoder.read();
			}
		PID(ENCODER_MODE);
		return;
		}	// no wall will switch to encoder
		

		//error = led->rightThreshold- rightD;
		//error = leftD - led->leftThreshold;
		encoderFlag = false;
		encoderOffset = 0;
		pError = Kp_led* error;
		dError = Kd_led* (error - lastError) / (time*0.001);
		//iError = Ki_led* integrator * time / 1000.0;
		break;

	case ENCODER_MODE:
		error = move->right_encoder.read() - move->left_encoder.read() - encoderOffset;  // positive steer to the left
		pError = Kp_encoder* error;
		dError = Kd_encoder* (error - lastError) / (time *0.001);
		//iError = Ki_encoder* integrator * time *0.001;
		break;
	}
	
	time = 0;
	lastError = error;
	//integrator += error * (time *0.001);

	double fix = pError + dError +iError;
	move->currentSpeedLeft = move->baseSpeed + fix;
	move->currentSpeedRight = move->baseSpeed - fix;

	

	if (move->currentSpeedLeft < (move->baseSpeed *(10-range)*0.1 )) move->currentSpeedLeft = move->baseSpeed * (10-range) *0.1;
	else if (move->currentSpeedLeft > (move->baseSpeed * (10+range) *0.1)) move->currentSpeedLeft = move->baseSpeed * (10 + range)*0.1;

	if (move->currentSpeedRight  < (move->baseSpeed *(10 - range) *0.1)) move->currentSpeedRight = move->baseSpeed *(10 - range) *0.1;
	else if (move->currentSpeedRight  > (move->baseSpeed * (10 + range) *0.1)) move->currentSpeedRight = move->baseSpeed * (10 + range) *0.1;
	
	move->goForward(move->currentSpeedLeft, move->currentSpeedRight);

}

