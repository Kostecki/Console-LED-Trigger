#pragma once

#include <Arduino.h>

String getMacSuffix();
time_t getSyncedUnixTime(uint32_t timeoutMs = 5000);

// ADC-related
int readAdcAverage(uint8_t pin, int samples = 64);

// Color-related
enum class ColorMode : uint8_t;
const char *colorModeToString(ColorMode mode);
uint8_t lerpColorComponent(uint8_t from, uint8_t to, uint8_t step, uint8_t maxStep);
void rgbFrom24(uint32_t color, uint8_t &r, uint8_t &g, uint8_t &b);

String toLower(String s);