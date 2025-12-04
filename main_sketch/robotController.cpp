#include "RobotController.h"
#include "globals.h" 

// =====================================================
// TURN PULSE SETTINGS
// =====================================================

enum PulseMode {
    PULSE_NONE,
    PULSE_FORWARD,
    PULSE_BACKWARD,
    PULSE_TURN_LEFT,
    PULSE_TURN_RIGHT
};

PulseMode pulseMode = PULSE_NONE;

// Tune these to taste
unsigned long pulseOnDuration  = 150;  // ms motors ON
unsigned long pulseOffDuration = 80;   // ms motors OFF

bool pulseOn = false;
unsigned long pulseEndTime = 0;

// -------------------------------------------------
// INITIALIZE MOTOR PINS
// -------------------------------------------------
void RobotController::begin() {
    stop_moving(); // ensure robot doesn't start moving
    startAction(10);
}

// -------------------------------------------------
// ACTION CONTROL SYSTEM
// -------------------------------------------------

void RobotController::startAction(unsigned long duration) {
    actionActive = true;
    actionEndTime = millis() + duration;
}

bool RobotController::actionDone() {
    return (!actionActive || millis() >= actionEndTime);
}

void RobotController::pulseMotion() {
    unsigned long now = millis();

    if (now >= pulseEndTime) {
        pulseOn = !pulseOn;

        if (pulseOn) {
            // ON PHASE
            switch (pulseMode) {
                case PULSE_FORWARD:     move_forward(); break;
                case PULSE_BACKWARD:    move_backward(); break;
                case PULSE_TURN_LEFT:   turn_left(); break;
                case PULSE_TURN_RIGHT:  turn_right(); break;
                default: break;
            }
            pulseEndTime = now + pulseOnDuration;
        } 
        else {
            // OFF PHASE
            stop_moving();
            pulseEndTime = now + pulseOffDuration;
        }
    }
}

// -------------------------------------------------
// STATE UPDATER (CALLED IN LOOP)
// -------------------------------------------------
// void RobotController::poll() {
//     // If we are performing a timed motion, wait for it to finish
//     if (!actionDone()) {
//         return;  // KEEP LOOP FREE FOR SENSOR READING
//     }
//     actionActive = false;
//     switch (currentState) {

//         case IDLE:
//             pulseMode = PULSE_NONE;
//             stop_moving();
//             startAction(40);
//             break;

//         case FOLLOW_LINE:

//             pulseMode = PULSE_FORWARD;
//             pulseMotion();
//             startAction(40);      // small forward bite
//             break;

//         case TURN_LEFT:
//             pulseMode = PULSE_TURN_LEFT;
//             pulseMotion();
//             startAction(40);      // gentle pivot
//             break;

//         case TURN_RIGHT:
//             pulseMode = PULSE_TURN_RIGHT;
//             pulseMotion();
//             startAction(40);      // gentle pivot
//             break;

//         case INTERSECTION:
//             pulseMode = PULSE_NONE;
//             stop_moving();
//             startAction(40);
//             break;

//         case CROSSWALK:
//             pulseMode = PULSE_NONE;
//             stop_moving();
//             startAction(40);
//             break;

//         case OBSTACLE_STOP:
//             pulseMode = PULSE_NONE;
//             stop_moving();
//             startAction(40);
//             break;

//         case END_OF_ROAD_TURN:
//             pulseMode = PULSE_TURN_LEFT;
//             pulseMotion();
//             startAction(50);      // slow search turn
//             break;
//     }
// }

void RobotController::poll() {
    // Decide what kind of motion we *want* based on currentState.
    // Then let pulseMotion() handle timing.

    switch (currentState) {

        case IDLE:
            pulseMode = PULSE_NONE;
            pulseOn   = false;   // reset pulse state
            stop_moving();
            return;

        case FOLLOW_LINE:
            // Go forward as long as both side sensors are 0
            pulseMode = PULSE_FORWARD;
            break;

        case TURN_LEFT:
            // One side sees the line → small left correction
            pulseMode = PULSE_TURN_LEFT;
            break;

        case TURN_RIGHT:
            // One side sees the line → small right correction
            pulseMode = PULSE_TURN_RIGHT;
            break;

        case END_OF_ROAD_TURN:
            // Slow search turn, still using pulses
            pulseMode = PULSE_TURN_LEFT;
            break;

        case INTERSECTION:
            pulseMode = PULSE_NONE;
            stop_moving();
            return;   // no pulses while in INTERSECTION}
        case CROSSWALK:
        case OBSTACLE_STOP:
            // For now: full stop in these states
            pulseMode = PULSE_NONE;
            pulseOn   = false;
            stop_moving();
            return;
    }

    // If we want motion, run the pulse state machine every loop.
    if (pulseMode == PULSE_NONE) {
        stop_moving();
        return;
    }

    pulseMotion();
}


// -------------------------------------------------
// MOVEMENT HELPERS, do not call directly
// -------------------------------------------------

void RobotController::move_forward() {
    //Serial.println("Moving robot forward");
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, HIGH);
    digitalWrite(leftTurnPin, HIGH);
    digitalWrite(rightTurnPin, HIGH);
}

void RobotController::move_backward() {
    //Serial.println("Moving robot backward");
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, LOW);
    digitalWrite(leftTurnPin, HIGH);
    digitalWrite(rightTurnPin, HIGH);
}

void RobotController::move_forward_left() {
    //Serial.println("Moving robot forward and left");
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, HIGH);
    digitalWrite(leftTurnPin, LOW);
    digitalWrite(rightTurnPin, HIGH);
}

void RobotController::move_forward_right() {
    //Serial.println("Moving robot forward and right");
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, HIGH);
    digitalWrite(leftTurnPin, HIGH);
    digitalWrite(rightTurnPin, LOW);
}

void RobotController::turn_left() {
    //Serial.println("Turning robot left");
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, HIGH);
    digitalWrite(leftTurnPin, LOW);
    digitalWrite(rightTurnPin, HIGH);
}

void RobotController::turn_right() {
    //Serial.println("Turning robot right");
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, HIGH);
    digitalWrite(leftTurnPin, HIGH);
    digitalWrite(rightTurnPin, LOW);
}

void RobotController::stop_moving() {
    //Serial.println("Stopping robot");
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, HIGH);
    digitalWrite(leftTurnPin, HIGH);
    digitalWrite(rightTurnPin, HIGH);
}
