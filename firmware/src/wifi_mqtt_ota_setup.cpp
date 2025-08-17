#include <Preferences.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Arduino.h>

#include <state.h>
#include <utils.h> // rgbFrom24 declared here
#include <serial_mux.h>
#include <pins.h>
#include <config.h>
#include <wifi_mqtt_ota_setup.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

unsigned long lastReconnectAttempt = 0;

String mqtt_server;
uint16_t mqtt_port;
String mqtt_user;
String mqtt_pass;
bool mqttConfigValid = false;

static WiFiManager wm;
static bool portalActive = false;
static bool wifiConnected = false;
static unsigned long portalDeadlineMs = 0;

static const uint16_t WIFI_CONNECT_TIMEOUT_S = 8;
static const uint16_t WIFI_PORTAL_TIMEOUT_S = 300;

static WiFiManagerParameter *g_pMqttServer = nullptr;
static WiFiManagerParameter *g_pMqttPort = nullptr;
static WiFiManagerParameter *g_pMqttUser = nullptr;
static WiFiManagerParameter *g_pMqttPass = nullptr;

static inline String haNodeId() { return "board-" + getMacSuffix(); }
static inline String haObjectId() { return haNodeId() + "-light"; }
static inline String haCfgTopic() { return "homeassistant/light/" + haNodeId() + "/config"; }
static inline String haCmdTopic() { return "console/" + haNodeId() + "/ha/set"; }
static inline String haStateTopic() { return "console/" + haNodeId() + "/ha/state"; }
static inline String haAvailTopic() { return "console/" + haNodeId() + "/status"; }

static void connectToMqtt();
static void saveMqttPrefs(Preferences &prefs);
static void loadMqttPrefs(Preferences &prefs);

bool wifiIsConnected() { return wifiConnected; }

static void publishHADiscovery();

void mqttCallback(char *topic, byte *payload, unsigned int length);

void wifiKickoff(const String &apName, Preferences &prefs)
{
  loadMqttPrefs(prefs);

  static bool eventsHooked = false;
  if (!eventsHooked)
  {
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t)
                 {
      switch (event) {
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
          wifiConnected = true;
          break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
          wifiConnected = false;
          Serial.println("WiFi disconnected");
          break;
        default:
          break;
      } });
    eventsHooked = true;
  }

  if (!g_pMqttServer)
  {
    g_pMqttServer = new WiFiManagerParameter("server", "MQTT Server", mqtt_server.c_str(), 64);
    g_pMqttPort = new WiFiManagerParameter("port", "MQTT Port", String(mqtt_port).c_str(), 6);
    g_pMqttUser = new WiFiManagerParameter("user", "MQTT User", mqtt_user.c_str(), 64);
    g_pMqttPass = new WiFiManagerParameter("pass", "MQTT Password", mqtt_pass.c_str(), 64);

    wm.addParameter(g_pMqttServer);
    wm.addParameter(g_pMqttPort);
    wm.addParameter(g_pMqttUser);
    wm.addParameter(g_pMqttPass);
  }
  else
  {
    g_pMqttServer->setValue(mqtt_server.c_str(), 64);
    g_pMqttPort->setValue(String(mqtt_port).c_str(), 6);
    g_pMqttUser->setValue(mqtt_user.c_str(), 64);
    g_pMqttPass->setValue(mqtt_pass.c_str(), 64);
  }

  wm.setSaveParamsCallback([&prefs]()
                           {
    mqtt_server     = g_pMqttServer->getValue();
    mqtt_port       = String(g_pMqttPort->getValue()).toInt();
    mqtt_user       = g_pMqttUser->getValue();
    mqtt_pass       = g_pMqttPass->getValue();
    mqttConfigValid = mqtt_server.length() > 0 && mqtt_port > 0;

    saveMqttPrefs(prefs);

    Serial.println("Saved MQTT params from config portal:");
    Serial.printf("  server='%s' port=%u user='%s' pass=%s\n",
                  mqtt_server.c_str(), mqtt_port, mqtt_user.c_str(),
                  mqtt_pass.length() ? "***" : "(empty)"); });

  wm.setConnectTimeout(WIFI_CONNECT_TIMEOUT_S);
  wm.setConfigPortalBlocking(false);
  wm.setConfigPortalTimeout(WIFI_PORTAL_TIMEOUT_S);
  wm.setBreakAfterConfig(true);
  wm.setDebugOutput(false);

  if (wm.autoConnect(apName.c_str()))
  {
    wifiConnected = true;
    Serial.println();
    Serial.print("WiFi connected: ");
    Serial.println(WiFi.localIP());
    return;
  }

  wm.startConfigPortal(apName.c_str());
  portalActive = true;
  portalDeadlineMs = millis() + (unsigned long)WIFI_PORTAL_TIMEOUT_S * 1000UL;

  Serial.println();
  Serial.println("WiFi portal started. Running offline until configured...");
}

