#include "SensorController.h"


const int ULTRASONIC_DISTANCE_THRESHOLD = 10; // in cm


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
    return; // no reading
  }

  //return duration / 29 / 2; // in cm
  _ultrasonicDistance = _ultrasonicDuration / 29 / 2;
  usO  = _ultrasonicDistance <= ULTRASONIC_DISTANCE_THRESHOLD;
}

void SensorController::readIR(){
    irL = updateIRSensor(_leftSensor, irLeftPin);
    irM = updateIRSensor(_middleSensor, irMiddlePin);
    irR = updateIRSensor(_rightSensor, irRightPin);
    //irC = updateIRSensor(_crosswalkSensor, crosswalkPin);
    Serial.print("IR L:");
    Serial.print(irL ? 1 : 0);
    Serial.print(" M:");
    Serial.print(irM ? 1 : 0);
    Serial.print(" R:");
    Serial.println(irR ? 1 : 0);
}

bool SensorController::updateIRSensor(IRSensorState &sensor, int pin) {
    int rawValue = analogRead(pin);
    // Serial.print(pin);
    // Serial.print(" ");
    // Serial.println(rawValue);

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
    //Serial.println("polling sensor data...");
    // === Read sensors ===
    readIR();
    readUltrasonic();

    static unsigned long lastLog = 0;
    if (now - lastLog > 200) {
        lastLog = now;
        Serial.print("IR L=");
        Serial.print(_leftSensor.smoothValue);
        Serial.print(" M=");
        Serial.print(_middleSensor.smoothValue);
        Serial.print(" R=");
        Serial.print(_rightSensor.smoothValue);
        Serial.print(" | flags L");
        Serial.print(irL);
        Serial.print(" M");
        Serial.print(irM);
        Serial.print(" R");
        Serial.print(irR);
        Serial.print(" | US=");
        Serial.println(_ultrasonicDistance);
    }

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
    if ((irL && irM && irR) || (irL && irM && !irR) || (!irL && irM && irR)) {
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
