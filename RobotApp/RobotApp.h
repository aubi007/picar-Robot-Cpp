#pragma once

struct RobotStatus {
    int bw_status;                      // back wheel status 0 - stop, 1 - forward, -1 - backward
    int speed;                          // back wheels speed 0 - 100
    int fw_pos;                         // front wheel position 0 - steady, <0 - left, >0 - right
    int cam_pan;                        // camera pan 0 - steady, >0 - left <0 - right
    int cam_tilt;                       // camera tilt 0 - steady,
};