void wifiProcess(Preferences &prefs)
{
  if (!portalActive && wifiConnected)
    return;

  if (portalActive)
  {
    wm.process();

    if (WiFi.status() == WL_CONNECTED)
    {
      portalActive = false;
      wm.stopConfigPortal();
      wifiConnected = true;

      Serial.print("WiFi connected: ");
      Serial.println(WiFi.localIP());

      mqtt_server = g_pMqttServer->getValue();
      mqtt_port = String(g_pMqttPort->getValue()).toInt();
      mqtt_user = g_pMqttUser->getValue();
      mqtt_pass = g_pMqttPass->getValue();
      mqttConfigValid = mqtt_server.length() > 0 && mqtt_port > 0;
      saveMqttPrefs(prefs);
      return;
    }

    if ((long)(millis() - portalDeadlineMs) >= 0)
    {
      portalActive = false;
      wm.stopConfigPortal();

      if (WiFi.status() != WL_CONNECTED)
      {
        WiFi.disconnect(true, true);
        WiFi.mode(WIFI_OFF);
        WiFi.setSleep(true);
        Serial.println("WiFi portal timed out. Staying offline");
      }
      else
      {
        Serial.println("Config portal timed out; staying connected");
      }
      return;
    }
  }
}

void maybeInitNetServices(Preferences &prefs)
{
  static bool didInit = false;
  static bool ntpStarted = false;
  static bool bootSet = false;

  if (!wifiConnected)
    return;

  if (!ntpStarted)
  {
    Serial.println("Syncing NTP time");
    configTime(0, 0, "pool.ntp.org", "time.google.com");
    ntpStarted = true;
  }

  if (!bootSet)
  {
    time_t now = time(nullptr);
    if (now > 100000)
    {
      bootTime = now;
      Serial.printf("Boot time set: %lu\n", bootTime);
      bootSet = true;
    }
  }

  if (!didInit)
  {
    if (mqttConfigValid)
    {
      mqttClient.setServer(mqtt_server.c_str(), mqtt_port);
      mqttClient.setCallback([](char *topic, byte *payload, unsigned int len)
                             { mqttCallback(topic, payload, len); });
      connectToMqtt();
    }
    else
    {
      Serial.println("MQTT config invalid. Skipping MQTT setup");
    }

    ArduinoOTA
        .onStart([]()
                 { Serial.println("OTA update starting"); })
        .onEnd([]()
               { Serial.println("\nOTA complete"); })
        .onProgress([](unsigned int p, unsigned int t)
                    { Serial.printf("OTA %u%%\r", (p * 100) / t); })
        .onError([](ota_error_t e)
                 { Serial.printf("OTA Error [%u]\n", e); });

    String host = "Console-LED-" + getMacSuffix();
    ArduinoOTA.setHostname(host.c_str());
    ArduinoOTA.begin();

    Serial.println();
    Serial.printf("OTA Ready. Hostname: %s.local\n", host.c_str());

    didInit = true;
  }
}

void publishState()
{
  if (!mqttClient.connected())
    return;

  Serial.println("Publishing state");

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

  String payload;
  serializeJson(doc, payload);

  String topic = "console/board-" + getMacSuffix() + "/state";
  mqttClient.publish(topic.c_str(), (const uint8_t *)payload.c_str(), payload.length(), true);
}

static void publishHADiscovery()
{
  if (!mqttClient.connected())
  {
    return;
  }

  {
    JsonDocument config;
    config["name"] = deviceName.length() ? deviceName : ("Console " + getMacSuffix());
    config["uniq_id"] = haNodeId();
    config["cmd_t"] = haCmdTopic();
    config["stat_t"] = haStateTopic();
    config["avty_t"] = haAvailTopic();
    config["pl_avail"] = "1";
    config["pl_not_avail"] = "0";
    config["schema"] = "json";
    config["color_mode"] = true;
    config["optimistic"] = false;

    JsonArray modes = config["supported_color_modes"].to<JsonArray>();
    modes.add("rgb");

    JsonObject device = config["device"].to<JsonObject>();
    device["ids"].add("console_" + haNodeId());
    device["name"] = config["name"];
    device["mf"] = HA_DEVICE_MANUFACTURER;
    device["mdl"] = HA_DEVICE_MODEL;
    device["sw"] = HA_DEVICE_FW_VERSION;

    String payload;
    serializeJson(config, payload);
    mqttClient.publish(haCfgTopic().c_str(), (const uint8_t *)payload.c_str(), payload.length(), true);
  }
}

