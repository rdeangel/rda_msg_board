# Comprehensive MQTT Publishing & Testing Guide

## Environment Setup

```bash
# Set your device hostname (found on device label or web config)
# Default format: RDA-MSG-XXXXXX
DEVICE_TOPIC_BASE="RDA-MSG-123456"

# Set your MQTT Broker details
MQTT_BROKER="192.168.1.10"
MQTT_PORT="1883" # Use 8883 for TLS
MQTT_USER="mqtt_user"
MQTT_PASS="mqtt_pass"

# For TLS testing (ESP32 only):
# MQTT_PORT="8883"
# MQTT_CA="--cafile /path/to/ca.crt" # If using CA validation

# Alias for mosquitto_pub to save typing
alias mpub="mosquitto_pub -h $MQTT_BROKER -p $MQTT_PORT -u $MQTT_USER -P $MQTT_PASS"
```

## 1. Message API (Legacy & JSON)

The device supports two primary ways to display messages via MQTT: Plain Text and JSON.

### A. Plain Text Messages
*Subscribes to:* `<DEVICE_HOSTNAME>`, `rdadotmatrix/generic` (or configured prefix)
*Payload:* The raw text message to display. Uses default settings for speed, brightness, etc.

**1. Send Simple Message:**
```bash
mpub -t "${DEVICE_TOPIC_BASE}" -m "Hello World!"
```

**2. Send Global Message (Broadcast):**
```bash
# Sends to all devices subscribed to the prefix
mpub -t "rdadotmatrix/generic" -m "Broadcast to all units!"
```

### B. JSON Messages
*Subscribes to:* `<DEVICE_HOSTNAME>/json`, `rdadotmatrix/json`, `rdadotmatrix/generic/json`
*Payload:* JSON object with message parameters.

**3. Basic JSON Message:**
```bash
mpub -t "${DEVICE_TOPIC_BASE}/json" -m '{"MSG":"JSON Test"}'
```

**4. Full Parameter Control:**
*Keys available:*
- `MSG`: Message Text
- `REP`: Repeat Count (0=infinite, >0=finite)
- `BUZ`: Buzzer Beeps (count)
- `DEL`: Scroll Delay (ms, lower=faster)
- `BRI`: Brightness (0-15)
- `ASC`: ASCII Conversion (1=on, 0=off)
- `FORCEREP`: Force Repetitions — block new messages until done (`true`/`false`, default `false`)

```bash
mpub -t "${DEVICE_TOPIC_BASE}/json" -m '{
  "MSG": "Full Control Message",
  "REP": 5,
  "BUZ": 3,
  "DEL": 30,
  "BRI": 15,
  "ASC": 1
}'
```

**4b. Force Repetitions — priority message that cannot be interrupted:**
```bash
mpub -t "${DEVICE_TOPIC_BASE}/json" -m '{
  "MSG": "Priority alert - do not interrupt!",
  "REP": 5,
  "BUZ": 3,
  "FORCEREP": true
}'
```
*Any MQTT or HTTP message received while this is displaying is silently dropped and logged to serial. The lock releases automatically when all repetitions complete.*

**5. Infinite Scroll:**
```bash
mpub -t "${DEVICE_TOPIC_BASE}/json" -m '{"MSG":"Scrolling forever...","REP":0}'
```

**6. Stop/Clear Message:**
```bash
mpub -t "${DEVICE_TOPIC_BASE}/json" -m '{"MSG":""}'
```

---

## 2. Clock Control API (Home Assistant Style)

The clock and system settings are controlled via specific topics designed for Home Assistant discovery. You can publish to these topics manually to control the device functions.

**Base Topic Pattern:** `<DEVICE_HOSTNAME>/ha/<ENTITY>/set`

### A. Clock Power & Display

**7. Enable Clock:**
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/clock_enable/set" -m "ON"
```

**8. Disable Clock:**
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/clock_enable/set" -m "OFF"
```

