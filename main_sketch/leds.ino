// ====================================================
// LED/State Helper Functions
// ====================================================
#include "globals.h"

unsigned long lastBlinkTime = 0;
bool ledState = LOW;
const int BLINK_INTERVAL = 300;  // ms

void updateLED(){
  __updateBLELED();
  __updateRobotLED();
}

void __setLED(bool r, bool g, bool b) {
  // ACTIVE LOW LEDs
  digitalWrite(LED_R, r ? LOW : HIGH);
  digitalWrite(LED_G, g ? LOW : HIGH);
  digitalWrite(LED_B, b ? LOW : HIGH);
}

void __updateBLELED() {
  if (isConnectedBLE) {
    // SOLID ON when connected
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    // BLINKING when advertising
    unsigned long now = millis();
    if (now - lastBlinkTime >= BLINK_INTERVAL) {
      lastBlinkTime = now;
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState);
    }
  }
}

void __updateRobotLED() {

  if (!isConnectedBLE){
    __setLED(false, false, false);
    return;
  }

  if (currentState == IDLE) {
    // STOPPED = RED
    __setLED(true, false, false);
    return;
  }

}