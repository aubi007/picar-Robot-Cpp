#pragma once

#ifdef __linux__ 
#include "PCA9685.h"
#endif

class Motor {
public:
	// constructors and destructor
	Motor(int bus, int address, int pin, int channel, int direction);
	~Motor();

	void speed(int speed);		// sets the specified spped 0 - 100
	void forward();				// changes direction to forward
	void backward();			// changes direction to backward
	void stop();				// changes spped to stop
#ifdef __linux__ 
	PCA9685* m_pwm;			// PWM driver
#endif

private:
	int m_frequency;		// pca frequency
	int m_bus;				// bus number
	int m_address;			// bus address
	int m_pin;				// channel number wiringPi style
	int m_channel;			// servo channel
	int m_direction;		// direction
	int m_directionFwd;		// forward direction
	int m_directionBck;		// backward direction
	int m_speed;			// motor speed 0-100
};