#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "HardwareConfig.h"
#include "CurrentSensor.h"
#include "ColorManager.h"

constexpr float ADC_REF_VOLTAGE = 3.3f;
constexpr float ADC_MAX_VALUE = 4095.0f;
constexpr float R_TOP = 10.0f;
constexpr float R_BOTTOM = 4.7f;
constexpr float VOLTAGE_DIVIDER_RATIO = R_TOP / (R_BOTTOM + R_TOP);
constexpr float ACS712_ZERO_CURRENT_VOLTAGE = 2.5f;
constexpr float ACS712_SENSITIVITY = 0.185f;

extern Adafruit_NeoPixel strip;

float readCurrent()
{
  int sensorValue = analogRead(SENSOR_PIN); // SENSOR_PIN - ideally passed as param or global
  Serial1.print("Raw ADC: ");
  Serial1.println(sensorValue);
  float voltageAtPin = (sensorValue / ADC_MAX_VALUE) * ADC_REF_VOLTAGE;
  float sensorVoltage = voltageAtPin / VOLTAGE_DIVIDER_RATIO;
  return (sensorVoltage - ACS712_ZERO_CURRENT_VOLTAGE) / ACS712_SENSITIVITY;
}

float readAverageCurrent(int samples, int delayMs)
{
  float sum = 0;
  for (int i = 0; i < samples; i++)
  {
    sum += readCurrent();
    float progress = (float)i / (samples - 1);
    float brightness = 10 + 118 * sin(progress * PI);
    strip.setBrightness((uint8_t)brightness);
    setAllPixels(0, 0, 255);
    delay(delayMs);
  }
  strip.setBrightness(128);
  clearStrip();
  return sum / samples;
}

float smoothCurrent(float currentRaw, float smoothedCurrent)
{
  constexpr float SMOOTHING_ALPHA = 0.1;
  return (SMOOTHING_ALPHA * currentRaw) + ((1 - SMOOTHING_ALPHA) * smoothedCurrent);
}
