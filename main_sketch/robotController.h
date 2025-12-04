#pragma once
#include <Arduino.h>

class RobotController {
public:
    void begin();
    void poll();

private:

    // timer functions
    void startAction(unsigned long duration);
    bool actionDone();
    unsigned long actionEndTime = 0;
    bool actionActive = false;

    void pulseMotion();

    // helper movement functions
    void move_forward();
    void move_backward();
    void move_forward_left();
    void move_forward_right();
    void turn_left();
    void turn_right();
    void stop_moving();

};
