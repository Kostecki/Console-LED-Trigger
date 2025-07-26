#include <Arduino.h>
#include "ColorManager.h"
#include "CurrentSensor.h"
#include "EEPROMUtils.h"
#include "StateMachine.h"
#include "HardwareConfig.h"

constexpr unsigned long TIMEOUT_DELAY = 2500;

void handleStateMachine(unsigned long now, Adafruit_NeoPixel &strip, float smoothedCurrent, bool &ledsOn, unsigned long &ledOnTimestamp, State &state)
{
  static unsigned long buttonPressStartTime = 0;

  switch (state)
  {
  case RUNNING:
    if (!ledsOn && smoothedCurrent > currentThresholdOn)
    {
      setAllPixels(savedR, savedG, savedB);
      ledsOn = true;
      ledOnTimestamp = now;
    }
    else if (ledsOn && smoothedCurrent < (currentThresholdOn - THRESHOLD_MARGIN))
    {
      clearStrip();
      ledsOn = false;
    }
    break;

  case WAITING_FOR_OFF_PRESS:
    if (smoothedCurrent < (currentThresholdOn - THRESHOLD_MARGIN))
    {
      Serial1.println("Power OFF detected, press button to set ON threshold");
      buttonPressStartTime = now;
      state = WAITING_FOR_ON_PRESS;
    }
    break;

  case WAITING_FOR_ON_PRESS:
    if (smoothedCurrent > (currentThresholdOn - THRESHOLD_MARGIN))
    {
      Serial1.println("Power ON detected, press button to confirm threshold");
      state = WAITING_FOR_CONFIRM_PRESS;
    }
    break;

  case WAITING_FOR_CONFIRM_PRESS:
    Serial1.print("Waiting for button press to confirm ON threshold (current: ");
    Serial1.print(smoothedCurrent, 3);

    if (digitalRead(CALC_BUTTON_PIN) == LOW)
    {
      float newThreshold = smoothedCurrent;
      saveThreshold(newThreshold);

      Serial1.print("Saved ON threshold: ");
      Serial1.println(newThreshold, 3);

      state = RUNNING;
    }
    break;
  }
}
