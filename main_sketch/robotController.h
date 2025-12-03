#pragma once
#include <Arduino.h>

class RobotController {
public:
    void begin();
    void poll();

    // Tunable movement timing
    void setMovementDelay(int ms) { moveDelay = ms; }

private:

    // helper movement functions
    void move_forward();
    void move_backward();
    void move_forward_left();
    void move_forward_right();
    void turn_left();
    void turn_right();
    void stop_moving();

    int moveDelay = 10; // in ms
};