void publishHAState()
{
  if (!mqttClient.connected())
    return;

  JsonDocument st;
  st["state"] = ledEnabled ? "ON" : "OFF";
  st["brightness"] = (int)currentBrightness;
  st["color_mode"] = "rgb";

  uint32_t c = (colorMode == ColorMode::Palette && currentColorIndex < NUM_COLORS) ? colors[currentColorIndex] : customColor;
  uint8_t r = 0, g = 0, b = 0;
  rgbFrom24(c, r, g, b);
  auto col = st["color"].to<JsonObject>();
  col["r"] = (int)r;
  col["g"] = (int)g;
  col["b"] = (int)b;

  String payload;
  serializeJson(st, payload);
  mqttClient.publish(haStateTopic().c_str(), (const uint8_t *)payload.c_str(), payload.length(), true);
}

void connectToMqtt()
{
  Serial.println();
  Serial.println("Connecting to MQTT");

  String clientId = haNodeId();
  String willTopic = haAvailTopic();
  int willQos = 0;
  bool willRetain = true;
  const char *willPayload = "0";

  if (mqttClient.connect(clientId.c_str(),
                         mqtt_user.c_str(),
                         mqtt_pass.c_str(),
                         willTopic.c_str(), willQos, willRetain, willPayload))
  {
    Serial.println("MQTT connected");

    String prefix = "console/" + clientId;
    mqttClient.subscribe((prefix + "/set").c_str());
    mqttClient.subscribe((prefix + "/identify").c_str());
    mqttClient.subscribe((prefix + "/reboot").c_str());
    mqttClient.subscribe((prefix + "/fw-update").c_str());
    mqttClient.subscribe((prefix + "/calibrate").c_str());

    mqttClient.subscribe(haCmdTopic().c_str());

    mqttClient.publish(haAvailTopic().c_str(), "1", willRetain);

    publishHADiscovery();
    publishHAState();
    publishState();

    lastReconnectAttempt = millis();
  }
  else
  {
    Serial.print("MQTT failed, rc=");
    Serial.print(mqttClient.state());
    Serial.println(" try again in 5 seconds");
    lastReconnectAttempt = millis();
  }
}