**9. Set Clock Brightness:**
*Payload:* `0` to `15`
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/clock_brightness/set" -m "10"
```

### B. Transition Effects

**10. Set Transition Effect:**
*Options:* `FADE`, `DISSOLVE`, `BLINDS`, `WIPE`, `GROW`, `SCROLL`, `SCAN`, `OPENING`, `CLOSING`, `PRINT`
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/clock_transition_effect/set" -m "WIPE"
```

**11. Enable Random Transitions:**
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/clock_randomize/set" -m "ON"
```

**12. Disable Random Transitions:**
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/clock_randomize/set" -m "OFF"
```

**13. Set Transition Speed (Delay):**
*Payload:* Milliseconds (e.g., 500)
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/clock_transition_delay/set" -m "500"
```

### C. Time Configuration

**14. Set NTP Server:**
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/clock_ntp_server/set" -m "pool.ntp.org"
```

**15. Set Timezone (POSIX String):**
*Example: US Eastern Time (EST5EDT)*
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/clock_custom_tz/set" -m "EST5EDT,M3.2.0,M11.1.0"
```

**16. Set Timezone (UTC):**
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/clock_custom_tz/set" -m "UTC0"
```

**17. Set Cloud Resync Interval:**
*Payload:* Hours (1-24)
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/clock_resync_interval/set" -m "12"
```

---

## 3. Timer Control API (Home Assistant Style)

The timer/stopwatch is controlled via specific MQTT topics.

**Base Topic Pattern:** `<DEVICE_HOSTNAME>/ha/<ENTITY>/set` or `<DEVICE_HOSTNAME>/ha/<ENTITY>/command`

### A. Timer Configuration

**18. Enable Timer Mode:**
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/timer_enable/set" -m "ON"
mpub -t "${DEVICE_TOPIC_BASE}/ha/timer_mode/set" -m "TIMER"  # or STOPWATCH
```

**19. Set Duration:**
*Payload:* HH:MM:SS string
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/timer_duration/set" -m "00:05:00"
```

**20. Configure Options:**
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/timer_auto_repeat/set" -m "ON"
mpub -t "${DEVICE_TOPIC_BASE}/ha/timer_alert_buzzer/set" -m "ON"
mpub -t "${DEVICE_TOPIC_BASE}/ha/timer_brightness/set" -m "10"
```

**20b. Select Alert Sound (Chirp):**
```bash
# Options: "Gentle Dawn", "Cheerful", "Urgent", "Beep", "Victory", "For Elise", "Mario Bros", etc.
mpub -t "${DEVICE_TOPIC_BASE}/ha/timer_alert_chirp/set" -m "Mario Bros"
```

### B. Timer Actions

**21. Start/Pause/Reset:**
```bash
# Start
mpub -t "${DEVICE_TOPIC_BASE}/ha/timer_start/command" -m "PRESS"

# Stop (Pause)
mpub -t "${DEVICE_TOPIC_BASE}/ha/timer_stop/command" -m "PRESS"

# Reset
mpub -t "${DEVICE_TOPIC_BASE}/ha/timer_reset/command" -m "PRESS"
```

---

## 4. System Commands (Home Assistant Style)

**22. Reboot Device:**
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/reboot/command" -m "PRESS"
```

**23. Clear Current Message:**
*Clears message and returns to clock (if enabled)*
```bash
mpub -t "${DEVICE_TOPIC_BASE}/ha/clear/command" -m "PRESS"
```

**24. Send Message via Split Controls:**
*Wait! This requires setting components first. Use JSON for atomic updates.*
*Steps:*
1. Set params: Rep, Buzzer, etc.
2. Set Message Content.
3. Trigger 'Send'.

```bash
# Step 1: Configure
mpub -t "${DEVICE_TOPIC_BASE}/ha/repeat/set" -m "3"
mpub -t "${DEVICE_TOPIC_BASE}/ha/buzzer/set" -m "2"

# Step 2: Set Message Content
mpub -t "${DEVICE_TOPIC_BASE}/ha/message/set" -m "Queued Message"

# Step 3: Trigger Display
mpub -t "${DEVICE_TOPIC_BASE}/ha/send/command" -m "PRESS"
```

**25. Toggle Global Buzzer:**
```bash
# Enable
mpub -t "${DEVICE_TOPIC_BASE}/ha/buzzer_enable/set" -m "ON"

