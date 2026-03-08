#include "web_data.h"
#include "globals.h"
#include "chirp_library.h"

void setMainPageVars() {
  mainPageVars = String("<data><clientid>") + clientId +
                 String("</clientid><repeat>") + String(repeatDefault) +
                 String("</repeat><buzzer>") + String(buzzerDefault) +
                 String("</buzzer><delay>") + String(scrollDelayDefault) +
                 String("</delay><brightness>") + String(ledBrightnessDefault) +
                 String("</brightness><alertchirp>") + String(alertChirpDefault) +
                 String("</alertchirp><version>") + String(version) +
                 String("</version><maxdevices>") + String(MAX_DEVICES) +
                 String("</maxdevices>");

  // Add chirp list for dropdown population
  mainPageVars += String("<chirplist>");
  int chirpCount = getChirpCount();
  for (int i = 0; i < chirpCount; i++) {
    const ChirpPattern* pattern = getChirpByIndex(i);
    if (pattern) {
      mainPageVars += String("<chirp>") + pattern->name + String("</chirp>");
    }
  }
  mainPageVars += String("</chirplist></data>");
}

void setMqttPageVars() {
  mqttPageVars =
      String("<data><clientid>") + clientId + String("</clientid><mqttonoff>") +
      String(mqttOnOff) + String("</mqttonoff><mqttanonymous>") +
      String(mqttAnonymous) + String("</mqttanonymous><mqttalert>") +
      String(mqttAlert) + String("</mqttalert><mqttusername>") +
      String(mqttUsername) + String("</mqttusername><mqttserveraddress>") +
      String(mqttServerAddress) +
      String("</mqttserveraddress><mqttserverport>") + String(mqttServerPort) +
      String("</mqttserverport><mqtttopicprefix>") + String(mqttTopicPrefix) +
      String("</mqtttopicprefix><mqtthadiscovery>") + String(mqttHaDiscovery) +
      String("</mqtthadiscovery><mqttmessagesenable>") + String(mqttMessagesEnable) +
#ifdef ESP32
      String("</mqttmessagesenable><tlsenabled>") + String(tlsEnabledMqtt) +
      String("</tlsenabled><certexists>") + (LittleFS.exists("/ca_cert.pem") ? "true" : "false") +
      String("</certexists><platform>ESP32</platform>") +
#else
      String("</mqttmessagesenable><platform>ESP8266</platform>") +
#endif
      String("<version>") + String(version) +
      String("</version><maxdevices>") + String(MAX_DEVICES) +
      String("</maxdevices></data>");
}

void setChangeCredVars() {
  changeCredVars = String("<data><clientid>") + clientId +
                   String("</clientid><username>") + String(web_username) +
                   String("</username><version>") + String(version) +
                   String("</version><maxdevices>") + String(MAX_DEVICES) +
                   String("</maxdevices>");

#ifndef DISABLE_ALARM_FEATURE
  changeCredVars += String("<maxalarms>") + String(MAX_ALARMS) + String("</maxalarms>");
#endif

#ifndef DISABLE_TIMER_FEATURE
  changeCredVars += String("<timerfeature>true</timerfeature>");
#else
  changeCredVars += String("<timerfeature>false</timerfeature>");
#endif

#ifndef DISABLE_SLEEP_MODE_FEATURE
  changeCredVars += String("<sleepmodefeature>true</sleepmodefeature>");
#else
  changeCredVars += String("<sleepmodefeature>false</sleepmodefeature>");
#endif

#ifndef DISABLE_WEATHER_FEATURE
  changeCredVars += String("<weatherfeature>true</weatherfeature>");
#else
  changeCredVars += String("<weatherfeature>false</weatherfeature>");
#endif

  changeCredVars += String("</data>");
}

