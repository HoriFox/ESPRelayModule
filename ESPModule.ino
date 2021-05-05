// Constants
#include "definitions.h"

#include <ESP8266WiFi.h>
// May be useful for discovery service
//#include <WiFiUdp.h>
//#include <WiFiClient.h>

// Over-the-air programming
#ifdef ENABLE_OTA
#include <ArduinoOTA.h>
#endif

#include "rom.h"
#include "server.h"

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

  initROM();
  setupServer();
  Serial.println("HTTP server started");
}

#ifdef BUTTON_PIN
void handleButtonPress() {
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && isYetChangeStage) {
    relayAction(!currentStage);
    isYetChangeStage = false;
  }
  if (buttonState == HIGH) {
    isYetChangeStage = true;
  }
}
#endif

void loop(void) {
#ifdef ENABLE_OTA
  // Over-the-air programming handling, always listen
  ArduinoOTA.handle();
#endif

  // Listen & serve HTTP requests
  handleRequests();

#ifdef BUTTON_PIN
  handleButtonPress();
#endif
}
