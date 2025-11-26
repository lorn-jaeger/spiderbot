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
#define backwardPin 2
#define forwardPin 3
#define leftTurnPin 4
#define rightTurnPin 5

  // ----------------------------------------------------
  // IR Line Tracking Sensor Pins
  // These read HIGH/LOW based on reflectivity
  // ----------------------------------------------------
#define irLeftPin 8
#define irRightPin 7

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
