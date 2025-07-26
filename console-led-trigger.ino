#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include "HardwareConfig.h"
#include "ButtonHandler.h"
#include "ColorManager.h"
#include "CurrentSensor.h"
#include "EEPROMUtils.h"
#include "StateMachine.h"

Adafruit_NeoPixel strip(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

State state = RUNNING;
bool firstCurrentRead = true;
float smoothedCurrent = 0.0;
bool ledsOn = false;
unsigned long ledOnTimestamp = 0;

unsigned long now = 0;

void setup()
{
  Serial1.println("Console LED Trigger starting...");
  Serial1.begin(115200);
  delay(1000); // Allow time for Serial Monitor to open

  EEPROM.begin(EEPROM_SIZE);

  strip.begin();
  strip.setBrightness(128);
  pinMode(CALC_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SENSOR_PIN, INPUT);
  clearStrip();

  loadThreshold();
  loadSavedColor();

  Serial1.println("Normal mode. Hold button 3 sec to enter calibration, double-click to select color.");
}

void loop()
{
  now = millis();

  float currentRaw = readCurrent();
  if (firstCurrentRead)
  {
    smoothedCurrent = currentRaw;
    firstCurrentRead = false;
  }
  else
  {
    smoothedCurrent = smoothCurrent(currentRaw, smoothedCurrent);
  }

  handleButton(now, state);
  handleStateMachine(now, strip, smoothedCurrent, ledsOn, ledOnTimestamp, state);

  delay(50);
}
