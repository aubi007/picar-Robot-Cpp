#include "Config.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#define CONFIGFILE "config.txt"

using namespace std;

// constructors and destructors
Config::Config()
{
    // default values
    m_fwChannel = 0;            // front wheels servo PCA channel
    m_fwOffset = 90;            // front wheels servo offset
    m_fwMinAngle = 50;          // front wheels minimal angle
    m_fwMaxAngle = 130;         // front wheels maximal angle

    m_camPanChannel = 1;            // camera pan servo PCA channel
    m_camPanOffset = 90;            // camera pan servo offset
    m_camPanMinAngle = 50;          // camera pan minimal angle
    m_camPanMaxAngle = 130;         // camera pan maximal angle

    m_camTiltChannel = 2;           // camera tilt servo PCA channel
    m_camTiltOffset = 90;           // camera tilt servo offset
    m_camTiltMinAngle = 50;         // camera tilt minimal angle
    m_camTiltMaxAngle = 130;        // camera tilt maximal angle

    m_rightWheelPin = 2;             // right wheel wiringPi pin
    m_rightWheelChannel = 4;         // right wheel PCA channel
    m_rightWheelDirection = 0;       // right wheel forward direction

    m_leftWheelPin = 0;             // left wheel wiringPi pin
    m_leftWheelChannel = 5;         // left wheel PCA channel
    m_leftWheelDirection = 0;       // left wheel forward direction

    // read the configuration from a config file
    cout << ">>> Reading the config from file " << CONFIGFILE << endl;

    // open the config file
    ifstream configfile(CONFIGFILE);  // config file input stream
    string line;                        // line fetched from the stream
    istringstream sin;

    // fetch line by line
    while (getline(configfile, line)) {
        // find the value position - 1 character after equation sign
        sin.str(line.substr(line.find("=") + 1));

        // check if line contains fwOffset
        if (line.find("fwOffset") != string::npos) {
            cout << ">>> fwOffset " << sin.str() << endl;
            sin >> m_fwOffset;
        } else if (line.find("camPanOffset") != string::npos) {
            cout << ">>> camPanOffset " << sin.str() << endl;
            sin >> m_camPanOffset;
        }
        else if (line.find("camTiltOffset") != string::npos) {
            cout << ">>> camTiltOffset " << sin.str() << endl;
            sin >> m_camTiltOffset;
        }
    
        sin.clear();
    }

    // close the stream
    configfile.close();
}

// updates the configuration file
void Config::update()
{
    // write the configuration from a config file
    cout << ">>> Writing the config from file " << CONFIGFILE << endl;

    // open the config file
    ofstream configfile(CONFIGFILE);  // config file input stream

    configfile << "fwOffset=" << m_fwOffset << endl;
    configfile << "camPanOffset=" << m_camPanOffset << endl;
    configfile << "camTiltOffset=" << m_camTiltOffset << endl;

    configfile.close();
}

// getters
int Config::getFwChannel() { return m_fwChannel; };
int Config::getFwOffset() { return m_fwOffset; };
int Config::getFwMinAngle() { return m_fwMinAngle; };
int Config::getFwMaxAngle() { return m_fwMaxAngle; };

int Config::getCamPanChannel() { return m_camPanChannel; };
int Config::getCamPanOffset() { return m_camPanOffset; };
int Config::getCamPanMinAngle() { return m_camPanMinAngle; };
int Config::getCamPanMaxAngle() { return m_camPanMaxAngle; };

int Config::getCamTiltChannel() { return m_camTiltChannel; };
int Config::getCamTiltOffset() { return m_camTiltOffset; };
int Config::getCamTiltMinAngle() { return m_camTiltMinAngle; };
int Config::getCamTiltMaxAngle() { return m_camTiltMaxAngle; };

int Config::getRightWheelPin() { return m_rightWheelPin; };
int Config::getRightWheelChannel() { return m_rightWheelChannel; };
int Config::getRightWheelDirection() { return m_rightWheelDirection; };

int Config::getLeftWheelPin() { return m_leftWheelPin; };
int Config::getLeftWheelChannel() { return m_leftWheelChannel; };
int Config::getLeftWheelDirection() { return m_leftWheelDirection; };

// setters
void Config::setFwOffset(int off) { m_fwOffset = off; };
void Config::setCamPanOffset(int off) { m_camPanOffset = off; };
void Config::setCamTiltOffset(int off) { m_camTiltOffset = off; };
