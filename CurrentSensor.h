#pragma once

float readCurrent();
float readAverageCurrent(int samples = 50, int delayMs = 100);
float smoothCurrent(float currentRaw, float smoothedCurrent);
