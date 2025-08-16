#include <Preferences.h>
#include <ArduinoOTA.h>
#include <RotaryEncoder.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiManager.h>
#include <time.h>

#include <state.h>
#include <wifi_mqtt_ota_setup.h>
#include <pins.h>
#include <colors.h>
#include <config.h>
#include <utils.h>
#include <serial_mux.h>

// Preferences setup
Preferences prefs;

// LED Setup
Adafruit_NeoPixel strip(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
bool ledEnabled = false;
bool inBrightnessMode = false;

// Encoder Setup
RotaryEncoder encoder(ENCODER_A, ENCODER_B, RotaryEncoder::LatchMode::FOUR3);

// Color management
ColorMode colorMode = ColorMode::Palette;
uint8_t currentColorIndex = 0;
uint32_t customColor = 0x000000;

// Current brightness
uint8_t currentBrightness = 128;

String deviceName = "";
unsigned long powerOffTime = 0;
bool wifi_enabled = false;
time_t bootTime = 0;

static bool wasResetButtonPressed = false;
static bool wasCalButtonPressed = false;

// Runtime current threshold (loaded from NVS. Fallback to config default)
int currentThreshold = CURRENT_THRESHOLD;

void setup()
{

  // Initialize Serial
  SerialBegin(115200);
  delay(1000);
  Serial.println();
  Serial.println("Console LED Trigger starting");

  // Initialize Pins
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(CURRENT_SENSE_PIN, INPUT);
  pinMode(POWER_CALIBRATE_PIN, INPUT_PULLUP);
  pinMode(WIFI_TOGGLE, INPUT_PULLUP);
  pinMode(WIFI_RESET, INPUT_PULLUP);

  // Initialize Preferences
  prefs.begin("led-config", false);

  // Load calibrated threshold if available
  currentThreshold = prefs.getInt("current_threshold", CURRENT_THRESHOLD);
  Serial.print("Current threshold: ");
  Serial.println(currentThreshold);

  // Handle device name
  deviceName = prefs.getString("name", "");
  if (deviceName.isEmpty())
  {
    deviceName = "Console-" + getMacSuffix();
    prefs.putString("name", deviceName);
  }
  Serial.print("Device name: ");
  Serial.println(deviceName);

  wifi_enabled = digitalRead(WIFI_TOGGLE) == LOW;

  // Setup WiFi and OTA
  if (wifi_enabled)
  {
    Serial.println();
    Serial.println("WiFi Enabled: Setting up WiFi, MQTT, and OTA");
    initWiFiAndMQTTAndOTA(prefs);
  }
  else
  {
    Serial.println("WiFi Disabled: Skipping WiFi, MQTT and OTA setup");
  }

  bootTime = getSyncedUnixTime();
  Serial.printf("Boot time set: %lu\n", bootTime);

  attachInterrupt(digitalPinToInterrupt(ENCODER_A), []
                  { encoder.tick(); }, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), []
                  { encoder.tick(); }, CHANGE);

  // Read saved color index from Preferences
  colorMode = static_cast<ColorMode>(prefs.getUChar("color_mode", 0));
  currentColorIndex = prefs.getUChar("color_index", 0);
  String hex = prefs.getString("custom_color", "000000");
  customColor = (uint32_t)strtoul(hex.c_str(), nullptr, 16);
  Serial.println();
  Serial.print("Color mode: ");
  Serial.println(colorModeToString(colorMode));
  if (colorMode == ColorMode::Custom)
  {
    Serial.print("Custom color: #");
    Serial.println(String(customColor, HEX));
  }
  else
  {
    Serial.print("Current color index: ");
    Serial.println(currentColorIndex);
  }

  // Read saved brightness from Preferences
  currentBrightness = prefs.getUChar("brightness", 128);
  Serial.print("Current brightness: ");
  Serial.println(currentBrightness);
  Serial.println();

  strip.begin();
  strip.setBrightness(currentBrightness);
  strip.clear();
  strip.show();

  ledEnabled = false;

  delay(100);

  updateLED(false);
}