void handleMqttLoop()
{
  if (!mqttConfigValid)
    return;

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

  if (written != (size_t)contentLength)
  {
    Serial.printf("Only wrote %u of %d bytes\n", (unsigned)written, contentLength);
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

  Serial.println("OTA update complete! Rebooting");
  http.end();
  delay(1000);
  ESP.restart();
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  String topicStr(topic);
  String msg((const char *)payload, length);
  Serial.printf("MQTT message on topic %s: %s\n", topic, msg.c_str());

  JsonDocument doc;
  auto err = deserializeJson(doc, msg);
  if (err)
  {
    Serial.printf("JSON parse failed: %s\n", err.c_str());
    return;
  }

  if (topicStr == haCmdTopic())
  {
    bool stateChanged = false;

    if (doc["state"].is<const char *>())
    {
      Serial.println("HA sent state ON/OFF — ignoring (device power is console-driven).");
      // Immediately republish real state so HA UI snaps back
      publishHAState();
    }

    if (doc["brightness"].is<int>())
    {
      int b = constrain(doc["brightness"].as<int>(), 0, 255);
      if (b != currentBrightness)
      {
        currentBrightness = b;
        strip.setBrightness(currentBrightness);
        updateLED(false);
        prefs.putUChar("brightness", currentBrightness);
        stateChanged = true;
      }
    }

    if (doc["color"].is<JsonObject>())
    {
      auto cobj = doc["color"].as<JsonObject>();
      if (cobj["r"].is<int>() && cobj["g"].is<int>() && cobj["b"].is<int>())
      {
        uint8_t r = constrain(cobj["r"].as<int>(), 0, 255);
        uint8_t g = constrain(cobj["g"].as<int>(), 0, 255);
        uint8_t b = constrain(cobj["b"].as<int>(), 0, 255);

        customColor = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
        colorMode = ColorMode::Custom;
        prefs.putUChar("color_mode", static_cast<uint8_t>(colorMode));
        char hex[7];
        snprintf(hex, sizeof(hex), "%02X%02X%02X", r, g, b);
        prefs.putString("custom_color", hex);

        updateLED(true);
        stateChanged = true;
      }
    }

    if (stateChanged)
    {
      publishHAState();
      publishState();
    }
    return;
  }

  if (topicStr.endsWith("/set"))
  {
    Serial.println("Received set command");
    bool stateChanged = false;

    if (doc["color"].is<int>())
    {
      int colorIndex = doc["color"];
      if (colorIndex >= 0 && colorIndex < NUM_COLORS)
      {
        stateChanged = true;
        colorMode = ColorMode::Palette;
        currentColorIndex = colorIndex;
        prefs.putUChar("color_mode", (uint8_t)colorMode);
        prefs.putUChar("color_index", currentColorIndex);
        updateLED(true);
      }
      else if (colorIndex == -1 && doc["customColor"].is<const char *>())
      {
        stateChanged = true;
        colorMode = ColorMode::Custom;
        String hex = doc["customColor"].as<const char *>();
        if (hex.startsWith("#"))
          hex = hex.substring(1);
        customColor = (uint32_t)strtoul(hex.c_str(), nullptr, 16);
        prefs.putUChar("color_mode", (uint8_t)colorMode);
        prefs.putString("custom_color", hex);
        updateLED(true);
      }
    }

    if (doc["brightness"].is<int>())
    {
      stateChanged = true;
      int brightness = constrain((int)doc["brightness"], 0, 255);
      currentBrightness = brightness;
      prefs.putUChar("brightness", currentBrightness);
      strip.setBrightness(currentBrightness);
      updateLED(false);
    }

    if (doc["name"].is<const char *>())
    {
      stateChanged = true;
      deviceName = doc["name"].as<String>();
      prefs.putString("name", deviceName);

      publishHADiscovery();
    }

    if (stateChanged)
    {
      publishState();
      publishHAState();
    }
  }
  else if (topicStr.endsWith("/fw-update"))
  {
    if (doc["url"].is<const char *>())
    {
      performOTAUpdate(doc["url"].as<String>());
    }
  }
  else if (topicStr.endsWith("/identify"))
  {
    uint32_t originalColor = (colorMode == ColorMode::Palette && currentColorIndex < NUM_COLORS)
                                 ? colors[currentColorIndex]
                                 : customColor;
    for (int i = 0; i < 3; ++i)
    {
      fadeToColor(strip.Color(255, 255, 255), 10, 15);
      delay(100);
      fadeToColor(originalColor, 10, 15);
    }
  }
  else if (topicStr.endsWith("/reboot"))
  {
    delay(500);
    ESP.restart();
  }
  else if (topicStr.endsWith("/calibrate"))
  {
    int baseline = readAdcAverage(CURRENT_SENSE_PIN, 64);
    currentThreshold = baseline;
    prefs.putInt("current_threshold", currentThreshold);
    blinkConfirm(strip.Color(255, 255, 255), 2);
  }
  else
  {
    Serial.printf("Unknown topic: %s\n", topicStr.c_str());
  }
}

static void loadMqttPrefs(Preferences &prefs)
{
  if (prefs.isKey("mqtt_server"))
    mqtt_server = prefs.getString("mqtt_server");
  else
    mqtt_server = "";
  if (prefs.isKey("mqtt_port"))
    mqtt_port = (uint16_t)prefs.getUInt("mqtt_port", 1883);
  else
    mqtt_port = 1883;
  if (prefs.isKey("mqtt_user"))
    mqtt_user = prefs.getString("mqtt_user");
  else
    mqtt_user = "";
  if (prefs.isKey("mqtt_pass"))
    mqtt_pass = prefs.getString("mqtt_pass");
  else
    mqtt_pass = "";

  mqttConfigValid = mqtt_server.length() > 0 && mqtt_port > 0;
}

static void saveMqttPrefs(Preferences &prefs)
{
  if (mqtt_server.length())
    prefs.putString("mqtt_server", mqtt_server);
  prefs.putUInt("mqtt_port", mqtt_port);
  prefs.putString("mqtt_user", mqtt_user);
  prefs.putString("mqtt_pass", mqtt_pass);
}

void reopenConfigPortal(const String &apName)
{
  if (portalActive)
  {
    Serial.println("Config portal already open");
    return;
  }

  WiFi.setSleep(false);
  WiFi.mode(WIFI_STA);

  wm.startConfigPortal(apName.c_str());

  portalActive = true;
  portalDeadlineMs = millis() + (unsigned long)WIFI_PORTAL_TIMEOUT_S * 1000UL;
  Serial.println("Reopening configuration portal");
}
