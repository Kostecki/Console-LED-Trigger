#include <Arduino.h>
#include "EEPROMUtils.h"

float currentThresholdOn = 0.045;

uint8_t savedR = 255, savedG = 0, savedB = 0;

void EEPROMWriteFloat(int address, float value)
{
  byte *p = (byte *)(void *)&value;
  for (unsigned int i = 0; i < sizeof(float); i++)
    EEPROM.write(address + i, *p++);
}

float EEPROMReadFloat(int address)
{
  float value = 0.0;
  byte *p = (byte *)(void *)&value;
  for (unsigned int i = 0; i < sizeof(float); i++)
    *p++ = EEPROM.read(address + i);
  return value;
}

void saveCurrentColor(uint8_t r, uint8_t g, uint8_t b)
{
  EEPROM.write(ADDR_COLOR_R, r);
  EEPROM.write(ADDR_COLOR_G, g);
  EEPROM.write(ADDR_COLOR_B, b);
  EEPROM.commit();
  savedR = r;
  savedG = g;
  savedB = b;
}

void loadSavedColor()
{
  savedR = EEPROM.read(ADDR_COLOR_R);
  savedG = EEPROM.read(ADDR_COLOR_G);
  savedB = EEPROM.read(ADDR_COLOR_B);
}

void saveThreshold(float thresholdOn)
{
  EEPROMWriteFloat(ADDR_THRESHOLD_ON, thresholdOn);
  EEPROM.commit();
  currentThresholdOn = thresholdOn;
}

void loadThreshold()
{
  float savedOn = EEPROMReadFloat(ADDR_THRESHOLD_ON);
  if (savedOn > 0.0f)
  {
    currentThresholdOn = savedOn;
  }
}
