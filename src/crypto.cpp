#include "crypto.h"
#include "globals.h"
#include "functions.h"
#include "config_manager.h"
#include "utf8_utils.h"

#ifndef DISABLE_CRYPTO_FEATURE

// ESP32 only — feature is disabled on ESP8266 via -DDISABLE_CRYPTO_FEATURE
#ifdef ESP32
  #include <HTTPClient.h>
  #include <WiFiClientSecure.h>
#endif

// --- Price Formatting ---

// Get currency prefix symbol for display
static const char* getCurrencyPrefix(const char* currency) {
  if (strcmp(currency, "USD") == 0) return "$";
  if (strcmp(currency, "EUR") == 0) return "EUR ";
  if (strcmp(currency, "GBP") == 0) return "GBP ";
  if (strcmp(currency, "JPY") == 0) return "JPY ";
  if (strcmp(currency, "BTC") == 0) return "BTC ";
  if (strcmp(currency, "ETH") == 0) return "ETH ";
  return "";
}

// Format a float price value into a compact string
static void formatPrice(char* buf, size_t bufSize, float price, const char* currency) {
  const char* prefix = getCurrencyPrefix(currency);
  if (price >= 100000.0f) {
    snprintf(buf, bufSize, "%s%.0f", prefix, price);
  } else if (price >= 1000.0f) {
    snprintf(buf, bufSize, "%s%.0f", prefix, price);
  } else if (price >= 100.0f) {
    snprintf(buf, bufSize, "%s%.1f", prefix, price);
  } else if (price >= 1.0f) {
    snprintf(buf, bufSize, "%s%.2f", prefix, price);
  } else {
    snprintf(buf, bufSize, "%s%.4f", prefix, price);
  }
}

// Uppercase and truncate coin ID for display label (max 8 chars)
static void makeCoinLabel(char* buf, size_t bufSize, const char* coinId) {
  size_t len = strlen(coinId);
  size_t maxLen = (bufSize - 1 < 8) ? bufSize - 1 : 8;
  size_t copyLen = (len < maxLen) ? len : maxLen;
  for (size_t i = 0; i < copyLen; i++) {
    buf[i] = toupper((unsigned char)coinId[i]);
  }
  buf[copyLen] = '\0';
}

// --- Fetch ---

// Fetch crypto price data from CoinPaprika API (free, no API key required)
// One HTTPS request per coin: GET /v1/tickers/{coin-id}?quotes={CURRENCY}
// forceRefresh bypasses the enabled check (for manual refresh from UI)
void fetchCryptoData(bool forceRefresh) {
  if (!forceRefresh && !cryptoEnabled) return;

  if (strlen(cryptoConfig.coins) == 0) {
    PRINTS("\nCrypto: No coins configured");
    cryptoDataValid = false;
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    PRINTS("\nCrypto: WiFi not connected");
    return;
  }

#ifdef ESP32
  PRINTS("\nFetching crypto data...");

  // Parse comma-separated coin IDs (max MAX_CRYPTO_COINS)
  char coinsCopy[CRYPTO_COINS_SIZE];
  strlcpy(coinsCopy, cryptoConfig.coins, sizeof(coinsCopy));

  char* coinIds[MAX_CRYPTO_COINS];
  int coinCount = 0;
  char* token = strtok(coinsCopy, ",");
  while (token && coinCount < MAX_CRYPTO_COINS) {
    while (*token == ' ') token++;  // Trim leading whitespace
    if (strlen(token) > 0) {
      coinIds[coinCount++] = token;
    }
    token = strtok(NULL, ",");
  }

  if (coinCount == 0) {
    PRINTS("\nCrypto: No valid coin IDs parsed");
    cryptoDataValid = false;
    lastCryptoFetch = millis();
    return;
  }

  // Build price buffer across all coin requests
  cryptoPriceBuffer[0] = '\0';
  bool anySuccess = false;

  // Unique User-Agent per device — prevents shared firmware from triggering User-Agent blocks
  char userAgent[48];
  snprintf(userAgent, sizeof(userAgent), "ESP32HTTPClient-%08X", (uint32_t)ESP.getEfuseMac());

  WiFiClientSecure client;
  client.setInsecure();  // Skip certificate verification (matches existing TLS pattern)

  for (int i = 0; i < coinCount; i++) {
    // URL: https://api.coinpaprika.com/v1/tickers/{coin-id}?quotes={CURRENCY}
    char url[256];
    snprintf(url, sizeof(url),
             "https://api.coinpaprika.com/v1/tickers/%s?quotes=%s",
             coinIds[i],
             cryptoConfig.currency);

    PRINT("\nCrypto URL: ", url);

    HTTPClient http;
    http.begin(client, url);
    http.setTimeout(8000);
    http.setUserAgent(userAgent);
    // If an API key is configured, send it — moves from IP-based to account-based rate limiting
    if (strlen(cryptoConfig.apiKey) > 0) {
      http.addHeader("Authorization", String("Api-Key ") + cryptoConfig.apiKey);
    }

    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();

      if (payload.length() > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
          // Use the ticker symbol from the response (e.g. "BTC", "ETH") as the label
          const char* symbol = doc["symbol"].as<const char*>();
          if (!symbol) symbol = coinIds[i];

          // Extract price: doc["quotes"][currency]["price"]
          JsonObject quotes = doc["quotes"];
          if (!quotes.isNull() && !quotes[cryptoConfig.currency].isNull()) {
            float price = quotes[cryptoConfig.currency]["price"].as<float>();

            char priceBuf[32];
            char coinLabel[16];
            formatPrice(priceBuf, sizeof(priceBuf), price, cryptoConfig.currency);
            makeCoinLabel(coinLabel, sizeof(coinLabel), symbol);

            if (anySuccess) {
              strlcat(cryptoPriceBuffer, " | ", CRYPTO_PRICE_BUF_SIZE);
            }
            strlcat(cryptoPriceBuffer, coinLabel, CRYPTO_PRICE_BUF_SIZE);
            strlcat(cryptoPriceBuffer, ": ", CRYPTO_PRICE_BUF_SIZE);
            strlcat(cryptoPriceBuffer, priceBuf, CRYPTO_PRICE_BUF_SIZE);
            anySuccess = true;

            PRINT("\nCrypto fetched: ", coinLabel);
            PRINT(" = ", priceBuf);
          } else {
            PRINT("\nCrypto: No quote found for coin: ", coinIds[i]);
          }
        } else {
          PRINT("\nCrypto JSON parse error for coin: ", coinIds[i]);
        }
      }
    } else {
      PRINT("\nCrypto HTTP error for coin: ", coinIds[i]);
      PRINT(" code: ", httpCode);
    }

    http.end();
  }

  cryptoDataValid = anySuccess;
  if (anySuccess) {
    PRINT("\nCrypto prices: ", cryptoPriceBuffer);
  } else {
    PRINTS("\nCrypto: No prices fetched successfully");
  }

  // CRITICAL: Update last fetch time even on failure to prevent rapid-fire retry loop
  lastCryptoFetch = millis();
