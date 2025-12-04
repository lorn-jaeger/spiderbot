#include "SensorController.h"


const int ULTRASONIC_DISTANCE_THRESHOLD = 10; // in cm
const bool LINE_IS_DARK = true;  // set false if line is lighter than floor


extern RobotState currentState;   // global robot state variable

void SensorController::begin() { 

    // IR sensors are analog — no pinMode needed
    _ultrasonicDuration = -1;
    _ultrasonicDistance = -1;

    irR = false;
    irM = false;
    irL = false;
    irC = false;
    usO = false;
    _leftSensor = IRSensorState();
    _middleSensor = IRSensorState();
    _rightSensor = IRSensorState();
    _crosswalkSensor = IRSensorState();
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
    usO = false;           // <-- clear obstacle when no reading
    return; // no reading
  }

  _ultrasonicDistance = _ultrasonicDuration / 29 / 2; // in cm
  usO  = _ultrasonicDistance <= ULTRASONIC_DISTANCE_THRESHOLD;
}


void SensorController::readIR(){
    irL = updateIRSensor(_leftSensor, irLeftPin);
    irM = updateIRSensor(_middleSensor, irMiddlePin);
    irR = updateIRSensor(_rightSensor, irRightPin);
    //irC = updateIRSensor(_crosswalkSensor, crosswalkPin);
}

bool SensorController::updateIRSensor(IRSensorState &sensor, int pin) {
    int rawValue = analogRead(pin);
    Serial.print(pin);
    Serial.print(" ");
    Serial.println(rawValue);

    if (!sensor.initialized) {
        sensor.smoothValue = rawValue;
        sensor.initialized = true;
    } else {
        sensor.smoothValue = _alpha * rawValue + (1.0f - _alpha) * sensor.smoothValue;
    }

    if (sensor.onLine) {
        if (sensor.smoothValue > _thresholdHigh) {
            sensor.onLine = false;
        }
    } else {
        if (sensor.smoothValue < _thresholdLow) {
            sensor.onLine = true;
        }
    }

    return sensor.onLine;
}


void SensorController::poll() {
    unsigned long now = millis();
    if (now - _lastRead < _pollInterval) return;
    _lastRead = now;

    // === Read sensors ===
    readIR();
    readUltrasonic();

    // --- Debug logging ---
    static unsigned long lastLog = 0;
    if (now - lastLog > 300) {
        lastLog = now;
        Serial.print("[IR] L=");
        Serial.print(_leftSensor.smoothValue, 1);
        Serial.print(irL ? " (ON)" : " (  )");
        Serial.print("  M=");
        Serial.print(_middleSensor.smoothValue, 1);
        Serial.print(irM ? " (ON)" : " (  )");
        Serial.print("  R=");
        Serial.print(_rightSensor.smoothValue, 1);
        Serial.print(irR ? " (ON)" : " (  )");
        Serial.print(" | US=");
        Serial.print(_ultrasonicDistance);
        Serial.println("cm");
    }

    // =====================================================
    // STATIC STATE FOR INTERSECTION & LINE RECOVERY
    // =====================================================
    static bool intersectionActive = false;           // currently handling an intersection
    static bool intersectionForwardPhase = false;     // in the 2s "move forward" phase
    static bool intersectionRearm = false;            // wait until sensors leave intersection before rearming
    static unsigned long intersectionStart = 0;
    static unsigned long intersectionForwardStart = 0;

    static unsigned long lastCenterOn = 0;
    static bool centerInit = false;
    static int lastDir = 0;    // -1 = left, 1 = right, 0 = straight
    static int searchDir = 0;  // -1 = left search, 1 = right search, 0 = none

    bool tripleOn = (irL && irM && irR);

    if (!centerInit) {
        lastCenterOn = now;
        centerInit = true;
    }
    if (irM) {
        // center sensor currently on line → reset timer
        lastCenterOn = now;
    }

    // =====================================================
    // 1) OBSTACLE OVERRIDE: stop immediately
    // =====================================================
    if (usO) {
        currentState = OBSTACLE_STOP;
        return;
    }

    // =====================================================
    // 2) INTERSECTION HANDLING
    //    - Phase 1: stop for 5 seconds when L,M,R all 1
    //    - Phase 2: move straight forward for 2 seconds
    //    - Then re-arm only after we've left the intersection
    // =====================================================

    // If we're in the middle of an intersection sequence:
    if (intersectionActive) {
        if (!intersectionForwardPhase) {
            // Phase 1: 5-second stop
            if (now - intersectionStart < 5000UL) {   // 5 seconds
                currentState = INTERSECTION;          // RobotController should stop_moving() here
                return;
            } else {
                // start Phase 2: 2s forward
                intersectionForwardPhase = true;
                intersectionForwardStart = now;
            }
        }

        if (intersectionForwardPhase) {
            if (now - intersectionForwardStart < 2000UL) { // 2 seconds forward
                // Force straight motion, ignore side sensors during this phase
                currentState = FOLLOW_LINE;
                return;
            } else {
                // Intersection sequence fully done
                intersectionActive = false;
                intersectionForwardPhase = false;
                intersectionRearm = true;   // don't trigger intersection again until we leave it

                // Reset "lost center" timer so we don't instantly go into search mode
                lastCenterOn = now;
                searchDir = 0;
                lastDir = 0;
                // fall through to normal behavior
            }
        }
    }

    // Re-arm intersection detection only after we've left the 3-sensors-on region
    if (intersectionRearm && !tripleOn) {
        intersectionRearm = false;
    }

    // Detect a NEW intersection if not in a sequence and armed
    if (!intersectionActive && !intersectionRearm && tripleOn) {
        intersectionActive = true;
        intersectionForwardPhase = false;
        intersectionStart = now;
        currentState = INTERSECTION;   // will stop for 5 seconds
        return;
    }

    // =====================================================
    // 3) "LOST LINE" RECOVERY:
    //    If center hasn't seen line for 2s, search in opposite direction
    // =====================================================
    const unsigned long NO_CENTER_TIMEOUT = 2000UL; // 2 seconds
    unsigned long sinceCenter = now - lastCenterOn;

    if (sinceCenter > NO_CENTER_TIMEOUT) {
        // Choose opposite of last turn; if unknown, default to turning right.
        if (searchDir == 0) {
            if (lastDir < 0)       searchDir = 1;   // was turning left → search right
            else if (lastDir > 0)  searchDir = -1;  // was turning right → search left
            else                   searchDir = 1;   // no history → just pick right
        }

        currentState = (searchDir < 0) ? TURN_LEFT : TURN_RIGHT;
        return;
    } else {
        // Back in normal tracking window, disable search
        searchDir = 0;
    }

    // =====================================================
    // 4) NORMAL LINE-FOLLOWING USING ONLY LEFT & RIGHT
    //    (middle sensor only used for the 2s timer above)
    // =====================================================
    if (!irL && !irR) {
        currentState = FOLLOW_LINE;
        lastDir = 0;
    }
    else if (irL && !irR) {
        currentState = TURN_LEFT;
        lastDir = -1;
    }
    else if (irR && !irL) {
        currentState = TURN_RIGHT;
        lastDir = 1;
    }
    else {
        // both side sensors see line → treat as centered
        currentState = FOLLOW_LINE;
        lastDir = 0;
    }
}


