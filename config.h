#pragma once

// EEPROM Config
#define EEPROM_ADDR_COLOR 0
#define EEPROM_ADDR_BRIGHTNESS 1

// Current Sense Config
const int CURRENT_THRESHOLD = 100;
const int HYSTERESIS = 10;
const int CURRENT_THRESHOLD_ON = CURRENT_THRESHOLD + HYSTERESIS;
const int CURRENT_THRESHOLD_OFF = CURRENT_THRESHOLD - HYSTERESIS;

// Encoder Config
constexpr int ENCODER_STEPS_PER_CLICK = 4;

// LED Config
constexpr uint8_t NUM_PIXELS = 15;

// Timing Config
constexpr uint8_t longPressThreshold = 2000; // 2 seconds
constexpr uint8_t powerOffDelay = 5000;      // 5 seconds