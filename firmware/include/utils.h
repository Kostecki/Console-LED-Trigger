#pragma once

#include <Arduino.h>

String getMacSuffix();
time_t getSyncedUnixTime(uint32_t timeoutMs = 5000);
const char *colorModeToString(ColorMode mode);