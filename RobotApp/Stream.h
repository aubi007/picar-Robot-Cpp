#pragma once
#include <iostream>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "RobotApp.h"

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

