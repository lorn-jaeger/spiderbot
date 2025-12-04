#ifndef GLOBALS_H
#define GLOBALS_H

// ---------------------------
// Global Includes
// ---------------------------
#include <Arduino.h>
#include <ArduinoBLE.h>

// ---------------------------
// Pin Declarations
// ---------------------------
#define LED_R 14
#define LED_G 15
#define LED_B 16

  // ----------------------------------------------------
  // Motor Control Pins (depends on your motor controller)
  // LOW  = activate
  // HIGH = stop
  // ----------------------------------------------------
#define backwardPin 4
#define forwardPin 5
#define leftTurnPin 6
#define rightTurnPin 7

  // ----------------------------------------------------
  // IR Line Tracking Sensor Pins
  // These read HIGH/LOW based on reflectivity
  // ----------------------------------------------------
#define irLeftPin A0
#define irRightPin A1
#define irMiddlePin A2
#define crosswalkPin A3

#define obstacleTrigPin 3
#define obstacleEchoPin 2

// ---------------------------
// Robot State Machine
// ---------------------------
enum RobotState {
    IDLE,
    FOLLOW_LINE,
    TURN_LEFT,
    TURN_RIGHT,
    INTERSECTION,
    CROSSWALK,
    OBSTACLE_STOP,
    END_OF_ROAD_TURN,
};

extern RobotState currentState;
extern RobotState lastState;


extern bool robotReady;

// ---------------------------
// BLE Globals
// ---------------------------
extern bool isConnectedBLE;


// ---------------------------
// Initialization functions
// ---------------------------
void initPins();
void initLEDs();
#endif // GLOBALS_H
