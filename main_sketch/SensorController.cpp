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
    //Serial.println("polling sensor data...");
    // === Read sensors ===
    readIR();
    readUltrasonic();

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

    // === Simple line-only state logic with noise handling ===
    // 1) If center sees the line, keep moving forward.
    // 2) If center is off but a side looks most like the line, pivot that way.
    // 3) If nothing is clear, continue the last turn direction to avoid ping-pong.
    static int lastDir = -1; // -1 left, 1 right, 0 center/unknown

    // Score helpers (lower is better when line is dark, higher is better when bright)
    auto score = [](float v, bool dark) {
        return dark ? v : -v;
    };

    float sL = score(_leftSensor.smoothValue, LINE_IS_DARK);
    float sM = score(_middleSensor.smoothValue, LINE_IS_DARK);
    float sR = score(_rightSensor.smoothValue, LINE_IS_DARK);

    if (irM) {
        currentState = FOLLOW_LINE;
        lastDir = 0;
        return;
    }

    // One side sees the line → steer that way
    if (irL && !irR) {
        currentState = TURN_LEFT;
        lastDir = -1;
        return;
    }
    if (irR && !irL) {
        currentState = TURN_RIGHT;
        lastDir = 1;
        return;
    }

    // No clean detections: compare which side looks more "line-like"
    if (!irL && !irR) {
        int bestDir = (sL < sR) ? -1 : 1;
        if (lastDir != 0) {
            bestDir = lastDir; // stick with previous lean to avoid flip-flop
        }
        currentState = (bestDir < 0) ? TURN_LEFT : TURN_RIGHT;
        lastDir = bestDir;
        return;
    }

    // Both side sensors think they're on the line (likely noise) → treat as centered
    currentState = FOLLOW_LINE;
    lastDir = 0;

}
