// bluetooth.ino
#include "globals.h"
BLEService uartService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");

BLECharacteristic txChar(
  "6E400003-B5A3-F393-E0A9-E50E24DCCA9E",
  BLERead | BLENotify,
  20
);

BLECharacteristic rxChar(
  "6E400002-B5A3-F393-E0A9-E50E24DCCA9E",
  BLEWrite | BLEWriteWithoutResponse,
  20
);

// Exposed state so main program can read commands
//extern bool isConnected;

void onConnect(BLEDevice central) {
    Serial.println("[BLE EVENT] Device connected!");
    isConnectedBLE = true;
}

void onDisconnect(BLEDevice central) {
    Serial.println("[BLE EVENT] Device disconnected.");
    isConnectedBLE = false;
}
// --------------------------
// Event-driven RX callback
// --------------------------
void onRXWrite(BLEDevice central, BLECharacteristic characteristic) {
    int len = characteristic.valueLength();
    const uint8_t* data = characteristic.value();

    for (int i = 0; i < len; i++) {
        char c = (char)data[i];
        Serial.print("[BLE EVENT] RX: ");
        Serial.println(c);
      //TODO: Implement bot inital states
    }
}

// Forward declarations
void setupBluetooth() {
  if (!BLE.begin()) {
    Serial.println("BLE init failed");
    while (1);
  }

  BLE.setLocalName("HexBug_1");
  BLE.setAdvertisedService(uartService);

  uartService.addCharacteristic(txChar);
  uartService.addCharacteristic(rxChar);
  BLE.addService(uartService);
  
  rxChar.setEventHandler(BLEWritten, onRXWrite);
  BLE.setEventHandler(BLEConnected, onConnect);
  BLE.setEventHandler(BLEDisconnected, onDisconnect);


  BLE.advertise();
  Serial.println("BLE advertising...");
}


void pollBluetooth() {
  BLE.poll();

  BLEDevice central = BLE.central();
  isConnectedBLE = central && central.connected();

}