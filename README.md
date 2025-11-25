# HexWalker Autonomous Robot
Arduino Nano ESP32 • BLE Control • IR Line Following • Modular Architecture

This project implements an autonomous hexwalker robot that navigates using IR line sensors, obstacle detection, and crosswalk sensing, with remote BLE control from an Android device. The codebase is fully modular so each team member can work on their subsystem independently.

------------------------------------------------------------
FEATURES
------------------------------------------------------------

AUTONOMOUS NAVIGATION
- Left, middle, and right IR sensors
- Obstacle detection
- Crosswalk detection
- End-of-road detection (turn-around behavior)
- Curve handling (slight/hard left or right)
- Intersection handling (M + L/R logic)

BLUETOOTH CONTROL (NANO ESP32)
- BLE UART service using Nordic UART UUIDs
- Android app commands:
    R → Run autonomous mode
    S → Stop the robot

MODULAR CODE ARCHITECTURE
The project is split into multiple .ino files:

HexWalker/
    main_sketch.ino          // Main setup() and loop()
    bluetooth.ino          // BLE initialization and command handling
    ir.ino                  // IR sensor inputs for path following
    ultrasonic.ino          // ultrasonic sensor inputs for obstacle sensing
    state_machine.ino      // Autonomous behavior and state machine
    leds.ino               // RGB LED state indicators
    robot_states.h         // Shared enums and global variables

Each subsystem is isolated for team collaboration.

------------------------------------------------------------
HOW THE ROBOT WORKS
------------------------------------------------------------

SENSOR INPUTS
- IR Left / Middle / Right: detects line position
- Crosswalk sensor
- Obstacle sensor

STATE MACHINE LOGIC
Priority order:
1. Obstacle detected → stop
2. Crosswalk detected → slow / wait / resume
3. Intersection detected → follow intersection routine
4. Curve detection (100, 110, 010, 011, 001)
5. Straight path
6. Lost condition
7. End-of-road (all sensors = 0) → turn around

MOVEMENTS
- move_forward()
- move_forward_left()
- move_forward_right()
- turn_left()
- turn_right()
- stop_moving()

------------------------------------------------------------
BLUETOOTH COMMANDS
------------------------------------------------------------

Commands sent over Nordic UART Service:
- R → Start autonomous navigation
- S → Stop immediately

The robot reports status back over the TX characteristic.

------------------------------------------------------------
HARDWARE REQUIREMENTS
------------------------------------------------------------

- Arduino Nano ESP32
- Motor driver (L293D or similar)
- Hexwalker motors or DC motors
- IR sensors (L, M, R)
- Obstacle sensor (IR or ultrasonic)
- Crosswalk reflectivity sensor
- RGB LED (built-in or external)
- Battery pack (LiPo recommended)

------------------------------------------------------------
SETUP INSTRUCTIONS
------------------------------------------------------------

1. Install ESP32 board support in Arduino IDE:
   Tools → Board Manager → search "esp32" → Install

2. Select the correct board:
   Tools → Board → Arduino ESP32 → Arduino Nano ESP32

3. Select your serial port:
   Tools → Port → (your COM or /dev/tty port)

4. Place all .ino files in the same directory.

5. Compile and upload.

------------------------------------------------------------
LED STATUS INDICATORS
------------------------------------------------------------

- Red    = Robot stopped
- Green  = Moving forward / normal operation
- Blue   = Turning
- Purple = Lost condition (needs recovery)
- Off    = No BLE connection

------------------------------------------------------------
PROJECT FILE STRUCTURE
------------------------------------------------------------

HexWalker/
    HexWalker.ino
    bluetooth.ino
    motors.ino
    sensors.ino
    leds.ino
    state_machine.ino
    robot_states.h

------------------------------------------------------------
TESTING PROCEDURE
------------------------------------------------------------

1. Connect the Android app to the Nano ESP32 via BLE.
2. Confirm LED shows solid connection (green/blue/etc depending on state).
3. Test the robot on:
    - Straight lines
    - Curved paths
    - Intersections
    - Dead ends
4. Send "S" to stop the robot.
5. Send "R" to resume autonomous mode.

------------------------------------------------------------

