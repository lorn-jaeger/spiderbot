#ifndef SENSOR_CONTROLLER_H
#define SENSOR_CONTROLLER_H

#include <Arduino.h>
#include "globals.h"

class SensorController {
public:
    void begin();
    void poll();

private:

    struct IRSensorState {
        float smoothValue = 0.0f;
        bool onLine = false;
        bool initialized = false;
    };

  
    int _thresholdLow = 1000;
    int _thresholdHigh = 2000;
    const float _alpha = 0.3f;
    unsigned long _lastRead = 0;
    const unsigned long _pollInterval = 40; // 50 Hz

    long _ultrasonicDuration;
    long _ultrasonicDistance;
    bool usO;
    void readUltrasonic();

    bool irR;
    bool irM;
    bool irL;
    bool irC;
    IRSensorState _leftSensor;
    IRSensorState _middleSensor;
    IRSensorState _rightSensor;
    IRSensorState _crosswalkSensor;
    void readIR();

    bool updateIRSensor(IRSensorState &sensor, int pin);


};

#endif
