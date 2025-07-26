#pragma once
#include <Adafruit_NeoPixel.h>

enum State
{
  RUNNING,
  WAITING_FOR_OFF_PRESS,
  WAITING_FOR_ON_PRESS,
  WAITING_FOR_CONFIRM_PRESS
};

void handleStateMachine(unsigned long now, Adafruit_NeoPixel &strip, float smoothedCurrent, bool &ledsOn, unsigned long &ledOnTimestamp, State &state);
