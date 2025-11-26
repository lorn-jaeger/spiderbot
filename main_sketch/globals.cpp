#include "globals.h"

// Create actual storage for globals
RobotState currentState = IDLE;

// BLE objects
bool isConnectedBLE = false;

// Initialization Functions
void initPins() {
    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(forwardPin, OUTPUT);
    pinMode(backwardPin, OUTPUT);
    pinMode(leftTurnPin, OUTPUT);
    pinMode(rightTurnPin, OUTPUT);

    pinMode(obstacleEchoPin, INPUT);
    pinMode(obstacleTrigPin, OUTPUT);
    digitalWrite(obstacleTrigPin, LOW);
}

void initLEDs() {
    // ACTIVE-LOW LEDs → OFF means HIGH
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, HIGH);
}