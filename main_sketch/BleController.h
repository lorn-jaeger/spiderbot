#ifndef BLE_CONTROLLER_H
#define BLE_CONTROLLER_H

#include <ArduinoBLE.h>

typedef void (*BleCommandCallback)(char cmd);
typedef void (*BleConnectCallback)();
typedef void (*BleDisconnectCallback)();



class BleController {
public:
    BleController();

    void begin();
    void poll();
    void send(String msg);

    void setCommandCallback(BleCommandCallback cb);
    void setConnectCallback(BleConnectCallback cb);
    void setDisconnectCallback(BleDisconnectCallback cb);



private:
    BLEService uartService;
    BLECharacteristic txChar;
    BLECharacteristic rxChar;

    BleCommandCallback onCommand = nullptr;
    BleConnectCallback onConnect = nullptr;
    BleDisconnectCallback onDisconnect = nullptr;

    String lastSentString = "";

    bool isConnected = false;
    void handleRX();
    void handleTX();
    void updateLED();
    char* robotStateToString();

};

extern BleController ble;

#endif