void setUpdateVars() {
  String defaultHostname = clientIdPrefix + chipId;
  defaultHostname.toUpperCase();
  updateVars = String("<data><clientid>") + clientId +
               String("</clientid><defaulthostname>") + defaultHostname +
               String("</defaulthostname><defaultuser>") + String(DEFAULT_WEB_USER) +
               String("</defaultuser><defaultpass>") + String(DEFAULT_WEB_PASS) +
               String("</defaultpass><version>") + String(version) +
               String("</version><maxdevices>") + String(MAX_DEVICES) +
               String("</maxdevices></data>");
}

// Set clock page variables for web UI (XML response)
void setClockPageVars() {
  clockPageVars = "<clockvars>";
  clockPageVars += "<enabled>" + String(clockConfig.enabled) + "</enabled>";
  clockPageVars += "<ntpserver>" + String(clockConfig.ntpServer) + "</ntpserver>";
  clockPageVars += "<tzstring>" + String(clockConfig.tzString) + "</tzstring>";
  clockPageVars += "<brightness>" + String(clockConfig.brightness) + "</brightness>";
  clockPageVars += "<transitiondelay>" + String(clockConfig.transitionDelayMs) + "</transitiondelay>";
  clockPageVars += "<transitioneffect>" + String(clockConfig.transitionEffect) + "</transitioneffect>";
  clockPageVars += "<transitionspeed>" + String(clockConfig.transitionSpeed) + "</transitionspeed>";
  clockPageVars += "<randomizetransition>" + String(clockConfig.randomizeTransition) + "</randomizetransition>";
  clockPageVars += "<resyncinterval>" + String(clockConfig.resyncIntervalHours) + "</resyncinterval>";
  clockPageVars += "<dateformat>" + String(clockConfig.dateFormat) + "</dateformat>";
  clockPageVars += "<datealternateseconds>" + String(clockConfig.dateAlternateSeconds) + "</datealternateseconds>";
  clockPageVars += "<customdateformat>" + String(clockConfig.customDateFormat) + "</customdateformat>";
  clockPageVars += "<clockface>" + String(clockConfig.clockFace) + "</clockface>";
  clockPageVars += "<datealternate>" + String(clockConfig.dateAlternate) + "</datealternate>";
  clockPageVars += "<clockampm>" + String(clockConfig.clockAmPm) + "</clockampm>";
  clockPageVars += "<clientid>" + String(clientId) + "</clientid>";
  clockPageVars += "<version>" + String(VERSION) + "</version>";
  clockPageVars += "</clockvars>";
}

// Parse boolean from JSON (handles true/false, "on"/"off", 1/0)
bool parseBoolean(JsonVariant value) {
  if (value.is<bool>()) return value.as<bool>();
  if (value.is<String>()) {
    String str = value.as<String>();
    return (str == "on" || str == "true" || str == "1");
  }
  if (value.is<int>()) return value.as<int>() != 0;
  return false;
}

// Validate transition effect name
bool isValidEffect(const char* effect) {
  const char* validEffects[] = {
    "FADE", "DISSOLVE", "BLINDS", "WIPE", "GROW",
    "SCROLL", "SCAN", "OPENING", "CLOSING", "PRINT",
    "SCROLL_DOWN", "SCROLL_LEFT", "SCROLL_RIGHT",
    "SLICE", "MESH", "GROW_DOWN",
    "SCAN_VERTX", "SCROLL_UP_LEFT", "SCROLL_UP_RIGHT", "OPENING_CURSOR",
    "SCAN_VERT", "SCROLL_DOWN_LEFT", "WIPE_CURSOR", "SCAN_HORIZX",
    "CLOSING_CURSOR", "SCROLL_DOWN_RIGHT"
  };
  for (int i = 0; i < 26; i++) {
    if (strcmp(effect, validEffects[i]) == 0) return true;
  }
  return false;
}

