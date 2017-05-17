// QuadEncoder.h

#ifndef _QUADENCODER_h
#define _QUADENCODER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class QuadEncoderClass
{
 protected:


 public:
	void init();
};

extern QuadEncoderClass QuadEncoder;

#endif

