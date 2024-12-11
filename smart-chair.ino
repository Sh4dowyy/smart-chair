#include <Adafruit_NeoPixel.h>

#define SENSOR_PIN1 A0
#define SENSOR_PIN2 A1
#define LED_PIN 6
#define MOTOR_PIN 5
#define NUM_LEDS 8
#define VOLTAGE_THRESHOLD 0.5

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

int litLeds = 0;
unsigned long lastUpdate = 0;
bool motorActive = false;
int motorIntensity = 255;

unsigned long motorVibrationStart = 0;
unsigned long motorPauseStart = 0;
bool vibrating = false;
unsigned long vibrationDuration = 1000;
unsigned long pauseDuration = 500;
unsigned long lastVibrationCycle = 0;

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(50);
  strip.show();
  pinMode(MOTOR_PIN, OUTPUT);
  analogWrite(MOTOR_PIN, 0);
}

void loop() {
  int sensorValue1 = analogRead(SENSOR_PIN1);
  int sensorValue2 = analogRead(SENSOR_PIN2);
  float voltage1 = sensorValue1 * (5.0 / 1023.0);
  float voltage2 = sensorValue2 * (5.0 / 1023.0);

  Serial.print("Voltage 1: ");
  Serial.print(voltage1);
  Serial.print(" V, Voltage 2: ");
  Serial.println(voltage2);

  unsigned long currentMillis = millis();

  if (voltage1 < 0.1 && voltage2 < 0.1) {
    if (currentMillis - lastUpdate >= 1000 && litLeds > 0) {
      lastUpdate = currentMillis;

      for (int i = 0; i < litLeds; i++) {
        strip.setPixelColor(i, strip.Color(50, 50, 50));
      }

      strip.show();

      delay(100);

      strip.setPixelColor(litLeds - 1, 0);
      litLeds--;
    }

    if (motorActive) {
      analogWrite(MOTOR_PIN, 0);
      motorActive = false;
    }
  } else {
    uint32_t color;
    if (abs(voltage1 - voltage2) <= VOLTAGE_THRESHOLD) {
      color = strip.Color(0, 100, 0);
    } else {
      color = strip.Color(40, 0, 0);
    }

    for (int i = 0; i < litLeds; i++) {
      strip.setPixelColor(i, color);
    }

    if (currentMillis - lastUpdate >= 1000 && litLeds < NUM_LEDS) {
      lastUpdate = currentMillis;

      strip.setPixelColor(litLeds, color);
      litLeds++;
    }

    if (litLeds == NUM_LEDS && !motorActive) {
      motorActive = true;
    }

    if (motorActive) {
      if (!vibrating && (currentMillis - lastVibrationCycle >= pauseDuration)) {
        analogWrite(MOTOR_PIN, motorIntensity);
        vibrating = true;
        motorVibrationStart = currentMillis;
      }

      if (vibrating && (currentMillis - motorVibrationStart >= vibrationDuration)) {
        analogWrite(MOTOR_PIN, 0);
        vibrating = false;
        motorPauseStart = currentMillis;
      }

      if (!vibrating && (currentMillis - motorPauseStart >= pauseDuration)) {
        lastVibrationCycle = currentMillis;
      }
    }
  }

  strip.show();
  delay(100);
}