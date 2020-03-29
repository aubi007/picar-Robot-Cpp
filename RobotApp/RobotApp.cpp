#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include "Config.h"
#include "Stream.h"
#include "Servo.h"
#include "Motor.h"
#ifdef __linux__ 
#include <wiringPi.h>
#endif

#define _BUS_NUMBER 1
#define _ADDRESS    0x40

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
    Config config;                      // robot config
    Stream m_stream;                    // video stream class
    RobotStatus m_status;               // robot status structure
    Servo *m_fw;                        // front wheels servo
    Servo *m_camPan;                    // camera pan servo
    Servo *m_camTilt;                   // camera tilt servo
    Motor *m_rightWheel;                // right wheel motor
    Motor* m_leftWheel;                 // left wheel motor

};


/* Constructor */
RobotApp::RobotApp() {
    std::cout << ">>> RobotApp init\n";
    m_status.bw_status = 0;
    m_status.speed = 30;
    m_status.fw_pos = 0;
    m_status.cam_pan = 0;
    m_status.cam_tilt = 0;

#ifdef __linux__ 
    // initialize wiringPi
    if (wiringPiSetup() == -1) {
        printf("!!! wiringPi setup failed !");
        return;
    }
#endif

    // init front wheels servo
    m_fw = new Servo(_BUS_NUMBER, _ADDRESS, config.getFwChannel(), config.getFwOffset(), config.getFwMinAngle(), config.getFwMaxAngle());
    m_fw->write(0);

    // init camera pan servo
    m_camPan = new Servo(_BUS_NUMBER, _ADDRESS, config.getCamPanChannel(), config.getCamPanOffset(), config.getCamPanMinAngle(), config.getCamPanMaxAngle());
    m_camPan->write(0);

    // init camera tilt
    m_camTilt = new Servo(_BUS_NUMBER, _ADDRESS, config.getCamTiltChannel(), config.getCamTiltOffset(), config.getCamTiltMinAngle(), config.getCamTiltMaxAngle());
    m_camTilt->write(0);

    // init right wheel
    m_rightWheel = new Motor(_BUS_NUMBER, _ADDRESS, config.getRightWheelPin(), config.getRightWheelChannel(), config.getRightWheelDirection());

    // init left wheel
    m_leftWheel = new Motor(_BUS_NUMBER, _ADDRESS, config.getLeftWheelPin(), config.getLeftWheelChannel(), config.getLeftWheelDirection());
    
}

/* Destructor */
RobotApp::~RobotApp() {
    std::cout << ">>> RobotApp destroy\n";

    if (m_fw) {
        m_fw->write(0);
        delete m_fw;
    }
    if (m_camPan) {
        m_camPan->write(0);
        delete m_camPan;
    }
    if (m_camTilt) {
        m_camTilt->write(0);
        delete m_camTilt;
    }
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

        if (s == -1) {
            // go backwards
            m_rightWheel->backward();
            m_leftWheel->backward();
            m_rightWheel->speed(m_status.speed);
            m_leftWheel->speed(m_status.speed);
        } else if (s == 0) {
            // go stop
            m_rightWheel->stop();
            m_leftWheel->stop();
        } else if (s == 1) {
            // go forward
            m_rightWheel->forward();
            m_leftWheel->forward();
            m_rightWheel->speed(m_status.speed);
            m_leftWheel->speed(m_status.speed);
        }

    }
    else {
        std::cerr << "### setBWStatus() invalid value <" << s << ">\n";
    }
}

