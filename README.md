
# Spiderbot - Line Following + Obstacle Avoidance (Remote-Control Interface)
-------------------------------------------------------------------------------
## Dependencies and Required Libraries
-------------------------------------------------------------------------------
1) Arduino IDE (or Arduino CLI)

2) ESP32 Board Support Package (Arduino ESP32 core)
   - Install via Arduino IDE Boards Manager:
     "esp32 by Espressif Systems"

3) Standard Arduino libraries included with the ESP32 core
   - Serial / HardwareSerial
   - (If used in your code) BluetoothSerial

No additional third-party libraries are required.

-------------------------------------------------------------------------------
### How to Obtain / Install Dependencies
-------------------------------------------------------------------------------
1) Install Arduino IDE.

2) Install the ESP32 board package:
   - Arduino IDE -> Tools -> Board -> Boards Manager
   - Search: "ESP32"
   - Install: "esp32 by Espressif Systems"

3) Select the correct board:
   - Tools -> Board
   - Choose the ESP32 board that matches your hardware
     (e.g., Arduino Nano ESP32 or a compatible ESP32 variant)

-------------------------------------------------------------------------------
## Step-by-Step Instructions to Run the Code
-------------------------------------------------------------------------------
1) Clone or download this repository.

2) Open the main sketch in Arduino IDE.

3) Connect the ESP32 to your computer via USB.

4) Configure Arduino IDE:
   - Tools -> Board: select your ESP32 model
   - Tools -> Port: select the correct serial/COM port

5) Confirm wiring matches the pin assignments in the code:
   - IR sensors (Left / Middle / Right)
   - Ultrasonic sensor (Trig / Echo)
   - Bluetooth/serial settings (if applicable)

6) Upload the sketch to the ESP32.

7) Place the robot on the track.

8) Connect to the robot using a Serial Bluetooth terminal.

9) Control commands:
   - 'R' = Run
   - 'S' = Stop

-------------------------------------------------------------------------------
## Notes / Common Setup Issues
-------------------------------------------------------------------------------
- IR sensors must be mounted at a consistent height and facing straight down.
- If line tracking is unstable, adjust IR thresholds or sensor height.
- Obstacle detection uses a ~10 cm threshold (adjust in code if needed).
- This project sends high-level movement commands through the robot’s
  remote-control interface rather than controlling motors directly.
