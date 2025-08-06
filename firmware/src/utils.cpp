#include <Arduino.h>

String getMacSuffix()
{
  uint64_t chipId = ESP.getEfuseMac();
  char suffix[7];
  sprintf(suffix, "%06X", (uint32_t)(chipId & 0xFFFFFF));
  return String(suffix);
}

time_t getSyncedUnixTime(uint32_t timeoutMs = 5000)
{
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, timeoutMs))
  {
    Serial.println("NTP time synced successfully");
    return time(nullptr);
  }
  else
  {
    return 0; // Failed to sync NTP
  }
}