#pragma once

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
};