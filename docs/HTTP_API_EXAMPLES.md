# Comprehensive HTTP API Testing Guide

## Environment Setup

```bash
# Set your device IP and credentials
DEVICE_IP="192.168.1.100"
USER="admin"
PASS="msgboard"
```

## Message API Tests (Existing Functionality)

### GET /arg Endpoint - URL Parameters

**1. Basic Message:**
```bash
curl --user admin:msgboard -X GET -G "http://${DEVICE_IP}/arg" \
  --data-urlencode "MSG=Hello World!"
```

**2. Full Parameters:**
```bash
curl --user admin:msgboard -X GET -G "http://${DEVICE_IP}/arg" \
  --data-urlencode "MSG=Testing all parameters!" \
  --data-urlencode "REP=5" \
  --data-urlencode "BUZ=3" \
  --data-urlencode "DEL=30" \
  --data-urlencode "BRI=10" \
  --data-urlencode "ASC=1" \
  --data-urlencode "ALERTCHIRP=Gentle Dawn"
```

**3. Infinite Repeat (REP=0):**
```bash
curl --user admin:msgboard -X GET -G "http://${DEVICE_IP}/arg" \
  --data-urlencode "MSG=Scrolling forever!" \
  --data-urlencode "REP=0"
```

**4. Silent Message (no buzzer):**
```bash
curl --user admin:msgboard -X GET -G "http://${DEVICE_IP}/arg" \
  --data-urlencode "MSG=Silent message" \
  --data-urlencode "BUZ=0"
```

**5. Fast Scroll:**
```bash
curl --user admin:msgboard -X GET -G "http://${DEVICE_IP}/arg" \
  --data-urlencode "MSG=Speed test!" \
  --data-urlencode "DEL=10"
```

**6. Dim Display:**
```bash
curl --user admin:msgboard -X GET -G "http://${DEVICE_IP}/arg" \
  --data-urlencode "MSG=Dim mode" \
  --data-urlencode "BRI=0"
```

**7. Bright Display:**
```bash
curl --user admin:msgboard -X GET -G "http://${DEVICE_IP}/arg" \
  --data-urlencode "MSG=Max brightness!" \
  --data-urlencode "BRI=15"
```

**8. Stop/Clear Current Message:**
```bash
curl --user admin:msgboard -X GET "http://${DEVICE_IP}/arg"
```

**9. UTF-8 Special Characters:**
```bash
curl --user admin:msgboard -X GET -G "http://${DEVICE_IP}/arg" \
  --data-urlencode "MSG=Símbolos: €£¥©®™°±" \
  --data-urlencode "ASC=1"
```

**10. Custom Alert Chirp:**
```bash
curl --user admin:msgboard -X GET -G "http://${DEVICE_IP}/arg" \
  --data-urlencode "MSG=Custom sound!" \
  --data-urlencode "ALERTCHIRP=Doorbell"
```

**11. Stop/Clear Current Message:**
```bash
curl --user admin:msgboard -X GET "http://${DEVICE_IP}/arg"
```

**12. UTF-8 Special Characters (with custom chirp):**
```bash
curl --user admin:msgboard -X GET -G "http://${DEVICE_IP}/arg" \
  --data-urlencode "MSG=Símbolos: €£¥©®™°±" \
  --data-urlencode "ASC=1" \
  --data-urlencode "ALERTCHIRP=Cheerful"
```

### POST /api Endpoint - JSON Messages

**13. Basic JSON Message:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api" \
  -H "Content-Type: application/json" \
  -d '{"MSG":"JSON test message"}'
```

**14. Full JSON Parameters:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api" \
  -H "Content-Type: application/json" \
  -d '{
    "MSG": "Complete JSON test",
    "REP": 3,
    "BUZ": 5,
    "DEL": 25,
    "BRI": 12,
    "ASC": 1,
    "ALERTCHIRP": "Gentle Dawn"
  }'
```

**15. Minimal JSON (defaults):**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api" \
  -H "Content-Type: application/json" \
  -d '{"MSG":"Quick alert"}'
```

**16. General Settings Control (POST):**
```bash
# Toggle Global Buzzer
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/savegeneral" \
  -d "BuzzerEnable=off"

