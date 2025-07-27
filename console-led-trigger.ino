#include <EEPROM.h>
#include <RotaryEncoder.h>
#include <Adafruit_NeoPixel.h>

#include "pins.h"
#include "colors.h"
#include "config.h"

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
  Serial1.begin(115200);
  delay(1000);
  Serial1.println("Console LED Trigger starting...");

  EEPROM.begin(64);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(CURRENT_SENSE_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(ENCODER_A), []
                  { encoder.tick(); }, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), []
                  { encoder.tick(); }, CHANGE);

  // Read saved color index from EEPROM
  currentColorIndex = EEPROM.read(EEPROM_ADDR_COLOR);
  Serial1.print("Saved color index: ");
  Serial1.println(currentColorIndex);
  if (currentColorIndex >= NUM_COLORS)
  {
    currentColorIndex = 0;
    EEPROM.write(EEPROM_ADDR_COLOR, currentColorIndex);
    EEPROM.commit();
  }

  // Read saved brightness from EEPROM
  currentBrightness = EEPROM.read(EEPROM_ADDR_BRIGHTNESS);
  Serial1.print("Saved brightness: ");
  Serial1.println(currentBrightness);
  if (currentBrightness == 0xFF || currentBrightness == 0)
  {
    currentBrightness = 128;
    EEPROM.write(EEPROM_ADDR_BRIGHTNESS, currentBrightness);
    EEPROM.commit();
  }

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
        EEPROM.write(EEPROM_ADDR_BRIGHTNESS, currentBrightness);
        EEPROM.commit();
        Serial1.print("Saved brightness: ");
        Serial1.println(currentBrightness);
        Serial1.println("Exiting brightness mode");
        inBrightnessMode = false;
      }
      else
      {
        if (EEPROM.read(EEPROM_ADDR_COLOR) != currentColorIndex)
        {
          EEPROM.write(EEPROM_ADDR_COLOR, currentColorIndex);
          EEPROM.commit();
          Serial1.print("Saved color to EEPROM: ");
          Serial1.println(currentColorIndex);
        }
      }
    }
  }

  wasButtonPressed = buttonPressed;
  delay(10);
}
