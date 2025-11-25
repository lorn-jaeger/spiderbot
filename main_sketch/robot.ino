// ====================================================
// Motor Movement Helper Functions
// ====================================================

void move_forward(int time) {
  Serial.println("Moving robot forward");
  digitalWrite(forwardPin, LOW);
  digitalWrite(backwardPin, HIGH);
  digitalWrite(leftTurnPin, HIGH);
  digitalWrite(rightTurnPin, HIGH);
  delay(time);
}

void move_backward(int time) {
  Serial.println("Moving robot backward");
  digitalWrite(forwardPin, HIGH);
  digitalWrite(backwardPin, LOW);
  digitalWrite(leftTurnPin, HIGH);
  digitalWrite(rightTurnPin, HIGH);
  delay(time);
}

void move_forward_left(int time) {
  Serial.println("Moving robot forward and left");
  digitalWrite(forwardPin, LOW);
  digitalWrite(backwardPin, HIGH);
  digitalWrite(leftTurnPin, LOW);
  digitalWrite(rightTurnPin, HIGH);
  delay(time);
}

void move_forward_right(int time) {
  Serial.println("Moving robot forward and right");
  digitalWrite(forwardPin, LOW);
  digitalWrite(backwardPin, HIGH);
  digitalWrite(leftTurnPin, HIGH);
  digitalWrite(rightTurnPin, LOW);
  delay(time);
}

void turn_right(int time) {
  Serial.println("Turning robot right");
  digitalWrite(forwardPin, HIGH);
  digitalWrite(backwardPin, HIGH);
  digitalWrite(leftTurnPin, HIGH);
  digitalWrite(rightTurnPin, LOW);
  delay(time);
}

void turn_left(int time) {
  Serial.println("Turning robot left");
  digitalWrite(forwardPin, HIGH);
  digitalWrite(backwardPin, HIGH);
  digitalWrite(leftTurnPin, LOW);
  digitalWrite(rightTurnPin, HIGH);
  delay(time);
}

void stop_moving(int time) {
  Serial.println("Stopping robot");
  digitalWrite(forwardPin, HIGH);
  digitalWrite(backwardPin, HIGH);
  digitalWrite(leftTurnPin, HIGH);
  digitalWrite(rightTurnPin, HIGH);
  delay(time);
}