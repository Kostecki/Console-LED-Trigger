#include <Arduino.h>

#include "utils.h"

String getMacSuffix()
{
  uint64_t chipId = ESP.getEfuseMac();
  char suffix[7];
  sprintf(suffix, "%06X", (uint32_t)(chipId & 0xFFFFFF));
  return String(suffix);
}