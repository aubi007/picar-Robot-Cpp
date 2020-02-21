#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include "Stream.h"

struct Frame {
    std::vector<uchar> m_data;          // JPEG image as uchar vector (python list)
    clock_t m_ts;                       // frame capture timestamp
};

class RobotApp {
public:
    RobotApp();                         // constructor
    ~RobotApp();                        // destructor
    Frame getFrame();                   // gets the most current frame as a JPEG image
    void setBWStatus(int s);            // sets back wheel status
    void setSpeed(int s);               // sets the speed
    void setFWPos(int s);               // sets front wheel position
    void camReady();                    // moves the camera to initial position
    void setCamPan(int s);              // sets the camera pan
    void setCamTilt(int s);             // sets the camera tilt
    void screenshot();                  // creates screenshot
    void test(int s);                   // runs selected test script

private:
    Stream m_stream;                    // video stream class
    RobotStatus m_status;               // robot status structure
};


/* Constructor */
RobotApp::RobotApp() {
    std::cout << ">>> RobotApp init\n";
    m_status.bw_status = 0;
    m_status.speed = 30;
    m_status.fw_pos = 0;
    m_status.cam_pan = 0;
    m_status.cam_tilt = 0;
}

/* Destructor */
RobotApp::~RobotApp() {
    std::cout << ">>> RobotApp destroy\n";
}

Frame RobotApp::getFrame()
{
    Frame ret;
 
//    std::cout << ">>> getFrame\n";

    // copy the frame and frame capture timestamp from the stream
    ret.m_data = m_stream.getFrameData(m_status);
    ret.m_ts = m_stream.getFrameTS();

    return ret;
}

/* sets back wheel status */
void RobotApp::setBWStatus(int s) {
    if ((s >= -1) && (s <= 1)) {
        m_status.bw_status = s;
    }
    else {
        std::cerr << "### setBWStatus() invalid value <" << s << ">\n";
    }
}

/* sets the speed */
void RobotApp::setSpeed(int s) {
    if ((s >= 0) && (s <= 100)) {
        m_status.speed = s;
    }
    else {
        std::cerr << "### setSpeed() invalid value <" << s << ">\n";
    }
}

/* sets front wheel position */
void RobotApp::setFWPos(int s) {
    if ((s >= -40) && (s <= 40)) {
        m_status.fw_pos = s;
    }
    else {
        std::cerr << "### setFWPos() invalid value <" << s << ">\n";
    }
}

/* moves the camera to initial position */
void RobotApp::camReady() {
    setCamPan(0);
    setCamTilt(0);
}

/* sets the camera pan */
void RobotApp::setCamPan(int s) {
    if ((s >= -40) && (s <= 40)) {
        m_status.cam_pan = s;
    }
    else {
        std::cerr << "### setCamPan() invalid value <" << s << ">\n";
    }
}

/* sets the camera tilt */
void RobotApp::setCamTilt(int s) {
    if ((s >= -40) && (s <= 40)) {
        m_status.cam_tilt = s;
    }
    else {
        std::cerr << "### setCamTilt() invalid value <" << s << ">\n";
    }
}

/* creates screenshot */
void RobotApp::screenshot() {
// TODO: impl
}

/* runs selected test script */
void RobotApp::test(int s) {
    if ((s >= 1) && (s <= 3)) {
        // TODO: Impl
    }
    else {
        std::cerr << "### test() invalid value <" << s << ">\n";
    }
}



/* Python binding */
namespace py = pybind11;

PYBIND11_MODULE(RobotApp, m) {
    py::class_<Frame>(m, "Frame")
        .def_readonly("data", &Frame::m_data)
        .def_readonly("ts", &Frame::m_ts);

    py::class_<RobotApp>(m, "RobotApp")
        .def(py::init())
        .def("getFrame", &RobotApp::getFrame, R"pbdoc(gets the most current frame from the camera.)pbdoc")
        .def("setBWStatus", &RobotApp::setBWStatus, R"pbdoc(sets the back wheel status -1 backwards, 0 stop, 1 forward.)pbdoc")
        .def("setSpeed", &RobotApp::setSpeed, R"pbdoc(sets the speed 0 - 100.)pbdoc")
        .def("setFWPos", &RobotApp::setFWPos, R"pbdoc(sets front wheel position -40 - 40.)pbdoc")
        .def("camReady", &RobotApp::camReady, R"pbdoc(moves the camera to initial position.)pbdoc")
        .def("setCamPan", &RobotApp::setCamPan, R"pbdoc(sets the camera pan -40 - 40.)pbdoc")
        .def("setCamTilt", &RobotApp::setCamTilt, R"pbdoc(sets the camera tilt -40 - 40.)pbdoc")
        .def("screenshot", &RobotApp::screenshot, R"pbdoc(creates screenshot.)pbdoc")
        .def("test", &RobotApp::test, R"pbdoc(runs selected test script #.)pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}

