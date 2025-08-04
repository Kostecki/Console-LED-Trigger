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

void connectToMqtt();
void mqttCallback(char *topic, byte *payload, unsigned int length);

void initWiFiAndMQTTAndOTA(Preferences &prefs)
{
  WiFiManager wm;

  String apName = "Console-LEDs-" + getMacSuffix();

  // Load saved MQTT config
  mqtt_server = prefs.getString("mqtt_server", "");
  mqtt_port = prefs.getString("mqtt_port", "1883").toInt();
  mqtt_user = prefs.getString("mqtt_user", "");
  mqtt_pass = prefs.getString("mqtt_pass", "");

  // Add to config portal
  WiFiManagerParameter p_mqtt_server("server", "MQTT Server", mqtt_server.c_str(), 40);
  WiFiManagerParameter p_mqtt_port("port", "MQTT Port", String(mqtt_port).c_str(), 6);
  WiFiManagerParameter p_mqtt_user("user", "MQTT User", mqtt_user.c_str(), 32);
  WiFiManagerParameter p_mqtt_pass("pass", "MQTT Password", mqtt_pass.c_str(), 32);

  wm.addParameter(&p_mqtt_server);
  wm.addParameter(&p_mqtt_port);
  wm.addParameter(&p_mqtt_user);
  wm.addParameter(&p_mqtt_pass);

  // Connect WiFi
  if (!wm.autoConnect(apName.c_str()))
  {
    Serial1.println("WiFi failed. Rebooting...");
    delay(3000);
    ESP.restart();
  }

  Serial1.print("WiFi connected! IP: ");
  Serial1.println(WiFi.localIP());

  // Save MQTT config
  mqtt_server = p_mqtt_server.getValue();
  mqtt_port = String(p_mqtt_port.getValue()).toInt();
  mqtt_user = p_mqtt_user.getValue();
  mqtt_pass = p_mqtt_pass.getValue();

  prefs.putString("mqtt_server", mqtt_server);
  prefs.putString("mqtt_port", String(mqtt_port));
  prefs.putString("mqtt_user", mqtt_user);
  prefs.putString("mqtt_pass", mqtt_pass);

  // NTP time sync
  Serial1.println("Syncing NTP time...");
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  time_t now = time(nullptr);
  int retry = 0;
  while (now < 100000 && retry++ < 30)
  {
    delay(100);
    now = time(nullptr);
  }

  if (now < 100000)
  {
    Serial1.println("Failed to sync NTP time");
  }
  else
  {
    Serial1.printf("NTP time synced: %lu\n", now);
  }

  // MQTT setup
  mqttClient.setServer(mqtt_server.c_str(), mqtt_port);
  mqttClient.setCallback(mqttCallback);

  // OTA setup
  ArduinoOTA
      .onStart([]()
               { Serial1.println("OTA update starting..."); })
      .onEnd([]()
             { Serial1.println("\nOTA complete"); })
      .onProgress([](unsigned int p, unsigned int t)
                  { Serial1.printf("OTA %u%%\r", (p * 100) / t); })
      .onError([](ota_error_t e)
               {
        Serial1.printf("OTA Error [%u]: ", e);
        if (e == OTA_AUTH_ERROR) Serial1.println("Auth Failed");
        else if (e == OTA_BEGIN_ERROR) Serial1.println("Begin Failed");
        else if (e == OTA_CONNECT_ERROR) Serial1.println("Connect Failed");
        else if (e == OTA_RECEIVE_ERROR) Serial1.println("Receive Failed");
        else if (e == OTA_END_ERROR) Serial1.println("End Failed"); });

  ArduinoOTA.setHostname(apName.c_str());
  ArduinoOTA.begin();

  Serial1.printf("OTA ready. Hostname: %s.local\n", apName.c_str());

  connectToMqtt();
}

void publishState()
{
  JsonDocument doc;
  doc["enabled"] = ledEnabled;
  doc["color"] = currentColorIndex;
  doc["brightness"] = currentBrightness;
  doc["bootTime"] = bootTime;

  char payload[128];
  serializeJson(doc, payload, sizeof(payload));

  String topic = "console/board-" + getMacSuffix() + "/state";
  mqttClient.publish(topic.c_str(), payload, true);
}

void connectToMqtt()
{
  Serial1.println("Connecting to MQTT...");

  String clientId = "board-" + getMacSuffix();

  if (mqttClient.connect(clientId.c_str(), mqtt_user.c_str(), mqtt_pass.c_str()))
  {
    Serial1.println("MQTT connected");

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
    Serial1.print("MQTT failed, rc=");
    Serial1.print(mqttClient.state());
    Serial1.println(" try again in 5 seconds");
  }
}

void handleMqttLoop()
{
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

  Serial1.printf("Starting OTA update from: %s\n", url.c_str());
  http.begin(client, url);

  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK)
  {
    Serial1.printf("HTTP GET failed: %d\n", httpCode);
    http.end();
    return;
  }

  int contentLength = http.getSize();
  if (contentLength <= 0)
  {
    Serial1.println("Invalid content length");
    http.end();
    return;
  }

  bool canBegin = Update.begin(contentLength);
  if (!canBegin)
  {
    Serial1.println("Not enough space for OTA");
    http.end();
    return;
  }

  WiFiClient *stream = http.getStreamPtr();
  size_t written = Update.writeStream(*stream);

  if (written != contentLength)
  {
    Serial1.printf("Only wrote %d of %d bytes\n", written, contentLength);
    http.end();
    return;
  }

  if (!Update.end())
  {
    Serial1.printf("Update failed. Error: %s\n", Update.errorString());
    http.end();
    return;
  }

  if (!Update.isFinished())
  {
    Serial1.println("Update not complete");
    http.end();
    return;
  }

  Serial1.println("OTA update complete! Rebooting...");
  http.end();
  delay(1000);
  ESP.restart();
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  payload[length] = '\0'; // Null-terminate
  String topicStr = String(topic);
  Serial1.printf("MQTT message on topic %s: %s\n", topic, (char *)payload);

  JsonDocument doc;
  deserializeJson(doc, (char *)payload);

  if (topicStr.endsWith("/set"))
  {
    if (doc["color"].is<int>())
    {
      int colorIndex = doc["color"];
      if (colorIndex >= 0 && colorIndex < NUM_COLORS)
      {
        currentColorIndex = colorIndex;
        prefs.putUChar("color", currentColorIndex);
        updateLED(true);
      }
    }

    if (doc["brightness"].is<int>())
    {
      int brightness = doc["brightness"];
      brightness = constrain(brightness, 0, 255);
      currentBrightness = brightness;
      prefs.putUChar("brightness", currentBrightness);
      strip.setBrightness(currentBrightness);
      updateLED(false);
    }
  }
  else if (topicStr.endsWith("/identify"))
  {
    for (int i = 0; i < 3; ++i)
    {
      fadeToColor(strip.Color(255, 255, 255), 10, 15);
      delay(100);
      fadeToColor(colors[currentColorIndex], 10, 15);
    }
  }
  else if (topicStr.endsWith("/reboot"))
  {
    delay(500);
    ESP.restart();
  }
  else if (topicStr.endsWith("/fw-update"))
  {
    if (doc["url"].is<const char *>())
    {
      performOTAUpdate(doc["url"].as<String>());
    }
  }
}