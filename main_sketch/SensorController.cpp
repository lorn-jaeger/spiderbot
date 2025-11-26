#include "SensorController.h"

// Pull pin globals from globals.cpp
//extern int irLeftPin;
//extern int irMiddlePin;
//extern int irRightPin;
//extern int crosswalkPin;
//extern int obstaclePin;

const int ULTRASONIC_THRESHOLD = 10; // in cm


extern RobotState currentState;   // global robot state variable

void SensorController::begin() {
    //pinMode(CROSSWALK_PIN, INPUT);
    //pinMode(OBSTACLE_PIN, INPUT);

    // IR sensors are analog — no pinMode needed
}

void SensorController::setThreshold(int threshold) {
    _threshold = threshold;
}

long SensorController::readUltrasonic(){

  //trigger pulse
  digitalWrite(obstacleTrigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(obstacleTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(obstacleTrigPin, LOW);

  // read echo pulse
  long duration = pulseIn(obstacleEchoPin, HIGH, 30000); // 30ms timeout

  if(duration == 0){
    return -1; // no reading
  }

  return duration / 29 / 2; // in cm
}

void SensorController::poll() {
    unsigned long now = millis();
    if (now - _lastRead < _pollInterval) return;
    _lastRead = now;

    // === Read sensors ===
    int L_raw = analogRead(irLeftPin);
    int M_raw = analogRead(irMiddlePin);
    int R_raw = analogRead(irRightPin);
    int crosswalk = analogRead(crosswalkPin);

    bool L = L_raw > _threshold;
    bool M = M_raw > _threshold;
    bool R = R_raw > _threshold;

    bool obstacle  = readUltrasonic() <= ULTRASONIC_THRESHOLD;

    // === State Decision Logic ===

    //TODO: Verify these states
    // Obstacle has highest priority
    if (obstacle) {
        currentState = OBSTACLE_STOP;
        return;
    }

    if (crosswalk) {
        currentState = CROSSWALK;
        return;
    }

    // Intersection (all three)
    if (L && M && R) {
        currentState = INTERSECTION;
        return;
    }

    // Turns
    if (L && !M && !R) {
        currentState = TURN_LEFT;
        return;
    }

    if (R && !M && !L) {
        currentState = TURN_RIGHT;
        return;
    }

    // Follow line (middle only)
    if (M && !L && !R) {
        currentState = FOLLOW_LINE;
        return;
    }

    // Centering (L+R both active)
    if (L && R && !M) {
        currentState = FOLLOW_LINE;
        return;
    }

    // End of road (none)
    if (!L && !M && !R) {
        currentState = END_OF_ROAD_TURN;
        return;
    }


}
