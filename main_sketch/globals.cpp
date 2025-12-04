#include "globals.h"

// Robot objects
RobotState currentState = IDLE;
RobotState lastState = IDLE;
bool robotReady = false;

const char* stateToString(RobotState s) {
    switch (s) {
        case IDLE:             return "IDLE";
        case FOLLOW_LINE:      return "FOLLOW_LINE";
        case TURN_LEFT:        return "TURN_LEFT";
        case TURN_RIGHT:       return "TURN_RIGHT";
        case INTERSECTION:     return "INTERSECTION";
        case CROSSWALK:        return "CROSSWALK";
        case OBSTACLE_STOP:    return "OBSTACLE_STOP";
        case END_OF_ROAD_TURN: return "END_OF_ROAD_TURN";
        default:               return "UNKNOWN";
    }
}

// BLE objects
bool isConnectedBLE = false;

// Initialization Functions
void initPins() {
    //Internal LED pins
    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    // Robot Controller pins
    pinMode(forwardPin, OUTPUT);
    pinMode(backwardPin, OUTPUT);
    pinMode(leftTurnPin, OUTPUT);
    pinMode(rightTurnPin, OUTPUT);

    // Ultra Sonic Pins
    pinMode(obstacleEchoPin, INPUT);
    pinMode(obstacleTrigPin, OUTPUT);
    digitalWrite(obstacleTrigPin, LOW);

    // IR pins
    pinMode(irLeftPin, INPUT);
    pinMode(irMiddlePin, INPUT);
    pinMode(irRightPin, INPUT);
}

void initLEDs() {
    // ACTIVE-LOW LEDs → OFF means HIGH
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, HIGH);
}