# Toggle Brightness Override
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/savegeneral" \
  -d "BrightnessOverrideEnable=on" \
  -d "BrightnessOverrideValue=12"
```

**17. JSON Stop Message:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api" \
  -H "Content-Type: application/json" \
  -d '{"MSG":""}'
```

**18. JSON with Special Characters:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api" \
  -H "Content-Type: application/json" \
  -d '{"MSG":"Quote test: \"Hello\" and backslash: \\test","REP":2}'
```

**19. JSON with Custom Chirp:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api" \
  -H "Content-Type: application/json" \
  -d '{"MSG":"Doorbell alert!","ALERTCHIRP":"Doorbell"}'
```

### Defaults Management

**20a. Set Custom Default:**
*Types: REP (Repeat), BUZ (Buzzer), DEL (Delay), BRI (Brightness), ALERTCHIRP (Alert Chirp)*
```bash
curl --user admin:msgboard -X GET -G "http://${DEVICE_IP}/setdefault" \
  --data-urlencode "type=REP" \
  --data-urlencode "value=5"
```

**20b. Set Custom Alert Chirp Default:**
```bash
curl --user admin:msgboard -X GET -G "http://${DEVICE_IP}/setdefault" \
  --data-urlencode "type=ALERTCHIRP" \
  --data-urlencode "value=Gentle Dawn"
```

**20c. Reset All Defaults:**
```bash
curl --user admin:msgboard -X GET "http://${DEVICE_IP}/resetdefaults"
```

## Timer API Tests (New Functionality)

### POST /api/timer Endpoint - Timer Configuration & Control

**21a. Enable Timer (Countdown):**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/timer" \
  -H "Content-Type: application/json" \
  -d '{
    "enabled": true,
    "mode": "TIMER",
    "durationSeconds": 300,
    "brightness": 10
  }'
```

**21b. Enable Stopwatch:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/timer" \
  -H "Content-Type: application/json" \
  -d '{
    "enabled": true,
    "mode": "STOPWATCH",
    "brightness": 8
  }'
```

**21c. Start Timer/Stopwatch:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/timer/start" -d ''
```

**21d. Pause Timer/Stopwatch:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/timer/pause" -d ''
```

**21e. Stop/Reset Timer:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/timer/reset" -d ''
```

**21f. Configure Timer Defaults (Auto-Repeat & Alerts):**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/timer" \
  -H "Content-Type: application/json" \
  -d '{
    "enabled": true,
    "autoRepeat": true,
    "alertBuzzer": true,
    "alertBuzzerCount": 5
  }'
```

## Clock API Tests (New Functionality)

### POST /api/clock Endpoint - Clock Configuration

**22. Enable Clock with Brightness:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{
    "enabled": true,
    "brightness": 10
  }'
```

**23. Disable Clock:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"enabled": false}'
```

**24. Set Brightness Only:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"brightness": 5}'
```

**25. Change NTP Server:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"ntpServer": "pool.ntp.org"}'
```

**26. Set Timezone (POSIX String):**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"tzString": "EST5EDT,M3.2.0,M11.1.0"}'
```

**27. Set Timezone (UTC):**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"tzString": "UTC0"}'
```

**28. Set Transition Effect:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"transitionEffect": "FADE"}'
```

**29. Test All Transition Effects:**

**FADE**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"transitionEffect": "FADE"}'
```

**WIPE**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"transitionEffect": "WIPE"}'
```

**DISSOLVE**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"transitionEffect": "DISSOLVE"}'
```

**BLINDS**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"transitionEffect": "BLINDS"}'
```

**GROW**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"transitionEffect": "GROW"}'
```

**SCROLL**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"transitionEffect": "SCROLL"}'
```

**SCAN**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"transitionEffect": "SCAN"}'
```

**OPENING**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"transitionEffect": "OPENING"}'
```

**CLOSING**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"transitionEffect": "CLOSING"}'
```

**PRINT**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"transitionEffect": "PRINT"}'
```

**30. Set Transition Delay (speed):**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"transitionDelayMs": 500}'
```

**31. Enable Random Transitions:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"randomizeTransition": true}'
```

**32. Disable Random Transitions:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"randomizeTransition": false}'
```

