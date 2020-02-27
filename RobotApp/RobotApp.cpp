#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include "Stream.h"
#include "Servo.h"

#define _BUS_NUMBER 1
#define _ADDRESS    0x40

struct Frame {
    std::vector<uchar> m_data;          // JPEG image as uchar vector (python list)
    clock_t m_ts;                       // frame capture timestamp
};

class Config {
public:
    int getFwChannel() { return m_fwChannel; };
    int getFwOffset() { return m_fwOffset; };
    int getFwMinAngle() { return m_fwMinAngle; };
    int getFwMaxAngle() { return m_fwMaxAngle; };

    int getCamPanChannel() { return m_camPanChannel; };
    int getCamPanOffset() { return m_camPanOffset; };
    int getCamPanMinAngle() { return m_camPanMinAngle; };
    int getCamPanMaxAngle() { return m_camPanMaxAngle; };

    int getCamTiltChannel() { return m_camTiltChannel; };
    int getCamTiltOffset() { return m_camTiltOffset; };
    int getCamTiltMinAngle() { return m_camTiltMinAngle; };
    int getCamTiltMaxAngle() { return m_camTiltMaxAngle; };
private:
    int m_fwChannel = 0;            // front wheels servo channel
    int m_fwOffset = 90;            // front wheels servo offset
    int m_fwMinAngle = 50;          // front wheels minimal angle
    int m_fwMaxAngle = 130;         // front wheels maximal angle

    int m_camPanChannel = 1;            // camera pan servo channel
    int m_camPanOffset = 90;            // camera pan servo offset
    int m_camPanMinAngle = 50;          // camera pan minimal angle
    int m_camPanMaxAngle = 130;         // camera pan maximal angle

    int m_camTiltChannel = 2;            // camera tilt servo channel
    int m_camTiltOffset = 90;            // camera tilt servo offset
    int m_camTiltMinAngle = 50;          // camera tilt minimal angle
    int m_camTiltMaxAngle = 130;         // camera tilt maximal angle
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
    Config config;                      // robot config
    Stream m_stream;                    // video stream class
    RobotStatus m_status;               // robot status structure
    Servo *m_fw;                        // front wheels servo
    Servo *m_camPan;                    // camera pan servo
    Servo *m_camTilt;                   // camera tilt servo

};


/* Constructor */
RobotApp::RobotApp() {
    std::cout << ">>> RobotApp init\n";
    m_status.bw_status = 0;
    m_status.speed = 30;
    m_status.fw_pos = 0;
    m_status.cam_pan = 0;
    m_status.cam_tilt = 0;

    // init front wheels servo
    m_fw = new Servo(_BUS_NUMBER, _ADDRESS, config.getFwChannel(), config.getFwOffset(), config.getFwMinAngle(), config.getFwMaxAngle());
    m_fw->write(0);

    // init camera pan servo
    m_camPan = new Servo(_BUS_NUMBER, _ADDRESS, config.getCamPanChannel(), config.getCamPanOffset(), config.getCamPanMinAngle(), config.getCamPanMaxAngle());
    m_camPan->write(0);

    // init camera tilt
    m_camTilt = new Servo(_BUS_NUMBER, _ADDRESS, config.getCamTiltChannel(), config.getCamTiltOffset(), config.getCamTiltMinAngle(), config.getCamTiltMaxAngle());
    m_camTilt->write(0);
}

/* Destructor */
RobotApp::~RobotApp() {
    std::cout << ">>> RobotApp destroy\n";

    if (m_fw) delete m_fw;
    if (m_camPan) delete m_camPan;
    if (m_camTilt) delete m_camTilt;
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
        m_fw->write(s);
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
        m_camPan->write(s);
    }
    else {
        std::cerr << "### setCamPan() invalid value <" << s << ">\n";
    }
}

/* sets the camera tilt */
void RobotApp::setCamTilt(int s) {
    if ((s >= -40) && (s <= 40)) {
        m_status.cam_tilt = s;
        m_camTilt->write(s);
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

