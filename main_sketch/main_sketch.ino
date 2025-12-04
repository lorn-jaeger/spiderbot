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
#include "SensorController.h"
#include "RobotController.h"

SensorController sensors;
RobotController robot;

// ====================================================
// Bluetooth Callbacks
// ====================================================
void onBleCommand(char c) {

    Serial.print("[BLE] New BLE Command: ");
    Serial.print(c);

    if(c == 'R'){
      robotReady = true;
    }

    if(c == 'S'){
      robotReady = false;
    }
}
void onBleConnect() {
    Serial.println("[BLE] Device Connected.");
    isConnectedBLE = true;
}

void onBleDisconnect() {
    Serial.println("[BLE] Device Disconnected.");
    isConnectedBLE = false;
}

// ====================================================
// Setup
// ====================================================
void setup() {
  Serial.begin(9600);
  delay(1000); //pause for serial connection

  initPins();
  initLEDs();

  sensors.begin();

  ble.begin();
  ble.setCommandCallback(onBleCommand);
  ble.setConnectCallback(onBleConnect);
  ble.setDisconnectCallback(onBleDisconnect);

  robot.begin();
}

// ====================================================
// Main Loop
// ====================================================
void loop() {
  if (isConnectedBLE && robotReady){
    sensors.poll();
  }else{
    currentState = IDLE;
  }

  robot.poll();
  
  ble.poll();
  updateLED();
  lastState = currentState;
}
