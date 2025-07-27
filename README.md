# Console LED Trigger

This project controls RGB LED strips for retro gaming consoles using current sensing and an encoder-based UI. The LED module turns on when a console powers up and allows for user-configurable color and brightness using a rotary encoder with a button.

## Features

- 🎮 **Automatic Power Detection**: Turns LEDs on/off based on console current draw.
- 🌈 **Color Selection**: Rotate encoder to cycle through predefined colors.
- 💡 **Brightness Adjustment**: Long-press (2s) to enter brightness mode, rotate to adjust.
- 💾 **EEPROM Saving**: Saves selected color and brightness between power cycles.
- 🌙 **Soft Off Delay**: Waits 5 seconds after power off before fading out.
- ✨ **Smooth Fading**: Fades between colors and off-state with a polished transition.

## Hardware

- RP2040-Zero
- WS2812B LED strip
- ZMCT103C current transformer
- EC11 Rotary encoder with push button
- Mean Well IRM-10-5 5V power supply

## Controls

| Action                   | Behavior                      |
|--------------------------|-------------------------------|
| Rotate encoder           | Change LED color              |
| Click encoder (short)    | Save color or brightness      |
| Long press (≥ 2s)        | Toggle brightness mode        |
| Rotate in brightness mode| Adjust LED brightness         |

## Configuration

Adjust thresholds and pin assignments in `config.h` and `pins.h`.  
Predefined colors are listed in `colors.h`.

| Constant                  | Description                                               |
| ------------------------- | --------------------------------------------------------- |
| `EEPROM_ADDR_COLOR`       | EEPROM address to store selected color index.             |
| `EEPROM_ADDR_BRIGHTNESS`  | EEPROM address to store brightness level (0-255).         |
| `NUM_PIXELS`              | Number of WS2812 LEDs connected.                          |
| `CURRENT_THRESHOLD`       | Baseline ADC threshold to detect console power.           |
| `CURRENT_SENSE_OFFSET`    | Hysteresis value to prevent flickering near threshold.    |
| `CURRENT_THRESHOLD_ON`    | Calculated threshold to detect "power on".                |
| `CURRENT_THRESHOLD_OFF`   | Calculated threshold to detect "power off".               |
| `ENCODER_STEPS_PER_CLICK` | Encoder resolution adjustment.                            |
| `LONG_PRESS_THRESHOLD`    | Time required to trigger brightness mode with long press. |
| `POWER_OFF_DELAY`         | Time to wait before fading LEDs off after current drops.  |


## Colors
| Index | Color   | Default |
|:-----:| ------- |:-------:|
| 0     | Red     |    ✓    |
| 1     | Green   |         |
| 2     | Blue    |         |
| 3     | Yellow  |         |
| 4     | Magenta |         |
| 5     | Orange  |         |
| 6     | Purple  |         |
| 7     | Cyan    |         |
| 8     | White   |         |
