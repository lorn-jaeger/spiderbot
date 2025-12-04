// ====================================================
// Motor Movement Helper Functions
// ====================================================

enum Motion {
  MOTION_STOP,
  MOTION_FORWARD,
  MOTION_FORWARD_LEFT,
  MOTION_FORWARD_RIGHT,
  MOTION_TURN_LEFT,
  MOTION_TURN_RIGHT,
};

Motion currentMotion = MOTION_STOP;

void applyMotion(Motion motion) {
  if (motion == currentMotion) return;
  currentMotion = motion;

  switch (motion) {
    case MOTION_FORWARD:
      Serial.println("Motion: forward");
      digitalWrite(forwardPin, LOW);
      digitalWrite(backwardPin, HIGH);
      digitalWrite(leftTurnPin, HIGH);
      digitalWrite(rightTurnPin, HIGH);
      break;

    case MOTION_FORWARD_LEFT:
      Serial.println("Motion: forward + left bias");
      digitalWrite(forwardPin, LOW);
      digitalWrite(backwardPin, HIGH);
      digitalWrite(leftTurnPin, LOW);
      digitalWrite(rightTurnPin, HIGH);
      break;

    case MOTION_FORWARD_RIGHT:
      Serial.println("Motion: forward + right bias");
      digitalWrite(forwardPin, LOW);
      digitalWrite(backwardPin, HIGH);
      digitalWrite(leftTurnPin, HIGH);
      digitalWrite(rightTurnPin, LOW);
      break;

    case MOTION_TURN_LEFT:
      Serial.println("Motion: pivot left");
      digitalWrite(forwardPin, HIGH);
      digitalWrite(backwardPin, HIGH);
      digitalWrite(leftTurnPin, LOW);
      digitalWrite(rightTurnPin, HIGH);
      break;

    case MOTION_TURN_RIGHT:
      Serial.println("Motion: pivot right");
      digitalWrite(forwardPin, HIGH);
      digitalWrite(backwardPin, HIGH);
      digitalWrite(leftTurnPin, HIGH);
      digitalWrite(rightTurnPin, LOW);
      break;

    case MOTION_STOP:
    default:
      Serial.println("Motion: stop");
      digitalWrite(forwardPin, HIGH);
      digitalWrite(backwardPin, HIGH);
      digitalWrite(leftTurnPin, HIGH);
      digitalWrite(rightTurnPin, HIGH);
      break;
  }
}

void updateRobotMotion() {
  switch (currentState) {
    case FOLLOW_LINE:
      applyMotion(MOTION_FORWARD);
      break;

    case TURN_LEFT:
      applyMotion(MOTION_FORWARD_LEFT);
      break;

    case TURN_RIGHT:
      applyMotion(MOTION_FORWARD_RIGHT);
      break;

    case INTERSECTION:
      applyMotion(MOTION_FORWARD);
      break;

    case END_OF_ROAD_TURN:
      applyMotion(MOTION_TURN_LEFT);
      break;

    case CROSSWALK:
    case OBSTACLE_STOP:
    case IDLE:
    default:
      applyMotion(MOTION_STOP);
      break;
  }
}
