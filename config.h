#pragma once

// EEPROM Config
#define EEPROM_ADDR_COLOR 0
#define EEPROM_ADDR_BRIGHTNESS 1

// LED Config
constexpr uint8_t NUM_PIXELS = 15;

// Current Sense Config
const int CURRENT_THRESHOLD = 100;
const int CURRENT_SENSE_OFFSET = 10;
const int CURRENT_THRESHOLD_ON = CURRENT_THRESHOLD + CURRENT_SENSE_OFFSET;
const int CURRENT_THRESHOLD_OFF = CURRENT_THRESHOLD - CURRENT_SENSE_OFFSET;

// Encoder Config
constexpr int ENCODER_STEPS_PER_CLICK = 4;

// Timing Config
constexpr uint8_t LONG_PRESS_THRESHOLD = 2000; // 2 seconds
constexpr uint8_t POWER_OFF_DELAY = 5000;      // 5 seconds