#ifndef WEB_DATA_H
#define WEB_DATA_H

#include "config.h"

// XML Data Setters
void setMainPageVars();
void setMqttPageVars();
void setChangeCredVars();
void setUpdateVars();
void setClockPageVars();
void setGeneralVars();
#ifndef DISABLE_TIMER_FEATURE
void setTimerVars();
#endif
#ifndef DISABLE_SLEEP_MODE_FEATURE
void setSleepModeVars();
#endif

// Utility
bool parseBoolean(JsonVariant value);
bool isValidEffect(const char* effect);

#endif // WEB_DATA_H
