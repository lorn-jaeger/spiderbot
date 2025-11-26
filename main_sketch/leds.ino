// ====================================================
// LED/State Helper Functions
// ====================================================
#include "globals.h"

unsigned long lastBlinkTime = 0;
bool ledState = LOW;
const int BLINK_INTERVAL = 300;  // ms

void updateLED(){
  __updateRobotLED();
}

void __setLED(bool r, bool g, bool b) {
  // ACTIVE LOW LEDs
  digitalWrite(LED_R, r ? LOW : HIGH);
  digitalWrite(LED_G, g ? LOW : HIGH);
  digitalWrite(LED_B, b ? LOW : HIGH);
}


void __updateRobotLED() {

  if (!isConnectedBLE) {
    // Not connected = LED OFF
    __setLED(false, false, false);
    return;
  }

  switch (currentState) {

    case IDLE:
      // Idle = BLUE
      __setLED(false, false, true);
      return;

    case FOLLOW_LINE:
      // Normal driving = GREEN
      __setLED(false, true, false);
      return;

    case TURN_LEFT:
    case TURN_RIGHT:
      // Turning = YELLOW (RED + GREEN)
      __setLED(true, true, false);
      return;

    case INTERSECTION:
      // Intersection decision = PURPLE (RED + BLUE)
      __setLED(true, false, true);
      return;

    case CROSSWALK:
      // Crosswalk detected = CYAN (GREEN + BLUE)
      __setLED(false, true, true);
      return;

    case OBSTACLE_STOP:
      // Obstacle = RED
      __setLED(true, false, false);
      return;

    case END_OF_ROAD_TURN:
      // End of road turn-around = WHITE (R+G+B)
      __setLED(true, true, true);
      return;

    default:
      // Fallback: LED OFF
      __setLED(false, false, false);
      return;
  }
}