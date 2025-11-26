#ifndef SENSOR_CONTROLLER_H
#define SENSOR_CONTROLLER_H

#include <Arduino.h>
#include "globals.h"

class SensorController {
public:
    void begin();
    void poll();
    void setThreshold(int threshold);

private:

  
    int _threshold = 1800;
    unsigned long _lastRead = 0;
    const unsigned long _pollInterval = 20; // 50 Hz

    long _ultrasonicDuration;
    long _ultrasonicDistance;
    bool usO;
    void readUltrasonic();

    bool irR;
    bool irM;
    bool irL;
    bool irC;
    void readIR();


};

#endif
