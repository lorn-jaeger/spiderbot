#include "SensorController.h"

// Pull pin globals from globals.cpp
//extern int irLeftPin;
//extern int irMiddlePin;
//extern int irRightPin;
//extern int crosswalkPin;
//extern int obstaclePin;

const int ULTRASONIC_DISTANCE_THRESHOLD = 10; // in cm


extern RobotState currentState;   // global robot state variable

void SensorController::begin() {
    //pinMode(CROSSWALK_PIN, INPUT);
    //pinMode(OBSTACLE_PIN, INPUT);

    // IR sensors are analog — no pinMode needed
    _ultrasonicDuration = -1;
    _ultrasonicDistance = -1;

    irR = false;
    irM = false;
    irL = false;
    irC = false;
    usO = false;
}

void SensorController::setThreshold(int threshold) {
    _threshold = threshold;
}

void SensorController::readUltrasonic(){

  //trigger pulse
  digitalWrite(obstacleTrigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(obstacleTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(obstacleTrigPin, LOW);

  // read echo pulse
  _ultrasonicDuration = pulseIn(obstacleEchoPin, HIGH, 30000); // 30ms timeout

  if(_ultrasonicDuration == 0){
    _ultrasonicDuration = -1;
    _ultrasonicDistance = -1;
    return; // no reading
  }

  //return duration / 29 / 2; // in cm
  _ultrasonicDistance = _ultrasonicDuration / 29 / 2;
  usO  = _ultrasonicDistance <= ULTRASONIC_DISTANCE_THRESHOLD;
}

void SensorController::readIR(){
  int L_raw = analogRead(irLeftPin);
  int M_raw = analogRead(irMiddlePin);
  int R_raw = analogRead(irRightPin);
  int crosswalk_raw = analogRead(crosswalkPin);

  irL = L_raw > _threshold;
  irM = M_raw > _threshold;
  irR = R_raw > _threshold;
  irC = crosswalk_raw > _threshold;

}

void SensorController::poll() {
    unsigned long now = millis();
    if (now - _lastRead < _pollInterval) return;
    _lastRead = now;

    // === Read sensors ===
    readIR();
    readUltrasonic();

    // === State Decision Logic ===

    //TODO: Verify these states
    // Obstacle has highest priority
    if (usO) {
        currentState = OBSTACLE_STOP;
        return;
    }

    if (irC) {
        currentState = CROSSWALK;
        return;
    }

    // Intersection (all three)
    if (irL && irM && irR) {
        currentState = INTERSECTION;
        return;
    }

    // Turns
    if (irL && !irM && !irR) {
        currentState = TURN_LEFT;
        return;
    }

    if (irR && !irM && !irL) {
        currentState = TURN_RIGHT;
        return;
    }

    // Follow line (middle only)
    if (irM && !irL && !irR) {
        currentState = FOLLOW_LINE;
        return;
    }

    // Centering (L+R both active)
    if (irL && irR && !irM) {
        currentState = FOLLOW_LINE;
        return;
    }

    // End of road (none)
    if (!irL && !irM && !irR) {
        currentState = END_OF_ROAD_TURN;
        return;
    }


}
