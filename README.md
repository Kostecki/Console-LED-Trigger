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

## EEPROM Layout

| Address         | Purpose           |
|----------------|-------------------|
| `EEPROM_ADDR_COLOR`      | Saved color index    |
| `EEPROM_ADDR_BRIGHTNESS` | Saved brightness (0–255) |
