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
  static const size_t BUFSIZE = 1024;
  static const char compile_date[] = __DATE__ " " __TIME__;
  static const char *response_template =
        "# HELP esp_module_reboot Device reboot count.\n"
        "# TYPE esp_module_reboot gauge\n"
        "# UNIT esp_module_reboot \n"
        "esp_module_reboot %d\n"
        "# HELP esp_module_compile Device compile date and time.\n"
        "# TYPE esp_module_compile gauge\n"
        "# UNIT esp_module_compile \n"
        "esp_module_compile %s\n";
  int count_reboot = getROMData(0);
  char response[BUFSIZE];
  snprintf(response, BUFSIZE, response_template, count_reboot, compile_date);
  server.send(200, "text/plain; charset=utf-8", response);
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
