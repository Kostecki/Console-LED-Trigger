#pragma once

#include <Preferences.h>

void initWiFiAndMQTTAndOTA(Preferences &prefs);
void handleMqttLoop();
void publishState();