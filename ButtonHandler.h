#pragma once

#include "StateMachine.h"

void handleButton(unsigned long now, State &state);

extern bool inColorSelectMode;
extern uint8_t selectedColorIndex;
extern unsigned long lastClickTime;