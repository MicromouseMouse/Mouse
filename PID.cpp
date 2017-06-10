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
	lastErrorMotor = 0;
	lastErrorLED = 0;
	integrator = 0;
	Ki_led = 0;
	Kp_led = 0.008;// 0.015;
	Kd_led = 2;// 15;

	Ki_encoder = 0;
	Kp_encoder = 0.03;
	Kd_encoder = 10;

	encoderFlag = false;
	encoderOffset = 0;
	
}

void PIDClass::PID(const PID_MODE &mode)   // stay in middle by led
{
	int range = 6;
	double error =0;
	double errorM = 0;
	double pError;
	double dError;
	double iError=0;
	int leftF = led->left_middle;
	int rightF = led->right_middle;
	int leftD = led->left_diagonal;
	int rightD = led->right_diagonal;
	
	if (leftD < led->leftThreshold * 1.015 && leftD >led->leftThreshold *0.985) leftD = led->leftThreshold;
	else if (leftD > led->leftThreshold * 1.015) leftD -= 0.015*led->leftThreshold;
	else leftD += 0.015*led->leftThreshold;
	
	if (rightD < led->rightThreshold * 1.04 && rightD > led->rightThreshold*0.96) rightD = led->rightThreshold;
	else if (rightD > led->rightThreshold * 1.04) rightD -= 0.04*led->rightThreshold;
	else if (rightD < led->rightThreshold *0.96) rightD += 0.04*led->rightThreshold;
	else if (rightD > led->rightThreshold * 1.1) rightD = led->rightThreshold * 1.08;
	else rightD = led->rightThreshold * 0.92;

	switch (mode)
	{
	case LED_MODE:
		
		
		if (leftF > 4200 * 0 && leftD > led->leftThreshold*0.7) //wall left
		{
			error = (leftD - led->leftThreshold) ;
		}
		else if (rightF > 4200 * 0 && rightD > led->rightThreshold*0.7)  //wall right
		{
			error = (led->rightThreshold - rightD);
		}
		else
		{
		PID(ENCODER_MODE);
		return;
		}	// no wall will switch to encoder
		
		

		//error = (led->rightThreshold - rightD);
		//error = (leftD - led->leftThreshold) ;
		//error = leftD - rightD - led->offsetLed + 3000;
		//error = (led->rightThreshold- rightD)*0.015 + 50;
		//error = (leftD - led->leftThreshold)*0.1 + 20 ;
		//error = (leftD - rightD - led->offsetLed)*0.008 + 100;
		encoderOffset = 0;
		pError = Kp_led* error;
		dError = Kd_led* (error - lastErrorLED) / (time*0.003);
		iError = Ki_led* integrator * time * 0.003;
		break;

	case ENCODER_MODE:
		errorM = 1.0058*move->right_encoder.read() - move->left_encoder.read() - encoderOffset;  // positive steer to the RIGHT
		pError = Kp_encoder* errorM;
		dError = Kd_encoder* (errorM - lastErrorMotor) / (time *0.003);
		iError = Ki_encoder* integrator * time *0.003;
		break;
	case HYBRID_MODE:
		
		if (leftF > 3600 * 0 && leftD > led->leftThreshold*0.9) //wall left
		{
			error = (leftD - led->leftThreshold);
		}
		else if (rightF > 3600 * 0 && rightD > led->rightThreshold*0.85)  //wall right
		{
			error = (led->rightThreshold - rightD);
		}
		else
		{
			PID(ENCODER_MODE);
			return;
		}
		errorM = 1.0061*move->right_encoder.read() - move->left_encoder.read() - encoderOffset;

		pError = 0.7*Kp_led* error + 0.3*Kp_encoder*errorM;
		dError = 1*Kd_led* (error - lastErrorLED) / (time*0.003) + 0.3*Kd_encoder* (errorM - lastErrorMotor) / (time *0.003);
		//iError = Ki_led* integrator * time * 0.003;

		break;
	}
	
	time = 0;
	lastErrorLED = error;
	lastErrorMotor = errorM;
	integrator += error * (time *0.003);
	
	double fix = pError + dError +iError;
	move->currentSpeedLeft = move->baseSpeed + fix;
	move->currentSpeedRight = move->baseSpeed - fix;
	
	if (move->currentSpeedLeft < (move->baseSpeed *(10-range)*0.1 )) move->currentSpeedLeft = move->baseSpeed * (10-range) *0.1;
	else if (move->currentSpeedLeft > (move->baseSpeed * (10+range) *0.1)) move->currentSpeedLeft = move->baseSpeed * (10 + range)*0.1;

	if (move->currentSpeedRight  < (move->baseSpeed *(10 - range) *0.1)) move->currentSpeedRight = move->baseSpeed *(10 - range) *0.1;
	else if (move->currentSpeedRight  > (move->baseSpeed * (10 + range) *0.1)) move->currentSpeedRight = move->baseSpeed * (10 + range) *0.1;
	
	move->goForward(move->currentSpeedLeft, move->currentSpeedRight);
	/*
	Serial.println(errorM);
	Serial.print(pError);
	Serial.print(" ");
	Serial.println(dError);
	Serial.print(" ");
	Serial.println(iError);
	Serial.println(fix);
	Serial.println("");
	*/

}

