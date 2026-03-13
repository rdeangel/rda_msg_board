#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "config.h"

// System Configuration
void loadConfiguration(const char *webConfigFile, webConfigObj &webConfig);
void saveConfiguration(const char *webConfigFile, const webConfigObj &webConfig);
void printWebFile(const char *webConfigFile);
void initWebStoreConfig();
void changeWebLoginCredentials();

// Defaults Configuration
void loadDefaultsConfiguration(const char *configFile, defaultsConfigObj &config);
void saveDefaultsConfiguration(const char *configFile, const defaultsConfigObj &config);
void initDefaultsStoreConfig();

// General Configuration
void loadGeneralConfiguration(const char *configFile, generalConfigObj &config);
void saveGeneralConfiguration(const char *configFile, const generalConfigObj &config);
void initGeneralStoreConfig();

// Clock Configuration
void loadClockConfiguration(const char *configFile, clockConfigObj &config);
void saveClockConfiguration(const char *configFile, const clockConfigObj &config);
void initClockStoreConfig();

#ifndef DISABLE_TIMER_FEATURE
// Timer Configuration
void loadTimerConfiguration(const char *configFile, timerConfigObj &config);
void saveTimerConfiguration(const char *configFile, const timerConfigObj &config);
void initTimerStoreConfig();
#endif

#ifndef DISABLE_SLEEP_MODE_FEATURE
// Sleep Mode Configuration
void loadSleepModeConfiguration(const char *configFile, sleepModeConfigObj &config);
void saveSleepModeConfiguration(const char *configFile, const sleepModeConfigObj &config);
void initSleepModeStoreConfig();
#endif

#ifndef DISABLE_ALARM_FEATURE
// Alarm Configuration
void loadAlarmConfiguration(const char *configFile, alarmConfigObj &config);
void saveAlarmConfiguration(const char *configFile, const alarmConfigObj &config);
void initAlarmStoreConfig();

// Recurrent Alarm Configuration
void loadRecurrentAlarmConfiguration(const char *configFile, recurrentAlarmConfigObj &config);
void saveRecurrentAlarmConfiguration(const char *configFile, const recurrentAlarmConfigObj &config);
void initRecurrentAlarmStoreConfig();
#endif

#ifndef DISABLE_WEATHER_FEATURE
// Weather Configuration
void loadWeatherConfiguration(const char *configFile, weatherConfigObj &config);
void saveWeatherConfiguration(const char *configFile, const weatherConfigObj &config);
void initWeatherStoreConfig();
#endif

#ifndef DISABLE_CRYPTO_FEATURE
// Crypto Price Ticker Configuration
void loadCryptoConfiguration(const char *configFile, cryptoConfigObj &config);
void saveCryptoConfiguration(const char *configFile, const cryptoConfigObj &config);
void initCryptoStoreConfig();
#endif

#endif // CONFIG_MANAGER_H
