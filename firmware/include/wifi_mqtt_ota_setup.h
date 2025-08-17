#pragma once

#include <Arduino.h>

#include <Preferences.h>

void wifiKickoff(const String &apName, Preferences &prefs);
void wifiProcess(Preferences &prefs);
bool wifiIsConnected();
void maybeInitNetServices(Preferences &prefs);
void handleMqttLoop();
void publishState();
void publishHAState();
void reopenConfigPortal(const String &apName);