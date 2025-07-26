#pragma once
#include <EEPROM.h>

constexpr int EEPROM_SIZE = 64;
constexpr int ADDR_THRESHOLD_ON = 0;
constexpr int ADDR_COLOR_R = ADDR_THRESHOLD_ON + sizeof(float);
constexpr int ADDR_COLOR_G = ADDR_COLOR_R + 1;
constexpr int ADDR_COLOR_B = ADDR_COLOR_G + 1;
constexpr float THRESHOLD_MARGIN = 0.010f;

void EEPROMWriteFloat(int address, float value);
float EEPROMReadFloat(int address);
void saveCurrentColor(uint8_t r, uint8_t g, uint8_t b);
void loadSavedColor();
void saveThreshold(float thresholdOn);
void loadThreshold();

// Shared variables to hold loaded values
extern float currentThresholdOn;
extern uint8_t savedR, savedG, savedB;