#endif // ESP32
}

// --- Display ---

// Check if crypto prices should be displayed
bool shouldDisplayCrypto() {
  if (!cryptoEnabled) return false;
  if (!cryptoDataValid) return false;
  if (!clockEnabled) return false;   // Crypto only shows when clock is enabled
  if (!clockNtpSynced) return false; // Need time synced

  // Don't show during messages
  if (newMessageAvailable || curMessage[0] != '\0') return false;

#ifndef DISABLE_TIMER_FEATURE
  if (timerRunning) return false;
#endif

#ifndef DISABLE_ALARM_FEATURE
  if (alarmActive) return false;
#endif

#ifndef DISABLE_SLEEP_MODE_FEATURE
  if (isSleepModeActive() && !sleepModeMuteOnly) return false;
#endif

  return true;
}

// Display crypto prices on LED matrix
void displayCrypto(bool withAnimation) {
#ifndef DISABLE_SLEEP_MODE_FEATURE
  if (isSleepModeActive() && !sleepModeMuteOnly) {
    P.displayClear();
    return;
  }
#endif

  if (!cryptoDataValid) return;

  P.setFont(nullptr);
  P.setIntensity(getEffectiveBrightness(cryptoBrightness));

  // cryptoPriceBuffer is already the full scrolling string
  static char cryptoDisplayString[CRYPTO_PRICE_BUF_SIZE];
  strlcpy(cryptoDisplayString, cryptoPriceBuffer, sizeof(cryptoDisplayString));

  if (withAnimation) {
    P.displayClear();

    int speed = atoi(clockConfig.transitionSpeed);
    uint16_t animSpeed = (speed < 10) ? 10 : speed;

    P.displayText(cryptoDisplayString, PA_CENTER, animSpeed, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    P.displayReset();
  } else {
    P.displayAnimate();
  }
}

// --- Main Update Loop ---

// Main crypto update function - called from loop
void updateCrypto() {
  // Handle manual refresh request from web UI (done here to avoid stack issues)
  if (cryptoRefreshRequested) {
    cryptoRefreshRequested = false;
    Serial.println(F("Processing crypto refresh request..."));
    fetchCryptoData(true); // Force refresh bypasses enabled check
    return;
  }

  if (!cryptoEnabled) return;

  // Fetch crypto data periodically
  unsigned long updateInterval = (unsigned long)atoi(cryptoConfig.updateIntervalMinutes) * 60000UL;

  // Initial fetch or periodic update
  if (lastCryptoFetch == 0 || (millis() - lastCryptoFetch >= updateInterval)) {
    fetchCryptoData(false);
  }
}

// --- JSON Status/Config for Web UI ---

// Get crypto status as JSON for web UI
String getCryptoStatusJson() {
  JsonDocument doc;

  doc["enabled"] = cryptoEnabled;
  doc["dataValid"] = cryptoDataValid;
  doc["priceBuffer"] = cryptoPriceBuffer;
  doc["lastUpdateAgo"] = lastCryptoFetch > 0 ? (millis() - lastCryptoFetch) / 1000 : -1;
  doc["apiKey"] = cryptoConfig.apiKey;
  doc["coins"] = cryptoConfig.coins;
  doc["currency"] = cryptoConfig.currency;
  doc["updateInterval"] = cryptoConfig.updateIntervalMinutes;
  doc["displayInterval"] = cryptoConfig.displayIntervalMinutes;
  doc["brightness"] = cryptoConfig.brightness;
  doc["displayRepeat"] = cryptoConfig.displayRepeatCount;

  String output;
  serializeJson(doc, output);
  return output;
}

// Get crypto configuration as JSON
String getCryptoConfigJson() {
  JsonDocument doc;

  doc["enabled"] = cryptoConfig.enabled;
  doc["coins"] = cryptoConfig.coins;
  doc["currency"] = cryptoConfig.currency;
  doc["updateIntervalMinutes"] = cryptoConfig.updateIntervalMinutes;
  doc["displayIntervalMinutes"] = cryptoConfig.displayIntervalMinutes;
  doc["brightness"] = cryptoConfig.brightness;
  doc["displayRepeatCount"] = cryptoConfig.displayRepeatCount;

  String output;
  serializeJson(doc, output);
  return output;
}

#endif // DISABLE_CRYPTO_FEATURE
