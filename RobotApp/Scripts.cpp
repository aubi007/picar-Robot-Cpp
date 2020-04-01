#include "RobotApp.h"


// parameter validation exception
struct InvalidParameterException : std::exception {
	const char* m_message;
	InvalidParameterException(const char* message) { m_message = message; }
	const char* what() const noexcept { return m_message; }
};

/*
 * Class that enables an automation script to be executed in a dedicated thread
 * provides API to set exit signal for the thread
 */
// default constructor - should never be used
Script::Script() : futureObj(exitSignal.get_future())
{
	init(NULL);
}

// constructor 
Script::Script(RobotApp *robotApp) : futureObj(exitSignal.get_future())
{
	// super clas init
	init(robotApp);
}

// super constructor to initialize protected class member variables
void Script::init(RobotApp* robotApp)
{
	// make compiler happy - initialize class member attributes
	m_tickDuration = 0;
	m_tick = 0;
	m_tickBegin = std::chrono::system_clock::now();

	// store the RoborApp class pointer - the robot API
	m_robotApp = robotApp;
}

// empty procedures for virtual workers
void Script::run() 
{
	std::cerr << "!!! Not implemented" << std::endl; 
}

//Checks if thread is requested to stop
bool Script::stopRequested()
{
	// checks if value in future object is available
	if (futureObj.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout)
		return false;
	return true;
}

// Request the thread to stop by setting value in promise object
void Script::stop()
{
	exitSignal.set_value();
}

// tells the robot to go to the initial state
void Script::goToInitialState()
{
	// front wheels to go straight forward position
	m_robotApp->setFWPos(0);

	// camera center position
	m_robotApp->setCamPan(0);
	m_robotApp->setCamTilt(0);

	// back wheels to go forward
	m_robotApp->setBWStatus(1);

	// motors full stop
	m_robotApp->setSpeed(0);
}

// initializes the tick counter with a tick duration
void Script::tickCounterInit(int duration)
{
	// copy the tick duration
	m_tickDuration = duration;

	// set the tick to initial tick number 0
	m_tick = 0;
}

// fetches and stores the current time at the beginning of the script tick loop
void Script::tickCounterBegin()
{
	// get current timestamp
	m_tickBegin = std::chrono::system_clock::now();

	std::cout << ">>> TICK " << m_tick << std::endl;
}

// sleeps the thread until the next tick to be awaken
void Script::tickCounterSleep()
{
	// increase tick counter
	m_tick++;

//	std::chrono::duration<double> diff = std::chrono::system_clock::now() - m_tickBegin;
//	std::cout << ">>> Tick lasted " << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << std::endl;
	
	// sleep until the end of tick
	std::this_thread::sleep_until(m_tickBegin + std::chrono::milliseconds(m_tickDuration));
}



/*
* Script class extends the Script Task - perform a circle
*/
ScriptCircle::ScriptCircle(RobotApp* robotApp, std::string radius)
{
	// convert string to int
	int rad = std::stoi(radius);

	// input validation
	if (abs(rad) < 10) throw (InvalidParameterException("Radius too small, must be at least 10 cm"));
	if (abs(rad) > 200) throw (InvalidParameterException("Radius too big, must be less than 201 cm"));

	// super class "constructor"
	init(robotApp);					
	
	// store the radius - cm to mm conversion, ignore sign
	m_radius = abs(rad) * 10;	
	
	// store the direction -1 .. left, 1 .. right
	m_direction = (rad < 0) ? -1 : 1;
}

void ScriptCircle::run()
{
	std::cout << ">>> Task Start" << std::endl;

	// parse config
	double wheelbase = m_robotApp->getConfig().getWheelbase();		// wheelbase in mm
	double tickdist50 = m_robotApp->getConfig().get100tickdist50();	// distance (mm) the car moves in 100 ticks = 10 seconds on speed 50

	// calculate the path
	double pi = 3.14159265358979323846;
	int angle = 180 * asin(wheelbase / m_radius) / pi;
	int path = 2 * pi * m_radius;
	int ticks = (path / tickdist50) * 100.0;
	
	std::cout << ">>> For radius " << m_radius << " wheelbase " << wheelbase << " the angle is " << angle << " making path " << path << " mm taking " << ticks << " ticks." << std::endl;

	// initalize tick counter for 100 ms ticks
	tickCounterInit(100);

	// Check if thread is requested to stop ?
	while (stopRequested() == false)
	{
		// begin the loop
		tickCounterBegin();

		if (m_tick == 0) {
			// initialize bot state for the algorithm
			m_robotApp->setFWPos(m_direction*angle);
			m_robotApp->setCamPan(m_direction*angle);
			m_robotApp->setCamTilt(0);
			m_robotApp->setBWStatus(1);
			m_robotApp->setSpeed(50);
		}

		// stop after calculated count of ticks
		if (m_tick == ticks) m_robotApp->setSpeed(0);

		// reset loop in 100 ticks after stop
		if (m_tick == ticks + 100) m_tick = -1;

		// sleep until the end of tick
		tickCounterSleep();
	}

	// initialize bot state
	goToInitialState();

	std::cout << "Task End" << std::endl;
}

// constructor
ScriptSnail::ScriptSnail(RobotApp* robotApp)
{
	// super class "constructor"
	init(robotApp);
}

/*
* Script class extends the Script Task - perform a snail move
*/
void ScriptSnail::run()
{
	std::cout << "Task Start" << std::endl;

	// initalize tick counter for 100 ms ticks
	tickCounterInit(100);

	// Check if thread is requested to stop ?
	while (stopRequested() == false)
	{
		// begin the loop
		tickCounterBegin();

		if (m_tick == 0) {
			// initialize bot state
			goToInitialState();
		}

//		if (m_tick == 20) m_robotApp->setBWStatus(0);

		if (m_tick == 0) m_robotApp->setSpeed(50); // turn right
		if (m_tick == 5) m_robotApp->setFWPos(30); // turn right
		if (m_tick == 10) m_robotApp->setFWPos(0); // straight ahead
		if (m_tick == 15) m_robotApp->setSpeed(100); // hurry
		if (m_tick == 20) m_robotApp->setSpeed(0); // stop
		if (m_tick == 25) {						// go back
			m_robotApp->setBWStatus(-1);
			m_robotApp->setSpeed(100);
		}
		if (m_tick == 30) m_robotApp->setSpeed(50); // slow down
		if (m_tick == 35) m_robotApp->setFWPos(30); // turn right
		if (m_tick == 40) m_robotApp->setFWPos(0); // straight ahead
		if (m_tick == 45) m_robotApp->setSpeed(0); // stop

		// reset loop in 100 ticks after stop
		if (m_tick == 100) m_tick = -1;

		// sleep until the end of tick
		tickCounterSleep();
	}

	// initialize bot state
	goToInitialState();

	std::cout << "Task End" << std::endl;
}