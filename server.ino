#include "server.h"

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
  int countReboot = getROMData(0);
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

void handleRequests() {
  server.handleClient();
}
