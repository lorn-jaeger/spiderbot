// bluetooth.ino
#include <ArduinoBLE.h>

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
extern bool isConnected;
extern RobotState robotState;

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

  BLE.advertise();
  Serial.println("BLE advertising...");
}

void pollBluetooth() {
  BLE.poll();

  BLEDevice central = BLE.central();
  isConnected = central && central.connected();

  if (central) {
    while (central.connected()) {
      BLE.poll();

      if (rxChar.written()) {
        for (int i = 0; i < rxChar.valueLength(); i++) {
          char c = (char)rxChar.value()[i];

          if (c == 'R') robotState = RUNNING;
          if (c == 'S') robotState = STOPPED;
        }
      }
    }
  }
}