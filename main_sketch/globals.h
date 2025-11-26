#ifndef GLOBALS_H
#define GLOBALS_H

// ---------------------------
// Global Includes
// ---------------------------
#include <Arduino.h>
#include <ArduinoBLE.h>

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
//extern BLEService robotService;
//extern BLEStringCharacteristic rxChar;
extern bool isConnectedBLE;

// ---------------------------
// Sensor Values (shared)
// ---------------------------
//extern int irLeft;
//extern int irMid;
//extern int irRight;
//extern bool crosswalkDetected;
//extern bool obstacleDetected;

// ---------------------------
// Function Prototypes
// (optional but recommended)
// ---------------------------
//void bluetoothSetup();
//void onConnect(BLEDevice central);
//void onDisconnect(BLEDevice central);
//void onRXWrite(BLEDevice central, BLECharacteristic characteristic);

#endif // GLOBALS_H
