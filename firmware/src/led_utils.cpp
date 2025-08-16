#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "state.h"
#include "config.h"
#include "colors.h"
#include "utils.h"

void updateLED(bool force)
{
  uint32_t color = 0;
  if (ledEnabled || force)
  {
    if (colorMode == ColorMode::Palette && currentColorIndex < NUM_COLORS)
      color = colors[currentColorIndex];
    else
      color = customColor;
  }
  for (int i = 0; i < NUM_PIXELS; ++i)
    strip.setPixelColor(i, color);
  strip.show();
}

void fadeToColor(uint32_t targetColor, uint8_t steps, uint16_t delayMs)
{
  uint32_t startColor = strip.getPixelColor(0);
  for (int i = 0; i <= steps; ++i)
  {
    uint32_t interp = strip.Color(
        lerpColorComponent((startColor >> 16) & 0xFF, (targetColor >> 16) & 0xFF, i, steps),
        lerpColorComponent((startColor >> 8) & 0xFF, (targetColor >> 8) & 0xFF, i, steps),
        lerpColorComponent(startColor & 0xFF, targetColor & 0xFF, i, steps));

    for (int j = 0; j < NUM_PIXELS; ++j)
    {
      strip.setPixelColor(j, interp);
    }

    strip.show();
    delay(delayMs);
  }
}

void blinkConfirm(uint32_t color, int times)
{
  // Animation constants
  const uint8_t steps = 10;      // Fade steps
  const uint16_t stepDelay = 10; // Delay between steps (ms)
  const uint16_t holdTime = 100; // Time to hold the color/black (ms)

  uint32_t black = strip.Color(0, 0, 0);

  for (int i = 0; i < times; ++i)
  {
    fadeToColor(color, steps, stepDelay);
    delay(holdTime);
    fadeToColor(black, steps, stepDelay);
    delay(holdTime);
  }

  // Restore the normal LED state
  updateLED(false);
}
