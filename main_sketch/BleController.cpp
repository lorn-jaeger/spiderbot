#include "BleController.h"

BleController::BleController()
    : uartService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E"),
      txChar("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLERead | BLENotify, 20),
      rxChar("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWrite | BLEWriteWithoutResponse, 20)
{}

void BleController::begin() {

    if (!BLE.begin()) {
        Serial.println("BLE init failed!");
        while (1);
    }

    BLE.setLocalName("HexBug_1");
    BLE.setAdvertisedService(uartService);

    uartService.addCharacteristic(txChar);
    uartService.addCharacteristic(rxChar);

    BLE.addService(uartService);

    rxChar.setEventHandler(BLEWritten, [](BLEDevice central, BLECharacteristic characteristic) {
        // Event is forwarded into class method
        // NOTE: Stored instance accesses are tricky in static callbacks,
        // so instead polling() will call handleRX().
    });

    BLE.advertise();
    Serial.println("BLE now advertising...");
}

void BleController::poll() {
    BLEDevice central = BLE.central();

    // Check for connect event
    if (central && central.connected()) {

        if (!isConnected) {
            isConnected = true;
            if (onConnect) onConnect();
        }

        updateLED();
        handleRX();
    }
    else {
        if (isConnected) {
            isConnected = false;
            if (onDisconnect) onDisconnect();
        }

        updateLED();
    }

    BLE.poll();
}

void BleController::handleRX() {

    if (!rxChar.written()) return;

    int len = rxChar.valueLength();
    const uint8_t* data = rxChar.value();

    for (int i = 0; i < len; i++) {
        char c = (char)data[i];

        // Skip junk characters
        if (c == '\r' || c == '\n' || c == '\0') continue;

        Serial.print("[BLE COMMAND] Received: ");
        Serial.println(c);

        if (onCommand) {
            onCommand(c);
        }
    }
}

void BleController::updateLED() {
    // ON when connected, slow blink when not
    if (isConnected) {
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
        static unsigned long last = 0;
        unsigned long now = millis();
        if (now - last > 300) {
            last = now;
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        }
    }
}

void BleController::setCommandCallback(BleCommandCallback cb) {
    onCommand = cb;
}

void BleController::setConnectCallback(BleConnectCallback cb) {
    onConnect = cb;
}

void BleController::setDisconnectCallback(BleDisconnectCallback cb) {
    onDisconnect = cb;
}
