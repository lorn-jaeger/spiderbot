#include "globals.h"

// Robot objects
RobotState currentState = IDLE;
bool robotReady = false;

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