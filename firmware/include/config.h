#pragma once

// LED Config
constexpr uint8_t NUM_PIXELS = 15;

// Current Sense Config
const int CURRENT_THRESHOLD = 1600;
const int CURRENT_THRESHOLD_OFFSET = 100;

// Encoder Config
constexpr int ENCODER_STEPS_PER_CLICK = 4;

// Timing Config
constexpr unsigned long LONG_PRESS_THRESHOLD = 2000; // 2 seconds
constexpr unsigned long POWER_OFF_DELAY = 1000;      // 1 second

// HA Device Config
constexpr const char *HA_DEVICE_MANUFACTURER = "Kostecki";
constexpr const char *HA_DEVICE_MODEL = "Console LED Trigger";
constexpr const char *HA_DEVICE_FW_VERSION = "1.0.0";