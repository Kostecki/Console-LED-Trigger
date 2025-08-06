#include <Preferences.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <Update.h>

#include <state.h>
#include <utils.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

unsigned long lastReconnectAttempt = 0;

String mqtt_server;
uint16_t mqtt_port;
String mqtt_user;
String mqtt_pass;
bool mqttConfigValid = false;

void connectToMqtt();
void mqttCallback(char *topic, byte *payload, unsigned int length);

void initWiFiAndMQTTAndOTA(Preferences &prefs)
{
  WiFiManager wm;

  String apName = "Console-LED-" + getMacSuffix();

  // Load saved MQTT config
  mqtt_server = prefs.getString("mqtt_server", "");
  mqtt_port = prefs.getString("mqtt_port", "1883").toInt();
  mqtt_user = prefs.getString("mqtt_user", "");
  mqtt_pass = prefs.getString("mqtt_pass", "");

  // Add to config portal
  WiFiManagerParameter p_mqtt_server("server", "MQTT Server", mqtt_server.c_str(), 40);
  WiFiManagerParameter p_mqtt_port("port", "MQTT Port", String(mqtt_port).c_str(), 6);
  WiFiManagerParameter p_mqtt_user("user", "MQTT User", mqtt_user.c_str(), 32);
  WiFiManagerParameter p_mqtt_pass("pass", "MQTT Password", mqtt_pass.c_str(), 64);

  wm.addParameter(&p_mqtt_server);
  wm.addParameter(&p_mqtt_port);
  wm.addParameter(&p_mqtt_user);
  wm.addParameter(&p_mqtt_pass);

  // Connect WiFi
  if (!wm.autoConnect(apName.c_str()))
  {
    Serial.println("WiFi failed. Rebooting...");
    delay(3000);
    ESP.restart();
  }

  Serial.print("WiFi connected! IP: ");
  Serial.println(WiFi.localIP());
  delay(3000);

  // Save MQTT config
  mqtt_server = p_mqtt_server.getValue();
  mqtt_port = String(p_mqtt_port.getValue()).toInt();
  mqtt_user = p_mqtt_user.getValue();
  mqtt_pass = p_mqtt_pass.getValue();
  mqttConfigValid = mqtt_server.length() > 0 && mqtt_port > 0;

  if (!mqttConfigValid)
  {
    Serial.println("MQTT config incomplete—skipping MQTT setup!");
    return;
  }

  prefs.putString("mqtt_server", mqtt_server);
  prefs.putString("mqtt_port", String(mqtt_port));
  prefs.putString("mqtt_user", mqtt_user);
  prefs.putString("mqtt_pass", mqtt_pass);

  // NTP time sync
  Serial.println();
  Serial.println("Syncing NTP time...");
  configTime(0, 0, "dk.pool.ntp.org", "time.google.com");

  // MQTT setup
  if (mqttConfigValid)
  {
    mqttClient.setServer(mqtt_server.c_str(), mqtt_port);
    mqttClient.setCallback(mqttCallback);
    connectToMqtt();
  }

  // OTA setup
  ArduinoOTA
      .onStart([]()
               { Serial.println("OTA update starting..."); })
      .onEnd([]()
             { Serial.println("\nOTA complete"); })
      .onProgress([](unsigned int p, unsigned int t)
                  { Serial.printf("OTA %u%%\r", (p * 100) / t); })
      .onError([](ota_error_t e)
               {
        Serial.printf("OTA Error [%u]: ", e);
        if (e == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (e == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (e == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (e == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (e == OTA_END_ERROR) Serial.println("End Failed"); });

  ArduinoOTA.setHostname(apName.c_str());
  ArduinoOTA.begin();

  Serial.println();
  Serial.printf("OTA Ready. Hostname: %s.local\n", apName.c_str());
}

void publishState()
{
  Serial.println("Publishing state...");

  JsonDocument doc;
  doc["enabled"] = ledEnabled;
  doc["brightness"] = currentBrightness;
  doc["bootTime"] = bootTime;
  doc["name"] = deviceName;

  doc["colorMode"] = (colorMode == ColorMode::Palette) ? "palette" : "custom";
  doc["colorIndex"] = currentColorIndex;
  char hexColor[8];
  snprintf(hexColor, sizeof(hexColor), "%06X", customColor);
  doc["customColor"] = String("#") + hexColor;

  size_t needed = measureJson(doc) + 1;
  char payload[needed];
  serializeJson(doc, payload, needed);

  String topic = "console/board-" + getMacSuffix() + "/state";
  mqttClient.publish(topic.c_str(), payload, true);
}

void connectToMqtt()
{
  Serial.println();
  Serial.println("Connecting to MQTT...");

  String clientId = "board-" + getMacSuffix();

  // Last Will and Testament (LWT) Config
  String willTopic = "console/" + clientId + "/status";
  int willQos = 0;
  bool willRetain = true;
  const char *willPayload = "0"; // Offline

  if (mqttClient.connect(clientId.c_str(), mqtt_user.c_str(), mqtt_pass.c_str(), willTopic.c_str(), willQos, willRetain, willPayload))
  {
    Serial.println("MQTT connected");

    String prefix = "console/" + clientId;

    mqttClient.subscribe((prefix + "/set").c_str());
    mqttClient.subscribe((prefix + "/identify").c_str());
    mqttClient.subscribe((prefix + "/reboot").c_str());
    mqttClient.subscribe((prefix + "/update").c_str());

    mqttClient.publish((prefix + "/status").c_str(), (uint8_t *)"1", 1, true);
    publishState();
  }
  else
  {
    Serial.print("MQTT failed, rc=");
    Serial.print(mqttClient.state());
    Serial.println(" try again in 5 seconds");
  }
}

void handleMqttLoop()
{
  if (!mqttConfigValid)
  {
    return; // Skip MQTT handling if config is invalid
  }

  if (!mqttClient.connected())
  {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000)
    {
      lastReconnectAttempt = now;
      connectToMqtt();
    }
  }
  else
  {
    mqttClient.loop();
  }
}

void performOTAUpdate(const String &url)
{
  WiFiClient client;
  HTTPClient http;

  Serial.printf("Starting OTA update from: %s\n", url.c_str());
  http.begin(client, url);

  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK)
  {
    Serial.printf("HTTP GET failed: %d\n", httpCode);
    http.end();
    return;
  }

  int contentLength = http.getSize();
  if (contentLength <= 0)
  {
    Serial.println("Invalid content length");
    http.end();
    return;
  }

  bool canBegin = Update.begin(contentLength);
  if (!canBegin)
  {
    Serial.println("Not enough space for OTA");
    http.end();
    return;
  }

  WiFiClient *stream = http.getStreamPtr();
  size_t written = Update.writeStream(*stream);

  if (written != contentLength)
  {
    Serial.printf("Only wrote %d of %d bytes\n", written, contentLength);
    http.end();
    return;
  }

  if (!Update.end())
  {
    Serial.printf("Update failed. Error: %s\n", Update.errorString());
    http.end();
    return;
  }

  if (!Update.isFinished())
  {
    Serial.println("Update not complete");
    http.end();
    return;
  }

  Serial.println("OTA update complete! Rebooting...");
  http.end();
  delay(1000);
  ESP.restart();
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  payload[length] = '\0'; // Null-terminate
  String topicStr = String(topic);
  Serial.printf("MQTT message on topic %s: %s\n", topic, (char *)payload);

  JsonDocument doc;
  deserializeJson(doc, (char *)payload);

  if (topicStr.endsWith("/set"))
  {
    Serial.println("Received set command");
    bool stateChanged = false;

    if (doc["color"].is<int>())
    {
      Serial.println("Setting color");

      int colorIndex = doc["color"];
      if (colorIndex >= 0 && colorIndex < NUM_COLORS)
      {
        stateChanged = true;
        colorMode = ColorMode::Palette;
        currentColorIndex = colorIndex;
        Serial.printf("Setting color index to %d\n", doc["color"].as<int>());
        prefs.putUChar("color_mode", static_cast<uint8_t>(colorMode));
        prefs.putUChar("color_index", currentColorIndex);
        updateLED(true);
      }
      else if (colorIndex == -1 && doc["customColor"].is<const char *>())
      {
        stateChanged = true;
        colorMode = ColorMode::Custom;
        String hex = doc["customColor"].as<const char *>();
        if (hex.startsWith("#"))
        {
          hex = hex.substring(1);
        }
        customColor = (uint32_t)strtoul(hex.c_str(), nullptr, 16);
        Serial.printf("Setting custom color to %s\n", hex.c_str());
        prefs.putUChar("color_mode", static_cast<uint8_t>(colorMode));
        prefs.putString("custom_color", hex);
        updateLED(true);
      }
    }

    if (doc["brightness"].is<int>())
    {
      stateChanged = true;
      int brightness = doc["brightness"];
      brightness = constrain(brightness, 0, 255);
      currentBrightness = brightness;
      Serial.printf("Setting brightness to %d\n", currentBrightness);
      prefs.putUChar("brightness", currentBrightness);
      strip.setBrightness(currentBrightness);
      updateLED(false);
    }

    if (doc["name"].is<const char *>())
    {
      stateChanged = true;
      deviceName = doc["name"].as<String>();
      Serial.printf("Setting device name to '%s'\n", deviceName.c_str());
      prefs.putString("name", deviceName);
    }

    if (stateChanged)
    {
      publishState();
    }
  }
  else if (topicStr.endsWith("/identify"))
  {
    Serial.println("Received identify command");

    // Compute current color just once:
    uint32_t originalColor = (colorMode == ColorMode::Palette && currentColorIndex < NUM_COLORS)
                                 ? colors[currentColorIndex]
                                 : customColor;

    for (int i = 0; i < 3; ++i)
    {
      Serial.printf("Identifying... iteration %d\n", i + 1);
      fadeToColor(strip.Color(255, 255, 255), 10, 15);
      delay(100);
      fadeToColor(originalColor, 10, 15);
    }
  }
  else if (topicStr.endsWith("/reboot"))
  {
    Serial.println("Received reboot command");
    delay(500);
    ESP.restart();
  }
  else if (topicStr.endsWith("/fw-update"))
  {
    Serial.println("Received firmware update command");

    if (doc["url"].is<const char *>())
    {
      Serial.printf("Firmware update URL: %s\n", doc["url"].as<String>().c_str());
      performOTAUpdate(doc["url"].as<String>());
    }
  }
}