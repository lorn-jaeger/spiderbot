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

#include <ArduinoBLE.h>

// ------------------------
// BLE UART Service Setup
// Uses Nordic UART Service UUIDs (common BLE serial standard)
// ------------------------
BLEService uartService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");

// TX characteristic (ESP32 → phone)
BLECharacteristic txChar(
  "6E400003-B5A3-F393-E0A9-E50E24DCCA9E",
  BLERead | BLENotify,
  20
);

// RX characteristic (phone → ESP32)
BLECharacteristic rxChar(
  "6E400002-B5A3-F393-E0A9-E50E24DCCA9E",
  BLEWrite | BLEWriteWithoutResponse,
  20
);

// LEDS for tracking BT and robot state
// LED state control
bool isConnected = false;
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

// Robot state machine
enum RobotState {
  STOPPED,
  RUNNING
};

RobotState robotState = STOPPED;



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
  Serial.println("Starting ArduinoBLE UART...");

  // ---- BLE Initialization ----
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  if (!BLE.begin()) {
    Serial.println("BLE begin failed!");
    while (1);
  }

  // robot state tracking leds
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  // RGB LEDs on this board are ACTIVE-LOW
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);


  BLE.setLocalName("HexBug_1");           // Name shown on your phone
  BLE.setAdvertisedService(uartService);  // Advertise UART service

  uartService.addCharacteristic(txChar);
  uartService.addCharacteristic(rxChar);

  BLE.addService(uartService);

  // Send startup message to phone (optional)
  txChar.writeValue((const uint8_t*)"READY", 5);

  // Begin advertising
  BLE.advertise();
  Serial.println("BLE UART ready and advertising!");
}
// ====================================================
// LED/State Helper Functions
// ====================================================

void updateBLELED() {
  if (isConnected) {
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

  if (!isConnected){
    setLED(false, false, false);
    return;
  }

  if (robotState == STOPPED) {
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
  BLE.poll();   // keep BLE stack running smoothly

  BLEDevice central = BLE.central();   // check if a phone is trying to connect

  // No connection yet → blinking state
  isConnected = central && central.connected();
  updateBLELED();

  // If a phone connects:
  if (central) {
    Serial.print("Connected to: ");
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH); // signal on board if connected with bt

    // Stay in this loop while connected
    while (central.connected()) {
      BLE.poll();

      // Keep LED solid durring connection
      isConnected = true;
      updateBLELED();
      updateRobotLED();

      // --- Robot Behavior ---
      if (robotState == RUNNING) {
        startRobot();
      } else {
        stopRobot();
      }

      // --- BLE Command Processing ---
      if (rxChar.written()) {
        int len = rxChar.valueLength();
        const uint8_t* data = rxChar.value();

        for (int i = 0; i < len; i++) {
          char c = (char)data[i];
          Serial.print("RX: ");
          Serial.println(c);

          // 'S' = STOP command from phone
          if (c == 'S') {
            robotState = STOPPED;
          }

          // 'R' = RUN command from phone
          if (c == 'R') {
            robotState = RUNNING;
          }
        }
      }
    }

    Serial.println("Phone disconnected.");
    isConnected = false;
    updateBLELED();
    updateRobotLED();
    //digitalWrite(LED_BUILTIN, LOW); // no longer connected to phone.

  }
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