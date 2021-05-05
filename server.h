#pragma once
#ifndef SERVER_H
#define SERVER_H

#include "definitions.h"
#include <ESP8266WebServer.h>
#include "rom.h"

// Web-pages
#include "index.h"

ESP8266WebServer server(80);
bool currentStage = false;
#ifdef BUTTON_PIN
bool isYetChangeStage = false;
#endif

void serverRoot();
void relayAction(bool switchModule);
void relayPage();
void metricsPage();
void restartPage();
void fileNotFound();
void setupServer();
void handleRequests();

#endif // SERVER_H
