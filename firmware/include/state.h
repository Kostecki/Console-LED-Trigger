#pragma once

#include <Adafruit_NeoPixel.h>
#include <Preferences.h>

enum class ColorMode : uint8_t
{
  Palette = 0,
  Custom = 1
};

extern Adafruit_NeoPixel strip;
extern Preferences prefs;
extern uint8_t currentColorIndex;
extern uint32_t customColor;
extern ColorMode colorMode;
extern uint8_t currentBrightness;
extern bool ledEnabled;
extern time_t bootTime;
extern String deviceName;

void updateLED(bool force = false);
void fadeToColor(uint32_t targetColor, uint8_t steps = 50, uint16_t delayMs = 25);

extern const uint32_t colors[];
extern const uint8_t NUM_COLORS;