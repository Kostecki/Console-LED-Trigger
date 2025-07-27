#include <EEPROM.h>
#include <Encoder.h>
#include <Adafruit_NeoPixel.h>

#include <pins.h>
#include <colors.h>
#include <config.h>

// LED Setup
Adafruit_NeoPixel strip(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
bool ledEnabled = false;

// Encoder Setup
Encoder encoder(ENCODER_A, ENCODER_B);
uint8_t currentColorIndex = 0;

void setup()
{
  Serial1.begin(115200);
  delay(1000); // Allow time for Serial Monitor to open
  Serial1.println("Console LED Trigger starting...");

  EEPROM.begin(64);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(CURRENT_SENSE_PIN, INPUT);

  // Read saved color index from EEPROM
  currentColorIndex = EEPROM.read(EEPROM_ADDR);
  if (currentColorIndex >= NUM_COLORS)
  {
    currentColorIndex = 0;
    EEPROM.write(EEPROM_ADDR, currentColorIndex); // Save default red
    EEPROM.commit();
  }

  strip.begin();
  strip.setBrightness(128);
  strip.clear();
  strip.show();

  updateLED(true);
}

void loop()
{
  int adc = analogRead(CURRENT_SENSE_PIN);
  Serial1.println("ADC Value: " + String(adc));
  delay(1000); // Slow down ADC reads for stability

  static bool lastLedEnabled = false;
  static unsigned long powerOffTime = 0;

  // Check current draw and determine LED state
  if (!ledEnabled && adc > CURRENT_THRESHOLD_ON)
  {
    ledEnabled = true;
  }
  else if (ledEnabled && adc < CURRENT_THRESHOLD_OFF)
  {
    if (powerOffTime == 0)
      powerOffTime = millis();

    // Wait 5 seconds before turning off
    if (millis() - powerOffTime >= 5000)
      ledEnabled = false;
  }
  else if (adc > CURRENT_THRESHOLD_OFF)
  {
    // Reset timer if current rises again during countdown
    powerOffTime = 0;
  }

  // Handle LED transition
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

  // Handle encoder rotation
  static int lastEncoderPosition = 0;
  int encoderPosition = encoder.read() / ENCODER_STEPS_PER_CLICK;

  int delta = encoderPosition - lastEncoderPosition;
  if (delta != 0)
  {
    lastEncoderPosition = encoderPosition;
    currentColorIndex = (currentColorIndex + delta + NUM_COLORS) % NUM_COLORS;
    updateLED(false);
  }

  // Handle encoder click
  if (digitalRead(ENCODER_SW) == LOW)
  {
    if (EEPROM.read(EEPROM_ADDR) != currentColorIndex)
    {
      EEPROM.write(EEPROM_ADDR, currentColorIndex);
      EEPROM.commit();
    }
    delay(300);
  }

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