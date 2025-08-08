# Console LED Trigger

This project controls RGB LED strips for retro gaming consoles using current sensing and an encoder-based UI. The LED module turns on when a console powers up and allows for user-configurable color and brightness using a rotary encoder with a button.

## Features

- 🎮 **Automatic Power Detection**: Turns LEDs on/off based on console current draw.
- 🌈 **Color Selection**: Rotate encoder to cycle through predefined colors.
- 💡 **Brightness Adjustment**: Long-press (2s) to enter brightness mode, rotate to adjust.
- 💾 **Persistent Settings**: Saves selected color and brightness between power cycles.
- 🌙 **Soft Off Delay**: Waits 5 seconds after power off before fading out.
- ✨ **Smooth Fading**: Fades between colors and off-state with a polished transition.
- 📶 **WiFi Support**: Configurable via captive portal for OTA and future expansion. (optional)
- 🧭 **Dashboard Control**: View all connected modules, their status, and update settings  from a web UI. (requires WiFi)
- 🔄 **OTA Updates**: Update firmware wirelessly using ArduinoOTA. (requires WiFi)

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

## Wi-Fi Enable Jumper
Wi-Fi and OTA functionality is **only initialized if a jumper is placed** across the first two pins (left to right) of the 3-pin header at boot.

## Connectivity

### WiFi Setup

- On first boot or if no WiFi is saved, the ESP32 starts an **Access Point** named `Console-LED-AP`.
- Connect to that network from your phone/laptop.
- A captive portal will appear. Enter your WiFi credentials.
- The ESP32 will save the SSID and password for future use.

If connection fails, the device reboots and retries WiFi/AP setup.

### Web Dashboard

If Wi-Fi is enabled, modules will appear in the web dashboard.

#### From the dashboard, you can:
- View all online/offline devices
- See current color, brightness, and power state
- Update color and brightness
- Change device display name

The dashboard runs separately under `/dashboard`, built with TypeScript, TanStack, and Mantine.

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

## BOM

| Name                    | Quantity | Description                                |
|-------------------------|----------|--------------------------------------------|
| 1000uF                  | 1        | Electrolytic capacitor                     |
| 100uF                   | 1        | Electrolytic capacitor                     |
| 100nF                   | 4        | Ceramic capacitors                         |
| 330R                    | 1        | Resistor                                   |
| 1k                      | 1        | Resistor                                   |
| 10k                     | 3        | Resistor                                   |
| 1N4148TR                | 2        | Diode                                      |
| Pin Header 2.54mm 3-pin | 1        | Jumper for Wi-Fi enable toggle             |
| Pin Header 2.54mm 6-pin | 1        | UART programming/debug header              |
| 15EDGRC-3.5-04P-14-00A  | 1        | 4-pin 5.0mm Pluggable terminal block       |
| KF128-5.0-3P            | 2        | 3-pin 5.0mm Screw terminal block           |
| 0PTF0078P               | 1        | PCB Glass fuse holder                      |
| SN74AHCT125N            | 1        | Quad bus buffer IC (logic level shifter)   |
| MCP6002-I/P             | 1        | Dual op-amp                                |
| TAS10-5-WEDT            | 1        | 5V AC-DC isolated power module (10W)       |
| EC110201E2J-HA1-005     | 1        | EC11 rotary encoder with switch            |
| ZMCT103C                | 1        | Current transformer (AC sensing)           |
| ESP32-C3 Super Mini     | 1        | Wi-Fi enabled MCU.                         |