#include <Arduino.h>

constexpr int kIrPin = A1;
constexpr int kSamples = 16;
constexpr uint32_t kSampleDelayUs = 200;
constexpr uint32_t kPrintIntervalMs = 100;

int blackMark = -1;
int tapeMark = -1;
int minSeen = 1023;
int maxSeen = 0;
unsigned long lastPrint = 0;

int readAveraged()
{
    long sum = 0;
    for (int i = 0; i < kSamples; ++i) {
        sum += analogRead(kIrPin);
        delayMicroseconds(kSampleDelayUs);
    }
    return static_cast<int>(sum / kSamples);
}

void printStatus(int v)
{
    int suggestion = -1;
    if (blackMark >= 0 && tapeMark >= 0) {
        suggestion = (blackMark + tapeMark) / 2;
    }

    Serial.print("v=");
    Serial.print(v);
    Serial.print(" min=");
    Serial.print(minSeen);
    Serial.print(" max=");
    Serial.print(maxSeen);
    Serial.print(" black=");
    if (blackMark >= 0) {
        Serial.print(blackMark);
    } else {
        Serial.print("-");
    }
    Serial.print(" tape=");
    if (tapeMark >= 0) {
        Serial.print(tapeMark);
    } else {
        Serial.print("-");
    }
    Serial.print(" thr=");
    if (suggestion >= 0) {
        Serial.print(suggestion);
    } else {
        Serial.print("-");
    }
    Serial.println();
}

void setup()
{
    Serial.begin(9600);
    pinMode(kIrPin, INPUT);
    Serial.println("IR threshold tester ready.");
    Serial.println("Commands: 'b' mark black, 't' mark tape, 'r' reset min/max.");
}

void loop()
{
    const int v = readAveraged();
    if (v < minSeen) {
        minSeen = v;
    }
    if (v > maxSeen) {
        maxSeen = v;
    }

    while (Serial.available()) {
        const char c = Serial.read();
        if (c == 'b') {
            blackMark = v;
            Serial.print("Marked black at ");
            Serial.println(v);
        } else if (c == 't') {
            tapeMark = v;
            Serial.print("Marked tape at ");
            Serial.println(v);
        } else if (c == 'r') {
            minSeen = 1023;
            maxSeen = 0;
            Serial.println("Reset min/max.");
        }
    }

    const unsigned long now = millis();
    if (now - lastPrint >= kPrintIntervalMs) {
        lastPrint = now;
        printStatus(v);
    }
}
