#include <Adafruit_NeoPixel.h>

#define SENSOR_PIN1 A0           // First FSR sensor connected to A0
#define SENSOR_PIN2 A1           // Second FSR sensor connected to A1
#define LED_PIN 6                // NeoPixel data pin connected to D6
#define MOTOR_PIN 5              // Motor connected to D5
#define NUM_LEDS 8               // Number of LEDs in the NeoPixel strip
#define VOLTAGE_THRESHOLD 0.5    // Threshold to consider voltages "equal" (adjustable)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

int litLeds = 0;                // Tracks how many LEDs are currently lit
unsigned long lastUpdate = 0;   // For timing

void setup() {
  Serial.begin(9600);
  strip.begin();                 // Initialize the NeoPixel strip
  strip.setBrightness(50);       // Set brightness (0-255)
  strip.show();                  // Ensure all LEDs are off at the start
  pinMode(MOTOR_PIN, OUTPUT);    // Set motor pin as output
  digitalWrite(MOTOR_PIN, LOW);  // Ensure motor is off at the start
}

void loop() {
  int sensorValue1 = analogRead(SENSOR_PIN1);
  int sensorValue2 = analogRead(SENSOR_PIN2);
  float voltage1 = sensorValue1 * (5.0 / 1023.0);
  float voltage2 = sensorValue2 * (5.0 / 1023.0);

  // Print voltage values to Serial Monitor
  Serial.print("Voltage 1: ");
  Serial.print(voltage1);
  Serial.print(" V, Voltage 2: ");
  Serial.println(voltage2);

  unsigned long currentMillis = millis();

  if (voltage1 < 0.1 && voltage2 < 0.1) {
    // Both voltages are near zero, gradually turn off LEDs
    if (currentMillis - lastUpdate >= 1000 && litLeds > 0) {  // Update every 1000 ms
      lastUpdate = currentMillis;

      // Change all lit LEDs to yellow before turning them off
      for (int i = 0; i < litLeds; i++) {
        strip.setPixelColor(i, strip.Color(50, 50, 30));  // Set to yellow
      }

      strip.show();

      // Delay a little before actually turning off the last LED
      delay(100);  // Small delay to show yellow

      // Turn off the last lit LED
      strip.setPixelColor(litLeds - 1, 0);  // Turn off the last lit LED
      litLeds--;                            // Decrease the count of lit LEDs
    }

    // Turn off the motor
    digitalWrite(MOTOR_PIN, LOW);
  } else {
    // Decide color based on voltage difference
    uint32_t color;
    if (abs(voltage1 - voltage2) <= VOLTAGE_THRESHOLD) {
      color = strip.Color(0, 100, 0);  // Green for similar voltages
      digitalWrite(MOTOR_PIN, LOW);   // Turn off the motor
    } else {
      color = strip.Color(40, 0, 0);  // Red for different voltages
      digitalWrite(MOTOR_PIN, HIGH);  // Turn on the motor
    }

    // Update color of all currently lit LEDs
    for (int i = 0; i < litLeds; i++) {
      strip.setPixelColor(i, color);  // Set the color for all lit LEDs
    }

    // Gradually light up new LEDs if there is voltage
    if (currentMillis - lastUpdate >= 1000 && litLeds < NUM_LEDS) {  // Update every 1000 ms
      lastUpdate = currentMillis;

      strip.setPixelColor(litLeds, color);  // Light up the next LED with the current color
      litLeds++;                            // Increase the count of lit LEDs
    }
  }

  strip.show();
  delay(500);  // Small delay to make the display smoother
}