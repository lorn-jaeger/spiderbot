/*
 * HexBug Robot BLE Controller
 * --------------------------------
 * This program controls a small robot using:
 *  - Android phone → BLE commands
 *  - Arduino Nano ESP32
 *  - IR tracking sensors for autonomous steering
 *
 * BLE Commands:
 *    'R' → Robot RUN (start autonomous driving)
 *    'S' → Robot STOP
 *
 * Hardware:
 *    - Two IR sensors (left + right)
 *    - Four motor control pins (forward/back/left/right)
 *
 * The robot drives forward until the IR sensors detect a line or boundary,
 * then it corrects itself using simple logic.
 */
#include "globals.h"
#include "BleController.h"
BleController ble;

// ====================================================
// Bluetooth Callbacks
// ====================================================
void onBleCommand(char c) {
    switch (c) {
        case 'R': currentState = FOLLOW_LINE; break;
        case 'S': currentState = IDLE; break;
        case 'O': currentState = OBSTACLE_STOP; break;
        case 'I': currentState = INTERSECTION; break;
        case 'C': currentState = CROSSWALK; break;
        case 'E': currentState = END_OF_ROAD_TURN; break;
        case 'l': currentState = TURN_LEFT; break;
        case 'r': currentState = TURN_RIGHT; break;
    }

    Serial.print("[STATE] New Robot State: ");
    Serial.println(currentState);
}
void onBleConnect() {
    Serial.println("[BLE] Device Connected.");
    isConnectedBLE = true;
    currentState = IDLE;
}

void onBleDisconnect() {
    Serial.println("[BLE] Device Disconnected.");
    isConnectedBLE = false;
    currentState = IDLE;
}

// ====================================================
// Setup
// ====================================================
void setup() {
  Serial.begin(9600);
  delay(1000); //pause for serial connection

  initPins();
  initLEDs();

  ble.begin();
  ble.setCommandCallback(onBleCommand);
  ble.setConnectCallback(onBleConnect);
  ble.setDisconnectCallback(onBleDisconnect);
}

// ====================================================
// Main Loop
// ====================================================
void loop() {
  ble.poll();
  updateLED();
}
