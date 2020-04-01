#pragma once

#include <iostream>
#include <stdio.h>
#include <thread>
#include <assert.h>
#include <chrono>
#include <future>
#include <time.h>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "Servo.h"
#include "Motor.h"

/*
* Structure that holds robot status
*/
struct RobotStatus {
    int bw_status;                      // back wheel status 0 - stop, 1 - forward, -1 - backward
    int speed;                          // back wheels speed 0 - 100
    int fw_pos;                         // front wheel position 0 - steady, <0 - left, >0 - right
    int cam_pan;                        // camera pan 0 - steady, <0 - left >0 - right
    int cam_tilt;                       // camera tilt 0 - steady,
};

/*
* Structure that holds last captured frame with metadata
*/
struct Frame {
    std::vector<uchar> m_data;          // JPEG image as uchar vector (python list)
    clock_t m_ts;                       // frame capture timestamp
};

/*
* Video stream class
*/
class Stream
{
public:
    Stream();                           // constructor
    ~Stream();                          // destructor
    std::vector<uchar> getFrameData(RobotStatus status);  // gets the most current frame as a JPEG image and draw in the bot status
    clock_t getFrameTS();               // gets the current frame capture timestamp

private:
    std::thread m_capture_thread;       // video capture thread
    cv::VideoCapture m_cap;             // video capture device
    cv::Mat m_frame_data;               // last processed frame as CV::Mat
    clock_t m_frame_ts;                 // frame capture timestamp

    void capture();                     // capture frame from the default camera
    void captureLoop();                 // infinite loop supposed to run capture in a dedicated thread
};

/*
* RobotApp Configuration class
*/
class Config {
public:
    // constructors and destructors
    Config();

    void update();              // updates the configuration file

    // getters
    int getFwChannel();
    int getFwOffset();
    int getFwMinAngle();
    int getFwMaxAngle();

    int getCamPanChannel();
    int getCamPanOffset();
    int getCamPanMinAngle();
    int getCamPanMaxAngle();

    int getCamTiltChannel();
    int getCamTiltOffset();
    int getCamTiltMinAngle();
    int getCamTiltMaxAngle();

    int getRightWheelPin();
    int getRightWheelChannel();
    int getRightWheelDirection();

    int getLeftWheelPin();
    int getLeftWheelChannel();
    int getLeftWheelDirection();

    int getWheelbase();
    int get100tickdist50();


    // setters 
    void setFwOffset(int off);
    void setCamPanOffset(int off);
    void setCamTiltOffset(int off);

private:
    int m_fwChannel;            // front wheels servo PCA channel
    int m_fwOffset;             // front wheels servo offset
    int m_fwMinAngle;           // front wheels minimal angle
    int m_fwMaxAngle;           // front wheels maximal angle

    int m_camPanChannel;        // camera pan servo PCA channel
    int m_camPanOffset;         // camera pan servo offset
    int m_camPanMinAngle;       // camera pan minimal angle
    int m_camPanMaxAngle;       // camera pan maximal angle

    int m_camTiltChannel;       // camera tilt servo PCA channel
    int m_camTiltOffset;        // camera tilt servo offset
    int m_camTiltMinAngle;      // camera tilt minimal angle
    int m_camTiltMaxAngle;      // camera tilt maximal angle

    int m_rightWheelPin;        // right wheel wiringPi pin
    int m_rightWheelChannel;    // right wheel PCA channel
    int m_rightWheelDirection;  // right wheel forward direction

    int m_leftWheelPin;         // left wheel wiringPi pin
    int m_leftWheelChannel;     // left wheel PCA channel
    int m_leftWheelDirection;   // left wheel forward direction

    int m_wheelbase;            // wheelbase (mm)
    int m_100tickdist50;        // distance (mm) the car moves in 100 ticks = 10 seconds on speed 50
};


// forward declaration of the Script class
class Script;

/*
* Main RobotApp class with all input and output API
*/
class RobotApp {
public:
    RobotApp();                         // constructor
    ~RobotApp();                        // destructor
    Config getConfig();                 // gets the config class
    Frame getFrame();                   // gets the most current frame as a JPEG image
    void setBWStatus(int s);            // sets back wheel status
    void setSpeed(int s);               // sets the speed
    void setFWPos(int s);               // sets front wheel position
    void camReady();                    // moves the camera to initial position
    void setCamPan(int s);              // sets the camera pan
    void setCamTilt(int s);             // sets the camera tilt
    void screenshot();                  // creates screenshot
    void run(int script, std::string param1, std::string param2);    // runs selected automation script
    void stop();                        // stops any automation script execution
    void calibration();                 // setups calibration
    void calibration_ok();              // confirms calibration
    void calibration_camup();           // calibration camera up
    void calibration_camdown();         // calibration camera down
    void calibration_camleft();         // calibration camera left
    void calibration_camright();        // calibration camera right
    void calibration_fwleft();          // calibration front wheels left
    void calibration_fwright();         // calibration front wheels right
    void calibration_bwleft();          // calibration back wheels left
    void calibration_bwright();         // calibration back wheels right

private:
    Config m_config;                    // robot config
    Stream m_stream;                    // video stream class
    RobotStatus m_status;               // robot status structure
    Servo* m_fw;                        // front wheels servo
    Servo* m_camPan;                    // camera pan servo
    Servo* m_camTilt;                   // camera tilt servo
    Motor* m_rightWheel;                // right wheel motor
    Motor* m_leftWheel;                 // left wheel motor
    Script* m_script;                   // Script function with automation script execution
    std::thread* m_thread;              // thread that executes the Script function
};


/*
 * Class that enables an automation script to be executed in a dedicated thread
 * provides API to set exit signal for the thread
 */
class Script
{
public:
    // constructors
    Script();                   // default constructor - should never be used
    Script(RobotApp* robotApp);	// create constructor

    virtual void run();			// worker loop to be placed here
    bool stopRequested();		// Checks if thread is requested to stop
    void stop();				// Request the thread to stop by setting value in promise object

private:
    std::promise<void> exitSignal;
    std::future<void> futureObj;
protected:
    void init(RobotApp* robotApp);  // super constructor to initialize protected class member variables
    void goToInitialState();        // tells the robot to go to the initial state
    void tickCounterInit(int duration);     // initializes the tick counter with a tick duration in milliseconds
    void tickCounterBegin();        // fetches and stores the current time at the beginning of the script tick loop
    void tickCounterSleep();        // sleeps the thread until the next tick to be awaken

    RobotApp* m_robotApp;           // related robotApp class providing the robot API
    int m_tickDuration;             // tick duration in milliseconds
    int m_tick;                     // tick number, 0 is the initial tick, increased by 1
    std::chrono::system_clock::time_point m_tickBegin;  // current tick begin timestamp
};

/*
* Script class extends the Script Task - perform a circle
*/
class ScriptCircle : public Script {
public:
    ScriptCircle(RobotApp* robotApp, std::string radius);   // constructor
    void run();			// worker loop to be placed here
private:
    double m_radius;    // circle radius
    int m_direction;    // circle direction -1 .. left, 1 .. right
};

/*
* Script class extends the Script Task - perform a snail move
*/
class ScriptSnail : public Script {
public:
    ScriptSnail(RobotApp* robotApp);    // constructor
    void run();			// worker loop to be placed here
};
