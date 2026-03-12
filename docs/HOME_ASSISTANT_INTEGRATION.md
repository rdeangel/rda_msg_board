# Home Assistant MQTT Integration Guide

This firmware supports **Home Assistant MQTT Discovery**, allowing the message board to be automatically detected and configured in Home Assistant without manual YAML configuration.

## Overview

When enabled, the device publishes discovery payloads to the configured MQTT broker. Home Assistant listens for these payloads and automatically creates a device entry with all relevant controls (Inputs, Sensors, and Buttons).

## Setup

1.  **Configure MQTT**:
    *   Navigate to the **MQTT Config** page in the device web interface.
    *   Enable **MQTT**.
    *   Enter your Broker Address, Port, Username, and Password.
    *   Set the **Topic Prefix** (default: `rdadotmatrix`).

2.  **Enable Discovery**:
    *   Toggle **Home Assistant Discovery** to **ON**.
    *   Save settings.

3.  **Home Assistant**:
    *   Ensure the MQTT integration is installed and active in Home Assistant.
    *   The device should appear automatically under **Settings > Devices & Services > MQTT**.

## Discovered Entities

The integration creates a unified device named **RDA MSG Board [ChipID]**. The following entities are available:

### Controls (Inputs)
These entities allow you to control the device settings directly from Home Assistant.

| Entity Name | Type | Domain | Description |
| :--- | :--- | :--- | :--- |
| **Message** | Text | `text` | Input field to type the message to be displayed. (Max 255 chars) |
| **Brightness** | Number | `number` | Slider (0-15) to set brightness for the *next* message. |
| **Scroll Delay** | Number | `number` | Slider (1-200 ms) to control scrolling speed. Lower is faster. |
| **Repeat Count** | Number | `number` | Number of times the message scrolls (0-999). |
| **Buzzer Chirps** | Number | `number` | Number of beeps when message arrives (0-99). |
| **Message Sound** | Select | `select` | Choose musical chirp pattern for alert. |
| **ASCII Conversion** | Switch | `switch` | Toggle extended ASCII character conversion (ON/OFF). |
| **MQTT Messages** | Switch | `switch` | Toggle display of incoming MQTT messages (ON/OFF). Preservation of connection. |
| **Force Repetitions** | Switch | `switch` | When ON, the next HA Send locks the display for the full repeat cycle — any new messages are rejected with `409` until all repetitions complete (ON/OFF). |
| **Buzzer Enable** | Switch | `switch` | Global master toggle for all audible alerts (ON/OFF). |
| **Bright. Override Enable**| Switch | `switch` | Global display brightness override toggle (ON/OFF). |
| **Bright. Override Value**| Number | `number` | Global brightness override level (0-15). |
| **Clock Enable** | Switch | `switch` | Toggle the Clock display mode (ON/OFF). |
| **Clock Brightness** | Number | `number` | Slider (0-15) for Clock display brightness. |
| **Clock Transition Speed** | Number | `number` | Transition speed in ms (10-200). Lower is faster. |
| **Clock Transition Effect** | Select | `select` | Choose the transition animation (WIPE, SCROLL, FADE, etc). |
| **Clock Randomize Transitions** | Switch | `switch` | Randomize the transition effect for every minute change. |
| **Clock NTP Server** | Text | `text` | NTP Server address (e.g., pool.ntp.org) (Config). |
| **Clock Timezone (POSIX)**| Text | `text` | POSIX Timezone string (e.g., `EST5EDT,M3.2.0,M11.1.0`). |
| **Clock Date Format** | Select | `select` | Choose date display format (TIME_ONLY, TIME_DATE, etc). |
| **Clock Date Alternate Interval** | Number | `number` | Interval in seconds to alternate date (4-module only). |
| **Clock Custom Date Format** | Text | `text` | Custom string for date format (8-module only). |
| **Clock Transition Delay** | Number | `number` | Display duration of static clock/date in ms before transition. |
| **Clock Resync Interval** | Number | `number` | Hours between NTP time synchronizations (Config). |
| **Sleep Mode Enable** | Switch | `switch` | Enable/disable scheduled display blackout (ON/OFF). |
| **Sleep Mode On Time** | Text | `time` | Time to turn display OFF (HH:MM format). |
| **Sleep Mode Off Time** | Text | `time` | Time to turn display ON (HH:MM format). |
| **Sleep Mode Mute Only** | Switch | `switch` | If ON, only alerts are muted; display remains ON. |
| **Recurrent Alarm Enable** | Switch | `switch` | Enable/disable recurrent alarm system (ON/OFF) (ESP32 Only). |
| **Recurrent Alarm Interval** | Select | `select` | Alarm interval (15min, 1hour, etc) (ESP32 Only). |
| **Recurrent Alarm Sound** | Select | `select` | Alarm musical chirp pattern (ESP32 Only). |

### Timer/Stopwatch Controls
These entities control the Timer and Stopwatch features. Can be disabled on ESP8266 via the `-DDISABLE_TIMER_FEATURE` build flag.

| Entity Name | Type | Domain | Description |
| :--- | :--- | :--- | :--- |
| **Timer Enabled** | Switch | `switch` | Toggle the Timer mode (ON/OFF). |
| **Timer Mode** | Select | `select` | Choose between Countdown Timer or Stopwatch. |
| **Timer Duration** | Number| `number` | Set countdown duration (seconds). |
| **Timer Auto-Repeat** | Switch | `switch` | Automatically restart timer when finished. |
| **Timer Alert Buzzer** | Switch | `switch` | Enable buzzer alert when timer finishes. |
| **Timer Alert Beep Count** | Number | `number` | Number of beeps when timer finishes (1-20). |
| **Timer Alert Sound** | Select | `select` | Choose musical chirp pattern for alert. |
| **Timer Brightness** | Number | `number` | Slider (0-15) for Timer display brightness. |

