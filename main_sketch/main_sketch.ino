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


// ====================================================
// Setup
// ====================================================
void setup() {

  // ---- Serial Monitor Setup ----
  Serial.begin(9600);
  delay(1000); //pause for serial connection

  initPins();
  initLEDs();
  setupBluetooth();
}

// ====================================================
// Main Loop
// ====================================================
void loop() {
  pollBluetooth();
  updateLED();
}
