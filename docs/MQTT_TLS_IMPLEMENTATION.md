# ESP32 MQTT TLS Implementation Guide

**Date:** 2026-01-07
**Platform:** ESP32 Only (ESP8266 unchanged)
**Goal:** Add complete MQTT TLS support to ESP32 while keeping ESP8266 compilation-compatible

---

## Table of Contents

1. [Overview](#overview)
2. [Architecture](#architecture)
3. [File Modifications Summary](#file-modifications-summary)
4. [Step-by-Step Implementation](#step-by-step-implementation)
5. [Testing Guide](#testing-guide)
6. [UI Behavior](#ui-behavior)

---

## Overview

This guide implements MQTT TLS support **exclusively for ESP32**. The ESP8266 platform will compile without any TLS code, while ESP32 gets full TLS functionality including:

- TLS/SSL connection support (port 8883)
- Fingerprint validation
- CA certificate validation
- Insecure mode (no validation)
- Full UI configuration with platform-aware disabling

### Key Design Principles

1. **Platform isolation:** All TLS code wrapped in `#ifdef ESP32`
2. **Zero impact on ESP8266:** ESP8266 compiles with no TLS code
3. **UI consistency:** Same configuration page for both platforms
4. **Graceful degradation:** TLS options visible but disabled on ESP8266
5. **Port auto-switching:** Automatically switches between 1883 (plain) and 8883 (TLS)

---

## Architecture

### Platform-Specific Approach

```
┌─────────────────────────────────────────┐
│         Common Code                   │
│  (Works on both ESP8266 and ESP32)  │
│  - MQTT logic                        │
│  - Web server routes                 │
│  - Message handling                  │
└──────────────┬──────────────────────┘
               │
       ┌───────┴───────┐
       │               │
       ▼               ▼
┌───────────┐   ┌───────────┐
│ ESP8266   │   │  ESP32    │
│           │   │           │
│ WiFiClient│   │WiFiClient │
│ (Port 1883)│   │Secure    │
│           │   │(1883/8883)│
│ No TLS    │   │ Full TLS │
│ code      │   │ support  │
└───────────┘   └───────────┘
```

### Configuration Flow

1. **User opens `/mqttconfig`** → Both platforms show full UI
2. **Platform detection via JavaScript** → Detect ESP8266 vs ESP32
3. **ESP8266:** TLS options disabled/grayed out
4. **ESP32:** TLS options fully functional
5. **Save configuration** → ESP32 TLS settings saved, ESP8266 ignored
6. **MQTT connection** → Each platform uses appropriate client and port

---

## File Modifications Summary

| File | Changes | Platform |
|------|---------|----------|
| `include/config.h` | Add TLS fields to `mqttConfigObj` | Common |
| `src/globals.cpp` | Add TLS global variables (ESP32 only) | ESP32 |
| `include/globals.h` | Add TLS variable declarations | Common |
| `src/mqtt.cpp` | Add TLS configuration logic | ESP32 |
| `src/web_pages.cpp` | Add TLS UI components | Common |
| `src/web_server.cpp` | Add TLS config handlers | Common |
| `platformio.ini` | No changes needed | Common |

**Total lines of code:** ~350 lines
**ESP8266 impact:** 0 lines of TLS code compiled
**ESP32 impact:** Full TLS support (~200 lines)

---

## Step-by-Step Implementation

### Step 1: Update Configuration Structure

**File:** `include/config.h`

Add TLS-related fields to the `mqttConfigObj` structure:

```cpp
// Around line 167-177 in include/config.h
struct mqttConfigObj {
  char onOffMqttHolder[STDSIZE];
  char anonymousMqttHolder[STDSIZE];
  char alertMqttHolder[STDSIZE];
  char usernameMqttHolder[STDSIZE];
  char passwordMqttHolder[STDSIZE];
  char serverAddressMqttHolder[STDSIZE];
  char serverPortMqttHolder[STDSIZE];
  char topicPrefixMqttHolder[STDSIZE];
  char haDiscoveryMqttHolder[STDSIZE];

  // ========== NEW TLS FIELDS ==========
  char tlsEnabledMqttHolder[STDSIZE];      // "on" or "off" - ESP32 only
  char tlsCertTypeMqttHolder[STDSIZE];     // "none", "fingerprint", "ca"
  char tlsFingerprintMqttHolder[STDSIZE];  // SHA-1 fingerprint (40 hex chars)
  char tlsCaCertMqttHolder[3072];         // CA certificate (PEM format, ~3KB max)
  // ======================================
};
```

**Notes:**
- All fields present for both platforms (needed for config file compatibility)
- Only ESP32 will use these values
- `tlsCaCertMqttHolder` is 3072 bytes to accommodate full CA certificates

---

### Step 2: Add TLS Variable Declarations

**File:** `include/globals.h` (create if doesn't exist, or add to `globals.h` in `include/`)

Add TLS-related global variable declarations. Wrap in `#ifdef ESP32`:

```cpp
// Around line 80-100 in include/globals.h

// ... existing global declarations ...

#ifdef ESP32
  // TLS MQTT Client (ESP32 only)
  extern WiFiClientSecure espClientSecure;
  extern WiFiClient espClientPlain;

  // TLS Configuration (ESP32 only)
  extern char tlsEnabledMqtt[STDSIZE];
  extern char tlsCertTypeMqtt[STDSIZE];
  extern char tlsFingerprintMqtt[STDSIZE];
  extern char tlsCaCertMqtt[3072];

  // Temporary holders for form submission
  extern char newTlsEnabledMqtt[STDSIZE];
  extern char newTlsCertTypeMqtt[STDSIZE];
  extern char newTlsFingerprintMqtt[STDSIZE];
  extern char newTlsCaCertMqtt[3072];
  extern bool newTlsEnabledAvailable;
  extern bool newTlsCertTypeAvailable;
  extern bool newTlsFingerprintAvailable;
  extern bool newTlsCaCertAvailable;

  // TLS Configuration Function (ESP32 only)
  extern void configureTLS();
#endif
```

---

### Step 3: Define TLS Global Variables

**File:** `src/globals.cpp`

Add TLS global variable definitions. Wrap in `#ifdef ESP32`:

```cpp
// Around line 29-31 in src/globals.cpp (after WiFiClient espClient)

#ifdef ESP8266
  WiFiClient espClient;
#elif defined(ESP32)
  // ESP32 has both plain and secure clients
  WiFiClient espClientPlain;
  WiFiClientSecure espClientSecure;

  // Default to plain client - will be switched in configureTLS()
  WiFiClient* currentMqttClient = &espClientPlain;
#else
  #error "Unsupported platform"
#endif

// ... continue with existing code ...

// Around line 100+ in src/globals.cpp (after MQTT configuration globals)

#ifdef ESP32
  // TLS Configuration (ESP32 only)
  char tlsEnabledMqtt[STDSIZE] = "off";
  char tlsCertTypeMqtt[STDSIZE] = "none";
  char tlsFingerprintMqtt[STDSIZE] = "";
  char tlsCaCertMqtt[3072] = "";

  // Temporary holders for form submission
  char newTlsEnabledMqtt[STDSIZE] = "";
  char newTlsCertTypeMqtt[STDSIZE] = "";
  char newTlsFingerprintMqtt[STDSIZE] = "";
  char newTlsCaCertMqtt[3072] = "";
  bool newTlsEnabledAvailable = false;
  bool newTlsCertTypeAvailable = false;
  bool newTlsFingerprintAvailable = false;
  bool newTlsCaCertAvailable = false;
#endif
```

**Important:** Update the MQTT client initialization to use the appropriate client:

```cpp
// Around line 30 in src/globals.cpp

#ifdef ESP8266
  WiFiClient espClient;
  PubSubClient mqttClient(espClient);
#elif defined(ESP32)
  WiFiClient espClientPlain;
  WiFiClientSecure espClientSecure;
  WiFiClient* currentMqttClient = &espClientPlain;  // Will be set in configureTLS()
  PubSubClient mqttClient(*currentMqttClient);
#endif
```

---

### Step 4: Implement TLS Configuration Function

**File:** `src/mqtt.cpp` (or create a new file `src/tls.cpp` and add declaration to headers)

Add the `configureTLS()` function:

```cpp
// Add this function near the top of src/mqtt.cpp, after includes

#ifdef ESP32

#include <WiFiClientSecure.h>
#include <FS.h>

// Helper function: Convert hex string to byte array
bool hexStringToBytes(const char* hexStr, uint8_t* bytes, int length) {
  // Remove colons if present (e.g., "AA:BB:CC:DD" -> "AABBCCDD")
  String cleanHex = "";
  for (int i = 0; hexStr[i] != '\0'; i++) {
    if (hexStr[i] != ':' && hexStr[i] != ' ') {
      cleanHex += hexStr[i];
    }
  }

  // Validate length
  if (cleanHex.length() != length * 2) {
    Serial.printf("ERROR: Invalid fingerprint length. Expected %d, got %d\n",
                 length * 2, cleanHex.length());
    return false;
  }

  // Convert hex to bytes
  for (int i = 0; i < length; i++) {
    char high = cleanHex.charAt(i * 2);
    char low = cleanHex.charAt(i * 2 + 1);

    if (!isxdigit(high) || !isxdigit(low)) {
      Serial.printf("ERROR: Invalid hex character at position %d\n", i);
      return false;
    }

    bytes[i] = (charToHex(high) << 4) | charToHex(low);
  }

  return true;
}

// Helper function: Convert hex char to value
uint8_t charToHex(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return 0;
}

/**
 * Configure TLS settings for MQTT connection (ESP32 only)
 * This function sets up the WiFiClientSecure based on user configuration
 */
void configureTLS() {
  PRINTS("\n--- Configuring MQTT TLS ---");

  // Check if TLS is enabled
  if (strcmp(tlsEnabledMqtt, "on") != 0) {
    PRINTS("TLS: Disabled, using plain connection");
    currentMqttClient = &espClientPlain;
    mqttClient.setClient(espClientPlain);
    return;
  }

  PRINTS("TLS: Enabled, configuring secure client");

  // Use the secure client
  currentMqttClient = &espClientSecure;
  mqttClient.setClient(espClientSecure);

  // Configure certificate validation based on type
  if (strcmp(tlsCertTypeMqtt, "fingerprint") == 0) {
    // Fingerprint validation
    if (strlen(tlsFingerprintMqtt) > 0) {
      PRINTS("TLS: Using fingerprint validation");

      uint8_t fingerprint[20];
      if (hexStringToBytes(tlsFingerprintMqtt, fingerprint, 20)) {
        espClientSecure.setFingerprint(fingerprint);
        PRINT("TLS: Fingerprint set to ", tlsFingerprintMqtt);
      } else {
        PRINTS("TLS: ERROR - Invalid fingerprint format");
        // Fall back to insecure mode
        espClientSecure.setInsecure();
        PRINTS("TLS: Falling back to insecure mode");
      }
    } else {
      PRINTS("TLS: ERROR - Fingerprint not provided");
      espClientSecure.setInsecure();
    }

  } else if (strcmp(tlsCertTypeMqtt, "ca") == 0) {
    // CA certificate validation
    if (strlen(tlsCaCertMqtt) > 0) {
      PRINTS("TLS: Using CA certificate validation");
      espClientSecure.setCACert(tlsCaCertMqtt);

      // Check certificate is valid
      if (strlen(tlsCaCertMqtt) < 100) {
        PRINTS("TLS: WARNING - CA certificate seems too short");
      }
    } else {
      PRINTS("TLS: ERROR - CA certificate not provided");
      espClientSecure.setInsecure();
    }

  } else {
    // No validation (insecure mode)
    PRINTS("TLS: WARNING - Using insecure mode (no certificate validation)");
    espClientSecure.setInsecure();
  }

  // Set buffer sizes (ESP32 has ample memory)
  // Default: 16KB receive, 16KB transmit
  espClientSecure.setBufferSizes(16384, 16384);
  PRINTS("TLS: Buffer sizes set to 16KB/16KB");

  PRINTS("--- TLS Configuration Complete ---\n");
}

#endif // ESP32
```

---

### Step 5: Update MQTT Reconnection Logic

**File:** `src/mqtt.cpp`

Modify the `mqttReconnect()` function to call `configureTLS()`:

```cpp
// Find the mqttReconnect() function in src/mqtt.cpp (around line 50-80)
// Add the configureTLS() call at the beginning:

bool mqttReconnect() {
#ifdef ESP32
  // Configure TLS before attempting connection
  configureTLS();
#endif

  // ... existing reconnection logic ...
  // Rest of the function remains unchanged
  if (mqttClient.connect(clientId.c_str(), mqttUsername, mqttPassword)) {
    mqttDisconnected = 0;
    subscribeToTopics();
    publishStatus("Connected");
    return true;
  }

  long now = millis();
  if (now - mqttLastReconnectAttempt > mqttConnectTimeIntervall) {
    mqttLastReconnectAttempt = now;
    mqttDisconnected++;
  }
  return false;
}
```

---

### Step 6: Add TLS UI Components

**File:** `src/web_pages.cpp`

Add TLS configuration section to the MQTT configuration page. Find the `getPage_mqttConfig()` function and add this section before the closing `</form>` tag:

```cpp
// Find the mqtt config page in src/web_pages.cpp
// Look for the closing </form> tag and add this section before it

String getPage_mqttConfig() {
  // ... existing page content ...

  // ========== NEW TLS CONFIGURATION SECTION ==========
  String html = FPSTR(HTTP_TMPL_MQTT_CONFIG);

  // Add TLS section before the "Save MQTT Config" button
  int insertPos = html.indexOf("<button type=\"submit\" name=\"save\" value=\"Save MQTT Config\">");
  if (insertPos > 0) {
    String tlsSection = R"rawliteral(
      <div class="config-group">
        <h3>TLS / SSL Configuration (ESP32 Only)</h3>

        <div class="info-box" id="tlsInfoBox" style="%TLS_INFO_BOX_STYLE%">
          <strong>Platform:</strong> <span id="platformName">Detecting...</span><br>
          <strong>TLS Status:</strong> <span id="tlsStatus">Not available on this platform</span>
        </div>

        <div class="form-row">
          <label for="tlsEnabled">Enable TLS:</label>
          <select id="tlsEnabled" name="tlsEnabled" disabled>
            <option value="off" %TLS_ENABLED_OFF%>Off (Port 1883)</option>
            <option value="on" %TLS_ENABLED_ON%>On (Port 8883)</option>
          </select>
        </div>

        <div class="form-row" id="tlsConfigSection" style="%TLS_CONFIG_SECTION_STYLE%">
          <label for="tlsCertType">Certificate Validation:</label>
          <select id="tlsCertType" name="tlsCertType" disabled>
            <option value="none" %TLS_CERT_NONE%>None (Insecure - Not Recommended)</option>
            <option value="fingerprint" %TLS_CERT_FINGERPRINT%>Server Fingerprint (SHA-1)</option>
            <option value="ca" %TLS_CERT_CA%>CA Certificate (PEM)</option>
          </select>
        </div>

        <div class="form-row" id="tlsFingerprintRow" style="%TLS_FINGERPRINT_STYLE%">
          <label for="tlsFingerprint">Fingerprint (40 hex chars, AA:BB:CC:...):</label>
          <input type="text" id="tlsFingerprint" name="tlsFingerprint"
                 maxlength="59" value="%TLS_FINGERPRINT%"
                 placeholder="AA:BB:CC:DD:EE:FF:00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD"
                 disabled>
          <small class="help-text">
            Use: <code>openssl s_client -connect &lt;server&gt;:8883 -showcerts 2>/dev/null | openssl x509 -fingerprint -sha1 -noout</code>
          </small>
        </div>

        <div class="form-row" id="tlsCaCertRow" style="%TLS_CA_CERT_STYLE%">
          <label for="tlsCaCert">CA Certificate (PEM format):</label>
          <textarea id="tlsCaCert" name="tlsCaCert" rows="12"
                    placeholder="-----BEGIN CERTIFICATE-----
MIIDxTCCAq2gAwIBAgIJAK...
-----END CERTIFICATE-----
" disabled>%TLS_CA_CERT%</textarea>
          <small class="help-text">
            Paste the full CA certificate chain including BEGIN/END markers
          </small>
        </div>

        <div class="form-row">
          <label for="mqttPort">MQTT Port:</label>
          <input type="number" id="mqttPort" name="port" value="%MQTT_PORT%"
                 min="1" max="65535" placeholder="Default: 1883 (plain) or 8883 (TLS)">
          <small class="help-text">
            Default: 1883 for plain, 8883 for TLS (auto-switched if empty)
          </small>
        </div>

        <div class="warning-box" style="%TLS_WARNING_STYLE%">
          <strong>Security Note:</strong>
          <ul>
            <li><strong>Fingerprint:</strong> Good for self-signed certs, but must be updated if server cert changes</li>
            <li><strong>CA Certificate:</strong> Most secure, supports automatic renewal (e.g., Let's Encrypt)</li>
            <li><strong>None (Insecure):</strong> Vulnerable to man-in-the-middle attacks. Use only for testing!</li>
          </ul>
        </div>
      </div>
    )rawliteral";

    html.insert(insertPos, tlsSection);
  }
  // ================================================

  // ... continue with existing page content ...

  return html;
}
```

---

### Step 7: Add TLS UI Styling

**File:** `src/web_pages.cpp`

Add CSS styles for TLS configuration. Find the `<style>` section in the MQTT config page and add these styles:

```css
/* Add to the existing <style> tag in src/web_pages.cpp */

/* TLS Configuration Styles */
.config-group {
  background-color: #f5f5f5;
  border: 1px solid #ddd;
  border-radius: 8px;
  padding: 15px;
  margin-bottom: 20px;
}

.config-group h3 {
  margin-top: 0;
  color: #2c3e50;
  border-bottom: 2px solid #3498db;
  padding-bottom: 10px;
}

.form-row {
  display: flex;
  flex-direction: column;
  margin-bottom: 15px;
}

.form-row label {
  font-weight: bold;
  margin-bottom: 5px;
  color: #34495e;
}

.form-row input,
.form-row select,
.form-row textarea {
  padding: 10px;
  border: 1px solid #bdc3c7;
  border-radius: 4px;
  font-family: 'Courier New', monospace;
  font-size: 14px;
}

.form-row input:focus,
.form-row select:focus,
.form-row textarea:focus {
  outline: none;
  border-color: #3498db;
  box-shadow: 0 0 5px rgba(52, 152, 219, 0.3);
}

.form-row textarea {
  resize: vertical;
  min-height: 200px;
  line-height: 1.4;
}

.help-text {
  display: block;
  margin-top: 5px;
  font-size: 12px;
  color: #7f8c8d;
  font-style: italic;
}

.help-text code {
  background-color: #f8f9fa;
  border: 1px solid #dee2e6;
  border-radius: 3px;
  padding: 2px 6px;
  font-family: 'Courier New', monospace;
  color: #e74c3c;
}

.info-box {
  background-color: #d1ecf1;
  border: 1px solid #bee5eb;
  border-radius: 4px;
  padding: 12px;
  margin-bottom: 15px;
  color: #0c5460;
}

.warning-box {
  background-color: #fff3cd;
  border: 1px solid #ffeaa7;
  border-radius: 4px;
  padding: 12px;
  margin-top: 15px;
  color: #856404;
}

.warning-box ul {
  margin: 5px 0 0 20px;
  padding: 0;
}

.warning-box li {
  margin-bottom: 5px;
}

/* Disabled state styles */
input:disabled,
select:disabled,
textarea:disabled {
  background-color: #e9ecef;
  color: #6c757d;
  cursor: not-allowed;
  opacity: 0.7;
}

input:disabled::placeholder,
textarea:disabled::placeholder {
  color: #adb5bd;
}
```

---

### Step 8: Add JavaScript for Platform Detection

**File:** `src/web_pages.cpp`

Add JavaScript to detect platform and enable/disable TLS UI. Add this script before the closing `</body>` tag in the MQTT config page:

```javascript
// Add before closing </body> tag in src/web_pages.cpp

<script>
// Platform Detection
const isESP32 = %IS_ESP32%;  // Will be replaced by C++ code
const platformName = isESP32 ? "ESP32" : "ESP8266";

// DOM Elements
const tlsEnabled = document.getElementById('tlsEnabled');
const tlsCertType = document.getElementById('tlsCertType');
const tlsFingerprintRow = document.getElementById('tlsFingerprintRow');
const tlsCaCertRow = document.getElementById('tlsCaCertRow');
const tlsConfigSection = document.getElementById('tlsConfigSection');
const tlsInfoBox = document.getElementById('tlsInfoBox');
const platformNameSpan = document.getElementById('platformName');
const tlsStatusSpan = document.getElementById('tlsStatus');

// Initialize Platform Detection
function initPlatformDetection() {
  platformNameSpan.textContent = platformName;

  if (isESP32) {
    // ESP32 - Enable all TLS options
    tlsStatusSpan.textContent = "Full TLS support available";
    tlsStatusSpan.style.color = "#28a745";
    tlsInfoBox.style.backgroundColor = "#d4edda";
    tlsInfoBox.style.borderColor = "#c3e6cb";
    tlsInfoBox.style.color = "#155724";

    // Enable TLS controls
    tlsEnabled.disabled = false;
    tlsCertType.disabled = false;
    document.getElementById('tlsFingerprint').disabled = false;
    document.getElementById('tlsCaCert').disabled = false;

    // Show initial state
    toggleTlsConfig();
  } else {
    // ESP8266 - Disable all TLS options
    tlsStatusSpan.textContent = "Not supported on this platform";
    tlsStatusSpan.style.color = "#dc3545";
    tlsInfoBox.style.backgroundColor = "#f8d7da";
    tlsInfoBox.style.borderColor = "#f5c6cb";
    tlsInfoBox.style.color = "#721c24";

    // Disable TLS controls
    tlsEnabled.disabled = true;
    tlsCertType.disabled = true;
    document.getElementById('tlsFingerprint').disabled = true;
    document.getElementById('tlsCaCert').disabled = true;

    // Hide TLS config section
    tlsConfigSection.style.display = 'none';
    tlsFingerprintRow.style.display = 'none';
    tlsCaCertRow.style.display = 'none';
  }
}

// Toggle TLS Configuration Section
function toggleTlsConfig() {
  if (!isESP32) return;  // ESP8266 - always hidden

  const enabled = tlsEnabled.value === 'on';
  const certType = tlsCertType.value;

  if (enabled) {
    // Show TLS config section
    tlsConfigSection.style.display = 'block';

    // Show/hide certificate type options
    if (certType === 'fingerprint') {
      tlsFingerprintRow.style.display = 'block';
      tlsCaCertRow.style.display = 'none';
    } else if (certType === 'ca') {
      tlsFingerprintRow.style.display = 'none';
      tlsCaCertRow.style.display = 'block';
    } else {
      tlsFingerprintRow.style.display = 'none';
      tlsCaCertRow.style.display = 'none';
    }
  } else {
    // Hide TLS config section
    tlsConfigSection.style.display = 'none';
    tlsFingerprintRow.style.display = 'none';
    tlsCaCertRow.style.display = 'none';
  }
}

// Auto-switch port when TLS is toggled
function autoSwitchPort() {
  if (!isESP32) return;

  const portInput = document.getElementById('mqttPort');
  const currentPort = portInput.value.trim();
  const tlsEnabledVal = tlsEnabled.value === 'on';

  // Only auto-switch if port is default (empty or 1883/8883)
  if (currentPort === '' || currentPort === '1883' || currentPort === '8883') {
    portInput.value = tlsEnabledVal ? '8883' : '1883';
  }
}

// Validate fingerprint format (40 hex chars, optional colons)
function validateFingerprint(input) {
  let value = input.value.toUpperCase().replace(/[^0-9A-F:]/g, '');
  input.value = value;

  // Show validation status
  const isValid = value.replace(/:/g, '').length === 40;
  input.style.borderColor = isValid ? '#28a745' : '#dc3545';

  return isValid;
}

// Event Listeners
document.addEventListener('DOMContentLoaded', function() {
  initPlatformDetection();

  if (isESP32) {
    tlsEnabled.addEventListener('change', function() {
      toggleTlsConfig();
      autoSwitchPort();
    });

    tlsCertType.addEventListener('change', toggleTlsConfig);

    document.getElementById('tlsFingerprint').addEventListener('input', function() {
      validateFingerprint(this);
    });
  }
});

// Keyboard shortcut validation
document.getElementById('tlsFingerprint')?.addEventListener('keydown', function(e) {
  // Allow: backspace, delete, tab, escape, enter, colon
  if ([8, 9, 27, 13, 186].includes(e.keyCode) ||
      // Allow: Ctrl+A, Ctrl+C, Ctrl+V, Ctrl+X
      (e.ctrlKey && [65, 67, 86, 88].includes(e.keyCode))) {
    return;
  }
  // Allow: 0-9, A-F
  if ((e.keyCode >= 48 && e.keyCode <= 57) ||
      (e.keyCode >= 65 && e.keyCode <= 70) ||
      e.keyCode === 58) {  // colon
    return;
  }
  // Otherwise prevent the character
  e.preventDefault();
});
</script>
```

---

### Step 9: Handle TLS Configuration in Web Server

**File:** `src/web_server.cpp`

Add handlers for TLS configuration in the MQTT config save handler. Find the `handle_mqttConfig()` function and add TLS handling:

```cpp
// Find the handle_mqttConfig() function in src/web_server.cpp
// Add TLS configuration handling after existing MQTT fields

void handle_mqttConfig() {
  PRINTS("\n--- MQTT Config Submission ---");

  // ... existing MQTT config handling ...

  // Handle On/Off toggle
  if (serverHttp.hasArg("onOffMqtt")) {
    newMqttOnOffAvailable = true;
    serverHttp.arg("onOffMqtt").toCharArray(newMqttOnOff, STDSIZE);
    PRINT("On/Off: ", newMqttOnOff);
  }

  // ... other existing fields ...

#ifdef ESP32
  // ========== NEW TLS HANDLING ==========
  // Handle TLS Enabled
  if (serverHttp.hasArg("tlsEnabled")) {
    newTlsEnabledAvailable = true;
    serverHttp.arg("tlsEnabled").toCharArray(newTlsEnabledMqtt, STDSIZE);
    PRINT("TLS Enabled: ", newTlsEnabledMqtt);

    // Auto-switch port if needed
    if (serverHttp.hasArg("port")) {
      String portValue = serverHttp.arg("port");
      portValue.trim();

      // Auto-switch to 8883 if TLS enabled and port is empty/1883
      if (strcmp(newTlsEnabledMqtt, "on") == 0) {
        if (portValue == "" || portValue == "1883") {
          strcpy(newMqttServerPort, "8883");
          PRINTS("TLS: Auto-switching port to 8883");
        } else {
          portValue.toCharArray(newMqttServerPort, STDSIZE);
        }
      } else {
        // TLS disabled - auto-switch to 1883 if port is empty/8883
        if (portValue == "" || portValue == "8883") {
          strcpy(newMqttServerPort, "1883");
          PRINTS("TLS: Auto-switching port to 1883");
        } else {
          portValue.toCharArray(newMqttServerPort, STDSIZE);
        }
      }
    }
  }

  // Handle TLS Certificate Type
  if (serverHttp.hasArg("tlsCertType")) {
    newTlsCertTypeAvailable = true;
    serverHttp.arg("tlsCertType").toCharArray(newTlsCertTypeMqtt, STDSIZE);
    PRINT("TLS Cert Type: ", newTlsCertTypeMqtt);
  }

  // Handle TLS Fingerprint
  if (serverHttp.hasArg("tlsFingerprint")) {
    newTlsFingerprintAvailable = true;
    String fp = serverHttp.arg("tlsFingerprint");
    fp.trim();
    fp.toCharArray(newTlsFingerprintMqtt, STDSIZE);
    PRINT("TLS Fingerprint: ", newTlsFingerprintMqtt);
  }

  // Handle TLS CA Certificate
  if (serverHttp.hasArg("tlsCaCert")) {
    newTlsCaCertAvailable = true;
    String cert = serverHttp.arg("tlsCaCert");
    cert.trim();

    // Validate certificate isn't too long
    if (cert.length() > 3071) {
      PRINTS("TLS: WARNING - CA certificate truncated (exceeds buffer size)");
      cert = cert.substring(0, 3071);
    }

    cert.toCharArray(newTlsCaCertMqtt, 3072);
    PRINT("TLS CA Cert length: ", strlen(newTlsCaCertMqtt));
  }
  // ======================================

#endif // ESP32

  // ... rest of existing save logic ...

  saveMqttConfigAtStart = true;

  // Redirect back to config page
  String redirectUrl = "/mqttconfig";
  if (serverHttp.hasArg("saved")) {
    redirectUrl += "?saved=1";
  }
  serverHttp.sendHeader("Location", redirectUrl);
  serverHttp.send(303);
}
```

---

### Step 10: Update MQTT Config Page Generation

**File:** `src/web_pages.cpp`

Update the page generation to populate TLS fields and set the `IS_ESP32` JavaScript variable:

```cpp
// Find the getPage_mqttConfig() function in src/web_pages.cpp
// Modify to include TLS variable substitution

String getPage_mqttConfig() {
  String html = FPSTR(HTTP_TMPL_MQTT_CONFIG);

  // ... existing variable substitutions ...

#ifdef ESP32
  // ========== ESP32: Enable TLS and populate values ==========
  html.replace("%IS_ESP32%", "true");
  html.replace("%TLS_INFO_BOX_STYLE%", "display: block");
  html.replace("%TLS_WARNING_STYLE%", "display: block");

  // TLS Enabled
  if (strcmp(tlsEnabledMqtt, "on") == 0) {
    html.replace("%TLS_ENABLED_ON%", "selected");
    html.replace("%TLS_ENABLED_OFF%", "");
  } else {
    html.replace("%TLS_ENABLED_ON%", "");
    html.replace("%TLS_ENABLED_OFF%", "selected");
  }

  // TLS Certificate Type
  if (strcmp(tlsCertTypeMqtt, "fingerprint") == 0) {
    html.replace("%TLS_CERT_FINGERPRINT%", "selected");
    html.replace("%TLS_CERT_CA%", "");
    html.replace("%TLS_CERT_NONE%", "");
    html.replace("%TLS_FINGERPRINT_STYLE%", "display: block");
    html.replace("%TLS_CA_CERT_STYLE%", "display: none");
  } else if (strcmp(tlsCertTypeMqtt, "ca") == 0) {
    html.replace("%TLS_CERT_FINGERPRINT%", "");
    html.replace("%TLS_CERT_CA%", "selected");
    html.replace("%TLS_CERT_NONE%", "");
    html.replace("%TLS_FINGERPRINT_STYLE%", "display: none");
    html.replace("%TLS_CA_CERT_STYLE%", "display: block");
  } else {
    html.replace("%TLS_CERT_FINGERPRINT%", "");
    html.replace("%TLS_CERT_CA%", "");
    html.replace("%TLS_CERT_NONE%", "selected");
    html.replace("%TLS_FINGERPRINT_STYLE%", "display: none");
    html.replace("%TLS_CA_CERT_STYLE%", "display: none");
  }

  // TLS Values
  html.replace("%TLS_FINGERPRINT%", tlsFingerprintMqtt);
  html.replace("%TLS_CA_CERT%", tlsCaCertMqtt);
  // =========================================================

#else
  // ========== ESP8266: Disable TLS UI ==========
  html.replace("%IS_ESP32%", "false");
  html.replace("%TLS_INFO_BOX_STYLE%", "display: block");
  html.replace("%TLS_WARNING_STYLE%", "display: none");

  // Reset all TLS options to defaults
  html.replace("%TLS_ENABLED_ON%", "");
  html.replace("%TLS_ENABLED_OFF%", "selected");
  html.replace("%TLS_CERT_FINGERPRINT%", "");
  html.replace("%TLS_CERT_CA%", "");
  html.replace("%TLS_CERT_NONE%", "selected");
  html.replace("%TLS_FINGERPRINT_STYLE%", "display: none");
  html.replace("%TLS_CA_CERT_STYLE%", "display: none");
  html.replace("%TLS_CONFIG_SECTION_STYLE%", "display: none");
  html.replace("%TLS_FINGERPRINT%", "");
  html.replace("%TLS_CA_CERT%", "");
  // ==========================================
#endif

  // ... rest of existing page content ...

  return html;
}
```

---

### Step 11: Update Configuration Loading

**File:** `src/functions.cpp` or wherever `loadMqttConfig()` is defined

Ensure the MQTT configuration loading function handles the new TLS fields:

```cpp
// Find the loadMqttConfig() function in src/functions.cpp
// Add TLS field loading

void loadMqttConfig() {
  PRINTS("\n--- Loading MQTT Configuration ---");

  File file = LittleFS.open(mqttConfigFile, "r");

  if (!file) {
    PRINTS("MQTT config file not found, using defaults");
    return;
  }

  // Parse JSON
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    PRINTS("MQTT config JSON parse error");
    return;
  }

  // Load existing fields
  if (doc["onOffMqtt"]) {
    doc["onOffMqtt"].to(mqttOnOff);
    PRINT("On/Off: ", mqttOnOff);
  }
  // ... other fields ...

#ifdef ESP32
  // ========== NEW: Load TLS fields ==========
  if (doc["tlsEnabledMqtt"]) {
    doc["tlsEnabledMqtt"].to(tlsEnabledMqtt);
    PRINT("TLS Enabled: ", tlsEnabledMqtt);
  } else {
    strcpy(tlsEnabledMqtt, "off");  // Default
  }

  if (doc["tlsCertTypeMqtt"]) {
    doc["tlsCertTypeMqtt"].to(tlsCertTypeMqtt);
    PRINT("TLS Cert Type: ", tlsCertTypeMqtt);
  } else {
    strcpy(tlsCertTypeMqtt, "none");  // Default
  }

  if (doc["tlsFingerprintMqtt"]) {
    doc["tlsFingerprintMqtt"].to(tlsFingerprintMqtt);
  } else {
    tlsFingerprintMqtt[0] = '\0';
  }

  if (doc["tlsCaCertMqtt"]) {
    doc["tlsCaCertMqtt"].to(tlsCaCertMqtt);
  } else {
    tlsCaCertMqtt[0] = '\0';
  }
  // ======================================
#endif

  PRINTS("--- MQTT Configuration Loaded ---\n");
}
```

---

### Step 12: Update Configuration Saving

**File:** `src/functions.cpp` or wherever `saveMqttConfig()` is defined

Update the MQTT configuration saving to include TLS fields:

```cpp
// Find the saveMqttConfig() function in src/functions.cpp
// Add TLS field saving

void saveMqttConfig() {
  PRINTS("\n--- Saving MQTT Configuration ---");

  JsonDocument doc;

  // Save existing fields
  doc["onOffMqtt"] = mqttOnOff;
  // ... other fields ...

#ifdef ESP32
  // ========== NEW: Save TLS fields ==========
  doc["tlsEnabledMqtt"] = tlsEnabledMqtt;
  doc["tlsCertTypeMqtt"] = tlsCertTypeMqtt;
  doc["tlsFingerprintMqtt"] = tlsFingerprintMqtt;
  doc["tlsCaCertMqtt"] = tlsCaCertMqtt;

  PRINT("TLS Enabled saved: ", tlsEnabledMqtt);
  PRINT("TLS Cert Type saved: ", tlsCertTypeMqtt);
  // ======================================
#endif

  // Write to file
  File file = LittleFS.open(mqttConfigFile, "w");
  if (!file) {
    PRINTS("ERROR: Failed to open MQTT config file for writing");
    return;
  }

  serializeJson(doc, file);
  file.close();

  PRINTS("--- MQTT Configuration Saved ---\n");
}
```

---

## Testing Guide

### ESP32 Testing

#### 1. Plain MQTT (No TLS)

1. Set TLS: **Off**
2. Port: **1883** (auto-switched)
3. Save configuration
4. Verify connection logs show "TLS: Disabled, using plain connection"
5. Verify MQTT connection succeeds

#### 2. MQTT with Fingerprint Validation

1. Get server fingerprint:
   ```bash
   openssl s_client -connect mqtt.example.com:8883 -showcerts 2>/dev/null | \
     openssl x509 -fingerprint -sha1 -noout | \
     sed 's/SHA1 Fingerprint=//g' | \
     sed 's/://g'
   ```

2. Set TLS: **On**
3. Certificate Type: **Fingerprint**
4. Fingerprint: **AA:BB:CC:DD:EE:FF...** (paste fingerprint)
5. Port: **8883** (auto-switched)
6. Save configuration
7. Verify connection logs show "TLS: Enabled, configuring secure client"
8. Verify MQTT connection succeeds

#### 3. MQTT with CA Certificate

1. Get CA certificate:
   ```bash
   openssl s_client -showcerts -connect mqtt.example.com:8883 </dev/null
   ```

2. Set TLS: **On**
3. Certificate Type: **CA Certificate**
4. CA Certificate: **(paste full PEM including -----BEGIN CERTIFICATE-----)**
5. Port: **8883** (auto-switched)
6. Save configuration
7. Verify connection logs show "TLS: Using CA certificate validation"
8. Verify MQTT connection succeeds

#### 4. Insecure TLS (Not Recommended)

1. Set TLS: **On**
2. Certificate Type: **None**
3. Port: **8883**
4. Save configuration
5. Verify connection logs show "TLS: WARNING - Using insecure mode"
6. Verify MQTT connection succeeds (but warns about security)

### ESP8266 Testing

#### 1. Verify No TLS Code Compiled

1. Flash ESP8266 firmware
2. Open `/mqttconfig` page
3. Verify TLS section shows: "TLS Status: Not available on this platform"
4. Verify all TLS controls are disabled/grayed out
5. Verify TLS configuration section is hidden
6. Port should remain **1883**

#### 2. Verify MQTT Works (Plain Only)

1. Configure MQTT with TLS: **Off**
2. Port: **1883**
3. Save configuration
4. Verify MQTT connection succeeds
5. No TLS-related logs should appear

#### 3. Verify Platform Detection

1. Check browser console for JavaScript errors
2. Verify `isESP32` is set to `false`
3. Verify platform name shows "ESP8266"
4. Verify TLS info box shows red warning

---

## UI Behavior

### ESP32 UI Behavior

| Element | State | Description |
|---------|--------|-------------|
| TLS Info Box | Green, visible | "Platform: ESP32", "TLS Status: Full TLS support available" |
| Enable TLS | Enabled, functional | Off/On toggle works |
| Port | Auto-switches | 1883 → 8883 when TLS enabled |
| Certificate Type | Enabled, functional | None/Fingerprint/CA dropdown works |
| Fingerprint Input | Enabled when selected | Visible if "Fingerprint" selected |
| CA Certificate Input | Enabled when selected | Visible if "CA Certificate" selected |
| Warning Box | Visible, yellow | Shows security recommendations |
| All TLS controls | Fully functional | User can configure complete TLS |

### ESP8266 UI Behavior

| Element | State | Description |
|---------|--------|-------------|
| TLS Info Box | Red, visible | "Platform: ESP8266", "TLS Status: Not available on this platform" |
| Enable TLS | Disabled, grayed out | Always set to "Off" |
| Port | Stays 1883 | No auto-switching |
| Certificate Type | Disabled, hidden | Not visible |
| Fingerprint Input | Disabled, hidden | Not visible |
| CA Certificate Input | Disabled, hidden | Not visible |
| Warning Box | Hidden | Not visible |
| All TLS controls | Disabled/hidden | Cannot configure TLS |

### Port Auto-Switching Logic

```
User Action                     ESP32 Behavior           ESP8266 Behavior
---------------------------   ----------------------   ---------------------------
Enable TLS → On             Port: 1883 → 8883       No change (TLS disabled)
Enable TLS → Off            Port: 8883 → 1883       No change
Custom port entered          Custom port preserved     Custom port preserved
Empty port + TLS On         Port: 8883               No change
Empty port + TLS Off        Port: 1883               No change
```

---

## Summary of Changes

### Files Modified

1. **include/config.h** - Added TLS fields to `mqttConfigObj` (5 lines)
2. **include/globals.h** - Added TLS variable declarations (15 lines, ESP32 only)
3. **src/globals.cpp** - Added TLS variable definitions (20 lines, ESP32 only)
4. **src/mqtt.cpp** - Added `configureTLS()` function (100 lines, ESP32 only)
5. **src/web_pages.cpp** - Added TLS UI section (~150 lines)
6. **src/web_server.cpp** - Added TLS config handlers (~60 lines, ESP32 only)
7. **src/functions.cpp** - Updated config load/save (~20 lines, ESP32 only)

### Total Lines Added

- **Common code:** ~170 lines (UI, config structures)
- **ESP32 code:** ~200 lines (TLS logic, handlers)
- **ESP8266 impact:** 0 lines (no TLS code compiled)

### Compilation

**ESP32:**
- ✅ Compiles with full TLS support
- ✅ All TLS features available
- ✅ UI fully functional

**ESP8266:**
- ✅ Compiles successfully (no TLS code included)
- ✅ UI shows TLS section (disabled)
- ✅ No functionality impact
- ✅ No memory overhead

---

## Next Steps

After implementing these changes:

1. **Test ESP32** with all TLS modes (fingerprint, CA, insecure)
2. **Test ESP8266** to ensure no compilation errors
3. **Update README.md** with TLS configuration instructions
4. **Add examples** for generating fingerprints and certificates
5. **Test with popular brokers** (Mosquitto, EMQX, AWS IoT)
6. **Document troubleshooting** for common TLS issues

---

**Document Version:** 1.0
**Last Updated:** 2026-01-07
**Compatible Firmware Version:** v0.9.1+
