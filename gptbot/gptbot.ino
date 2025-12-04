#include <ArduinoBLE.h>

char state = 0;
char lastState = 0;
String lastAction = "";

// Line sensors (all analog, ~1 inch apart, front of bot)
// Our Nano ESP32 pinout:
//  A0 = left, A2 = middle, A1 = right
const int leftSensorPin   = A0;
const int middleSensorPin = A2;
const int rightSensorPin  = A1;
const bool LINE_IS_DARK   = false;  // yellow line is brighter than floor; flip to true if yours is darker

// Ultrasonic pins (front)
const int echoPin = 2;
const int trigPin = 3;

// Motor pins (active LOW to run)
const int forwardPin  = 5;
const int backwardPin = 4;
const int leftPin     = 6;
const int rightPin    = 7;

// BLE UART service
BLEService myService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
BLECharacteristic rxChar("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWrite, 20);
BLECharacteristic txChar("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLENotify, 20);

// Logic Variables
bool  doObsRecovery = false;
float lastDistance  = 100;

void setup() {
  Serial.begin(9600);

  // BLE setup
  BLE.begin();
  BLE.setLocalName("hexbot");
  BLE.setAdvertisedService(myService);
  myService.addCharacteristic(rxChar);
  myService.addCharacteristic(txChar);
  BLE.addService(myService);
  BLE.advertise();

  // Pin setup
  pinMode(forwardPin,  OUTPUT);
  pinMode(backwardPin, OUTPUT);
  pinMode(leftPin,     OUTPUT);
  pinMode(rightPin,    OUTPUT);

  pinMode(leftSensorPin,    INPUT);
  pinMode(middleSensorPin,  INPUT);
  pinMode(rightSensorPin,   INPUT);
  pinMode(trigPin,          OUTPUT);
  pinMode(echoPin,          INPUT);

  // Start idle
  digitalWrite(forwardPin,  HIGH);
  digitalWrite(backwardPin, HIGH);
  digitalWrite(leftPin,     HIGH);
  digitalWrite(rightPin,    HIGH);

  Serial.println("Bot Ready. Waiting for BLE connection...");
}

struct LineSensor {
  int   pin;
  float smooth = 0;
  float minSeen = 1e9;
  float maxSeen = -1e9;
  bool  onLine = false;
};

LineSensor sLeft  { leftSensorPin };
LineSensor sMid   { middleSensorPin };
LineSensor sRight { rightSensorPin };

float score(float v) {
  return LINE_IS_DARK ? v : -v;  // invert if line is brighter
}

bool updateSensor(LineSensor &s) {
  const float alpha = 0.25f;   // smoothing for noise
  const float hyst  = 50.0f;   // hysteresis band
  int raw = analogRead(s.pin);
  if (s.minSeen > 1e8) {       // first sample
    s.smooth = raw;
    s.minSeen = s.maxSeen = score(raw);
  } else {
    s.smooth = alpha * raw + (1.0f - alpha) * s.smooth;
    float sv = score(s.smooth);
    if (sv < s.minSeen) s.minSeen = sv;
    if (sv > s.maxSeen) s.maxSeen = sv;
  }

  // Adaptive threshold in signed space
  float sv   = score(s.smooth);
  float thr  = s.minSeen + 0.55f * (s.maxSeen - s.minSeen); // bias slightly toward floor to avoid false ON

  if (s.onLine) {
    if (sv < thr - hyst) s.onLine = false;
  } else {
    if (sv > thr + hyst) s.onLine = true;
  }
  return s.onLine;
}

void moveStop() {
  digitalWrite(forwardPin,  HIGH);
  digitalWrite(backwardPin, HIGH);
  digitalWrite(leftPin,     HIGH);
  digitalWrite(rightPin,    HIGH);
}

void moveForward(int time = 0) {
  digitalWrite(forwardPin,  LOW);
  digitalWrite(backwardPin, HIGH);
  digitalWrite(leftPin,     HIGH);
  digitalWrite(rightPin,    HIGH);
  delay(time);
}

void moveBackward(int time = 0) {
  digitalWrite(forwardPin,  HIGH);
  digitalWrite(backwardPin, LOW);
  digitalWrite(leftPin,     HIGH);
  digitalWrite(rightPin,    HIGH);
  delay(time);
}

void moveTurnLeft(int time = 0) {
  digitalWrite(forwardPin,  HIGH);
  digitalWrite(backwardPin, HIGH);
  digitalWrite(leftPin,     LOW);
  digitalWrite(rightPin,    HIGH);
  delay(time);
}

void moveTurnRight(int time = 0) {
  digitalWrite(forwardPin,  HIGH);
  digitalWrite(backwardPin, HIGH);
  digitalWrite(leftPin,     HIGH);
  digitalWrite(rightPin,    LOW);
  delay(time);
}

long microsecondsToInches(long duration_ms){
  return (34 * duration_ms / 1000 / 2 / 2.54);
}

long ultra(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  long distance_inches = microsecondsToInches(duration);

  if (distance_inches == 0){
    distance_inches = lastDistance;
  }

  lastDistance = distance_inches;
  return distance_inches;
}

void loop() {
  BLEDevice central = BLE.central();

  if (central && central.connected()) {

    // Receive BLE command
    if (rxChar.written()) {
      int len = rxChar.valueLength();
      String cmd = "";
      for (int i = 0; i < len; i++) {
        cmd += (char)rxChar.value()[i];
      }
      cmd.trim();

      if (cmd.length() > 0) {
        state = cmd[0];
        Serial.print("Received command: ");
        Serial.println(state);
      }
    }

    // Read front line sensors (all analog)
    bool midOn   = updateSensor(sMid);
    bool leftOn  = updateSensor(sLeft);
    bool rightOn = updateSensor(sRight);

    String action = "";
    long distance = ultra();

    // --- Behavior logic ---
    if (state == 'B') {

      // Obstacle handling (unchanged)
      if (doObsRecovery){
        moveTurnLeft(2000);
        moveStop();
        delay(25);
        doObsRecovery = false;
      }

      if (distance <= 2) {
        // obstacle within 2 inches
        moveStop();
        for (int timer = 0; timer <= 50; timer++){
          Serial.print("Iteration");
          Serial.println(timer);
          delay(100);

          if (ultra() > 4){
            // further than 4 inches
            return;
          }
        }
        // delayed for 5 seconds, rewind
        doObsRecovery = true;
        return;
      }

      // ======== LINE FOLLOWING (simple, slow, front 3 sensors) ========
      if (midOn) {
        action = "FORWARD";
        moveForward(80);    // small bite forward
        moveStop();
      } 
      else if (leftOn && !rightOn) {
        action = "TURN LEFT";
        moveTurnLeft(90);   // gentle pivot
        moveStop();
      } 
      else if (rightOn && !leftOn) {
        action = "TURN RIGHT";
        moveTurnRight(90);
        moveStop();
      } 
      else if (!leftOn && !rightOn && !midOn) {
        // nothing seen: keep slowly turning left to reacquire
        action = "SEARCH LEFT";
        moveTurnLeft(120);
        moveStop();
      } 
      else {
        // noisy combo: treat as centered
        action = "FORWARD";
        moveForward(80);
        moveStop();
      }
    } 
    else if (state == 'S') {
      // Stop
      moveStop();
      action = "BOT STOP";
    }

    // --- Only send BLE updates when action changes ---
    if (action != lastAction && action.length() > 0) {
      txChar.writeValue(action.c_str());
      Serial.println(action);
      lastAction = action;
    }

    // Optional debug
    /*
    Serial.print("L: ");  Serial.print(leftVal);
    Serial.print(" R: ");  Serial.print(rightVal);
    Serial.print(" FL: "); Serial.print(frontLeftOnLine);
    Serial.print(" FM: "); Serial.print(frontMidOnLine);
    Serial.print(" FR: "); Serial.print(frontRightOnLine);
    Serial.print(" | frontCount: "); Serial.print(frontCount);
    Serial.print(" | State: "); Serial.println(state);
    */
  }
}
