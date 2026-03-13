# Crypto Price Ticker

The Crypto Price Ticker is an **ESP32-only** feature that periodically interrupts the clock to scroll live cryptocurrency prices across the LED matrix. Prices are fetched from the [CoinPaprika API](https://api.coinpaprika.com/) — free, no API key required.

## Overview

- Fetches prices for up to 10 configurable coins
- Supports USD, EUR, GBP, JPY, BTC, and ETH as display currencies
- Decoupled fetch and display intervals — cache data infrequently, show it more often
- Configurable scroll repetitions per display cycle
- Configurable brightness independent of main display
- Home Assistant MQTT Discovery integration (enable/disable switch + price sensor)
- Disabled on ESP8266 by default via `-DDISABLE_CRYPTO_FEATURE` (heap constraints)

## Platform Support

| Platform | Support |
|---|---|
| ESP32 | ✅ Fully supported |
| ESP8266 | ❌ Disabled by default (`-DDISABLE_CRYPTO_FEATURE`) |

ESP8266 lacks the heap headroom for HTTPS connections alongside WiFiManager and other active features. The build flag is applied automatically to all ESP8266 environments in `platformio.ini`.

## Configuration

Access via the web interface: **Device Config → Crypto Settings**.

### Coin IDs

CoinPaprika uses the format `{ticker}-{name}`. Enter a comma-separated list of up to 10 IDs:

| Coin | CoinPaprika ID |
|---|---|
| Bitcoin | `btc-bitcoin` |
| Ethereum | `eth-ethereum` |
| Solana | `sol-solana` |
| BNB | `bnb-binance-coin` |
| XRP | `xrp-xrp` |
| Cardano | `ada-cardano` |
| Dogecoin | `doge-dogecoin` |
| Polkadot | `dot-polkadot` |
| Litecoin | `ltc-litecoin` |
| Avalanche | `avax-avalanche` |

Find any coin's ID at [coinpaprika.com/coins](https://coinpaprika.com/coins/).

### Display Currency

Select one of: **USD**, **EUR**, **GBP**, **JPY**, **BTC**, **ETH**. The selected currency is passed as the `quotes` parameter in each API request, so conversion is done server-side by CoinPaprika.

### Fetch Interval vs Display Interval

These two settings are independent:

| Setting | Purpose | Range |
|---|---|---|
| **Fetch Interval** | How often to call the CoinPaprika API | 15 min – 4h |
| **Display Interval** | How often to interrupt the clock to show prices | 1 min – 4h |

Example: fetch every 60 min, display every 5 min — prices show frequently using cached data, API calls remain minimal.

### API Key (Optional)

CoinPaprika supports unauthenticated access and an optional free registered API key:

| Mode | Limit | Rate Limiting |
|---|---|---|
| No key (unauthenticated) | **1,000 requests/day** per IP | IP-based |
| Free registered key | **20,000 requests/month** | Account-based |

Register a free key at [coinpaprika.com/api](https://coinpaprika.com/api/). Leave the field empty to use unauthenticated access.

> **User-Agent:** Each device sends a unique `ESP32HTTPClient-{ChipID}` User-Agent. This prevents the shared firmware from triggering a global User-Agent block at CoinPaprika — each physical device is identified distinctly.

### Rate Limit Reference

The ticker makes **one request per coin per fetch cycle**:

| Coins | Fetch Interval | Calls/day | Mode |
|---|---|---|---|
| 2 | 30 min | 96 | No key needed |
| 5 | 30 min | 240 | No key needed |
| 10 | 30 min | 480 | No key needed |
| 10 | 15 min | 960 | No key needed (just under 1,000/day) |
| 10 | 15 min sustained | >1,000 | **Key required** |

Default (2 coins, 30 min) uses 96 calls/day — well within either tier.

### Scroll Repetitions

The number of times the price string scrolls across the display before returning to the clock (1–5). The current scroll always completes — no mid-word cutoffs. If a new message arrives during a scroll, remaining repetitions are skipped after the current one finishes.

### Brightness

Independent brightness level (0–15) for the crypto display, separate from the main message brightness.

## Display Format

Prices are formatted as:

```
BTC: $95,432 | ETH: $3,210 | SOL: $185.32
```

- Coin label = ticker symbol from API response (e.g. `BTC`, `ETH`)
- Price precision scales automatically:
  - ≥ $1,000 → no decimal (e.g. `$95432`)
  - ≥ $100 → 1 decimal (e.g. `$234.5`)
  - ≥ $1 → 2 decimals (e.g. `$3.21`)
  - < $1 → 4 decimals (e.g. `$0.0012`)

## Build Flags

```ini
; Disable crypto feature (default on all ESP8266 environments):
-DDISABLE_CRYPTO_FEATURE

; Re-enable on ESP32 if previously disabled:
; (remove -DDISABLE_CRYPTO_FEATURE from build_flags)
```

## Home Assistant Integration

When MQTT and HA Discovery are enabled, the following entities are published automatically:

| Entity | Type | Description |
|---|---|---|
| `Crypto Enable` | Switch | Enable/disable the crypto ticker |
| `Crypto Prices` | Sensor | Current scrolling price string |
| `Crypto Currency` | Sensor (diagnostic) | Currently selected currency |

## Disable on ESP32

To disable on ESP32 (e.g. to save heap), add to the environment's `build_flags` in `platformio.ini`:

```ini
[env:esp32_4m]
build_flags =
    ${env.build_flags}
    -DDISABLE_CRYPTO_FEATURE
```

## Configuration File

Settings are stored in LittleFS as `/crypto_config.json`:

```json
{
  "enabled": "off",
  "coins": "btc-bitcoin,eth-ethereum",
  "currency": "USD",
  "updateIntervalMinutes": "30",
  "displayIntervalMinutes": "5",
  "displayRepeatCount": "2",
  "brightness": "5"
}
```

This file is included in the full config export/import (`/exportconfig`, `/importconfig`).

## Attribution & Disclaimer

**Data Source:** All cryptocurrency data is fetched from the [CoinPaprika API](https://api.coinpaprika.com/).

**Personal Use Only:** This is a hobbyist project created for educational and personal purposes. In accordance with [CoinPaprika's Terms of Use](https://coinpaprika.com/terms-of-use/):
- This software does not support commercial redistribution.
- Users are responsible for adhering to the free plan rate limits (20,000 requests/month).
- Any individual or entity selling hardware pre-loaded with this software is responsible for securing a commercial/Enterprise license from CoinPaprika.

**Liability:** The developer of this project is not responsible for any API bans, financial losses, or legal actions taken by third-party data providers against the user.
