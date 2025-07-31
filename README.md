# Console LED Trigger

This project controls RGB LED strips for retro gaming consoles using current sensing and an encoder-based UI. The LED module turns on when a console powers up and allows for user-configurable color and brightness using a rotary encoder with a button.

## Features

- 🎮 **Automatic Power Detection**: Turns LEDs on/off based on console current draw.
- 🌈 **Color Selection**: Rotate encoder to cycle through predefined colors.
- 💡 **Brightness Adjustment**: Long-press (2s) to enter brightness mode, rotate to adjust.
- 💾 **EEPROM Saving**: Saves selected color and brightness between power cycles.
- 🌙 **Soft Off Delay**: Waits 5 seconds after power off before fading out.
- ✨ **Smooth Fading**: Fades between colors and off-state with a polished transition.
- 📶 **WiFi Support**: Configurable via captive portal for OTA and future expansion.
- 🔄 **OTA Updates**: Update firmware wirelessly using ArduinoOTA.

## Hardware

- ESP32-C3 Super Mini  
- WS2812B LED strip  
- ZMCT103C current transformer  
- EC11 Rotary encoder with push button  
- TAS10-5-WEDT AC-DC 5V power supply  

## Controls

| Action                    | Behavior                      |
|---------------------------|-------------------------------|
| Rotate encoder            | Change LED color              |
| Click encoder (short)     | Save color or brightness      |
| Long press (≥ 2s)         | Toggle brightness mode        |
| Rotate in brightness mode | Adjust LED brightness         |

## Configuration

Adjust thresholds and pin assignments in `config.h` and `pins.h`.  
Predefined colors are listed in `colors.h`.

| Constant                  | Description                                               |
|---------------------------|-----------------------------------------------------------|
| `NUM_PIXELS`              | Number of WS2812 LEDs connected.                          |
| `CURRENT_THRESHOLD`       | Baseline ADC threshold to detect console power.           |
| `CURRENT_SENSE_OFFSET`    | Hysteresis value to prevent flickering near threshold.    |
| `CURRENT_THRESHOLD_ON`    | Calculated threshold to detect "power on".                |
| `CURRENT_THRESHOLD_OFF`   | Calculated threshold to detect "power off".               |
| `ENCODER_STEPS_PER_CLICK` | Encoder resolution adjustment.                            |
| `LONG_PRESS_THRESHOLD`    | Time required to trigger brightness mode with long press. |
| `POWER_OFF_DELAY`         | Time to wait before fading LEDs off after current drops.  |

## Connectivity

### WiFi Setup

- On first boot or if no WiFi is saved, the ESP32 starts an **Access Point** named `Console-LEDs-AP`.
- Connect to that network from your phone/laptop.
- A captive portal will appear. Enter your WiFi credentials.
- The ESP32 will save the SSID and password for future use.

If connection fails, the device reboots and retries WiFi/AP setup.

### OTA Updates

Once connected to WiFi:

- The device starts the **Arduino OTA service**.
- You can use the Arduino IDE or `arduino-cli` to push firmware updates over the network.

## Colors

| Index | Color   | Default |
|:-----:|---------|:-------:|
| 0     | Red     |    ✓    |
| 1     | Green   |         |
| 2     | Blue    |         |
| 3     | Yellow  |         |
| 4     | Magenta |         |
| 5     | Orange  |         |
| 6     | Cyan    |         |
| 7     | White   |         |
