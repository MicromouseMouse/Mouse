// PID.h

#ifndef _PID_h
#define _PID_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include"Led.h"
#include"Movement.h"
#include"Bounce.h"
/*
const double Ki_led = 0;
const double Kp_led = 0.0005;
const double Kd_led =  0.1;


const float Ki_encoder = 0.00;
const float Kp_encoder = 3;
const float Kd_encoder = 5;
*/



enum PID_MODE{LED_MODE,GYRO_MODE,ENCODER_MODE,HYBRID_MODE};

class PIDClass
{
public:
	MovementClass* move;
	LedClass* led;
	double lastErrorLED;
	double lastErrorMotor;
	double integrator;
	elapsedMicros time;

	int encoderOffset;

	bool encoderFlag;

	double Ki_led ;
	double Kp_led ;
	double Kd_led ;


	double Ki_encoder;
	double Kp_encoder;
	double Kd_encoder;

	PIDClass(MovementClass* a, LedClass* b, elapsedMicros & t);

	void PID(const PID_MODE &mode);   // stay in middle by LED_MODE, GYRO_MODE or ENCODER_MODE

	
};



#endif

