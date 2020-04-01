#pragma once

#include <thread>
#include <iostream>
#include <assert.h>
#include <chrono>
#include <future>

/*
 * Class that encapsulates promise and future object and
 * provides API to set exit signal for the thread
 */
class Stoppable
{
	std::promise<void> exitSignal;
	std::future<void> futureObj;
public:
	// constructors
	Stoppable();				// create constructor
//	Stoppable(Stoppable&& obj);	// move constructor

	virtual void run();			// worker loop to be placed here
	bool stopRequested();		//Checks if thread is requested to stop
	void stop();				// Request the thread to stop by setting value in promise object
};

// Script class extends the Stoppable Task - perform a circle
class ScriptCircle : public Stoppable {
	void run();			// worker loop to be placed here
};	
