#pragma once

#ifdef __linux__ 
#include "PCA9685.h"
#endif


class Servo {
public:
	// constructors and destructor
	Servo(int bus, int address, int channel, int offset, int minAngle, int maxAngle);
	Servo(int bus, int address, int channel, int offset);
	~Servo();

	void write(int angle);		// writes specified angle to the servo

private:
	int m_minAngle;			// minimal servo angle, default 0
	int m_maxAngle;			// maximal servo angle, default 180
	int m_frequency;		// pca frequency
	int m_bus;				// bus number
	int m_address;			// bus address
	int m_channel;			// servo channel
	int m_offset;			// angle offset
#ifdef __linux__ 
	PCA9685* m_pwm;			// PWM driver
#endif

	int angleToAnalog(int angle);		// calculates analog value for specified angle applying min angle and max angle to the argument
};