void setGeneralVars() {
  String generalVars = F("<?xml version='1.0'?>\n<generalvars>");
  generalVars += F("<buzzerEnable>");
  generalVars += generalConfig.buzzerEnable;
  generalVars += F("</buzzerEnable>");
  generalVars += F("<brightnessOverrideEnable>");
  generalVars += generalConfig.brightnessOverrideEnable;
  generalVars += F("</brightnessOverrideEnable>");
  generalVars += F("<brightnessOverrideValue>");
  generalVars += generalConfig.brightnessOverrideValue;
  generalVars += F("</brightnessOverrideValue>");
#ifndef DISABLE_SLEEP_MODE_FEATURE
  generalVars += F("<sleepmodefeature>true</sleepmodefeature>");
#else
  generalVars += F("<sleepmodefeature>false</sleepmodefeature>");
#endif
  generalVars += F("</generalvars>");
  serverHttp.send(200, "text/xml", generalVars);
}

#ifndef DISABLE_TIMER_FEATURE
void setTimerVars() {
  String xml = F("<?xml version='1.0'?>\n<data>");
  xml += F("<enabled>");
  xml += timerConfig.enabled;
  xml += F("</enabled>");
  xml += F("<mode>");
  xml += timerConfig.mode;
  xml += F("</mode>");
  xml += F("<durationseconds>");
  xml += timerConfig.durationSeconds;
  xml += F("</durationseconds>");
  xml += F("<brightness>");
  xml += timerConfig.brightness;
  xml += F("</brightness>");
  xml += F("<alertbuzzer>");
  xml += timerConfig.alertBuzzer;
  xml += F("</alertbuzzer>");
  xml += F("<alertbuzzercount>");
  xml += timerConfig.alertBuzzerCount;
  xml += F("</alertbuzzercount>");
  xml += F("<alertchirp>");
  xml += timerConfig.alertChirp;
  xml += F("</alertchirp>");
  xml += F("<autorepeat>");
  xml += timerConfig.autoRepeat;
  xml += F("</autorepeat>");
  xml += F("<globalbuzzer>");
  xml += generalConfig.buzzerEnable;
  xml += F("</globalbuzzer>");

  // Add chirp list for dropdown population
  xml += F("<chirplist>");
  int chirpCount = getChirpCount();
  for (int i = 0; i < chirpCount; i++) {
    const ChirpPattern* pattern = getChirpByIndex(i);
    if (pattern) {
      xml += F("<chirp>");
      xml += pattern->name;
      xml += F("</chirp>");
    }
  }
  xml += F("</chirplist>");

  xml += F("</data>");
  serverHttp.send(200, "text/xml", xml);
}
#endif

#ifndef DISABLE_SLEEP_MODE_FEATURE
void setSleepModeVars() {
  String xml = F("<?xml version='1.0'?>\n<data>");
  xml += F("<enabled>");
  xml += sleepModeConfig.enabled;
  xml += F("</enabled>");
  xml += F("<ontime>");
  xml += sleepModeConfig.onTime;
  xml += F("</ontime>");
  xml += F("<offtime>");
  xml += sleepModeConfig.offTime;
  xml += F("</offtime>");
  xml += F("<muteonly>");
  xml += sleepModeConfig.muteOnly;
  xml += F("</muteonly>");
  xml += F("<weekendenabled>");
  xml += sleepModeConfig.weekendEnabled;
  xml += F("</weekendenabled>");
  xml += F("<weekendontime>");
  xml += sleepModeConfig.weekendOnTime;
  xml += F("</weekendontime>");
  xml += F("<weekendofftime>");
  xml += sleepModeConfig.weekendOffTime;
  xml += F("</weekendofftime>");
  xml += F("<active>");
  xml += sleepModeActive ? "true" : "false";
  xml += F("</active>");
  xml += F("<clockenabled>");
  xml += clockEnabled ? "true" : "false";
  xml += F("</clockenabled>");
  xml += F("</data>");
  serverHttp.send(200, "text/xml", xml);
}
#endif
