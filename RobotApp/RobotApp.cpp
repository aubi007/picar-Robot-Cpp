#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "RobotApp.h"

#ifdef __linux__ 
#include <wiringPi.h>
#endif

#define _BUS_NUMBER 1
#define _ADDRESS    0x40

/* Constructor */
RobotApp::RobotApp() {
    std::cout << ">>> RobotApp init\n";
    m_status.bw_status = 0;
    m_status.speed = 30;
    m_status.fw_pos = 0;
    m_status.cam_pan = 0;
    m_status.cam_tilt = 0;
    m_script = NULL;
    m_thread = NULL;

#ifdef __linux__ 
    // initialize wiringPi
    if (wiringPiSetup() == -1) {
        printf("!!! wiringPi setup failed !");
        return;
    }
#endif

    // init front wheels servo
    m_fw = new Servo(_BUS_NUMBER, _ADDRESS, m_config.getFwChannel(), m_config.getFwOffset(), m_config.getFwMinAngle(), m_config.getFwMaxAngle());
    m_fw->write(0);

    // init camera pan servo
    m_camPan = new Servo(_BUS_NUMBER, _ADDRESS, m_config.getCamPanChannel(), m_config.getCamPanOffset(), m_config.getCamPanMinAngle(), m_config.getCamPanMaxAngle());
    m_camPan->write(0);

    // init camera tilt
    m_camTilt = new Servo(_BUS_NUMBER, _ADDRESS, m_config.getCamTiltChannel(), m_config.getCamTiltOffset(), m_config.getCamTiltMinAngle(), m_config.getCamTiltMaxAngle());
    m_camTilt->write(0);

    // init right wheel
    m_rightWheel = new Motor(_BUS_NUMBER, _ADDRESS, m_config.getRightWheelPin(), m_config.getRightWheelChannel(), m_config.getRightWheelDirection());

    // init left wheel
    m_leftWheel = new Motor(_BUS_NUMBER, _ADDRESS, m_config.getLeftWheelPin(), m_config.getLeftWheelChannel(), m_config.getLeftWheelDirection());
    
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

// gets the config class
Config RobotApp::getConfig()
{
    return m_config;
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
/* due to the servo position the angle must be inverted so left is negative, right is positive */
void RobotApp::setCamPan(int s) {
    if ((s >= -40) && (s <= 40)) {
        m_status.cam_pan = -s;
        m_camPan->write(-s);
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

/* runs selected automation script */
void RobotApp::run(int script, std::string param1, std::string param2) {
    std::cout << ">>> run(" << script << ", " << param1 << ", " << param2 << ")" << std::endl;
    if (script == 1) {
        try {
            // create the script class and pass parameters
            m_script = new ScriptCircle(this, param1);

            //Creating a thread to execute our task
            m_thread = new std::thread([&]()
                {
                    m_script->run();
                });
        }
        catch (const std::exception& e) {
            std::cerr << "### run() failed: " << e.what() << std::endl;
        }

    } else if (script == 2) {
        // snail move without params
        m_script = new ScriptSnail(this);

        //Creating a thread to execute our task
        m_thread = new std::thread([&]()
            {
                m_script->run();
            });

        }

    else {
        std::cerr << "### run() invalid script <" << script << ">" << std::endl;
    }
}

/* stops running automation script */
void RobotApp::stop() {
    std::cout << ">>> stop()" << std::endl;

    if (m_script) {
        // Stop the Task
        m_script->stop();
        
        //Waiting for thread to join
        m_thread->join();
        
        // clear values
        m_script = NULL;
        m_thread = NULL;
        std::cout << ">>> Thread Joined" << std::endl;
    }
    else {

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
    m_config.update();
};

// calibration camera up
void RobotApp::calibration_camup()
{
    std::cout << ">>> calibration_camup" << std::endl;

    // update config offset
    m_config.setCamTiltOffset(m_config.getCamTiltOffset() + 1);
    
    // update servo offset
    m_camTilt->setOffset(m_config.getCamTiltOffset());

    // move the servo
    setCamTilt(0);
};

// calibration camera down
void RobotApp::calibration_camdown()
{
    std::cout << ">>> calibration_camdown" << std::endl;

    // update config offset
    m_config.setCamTiltOffset(m_config.getCamTiltOffset() - 1);

    // update servo offset
    m_camTilt->setOffset(m_config.getCamTiltOffset());

    // move the servo
    setCamTilt(0);
};

// calibration camera left
void RobotApp::calibration_camleft()
{
    std::cout << ">>> calibration_camleft" << std::endl;

    // update config offset
    m_config.setCamPanOffset(m_config.getCamPanOffset() + 1);

    // update servo offset
    m_camPan->setOffset(m_config.getCamPanOffset());

    // move the servo
    setCamPan(0);
};

// calibration camera right
void RobotApp::calibration_camright()
{
    std::cout << ">>> calibration_camright" << std::endl;

    // update config offset
    m_config.setCamPanOffset(m_config.getCamPanOffset() - 1);

    // update servo offset
    m_camPan->setOffset(m_config.getCamPanOffset());

    // move the servo
    setCamPan(0);
};

// calibration front wheels left
void RobotApp::calibration_fwleft()
{
    std::cout << ">>> calibration_fwleft" << std::endl;

    // update config offset
    m_config.setFwOffset(m_config.getFwOffset() - 1);

    // update servo offset
    m_fw->setOffset(m_config.getFwOffset());

    // move the servo
    setFWPos(0);
};

// calibration front wheels right
void RobotApp::calibration_fwright()
{
    std::cout << ">>> calibration_fwright" << std::endl;

    // update config offset
    m_config.setFwOffset(m_config.getFwOffset() + 1);

    // update servo offset
    m_fw->setOffset(m_config.getFwOffset());

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
        .def("run", &RobotApp::run, R"pbdoc(runs selected automation script.)pbdoc")
        .def("stop", &RobotApp::stop, R"pbdoc(stops running automation script.)pbdoc")
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

