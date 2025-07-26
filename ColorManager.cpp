#include "HardwareConfig.h"
#include "ColorManager.h"

const uint8_t COLOR_OPTIONS[][3] = {
    {255, 0, 0},     // Red
    {0, 255, 0},     // Green
    {0, 0, 255},     // Blue
    {255, 255, 0},   // Yellow
    {0, 255, 255},   // Cyan
    {255, 0, 255},   // Magenta
    {255, 128, 0},   // Orange
    {128, 0, 255},   // Purple
    {255, 255, 255}, // White
};

extern Adafruit_NeoPixel strip;

void setAllPixels(uint8_t r, uint8_t g, uint8_t b)
{
  uint32_t color = strip.Color(r, g, b);
  for (int i = 0; i < NUM_PIXELS; i++)
  {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void blinkColor(uint8_t r, uint8_t g, uint8_t b, int times, int delayMs)
{
  for (int i = 0; i < times; i++)
  {
    setAllPixels(r, g, b);
    delay(delayMs);
    clearStrip();
    delay(delayMs);
  }
}

void clearStrip()
{
  strip.clear();
  strip.show();
}
