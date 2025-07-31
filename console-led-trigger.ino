#include <WiFi.h>
#include <WiFiManager.h>
#include <Preferences.h>
#include <ArduinoOTA.h>
#include <RotaryEncoder.h>
#include <Adafruit_NeoPixel.h>

#include "pins.h"
#include "colors.h"
#include "config.h"

// Preferences setup
Preferences prefs;

// LED Setup
Adafruit_NeoPixel strip(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
bool ledEnabled = false;
bool inBrightnessMode = false;

// Encoder Setup
RotaryEncoder encoder(ENCODER_A, ENCODER_B, RotaryEncoder::LatchMode::FOUR3);
uint8_t currentColorIndex = 0;
uint8_t currentBrightness = 128;

unsigned long powerOffTime = 0;

void updateLED(bool force)
{
  uint32_t color = ledEnabled || force ? colors[currentColorIndex] : 0;
  for (int i = 0; i < NUM_PIXELS; ++i)
  {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void fadeToColor(uint32_t targetColor, uint8_t steps = 50, uint16_t delayMs = 25)
{
  uint32_t startColor = strip.getPixelColor(0);

  for (int i = 0; i <= steps; ++i)
  {
    uint32_t interp = strip.Color(
        lerpColorComponent((startColor >> 16) & 0xFF, (targetColor >> 16) & 0xFF, i, steps),
        lerpColorComponent((startColor >> 8) & 0xFF, (targetColor >> 8) & 0xFF, i, steps),
        lerpColorComponent(startColor & 0xFF, targetColor & 0xFF, i, steps));
    for (int j = 0; j < NUM_PIXELS; ++j)
    {
      strip.setPixelColor(j, interp);
    }
    strip.show();
    delay(delayMs);
  }
}

uint8_t lerpColorComponent(uint8_t from, uint8_t to, uint8_t step, uint8_t maxStep)
{
  return from + ((to - from) * step) / maxStep;
}

void setup()
{
  // Initialize Serial for debugging
  Serial1.begin(115200);
  delay(1000);
  Serial1.println("Console LED Trigger starting...");

  // Initialize WiFi
  WiFiManager wm;
  if (!wm.autoConnect("Console-LED-AP"))
  {
    Serial1.println("⚠️ Failed to connect to WiFi. Rebooting...");
    delay(3000);
    ESP.restart();
  }

  Serial1.print("✅ Connected to WiFi! IP: ");
  Serial1.println(WiFi.localIP());

  // Initialize OTA
  ArduinoOTA
      .onStart([]()
               { Serial1.println("🔄 OTA update starting..."); })
      .onEnd([]()
             { Serial1.println("\n✅ OTA update complete"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial1.printf("📶 OTA Progress: %u%%\r", (progress * 100) / total); })
      .onError([](ota_error_t error)
               {
    Serial1.printf("❌ OTA Error [%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial1.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial1.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial1.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial1.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial1.println("End Failed"); });

  ArduinoOTA.begin();
  Serial1.println("📡 OTA ready");

  // Initialize Preferences
  prefs.begin("led-config", false);

  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(CURRENT_SENSE_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(ENCODER_A), []
                  { encoder.tick(); }, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), []
                  { encoder.tick(); }, CHANGE);

  // Read saved color index from Preferences
  currentColorIndex = prefs.getUChar("color", 0);
  Serial1.println(currentColorIndex);

  // Read saved brightness from Preferences
  currentBrightness = prefs.getUChar("brightness", 128);
  Serial1.println(currentBrightness);

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
  ArduinoOTA.handle();

  int adc = analogRead(CURRENT_SENSE_PIN);

  static bool lastLedEnabled = false;

  if (!ledEnabled && adc > CURRENT_THRESHOLD_ON)
  {
    ledEnabled = true;
  }
  else if (ledEnabled && adc < CURRENT_THRESHOLD_OFF)
  {
    if (powerOffTime == 0)
      powerOffTime = millis();
    if (millis() - powerOffTime >= POWER_OFF_DELAY)
      ledEnabled = false;
  }
  else if (adc > CURRENT_THRESHOLD_OFF)
  {
    powerOffTime = 0;
  }

  if (ledEnabled != lastLedEnabled)
  {
    Serial1.println(ledEnabled ? "Turning ON LEDs" : "Turning OFF LEDs");
    if (ledEnabled)
    {
      fadeToColor(colors[currentColorIndex]);
    }
    else
    {
      fadeToColor(strip.Color(0, 0, 0));
    }
    lastLedEnabled = ledEnabled;
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
      currentColorIndex = (currentColorIndex + delta + NUM_COLORS) % NUM_COLORS;
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
    uint8_t previousBrightness = currentBrightness;

    strip.setBrightness(128);
    uint32_t originalColor = colors[currentColorIndex];
    fadeToColor(strip.Color(255, 255, 255), 10, 10);
    delay(100);
    fadeToColor(originalColor, 10, 10);
    strip.setBrightness(previousBrightness);

    updateLED(false);
    feedbackShown = true;
  }

  if (!buttonPressed && wasButtonPressed)
  {
    unsigned long pressDuration = millis() - pressStartTime;

    if (pressDuration >= LONG_PRESS_THRESHOLD)
    {
      inBrightnessMode = !inBrightnessMode;
      Serial1.println(inBrightnessMode ? "Entered brightness mode" : "Exited brightness mode");
    }
    else
    {
      if (inBrightnessMode)
      {
        prefs.putUChar("brightness", currentBrightness);
        Serial1.print("Saved brightness: ");
        Serial1.println(currentBrightness);
        Serial1.println("Exiting brightness mode");
        inBrightnessMode = false;
      }
      else
      {
        prefs.putUChar("color", currentColorIndex);
        Serial1.print("Saved color to Preferences: ");
        Serial1.println(currentColorIndex);
      }
    }
  }

  wasButtonPressed = buttonPressed;
  delay(10);
}
