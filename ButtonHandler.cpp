#include <Arduino.h>
#include "HardwareConfig.h"
#include "ButtonHandler.h"
#include "ColorManager.h"
#include "EEPROMUtils.h"
#include "StateMachine.h"

constexpr unsigned long debounceDelay = 50;
constexpr unsigned long longPressDuration = 3000;
constexpr unsigned long doubleClickThreshold = 400;

bool inColorSelectMode = false;
uint8_t selectedColorIndex = 0;
unsigned long lastClickTime = 0;

static unsigned long buttonPressStartTime = 0;
static bool buttonPrev = false;
static bool calibrationTriggered = false;

void handleButton(unsigned long now, State &state)
{
  bool buttonCurr = digitalRead(CALC_BUTTON_PIN) == LOW;

  // --- Check long press first ---
  if (buttonCurr && buttonPrev && !inColorSelectMode && !calibrationTriggered &&
      (now - buttonPressStartTime >= longPressDuration))
  {
    Serial1.println("Entering calibration mode...");
    state = WAITING_FOR_OFF_PRESS;
    calibrationTriggered = true; // ✅ prevent re-trigger
  }

  // --- Handle edge (change in button state) ---
  if (buttonCurr != buttonPrev)
  {
    if ((now - lastClickTime) > debounceDelay)
    {
      if (buttonCurr)
      {
        // Button just pressed
        if ((now - lastClickTime) < doubleClickThreshold)
        {
          if (inColorSelectMode)
          {
            uint8_t r = COLOR_OPTIONS[selectedColorIndex][0];
            uint8_t g = COLOR_OPTIONS[selectedColorIndex][1];
            uint8_t b = COLOR_OPTIONS[selectedColorIndex][2];
            saveCurrentColor(r, g, b);
            blinkColor(r, g, b, 2, 150);
            inColorSelectMode = false;
            Serial1.println("Color saved.");
          }
          else
          {
            inColorSelectMode = true;
            selectedColorIndex = 0;
          }
        }

        buttonPressStartTime = now; // only set when press begins
      }
      else
      {
        // Button just released
        if (inColorSelectMode)
        {
          selectedColorIndex = (selectedColorIndex + 1) % NUM_COLORS;
          setAllPixels(
              COLOR_OPTIONS[selectedColorIndex][0],
              COLOR_OPTIONS[selectedColorIndex][1],
              COLOR_OPTIONS[selectedColorIndex][2]);
          calibrationTriggered = false;
        }
      }

      lastClickTime = now;
    }
  }

  buttonPrev = buttonCurr;
}