### Weather Controls (ESP32 Only)
These entities control the OpenWeatherMap Weather feature.

| Entity Name | Type | Domain | Description |
| :--- | :--- | :--- | :--- |
| **Weather Enable** | Switch | `switch` | Toggle the Weather display mode (ON/OFF). |

> [!NOTE]
> **Messaging vs. Clock Controls**: "Brightness" controls the message intensity, while "Clock Brightness" controls the time display. Changes to message parameters apply to the *next* message sent. Clock changes apply immediately.

### Actions (Buttons)
Trigger specific actions on the device.

| Entity Name | Domain | Description |
| :--- | :--- | :--- |
| **Send Message** | `button` | Sends the currently queued message (from the *Message* input) along with all current parameters (Brightness, Scroll Delay, etc.) to the display. |
| **Clear Message** | `button` | Clears the current message immediately. |
| **Reboot** | `button` | Reboots the ESP device. |
| **Timer Start** | `button` | Start or resume the timer/stopwatch. |
| **Timer Pause** | `button` | Pause the running timer/stopwatch. |
| **Timer Stop** | `button` | Stop the timer/stopwatch (exit timer mode). |
| **Timer Reset** | `button` | Reset timer to initial duration or stopwatch to zero. |

### Sensors (Telemetry)
Read-only sensors providing device status.

| Entity Name | Domain | Description | Notes |
| :--- | :--- | :--- | :--- |
| **Message Displayed** | `sensor` | The message currently scrolling on the board. | *See Note 1* |
| **Time** | `sensor` | Current device time. | |
| **WiFi SSID** | `sensor` | Connected WiFi Network Name. | *Disabled by default* |
| **WiFi Signal** | `sensor` | Signal strength (RSSI) in dBm. | *Disabled by default* |
| **IP Address** | `sensor` | Device Local IP Address. | *Disabled by default* |
| **Hostname** | `sensor` | Device Hostname (e.g., `RDA-MSG-123456`). | *Disabled by default* |
| **Firmware Version** | `sensor` | Current firmware version string. | |
| **Uptime** | `sensor` | Device uptime (numeric seconds). | *Disabled by default* |
| **Uptime Text** | `sensor` | Device uptime (friendly text). | *Disabled by default* |
| **Repeat Countdown** | `sensor` | Remaining repeats for current message. | *See Note 2* |
| **NTP Synced** | `binary_sensor` | Connectivity status to time server. | *Disabled by default* |
| **Display Active** | `binary_sensor` | Whether clock/transition is currently active. | *Disabled by default* |
| **TLS Status** | `binary_sensor` | Secure connection status. | *ESP32 only* |
| **Display Mode** | `sensor` | Current internal mode (CLOCK, MESSAGE, etc). | *Disabled by default* |
| **Timer State** | `sensor` | Current timer status (running, paused, stopped, completed). | |
| **Timer Display** | `sensor` | Formatted time string (HH:MM:SS or MM:SS). | |
| **Timer Remaining** | `sensor` | Seconds remaining in countdown. | |
| **Timer Elapsed** | `sensor` | Seconds elapsed since start. | |
| **Sleep Mode Status** | `binary_sensor` | Whether sleep mode is currently active (blackout). | *Active/Inactive* |
| **Weather Temperature** | `sensor` | Current temperature from OpenWeatherMap. | *ESP32 Only* |
| **Weather Condition** | `sensor` | Current weather condition description. | *ESP32 Only* |
| **Weather Location** | `sensor` | Current city/configured location. | *ESP32 Only, Disabled by default* |

> [!NOTE]
> **Disabled Sensors**: To reduce database growth and UI clutter, most telemetry sensors (WiFi, Uptime, IP, Hostname, Diagnostics) are disabled by default. You can enable them in Home Assistant by going to **Settings > Devices & Services > MQTT > [Device Name]**, clicking on the sensor, and adding it to your dashboard or toggling "Enable".

### Note 1: Long Message Handling
Home Assistant has a strict **255-character limit** for sensor states.
-   If a message is **shorter than 255 characters**, it is shown fully in the *Message Displayed* sensor state.
-   If a message is **longer than 255 characters**:
    -   The sensor state (icon/badge) shows the first 250 characters followed by `...`.
    -   The **full message** is available in the sensor's attributes. You can access it in templates via `state_attr('sensor.device_name_msg_displayed', 'full_message')`.

### Note 2: Repeat Countdown States
The **Repeat Countdown** sensor indicates how many times the current message will loop before stopping.
-   **Positive Number (e.g., 3, 2, 1)**: The number of loops remaining. The counters decreases as the message scrolls.
-   **0**: The display is empty/idle (or the message has finished scrolling).
-   **-1**: The message is set to **Infinite Repeat**. It will scroll forever until you Send a new message or click **Clear Message**.

## Technical Details

### Topic Structure
The integration follows the standard Home Assistant discovery structure:
-   **Discovery Prefix**: `homeassistant` (standard default)
-   **Device Topic**: `rdadotmatrix/generic/RDA-MSG-XXXXXX` (configurable)

### Discovery Topics
Configuration payloads are published to:
`homeassistant/<component>/<device_id>/<entity_id>/config`

### State & Command Topics
-   **State**: `<device_topic>/ha/<entity>/state`
-   **Command**: `<device_topic>/ha/<entity>/set` (or `/command` for buttons)
-   **Attributes**: `<device_topic>/ha/<entity>/attrs` (for large payloads like full messages)

### Attributes Handling
To bypass the 255-character state limit, the firmware allocates a large JSON buffer on the heap to publish the full message text to a dedicated attributes topic. This ensures stability (preventing stack overflows) while maintaining compatibility with Home Assistant's architecture.
