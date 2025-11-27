#include <Arduino.h>

constexpr int kIrPin = A1;
constexpr int kAveragingSamples = 8;
constexpr uint32_t kSampleDelayUs = 500;

constexpr uint32_t kHoldTimeMs = 5000;
constexpr uint32_t kSampleWindowMs = 20000;
constexpr int kStableCountNeeded = 3; 

struct Stats {
    long sum = 0;
    int count = 0;
    int min = 1023;
    int max = 0;

    void add(int v)
    {
        sum += v;
        count++;
        if (v < min) min = v;
        if (v > max) max = v;
    }

    int mean() const { return count > 0 ? static_cast<int>(sum / count) : 0; }
};

enum class Phase {
    WaitBlack,
    SampleBlack,
    WaitYellow,
    SampleYellow,
    Monitor
};

Phase phase = Phase::WaitBlack;
unsigned long phaseStart = 0;
Stats blackStats;
Stats yellowStats;
int threshold = 0;
bool yellowIsHigher = true;
bool onYellow = false;
bool candidateState = false;
int candidateCount = 0;
unsigned long lastPrint = 0;

int readAveraged()
{
    long sum = 0;
    for (int i = 0; i < kAveragingSamples; ++i) {
        sum += analogRead(kIrPin);
        delayMicroseconds(kSampleDelayUs);
    }
    return static_cast<int>(sum / kAveragingSamples);
}

void printSummary()
{
    Serial.println("== Summary ==");
    Serial.print("Black  mean=");
    Serial.print(blackStats.mean());
    Serial.print(" min=");
    Serial.print(blackStats.min);
    Serial.print(" max=");
    Serial.println(blackStats.max);

    Serial.print("Yellow mean=");
    Serial.print(yellowStats.mean());
    Serial.print(" min=");
    Serial.print(yellowStats.min);
    Serial.print(" max=");
    Serial.println(yellowStats.max);

    Serial.print("Threshold=");
    Serial.print(threshold);
    Serial.print(" (yellow is ");
    Serial.print(yellowIsHigher ? "brighter" : "darker");
    Serial.println(")");
    Serial.println("Swap between surfaces; I will print when I detect a change.");
}

void setup()
{
    Serial.begin(9600);
    pinMode(kIrPin, INPUT);
    phaseStart = millis();
    Serial.println("IR test starting.");
    Serial.println("Hold sensor over BLACK in 5 seconds...");
}

void loop()
{
    const unsigned long now = millis();
    const int v = readAveraged();

    switch (phase) {
    case Phase::WaitBlack:
        if (now - phaseStart >= kHoldTimeMs) {
            phase = Phase::SampleBlack;
            phaseStart = now;
            blackStats = Stats{};
            Serial.println("Sampling BLACK for 20 seconds...");
        }
        break;

    case Phase::SampleBlack:
        blackStats.add(v);
        if (now - phaseStart >= kSampleWindowMs) {
            phase = Phase::WaitYellow;
            phaseStart = now;
            Serial.println("Hold sensor over YELLOW in 5 seconds...");
        }
        break;

    case Phase::WaitYellow:
        if (now - phaseStart >= kHoldTimeMs) {
            phase = Phase::SampleYellow;
            phaseStart = now;
            yellowStats = Stats{};
            Serial.println("Sampling YELLOW for 20 seconds...");
        }
        break;

    case Phase::SampleYellow:
        yellowStats.add(v);
        if (now - phaseStart >= kSampleWindowMs) {
            const int blackMean = blackStats.mean();
            const int yellowMean = yellowStats.mean();
            yellowIsHigher = yellowMean > blackMean;
            threshold = (blackMean + yellowMean) / 2;
            onYellow = yellowIsHigher ? (v >= threshold) : (v <= threshold);
            candidateState = onYellow;
            candidateCount = 0;
            phase = Phase::Monitor;
            printSummary();
        }
        break;

    case Phase::Monitor: {
        const bool expectedYellow = yellowIsHigher ? (v >= threshold) : (v <= threshold);
        if (expectedYellow == candidateState) {
            candidateCount = min(candidateCount + 1, kStableCountNeeded);
        } else {
            candidateState = expectedYellow;
            candidateCount = 1;
        }

        if (candidateCount >= kStableCountNeeded && expectedYellow != onYellow) {
            onYellow = expectedYellow;
            if (onYellow) {
                Serial.print("Detected YELLOW at v=");
                Serial.println(v);
            } else {
                Serial.print("Detected BLACK at v=");
                Serial.println(v);
            }
        }

        if (now - lastPrint >= 1000) {
            lastPrint = now;
            Serial.print("v=");
            Serial.print(v);
            Serial.print(" thr=");
            Serial.print(threshold);
            Serial.print(" state=");
            Serial.println(onYellow ? "YELLOW" : "BLACK");
        }
    } break;
    }
}
