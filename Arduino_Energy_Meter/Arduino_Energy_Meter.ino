
#include <Arduino.h>
#include <math.h>    // for expf()

// ------------------- Pins -------------------
const uint8_t PIN_TRIG   = D2;   // GPIO 4
const uint8_t PIN_ECHO   = D1;   // GPIO 5
const uint8_t LED_RED    = D5;   // GPIO 14
const uint8_t LED_GREEN  = D6;   // GPIO 12

// ----------------- ML model -----------------
// Paste your own W and B from training here
const float W =  0.48f;   // weight
const float B = -23.5f;   // bias  (shift)

// inline sigmoid + predict (returns 0 or 1)
static inline int predict(float x)
{
  float z = W * x + B;
  float p = 1.0f / (1.0f + expf(-z));   // sigmoid
  return p > 0.5f;                      // 1 = HIGH, 0 = LOW
}

// ------------ Ultrasonic reading ------------
float readDistanceCM()
{
  digitalWrite(PIN_TRIG, LOW);   delayMicroseconds(5);
  digitalWrite(PIN_TRIG, HIGH);  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  uint32_t us = pulseIn(PIN_ECHO, HIGH, 25000);   // timeout 25 ms
  if (us == 0) return -1;                         // out-of-range
  return us * 0.0343f / 2.0f;                     // cm
}

// ------------------- Setup -------------------
void setup()
{
  Serial.begin(115200);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(LED_RED,   OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  Serial.println("\nESP8266 - Ultra Tiny-ML demo ready");
}

// -------------------- Loop -------------------
void loop()
{
  float d = readDistanceCM();

  if (d < 0) {                    // sensor timed-out
    Serial.println("Distance: out-of-range");
    digitalWrite(LED_RED,   LOW);
    digitalWrite(LED_GREEN, LOW);
    delay(300);
    return;
  }

  int cls = predict(d);           // 0 = LOW, 1 = HIGH

  // LED logic
  digitalWrite(LED_RED,   cls);   // HIGH class  → RED on
  digitalWrite(LED_GREEN, !cls);  // LOW  class  → GREEN on

  Serial.printf("d = %5.1f cm  →  %s\n", d, cls ? "HIGH (RED)" : "LOW (GREEN)");
  delay(300);
}
