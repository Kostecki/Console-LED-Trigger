#include <WiFi.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>

String getMacSuffix()
{
  uint64_t chipId = ESP.getEfuseMac();
  char suffix[7];
  sprintf(suffix, "%06X", (uint32_t)(chipId & 0xFFFFFF));
  return String(suffix);
}

void setupWiFiAndOTA(Preferences &prefs)
{
  WiFiManager wm;

  String apName = "Console-LEDs-" + getMacSuffix();

  if (!wm.autoConnect(apName.c_str()))
  {
    Serial1.println("⚠️ WiFi failed. Rebooting...");
    delay(3000);
    ESP.restart();
  }

  Serial1.print("✅ WiFi connected! IP: ");
  Serial1.println(WiFi.localIP());

  // OTA setup
  ArduinoOTA
      .onStart([]()
               { Serial1.println("🔄 OTA update starting..."); })
      .onEnd([]()
             { Serial1.println("\n✅ OTA complete"); })
      .onProgress([](unsigned int p, unsigned int t)
                  { Serial1.printf("📶 OTA %u%%\r", (p * 100) / t); })
      .onError([](ota_error_t e)
               {
        Serial1.printf("❌ OTA Error [%u]: ", e);
        if (e == OTA_AUTH_ERROR) Serial1.println("Auth Failed");
        else if (e == OTA_BEGIN_ERROR) Serial1.println("Begin Failed");
        else if (e == OTA_CONNECT_ERROR) Serial1.println("Connect Failed");
        else if (e == OTA_RECEIVE_ERROR) Serial1.println("Receive Failed");
        else if (e == OTA_END_ERROR) Serial1.println("End Failed"); });

  ArduinoOTA.setHostname(apName.c_str());
  ArduinoOTA.begin();

  Serial1.println("📡 OTA ready");
}