# Disable
mpub -t "${DEVICE_TOPIC_BASE}/ha/buzzer_enable/set" -m "OFF"
```

**26. Toggle MQTT Messages Display:**
*Prevents new MQTT messages from being displayed (useful for maintenance or "Do Not Disturb")*
```bash
# Enable (Default)
mpub -t "${DEVICE_TOPIC_BASE}/ha/mqtt_messages/set" -m "ON"

# Disable
mpub -t "${DEVICE_TOPIC_BASE}/ha/mqtt_messages/set" -m "OFF"
```

**26b. Force Repetitions Switch:**
*Stages the FORCEREP flag in HA RAM. Applied when the Send button/command is triggered.*
```bash
# Enable — next HA Send will lock the display until all repeats finish
mpub -t "${DEVICE_TOPIC_BASE}/ha/force_rep/set" -m "ON"

# Disable (default)
mpub -t "${DEVICE_TOPIC_BASE}/ha/force_rep/set" -m "OFF"
```
*Note: This only affects messages sent via the HA "Send" command (`ha/send/command`). Messages sent directly via JSON topics carry their own `FORCEREP` flag and are not affected by this switch.*

**27. Global Brightness Override:**
```bash
# Enable override
mpub -t "${DEVICE_TOPIC_BASE}/ha/brightness_override/set" -m "ON"

# Set override value (0-15)
mpub -t "${DEVICE_TOPIC_BASE}/ha/brightness_override_value/set" -m "15"
```

**28. Sleep Mode (Scheduled Blackout):**
```bash
# Enable sleep mode
mpub -t "${DEVICE_TOPIC_BASE}/ha/sleep_mode_enable/set" -m "ON"

# Set times (HH:MM)
mpub -t "${DEVICE_TOPIC_BASE}/ha/sleep_mode_on_time/set" -m "23:00"
mpub -t "${DEVICE_TOPIC_BASE}/ha/sleep_mode_off_time/set" -m "07:30"

# Set alternate weekend times
mpub -t "${DEVICE_TOPIC_BASE}/ha/sleep_mode_weekend_enable/set" -m "ON"
mpub -t "${DEVICE_TOPIC_BASE}/ha/sleep_mode_weekend_on_time/set" -m "00:00"
mpub -t "${DEVICE_TOPIC_BASE}/ha/sleep_mode_weekend_off_time/set" -m "08:30"
```

---

## 5. Automation Helper Script (Bash)

Save as `test_mqtt_clock.sh`:

```bash
#!/bin/bash
# Usage: ./test_mqtt_clock.sh <DEVICE_HOSTNAME> <BROKER_IP>

DEVICE=$1
BROKER=$2

if [ -z "$DEVICE" ]; then
    echo "Usage: $0 <DEVICE_HOSTNAME> <BROKER_IP>"
    echo "Example: $0 RDA-MSG-A1B2C3 192.168.1.50"
    exit 1
fi

echo "Testing Clock Features on $DEVICE..."

# 1. Enable Clock
mosquitto_pub -h $BROKER -t "$DEVICE/ha/clock_enable/set" -m "ON"
echo "Enable: ON"
sleep 1

# 2. Cycle Effects
EFFECTS=("WIPE" "SCROLL" "OPENING" "CLOSING" "SCAN")
for EFFECT in "${EFFECTS[@]}"; do
    echo "Effect: $EFFECT"
    mosquitto_pub -h $BROKER -t "$DEVICE/ha/clock_transition_effect/set" -m "$EFFECT"
    # Force a refresh by toggling display mode implicitly or just wait
    sleep 3
done

# 3. Test Random mode
echo "Randomize: ON"
mosquitto_pub -h $BROKER -t "$DEVICE/ha/clock_randomize/set" -m "ON"
sleep 5

# 4. Show a message interruption
echo "Interrupting with Message..."
mosquitto_pub -h $BROKER -t "$DEVICE/json" -m '{"MSG":"Interruption!","REP":2}'
sleep 10 # Wait for message to finish (2 repeats) and clock to return

echo "Test Complete."
```
