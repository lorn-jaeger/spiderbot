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
// LEDS for tracking BT and robot state
// LED state control
//bool isConnected = false;
unsigned long lastBlinkTime = 0;
bool ledState = LOW;
const int BLINK_INTERVAL = 300;  // ms
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

// ----------------------------------------------------
// Robot State Variables
// ----------------------------------------------------
bool doLeftTurn;
bool doRightTurn;
bool isLost;

int timeDelay = 250;   // small delay for movement timing



// ====================================================
// Setup
// ====================================================
void setup() {
  // ---- IR Tracking Sensor Setup ----
  pinMode(irLeftPin, INPUT);
  pinMode(irRightPin, INPUT);

  // ---- Motor Control Setup ----
  pinMode(backwardPin, OUTPUT);
  pinMode(forwardPin, OUTPUT);
  pinMode(leftTurnPin, OUTPUT);
  pinMode(rightTurnPin, OUTPUT);

  // ---- Serial Monitor Setup ----
  Serial.begin(9600);
  //while (!Serial);   // wait for serial to open
  delay(1000); //pause for serial connection

  // robot state tracking leds
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  // RGB LEDs on this board are ACTIVE-LOW
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);

  setupBluetooth();
}
// ====================================================
// LED/State Helper Functions
// ====================================================

void updateBLELED() {
  if (isConnectedBLE) {
    // SOLID ON when connected
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    // BLINKING when advertising
    unsigned long now = millis();
    if (now - lastBlinkTime >= BLINK_INTERVAL) {
      lastBlinkTime = now;
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState);
    }
  }
}

void setLED(bool r, bool g, bool b) {
  // ACTIVE LOW LEDs
  digitalWrite(LED_R, r ? LOW : HIGH);
  digitalWrite(LED_G, g ? LOW : HIGH);
  digitalWrite(LED_B, b ? LOW : HIGH);
}

void updateRobotLED() {

  if (!isConnectedBLE){
    setLED(false, false, false);
    return;
  }

  if (currentState == IDLE) {
    // STOPPED = RED
    setLED(true, false, false);
    return;
  }

  // robotState == RUNNING
  if (isLost) {
    // LOST CONDITION = PURPLE
    setLED(true, false, true);
  }
  else if (doLeftTurn || doRightTurn) {
    // Turning = BLUE
    setLED(false, false, true);
  }
  else {
    // Running straight = GREEN
    setLED(false, true, false);
  }
}



// ====================================================
// Main Loop
// ====================================================
void loop() {
  pollBluetooth();
  updateBLELED();
  updateRobotLED();

}



// ====================================================
// Autonomous Robot Behavior (Line Following Logic)
// ====================================================
void startRobot() {
  // Read sensors
  doLeftTurn  = digitalRead(irLeftPin)  == 1;
  doRightTurn = digitalRead(irRightPin) == 1;

  // If both sensors see line → robot is "lost"
  isLost = (doLeftTurn == 1) && (doRightTurn == 1);


  if (isLost) {
    // If lost, perform recovery turn
    turn_left(timeDelay);
    return;
  }

  // If only left sensor picks up line → turn right
  if (doLeftTurn) {
    move_forward_right(timeDelay);
  }
  // If only right sensor picks up line → turn left
  else if (doRightTurn) {
    move_forward_left(timeDelay);
  }
  // Otherwise go straight
  else {
    move_forward(timeDelay);
  }
}


// ====================================================
// Robot STOP Behavior
// ====================================================
void stopRobot() {  
  stop_moving(timeDelay);
}