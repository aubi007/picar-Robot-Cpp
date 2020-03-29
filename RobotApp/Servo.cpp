#include "Servo.h"
#include <iostream>
#include <stdio.h>
#ifdef __linux__ 
#include <unistd.h>
#endif

// defaults
#define _MIN_ANGLE 0				// minimal TECHNICAL angle of the servo
#define _MAX_ANGLE 180				// maximal TECHNICAL angle of the servo
#define _MIN_PULSE_WIDTH 600
#define _MAX_PULSE_WIDTH 2400
#define _DEFAULT_PULSE_WIDTH 1500
#define _FREQUENCY 60
#define _DEBUG 1

using namespace std;

// constructor
Servo::Servo(int bus, int address, int channel, int offset, int minAngle, int maxAngle) {

	printf(">>> Servo init bus %d address %d, channel %d\n", bus, address, channel);

	// set the properties
	m_minAngle = minAngle;				// minimal servo angle
	m_maxAngle = maxAngle;				// maximal servo angle
	m_frequency = _FREQUENCY;			// pca frequency
	m_bus = bus;						// bus number
	m_address = address;				// bus address
	m_channel = channel;				// servo channel
	m_offset = offset;					// channel offset

	// initialise the driver
#ifdef __linux__ 
	m_pwm = new PCA9685();
	m_pwm->init(m_bus, m_address);
	m_pwm->setPWMFreq(m_frequency);
#endif

}

// simplified constructor
Servo::Servo(int bus, int address, int channel, int offset) : Servo(bus, address, channel, offset, _MIN_ANGLE, _MAX_ANGLE) {};

// destructor
Servo::~Servo() {
	printf(">>> Servo channel %d destroy \n", m_channel);
#ifdef __linux__ 
	delete m_pwm;
#endif
}

// calculates analog value for specified angle applying min angle and max angle to the argument
int Servo::angleToAnalog(int angle) {
	// printf(">>> Angle: %d, offset %d\n", angle, m_offset);
    // allow servo angle m_minAngle .. m_maxAngle degrees only
    if (angle < m_minAngle) angle = m_minAngle;
    if (angle > m_maxAngle) angle = m_maxAngle;

    // calculate pulse wide
    int pulse_wide = ((angle - _MIN_ANGLE) * (_MAX_PULSE_WIDTH - _MIN_PULSE_WIDTH)) / (_MAX_ANGLE - _MIN_ANGLE) + _MIN_PULSE_WIDTH;

	// calculate the analog value
    int analog_value = (float)pulse_wide / 1000000 * m_frequency * 4096;
    printf (">>> For angle %d analog_value %d\n", angle, analog_value);

    return (analog_value);
}

// writes specified angle to the servo
void Servo::write(int angle) {
	// calculate analog value for the angle and apply offset to the angle
	int analog = angleToAnalog(angle + m_offset);
#ifdef __linux__ 
	m_pwm->setPWM(m_channel, 0, analog);
#endif
}


int testServo () {
#define ch 0 // kanal 
	Servo servo(1, 0x40, ch, 0, 50, 130);

	printf("Testing servo\n");
	servo.write(90);

	for (int i = 10; i < 170; i = i + 10) {
		printf ("angle %d ", i);
		servo.write(i);
#ifdef __linux__ 
		usleep(1000 * 1000);
#endif
	}

	servo.write(90);

	printf("turning off servo\n");
#ifdef __linux__ 
	servo.m_pwm->setPWM(ch, 0, 4096);
#endif
	return 0;
} 
