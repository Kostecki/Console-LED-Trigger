#pragma once
#include <Adafruit_NeoPixel.h>

extern const uint8_t COLOR_OPTIONS[][3];
constexpr uint8_t NUM_COLORS = 9;

void setAllPixels(uint8_t r, uint8_t g, uint8_t b);
void blinkColor(uint8_t r, uint8_t g, uint8_t b, int times, int delayMs);
void clearStrip();
