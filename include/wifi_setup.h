#include <WiFi.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>

void setupWiFiAndOTA(Preferences &prefs)
{
  WiFiManager wm;

  // Custom MQTT parameters
  WiFiManagerParameter p_mqtt_server("mqtt_server", "MQTT Server", prefs.getString("mqtt_server", "").c_str(), 40);
  WiFiManagerParameter p_mqtt_port("mqtt_port", "MQTT Port", prefs.getString("mqtt_port", "1883").c_str(), 6);
  WiFiManagerParameter p_mqtt_user("mqtt_user", "MQTT Username", prefs.getString("mqtt_user", "").c_str(), 32);
  WiFiManagerParameter p_mqtt_pass("mqtt_pass", "MQTT Password", prefs.getString("mqtt_pass", "").c_str(), 32);

  wm.addParameter(&p_mqtt_server);
  wm.addParameter(&p_mqtt_port);
  wm.addParameter(&p_mqtt_user);
  wm.addParameter(&p_mqtt_pass);

  if (!wm.autoConnect("Console-LED-AP"))
  {
    Serial1.println("⚠️ WiFi failed. Rebooting...");
    delay(3000);
    ESP.restart();
  }

  Serial1.print("✅ WiFi connected! IP: ");
  Serial1.println(WiFi.localIP());

  // Save MQTT parameters
  prefs.putString("mqtt_server", p_mqtt_server.getValue());
  prefs.putString("mqtt_port", p_mqtt_port.getValue());
  prefs.putString("mqtt_user", p_mqtt_user.getValue());
  prefs.putString("mqtt_pass", p_mqtt_pass.getValue());

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

  ArduinoOTA.begin();
  Serial1.println("📡 OTA ready");
}