**33. Set NTP Resync Interval:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"resyncIntervalHours": 12}'
```

**34. Complete Clock Setup (all parameters):**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{
    "enabled": true,
    "brightness": 8,
    "ntpServer": "time.google.com",
    "tzString": "EST5EDT,M3.2.0,M11.1.0",
    "transitionDelayMs": 300,
    "transitionEffect": "FADE",
    "randomizeTransition": false,
    "resyncIntervalHours": 6
  }'
```

## Validation Error Tests (Should Return 400)

**35. Invalid Brightness (too high):**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"brightness": 99}'
```

**36. Invalid GMT Offset:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"gmtOffsetSeconds": 999999}'
```

**37. Invalid Transition Effect:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"transitionEffect": "INVALID"}'
```

**38. Invalid DST Offset:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"dstOffsetSeconds": 1800}'
```

**39. Invalid Resync Interval:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"resyncIntervalHours": 0}'
```

**40. Invalid Transition Delay:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"transitionDelayMs": 99999}'
```

## Configuration Management Tests

**41. Export Configuration:**
```bash
curl --user admin:msgboard "http://${DEVICE_IP}/exportconfig" > config_backup.json
```

**42. Import Configuration:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/importconfig" \
  -H "Content-Type: application/json" \
  -d @config_backup.json
```

## Boolean Format Tests (Clock API)

The `parseBoolean()` helper accepts multiple formats:

**43. Enable with string "on":**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"enabled": "on"}'
```

**44. Enable with string "true":**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"enabled": "true"}'
```

**45. Enable with integer 1:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"enabled": 1}'
```

**46. Randomize with string "off":**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"randomizeTransition": "off"}'
```

## Common Use Case Scenarios

**47. Morning Routine - Enable clock at 6 AM:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"enabled": true, "brightness": 5}'
```

**48. Evening Routine - Show message before bed:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api" \
  -H "Content-Type: application/json" \
  -d '{"MSG":"Good night!","REP":2,"BRI":3}'
```

**49. Doorbell Alert:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api" \
  -H "Content-Type: application/json" \
  -d '{"MSG":"Visitor at door!","REP":3,"BUZ":10,"BRI":15,"ALERTCHIRP":"Doorbell"}'
```

**50. Weather Update:**
```bash
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api" \
  -H "Content-Type: application/json" \
  -d '{"MSG":"Today: Sunny 75°F","REP":5,"DEL":40}'
```

**51. Switch from Clock to Message:**
```bash
# Disable clock
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"enabled": false}'

# Show message
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api" \
  -H "Content-Type: application/json" \
  -d '{"MSG":"Important notification!"}'
```

**52. Switch from Message back to Clock:**
```bash
# Clear message
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api" \
  -H "Content-Type: application/json" \
  -d '{"MSG":""}'

# Enable clock
curl --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"enabled": true}'
```

## Quick Test Script (Linux/Mac/Git Bash)

Save this as `test_api.sh`:

```bash
#!/bin/bash
DEVICE_IP="192.168.1.100"

echo "Testing Message API..."
curl -s --user admin:msgboard -X POST "http://${DEVICE_IP}/api" \
  -H "Content-Type: application/json" \
  -d '{"MSG":"API Test 1/3"}'
sleep 3

echo "Testing Clock Enable..."
curl -s --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"enabled":true,"brightness":10}'
sleep 5

echo "Testing Clock Disable..."
curl -s --user admin:msgboard -X POST "http://${DEVICE_IP}/api/clock" \
  -H "Content-Type: application/json" \
  -d '{"enabled":false}'
sleep 2

echo "Done!"
```

## Expected Responses

**Success (200 OK):**
```json
{"status":"ok"}
```

**Authentication Error (401):**
```text
HTTP Basic: Access denied.
```

**Validation Error (400):**
```json
{"error":"Brightness must be 0-15"}
{"error":"Invalid transition effect"}
{"error":"GMT offset out of range"}
```

**Invalid JSON (400):**
```json
{"error":"Invalid JSON"}
```

This comprehensive guide covers all existing message API functionality plus the clock API features implemented!
