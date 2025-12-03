#include "RobotController.h"
#include "globals.h" 

// -------------------------------------------------
// INITIALIZE MOTOR PINS
// -------------------------------------------------
void RobotController::begin() {
    stop_moving(); // ensure robot doesn't start moving
}

// -------------------------------------------------
// STATE UPDATER (CALLED IN LOOP)
// -------------------------------------------------
void RobotController::poll() {
    switch (currentState) {
        case IDLE:
            stop_moving();
            break;

        case FOLLOW_LINE:
            move_forward();
            break;

        case TURN_LEFT:
            turn_left();
            break;

        case TURN_RIGHT:
            turn_right();
            break;

        case INTERSECTION:
            // FIX: this needs to be handled
            stop_moving(); 
            move_forward();
            break;

        case CROSSWALK:
            // FIX: this needs to be handled
            stop_moving();
            break;

        case OBSTACLE_STOP:
            stop_moving();
            break;

        case END_OF_ROAD_TURN:
            turn_left();
            break;
    }
}

// -------------------------------------------------
// MOVEMENT HELPERS (using your exact logic)
// -------------------------------------------------

void RobotController::move_forward() {
    Serial.println("Moving robot forward");
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, HIGH);
    digitalWrite(leftTurnPin, HIGH);
    digitalWrite(rightTurnPin, HIGH);
    delay(moveDelay);
}

void RobotController::move_backward() {
    Serial.println("Moving robot backward");
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, LOW);
    digitalWrite(leftTurnPin, HIGH);
    digitalWrite(rightTurnPin, HIGH);
    delay(moveDelay);
}

void RobotController::move_forward_left() {
    Serial.println("Moving robot forward and left");
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, HIGH);
    digitalWrite(leftTurnPin, LOW);
    digitalWrite(rightTurnPin, HIGH);
    delay(moveDelay);
}

void RobotController::move_forward_right() {
    Serial.println("Moving robot forward and right");
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, HIGH);
    digitalWrite(leftTurnPin, HIGH);
    digitalWrite(rightTurnPin, LOW);
    delay(moveDelay);
}

void RobotController::turn_left() {
    Serial.println("Turning robot left");
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, HIGH);
    digitalWrite(leftTurnPin, LOW);
    digitalWrite(rightTurnPin, HIGH);
    delay(moveDelay);
}

void RobotController::turn_right() {
    Serial.println("Turning robot right");
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, HIGH);
    digitalWrite(leftTurnPin, HIGH);
    digitalWrite(rightTurnPin, LOW);
    delay(moveDelay);
}

void RobotController::stop_moving() {
    Serial.println("Stopping robot");
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, HIGH);
    digitalWrite(leftTurnPin, HIGH);
    digitalWrite(rightTurnPin, HIGH);
    delay(moveDelay);
}
