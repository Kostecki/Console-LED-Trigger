#pragma once

// EEPROM Config
#define EEPROM_ADDR_COLOR 0
#define EEPROM_ADDR_BRIGHTNESS 1

// LED Config
constexpr uint8_t NUM_PIXELS = 15;

// Current Sense Config
const int CURRENT_THRESHOLD = 150;
const int CURRENT_SENSE_OFFSET = 15;
const int CURRENT_THRESHOLD_ON = CURRENT_THRESHOLD + CURRENT_SENSE_OFFSET;
const int CURRENT_THRESHOLD_OFF = CURRENT_THRESHOLD - CURRENT_SENSE_OFFSET;

// Encoder Config
constexpr int ENCODER_STEPS_PER_CLICK = 4;

// Timing Config
constexpr unsigned long LONG_PRESS_THRESHOLD = 2000; // 2 seconds
constexpr unsigned long POWER_OFF_DELAY = 1000;      // 1 second