#include <Arduino.h>

#include <utils.h>
#include <state.h>
#include <serial_mux.h>

String getMacSuffix()
{
  uint64_t chipId = ESP.getEfuseMac();
  char suffix[7];
  sprintf(suffix, "%06X", (uint32_t)(chipId & 0xFFFFFF));
  return String(suffix);
}

time_t getSyncedUnixTime(uint32_t timeoutMs)
{
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, timeoutMs))
  {
    Serial.println();
    Serial.println("NTP time synced successfully");
    return time(nullptr);
  }
  else
  {
    return 0; // Failed to sync NTP
  }
}

// Read ADC average over multiple samples
int readAdcAverage(uint8_t pin, int samples)
{
  long sum = 0;
  for (int i = 0; i < samples; ++i)
  {
    sum += analogRead(pin);
    delayMicroseconds(1000);
  }

  return (int)(sum / samples);
}

// Convert color mode enum to string
const char *colorModeToString(ColorMode mode)
{
  switch (mode)
  {
  case ColorMode::Palette:
    return "Palette";
  case ColorMode::Custom:
    return "Custom";
  default:
    return "Unknown";
  }
}

// Linearly interpolate between two color components
uint8_t lerpColorComponent(uint8_t from, uint8_t to, uint8_t step, uint8_t maxStep)
{
  return from + ((to - from) * step) / maxStep;
}

// Convert 24-bit RGB color to separate R, G, B components
void rgbFrom24(uint32_t color, uint8_t &r, uint8_t &g, uint8_t &b)
{
  r = (uint8_t)((color >> 16) & 0xFF);
  g = (uint8_t)((color >> 8) & 0xFF);
  b = (uint8_t)(color & 0xFF);
}