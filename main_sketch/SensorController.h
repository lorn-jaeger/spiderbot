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

    //SensorCallback _callback;
    int _threshold = 1800;
    unsigned long _lastRead = 0;
    const unsigned long _pollInterval = 20; // 50 Hz

    long readUltrasonic();

};

#endif
