#include "Motor.h"
#include <iostream>
#include <stdio.h>
#ifdef __linux__ 
#include <unistd.h>
#include <wiringPi.h>
#endif

// defaults
#define _FREQUENCY 60
#define _DEBUG 1

using namespace std;


// constructor
Motor::Motor(int bus, int address, int pin, int channel, int direction) {

	printf(">>> Motor init bus %d address %d, channel %d\n", bus, address, channel);

	// set the properties
	m_frequency = _FREQUENCY;			// pca frequency
	m_bus = bus;						// bus number
	m_address = address;				// bus address
	m_pin = pin;						// channel number wiringPi style
	m_channel = channel;				// servo channel
	m_direction = direction;			// direction
	m_directionFwd = direction;			// forward direction
	m_directionBck = !direction;		// backward direction
	m_speed = 0;						// set to stop

	// initialise the driver
#ifdef __linux__ 
	m_pwm = new PCA9685();
	m_pwm->init(m_bus, m_address);
	m_pwm->setPWMFreq(m_frequency);
	pinMode(m_pin, OUTPUT);
	digitalWrite(m_pin, m_direction);
	m_pwm->setPWM(m_channel, 0, m_speed);
#endif

}

// destructor
Motor::~Motor() {
	printf(">>> Motor channel %d destroy \n", m_channel);
#ifdef __linux__ 
	speed(0);		// stop the motor
	delete m_pwm;
#endif
}

// sets the specified spped 0 - 100
void Motor::speed(int speed)
{
	if (speed < 0) speed = 0;
	if (speed > 100) speed = 100;
	printf(">>> Motor %d speed %d\n", m_pin, speed);
	m_speed = speed;

	int analog_value = (speed / 100.0) * 4095;
	printf(">>> Analog value %d\n", analog_value);

#ifdef __linux__ 
	m_pwm->setPWM(m_channel, 0, analog_value);
#endif
}

// changes direction to forward
void Motor::forward()
{
	printf(">>> Motor %d direction %d\n", m_pin, m_directionFwd);
	m_direction = m_directionFwd;
#ifdef __linux__ 
	digitalWrite(m_pin, m_direction);
#endif
}

// changes direction to backward
void Motor::backward()
{
	printf(">>> Motor %d direction %d\n", m_pin, m_directionBck);
	m_direction = m_directionBck;
#ifdef __linux__ 
	digitalWrite(m_pin, m_direction);
#endif
}

// changes spped to stop
void Motor::stop()
{
	printf(">>> Motor %d stop\n", m_pin);
	m_speed = 0;

#ifdef __linux__ 
	m_pwm->setPWM(m_channel, 0, 0);
#endif
}


int testMotor() {
#ifdef __linux__ 
	if (wiringPiSetup() == -1) { //when initialize wiring failed,print messageto screen
		printf("setup wiringPi failed !");
		return 1;
	}

	Motor m(1, 0x40, 0, 5, 0);
	//	Motor m(1, 0x40, 2, 4, 0);

	printf("Testing Motor\n");

	m.forward();
	m.speed(50);

	usleep(1000 * 1000);
	m.stop();
	usleep(1000 * 1000);
	m.backward();
	m.speed(50);

	usleep(1000 * 1000);
	m.stop();
	usleep(1000 * 1000);
	m.forward();
	m.speed(50);

	for (int i = 10; i <= 110; i = i + 10) {
		m.speed(i);
		usleep(1000 * 1000);
	}

	usleep(1000 * 1000);
	printf("End\n");
#endif

	return 0;
}
