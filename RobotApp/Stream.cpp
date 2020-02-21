#include <iostream>
#include <opencv2/imgproc.hpp>
#include "Stream.h"

using namespace cv;

/* Constructor */
Stream::Stream() {
    std::cout << ">>> Stream init\n";

    int deviceID = 0;                   // 0 = open default camera
    int apiID = cv::CAP_ANY;            // 0 = autodetect default API

    // open selected camera using selected API
    m_cap.open(deviceID + apiID);

    // check if we succeeded
    if (!m_cap.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        throw std::runtime_error("ERROR! Unable to open camera");
    }

    // initial frame capture
    capture();

    // start the capture thread
    m_capture_thread = std::thread(&Stream::captureLoop, this);
}

/* Destructor */
Stream::~Stream() {
    std::cout << ">>> Stream destroy\n";
}

/* capture one frame and store to member variables */
void Stream::capture() {
    Mat frame;

    // wait for a new frame from camera and store it into 'frame'
    m_cap.read(frame);

    // check if we succeeded
    if (frame.empty()) {
        std::cerr << "### ERROR! blank frame grabbed\n";
        return;
    }

 //   std::cout << ">>> Frame captured\n";

    // copy the captured frame to the class member
    m_frame_data = frame;

    // update the frame timestamp
    m_frame_ts = clock();

}

/* infinite loop supposed to run capture in dedicated thread */
void Stream::captureLoop() {
    for (;;) {
        capture();
        //       std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

/* gets the most current frame as a JPEG image and draw in the bot status */
std::vector<uchar> Stream::getFrameData(RobotStatus status)
{
    std::vector<uchar> ret;
    Mat image = m_frame_data.clone();               // clone the MAT to a new object so we can modify it

    if (!image.empty()) {
        const size_t bufSize = 1024;
        char caption[bufSize];
        snprintf(caption, bufSize, "BW: %d SPD: %d FW: %d CPAN: %d CTLT: %d", status.bw_status, status.speed, status.fw_pos, status.cam_pan, status.cam_tilt);
//        std::cout << "Caption: " << caption << "\n";
        putText(image, caption, Point(10, 16), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255));

        // encode the Mat frame_data as JPEG
        imencode(".jpg", image, ret);
    }

    return ret;
}

/* gets the current frame capture timestamp */
clock_t Stream::getFrameTS()
{
    return m_frame_ts;
}