/* sets the speed */
void RobotApp::setSpeed(int s) {
    if ((s >= 0) && (s <= 100)) {
        // sets the speed to the status varaible
        m_status.speed = s;
        
        // if the motors are running, adjust the speed
        if (m_status.bw_status != 0) {
            m_rightWheel->speed(m_status.speed);
            m_leftWheel->speed(m_status.speed);
        }
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

// setups calibration
void RobotApp::calibration()
{
    std::cout << ">>> calibration" << std::endl;

    // set all servos to default position
    setFWPos(0);
    setCamPan(0);
    setCamTilt(0);
    // start the back wheels slowly forward
    setBWStatus(1);
    setSpeed(30);
};


// confirms calibration
void RobotApp::calibration_ok()
{
    std::cout << ">>> calibration_ok" << std::endl;

    // save config file
    config.update();
};

// calibration camera up
void RobotApp::calibration_camup()
{
    std::cout << ">>> calibration_camup" << std::endl;

    // update config offset
    config.setCamTiltOffset(config.getCamTiltOffset() + 1);
    
    // update servo offset
    m_camTilt->setOffset(config.getCamTiltOffset());

    // move the servo
    setCamTilt(0);
};

// calibration camera down
void RobotApp::calibration_camdown()
{
    std::cout << ">>> calibration_camdown" << std::endl;

    // update config offset
    config.setCamTiltOffset(config.getCamTiltOffset() - 1);

    // update servo offset
    m_camTilt->setOffset(config.getCamTiltOffset());

    // move the servo
    setCamTilt(0);
};

// calibration camera left
void RobotApp::calibration_camleft()
{
    std::cout << ">>> calibration_camleft" << std::endl;

    // update config offset
    config.setCamPanOffset(config.getCamPanOffset() + 1);

    // update servo offset
    m_camPan->setOffset(config.getCamPanOffset());

    // move the servo
    setCamPan(0);
};

// calibration camera right
void RobotApp::calibration_camright()
{
    std::cout << ">>> calibration_camright" << std::endl;

    // update config offset
    config.setCamPanOffset(config.getCamPanOffset() - 1);

    // update servo offset
    m_camPan->setOffset(config.getCamPanOffset());

    // move the servo
    setCamPan(0);
};

// calibration front wheels left
void RobotApp::calibration_fwleft()
{
    std::cout << ">>> calibration_fwleft" << std::endl;

    // update config offset
    config.setFwOffset(config.getFwOffset() - 1);

    // update servo offset
    m_fw->setOffset(config.getFwOffset());

    // move the servo
    setFWPos(0);
};

// calibration front wheels right
void RobotApp::calibration_fwright()
{
    std::cout << ">>> calibration_fwright" << std::endl;

    // update config offset
    config.setFwOffset(config.getFwOffset() + 1);

    // update servo offset
    m_fw->setOffset(config.getFwOffset());

    // move the servo
    setFWPos(0);
};

// calibration back wheels left
void RobotApp::calibration_bwleft()
{
    /*
    std::cout << ">>> calibration_camup" << std::endl;

    // update config offset
    config.setCamTiltOffset(config.getCamTiltOffset() + 1);

    // update servo offset
    m_camTilt->setOffset(config.getCamTiltOffset());

    // move the servo
    setCamTilt(0);
    */
};

// calibration back wheels right
void RobotApp::calibration_bwright()
{
    /*
    std::cout << ">>> calibration_camup" << std::endl;

    // update config offset
    config.setCamTiltOffset(config.getCamTiltOffset() + 1);

    // update servo offset
    m_camTilt->setOffset(config.getCamTiltOffset());

    // move the servo
    setCamTilt(0);
    */
};

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
        .def("test", &RobotApp::test, R"pbdoc(runs selected test script.)pbdoc")
        .def("calibration", &RobotApp::calibration, R"pbdoc(setups servos for calibration.)pbdoc")
        .def("calibration_ok", &RobotApp::calibration_ok, R"pbdoc(confirms calibration.)pbdoc")
        .def("calibration_camup", &RobotApp::calibration_camup, R"pbdoc(calibration camera up.)pbdoc")
        .def("calibration_camdown", &RobotApp::calibration_camdown, R"pbdoc(calibration camera down.)pbdoc")
        .def("calibration_camleft", &RobotApp::calibration_camleft, R"pbdoc(calibration camera left.)pbdoc")
        .def("calibration_camright", &RobotApp::calibration_camright, R"pbdoc(calibration camera right.)pbdoc")
        .def("calibration_fwleft", &RobotApp::calibration_fwleft, R"pbdoc(calibration front wheels left.)pbdoc")
        .def("calibration_fwright", &RobotApp::calibration_fwright, R"pbdoc(calibration front wheels right.)pbdoc")
        .def("calibration_bwleft", &RobotApp::calibration_bwleft, R"pbdoc(calibration back wheels left.)pbdoc")
        .def("calibration_bwright", &RobotApp::calibration_bwright, R"pbdoc(calibration back wheels right.)pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}

