// Constants
#include "definitions.h"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// Over-the-air programming
#ifdef ENABLE_OTA
#include <ArduinoOTA.h>
#endif

// Web-pages
#include "index.h"

// May be useful for discovery service
//#include <WiFiUdp.h>
//#include <WiFiClient.h>

ESP8266WebServer server(80);
bool currentStage = false;
#ifdef BUTTON_PIN
bool isYetChangeStage = false;
#endif

void serverRoot() {
  server.send(200, "text/html", MAIN_page);
}

void relayAction(bool switchModule) {
  if (switchModule == 0) {
    currentStage = false;
    digitalWrite(CONTROL_PIN, LOW);
    server.send(200, "text/plain", "switch off");
  } else {
    currentStage = true;
    digitalWrite(CONTROL_PIN, HIGH);
    server.send(200, "text/plain", "switch on");
  }
}

void relayPage() {
  String value = server.arg("value");
  int activeValue = value.toInt();
  relayAction(activeValue);
}

void metricsPage() {
  int countReboot = EEPROM.read(0);
  String message = "{\n";
  message += "  \"countReboot\": "; message += countReboot; message += "\n";
  message += "}";
  server.send(200, "application/json", message);
}

void restartPage() {
  server.send(200, "text/plain", "restart esp");
  ESP.restart();
}

void fileNotFound() {
  server.send(404, "text/plain", "File Not Found");
}

void setupServer() {
  server.on("/", serverRoot);
  server.on("/relay", relayPage);
  server.on("/restart", restartPage);
  server.on("/metrics", metricsPage);
  server.onNotFound(fileNotFound);
  server.begin();
}

void setup(void) {
  pinMode(CONTROL_PIN, OUTPUT);
#ifdef BUTTON_PIN
  pinMode(BUTTON_PIN, INPUT);
#endif

  Serial.begin(SERIAL_BAUD);

  // WIFI_STA - client mode
  // WIFI_AP - access point mode
  // WIFI_AP_STA - mixed mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

#ifdef ENABLE_STATIC_IP
  IPAddress ip(WIFI_STATIC_IP);
  IPAddress gateway(WIFI_STATIC_GW);
  IPAddress subnet(WIFI_STATIC_MASK);
  WiFi.config(ip, gateway, subnet);
#endif

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to: ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

#ifdef ENABLE_OTA
  // Over-the-air programming setup
  ArduinoOTA.setHostname(DEVICE_NAME);
  ArduinoOTA.setPassword(DEVICE_PASS);
  ArduinoOTA.begin();
#endif

  setupServer();
  Serial.println("HTTP server started");

  // Init EEPROM
  EEPROM.begin(512);
  int countReboot = EEPROM.read(0); // Get current reboot count
  EEPROM.write(0, countReboot + 1); // Update reboot count
  EEPROM.commit();                  // Save to ROM
}

void loop(void) {
#ifdef ENABLE_OTA
  // Over-the-air programming handling, always listen
  ArduinoOTA.handle();
#endif
  // Listen & serve HTTP requests
  server.handleClient();

#ifdef BUTTON_PIN
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && isYetChangeStage) {
    relayAction(!currentStage);
    isYetChangeStage = false;
  }
  if (buttonState == HIGH) {
    isYetChangeStage = true;
  }
#endif
}
