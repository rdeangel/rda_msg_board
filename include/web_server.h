#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "config.h"
#include "config_manager.h"
#include "web_data.h"

// Web Server Functions
void showWebpageHttp();
void showChangeCredentialsHttp();
void usernamePasswordHttp();
void showChangeMqttConfigHttp();
void onMqttConfigChangeHttp();
void onNotFoundUriHttp();

// Controller / Handlers (Remaining in web_server.cpp)
void handleSaveGeneral();

// Setup
void httpWebDirDef();
void handleHttpServer();

#endif // WEB_SERVER_H
