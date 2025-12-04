#ifndef ROBOT_H
#define ROBOT_H

#include <Arduino.h>

// Motion commands used by the simple state-driven motor helper.
enum Motion {
    MOTION_STOP,
    MOTION_FORWARD,
    MOTION_FORWARD_LEFT,
    MOTION_FORWARD_RIGHT,
    MOTION_TURN_LEFT,
    MOTION_TURN_RIGHT,
};

void updateRobotMotion();

#endif // ROBOT_H
