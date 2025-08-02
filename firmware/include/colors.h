#pragma once
#include <Adafruit_NeoPixel.h>

const uint32_t colors[] = {
    Adafruit_NeoPixel::Color(255, 0, 0),    // Red
    Adafruit_NeoPixel::Color(0, 255, 0),    // Green
    Adafruit_NeoPixel::Color(0, 0, 255),    // Blue
    Adafruit_NeoPixel::Color(255, 255, 0),  // Yellow
    Adafruit_NeoPixel::Color(255, 0, 255),  // Magenta
    Adafruit_NeoPixel::Color(0, 255, 255),  // Cyan
    Adafruit_NeoPixel::Color(21, 184, 134), // Teal
    Adafruit_NeoPixel::Color(255, 255, 255) // White
};

constexpr uint8_t NUM_COLORS = sizeof(colors) / sizeof(colors[0]);