void loop()
{
  // Handle WiFi reset button
  bool resetBtnPressed = digitalRead(WIFI_RESET) == LOW;
  if (resetBtnPressed && !wasResetButtonPressed)
  {
    delay(50);
    if (digitalRead(WIFI_RESET) == LOW)
    {
      Serial.println();
      Serial.print("Reset button pressed:");
      if (wifi_enabled)
      {
        Serial.println(" Resetting WiFi and MQTT settings");
        WiFiManager wifiManager;
        wifiManager.resetSettings();
        Serial.println("WiFi and MQTT settings reset. Restarting");
      }
      else
      {
        Serial.println(" Restarting without resetting WiFi and MQTT settings");
      }
      Serial.println();

      ESP.restart();
    }
  }
  wasResetButtonPressed = resetBtnPressed;

  // Single-press calibration on POWER_CALIBRATE-PIN
  bool calPressed = (digitalRead(POWER_CALIBRATE_PIN) == LOW);
  if (calPressed && !wasCalButtonPressed)
  {
    delay(50);
    if (digitalRead(POWER_CALIBRATE_PIN) == LOW)
    {
      Serial.println();
      Serial.println("Calibration button pressed. Sampling ADC");

      int baseline = readAdcAverage(CURRENT_SENSE_PIN, 64);
      currentThreshold = baseline;
      prefs.putInt("current_threshold", currentThreshold);
      Serial.print("Calibrated baseline saved: ");
      Serial.println(currentThreshold);
      Serial.print("TH_ON / TH_OFF: ");
      Serial.print(currentThreshold + CURRENT_THRESHOLD_OFFSET);
      Serial.print(" / ");
      Serial.println(currentThreshold - CURRENT_THRESHOLD_OFFSET);
      Serial.println();

      blinkConfirm(strip.Color(255, 255, 255), 2);
    }
  }
  wasCalButtonPressed = calPressed;

  if (wifi_enabled)
  {
    handleMqttLoop();
    ArduinoOTA.handle();
  }

  int adc = analogRead(CURRENT_SENSE_PIN);

  // Serial.printf("ADC Value: %d\n", adc, " > ", CURRENT_THRESHOLD_ON);
  // delay(500);

  static bool lastLedEnabled = false;

  // Use runtime threshold derived from calibrated baseline + fixed offset
  const int TH_ON = currentThreshold + CURRENT_THRESHOLD_OFFSET;
  const int TH_OFF = currentThreshold - CURRENT_THRESHOLD_OFFSET;

  if (!ledEnabled && adc > TH_ON)
  {
    ledEnabled = true;
  }
  else if (ledEnabled && adc < TH_OFF)
  {
    if (powerOffTime == 0)
      powerOffTime = millis();
    if (millis() - powerOffTime >= POWER_OFF_DELAY)
      ledEnabled = false;
  }
  else if (adc > TH_OFF)
  {
    powerOffTime = 0;
  }

  if (ledEnabled != lastLedEnabled)
  {
    Serial.println(ledEnabled ? "Turning ON LEDs" : "Turning OFF LEDs");
    if (ledEnabled)
    {
      uint32_t fadeTarget = (colorMode == ColorMode::Palette && currentColorIndex < NUM_COLORS)
                                ? colors[currentColorIndex]
                                : customColor;

      fadeToColor(fadeTarget);
    }
    else
    {
      fadeToColor(strip.Color(0, 0, 0));
    }
    lastLedEnabled = ledEnabled;

    if (wifi_enabled)
    {
      publishState();
    }
  }

  // Handle encoder input using getDirection()
  RotaryEncoder::Direction dir = encoder.getDirection();
  if (dir != RotaryEncoder::Direction::NOROTATION)
  {
    int delta = (dir == RotaryEncoder::Direction::CLOCKWISE) ? 1 : -1;

    if (inBrightnessMode)
    {
      int newBrightness = constrain(currentBrightness + delta * 5, 0, 255);
      if (newBrightness != currentBrightness)
      {
        currentBrightness = newBrightness;
        strip.setBrightness(currentBrightness);
        updateLED(false);
      }
    }
    else
    {
      colorMode = ColorMode::Palette;
      currentColorIndex = (currentColorIndex + delta + NUM_COLORS) % NUM_COLORS;
      prefs.putUChar("color_mode", static_cast<uint8_t>(colorMode));
      prefs.putUChar("color_index", currentColorIndex);
      updateLED(false);
    }
  }

  // Handle button presses
  static bool wasButtonPressed = false;
  static bool feedbackShown = false;
  static unsigned long pressStartTime = 0;

  bool buttonPressed = digitalRead(ENCODER_SW) == LOW;

  if (buttonPressed && !wasButtonPressed)
  {
    pressStartTime = millis();
    feedbackShown = false;
  }

  // Show when brightness mode has been activated and button can be released
  if (buttonPressed && !feedbackShown && millis() - pressStartTime >= LONG_PRESS_THRESHOLD)
  {
    blinkConfirm(strip.Color(255, 255, 255), 2);
    feedbackShown = true;
  }

  if (!buttonPressed && wasButtonPressed)
  {
    unsigned long pressDuration = millis() - pressStartTime;

    if (pressDuration >= LONG_PRESS_THRESHOLD)
    {
      inBrightnessMode = !inBrightnessMode;
      Serial.println(inBrightnessMode ? "Entered brightness mode" : "Exited brightness mode");
    }
    else
    {
      if (inBrightnessMode)
      {
        prefs.putUChar("brightness", currentBrightness);
        Serial.print("Saved brightness: ");
        Serial.println(currentBrightness);
        Serial.println("Exiting brightness mode");
        inBrightnessMode = false;

        if (wifi_enabled)
        {
          publishState();
        }
      }
      else
      {
        colorMode = ColorMode::Palette;
        prefs.putUChar("color_mode", static_cast<uint8_t>(colorMode));
        prefs.putUChar("color_index", currentColorIndex);
        Serial.print("Saved color to Preferences: ");
        Serial.println(currentColorIndex);

        if (wifi_enabled)
        {
          publishState();
        }
      }
    }
  }

  wasButtonPressed = buttonPressed;
  delay(10);
}
