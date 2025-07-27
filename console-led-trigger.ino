#include <EEPROM.h>
#include <Encoder.h>
#include <Adafruit_NeoPixel.h>

#include <pins.h>
#include <colors.h>
#include <config.h>

// LED Setup
Adafruit_NeoPixel strip(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
bool ledEnabled = false;
bool inBrightnessMode = false;

// Encoder Setup
Encoder encoder(ENCODER_A, ENCODER_B);
uint8_t currentColorIndex = 0;
uint8_t currentBrightness = 128;

void setup()
{
  Serial1.begin(115200);
  delay(1000);
  Serial1.println("Console LED Trigger starting...");

  EEPROM.begin(64);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(CURRENT_SENSE_PIN, INPUT);

  // Read saved color index from EEPROM
  currentColorIndex = EEPROM.read(EEPROM_ADDR_COLOR);
  if (currentColorIndex >= NUM_COLORS)
  {
    currentColorIndex = 0;
    EEPROM.write(EEPROM_ADDR_COLOR, currentColorIndex);
    EEPROM.commit();
  }

  // Read saved brightness from EEPROM
  currentBrightness = EEPROM.read(EEPROM_ADDR_BRIGHTNESS);
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

  updateLED(true);
}

void loop()
{
  int adc = analogRead(CURRENT_SENSE_PIN);
  Serial1.println("ADC Value: " + String(adc));
  delay(1000);

  static bool lastLedEnabled = false;
  static unsigned long powerOffTime = 0;

  if (!ledEnabled && adc > CURRENT_THRESHOLD_ON)
  {
    ledEnabled = true;
  }
  else if (ledEnabled && adc < CURRENT_THRESHOLD_OFF)
  {
    if (powerOffTime == 0)
      powerOffTime = millis();
    if (millis() - powerOffTime >= powerOffDelay)
      ledEnabled = false;
  }
  else if (adc > CURRENT_THRESHOLD_OFF)
  {
    powerOffTime = 0;
  }

  if (ledEnabled != lastLedEnabled)
  {
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

  // Handle encoder input
  static int lastEncoderPosition = 0;
  int encoderPosition = encoder.read() / ENCODER_STEPS_PER_CLICK;
  int delta = encoderPosition - lastEncoderPosition;

  if (delta != 0)
  {
    lastEncoderPosition = encoderPosition;

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
  static unsigned long pressStartTime = 0;

  bool buttonPressed = digitalRead(ENCODER_SW) == LOW;

  if (buttonPressed && !wasButtonPressed)
  {
    pressStartTime = millis();
  }

  if (!buttonPressed && wasButtonPressed)
  {
    unsigned long pressDuration = millis() - pressStartTime;

    if (pressDuration >= longPressThreshold)
    {
      inBrightnessMode = !inBrightnessMode;
      Serial1.println(inBrightnessMode ? "Entered brightness mode" : "Exited brightness mode");
    }
    else
    {
      if (inBrightnessMode)
      {
        Serial1.println("Saved brightness & exiting brightness mode");
        EEPROM.write(EEPROM_ADDR_BRIGHTNESS, currentBrightness);
        EEPROM.commit();
        inBrightnessMode = false;
      }
      else
      {
        if (EEPROM.read(EEPROM_ADDR_COLOR) != currentColorIndex)
        {
          EEPROM.write(EEPROM_ADDR_COLOR, currentColorIndex);
          EEPROM.commit();
          Serial1.println("Saved color to EEPROM");
        }
      }
    }
  }

  wasButtonPressed = buttonPressed;
  delay(10);
}

void updateLED(bool force)
{
  uint32_t color = ledEnabled || force ? colors[currentColorIndex] : 0;
  for (int i = 0; i < NUM_PIXELS; ++i)
  {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void fadeToColor(uint32_t targetColor, uint8_t steps = 20, uint16_t delayMs = 10)
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
